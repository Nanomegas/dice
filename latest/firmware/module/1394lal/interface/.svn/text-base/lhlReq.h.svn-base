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

#ifndef _LHLREQ_H
#define _LHLREQ_H

#include "1394.h"
#include "packetBlock.h"
#include "TCTypes.h"
#include "bmdefs.h"
#include "lalDefs.h"
#include "lhlDefs.h"
#ifdef _WINMP
#pragma warning( disable : 4514 )
#endif //_WINMP

/****************************************************************************/
// private

// handle local node requests
HRESULT lhlReqLocalNode(OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 reqType);

// lhlReq1394 helper function for handling header data
HRESULT	lhlReq1394HandleHeader(PB* packetBlock, uint32	nodeAddr, OFFSET_1394 destOffset, uint32 tLabel, uint16 numBytes, uint16 reqType, PB_LOCKTYPE lockType);

// lhlReq1394 helper function for handling payload data
HRESULT lhlReq1394HandlePayload(PB* packetBlock, uint16 numBytes, QUADLET *data, uint16 reqType);

HRESULT lhlReq1394New(	LAL_NODE_HANDLE handle,
						uint32 nodeAddr,
						OFFSET_1394 destOffset,
						uint16 numBytes,
						QUADLET *data,
						uint16 queue,
						uint16 reqType,
						PB_LOCKTYPE lockType, 
						BOOL bNonBlocking, 
						uint32 msec);

// this function handles the actual request
HRESULT	lhlReq1394(	PB* packetBlock,
					uint32 nodeAddr,
					OFFSET_1394 destOffset,
					uint16 numBytes,
					QUADLET *data,
					uint16 queue,
					uint16 reqType,
					PB_LOCKTYPE lockType, 
					uint32 msec);

/*****************************************************************************

	Blocking and Non-Blocking Read,Write,Lock functions

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

#ifdef _LOOSE_ISO
HRESULT lhlAsyncStreamSendData(uint8 channel, uint8 tag, uint8 sy, uint16 numBytes, QUADLET *data, uint16 queue);
#endif

HRESULT lhlReqHandle(LAL_NODE_HANDLE handle, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue, uint16 reqType, PB_LOCKTYPE lockType, BOOL bNonBlocking, uint32 msec);

_TC_INLINE HRESULT lhlReadHandle(LAL_NODE_HANDLE handle, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT lhlReadHandle(LAL_NODE_HANDLE handle, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue)
{
	return lhlReqHandle(handle, destOffset, numBytes, data, queue, LHL_REQ_READ, PB_LOCKTYPE_NONE, FALSE, LHL_TX_REQ_TIMEOUT_MSECS);
}

_TC_INLINE HRESULT lhlWriteHandle(LAL_NODE_HANDLE handle, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT lhlWriteHandle(LAL_NODE_HANDLE handle, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue)
{
	return lhlReqHandle(handle, destOffset, numBytes, data, queue, LHL_REQ_WRITE, PB_LOCKTYPE_NONE, FALSE, LHL_TX_REQ_TIMEOUT_MSECS);
}

_TC_INLINE HRESULT lhlLockHandle(LAL_NODE_HANDLE handle, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue, PB_LOCKTYPE lockType) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT lhlLockHandle(LAL_NODE_HANDLE handle, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue, PB_LOCKTYPE lockType)
{
	return lhlReqHandle(handle, destOffset, numBytes, data, queue, LHL_REQ_LOCK, lockType, FALSE, LHL_TX_REQ_TIMEOUT_MSECS);
}

_TC_INLINE HRESULT lhlReqNode(uint32 nodeAddr, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue, uint16 reqType, PB_LOCKTYPE lockType, BOOL bNonBlocking, uint32 msec) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT lhlReqNode(uint32 nodeAddr, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue, uint16 reqType, PB_LOCKTYPE lockType, BOOL bNonBlocking, uint32 msec)
{
	return lhlReq1394New(nodeAddr, nodeAddr, destOffset, numBytes, data, queue, reqType, lockType, bNonBlocking, msec);
}

_TC_INLINE HRESULT lhlReadNode(uint32 nodeAddr, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT lhlReadNode(uint32 nodeAddr, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue)
{
	// nodeAddr is the handle here
	return lhlReqNode(nodeAddr, destOffset, numBytes, data, queue, LHL_REQ_READ, PB_LOCKTYPE_NONE, FALSE, LHL_TX_REQ_TIMEOUT_MSECS);
}

_TC_INLINE HRESULT lhlWriteNode(uint32 nodeAddr, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT lhlWriteNode(uint32 nodeAddr, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue)
{
	// nodeAddr is the handle here
	return lhlReqNode(nodeAddr, destOffset, numBytes, data, queue, LHL_REQ_WRITE, PB_LOCKTYPE_NONE, FALSE, LHL_TX_REQ_TIMEOUT_MSECS);
}

_TC_INLINE HRESULT lhlLockNode(uint32 nodeAddr, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue, PB_LOCKTYPE lockType) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT lhlLockNode(uint32 nodeAddr, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue, PB_LOCKTYPE lockType)
{
	return lhlReqNode(nodeAddr, destOffset, numBytes, data, queue, LHL_REQ_LOCK, lockType, FALSE, LHL_TX_REQ_TIMEOUT_MSECS);
}

_TC_INLINE HRESULT lhlReadNodeTimeout(uint32 nodeAddr, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue, uint32 msec) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT lhlReadNodeTimeout(uint32 nodeAddr, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue, uint32 msec)
{
	// nodeAddr is the handle here
	return lhlReqNode(nodeAddr, destOffset, numBytes, data, queue, LHL_REQ_READ, PB_LOCKTYPE_NONE, FALSE, msec);
}

_TC_INLINE HRESULT lhlWriteNodeTimeout(uint32 nodeAddr, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue, uint32 msec) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT lhlWriteNodeTimeout(uint32 nodeAddr, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue, uint32 msec)
{
	// nodeAddr is the handle here
	return lhlReqNode(nodeAddr, destOffset, numBytes, data, queue, LHL_REQ_WRITE, PB_LOCKTYPE_NONE, FALSE, msec);
}

_TC_INLINE HRESULT lhlLockNodeTimeout(uint32 nodeAddr, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue, PB_LOCKTYPE lockType, uint32 msec) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT lhlLockNodeTimeout(uint32 nodeAddr, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 queue, PB_LOCKTYPE lockType, uint32 msec)
{
	return lhlReqNode(nodeAddr, destOffset, numBytes, data, queue, LHL_REQ_LOCK, lockType, FALSE, msec);
}




#endif //_LHLREQ_H
