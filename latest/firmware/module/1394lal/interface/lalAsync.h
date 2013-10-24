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
 * DESCRIPTION: Link Abstraction Layer Interfaces for blocking 1394 i/o

	Purpose:	Link Abstraction Layer 
				Interfaces for blocking 1394 i/o
	
	Revisions:
		created 04/22/2002 lm
****************************************************************************/

/** @addtogroup LinkAbstractionLayer
	@{
*/

/** @defgroup LALAsync Asynchronous Transaction Interface
	This modules provides the interface for blocking asynchronous transactions. This
	is the main interface used by higher level protocols.

	<H2>
	Interfaces for Blocking 1394 i/o Read,Write,Lock functions
	</H2>

	Using node addresses directly:
	lalReadNode()	performs a read transaction on the specified node
	lalWriteNode()	performs a write transaction on the specified node
	lalLockNode()	performs a lock transaction on the specified node

	Using handles mapped to the corresponding node address:
	@sa NodeInterfaceHandles

	lalRead1394()	performs a read transaction on the specified node
	lalWrite1394()	performs a write transaction on the specified node
	lalLock1394()	performs a lock transaction on the specified node

	<H2>
	Interfaces for sending responses
	</H2>

	lalReplyReadResponse()	sends a response to a read request
	lalReplyWriteResponse()	sends a response to a write request
	lalReplyLockResponse()	sends a response to a lock request
	lalReplyErrorResponse()	sends an error response

	<H2>
	Interfaces for blocking i/o
	</H2>
	lalSendWriteRequest()	sends a blocking write request


	@{
*/



#ifndef _LALASYNC_H
#define _LALASYNC_H

#include "TCTypes.h"
#include "1394.h"
#include "1394defs.h"
#include "packetBlock.h"
#include "lalDefs.h"
#include "lhlReq.h"
#include "lhlRsp.h"
#include "lhlMsgQueue.h"
#include "lhlStatistics.h"
#include "nciHandles.h"

#ifdef _WINMP
#pragma warning( disable : 4514 )
#endif //_WINMP


/** Performs a blocking read transaction on the specified node.
	@param nodeAddr		The node address of the target node.
	@param destOffset	The address to read on the target node. @sa OFFSET_1394
	@param numBytes		The number of bytes to read.
	@param data			The memory location to store the read data.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	lalReadNode(uint32 nodeAddr, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalReadNode(uint32 nodeAddr, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data)
								{
									return lhlReqNode(nodeAddr, destOffset, numBytes, data, LHL_QUEUE_GENERAL, LHL_REQ_READ, PB_LOCKTYPE_NONE, FALSE, LHL_TX_REQ_TIMEOUT_MSECS);
								}

/** Performs a blocking write transaction on the specified node.
	@param nodeAddr		The node address of the target node.
	@param destOffset	The address to write to on the target node. @sa OFFSET_1394
	@param numBytes		The number of bytes to write.
	@param data			The memory location containing the data to write.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	lalWriteNode(uint32 nodeAddr, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalWriteNode(uint32 nodeAddr, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data)
								{
									return lhlReqNode(nodeAddr, destOffset, numBytes, data, LHL_QUEUE_GENERAL, LHL_REQ_WRITE, PB_LOCKTYPE_NONE, FALSE, LHL_TX_REQ_TIMEOUT_MSECS);
								}

/** Performs a blocking lock transaction on the specified node.
	@param nodeAddr		The node address of the target node.
	@param destOffset	The address to perform the lock on the target node.
	@param numBytes		The number of bytes to write.
	@param data			The memory location containing the data to write.
	@param lockType		The type of lock. The most common is PB_LOCKTYPE_COMPARE_SWAP.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	lalLockNode(uint32 nodeAddr, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, PB_LOCKTYPE lockType) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalLockNode(uint32 nodeAddr, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, PB_LOCKTYPE lockType)
								{
									return lhlReqNode(nodeAddr, destOffset, numBytes, data, LHL_QUEUE_GENERAL, LHL_REQ_LOCK, lockType, FALSE, LHL_TX_REQ_TIMEOUT_MSECS);
								}

/** Performs a blocking read transaction on the specified node.
	@param handle		The handle for the target node.
	@param destOffset	The address to read on the target node. 
	@param numBytes		The number of bytes to read.
	@param data			The memory location to store the read data.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	lalRead1394(LAL_NODE_HANDLE handle, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalRead1394(LAL_NODE_HANDLE handle, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data)
								{
									return lhlReqHandle(handle, destOffset, numBytes, data, LHL_QUEUE_GENERAL, LHL_REQ_READ, PB_LOCKTYPE_NONE, FALSE, LHL_TX_REQ_TIMEOUT_MSECS);
								}

/** Performs a blocking write transaction on the specified node.
	@param handle		The handle for the target node.
	@param destOffset	The address to write to on the target node. 
	@param numBytes		The number of bytes to write.
	@param data			The memory location containing the data to write.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	lalWrite1394(LAL_NODE_HANDLE handle, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalWrite1394(LAL_NODE_HANDLE handle, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data)
								{
									return lhlReqHandle(handle, destOffset, numBytes, data, LHL_QUEUE_GENERAL, LHL_REQ_WRITE, PB_LOCKTYPE_NONE, FALSE, LHL_TX_REQ_TIMEOUT_MSECS);
								}

/** Performs a blocking lock transaction on the specified node.
	@param handle		The handle for the target node.
	@param destOffset	The address to perform the lock on the target node.
	@param numBytes		The number of bytes to write.
	@param data			The memory location containing the data to write.
	@param lockType		The type of lock. @sa PB_LOCKTYPE.  The most common is PB_LOCKTYPE_COMPARE_SWAP.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	lalLock1394(LAL_NODE_HANDLE handle, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, PB_LOCKTYPE lockType) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalLock1394(LAL_NODE_HANDLE handle, OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, PB_LOCKTYPE lockType)
								{
									return lhlReqHandle(handle, destOffset, numBytes, data, LHL_QUEUE_GENERAL, LHL_REQ_LOCK, lockType, FALSE, LHL_TX_REQ_TIMEOUT_MSECS);
								}

/** Sends a response to a read request.
	@param packetBlock	The packet block for which a response is being sent.
	@param responseCode	The IEEE 1394 response code.
	@param numBytes		The number of bytes in the response.
	@param data			The address of the data to be sent.
	@param bNonBlocking	TRUE to send non-blocking (function will return immediately and response will be queued).
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	lalReplyReadResponse(PB *packetBlock, RCODE_1394 responseCode, uint16 numBytes, uint32* data, BOOL bNonBlocking) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalReplyReadResponse(PB *packetBlock, RCODE_1394 responseCode, uint16 numBytes, uint32* data, BOOL bNonBlocking)
								{
									return lhlReplyReadResponse(packetBlock, responseCode, numBytes, data, bNonBlocking);
								}

/** Sends a response to a write request.
	@param packetBlock	The packet block for which a response is being sent.
	@param responseCode	The IEEE 1394 response code.
	@param bNonBlocking	TRUE to send non-blocking (function will return immediately and response will be queued).
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	lalReplyWriteResponse(PB *packetBlock, RCODE_1394 responseCode, BOOL bNonBlocking) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalReplyWriteResponse(PB *packetBlock, RCODE_1394 responseCode, BOOL bNonBlocking)
								{
									return lhlReplyWriteResponse(packetBlock, responseCode, bNonBlocking);
								}

/** Sends a response to a lock request.
	@param packetBlock	The packet block for which a response is being sent.
	@param responseCode	The IEEE 1394 response code.
	@param numBytes		The number of bytes in the response.
	@param data			The address of the data to be sent.
	@param bNonBlocking	TRUE to send non-blocking (function will return immediately and response will be queued).
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	lalReplyLockResponse(PB *packetBlock, RCODE_1394 responseCode, uint16 numBytes, uint32* data, BOOL bNonBlocking) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalReplyLockResponse(PB *packetBlock, RCODE_1394 responseCode, uint16 numBytes, uint32* data, BOOL bNonBlocking)
								{
									return lhlReplyLockResponse(packetBlock, responseCode, numBytes, data, bNonBlocking);
								}

/** Sends an error response to a transaction request.
	@param packetBlock	The packet block for which a response is being sent.
	@param responseCode	The IEEE 1394 response code.
	@param bNonBlocking	TRUE to send non-blocking (function will return immediately and response will be queued).
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	lalReplyErrorResponse(PB *packetBlock, RCODE_1394 responseCode, BOOL bNonBlocking) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalReplyErrorResponse(PB *packetBlock, RCODE_1394 responseCode, BOOL bNonBlocking)
								{
									return lhlReplyErrorResponse(packetBlock, responseCode, bNonBlocking);
								}
/** Sends a write request. 
	@param packetBlock	The packet block to send.
	@param destOffset	The address of the remote node to send the packet block to.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT				lalSendWriteRequest (PB* packetBlock, OFFSET_1394 destOffset);

//! @}
//! @}
#endif
