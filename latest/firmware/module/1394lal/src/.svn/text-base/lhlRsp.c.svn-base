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
 * DESCRIPTION: Link Hardware Abstraction Layer (Response)

	Purpose:	Link Hardware Abstraction Layer transmit packet i/o
				handling for asynchronous transmit packets
				Response Packet functions (Read, Write, Lock)
	
	Revisions:
		created 04/12/2002 lm

****************************************************************************/

#include "TCTasking.h"
#include "lhl.h"
#include "lhlCli.h"
#include "lhlLLC.h"
#include "lhlMsgQueue.h"
#include "lhlPendingTx.h"
#include "lhlRsp.h"
#include "lhlStatistics.h"
#include "lhlStatusQueue.h"
#include "bri.h"
#ifdef _FCP_LOCAL
#include "fcp.h"
#endif //_FCP_LOCAL

static char 		moduleName[] = __FILE__ ;

/*****************************************************************************

	lhlRsp1394HandleHeader()

	lhlRsp1394 helper function for handling header data

*****************************************************************************/

HRESULT	lhlRsp1394HandleHeader(PB* packetBlock,
						RCODE_1394 responseCode,
						uint32 *nodeAddr,
						uint32 *tLabel,
						uint16 numBytes,
						uint16 rspType)
{
	HRESULT			hResult = NO_ERROR;
	PB_HEADER		*pHeader = 0;
	PB_PACKETTYPE	packetTypeOrig = PB_TYPE_UNDEF;
	PB_PACKETTYPE	packetType = PB_TYPE_UNDEF;
	uint16			payloadNumBytes = 0;
	uint32			thisnodeAddr = 0;
	
	// set up the packet header
	hResult = pbGetPacketHeader (packetBlock, &pHeader);
	if (hResult != NO_ERROR) return hResult;
	
	hResult = pbGetPacketType(packetBlock, &packetTypeOrig);
	if (hResult != NO_ERROR) return hResult;
	hResult = lhlGetResponsePacketType(rspType, packetTypeOrig, &packetType);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbSetPacketType(packetBlock, packetType);
	if (hResult != NO_ERROR) return hResult;

	hResult = pbGetPacketSrcNodeAddr (packetBlock, nodeAddr);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPacketTLabel (packetBlock, tLabel);
	if (hResult != NO_ERROR) return hResult;

	hResult = briGetThisNodeAddr(&thisnodeAddr);
	if (hResult != NO_ERROR) return hResult;

	pHeader->quadlets[0] &= 0x0000ff0f;												// preserve speed, tLabel, retry protocol and pri
	pHeader->quadlets[0] |= (*nodeAddr << SHIFT_NODEADDR);							// destination_ID
	pHeader->quadlets[0] |= (packetType << SHIFT_TCODE);							// tCode

	pHeader->quadlets[1] = (thisnodeAddr << SHIFT_NODEADDR);						// source_ID
	pHeader->quadlets[1] &= 0xffff0000;
	pHeader->quadlets[1] |= (responseCode << SHIFT_RESP_CODE);						// rcode

	pHeader->quadlets[2] = 0;														// reserved

	// set up numBytes
	switch (packetType)
	{
		case PB_TYPE_READ_RESPONSE:
		case PB_TYPE_LOCK_RESPONSE:
			payloadNumBytes = numBytes;
			break;

		default:			
			payloadNumBytes = 0;
			break;
	}

	hResult = lhlLLCSetPayloadSize((uint16) packetType, PB_NUM_HEADER_QUADLETS, pHeader->quadlets, payloadNumBytes);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*****************************************************************************

	lhlRsp1394HandlePayload

	lhlRsp1394 helper function for handling payload data

*****************************************************************************/

HRESULT lhlRsp1394HandlePayload(PB* packetBlock,
						RCODE_1394 responseCode,
						uint16 numBytes,
						QUADLET *data,
						uint16 rspType)
{
	HRESULT			hResult = NO_ERROR;
	uint16			i;
	PB_PACKETTYPE	packetType = PB_TYPE_UNDEF;
	uint32			*pPayload;

	hResult = pbGetPacketType(packetBlock, &packetType);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPayload(packetBlock, (void **) &pPayload);
	if (hResult != NO_ERROR) return hResult;

	// build the payload
	switch (rspType)
	{
		case LHL_RSP_READ:
			// next quadlet is the data for quadlet read response
			if (packetType == PB_TYPE_READ_RESPONSE_QUADLET)
			{
				pPayload[0] = data[0];
			}
			else if (packetType == PB_TYPE_READ_RESPONSE)	// or data length for block read response
			{
				for (i = 0; i < numBytes / 4; i++)			// length in quads
				{
					if (responseCode != RSP_COMPLETE)
					{
						pPayload[i] = 0;
					}
					else
					{
						pPayload[i] = data[i];
					}
				}
			}
			break;

		case LHL_RSP_WRITE:
			break;

		case LHL_RSP_LOCK:
			for (i = 0; i < numBytes; i++) //LM???
			{
				pPayload[i] = data[i];
			}
			break;

		case LHL_RSP_ERROR: //LM??? check
			for (i = 0; i < numBytes / 4; i++)			// length in quads
			{
				pPayload[i] = 0;
			}
			break;
	}

	return hResult;
}

/*****************************************************************************

	lhlRsp1394GetNewPayloadSize()

	lhlRsp1394 helper function for handling payload data size

*****************************************************************************/

HRESULT lhlRsp1394GetNewPayloadSize(PB*		packetBlock,
									uint16	*payloadBytes,
									uint16	rspType)
{
	HRESULT			hResult = NO_ERROR;
	PB_HEADER		*pHeader = 0;
	PB_PACKETTYPE	packetType = PB_TYPE_UNDEF;
	uint32			dataLen = 0;

	// set up the packet header
	hResult = pbGetPacketHeader (packetBlock, &pHeader);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPacketType(packetBlock, &packetType);
	if (hResult != NO_ERROR) return hResult;

	*payloadBytes = 0;
	
	// get new the payload size
	switch (rspType)
	{
		case LHL_RSP_READ:
			break;

		case LHL_RSP_WRITE:
			break;

		case LHL_RSP_LOCK:
			break;

		case LHL_RSP_ERROR:
			if (packetType == PB_TYPE_READ_REQUEST_QUADLET)
			{
				*payloadBytes = 4;
			}
			else if (packetType == PB_TYPE_READ_REQUEST)
			{
				hResult = pbGetDataLen(packetBlock, &dataLen);
				if (hResult != NO_ERROR) return hResult;

				*payloadBytes = (uint16) dataLen;
			}
			else if ((packetType == PB_TYPE_WRITE_REQUEST_QUADLET) || (packetType == PB_TYPE_WRITE_REQUEST))
			{
				*payloadBytes = 0;
			}
			else if (packetType == PB_TYPE_LOCK_REQUEST)
			{
				hResult = pbGetDataLen(packetBlock, &dataLen);
				if (hResult != NO_ERROR) return hResult;

				*payloadBytes = (uint16) (dataLen * 2);
			}
			else
			{
				hResult = E_LHL_ASYNC_ERROR_RSP_FAILURE;
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}
			break;
	}
	
	return hResult;
}

/*****************************************************************************

	lhlRsp1394

	this function is generic for all lhlSendResponse functions

*****************************************************************************/
HRESULT lhlRsp1394New(PB *packetBlock,
					RCODE_1394 responseCode,
					uint16 numBytes,
					uint32* data,
					uint16 rspType, 
					BOOL bNonBlocking, 
					uint32 msec)
{
	HRESULT		hResult = NO_ERROR;
	PB			*respPacket = NULL;

	hResult = pbCreateDuplicatePacketWithSize(packetBlock, &respPacket, numBytes, NULL, PB_CREATE_LHL_RSP);

	pbSetNonBlocking(respPacket, bNonBlocking);
	
	if (hResult == NO_ERROR)
	{
		hResult = lhlRsp1394(respPacket, responseCode, numBytes, data, rspType, msec);
	}

	if (bNonBlocking == FALSE)
	{
		// clean up the resp packetBlock
		if (pbIsValid(respPacket))
		{
			if (pbIsNonBlocking(respPacket) == FALSE)
			{
				respPacket->creatorCBProc = NULL;
				pbPacketDone(respPacket, PB_DONE_LHL_RSP);
			}
		}
	}
	
	return hResult;
}

static HRESULT lhlRspReadCB(PB *packetBlock);
static HRESULT lhlRspWriteCB(PB *packetBlock);
static HRESULT lhlRspLockCB(PB *packetBlock);
static HRESULT lhlRspErrorCB(PB *packetBlock);


HRESULT lhlRsp1394(PB *packetBlock,
					RCODE_1394 responseCode,
					uint16 numBytes,
					uint32* data,
					uint16 rspType, 
					uint32 msec)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultMatch = NO_ERROR;
	PB_PACKETTYPE	packetTypeOrig = PB_TYPE_UNDEF;
	PB_PACKETTYPE	packetTypeRsp = PB_TYPE_UNDEF;
	STATUS_INFO 	*statusInfo = NULL;
	uint32			nodeAddr = 0;
	uint32			tLabel = 0;

	if (!pbIsValid(packetBlock))
	{
		hResult = E_LHL_INVALID_PB;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (rspType == LHL_RSP_ERROR)
	{
		if (pbPacketIsBroadcast(packetBlock)) // do not send error responses to broadcasts packets
		{
			if (pbIsNonBlocking(packetBlock) == TRUE)
			{
				packetBlock->creatorCBProc = NULL;
				pbPacketDone(packetBlock, PB_DONE_LHL_RSP);
			}
			return hResult;
		}
	}

#ifdef _FCP_LOCAL
	{
		OFFSET_1394	destOffset;

		// trap local responses here
		hResult = pbGetPacketSrcNodeAddr (packetBlock, &nodeAddr);
		if (hResult != NO_ERROR) return hResult;

		hResult = pbGetDestinationOffset(packetBlock, &destOffset);
		if (hResult != NO_ERROR) return hResult;

		if (briIsThisNodeAddr(nodeAddr) &&
			destOffset.High == REGISTER_SPACE_ADDR_HI && 
			(destOffset.Low == FCP_COMMAND_BASE_ADDR || destOffset.Low == FCP_RESPONSE_BASE_ADDR))
		{
			if (pbIsNonBlocking(packetBlock) == TRUE)
			{
				packetBlock->creatorCBProc = NULL;
				pbPacketDone(packetBlock, PB_DONE_LHL_RSP);
			}
			return hResult; // just skip response packet
		}
	}
#endif //_FCP_LOCAL

	hResult = pbGetPacketType(packetBlock, &packetTypeOrig); // call before lhlRsp1394HandleHeader
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlRsp1394HandleHeader(packetBlock, responseCode, &nodeAddr, &tLabel, numBytes, rspType);
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlRsp1394HandlePayload(packetBlock, responseCode, numBytes, data, rspType);
	if (hResult != NO_ERROR) return hResult;

#ifdef _STATISTICS
	lhlStatistics.TxRspCount++;
#endif //_STATISTICS

#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_PACKETS))
	{
		sysDebugPrintf("lhlRsp EnQPacket: nodeAddr 0x%04x, tLabel 0x%04x, rspType:0x%04x\n\r", nodeAddr, tLabel, rspType);

		if (sysDebugIsEnabled(SYSDEBUG_TRACE_ERRORS))
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
	}
#endif //_SYSDEBUG

	if (pbIsNonBlocking(packetBlock) == TRUE)
	{
		hResult = lhlMsgQueueEnQPacketBlock(LHL_QUEUE_PRIORITY, packetBlock);
		if ((hResult != NO_ERROR) && pbIsNonBlocking(packetBlock) == TRUE)
		{
			pbPacketDone(packetBlock, PB_DONE_LHL_RSP);
		}
	}
	else
	{
		hResult = lhlGetResponsePacketType(rspType, packetTypeOrig, &packetTypeRsp);
		if (hResult != NO_ERROR) return hResult;

		hResult = lhlStatusQueueCreate(&statusInfo);
		if (hResult != NO_ERROR) return hResult;

		statusInfo->nodeAddr = nodeAddr;
		statusInfo->tLabel = tLabel;
		statusInfo->genType = rspType;
		statusInfo->packetType = packetTypeRsp;
		statusInfo->numBytes = numBytes;
		statusInfo->data = data;

		// install our own callback, low-level behavior only
		switch (rspType)
		{
			case LHL_RSP_READ:
				packetBlock->creatorCBProc = (PB_CALLBACK)lhlRspReadCB;
				break;
			case LHL_RSP_WRITE:
				packetBlock->creatorCBProc = (PB_CALLBACK)lhlRspWriteCB;
				break;
			case LHL_RSP_LOCK:
				packetBlock->creatorCBProc = (PB_CALLBACK)lhlRspLockCB;
				break;
			case LHL_RSP_ERROR:
				packetBlock->creatorCBProc = (PB_CALLBACK)lhlRspErrorCB;
				break;
		}

		hResult = lhlMsgQueueEnQPacketBlockWaitTimeout(LHL_QUEUE_PRIORITY, packetBlock, statusInfo->semID, msec);

		SYS_DEBUG(SYSDEBUG_TRACE_PACKETS, "lhlRsp EnQPacket: %s nodeAddr 0x%04x\n\r", (hResult == NO_ERROR) ? "finish" : "timeout", nodeAddr);

		hResultMatch = lhlStatusQueueMatchFinish(nodeAddr, tLabel, rspType, packetTypeRsp);

		if (hResult == NO_ERROR)
		{
			// semaphore was incremented by the callback
			hResult = hResultMatch;

#ifdef _SYSDEBUGERROR
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_ERRORS))
			{
				if (hResultMatch)
				{
					sysDebugPrintf("lhlRsp statusQueue returns hResult: 0x%08x\n\r", hResult);
				}
			}
#endif //_SYSDEBUG
		}
		else if (hResult == E_SEMAPHORE_TIMEOUT)
		{
			// semaphore timed out, usually due to no response from the target
			hResult = E_LAL_RESPONSE_TIMEOUT;
			sysLogError(hResult, __LINE__, moduleName);
#ifdef _STATISTICS
			lhlStatistics.TxRspTimeout++;
#endif //_STATISTICS
			// we had an error, remove the packet in the pending list
			if (hResult != NO_ERROR)
			{
				// error in sending rsp - match corresponding type in PendingTx
				lhlPendingTxMatch(tLabel, nodeAddr, LHL_PENDING_RSP, NULL, REMOVE_PENDING_ENTRY);
			}
		}
	}

	return hResult;
}

/*****************************************************************************

	lhlRspCB

	this callback handles the acknowledge of the response

*****************************************************************************/
static HRESULT lhlRspCB(PB *packetBlock, uint16 rspType)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultFinal = NO_ERROR;
	PB_PACKETTYPE	packetType = PB_TYPE_UNDEF;
	PB_PACKETTYPE	packetTypeRsp = PB_TYPE_UNDEF;
	STATUS_INFO 	*statusInfo = NULL;
	uint32			nodeAddr;
	uint32			tLabel;
	BOOL			bSignalSema = TRUE;
	
	hResult = pbGetPacketType(packetBlock, &packetType);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetFinalStatus(packetBlock, &hResultFinal);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPacketSrcNodeAddr (packetBlock, &nodeAddr);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPacketTLabel (packetBlock, &tLabel);
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlGetResponsePacketType(rspType, packetType, &packetTypeRsp);
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlStatusQueueMatchUpdate(nodeAddr, tLabel, rspType, packetTypeRsp, &statusInfo);
	if (hResult != NO_ERROR) return hResult;

	SYS_DEBUG(SYSDEBUG_TRACE_PACKETS, "lhlRspCB: nodeAddr 0x%04x, tLabel 0x%04x, status: 0x%08x\n\r", nodeAddr, tLabel, hResultFinal);

	if (bSignalSema == TRUE)
	{
		statusInfo->hResultFinal = hResultFinal;
		TCSemaphoreSignal(statusInfo->semID); // wake up caller which will look at the status.
	}

	return hResult;
}

static HRESULT lhlRspReadCB(PB *packetBlock)
{
	return lhlRspCB(packetBlock, LHL_RSP_READ);
}

static HRESULT lhlRspWriteCB(PB *packetBlock)
{
	return lhlRspCB(packetBlock, LHL_RSP_WRITE);
}

static HRESULT lhlRspLockCB(PB *packetBlock)
{
	return lhlRspCB(packetBlock, LHL_RSP_LOCK);
}

static HRESULT lhlRspErrorCB(PB *packetBlock)
{
	return lhlRspCB(packetBlock, LHL_RSP_ERROR);
}
