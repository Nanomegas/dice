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
 * DESCRIPTION: 
	Purpose:	Implementing Semaphore Platform Generic layer
	
	Revisions:
		created 09/18/2002 lm

****************************************************************************/

#ifndef _TC_MSGQUEUE_H_
#define _TC_MSGQUEUE_H_

#include "TCTypes.h"

/** @addtogroup RTOSQueue
	Provides RTOS message queues.
	@{
*/

/** Possible sizes of the queue elements.
*/
enum
{
	SIZE_1_ULONG	= 0x01,
	SIZE_2_ULONG	= 0x02,
	SIZE_4_ULONG	= 0x04,
	SIZE_8_ULONG	= 0x08,
	SIZE_16_ULONG	= 0x10
}; //TC_MSGQUEUE_ELEM_SIZE;

/** Create a new message queue.
	@param queueID			The address to place the new queue ID in.
	@param elemSizeQuads	 The size of the queue elements in quadlets.
	@param queueStart		The start of the memory to use for the queue.
	@param queueSize		The size of the queue memory.
	@param bUseSema			Use a semaphore to make the queue thread safe.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		TCMsgQueueOpen (uint32 *queueID, uint32 elemSizeQuads, void *queueStart, uint32 queueSize, BOOL bUseSema);

/** Close a message queue.
	@param queueID			The ID of the queue to close.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		TCMsgQueueClose (uint32 queueID);

/** Reset a message queue.
	@param queueID			The ID of the queue to close.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		TCMsgQueueReset (uint32 queueID);

/** Obtain the number of items in a queue.
	@param queueID			The ID of the queue to use.
	@param items			The address to place the number of items in.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		TCMsgQueueGetNoOfItems (uint32 queueID, uint32 *items);

/** Send data to a queue.
	@param queueID			The ID of the queue to use.
	@param pSrc				The address of the data to send.
	@param msec				TC_WAIT_FOREVER, TC_NO_WAIT or timeout time in milliseconds.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		TCMsgQueueSend (uint32 queueID, void *pSrc, uint32 msec);

/** Receive from a queue.
	@param queueID			The ID of the queue to use.
	@param pDst				The address to copy the queue message into.
	@param msec				TC_WAIT_FOREVER, TC_NO_WAIT or timeout time in milliseconds.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		TCMsgQueueReceive (uint32 queueID, void *pDst, uint32 msec);

//! @}
#endif
