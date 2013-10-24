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

 DESCRIPTION:

	Implementation of avc unit notify utilities
	
  Revisions:
		created 08/04/2003 LM

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "avc.h"
#include "avcDefs.h"
#include "avcUtils.h"
#include "avcUnitNotify.h"
#include "aqm.h"

// task (thread) IDs for this module
uint32					avcUnitNotifyThreadID;			// avcUnitNotify handler thread
const char*				avcUnitNotifyThreadName = "avcUnitNotifyThread";

// msgQueue IDs for this module
uint32					avcUnitNotifyQueueID;			// message queue for avcUnitNotify handler thread

#define		NOTIFY_QUEUE_SIZE		128					// number of queue elements
uint32					avcUnitNotifyQueueMemory[NOTIFY_QUEUE_SIZE];

HRESULT	avcUnitNotifyAdd(LM_CONTEXT* notifyList, UNION_NOTIFY *notify, PB *packetBlock, BOOL bUsePacketBlock)
{
	HRESULT				hResult = NO_ERROR;
	uint32				notifyAddr = 0;
				
	notify->notifyComm.bUsePacketBlock = bUsePacketBlock;

	hResult = pbGetPacketSrcNodeAddr (packetBlock, &notifyAddr);
	if (hResult != NO_ERROR) return hResult;

	// determine if there is a notify from same nodeAddr
	hResult = avcUnitNotifyRemoveOld(notifyList, notifyAddr);
	if (hResult != NO_ERROR) return hResult;

	if (bUsePacketBlock)
	{
		// copy packetBlock, and add it to our list of Notifications
		hResult = pbCreateDuplicatePacket(packetBlock, &notify->notifyComm.packetBlock, NULL, PB_CREATE_AVC_CB);
		if (hResult != NO_ERROR) return hResult;
	}
	else
	{
		NODEHANDLE handle = 0;

		lalGetHandleFromNodeAddr(notifyAddr, &handle);
		notify->notifyComm.handle = handle;
	}

	hResult = lmAddElement(notifyList, notify, NULL);
	if (hResult != NO_ERROR)
	{
		avcUnitNotifyRemoveComm(notify);
		return hResult;
	}


#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC & SYSDEBUG_TRACE_ERRORS)) //SYSDEBUG_TRACE_AVC
	{
		sysPrintCurTime();
		sysDebugPrintf("avcUnitNotify insert new notify\n\r");
	}
#endif //_SYSDEBUG

	return hResult;
}

HRESULT	avcUnitNotifyRemoveOld(LM_CONTEXT* notifyList, uint32 nodeAddr)
{
	HRESULT				hResult = NO_ERROR;
	uint32				pos = 0;
	uint32				index = 0;
	UNION_NOTIFY		*notify;
	uint32				notifyAddr = 0;
	BOOL				bFound = FALSE;

	DO_FOREVER
	{
		hResult = lmGetNthElement(notifyList, (void **) &notify, pos++, &index);
		if (hResult != NO_ERROR) return NO_ERROR;

		hResult = pbGetPacketSrcNodeAddr (notify->notifyComm.packetBlock, &notifyAddr);

		lmReleaseElement(notifyList, index);

		if (hResult != NO_ERROR) return NO_ERROR;
			
		if (nodeAddr == notifyAddr)
		{
			lmRemoveElement(notifyList, index);
			bFound = TRUE;
			break;
		}
	}

#ifdef _SYSDEBUG
	if (bFound)
	{
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC & SYSDEBUG_TRACE_ERRORS)) //SYSDEBUG_TRACE_AVC
		{
			sysPrintCurTime();
			sysDebugPrintf("avcUnitNotify removed old notify\n\r");
		}
	}
#endif //_SYSDEBUG

	return hResult;
}

HRESULT avcUnitNotifyCheck(LM_CONTEXT* notifyList, NOTIFY_CHECK_CALLBACK notifyCheckCB, NOTIFY_UPDATE_CALLBACK notifyUpdateCB)
{
	HRESULT				hResult = NO_ERROR;
	uint32				pos = 0;
	uint32				index = 0;
	pDataStream			pStream = NULL;
	UNION_NOTIFY*		notify = NULL;
	AVC_HEADER			avcHeader;
	BOOL				bChanged = FALSE;
	PB					*packetBlock;

	// determine if there is a notify on the specified subunit
	DO_FOREVER
	{
		hResult = lmGetNthElement(notifyList, (void **) &notify, pos, &index);
		if (hResult != NO_ERROR) return NO_ERROR;

		bChanged = FALSE;

		// call callback to make notify specific check on notify state
		hResult = (* notifyCheckCB) (notify, &bChanged);

		if (bChanged)
		{
#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC & SYSDEBUG_TRACE_ERRORS)) //SYSDEBUG_TRACE_AVC
			{
				sysPrintCurTime();
				sysDebugPrintf("avcUnitNotify changed state for notify\n\r");
			}
#endif //_SYSDEBUG

			hResult = pbGetApplicationDatastream(notify->notifyComm.packetBlock, &pStream);
			if (hResult != NO_ERROR) break;
			
			hResult = avcDecodeHeader(pStream, &avcHeader);
			if (hResult != NO_ERROR) break;

			hResult = dsGotoMarker(pStream, DSMARKER_OPERAND_0);
			if (hResult != NO_ERROR) break;

			hResult = dsSwitchMode(pStream, dsMODE_WRITE); 
			if (hResult != NO_ERROR) break;
				
			// call callback to write notify specific data into stream (from operand[0])
			hResult = (* notifyUpdateCB) (notify, pStream);
			if (hResult != NO_ERROR) break;
			
			packetBlock = notify->notifyComm.packetBlock;

			hResult = lmReleaseElement(notifyList, index);
			if (hResult != NO_ERROR) break;

			hResult = lmRemoveElement(notifyList, index);
			if (hResult != NO_ERROR) break;

			hResult = avcReplyResponse (AVC_RESPONSE_CHANGED, packetBlock);
			if (hResult != NO_ERROR) break;
		}
		else
		{
			lmReleaseElement(notifyList, index);
			pos++;
		}
	}

	lmReleaseElement(notifyList, index);

	return hResult;
}

BOOL avcUnitNotifyCheckAddress(PB *packetBlock, UNION_NOTIFY *notify)
{
	uint32		nodeAddr = 0;
	BOOL		bEqual = FALSE;

	pbGetPacketSrcNodeAddr(packetBlock, &nodeAddr);

	if (notify->notifyComm.bUsePacketBlock)
	{
		uint32		nodeAddrNotify = 0;
	
		pbGetPacketSrcNodeAddr(notify->notifyComm.packetBlock, &nodeAddrNotify);

		bEqual = (nodeAddr == nodeAddrNotify);
	}
	else
	{
		NODEHANDLE	handle = 0;

		lalGetHandleFromNodeAddr(nodeAddr, &handle);

		bEqual = (notify->notifyComm.handle == handle);
	}

	return bEqual;
}

HRESULT avcUnitNotifyRemoveComm(UNION_NOTIFY *notify)
{
	HRESULT		hResult = NO_ERROR;
	
	if (notify->notifyComm.bUsePacketBlock)
	{
		pbPacketDone(notify->notifyComm.packetBlock, PB_DONE_AVC_CB);
	}
	else
	{
		hResult = lalCloseHandle(notify->notifyComm.handle);
	}

	return hResult;
}

HRESULT avcUnitNotifyChange(NOTIFY_CHANGE_CALLBACK notifyChange)
{
	HRESULT		hResult = NO_ERROR;	

	hResult = aqmQueueSend(avcUnitNotifyQueueID, (void *) &notifyChange, TC_NO_WAIT);

	return hResult;
}

/*********************************************************
	Thread waiting for and process incoming request packets
*/
void avcUnitNotifyThread(void *dummy)
{
	HRESULT					hResult = NO_ERROR;
	NOTIFY_CHANGE_CALLBACK	notifyChange = NULL;

	UNUSED_ARG(dummy);

	DO_FOREVER
	{
		// get a valid notify from the queue
		hResult = aqmQueueReceive(avcUnitNotifyQueueID, &notifyChange, TC_WAIT_FOREVER);
		if (hResult == NO_ERROR)
		{
			hResult = (* notifyChange) ();
		}
	}
}

/*********************************************************
	Create message queues for this module
*/
HRESULT avcUnitNotifyCreateMessageQueues(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = aqmQueueOpen(	&avcUnitNotifyQueueID,		// the queue id
							1,							// size of queue element = 1 quadlet
							&avcUnitNotifyQueueMemory,	// the memory to use
							NOTIFY_QUEUE_SIZE);			// how big the queue is
	
	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT avcUnitNotifyResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avcUnitNotify handler thread
	hResult = TCTaskResume(avcUnitNotifyThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT avcUnitNotifySuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avcUnitNotify handler thread
	hResult = TCTaskSuspend(avcUnitNotifyThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Create tasks for this module
*/
HRESULT avcUnitNotifyInitializeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avcUnitNotify handler thread
	hResult = TCTaskCreate(&avcUnitNotifyThreadID, avcUnitNotifyThreadName, avcUnitNotifyThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Initialization of this module
*/
HRESULT avcUnitNotifyInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = avcUnitNotifyCreateMessageQueues();
	if (hResult != NO_ERROR) return hResult;
		
	return hResult;
}
