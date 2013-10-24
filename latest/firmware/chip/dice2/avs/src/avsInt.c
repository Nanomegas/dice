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
	
	Purpose:	AVS interrupt and event thread 
				for handling of events from the AVS system
	
	Revisions:
		created 12/06/2003 kbj/lm


****************************************************************************/

#include "TCTasking.h"
#include "avsDefs.h"
#include "avsEnvDefs.h"
#include "avs.h"
#include "avsInt.h"
#include "lal.h"

//static char 	moduleName[] = __FILE__ ;

#ifdef _AVSINT_POLLING
// task (thread) IDs for this module
uint32					avsIntThreadID;						// avs interrupt handler thread
const char*				avsIntThreadName = "avsIntThread";
#endif //_AVSINT_POLLING

// semaphore IDs for this module
uint32					avsIntSemID;						// interrupt avs semaphore 

// task (thread) IDs for this module
uint32					avsIntEventThreadID;				// avs interrupt event handler thread
const char*				avsIntEventThreadName = "avsIntEventThread";

AVS_EVENTS				avsIntEventState[NUM_AVS_EVENTS];	// AVS interrupt status array structure

/*********************************************************
	Clear interrupt counters
*/
void avsIntClearCounters(void)
{
	uint32 i, j;

	for (i = 0; i < NUM_AVS_EVENTS; i++)
	{
		for (j = 0; j < avsIntEventState[i].numEventItems; j++)
		{
			avsIntEventState[i].avsIntCounter[j] = 0;
		}
	}
}

void avsIntEventThread(void *dummy)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i;
	uint32		j;
	uint32		bit = 0;

	UNUSED_ARG(dummy);

	DO_FOREVER
	{
		hResult = TCSemaphoreWait(avsIntSemID);
		if (hResult != NO_ERROR)
		{
			DO_FOREVER {}
		}

		// For all AVS interrupt events registers, check, count and clear
		for (i = 0; i < NUM_AVS_EVENTS; i++)
		{
			for (j = 0; j < avsIntEventState[i].numEventItems; j++)
			{
				bit = avsIntEventState[i].avsIntEvents[j];
				if (avsIntEventState[i].avsIntEventCopy & bit)
				{
					SYS_DEBUG(SYSDEBUG_TRACE_AVS, "avsEvent: %s\n\r", (char *)((int)avsIntEventState[i].avsIntTexts + AVS_EVENT_TEXT_SIZE * j));

					// Increment event counter
					avsIntEventState[i].avsIntCounter[j]++;

					avsIntEventHandleEvent(i, j);

					// Clear AVS event
					avsIntEventState[i].avsIntEventCopy &= ~bit;
				}
			}
		}
	}
}

BOOL avsIntGetPollEnable(void)
{
#ifdef _AVSINT_POLLING
	return TRUE;
#else //_AVSINT_POLLING
	return FALSE;
#endif //_AVSINT_POLLING
}

#ifdef _AVSINT_POLLING
/*********************************************************
	avs interrupt handler thread
*/
void avsIntThread(void *dummy)
{
	BOOL		bResetDetected = FALSE;

	UNUSED_ARG(dummy);

	// wait for bus reset completion
	lalWaitOnResetCompletion(&bResetDetected);

	DO_FOREVER
	{
		avsIntPollHandlers();
		TCTaskYield();
	}
}
#endif //_AVSINT_POLLING

/*********************************************************
	Enable interrupts
*/
static HRESULT avsIntSetup(void)
{
	HRESULT	hResult = NO_ERROR;
	uint32	intEnable;
	uint32	i;
	uint32	j;
	
	for (i = 0; i < NUM_AVS_EVENTS; i++)
	{
		avsIntEventState[i].numEventItems = avsIntItems[i];
		avsIntEventState[i].avsIntEvents = avsIntEvents[i];
		avsIntEventState[i].avsIntCounter = avsIntCounter[i];
#ifdef _SYSDEBUG
		avsIntEventState[i].avsIntTexts = (char ***)&avsIntTexts[i];
#endif //_SYSDEBUG
		avsIntEventState[i].avsIntEventCopy = 0;
	}

	// Disable all interrupts
	for (i = 0; i < NUM_AVS_EVENTS; i++)
	{
		avsRegWrite(avsIntMask[i], 0);
	}

	avsIntClearCounters();

	for (i = 0; i < NUM_AVS_EVENTS; i++)
	{
		intEnable = 0;
		for (j = 0; j < avsIntEventState[i].numEventItems; j++)
		{
			intEnable |= avsIntEventState[i].avsIntEvents[j];
		}

		// Enable interrupts from AVS INT
		avsRegWrite(avsIntMask[i], intEnable);
	}

	return hResult;
}


/*********************************************************
	Signal avs int event semaphore for this module
*/

#ifdef _AVSTRIG
extern void targetSignalAVSTrigger(void);
#endif



HRESULT avsIntSignalSemaphore(void)
{
	HRESULT hResult = NO_ERROR;

#ifdef _AVSTRIG
	targetSignalAVSTrigger();
#endif //_AVSTRIG
	TCSemaphoreSignal(avsIntSemID);

	return hResult;
}


/*********************************************************
	Create semaphores for this module
*/
HRESULT avsIntCreateSemaphores(void)
{
	HRESULT hResult = NO_ERROR;

	// transmit request packet done
	hResult = TCSemaphoreOpenIRQ(&avsIntSemID, 0);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT avsIntResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avs interrupt event handler thread
	hResult = TCTaskResume(avsIntEventThreadID);
	if (hResult != NO_ERROR) return hResult;
	
#ifdef _AVSINT_POLLING
	// avs interrupt handler thread
	hResult = TCTaskResume(avsIntThreadID);
	if (hResult != NO_ERROR) return hResult;
#endif //_AVSINT_POLLING

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT avsIntSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avs interrupt event handler thread
	hResult = TCTaskSuspend(avsIntEventThreadID);
	if (hResult != NO_ERROR) return hResult;

#ifdef _AVSINT_POLLING
	// avs interrupt handler thread
	hResult = TCTaskSuspend(avsIntThreadID);
	if (hResult != NO_ERROR) return hResult;
#endif //_AVSINT_POLLING

	return hResult;
}

HRESULT avsIntInitialize(void)
{	
	HRESULT hResult = NO_ERROR;
	
	avsIntSetup();
	
	hResult = avsIntCreateSemaphores();
	if (hResult != NO_ERROR) return hResult;

	// Start avs threads
	avsIntResumeTasks();

#ifdef _AVSINT_POLLING
#else //_AVSINT_POLLING
	avsIntInstallHandlers();
#endif //_AVSINT_POLLING

	return hResult;
}

HRESULT avsIntInitializeTasks(void)
{	
	HRESULT hResult = NO_ERROR;
	
	// avs interrupt event handler thread
	hResult = TCTaskCreate(&avsIntEventThreadID, avsIntEventThreadName, avsIntEventThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

#ifdef _AVSINT_POLLING
	// avs interrupt handler thread
	hResult = TCTaskCreate(&avsIntThreadID, avsIntThreadName, avsIntThread, TASK_PRIORITY_BELOW_NORMAL, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;
#endif //_AVSINT_POLLING

	return hResult;
}
