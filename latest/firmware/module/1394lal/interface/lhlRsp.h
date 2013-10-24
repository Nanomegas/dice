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

#ifndef _LHLRSP_H
#define _LHLRSP_H

#include "TCTypes.h"
#include "1394.h"
#include "packetBlock.h"
#include "bmdefs.h"
#ifdef _WINMP
#pragma warning( disable : 4514 )
#endif //_WINMP

/****************************************************************************/
// private

// lhlRsp1394 helper function for handling header data
HRESULT	lhlRsp1394HandleHeader(PB* packetBlock,
						RCODE_1394 responseCode,
						uint32 *nodeAddr,
						uint32 *tLabel,
						uint16 numBytes,
						uint16 reqType);

// lhlRsp1394 helper function for handling payload data
HRESULT lhlRsp1394HandlePayload(PB* packetBlock,
						RCODE_1394 responseCode,
						uint16 numBytes,
						QUADLET *data,
						uint16 reqType);

// lhlRsp1394 helper function for handling payload data size
HRESULT lhlRsp1394GetNewPayloadSize(PB* packetBlock,
						uint16 *payloadBytes,
						uint16 reqType);

// this function handles the actual response
HRESULT lhlRsp1394New(PB *packetBlock,
						RCODE_1394 responseCode,
						uint16 numBytes,
						uint32* data,
						uint16 rspType, 
						BOOL bNonBlocking, 
						uint32 msec);

HRESULT lhlRsp1394(PB *packetBlock,
						RCODE_1394 responseCode,
						uint16 numBytes,
						uint32* data,
						uint16 rspType, 
						uint32 msec);


/*****************************************************************************

	lhlSendResponse functions

	these function is used to send responses for request packets

	lhlReplyResponse: packetBlock is the original packet and this packetBlock is not being freed
	lhlSendResponse: packetBlock is the response packet
*****************************************************************************/
_TC_INLINE HRESULT lhlReplyReadResponse(PB *packetBlock, RCODE_1394 responseCode, uint16 numBytes, uint32* data, BOOL bNonBlocking) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT lhlReplyReadResponse(PB *packetBlock, RCODE_1394 responseCode, uint16 numBytes, uint32* data, BOOL bNonBlocking)
{
	// send response for read request packets
	return lhlRsp1394New(packetBlock, responseCode, numBytes, data, LHL_RSP_READ, bNonBlocking, LHL_TX_RSP_TIMEOUT_MSECS);
}

_TC_INLINE HRESULT lhlReplyWriteResponse(PB *packetBlock, RCODE_1394 responseCode, BOOL bNonBlocking) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT lhlReplyWriteResponse(PB *packetBlock, RCODE_1394 responseCode, BOOL bNonBlocking)
{
	// send response for write request packets
	return lhlRsp1394New(packetBlock, responseCode, 0, NULL, LHL_RSP_WRITE, bNonBlocking, LHL_TX_RSP_TIMEOUT_MSECS);
}

_TC_INLINE HRESULT lhlReplyLockResponse(PB *packetBlock, RCODE_1394 responseCode, uint16 numBytes, uint32* data, BOOL bNonBlocking) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT lhlReplyLockResponse(PB *packetBlock, RCODE_1394 responseCode, uint16 numBytes, uint32* data, BOOL bNonBlocking)
{
	// send response for lock request packets
	return lhlRsp1394New(packetBlock, responseCode, numBytes, data, LHL_RSP_LOCK, bNonBlocking, LHL_TX_RSP_TIMEOUT_MSECS);
}

_TC_INLINE HRESULT lhlReplyErrorResponse(PB *packetBlock, RCODE_1394 responseCode, BOOL bNonBlocking) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT lhlReplyErrorResponse(PB *packetBlock, RCODE_1394 responseCode, BOOL bNonBlocking)
{
	// send error response for a request packet
	return lhlRsp1394New(packetBlock, responseCode, 0, NULL, LHL_RSP_ERROR, bNonBlocking, LHL_TX_RSP_TIMEOUT_MSECS);
}

_TC_INLINE HRESULT lhlSendReadResponse(PB *packetBlock, RCODE_1394 responseCode, uint16 numBytes, uint32* data) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT lhlSendReadResponse(PB *packetBlock, RCODE_1394 responseCode, uint16 numBytes, uint32* data)
{
	// send response for read request packets
	return lhlRsp1394(packetBlock, responseCode, numBytes, data, LHL_RSP_READ, LHL_TX_RSP_TIMEOUT_MSECS);
}

_TC_INLINE HRESULT lhlSendWriteResponse(PB *packetBlock, RCODE_1394 responseCode) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT lhlSendWriteResponse(PB *packetBlock, RCODE_1394 responseCode)
{
	// send response for write request packets
	return lhlRsp1394(packetBlock, responseCode, 0, NULL, LHL_RSP_WRITE, LHL_TX_RSP_TIMEOUT_MSECS);
}

_TC_INLINE HRESULT lhlSendLockResponse(PB *packetBlock, RCODE_1394 responseCode, uint16 numBytes, uint32* data) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT lhlSendLockResponse(PB *packetBlock, RCODE_1394 responseCode, uint16 numBytes, uint32* data)
{
	// send response for lock request packets
	return lhlRsp1394(packetBlock, responseCode, numBytes, data, LHL_RSP_LOCK, LHL_TX_RSP_TIMEOUT_MSECS);
}

_TC_INLINE HRESULT lhlSendErrorResponse(PB *packetBlock, RCODE_1394 responseCode) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT lhlSendErrorResponse(PB *packetBlock, RCODE_1394 responseCode)
{
	// send error response for a request packet
	return lhlRsp1394(packetBlock, responseCode, 0, NULL, LHL_RSP_ERROR, LHL_TX_RSP_TIMEOUT_MSECS);
}


#endif //_LHLRSP_H
