//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
 * DESCRIPTION: Link Hardware Abstraction Layer (Request)

	Purpose:	Link Hardware Abstraction Layer transmit packet i/o
				handling for asynchronous transmit packets
				Request Packet functions (Read, Write, Lock)
	
	Revisions:
		created 04/12/2002 lm

****************************************************************************/

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "lal.h"
#include "lhl.h"
#include "lhlCli.h"
#include "lhlDefs.h"
#include "lhlLLC.h"
#include "lhlMsgQueue.h"
#include "lhlPendingTx.h"
#include "lhlReq.h"
#include "lhlStatistics.h"
#include "lhlStatusQueue.h"
#include "lhlTLabel.h"
#include "nciBM.h"
#include "nciHandles.h"
#include "nciIRM.h"
#include "csr.h"
#include "bri.h"
#ifdef _FCP_LOCAL
#include "fcp.h"
#endif //_FCP_LOCAL

/****************************************************************************
 ***************************************************************************

	Blocking Read,Write,Lock

 ***************************************************************************
****************************************************************************/

/*****************************************************************************

	lhlWriteAsyncStreamData

	this function handles the actual write. using the callback function and a 
	blocking semaphore, it enqueues the request and waits for the result or a 
	timeout.  the function returns with the status of the write response, or 
	with the status of the write request if the was an error.

*****************************************************************************/
#ifdef _LOOSE_ISO
HRESULT lhlAsyncStreamSendData(uint8 channel, uint8 tag, uint8 sy, uint16 numBytes, QUADLET *data, uint16 queue)
{
	HRESULT		hResult = NO_ERROR;
	uint32		nodeAddr;
	OFFSET_1394 destOffset = {0, 0};

	// nodeAddr is the handle here
	nodeAddr = LHL_ASYNC_DUMMY_NODE_ADDR;

	// encode channel, tag and sy into destOffset for comforming with general handling
	hResult = lhlLLCSetAsyncStreamQuad(&destOffset.Low, channel, tag, sy);
	if (hResult != NO_ERROR) return hResult;

	return lhlReqNode(nodeAddr, destOffset, numBytes, data, queue, LHL_REQ_STREAM, PB_LOCKTYPE_NONE, FALSE, LHL_TX_REQ_TIMEOUT_MSECS);
}
#endif //_LOOSE_ISO

/*****************************************************************************

	lhlReqHandle functions

	These are the main functions used by the LAL services for request accesses.
	Here, the handle is mapped to the corresponding nodeAddress to determine if
	the request is to the local node.  

	lhlReqNode functions

	These function is mainly used by low layers during bus reset to perform
	bus management functions and to scan busInfoBlocks (read).
	In this case, handles are not yet remapped to nodeAddresses, so we must 
	use the nodeAddress directly.

*****************************************************************************/

HRESULT lhlReqHandle(LAL_NODE_HANDLE handle, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue, uint16 reqType, PB_LOCKTYPE lockType, BOOL bNonBlocking, uint32 msec)
{
	HRESULT			hResult = NO_ERROR;
	uint32			nodeAddr;

	hResult = nciGetAddrFromHandle(handle, &nodeAddr);
	if (hResult != NO_ERROR)
	{
#ifdef _STATISTICS
		lhlStatistics.handleInvalid++;
#endif //_STATISTICS
		return hResult;
	}

	return lhlReq1394New(handle, nodeAddr, destOffset, numBytes, data, queue, reqType, lockType, bNonBlocking, msec);
}

/*****************************************************************************

	lhlReqLocalNode

	handle local node requests

*****************************************************************************/
HRESULT lhlReqLocalNode(OFFSET_1394 destOffset,
						uint16 numBytes,
						QUADLET *data,
						uint16 reqType)
{
	HRESULT		hResult = NO_ERROR;
	uint16		i;

	hResult = lhlLLCHandleCSR(destOffset, numBytes, data, reqType);
	if (hResult != E_NOT_IMPLEMENTED) return hResult;	 // LLC has taken care of handling request
	hResult = NO_ERROR;

	switch (reqType)
	{
		case LHL_REQ_READ:
			// Config ROM access
			if ((destOffset.Low >= CSR_ROM_SPACE_START) && 
				(destOffset.Low + numBytes <= CSR_ROM_SPACE_START + csrGetConfigROMQuadletSize() * 4))
			{
				for (i = 0; i < numBytes / 4; i++)
				{
					data[i] = csrReadConfigROMQuadlet(destOffset.Low + (i * 4));
				}

				return hResult;
			}
#ifdef _IRMC_CAPS
			// Serial Bus Space access
			if ((destOffset.Low >= CSR_SB_SPACE_START) && 
				(destOffset.Low + numBytes <= CSR_SB_SPACE_END + 1))
			{
				if (nciIRMIsThisNodeIRM() == TRUE)
				{
					RCODE_1394	rCode = RSP_COMPLETE;

					switch (destOffset.Low)
					{
						case CSR_SB_BM_ID:
						case CSR_SB_BW_AVAIL:
						case CSR_SB_CH_AVAIL_HI:
						case CSR_SB_CH_AVAIL_LO:
							if (numBytes == 4) // only allow quadlet read
							{
								rCode = nciIRMRead(destOffset.Low, data);
							}
							else
							{
								hResult = E_BAD_INPUT_PARAMETERS;
							}
							break;
						default:
							rCode = RSP_ADDRESS_ERROR;
							break;
					}
					if (rCode != RSP_COMPLETE)
					{
						hResult = E_LHL_RSP_BASE + rCode;
					}
					if (hResult != NO_ERROR)
					{
						sysLogError(hResult, __LINE__, moduleName);
						return hResult;
					}
				}
				return hResult;
			}
#endif //_IRMC_CAPS
#ifdef _BMC_CAPS
			// Topology Map access
			if ((destOffset.Low >= CSR_TOPOLOGY_MAP_START) && 
				(destOffset.Low + numBytes <= CSR_TOPOLOGY_MAP_END + 1))
			{
				if (nciBMIsThisNodeBM() == TRUE)
				{
					uint32	index = (destOffset.Low - CSR_TOPOLOGY_MAP_START) / 4;
					BOOL	bBusMaster = TRUE;
					for (i = 0; i < numBytes / 4; i++)
					{
						hResult = nciBMGetTopologyMapIndex(bBusMaster, index + i, &(data[i]));
						if (hResult != NO_ERROR) return hResult;
					}
					return hResult;
				}
			}
			// Speed Map access
			if ((destOffset.Low >= CSR_SPEED_MAP_START) && 
				(destOffset.Low + numBytes <= CSR_SPEED_MAP_END + 1))
			{
				if (nciBMIsThisNodeBM() == TRUE)
				{
					uint32	index = (destOffset.Low - CSR_SPEED_MAP_START) / 4;
					BOOL	bBusMaster = TRUE;
					for (i = 0; i < numBytes / 4; i++)
					{
						hResult = nciBMGetSpeedMapIndex(bBusMaster, index + i, &(data[i]));
						if (hResult != NO_ERROR) return hResult;
					}
					return hResult;
				}
			}
			break;
#endif //_BMC_CAPS
		case LHL_REQ_WRITE:
			break;
		case LHL_REQ_LOCK:
#ifdef _IRMC_CAPS
			// Serial Bus Space access
			if ((destOffset.Low >= CSR_SB_SPACE_START) && 
				(destOffset.Low + numBytes <= CSR_SB_SPACE_END))
			{
				if (nciIRMIsThisNodeIRM() == TRUE)
				{
					RCODE_1394	rCode = RSP_COMPLETE;

					switch (destOffset.Low)
					{
						case CSR_SB_BM_ID:
						case CSR_SB_BW_AVAIL:
						case CSR_SB_CH_AVAIL_HI:
						case CSR_SB_CH_AVAIL_LO:
							if (numBytes == 8) // only allow quadlet lock
							{
								uint32	lockArg = data[0];
								uint32	lockData = data[1];
								uint32	lockReturnData;

								rCode = nciIRMLock(destOffset.Low, lockArg, lockData, &lockReturnData);

								// check for compare_swap success/fail 
								if (lockArg != lockReturnData) // req:arg_value(32bit) == rsp:old_value(32bit) 
								{
									hResult = E_LAL_LOCK_COMPARE_SWAP_FAIL;
								}
							}
							else
							{
								hResult = E_BAD_INPUT_PARAMETERS;
							}
							break;
						default:
							rCode = RSP_ADDRESS_ERROR;
							break;
					}
					if (rCode != RSP_COMPLETE)
					{
						hResult = E_LHL_RSP_BASE + rCode;
					}
					if (hResult != NO_ERROR)
					{
						sysLogError(hResult, __LINE__, moduleName);
						return hResult;
					}
				}
				return hResult;
			}
#endif //_IRMC_CAPS
			break;
#ifdef _LOOSE_ISO
		case LHL_REQ_STREAM:
			// where is this request useful to upper layers?
			break;
#endif //_LOOSE_ISO
	}

	hResult = E_LAL_ADDR_RNG_NOT_FOUND;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

/*****************************************************************************

	lhlReq1394HandleHeader

	lhlReq1394 helper function for handling header data

*****************************************************************************/

HRESULT	lhlReq1394HandleHeader(PB* packetBlock,
						uint32 nodeAddr,
						OFFSET_1394 destOffset,
						uint32 tLabel,
						uint16 numBytes,
						uint16 reqType,
						PB_LOCKTYPE lockType)
{
	HRESULT			hResult = NO_ERROR;
	PB_HEADER		*pHeader = 0;
	PB_PACKETTYPE	packetType = PB_TYPE_UNDEF;
	uint32			thisnodeAddr = 0;

	// set up the packet header
	hResult = pbGetPacketHeader (packetBlock, &pHeader);
	if (hResult != NO_ERROR) return hResult;

	if (numBytes <= 4) // if the request is 4 bytes, a quadlet request is used
	{
		switch (reqType)
		{
			case LHL_REQ_READ:
				packetType = PB_TYPE_READ_REQUEST_QUADLET;
				break;
			case LHL_REQ_WRITE:
				packetType = PB_TYPE_WRITE_REQUEST_QUADLET;
				break;
			case LHL_REQ_LOCK:
				hResult = E_LHL_ASYNC_LOCK_FAILURE;
				sysLogError(hResult, __LINE__, moduleName);
				if (hResult != NO_ERROR) return hResult;
				break;
#ifdef _LOOSE_ISO
			case LHL_REQ_STREAM:
				packetType = PB_TYPE_ASYNC_STREAM;
				break;
#endif //_LOOSE_ISO
		}
	}
	else if (numBytes > 4) // otherwise a block request is used
	{
		switch (reqType)
		{
			case LHL_REQ_READ:
				packetType = PB_TYPE_READ_REQUEST;
				break;
			case LHL_REQ_WRITE:
				packetType = PB_TYPE_WRITE_REQUEST;
				break;
			case LHL_REQ_LOCK:
				packetType = PB_TYPE_LOCK_REQUEST;
				break;
#ifdef _LOOSE_ISO
			case LHL_REQ_STREAM:
				packetType = PB_TYPE_ASYNC_STREAM;
				break;
#endif //_LOOSE_ISO
		}
	}

	hResult = briGetThisNodeAddr(&thisnodeAddr);
	if (hResult != NO_ERROR) return hResult;

	// set up transaction type
	hResult = pbSetPacketType (packetBlock, packetType);
	if (hResult != NO_ERROR) return hResult;

	// set up numBytes
	hResult = lhlLLCSetPayloadSize((uint16) packetType, PB_NUM_HEADER_QUADLETS, pHeader->quadlets, numBytes);
	if (hResult != NO_ERROR) return hResult;

	switch (reqType)
	{
		case LHL_REQ_READ:
		case LHL_REQ_WRITE:
		case LHL_REQ_LOCK:
			pHeader->quadlets[0] = (nodeAddr << SHIFT_NODEADDR);					// destination_ID
			pHeader->quadlets[0] |= (packetType << SHIFT_TCODE) | tLabel;			// tlabel

			pHeader->quadlets[1] = (thisnodeAddr << SHIFT_NODEADDR);				// source_ID
			pHeader->quadlets[1] |= destOffset.High;								// destination_offsetHigh
			pHeader->quadlets[2] = destOffset.Low;									// destination_offsetLow

			// set up lockType
			if (reqType == LHL_REQ_LOCK)
			{
				if (lockType != PB_LOCKTYPE_COMPARE_SWAP)
				{
					hResult = E_LHL_UNSUPPORTED_LOCK_TYPE;
					sysLogError(hResult, __LINE__, moduleName);
					return hResult;
				}
				pHeader->quadlets[3] |= lockType;									// extended_tcode
			}
			break;
#ifdef _LOOSE_ISO
		case LHL_REQ_STREAM:
			{
				uint8	channel; 
				uint8	tag;
				uint8	sy;

				// decode channel, tag and sy from destOffset for comforming with general handling
				hResult = lhlLLCGetAsyncStreamQuad(destOffset.Low, &channel, &tag, &sy);
				if (hResult != NO_ERROR) return hResult;

				hResult = lhlLLCSetAsyncStreamHeader(pHeader->quadlets, channel, tag, sy);
				if (hResult != NO_ERROR) return hResult;
			}
			break;
#endif //_LOOSE_ISO
	}

	return hResult;
}

/*****************************************************************************

	lhlReq1394HandlePayload

	lhlReq1394 helper function for handling payload data

*****************************************************************************/

HRESULT lhlReq1394HandlePayload(PB*		packetBlock,
						uint16			numBytes,
						QUADLET			*data,
						uint16			reqType)
{
	HRESULT			hResult = NO_ERROR;

	switch (reqType)
	{
		case LHL_REQ_READ:
			break;
		case LHL_REQ_WRITE:
		case LHL_REQ_LOCK:
#ifdef _LOOSE_ISO
		case LHL_REQ_STREAM:
#endif //_LOOSE_ISO
			{
				uint32*	pPayload;
				uint16	i;
				
				// copy the request data into the payload
				pbGetPayload(packetBlock, (void **) &pPayload);

				for (i = 0; i < numBytes / 4; i++)
				{
					*(pPayload + i) = data[i];
				}
			}
			break;
	}
	
	return hResult;
}

/*****************************************************************************

	lhlReq1394()

	this function handles the actual request
	using the callback function and a blocking semaphore, 
	it enqueues the request and waits for the result or a timeout.
	
	The function returns:
	
		Read:	with the data available in the supplied buffer, 
				or an error in which case there is no valid data.
		Write:	with the hResult of the write response 
				or with the hResult of the write request if the was an error.		
		Lock:	with the data available in the supplied buffer, 
				or an error in which case there is no valid data.
				this function supports only compare_swap lock types.

*****************************************************************************/
HRESULT lhlReq1394New(	LAL_NODE_HANDLE handle,
						uint32 nodeAddr,
						OFFSET_1394 destOffset,
						uint16 numBytes,
						QUADLET *data,
						uint16 queue,
						uint16 reqType,
						PB_LOCKTYPE lockType, 
						BOOL bNonBlocking, 
						uint32 msec)
{
	HRESULT		hResult = NO_ERROR;
	PB			*packetBlock = NULL;
	BOOL		bPayloadReferenced = TRUE;

	// trap local requests here
	if (briIsThisNodeAddr(nodeAddr))
	{
#ifdef _FCP_LOCAL
		if (destOffset.High == REGISTER_SPACE_ADDR_HI && 
			(destOffset.Low == FCP_COMMAND_BASE_ADDR || destOffset.Low == FCP_RESPONSE_BASE_ADDR))
		{
		}
		else
#endif //_FCP_LOCAL
		{
			return lhlReqLocalNode(destOffset, numBytes, data, reqType);
		}
	}

	
	if (reqType == LHL_REQ_LOCK) bPayloadReferenced = FALSE;	// compare data will be overwritten otherwise

	hResult = pbCreatePacketOptimize(&packetBlock, handle, NULL, numBytes, PB_CREATE_LHL_REQ, bPayloadReferenced, (uint8 *) data);
	if (hResult != NO_ERROR) return hResult;

#if 1 //LM???
	pbSetPacketSpeed(packetBlock, (lal400 << SHIFT_SPEEDCODE));
#else
	{
		LAL_SPEED		speed = lalSpeedDefault;
		hResult = nciGetSpeed(handle, &speed);
		if (hResult != NO_ERROR)
		{
#ifdef _BM
			hResult = lalGetNodeAddrSpeed(nodeAddr, &speed);
#endif //_BM
		}
		pbSetPacketSpeed(packetBlock, (speed << SHIFT_SPEEDCODE));
	}
#endif

	pbSetNonBlocking(packetBlock, bNonBlocking);
	
	hResult = lhlReq1394(packetBlock, nodeAddr, destOffset, numBytes, data, queue, reqType, lockType, msec);

	if (bNonBlocking == FALSE)
	{
		if (pbIsValid(packetBlock))
		{
			if (pbIsNonBlocking(packetBlock) == FALSE)
			{
				packetBlock->creatorCBProc = NULL;
				pbPacketDone(packetBlock, PB_DONE_LHL_REQ);
			}
		}
	}

	return hResult;
}

static HRESULT lhlReqReadCB(PB *packetBlock);
static HRESULT lhlReqWriteCB(PB *packetBlock);
static HRESULT lhlReqLockCB(PB *packetBlock);
#ifdef _LOOSE_ISO
static HRESULT lhlReqStreamCB(PB *packetBlock);
#endif //_LOOSE_ISO

HRESULT	lhlReq1394(PB* packetBlock,
					uint32 nodeAddr,
					OFFSET_1394 destOffset,
					uint16 numBytes,
					QUADLET *data,
					uint16 queue,
					uint16 reqType,
					PB_LOCKTYPE lockType, 
					uint32 msec)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultMatch = NO_ERROR;
	PB_PACKETTYPE	packetType = PB_TYPE_UNDEF;
	PB_PACKETTYPE	packetTypeReq = PB_TYPE_UNDEF;
	STATUS_INFO 	*statusInfo = NULL;
	uint32			tLabel = 0;
#ifdef _SYSDEBUGERROR
	uint32			starttime = 0;
#endif //_SYSDEBUGERROR

	if (!pbIsValid(packetBlock))
	{
		hResult = E_LHL_INVALID_PB;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (!lhlIsPayloadSizeValid(packetBlock, numBytes,reqType==LHL_REQ_READ))
	{
		hResult = E_LHL_INVALID_SIZE;
		sysLogError(hResult, __LINE__, moduleName);
		SYS_DEBUG(SYSDEBUG_TRACE_PACKETS | SYSDEBUG_TRACE_ERRORS,
				"lhlReq: payload size not valid: %i, reqType:0x%04x\n\r", numBytes, reqType);
		
		return hResult;
	}

	switch (reqType)
	{
		case LHL_REQ_READ:
		case LHL_REQ_WRITE:
		case LHL_REQ_LOCK:
			hResult = lhlTLabelGetShifted(&tLabel);
			if (hResult != NO_ERROR) return hResult;
			break;

#ifdef _LOOSE_ISO
		case LHL_REQ_STREAM:
			{
				uint8	channel; 
				uint8	tag;
				uint8	sy;

				// decode channel, tag and sy from destOffset for comforming with general handling
				hResult = lhlLLCGetAsyncStreamQuad(destOffset.Low, &channel, &tag, &sy);
				if (hResult != NO_ERROR) return hResult;
				hResult = lhlTLabelGetShiftedAsyncStream(channel, &tLabel);
				if (hResult != NO_ERROR) return hResult;
			}
			break;
#endif //_LOOSE_ISO
	}

	hResult = lhlReq1394HandleHeader(packetBlock, nodeAddr, destOffset, tLabel, numBytes, reqType, lockType);
	if (hResult != NO_ERROR) goto ErrorExit;

	if (pbIsPayloadReferenced(packetBlock) == FALSE)
	{
		hResult = lhlReq1394HandlePayload(packetBlock, numBytes, data, reqType);
		if (hResult != NO_ERROR) goto ErrorExit;
	}

#ifdef _STATISTICS
	lhlStatistics.TxReqCount++;
#endif //_STATISTICS

#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_PACKETS))
	{
		sysDebugPrintf("lhlReq EnQPacket: nodeAddr 0x%04x, tLabel 0x%04x, reqType:0x%04x\n\r", nodeAddr, tLabel, reqType);
#if 1
		if (reqType != LHL_REQ_READ)
		{
			uint32*	pPayload;
			uint32	payloadSize;
			uint16	i;
				
			// copy the request data into the payload
			pbGetPayload(packetBlock, (void **) &pPayload);
			pbGetPayloadSize(packetBlock, &payloadSize);

			sysDebugPrintf("payload data [%i]:\n\r", payloadSize);
			for (i = 0; i < (payloadSize + 3) / 4; i++)
			{
				sysDebugPrintf("%i: 0x%08x\n\r", i, pPayload[i]);
			}
		}
#endif
	}
#endif //_SYSDEBUG

#ifdef _FCP_LOCAL
	{
		// trap local requests here
		if ((briIsThisNodeAddr(nodeAddr) || briIsBroadcastNodeAddr(nodeAddr)) &&
			destOffset.High == REGISTER_SPACE_ADDR_HI && 
			(destOffset.Low == FCP_COMMAND_BASE_ADDR || destOffset.Low == FCP_RESPONSE_BASE_ADDR))
		{
			PB	*duplicatedPacketBlock;
	
			hResult = pbCreateDuplicatePacket(packetBlock, &duplicatedPacketBlock, NULL, PB_CREATE_LHL_REQ);
			if (hResult != NO_ERROR) return hResult;

			if (briIsBroadcastNodeAddr(nodeAddr) == FALSE)
			{
				if (pbIsNonBlocking(packetBlock) == TRUE)
				{
					packetBlock->creatorCBProc = NULL;
					pbPacketDone(packetBlock, PB_DONE_LHL_REQ);
				}
			}

			hResult = fcpCallback(duplicatedPacketBlock);

			if (briIsBroadcastNodeAddr(nodeAddr) == FALSE)
			{
				goto ErrorExit;
			}
		}
	}
#endif //_FCP_LOCAL

	if (pbIsNonBlocking(packetBlock) == TRUE)
	{
		hResult = lhlMsgQueueEnQPacketBlock(queue, packetBlock);
	}
	else
	{
		hResult = pbGetPacketType(packetBlock, &packetType); // call after lhlReq1394HandleHeader
		if (hResult != NO_ERROR) goto ErrorExit;

		hResult = lhlGetRequestPacketType(reqType, packetType, &packetTypeReq);
		if (hResult != NO_ERROR) goto ErrorExit;

		hResult = lhlStatusQueueCreate(&statusInfo);
		if (hResult != NO_ERROR) goto ErrorExit;

		statusInfo->nodeAddr = nodeAddr;
		statusInfo->tLabel = tLabel;
		statusInfo->genType = reqType;
		statusInfo->packetType = packetTypeReq;
		statusInfo->numBytes = numBytes;
		statusInfo->data = data;

		// install our own callback, low-level behavior only
		switch (reqType)
		{
			case LHL_REQ_READ:
				packetBlock->creatorCBProc = (PB_CALLBACK)lhlReqReadCB;
				break;
			case LHL_REQ_WRITE:
				packetBlock->creatorCBProc = (PB_CALLBACK)lhlReqWriteCB;
				break;
			case LHL_REQ_LOCK:
				packetBlock->creatorCBProc = (PB_CALLBACK)lhlReqLockCB;
				break;
#ifdef _LOOSE_ISO
			case LHL_REQ_STREAM:
				packetBlock->creatorCBProc = (PB_CALLBACK)lhlReqStreamCB;
#ifdef _STATISTICS
				lhlStatistics.TxStmCount++;
#endif //_STATISTICS
				break;
#endif //_LOOSE_ISO
		}

#ifdef _SYSDEBUGERROR
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_ERRORS))
		{
			starttime = TCTimeGet();
		}
#endif //_SYSDEBUGERROR
		
		hResult = lhlMsgQueueEnQPacketBlockWaitTimeout(queue, packetBlock, statusInfo->semID, msec);

		/*
			The request has been queued and the packet transmit thread will send it.
			The packet will be acknowledged by the target.
			The ack will be matched to the nodeAddess and transaction label for this packet, 
			causing the callback function to be called.
			The callback will then release the original request packet if no errors.
			When the following response packet arrives, it is matched by nodeAddress and transaction label, 
			causing the callback to execute again.
			If the transaction was successful, the returned data is copied into the	statusInfo structures 
			and the semaphore is incremented, where we now continue from here.
			If the original request had an error, the semaphore is incremented and
			we wake up here and free the original request and return the error to the caller.
		*/

		SYS_DEBUG(SYSDEBUG_TRACE_PACKETS, "lhlReq EnQPacket: %s nodeAddr 0x%04x\n\r", (hResult == NO_ERROR) ? "finish" : "timeout", nodeAddr);

		hResultMatch = lhlStatusQueueMatchFinish(nodeAddr, tLabel, reqType, packetTypeReq);

		if (hResult == NO_ERROR)
		{
			// semaphore was incremented by the callback
			hResult = hResultMatch;

#ifdef _SYSDEBUGERROR
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_ERRORS))
			{
				if (hResultMatch)
				{
					sysDebugPrintf("lhlReq statusQueue returns hResult: 0x%08x\n\r", hResult);
				}
			}
#endif //_SYSDEBUG
		}
		else if (hResult == E_SEMAPHORE_TIMEOUT)
		{
			// semaphore timed out, usually due to no response from the target
			hResult = E_LAL_REQUEST_TIMEOUT;
			sysLogError(hResult, __LINE__, moduleName);
#ifdef _STATISTICS
			lhlStatistics.TxReqTimeout++;
#endif //_STATISTICS
#ifdef _SYSDEBUGERROR
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_ERRORS))
			{
				char timestr[16];

				sysGetCurTimeStr(timestr);
				sysDebugPrintf("lhlReq Timeout nodeAddr 0x%04x, tLabel 0x%04x (%i), type:0x%02x, ", nodeAddr, tLabel, tLabel >> SHIFT_TLABEL, reqType);
#ifdef _CLI
				lhlCliPrintTCode(packetTypeReq);
#endif
				sysDebugPrintf(", numBytes:%i\n\r", numBytes);
				sysDebugPrintf("lhlReq Timeout time before enqueue: ");
				sysPrintTime(starttime);
				sysDebugPrintf("now: %s\n\r", timestr);
			}
#endif //_SYSDEBUGERROR
			// we had an error, remove the packet in the pending list
			if (hResult != NO_ERROR)
			{
				// error in sending req - match corresponding type in PendingTx
				lhlPendingTxMatch(tLabel, nodeAddr, LHL_PENDING_REQ, NULL, REMOVE_PENDING_ENTRY);
			}
		}
	}

ErrorExit: // make sure to put back the tlabel to the pool even if an error occured

	switch (reqType)
	{
		case LHL_REQ_READ:
		case LHL_REQ_WRITE:
		case LHL_REQ_LOCK:
			lhlTLabelPutShifted(tLabel);
			break;

#ifdef _LOOSE_ISO
		case LHL_REQ_STREAM:
			lhlTLabelPutShiftedAsyncStream(tLabel);
			break;
#endif //_LOOSE_ISO
	}

	return hResult;
}

/*****************************************************************************

	lhlReqCB

	this callback handles the acknowledge of the original request and the 
	corresponding response from the target node.  acknowledge and response
	are matched using the nodeAddress and the transaction label for the 
	original request.  the callback also handles disposal of packet blocks.

	the transaction status is supplied by the transmit packet queue handler.

	Note: this is a special case where the packet is handled directly in the
	callback.  In the case of an AddressRangeCallback, the packet block should
	be queued and handled by a thread that waits on that queue.

*****************************************************************************/
static HRESULT lhlReqCB(PB *packetBlock, uint16 reqType)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultFinal = NO_ERROR;
	PB_PACKETTYPE	packetType = PB_TYPE_UNDEF;
	PB_PACKETTYPE	packetTypeReq = PB_TYPE_UNDEF;
	PB_HEADER		*pHeader = 0;
	STATUS_INFO 	*statusInfo = NULL;
	uint32			nodeAddr = 0;
	uint32			tLabel = 0;
	uint32			payloadNumBytes = 0;
	uint32			*pPayload = 0;
	BOOL			bSignalSema = TRUE;

	hResult = pbGetPacketHeader (packetBlock, &pHeader);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPacketType(packetBlock, &packetType);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPayload(packetBlock, (void **) &pPayload);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPayloadSize(packetBlock, &payloadNumBytes);
	if (hResult != NO_ERROR) return hResult;

	switch (reqType)
	{
		case LHL_REQ_READ:
		case LHL_REQ_WRITE:
		case LHL_REQ_LOCK:
			hResult = pbGetPacketTLabel (packetBlock, &tLabel);
			if (hResult != NO_ERROR) return hResult;
			hResult = pbGetPacketSrcNodeAddr (packetBlock, &nodeAddr);
			if (hResult != NO_ERROR) return hResult;
			break;

#ifdef _LOOSE_ISO
		case LHL_REQ_STREAM:
			{
				uint8	channel; 
				uint8	tag;
				uint8	sy;

				hResult = lhlLLCGetAsyncStreamHeader(pHeader->quadlets, &channel, &tag, &sy);
				if (hResult != NO_ERROR) return hResult;
				tLabel = channel;
				tLabel = tLabel << SHIFT_TLABEL;
				nodeAddr = LHL_ASYNC_DUMMY_NODE_ADDR;
			}
			break;
#endif //_LOOSE_ISO
	}

	hResult = lhlGetRequestPacketType(reqType, packetType, &packetTypeReq);
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlStatusQueueMatchUpdate(nodeAddr, tLabel, reqType, packetTypeReq, &statusInfo);
	if (hResult != NO_ERROR) return hResult;

	switch (packetType)
	{
		case PB_TYPE_READ_REQUEST_QUADLET:
		case PB_TYPE_READ_REQUEST:
		case PB_TYPE_WRITE_REQUEST_QUADLET:
		case PB_TYPE_WRITE_REQUEST:
		case PB_TYPE_LOCK_REQUEST:
#ifdef _LOOSE_ISO
		case PB_TYPE_ASYNC_STREAM:
#endif //_LOOSE_ISO
			// the original request packet confirmation
			// if no error keep waiting until rsp arrives or times out
			// otherwise, wake up caller which will look at the error status
			pbGetFinalStatus (packetBlock, &hResultFinal);

			if ((hResultFinal == NO_ERROR) 
				&& (packetType != PB_TYPE_WRITE_REQUEST_QUADLET)	// POSTED_WRITES
				&& (packetType != PB_TYPE_WRITE_REQUEST)			// POSTED_WRITES
#ifdef _LOOSE_ISO
				&& (packetType != PB_TYPE_ASYNC_STREAM)
#endif //_LOOSE_ISO
				//LM??? check BCast packets (no response - complete) ????
				)
			{
				bSignalSema = FALSE; 
			}
			break;

		case PB_TYPE_READ_RESPONSE_QUADLET:
		case PB_TYPE_READ_RESPONSE:
		case PB_TYPE_WRITE_RESPONSE:	
		case PB_TYPE_LOCK_RESPONSE:		
			// the response packet
			hResultFinal = (HRESULT) ((pHeader->quadlets[1] >> SHIFT_RCODE) & MASK_RCODE);
			if (hResultFinal != NO_ERROR)
			{
				hResultFinal += E_LHL_RSP_BASE;
			}

			if (hResultFinal == NO_ERROR) 
			{
				switch (packetType)
				{
					case PB_TYPE_READ_RESPONSE_QUADLET:
					case PB_TYPE_READ_RESPONSE:
						// the read response packet
						if (statusInfo->numBytes == payloadNumBytes &&
//						if ((statusInfo->numBytes == payloadNumBytes ||
//							((statusInfo->numBytes + 3) / 4 == payloadNumBytes / 4)) &&	//LM??? allow reading non-quadlet sized requests
							((packetType == PB_TYPE_READ_RESPONSE_QUADLET && payloadNumBytes == 4) ||
							(packetType == PB_TYPE_READ_RESPONSE && payloadNumBytes != 0)))
						{
							if (pbIsPayloadReferenced(packetBlock) == FALSE)
							{
								uint32 numQuads;
								// load response data into the transfer data buffer
								for (numQuads = 0; numQuads < (payloadNumBytes / 4); numQuads++)
								{
									statusInfo->data[numQuads] = pPayload[numQuads];
#if 0 //def _SYSDEBUG
									if (sysDebugIsEnabled(SYSDEBUG_TRACE_PACKETS))
									{
										sysDebugPrintf("%i: 0x%08x\n\r", numQuads, statusInfo->data[numQuads]);
									}
#endif //_SYSDEBUG
								}
							}
						}
						else
						{
#ifdef _SYSDEBUG
							if (sysDebugIsEnabled(SYSDEBUG_TRACE_PACKETS) || sysDebugIsEnabled(SYSDEBUG_TRACE_ERRORS))
							{
#ifdef _CLI
								lhlCliPrintTCode(packetType);
#endif
								sysDebugPrintf(" size %i: 0x%08x\n\r", statusInfo->numBytes, payloadNumBytes);
							}
#endif //_SYSDEBUG
							hResult = E_LHL_INVALID_SIZE;
							hResultFinal = hResult;
							sysLogError(hResult, __LINE__, moduleName);
						}
						break;

					case PB_TYPE_WRITE_RESPONSE:
						// the write response packet
						break;
					
					case PB_TYPE_LOCK_RESPONSE:
						// the lock response packet
						// check for compare_swap success/fail 
						if (statusInfo->numBytes != 0 && 
							(statusInfo->numBytes / 2) == payloadNumBytes) // req:arg_value(32/64bit) data_value(32/64bit) - rsp:old_value(32/64bit) 
						{
							uint32 numQuads;

#ifdef _SYSDEBUG
							if (sysDebugIsEnabled(SYSDEBUG_TRACE_WARNINGS))
							{
								sysDebugPrintf("lhlReqCB: Lock returned: ");
								for (numQuads = 0; numQuads < (payloadNumBytes / 4); numQuads++)
								{
									sysDebugPrintf("0x%08x, ", pPayload[numQuads]);
								}
								sysDebugPrintf("\n\r");
								sysDebugPrintf("lhlReqCB: lock arg and data: ");
								for (numQuads = 0; numQuads < (statusInfo->numBytes / 4); numQuads++)
								{
									sysDebugPrintf("0x%08x, ", statusInfo->data[numQuads]);
								}
								sysDebugPrintf("\n\r");
							}
							else if (sysDebugIsEnabled(SYSDEBUG_TRACE_PACKETS))
							{
								sysDebugPrintf("lhlReqCB: Lock returned: ");
								for (numQuads = 0; numQuads < (payloadNumBytes / 4); numQuads++)
								{
									sysDebugPrintf("0x%08x, ", pPayload[numQuads]);
								}
								sysDebugPrintf("\n\r");
							}
#endif //_SYSDEBUG

							for (numQuads = 0; numQuads < (payloadNumBytes / 4); numQuads++)
							{
								if (pPayload[numQuads] != statusInfo->data[numQuads]) // req:arg_value(32/64bit) == rsp:old_value(32/64bit) 
								{
									hResult = E_LAL_LOCK_COMPARE_SWAP_FAIL;
									break;
								}
							}
						}
						else
						{
							hResult = E_LAL_LOCK_COMPARE_SWAP_FAIL;
						}
						if (hResult != NO_ERROR)
						{
							sysLogError(hResult, __LINE__, moduleName);
							hResultFinal = hResult;
						}
						break;
				}
			}
			break;
	}

	SYS_DEBUG(SYSDEBUG_TRACE_PACKETS, "lhlReqCB: nodeAddr 0x%04x, tLabel 0x%04x, status: 0x%08x\n\r", nodeAddr, tLabel, hResultFinal);

	if (bSignalSema == TRUE)
	{
		statusInfo->hResultFinal = hResultFinal;
		TCSemaphoreSignal(statusInfo->semID); // wake up caller which will look at the status.
	}

	return hResult;
}

static HRESULT lhlReqReadCB(PB *packetBlock)
{
	return lhlReqCB(packetBlock, LHL_REQ_READ);
}

static HRESULT lhlReqWriteCB(PB *packetBlock)
{
	return lhlReqCB(packetBlock, LHL_REQ_WRITE);
}

static HRESULT lhlReqLockCB(PB *packetBlock)
{
	return lhlReqCB(packetBlock, LHL_REQ_LOCK);
}

#ifdef _LOOSE_ISO
static HRESULT lhlReqStreamCB(PB *packetBlock)
{
	return lhlReqCB(packetBlock, LHL_REQ_STREAM);
}
#endif //_LOOSE_ISO


