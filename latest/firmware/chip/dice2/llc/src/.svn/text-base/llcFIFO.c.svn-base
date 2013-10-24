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
	
	Purpose:	LLC initialization and abstraction layer for
				FIFO handling for asynchronous transmit and receive packets
				for the Samsung 1394 LLC
	
	Revisions:
		created 12/18/2002 kbj

****************************************************************************/

#include "1394.h"
#include "ErrorCodes.h"
#include "TCTypes.h"
#include "TCTasking.h"
#include "TCDebug.h"
#include "llcReg.h"
#include "llcInt.h"
#include "llcFIFO.h"
#include "packetBlock.h"
#include "lhl.h"
#include "lhlARx.h"
#include "lhlLLCDefs.h"
#include "lhlLLC.h"
#include "lal.h"
#include "lhlStatistics.h"
#include "lhlPendingTx.h"
#include "llcUtils.h"

static char 		moduleName[] = __FILE__ ;

#define TX_FIFO_FIRST		BIT0	 
#define TX_FIFO_LAST		BIT1


// Resetting TX and RX FIFO buffers
void llcResetFIFOBuffers(void)
{
	llcLinkSetBits(ARFIFO_STAT_REG_DP, LLC_ARFIFO_CLR);
	llcLinkSetBits(ATFIFO_STAT_REG_DP, LLC_ATFIFO_CLR);
}



/****************************************************************************
 ***************************************************************************

	Packet Receive

 ***************************************************************************
****************************************************************************/
#ifdef _LOOSE_ISO

static HRESULT lhlARxGetStreamPacket(uint32 tLabel, uint16 tCode, uint32 nodeAddr, PB *packetBlock)
{
	// assume valid stream packet
	// look for it in the stream channel callback table and call the callback if specified.

	HRESULT			hResult = NO_ERROR;
	PB_HEADER		*pHeader = NULL;
	LAL_CALLBACK	callback = NULL;

	UNUSED_ARG(tLabel);
	UNUSED_ARG(tCode);
	UNUSED_ARG(nodeAddr);

#ifdef _STATISTICS
	lhlStatistics.RxStmCount++;
#endif //_STATISTICS
	
	if (lalAsyncStreamIsEnabled())
	{
		uint8	channel; 
		uint8	tag;
		uint8	sy;

		hResult = pbGetPacketHeader (packetBlock, &pHeader);
		if (hResult != NO_ERROR) return hResult;
		hResult = lhlLLCGetAsyncStreamHeader(pHeader->quadlets, &channel, &tag, &sy);
		if (hResult != NO_ERROR) return hResult;

		// lookup channel in dispatch table
		if (lalAsyncStreamLookupChannelCB(channel, &callback) == TRUE)	
		{
			//	execute callback for the interested module.
			//	packetBlock will be released here if it returns other than NO_ERROR.
			hResult = (*callback)(packetBlock);
		}	
	}

	if ((pbIsValid(packetBlock)) && 
		(pbIsSelfDestruct(packetBlock) == FALSE)) 
	{
		pbPacketDone (packetBlock, PB_DONE_LHL_ARX_STREAM);
	}

	return hResult;
}
#endif //_LOOSE_ISO

// Called from ohciReadARDMAContext when a valid request packet is found
static HRESULT lhlARxGetReqPacket(uint32 tLabel, uint16 tCode, uint32 nodeAddr, PB *packetBlock)
{
	// assume valid request packet
	// look for it in the pending requests table or
	// address range callback table and call the callback if specified.

	HRESULT			hResult = NO_ERROR;
	OFFSET_1394		addressLow;
	OFFSET_1394		addressHi;
	uint32			dataLenBytes;
	LAL_CALLBACK	callback = NULL;
	BOOL			bFound = FALSE;

	UNUSED_ARG(tLabel);
	UNUSED_ARG(tCode);
	UNUSED_ARG(nodeAddr);

#ifdef _STATISTICS
	lhlStatistics.RxReqCount++;
#endif //_STATISTICS
	
	SYS_DEBUG(SYSDEBUG_TRACE_PACKETS, "lhlARx reqPacket: nodeAddr 0x%04x, tLabel 0x%04x, tCode:0x%04x\n\r", nodeAddr, tLabel, tCode);

	// set up address range based on base addr and length
	dataLenBytes = 4;

	// lower bound of address region
	hResult = pbGetDestinationOffset(packetBlock, &addressLow);
	if (hResult != NO_ERROR) return hResult;

	// upper bound
	addressHi.High = addressLow.High;

	// block read or write may cause a carry to high address
	if ((tCode == TCODE_RDBLOCK) || 
		(tCode == TCODE_WRBLOCK))	
	{
		hResult = pbGetDataLen (packetBlock, &dataLenBytes);
		if (hResult != NO_ERROR) return hResult;

		if ((0xffffffff - addressLow.Low) < dataLenBytes)	
		{
			addressHi.High++;
			addressHi.Low = dataLenBytes - (0xffffffff - addressLow.Low);
		}	
		else	
		{
			addressHi.Low = addressLow.Low + dataLenBytes;
		}
	}	
	else	
	{
		addressHi.Low = addressLow.Low + dataLenBytes;
	}

	// lookup address in dispatch table
	bFound = lalAddressRangeLookup(addressLow, addressHi, &callback);
	if (bFound == FALSE)	
	{
		// no module is interested in this address range
		hResult = E_LAL_ADDR_RNG_NOT_FOUND;
		sysLogError(hResult, __LINE__, moduleName);

#ifdef _SYSDEBUG
			sysDebugPrintf("lhlARx reqPacket: Address range not found: 0x%04x 0x%08x - 0x%04x 0x%08x\n\r", addressLow.High, addressLow.Low, addressHi.High, addressHi.Low);
#endif //_SYSDEBUG

		lhlReplyErrorResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);
	}	
	else if (callback == NULL) 
	{
		// problem with installed callback
		hResult = E_LAL_UNMAPPED_ERROR;
		sysLogError(hResult, __LINE__, moduleName);
 	}
	else
	{
		//	execute callback for the interested module.
		hResult = (*callback)(packetBlock);
	}

	if ((pbIsValid(packetBlock)) && 
		(pbIsSelfDestruct(packetBlock) == FALSE)) 
	{
		pbPacketDone (packetBlock, PB_DONE_LHL_ARX_REQ);
	}

	return hResult;
}

// Called from ohciARDMAReadContext - given a response packet find and return a matching packetBlock in the pending request packet table 
static HRESULT lhlARxHandleRspPacketFindRequest(uint32 tLabel, uint16 tCode, uint32 nodeAddr, PB **packetBlock)
{
	HRESULT	hResult = NO_ERROR;
	uint8	pendingTxType = LHL_PENDING_REQ; // receiving a response - match a request in the PendingTX
	
#ifdef _STATISTICS
	lhlStatistics.RxRspCount++;
#endif //_STATISTICS

	*packetBlock = NULL;
	
	SYS_DEBUG(SYSDEBUG_TRACE_PACKETS, "lhlARx RspPacket: nodeAddr 0x%04x, tLabel 0x%04x, tCode:0x%04x\n\r", nodeAddr, tLabel, tCode);

	UNUSED_ARG(tCode);

	// find this response in the pending table and route to callback
	hResult = lhlPendingTxMatch(tLabel, nodeAddr, pendingTxType, packetBlock, REMOVE_PENDING_ENTRY);

	if (hResult == NO_ERROR) 
	{
		// return the old request packet to its creator
		if (pbIsValid(*packetBlock))	
		{
			SYS_DEBUG(SYSDEBUG_TRACE_PACKETS, "lhlARx RspPacket: Match found (rsp to pending req)\n\r");

			if ((*packetBlock)->creatorCBProc == NULL)	
			{
				// no callback was specified, so just release the packet
				pbPacketDone(*packetBlock, PB_DONE_LHL_ARX_RSP_1);
				*packetBlock = NULL;
			}
		}	
		else 
		{
			hResult = E_LHL_INVALID_PB;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}
	}	

	return hResult;
}

/*=======================================================================================*/


static HRESULT llcCheckTrailer(QUADLET trailer, uint32 *speedCode)
{
	HRESULT		hResult = NO_ERROR;
	uint16		ackCode;

	// set packet block rx speed field
	if (speedCode)
	{
		*speedCode = (trailer & LLC_SPEED_MASK) >> LLC_SPEED_SHIFT;	
	}

	ackCode = (uint8) ((trailer & LLC_ACK_MASK) >> LLC_ACK_SHIFT);

	// checking package event AR code
	//KBJ??? why does LLC return ackcodes 0x12 and 0x11 (Unused code)
	if (((ackCode & 0xf) != ACK_1394_COMPLETE) &&
		((ackCode & 0xf) != ACK_1394_PENDING)) 
	{
		// Invalid package eventCode then skip packet
		hResult = E_LLC_ERROR_EVENT_CODE;
		sysLogError(hResult, __LINE__, moduleName);
		sysDebugPrintf("Packet E_LLC_ERROR_EVENT_CODE 0x%02x\n\r", ackCode);
	}

	return hResult;
}



static HRESULT llcProcessRequest (QUADLET *qData, uint16 headerNumQuads, uint16 payloadNumQuads, uint32 tLabel, uint16 tCode, uint32 nodeAddr)
{
	HRESULT		hResult = NO_ERROR;
	QUADLET		*pPBPayload = NULL;
	PB			*packetBlock = NULL;
	PB_HEADER	*pPBHeader = NULL;
	NODEHANDLE	nodeHandle = 0;
	uint32		speedCode = 0;

#ifdef _AUTO_REQ_HANDLE
	lalGetHandleFromNodeAddr(nodeAddr, &nodeHandle);
#endif
	//SYS_TRACE1(SYSDEBUG_TRACE_TESTS,qData[2]);
	// get a packet block based on size
	hResult = pbCreatePacket (&packetBlock, (NODEHANDLE) nodeHandle, (PB_CALLBACK)NULL, (uint32) (payloadNumQuads*4), PB_CREATE_OHCI_ARDMA);
	if ((hResult != NO_ERROR) || (packetBlock == NULL))
	{
		//ML: TODO need to log this error
		return hResult;
	}
	// get packet header pointer
	pbGetPacketHeader (packetBlock, &pPBHeader);
	memcpy (pPBHeader->quadlets,qData,4*headerNumQuads);
	qData +=headerNumQuads;

	// set the packet type
	pbSetPacketType (packetBlock, (PB_PACKETTYPE) tCode);

	// get packet payload ptr
	pbGetPayload (packetBlock, (void **) &pPBPayload);

	// set the packet payload size in bytes
	pbSetPayloadSize (packetBlock, (uint32) (payloadNumQuads*4));

	// copy payload into packet
	fastCopyQuads(pPBPayload, qData, payloadNumQuads);
	// the next quadlet in the ohci buffer should be a status trailer quadlet
	// Check that for errors
	
	hResult = llcCheckTrailer(qData[payloadNumQuads],&speedCode);
	if (hResult != NO_ERROR) 
	{
		//illegal packet, we would not know where to send it
		packetBlock->creatorCBProc = NULL;
		pbPacketDone(packetBlock, PB_DONE_OHCI_ARDMA);
		return hResult;		
	}

	pbSetPacketSpeed(packetBlock, (speedCode << SHIFT_SPEEDCODE));
	{
		hResult = lhlARxGetReqPacket(tLabel, tCode, nodeAddr, packetBlock);
	}
	return hResult;
}

static HRESULT llcProcessResponse (QUADLET *qData, uint16 headerNumQuads, uint16 payloadNumQuads, uint32 tLabel, uint16 tCode, uint32 nodeAddr)
{
	HRESULT		hResult = NO_ERROR;
	QUADLET		*pPBPayload = NULL;
	PB			*packetBlock = NULL;
	PB_HEADER	*pPBHeader = NULL;
	uint32		speedCode = 0;

#ifdef _AUTO_REQ_HANDLE
		//ML: This does not make sense.
		//lalGetHandleFromNodeAddr(nodeAddr, &nodeHandle);
#endif
					
	// if found associated request and registered callback
	hResult = lhlARxHandleRspPacketFindRequest(tLabel, tCode, nodeAddr, &packetBlock);
					
	if (hResult != NO_ERROR)
	{
		//ML: TODO need to log this error
		return hResult;
	}
	if (packetBlock == NULL)
	{
		//no action required, packet was released as there were no callback
		return hResult;
	}
	// get packet header pointer
	pbGetPacketHeader (packetBlock, &pPBHeader);
	memcpy (pPBHeader->quadlets,qData,4*headerNumQuads);
	qData +=headerNumQuads;

	// set the packet type
	pbSetPacketType (packetBlock, (PB_PACKETTYPE) tCode);

	// get packet payload ptr
	pbGetPayload (packetBlock, (void **) &pPBPayload);

	// set the packet payload size in bytes
	pbSetPayloadSize (packetBlock, (uint32) (payloadNumQuads*4));

	// copy payload into packet
	fastCopyQuads(pPBPayload, qData, payloadNumQuads);

	// the next quadlet in the ohci buffer should be a status trailer quadlet
	// Check that for errors
	
	hResult = llcCheckTrailer(qData[payloadNumQuads],&speedCode);
	if (hResult != NO_ERROR) 
	{
		//we will notify the matching request packet of the error
		pbSetFinalStatus(packetBlock, hResult);
		pbPacketDone(packetBlock, PB_DONE_OHCI_ARDMA);
		return hResult;		
	}

	pbSetPacketSpeed(packetBlock, (speedCode << SHIFT_SPEEDCODE));
	
	// the packet creator callback will receive this response
	pbPacketDone(packetBlock, PB_DONE_LHL_ARX_RSP_2);

	return hResult;
}

#ifdef _LOOSE_ISO

static HRESULT llcProcessStream (QUADLET *qData, uint16 headerNumQuads, uint16 payloadNumQuads, uint32 tLabel, uint16 tCode, uint32 nodeAddr)
{
	HRESULT		hResult = NO_ERROR;
	QUADLET		*pPBPayload = NULL;
	PB			*packetBlock = NULL;
	PB_HEADER	*pPBHeader = NULL;
	NODEHANDLE	nodeHandle = 0;
	uint32		speedCode = 0;
					
	// get a packet block based on size
	hResult = pbCreatePacket (&packetBlock, (NODEHANDLE) nodeHandle, (PB_CALLBACK)NULL, (uint32) (payloadNumQuads*4), PB_CREATE_OHCI_ARDMA);
					
	if ((hResult != NO_ERROR) || (packetBlock == NULL))
	{
		//ML: TODO need to log this error
		return hResult;
	}

	// get packet header pointer
	pbGetPacketHeader (packetBlock, &pPBHeader);
	memcpy (pPBHeader->quadlets,qData,4*headerNumQuads);
	qData +=headerNumQuads;

	// set the packet type
	pbSetPacketType (packetBlock, (PB_PACKETTYPE) tCode);

	// get packet payload ptr
	pbGetPayload (packetBlock, (void **) &pPBPayload);

	// set the packet payload size in bytes
	pbSetPayloadSize (packetBlock, (uint32) (payloadNumQuads*4));

	// copy payload into packet
	fastCopyQuads(pPBPayload, qData, payloadNumQuads);

	// the next quadlet in the ohci buffer should be a status trailer quadlet
	// Check that for errors
	
	hResult = llcCheckTrailer(qData[payloadNumQuads],&speedCode);
	if (hResult != NO_ERROR) 
	{
		packetBlock->creatorCBProc = NULL;
		return hResult;		
	}

	pbSetPacketSpeed(packetBlock, (speedCode << SHIFT_SPEEDCODE));

	hResult = lhlARxGetStreamPacket(tLabel, tCode, nodeAddr, packetBlock);
	return hResult;
}
#endif //_LOOSE_ISO



static HRESULT llcProcessPhy(QUADLET *qHeader)
{
	HRESULT		hResult = NO_ERROR;

	//we just discard them now, should be an installable callback for this

	return hResult;
}




#define TTYPE_FLAG_RESP					0x00000001	//packet is a response, otherwise it is a request
#define TTYPE_FLAG_PAYLOAD			0x00000002  //packet has payload
#define TTYPE_FLAG_EMBEDPAYLOAD	0x00000004	//packet has one quadlet embedded payload

typedef struct
{
	uint32  flags;		//is this a response
	uint32	nHeader;	//length of header (from hw), if 0 unsupported
	uint32	iPayload;	//index of payload size
	uint32  reserved;
} TCODE_INFO_DESC;

static TCODE_INFO_DESC tcodeRxInfo[16] =
{
	{TTYPE_FLAG_PAYLOAD | TTYPE_FLAG_EMBEDPAYLOAD										,3,0,0},	//TCODE_WRQUAD			Includes one data quadlet
	{TTYPE_FLAG_PAYLOAD                          										,4,3,0},	//TCODE_WRBLOCK			Length in h[3]>>16
	{TTYPE_FLAG_RESP                             										,3,0,0},	//TCODE_WRRESPONSE	No further data
	{0                                           										,0,0,0},	//TCODE_RESERVED_1	
	{0																															,3,0,0},	//TCODE_RDQUAD			No further data
	{0																															,4,0,0},	//TCODE_RDBLOCK			No Further data
	{TTYPE_FLAG_RESP | TTYPE_FLAG_PAYLOAD | TTYPE_FLAG_EMBEDPAYLOAD	,3,0,0},	//TCODE_RDQUADRSP		Includes one data quadlet
	{TTYPE_FLAG_RESP | TTYPE_FLAG_PAYLOAD                           ,4,3,0},	//TCODE_RDBLOCKRSP	Length in h[3]>>16
	{0                                                              ,0,0,0},	//TCODE_CYCLESTART	
	{TTYPE_FLAG_PAYLOAD                                             ,4,3,0},	//TCODE_LOCKRQST		Length in h[3]>>16
	{TTYPE_FLAG_PAYLOAD                                             ,1,0,0},	//TCODE_ISOBLOCK		Length in h[0]>>16
	{TTYPE_FLAG_RESP | TTYPE_FLAG_PAYLOAD                           ,4,3,0},	//TCODE_LOCKRSP			Length in h[3]>>16
	{0                                                              ,0,0,0},	//TCODE_RESERVED_2	
	{0                                                              ,0,0,0},	//TCODE_RESERVED_3	
	{0                                                              ,3,0,0},	//TCODE_LLC_SPECIFIC	Always 3 for this LLC
	{0                                                              ,0,0,0}		//TCODE_RESERVED_4	
};


//*******************************************************************************
/*
 Read FIFO buffer and creates a new packetBlock for first valid packet found in FIFO buffer 
 pointed by qPacket. FIFO buffer must contain exactly one packet.  
 If any error occures the FIFO buffer is discarded.

	called when the rx thread is notified of a packet in the AR request or response FIFO buffer
		this function gets packets from buffer, creates a packet block when necessary, and
		dispatches it to the appropriate module, based on previous address allocations and callback.
		Also, when the packet is a confirmation to a previous request, the appropriate callback is 
		executed.
*/
HRESULT llcProcessRxPacket(QUADLET *pPacket, uint32 size)
{
	HRESULT		hResult = NO_ERROR;
	TCODE_INFO_DESC * pTcodeInfo;
	
	uint32		tLabel = 0;
	uint16		tCode = 0;
	uint32		nodeAddr = 0;
	uint16		payloadNumBytes = 0;
	uint16		payloadNumQuads = 0;
	
	//At this point pPacket points to exactly one packet
	// Read data package header info
	//SYS_TRACE1(SYSDEBUG_TRACE_TESTS,0);
	tCode = (uint16)((pPacket[0]&MASK_TCODE)>>SHIFT_TCODE);
	pTcodeInfo = &tcodeRxInfo[tCode];
	// Verify valid tCode
	if (!pTcodeInfo->nHeader)
	{
		// Packet error, unsupported packet
		hResult = E_LHL_UNSUPPORTED_TCODE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
#ifndef _LOOSE_ISO
	if (tCode == TCODE_ISOBLOCK)
	{
		// Packet error, unsupported packet
		hResult = E_LHL_UNSUPPORTED_TCODE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
#else //_LOOSE_ISO
	if (tCode == TCODE_ISOBLOCK)
	{		
		if (lalAsyncStreamIsEnabled())
		{
			uint8			channel; 
			uint8			tag;
			uint8			sy;
			LAL_CALLBACK	callback;

			hResult = lhlLLCGetAsyncStreamHeader(&pPacket[0], &channel, &tag, &sy);
			if (hResult != NO_ERROR) return hResult;
			tLabel = channel;
			tLabel = tLabel << SHIFT_TLABEL;
			nodeAddr = LHL_ASYNC_DUMMY_NODE_ADDR;
			// lookup channel in dispatch table
			if (!lalAsyncStreamLookupChannelCB(channel, &callback))
			{
				return hResult;
			}	
		}
		else
			return hResult;
	}
#endif //_LOOSE_ISO	
	else
	{
		tLabel = (pPacket[0] & MASK_TLABEL);
		nodeAddr = (pPacket[1] & MASK_NODEADDR) >> SHIFT_NODEADDR;
	}

	// get payload size based on tcode
	if (pTcodeInfo->flags & TTYPE_FLAG_PAYLOAD)
	{
		if (pTcodeInfo->flags & TTYPE_FLAG_EMBEDPAYLOAD)
			payloadNumBytes = 4;
		else
			payloadNumBytes = pPacket[pTcodeInfo->iPayload]>>16;
	}
	else
	{
		payloadNumBytes = 0;
	}		
	if (payloadNumBytes > LINK_MAXRXPAYLOAD_SIZE_BYTES)
	{
		hResult = E_PB_PAYLOAD_TOO_LARGE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// Round payload size up to even number of quadlets
	payloadNumQuads = (uint16) (payloadNumBytes +3)/ 4;

	if (tCode == TCODE_PHY_PACKET) 
	{
		hResult = llcProcessPhy(pPacket);
	}
#ifdef _LOOSE_ISO
	else if (tCode == TCODE_ISOBLOCK)
	{
		hResult = llcProcessStream (pPacket, pTcodeInfo->nHeader, payloadNumQuads, tLabel, tCode, nodeAddr);
	}
#endif
	else
	{
		if (pTcodeInfo->flags & TTYPE_FLAG_RESP)
			hResult = llcProcessResponse (pPacket, pTcodeInfo->nHeader, payloadNumQuads, tLabel, tCode, nodeAddr);
		else
			hResult = llcProcessRequest (pPacket, pTcodeInfo->nHeader, payloadNumQuads, tLabel, tCode, nodeAddr);
	}
	return hResult;
}

