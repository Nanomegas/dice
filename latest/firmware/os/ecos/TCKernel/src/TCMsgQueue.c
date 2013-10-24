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
	Purpose:	Implementing Message Queue Platform Generic layer
				for the windows platform
	
	Revisions:
		created 03/25/2003 lm

****************************************************************************/

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"

#define MSGQUEUE_ITEMS	64

typedef struct
{
	BOOL		bUseSema;
	uint32		semID;
	uint32		mutexID;
	uint32		elemSizeQuads;
	uint32		elemMaxItems;
	void		*queueStart;
	uint32		queueSize;
	uint32		putpos;
	uint32		getpos;
	BOOL		bFull;
} MSGQUEUE;

typedef struct 
{
	MSGQUEUE	queue[MSGQUEUE_ITEMS];
	BOOL		allocated[MSGQUEUE_ITEMS];
	uint32		ptr;
} MSGQUEUE_POOL;

static MSGQUEUE_POOL	msgQueuePool;

static BOOL TCMsgQueueGetFreeID (uint32 *queueID) 
{
	BOOL	bFound = FALSE;
	uint32	index;
	uint32	ptr;

	ptr = msgQueuePool.ptr;
	
	for (index = 0; index < MSGQUEUE_ITEMS; index++)
	{
		if (msgQueuePool.allocated[ptr] == FALSE)
		{
			*queueID = ptr;
			msgQueuePool.allocated[ptr] = TRUE;
			bFound = TRUE;
		}

		ptr = (ptr + 1) % MSGQUEUE_ITEMS;
		
		if (bFound == TRUE)
		{
			msgQueuePool.ptr = ptr;
			break;
		}
	}

	return bFound;
}

static MSGQUEUE* TCMsgQueueGetFromID (uint32 queueID) 
{
	HRESULT		hResult = NO_ERROR;

	if (queueID >= MSGQUEUE_ITEMS)
	{
		hResult = E_MSGQUEUE_ID_NOT_VALID;
		sysLogError(hResult, __LINE__, moduleName);
		return NULL;
	}

	return (&msgQueuePool.queue[queueID]);
}

HRESULT TCMsgQueueOpen (uint32 *queueID, uint32 elemSizeQuads, void *queueStart, uint32 queueSize, BOOL bUseSema) 
{
	HRESULT		hResult = NO_ERROR;
	MSGQUEUE	*queue = NULL;

	if (TCMsgQueueGetFreeID (queueID) == FALSE)
	{
		hResult = E_MSGQUEUE_OPEN_ERROR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	queue = TCMsgQueueGetFromID (*queueID);

	queue->bUseSema = bUseSema;
	if (queue->bUseSema)
	{
		hResult = TCSemaphoreOpen (&queue->semID, 0);
		if (hResult != NO_ERROR) return hResult;
	}

	// exclusive access for the queue pointers (mutex)
	hResult = TCMutexOpen(&queue->mutexID);
	if (hResult != NO_ERROR) return hResult;

	queue->elemSizeQuads = elemSizeQuads;
	queue->queueStart = queueStart;
	queue->queueSize = queueSize;
	queue->putpos = 0;
	queue->getpos = 0;
	queue->bFull = FALSE;
	queue->elemMaxItems = (queueSize / (elemSizeQuads * 4));

	return hResult;
}

HRESULT TCMsgQueueClose (uint32 queueID) 
{
	HRESULT		hResult = NO_ERROR;
	MSGQUEUE	*queue = NULL;

	if (queueID >= MSGQUEUE_ITEMS)
	{
		hResult = E_MSGQUEUE_ID_NOT_VALID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (msgQueuePool.allocated[queueID] == FALSE)
	{
		hResult = E_MSGQUEUE_CLOSE_ERROR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	queue = TCMsgQueueGetFromID (queueID);

	if (queue->bUseSema)
	{
		hResult = TCSemaphoreClose (queue->semID);
		if (hResult != NO_ERROR) return hResult;
	}

	hResult = TCMutexClose (queue->mutexID);
	if (hResult != NO_ERROR) return hResult;

	queue->semID = 0;
	queue->mutexID = 0;
	queue->elemSizeQuads = 0;
	queue->queueStart = 0;
	queue->queueSize = 0;
	queue->putpos = 0;
	queue->getpos = 0;
	queue->bFull = FALSE;

	msgQueuePool.allocated[queueID] = FALSE;

	return hResult;
}

HRESULT TCMsgQueueReset (uint32 queueID) 
{
	HRESULT		hResult = NO_ERROR;
	MSGQUEUE	*queue = NULL;

	if (queueID >= MSGQUEUE_ITEMS ||
		msgQueuePool.allocated[queueID] == FALSE)
	{
		hResult = E_MSGQUEUE_ID_NOT_VALID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	queue = TCMsgQueueGetFromID (queueID);

	if (queue->bUseSema)
	{
		hResult = TCSemaphoreReset (queue->semID);
		if (hResult != NO_ERROR) return hResult;
	}

	// exclusive access for the queue pointers (mutex)
	hResult = TCMutexLock(queue->mutexID);
	if (hResult != NO_ERROR) return hResult;

	queue->putpos = 0;
	queue->getpos = 0;
	queue->bFull = FALSE;
	// exclusive access for the queue pointers(mutex)
	TCMutexUnlock(queue->mutexID);

	return hResult;
}

HRESULT TCMsgQueueGetNoOfItems (uint32 queueID, uint32 *items) 
{
	HRESULT		hResult = NO_ERROR;
	MSGQUEUE	*queue = NULL;

	*items = 0;

	if (queueID >= MSGQUEUE_ITEMS ||
		msgQueuePool.allocated[queueID] == FALSE)
	{
		hResult = E_MSGQUEUE_ID_NOT_VALID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	queue = TCMsgQueueGetFromID (queueID);

#if 0 // could be used to check integrity of get,put pointers and signalled semaphores
	if (queue->bUseSema)
	{
		hResult = TCSemaphoreNoOfItems (queue->semID);
		if (hResult != NO_ERROR) return hResult;
	}
#endif

	// exclusive access for the queue pointers (mutex)
	hResult = TCMutexLock(queue->mutexID);
	if (hResult != NO_ERROR) return hResult;

	if (queue->getpos == queue->putpos)	// queue is full or empty
	{
		*items = (queue->bFull == TRUE) ? queue->elemMaxItems : 0;
	}
	else								// queue is non-empty
	{
		*items = (queue->putpos - queue->getpos + queue->elemMaxItems) % queue->elemMaxItems;
	}

	// exclusive access for the queue pointers(mutex)
	TCMutexUnlock(queue->mutexID);

	return hResult;
}

HRESULT TCMsgQueueSend (uint32 queueID, void *pSrc, uint32 msec) 
{
	HRESULT		hResult = NO_ERROR;
	MSGQUEUE	*queue = NULL;
	uint32		*pSrcQuad = NULL;
	uint32		*pDstQuad = NULL;
	uint32		i;

	UNUSED_ARG(msec);

	if (queueID >= MSGQUEUE_ITEMS ||
		msgQueuePool.allocated[queueID] == FALSE)
	{
		hResult = E_MSGQUEUE_ID_NOT_VALID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	queue = TCMsgQueueGetFromID (queueID);

	// exclusive access for the queue pointers (mutex)
	hResult = TCMutexLock(queue->mutexID);
	if (hResult != NO_ERROR) return hResult;

	if (queue->bFull == FALSE)
	{
		pSrcQuad = (uint32 *) pSrc;
		pDstQuad = (uint32 *) (queue->queueStart) + (queue->putpos * queue->elemSizeQuads);

		for (i = 0; i < queue->elemSizeQuads; i++)
		{
			pDstQuad[i] = pSrcQuad[i];
		}

		queue->putpos = (queue->putpos + 1) % queue->elemMaxItems;
		queue->bFull = (queue->putpos == queue->getpos);
	}
	else
	{
		hResult = E_MSGQUEUE_FULL;
		sysLogError(hResult, __LINE__, moduleName);
	}

	// exclusive access for the queue pointers(mutex)
	TCMutexUnlock(queue->mutexID);

	if (queue->bUseSema)
	{
		hResult = TCSemaphoreSignal (queue->semID);
		if (hResult != NO_ERROR) return hResult;
	}

	return hResult;
}

HRESULT TCMsgQueueReceive (uint32 queueID, void *pDst, uint32 msec) 
{
	HRESULT		hResult = NO_ERROR;
	MSGQUEUE	*queue = NULL;
	uint32		*pSrcQuad = NULL;
	uint32		*pDstQuad = NULL;
	uint32		i;

	if (queueID >= MSGQUEUE_ITEMS ||
		msgQueuePool.allocated[queueID] == FALSE)
	{
		hResult = E_MSGQUEUE_ID_NOT_VALID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	queue = TCMsgQueueGetFromID (queueID);

	if (queue->bUseSema)
	{
		if (msec == TC_WAIT_FOREVER)
		{
			hResult = TCSemaphoreWait (queue->semID);
		}
		else
		{
			hResult = TCSemaphoreWaitTimeout (queue->semID, msec);
		}

		if (hResult != NO_ERROR)
		{
			if (hResult == E_SEMAPHORE_TIMEOUT) hResult = E_MSGQUEUE_TIMEOUT;
			return hResult;
		}
	}

	// exclusive access for the queue pointers (mutex)
	hResult = TCMutexLock(queue->mutexID);
	if (hResult != NO_ERROR) return hResult;

	if (queue->getpos != queue->putpos || 
		queue->bFull == TRUE)
	{
		pSrcQuad = (uint32 *) (queue->queueStart) + (queue->getpos * queue->elemSizeQuads);
		pDstQuad = (uint32 *) pDst;
		
		for (i = 0; i < queue->elemSizeQuads; i++)
		{
			pDstQuad[i] = pSrcQuad[i];
		}

		queue->getpos = (queue->getpos + 1) % queue->elemMaxItems;
		queue->bFull = FALSE;
	}
	else
	{
		hResult = E_MSGQUEUE_EMPTY;
	}

	// exclusive access for the queue pointers(mutex)
	TCMutexUnlock(queue->mutexID);

	return hResult;
}
