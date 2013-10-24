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
 * DESCRIPTION: Link Hardware Abstraction Layer Message Queue

	Purpose:	Message Queue for handling transfer of packetblocks
				between different tasks
	
	Revisions:
		created 04/08/2002 lm

****************************************************************************/

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "lhl.h"
#include "lhlATx.h"
#include "lhlMsgQueue.h"
#include "lhlStatistics.h"

#if 0 //LM???

#define MSGQUEUE_ELEMENT_ITEMS			64	// Maximum number of MSGQUEUE_ELEMENT

static PB*		msgQueue[LHL_QUEUE_ITEMS][MSGQUEUE_ELEMENT_ITEMS];

static uint32	msgQueueID[LHL_QUEUE_ITEMS];

HRESULT lhlMsgQueueInitialize(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i;

#ifdef _STATISTICS
	lhlStatistics.msgQueueInuse = 0;
	lhlStatistics.msgQueueInuseMax = 0;
#endif //_STATISTICS

	for (i = 0; i < LHL_QUEUE_ITEMS; i++)
	{
		hResult = TCMsgQueueOpen (&(msgQueueID[i]), sizeof (PB *) / sizeof (QUADLET), (void *) &(msgQueue[i]), sizeof(msgQueue[i]), FALSE);
		if (hResult != NO_ERROR) return hResult;
	}

	return hResult;
}

HRESULT lhlMsgQueueGetPacketBlock(uint16 queue, PB **packetBlock, uint32 msec)
{
	HRESULT		hResult = NO_ERROR;

	*packetBlock = NULL;

	if (queue >= LHL_QUEUE_ITEMS)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	hResult = TCMsgQueueReceive (msgQueueID[queue], (void *) packetBlock, msec);
	if (hResult != NO_ERROR)
	{
		if ((hResult == E_MSGQUEUE_TIMEOUT || hResult == E_MSGQUEUE_EMPTY) && (msec == TC_NO_WAIT))
		{
			hResult = NO_ERROR;
		}
		return hResult;
	}

#ifdef _STATISTICS
	lhlStatistics.msgQueueInuse--;
#endif //_STATISTICS

	return hResult;
}

HRESULT lhlMsgQueuePutPacketBlock(uint16 queue, PB *packetBlock)
{
	HRESULT		hResult = NO_ERROR;

	if (queue >= LHL_QUEUE_ITEMS)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = TCMsgQueueSend (msgQueueID[queue], (void *) &packetBlock, TC_NO_WAIT);
	if (hResult != NO_ERROR) return hResult;

#ifdef _STATISTICS
	lhlStatistics.msgQueueInuse++;
	if (lhlStatistics.msgQueueInuseMax < lhlStatistics.msgQueueInuse)
	{
		lhlStatistics.msgQueueInuseMax = lhlStatistics.msgQueueInuse;
	}
#endif //_STATISTICS

	return hResult;
}

#else //LM???

#define MSGQUEUE_ELEMENT_ITEMS			64	// Maximum number of MSGQUEUE_ELEMENT

typedef struct _MSGQUEUE_ELEMENT
{
	PB							*packetBlock;
	uint32						index;
	struct _MSGQUEUE_ELEMENT	*next;
} MSGQUEUE_ELEMENT;

typedef struct 
{
	MSGQUEUE_ELEMENT	msg[MSGQUEUE_ELEMENT_ITEMS];
	BOOL				allocated[MSGQUEUE_ELEMENT_ITEMS];
	uint32				ptr;
} MSGQUEUE_ELEMENT_POOL;

static MSGQUEUE_ELEMENT_POOL	msgQueuePool;
static MSGQUEUE_ELEMENT			*msgQueuePriority;
static MSGQUEUE_ELEMENT			*msgQueueGeneral;

static uint32 lhlMsgQueueMutexSemID; // mutual exclusion semaphore for the msgQueue (mutex)

HRESULT lhlMsgQueueInitialize(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		index;

	// exclusive access for the msgQueue (mutex)
	hResult = TCMutexOpen(&lhlMsgQueueMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	for (index = 0; index < MSGQUEUE_ELEMENT_ITEMS; index++)
	{
		msgQueuePool.msg[index].index = index;
	}

#ifdef _STATISTICS
	lhlStatistics.msgQueueInuse = 0;
	lhlStatistics.msgQueueInuseMax = 0;
#endif //_STATISTICS

	return hResult;
}

static HRESULT lhlMsgQueueAllocateMsg(MSGQUEUE_ELEMENT **msg)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bFound = FALSE;
	uint32		index;
	uint32		ptr;

	ptr = msgQueuePool.ptr;
	
	for (index = 0; index < MSGQUEUE_ELEMENT_ITEMS; index++)
	{
		if (msgQueuePool.allocated[ptr] == FALSE)
		{
			*msg = &msgQueuePool.msg[ptr];
			msgQueuePool.allocated[ptr] = TRUE;
			bFound = TRUE;

#ifdef _STATISTICS
			lhlStatistics.msgQueueInuse++;
			if (lhlStatistics.msgQueueInuseMax < lhlStatistics.msgQueueInuse)
			{
				lhlStatistics.msgQueueInuseMax = lhlStatistics.msgQueueInuse;
			}
#endif //_STATISTICS

		}

		ptr = (ptr + 1) % MSGQUEUE_ELEMENT_ITEMS;
		
		if (bFound == TRUE)
		{
			msgQueuePool.ptr = ptr;
			break;
		}
	}

	if (bFound == FALSE)
	{
		hResult = E_QUEUE_FULL;	
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}
	
static HRESULT lhlMsgQueueFreeMsg(MSGQUEUE_ELEMENT *msg)
{
	HRESULT		hResult = NO_ERROR;

	if (msg == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (msg->index >= MSGQUEUE_ELEMENT_ITEMS ||
		msgQueuePool.allocated[msg->index] == FALSE)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	msgQueuePool.allocated[msg->index] = FALSE;
#ifdef _STATISTICS
	lhlStatistics.msgQueueInuse--;
#endif //_STATISTICS

	return hResult;
}
	
HRESULT lhlMsgQueueGetPacketBlock(uint16 queue, PB **packetBlock, uint32 msec)
{
	HRESULT		hResult = NO_ERROR;
	MSGQUEUE_ELEMENT **msgQueue;
	MSGQUEUE_ELEMENT *tmpQueue;

	UNUSED_ARG(msec);
	
	*packetBlock = NULL;

	// exclusive access for the msgQueue (mutex)
	hResult = TCMutexLock(lhlMsgQueueMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	msgQueue = (queue == LHL_QUEUE_PRIORITY) ? &msgQueuePriority : &msgQueueGeneral;

	if (*msgQueue)
	{
		tmpQueue = *msgQueue;
		*packetBlock = tmpQueue->packetBlock;
		*msgQueue = tmpQueue->next;
		lhlMsgQueueFreeMsg(tmpQueue);
	}

	// exclusive access for the msgQueue (mutex)
	TCMutexUnlock(lhlMsgQueueMutexSemID);

	return hResult;
}

HRESULT lhlMsgQueueCancelPacketBlock(uint16 queue, PB *packetBlock)
{
	HRESULT		hResult = NO_ERROR;
	MSGQUEUE_ELEMENT **msgQueue;
	MSGQUEUE_ELEMENT *tmpQueue;

		
	// exclusive access for the msgQueue (mutex)
	hResult = TCMutexLock(lhlMsgQueueMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	msgQueue = (queue == LHL_QUEUE_PRIORITY) ? &msgQueuePriority : &msgQueueGeneral;

	while (*msgQueue)
	{
		tmpQueue = *msgQueue;
		if (tmpQueue->packetBlock == packetBlock)
		{
		   *msgQueue = tmpQueue->next;
		   lhlMsgQueueFreeMsg(tmpQueue);
		   break; 		   
		}
		if (!tmpQueue->next) break;
		msgQueue = &(tmpQueue->next);
	}

	// exclusive access for the msgQueue (mutex)
	TCMutexUnlock(lhlMsgQueueMutexSemID);

	return hResult;
}

HRESULT lhlMsgQueuePutPacketBlock(uint16 queue, PB *packetBlock)
{
	HRESULT				hResult = NO_ERROR;
	MSGQUEUE_ELEMENT	**msgQueue = NULL;
	MSGQUEUE_ELEMENT	*tmpQueue = NULL;
	MSGQUEUE_ELEMENT	*msg = NULL;
	
	// exclusive access for the msgQueue (mutex)
	hResult = TCMutexLock(lhlMsgQueueMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlMsgQueueAllocateMsg(&msg);
	
	if (hResult == NO_ERROR) 
	{
		msgQueue = (queue == LHL_QUEUE_PRIORITY) ? &msgQueuePriority : &msgQueueGeneral;

		msg->packetBlock = packetBlock;
		msg->next = NULL;
		
		if (*msgQueue)
		{
			tmpQueue = (*msgQueue);
			while (tmpQueue->next) tmpQueue = tmpQueue->next;
			tmpQueue->next = msg;
		}
		else
		{
			*msgQueue = msg;
		}
	}

	// exclusive access for the msgQueue (mutex)
	TCMutexUnlock(lhlMsgQueueMutexSemID);

	return hResult;
}

#endif //LM???

HRESULT lhlMsgQueueEnQPacketBlock(uint16 queue, PB *packetBlock)
{
	HRESULT		hResult = NO_ERROR;

	hResult = lhlMsgQueuePutPacketBlock(queue, packetBlock);
	if (hResult != NO_ERROR) return hResult;

	lhlTxSndPendSignalSema();

	return hResult;
}

HRESULT lhlMsgQueueEnQPacketBlockWaitTimeout(uint16 queue, PB *packetBlock, uint32 semID, uint32 msec)
{
	HRESULT		hResult = NO_ERROR;

	hResult = lhlMsgQueueEnQPacketBlock(queue, packetBlock);
	if (hResult != NO_ERROR) return hResult;

	// Wait for send packet to be acknowledge by response, acknowledge or error
	hResult = TCSemaphoreWaitTimeout (semID, msec);
	
	//If it timed out the macket might not even have left the queue, pull it out.
	if (hResult == E_SEMAPHORE_TIMEOUT) lhlMsgQueueCancelPacketBlock(queue,packetBlock);
	return hResult;
}

