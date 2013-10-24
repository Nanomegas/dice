/*===================================================================*
  Copyright (c) 2002 TC Applied Technologies Ltd.
  -------------------------------------------------------------------
  $Header: /usr/local/cvsroot/firmware/chip/diceJr/gray/src/gray.c,v 1.2 2007/02/08 17:59:33 zluo Exp $
 *===================================================================*/ 

 /*********************************************************************
 * DESCRIPTION: 

	Gray Module for Gray encoder interface
		
	Revisions:
		created 12/11/2004 lm

***********************************************************************/

#include "TCTasking.h"
#include "TCInterrupt.h"
#include "coreDefs.h"
#include "gray.h"
#include <cyg/hal/hal_platform_ints.h> //for interrupt number

#ifdef _CLI
#include "cli.h"
#endif

static char 		moduleName[] = __FILE__;

typedef struct
{
	uint32					mode[GRAY_ENC_ITEMS];
	uint32					accMode[GRAY_ENC_ITEMS];
	GRAYACC_CALLBACK		accCustomCallback[GRAY_ENC_ITEMS];
	GRAYEVENT_CALLBACK		callback[GRAY_ENC_ITEMS];
	uint32					lastEventTime[GRAY_ENC_ITEMS];
	uint32					mask;
} GRAY_EVENT_SETUP;			
							
static GRAY_EVENT_SETUP		grayEventSetup;
							
typedef struct				
{							
	uint32					index;
	uint32					mode;
	int8					change[GRAY_ENC_ITEMS];
	uint32					timeDiff;
} GRAY_EVENT;
							
#ifdef _GRAYINT_POLLING
// task (thread) IDs for this module
uint32						grayIntThreadID;						// gray Interrupts handler thread
const char*					grayIntThreadName = "grayIntThread";

static BOOL					grayEventPollingEnable = FALSE; //TRUE;
#endif //_GRAYINT_POLLING

#ifdef _GRAYINT_DEFERRED

// task (thread) IDs for this module
uint32						grayDeferredThreadID;					// gray deferred event handler thread
const char*					grayDeferredThreadName = "grayDeferredThread";

// msgQueue IDs for this module											
uint32						grayDeferredQueueID;					// gray deferred event handler thread message queue
																		
#define GRAY_DEFERRED_ITEMS	16
#define GRAY_DEFERRED_QUEUE_SIZE		(GRAY_DEFERRED_ITEMS*4)

static BYTE grayDeferredEventQueueMemory[GRAY_DEFERRED_QUEUE_SIZE];	// the queue memory space

typedef struct				
{							
	GRAY_EVENT				events[GRAY_DEFERRED_ITEMS];
	BOOL					allocated[GRAY_DEFERRED_ITEMS];
	uint32					ptr;
} GRAY_DEFERRED_EVENT;

static GRAY_DEFERRED_EVENT	grayDeferredEvent;

#endif //_GRAYINT_DEFERRED

static BOOL					grayEventInitialized;

// Interrupt events from Gray Encoders
static uint32				grayEvents;								// gray interrupt event
static uint32				grayEvent[GRAY_ENC_ITEMS];

BOOL grayEventIsIntEnabled(uint32 enc)
{
	uint32	bit;

	if (enc > GRAY_ENC_ITEMS - 1)
	{
		return FALSE;
	}

	bit = 1 << enc;

	return ((*((volatile uint32 *) GRAY_INT_SETUP) & bit) == bit);
}

uint32 grayEventGetIntCount(uint32 enc)
{
	if (enc > GRAY_ENC_ITEMS - 1)
	{
		return 0;
	}

	return grayEvent[enc];
}

uint32 grayEventGetIntCounts(void)
{
	return grayEvents;
}

void grayEventClearIntCounts(void)
{
	uint32	enc;

	for (enc = 0; enc < GRAY_ENC_ITEMS; enc++)
	{
		grayEvent[enc] = 0;
	}
	grayEvents = 0;
}

void grayEventHandleChange(GRAY_EVENT *event)
{
	int8	change = 0;
	uint32	enc;

	for (enc = 0; enc < GRAY_ENC_ITEMS; enc++)
	{
		change = event->change[enc];
		
		if (change)
		{
			switch (grayEventSetup.accMode[enc])
			{
				default:
				case GRAY_ENC_ACC_MODE_NONE:
					break;

				case GRAY_ENC_ACC_MODE_TYPE1:
					switch (event->mode)
					{
						case GRAY_EVENT_DIRECT_POLLING:
						case GRAY_EVENT_DEFERRED_POLLING:
							if ((change == 1) || (change == -1))
							{
								change *= 1;
							}
							else if ((change == 2) || (change == -2))
							{
								change *= 2;
							}
							else if ((change == 3) || (change == -3))
							{
								change *= 4;
							}
							else if ((change < 5) || (change > -5))
							{
								change *= 8;
							}
							else if ((change > 5) || (change < -5))
							{
								change *= 16;
							}
							break;

						case GRAY_EVENT_DIRECT_INTERRUPT:
						case GRAY_EVENT_DEFERRED_INTERRUPT:
							if (event->timeDiff <= 10)
							{
								change *= 10;
							}
							else if (event->timeDiff <= 20)
							{
								change *= 5;
							}
							else if (event->timeDiff <= 50)
							{
								change *= 2;
							}
							break;
					}
					break;

				case GRAY_ENC_ACC_MODE_TYPE2:
					switch (event->mode)
					{
						case GRAY_EVENT_DIRECT_POLLING:
						case GRAY_EVENT_DEFERRED_POLLING:
							if ((change == 2) || (change == -2))
							{
								change *= 2;
							}
							else if ((change == 3) || (change == -3))
							{
								change *= 3;
							}
							else if ((change > 3) || (change < -3))
							{
								change *= 4;
							}
							break;

						case GRAY_EVENT_DIRECT_INTERRUPT:
						case GRAY_EVENT_DEFERRED_INTERRUPT:
							if (event->timeDiff <= 10)
							{
								change *= 6;
							}
							else if (event->timeDiff <= 20)
							{
								change *= 4;
							}
							else if (event->timeDiff <= 40)
							{
								change *= 2;
							}
							break;
					}
					break;
				case GRAY_ENC_ACC_MODE_CUSTOM:
					if (grayEventSetup.accCustomCallback[enc] != NULL)
					{
						(*grayEventSetup.accCustomCallback[enc])(&change, event->timeDiff);
					}
			}

//			cliPrintf("grayEventHandleChange: enc:%i, change:%i Acc:%i timeDiff:%i accMode:%i mode:%i\n\r", enc, event->change[enc], change, event->timeDiff, grayEventSetup.accMode[enc], event->mode);
//			cliPrintf("grayEventHandleChange: enc:%i, change:%i Acc:%i timeDiff:%i\n\r", enc, event->change[enc], change, event->timeDiff,);
			grayEventCallCallback(enc, change);
		}
	}

	switch (event->mode)
	{
#ifdef _GRAYINT_DEFERRED
		case GRAY_EVENT_DEFERRED_POLLING:
		case GRAY_EVENT_DEFERRED_INTERRUPT:
			grayDeferredEvent.allocated[event->index] = FALSE;
			break;
#endif //_GRAYINT_DEFERRED

		case GRAY_EVENT_DIRECT_POLLING:
		case GRAY_EVENT_DIRECT_INTERRUPT:
			break;
	}
}

static uint32 grayEventGetCounters(void)
{
	uint32	counters = 0;

	counters = *((volatile uint32 *) GRAY_COUNTERS);
	counters &= grayEventSetup.mask;

	return counters;
}

void grayEventGetChange(GRAY_EVENT *event)
{
	uint32	enc;
	uint32	counters = 0;
	uint32	time;

	counters = grayEventGetCounters();

	time = TCTimeGet();
	grayEvents++;

	for (enc = 0; enc < GRAY_ENC_ITEMS; enc++)
	{
		if (counters & 0xff)
		{
			event->change[enc] = (counters & 0x80) ? -1 - (0xff - (counters & 0xff)) : (counters & 0xff);
			event->timeDiff = TCTimeDifference(time, grayEventSetup.lastEventTime[enc]);
			grayEventSetup.lastEventTime[enc] = time;
			grayEvent[enc]++;
//			cliPrintf("grayEventGetChange: enc: %i, counters: 0x%08x, change:%i timeDiff:%i\n\r", enc, counters, event->change[enc], event->timeDiff);
		}
		counters >>= 8;
		if (counters == 0) break;
	}
}

_TC_INLINE_STATIC void grayEventHandlerDirect(uint32 mode) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void grayEventHandlerDirect(uint32 mode)
{
	GRAY_EVENT	event = {0};

	event.mode = mode;
	grayEventGetChange(&event);
	grayEventHandleChange(&event);
}

#ifdef _GRAYINT_DEFERRED
_TC_INLINE_STATIC void grayEventHandlerDeferred(uint32 mode) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void grayEventHandlerDeferred(uint32 mode)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i;
	uint32		ptr;
	uint32		enc;
	BOOL		bFound = FALSE;
	GRAY_EVENT	*event;

	ptr = grayDeferredEvent.ptr;

	for (i = 0; i < GRAY_DEFERRED_ITEMS; i++)
	{
		if (grayDeferredEvent.allocated[ptr] == FALSE)
		{
			grayDeferredEvent.allocated[ptr] = TRUE;
			event = &grayDeferredEvent.events[ptr];
			bFound = TRUE;
		}

		ptr++;
		if (ptr > GRAY_DEFERRED_ITEMS - 1)
		{
			ptr = 0;
		}
		if (bFound == TRUE)
		{
			grayDeferredEvent.ptr = ptr;
			break;
		}
	}

	if (bFound == FALSE)
	{
		grayEventGetCounters(); // clear interrupt
		hResult = E_GRAY_EVENT_FULL;
		sysLogErrorISR(hResult, __LINE__, moduleName);
		return;
	}

	event->mode = mode;
	for (enc = 0; enc < GRAY_ENC_ITEMS; enc++)
	{
		event->change[enc] = 0;
	}
	event->timeDiff = 0;
	grayEventGetChange(event);

	// gray deferred event handler thread message queue
	hResult = TCMsgQueueSend(grayDeferredQueueID, (void *) &event, TC_NO_WAIT);
	if (hResult != NO_ERROR)
	{
		sysLogErrorISR(hResult, __LINE__, moduleName);
		return;
	}
}
#endif //_GRAYINT_DEFERRED

_TC_INLINE_STATIC void grayEventHandlerDirectInterrupt(void) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void grayEventHandlerDirectInterrupt(void)
{
	grayEventHandlerDirect(GRAY_EVENT_DIRECT_INTERRUPT);
}

_TC_INLINE_STATIC void grayEventHandlerDirectPolling(void) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void grayEventHandlerDirectPolling(void)
{
	grayEventHandlerDirect(GRAY_EVENT_DIRECT_POLLING);
}

#ifdef _GRAYINT_DEFERRED
_TC_INLINE_STATIC void grayEventHandlerDeferredInterrupt(void) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void grayEventHandlerDeferredInterrupt(void)
{
	grayEventHandlerDeferred(GRAY_EVENT_DEFERRED_INTERRUPT);
}

_TC_INLINE_STATIC void grayEventHandlerDeferredPolling(void) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void grayEventHandlerDeferredPolling(void)
{
	grayEventHandlerDeferred(GRAY_EVENT_DEFERRED_POLLING);
}
#endif //_GRAYINT_DEFERRED

HRESULT grayEventInitCallbacks(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		enc;
	uint32		counters;

#ifdef _GRAYINT_DEFERRED
	{
		uint32		i;

		for (i = 0; i < GRAY_DEFERRED_ITEMS; i++)
		{
			grayDeferredEvent.allocated[i] = FALSE;
			grayDeferredEvent.events[i].index = i;
		}
		grayDeferredEvent.ptr = 0;
	}
#endif //_GRAYINT_DEFERRED

	for (enc = 0; enc < GRAY_ENC_ITEMS; enc++)
	{
		grayEventSetup.mode[enc] = GRAY_ENC_MODE_DEFAULT;
		grayEventSetup.accMode[enc] = GRAY_ENC_ACC_MODE_DEFAULT;
		grayEventSetup.accCustomCallback[enc] = NULL;
		grayEventSetup.callback[enc] = NULL;
		grayEventSetup.lastEventTime[enc] = 0;
	}
	grayEventSetup.mask = 0;

	*((volatile uint32 *) GRAY_INT_SETUP) = 0;
	counters = *((volatile uint32 *) GRAY_COUNTERS); // clear status and counters

	return hResult;
}

HRESULT grayEventSetMode(uint32 enc, uint32 mode)
{
	HRESULT hResult = NO_ERROR;

	if (enc > GRAY_ENC_ITEMS - 1)
	{
		hResult = E_GRAY_EVENT_CB_RANGE_ERR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	switch (mode)
	{
		case GRAY_ENC_MODE_DIRECT:
#ifdef _GRAYINT_DEFERRED
		case GRAY_ENC_MODE_DEFERRED:
#endif //_GRAYINT_DEFERRED
			break;
		default:
			hResult = E_BAD_INPUT_PARAMETERS;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
	}

	grayEventSetup.mode[enc] = mode;

	return hResult;
}

HRESULT grayEventSetAccMode(uint32 enc, uint32 accMode, GRAYACC_CALLBACK accCustomCallback)
{
	HRESULT hResult = NO_ERROR;

	if (enc > GRAY_ENC_ITEMS - 1)
	{
		hResult = E_GRAY_EVENT_CB_RANGE_ERR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	switch (accMode)
	{
		case GRAY_ENC_ACC_MODE_NONE:
		case GRAY_ENC_ACC_MODE_TYPE1:
		case GRAY_ENC_ACC_MODE_TYPE2:
			break;
		case GRAY_ENC_ACC_MODE_CUSTOM:
			grayEventSetup.accCustomCallback[enc] = accCustomCallback;
			break;
		default:
			hResult = E_BAD_INPUT_PARAMETERS;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
	}

	grayEventSetup.accMode[enc] = accMode;

	return hResult;
}

HRESULT grayEventGetMode(uint32 enc, uint32 *mode)
{
	HRESULT hResult = NO_ERROR;

	if (enc > GRAY_ENC_ITEMS - 1)
	{
		hResult = E_GRAY_EVENT_CB_RANGE_ERR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*mode = grayEventSetup.mode[enc];

	return hResult;
}

HRESULT grayEventGetAccMode(uint32 enc, uint32 *accMode)
{
	HRESULT hResult = NO_ERROR;

	if (enc > GRAY_ENC_ITEMS - 1)
	{
		hResult = E_GRAY_EVENT_CB_RANGE_ERR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*accMode = grayEventSetup.accMode[enc];

	return hResult;
}

HRESULT grayEventAddCallback(uint32 enc, GRAYEVENT_CALLBACK callback)
{
	HRESULT hResult = NO_ERROR;

	if (enc > GRAY_ENC_ITEMS - 1)
	{
		hResult = E_GRAY_EVENT_CB_RANGE_ERR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (grayEventSetup.callback[enc] == NULL)
	{
		grayEventSetup.callback[enc] = callback;

		grayEventHandlerDirectPolling(); // clear status and counters before installing enc callback (handle possible installed ones)

		grayEventSetup.mask |= 0xff << (enc * 8);
		*((volatile uint32 *) GRAY_INT_SETUP) |= 1 << enc;
	}
	else
	{
		hResult = E_GRAY_EVENT_CB_ERR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

HRESULT grayEventGetCallback(uint32 enc, GRAYEVENT_CALLBACK *callback)
{
	HRESULT hResult = NO_ERROR;

	if (enc > GRAY_ENC_ITEMS - 1)
	{
		hResult = E_GRAY_EVENT_CB_RANGE_ERR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*callback = grayEventSetup.callback[enc];

	return hResult;
}

HRESULT grayEventCallCallback(uint32 enc, int8 change)
{
	HRESULT hResult = NO_ERROR;

	if (enc > GRAY_ENC_ITEMS - 1)
	{
		hResult = E_GRAY_EVENT_CB_RANGE_ERR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (grayEventSetup.callback[enc] != NULL)
	{
		(*grayEventSetup.callback[enc])((uint8) enc, change);
	}
	else
	{
		hResult = E_GRAY_EVENT_CB_ERR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

HRESULT grayEventRemoveCallback(uint32 enc)
{
	HRESULT hResult = NO_ERROR;

	if (enc > GRAY_ENC_ITEMS - 1)
	{
		hResult = E_GRAY_EVENT_CB_RANGE_ERR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (grayEventSetup.callback[enc] != NULL)
	{
		grayEventSetup.callback[enc] = NULL;
		grayEventSetup.mask &= ~(0xff << (enc * 8));
		*((volatile uint32 *) GRAY_INT_SETUP) &= ~(1 << enc);
	}
	else
	{
		hResult = E_GRAY_EVENT_CB_ERR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

void grayIntEventHandler(void)
{
#ifdef _GRAYINT_DEFERRED
	// put gray event to gray event handler threads messageQueue
	grayEventHandlerDeferredInterrupt();
#else //_GRAYINT_DEFERRED
	// call gray event handler directly
	grayEventHandlerDirectInterrupt();
#endif //_GRAYINT_DEFERRED
}

#ifdef _GRAYINT_DEFERRED
void grayDeferredThread(void *dummy)
{
	HRESULT		hResult = NO_ERROR;
	GRAY_EVENT*	event;

	UNUSED_ARG(dummy);

	DO_FOREVER
	{
		// gray deferred event handler thread message queue
		hResult = TCMsgQueueReceive(grayDeferredQueueID, (void *) &event, TC_WAIT_FOREVER);
		
		if (hResult == NO_ERROR)
		{
			grayEventHandleChange(event);
		}
	}
}
#endif //_GRAYINT_DEFERRED

#ifdef _GRAYINT_POLLING

BOOL TCInterruptPollGray(void)
{
#ifdef _DICEJR
	return ((*((uint32 *)ICTL_RAWSTAT)) & (1 << CYGNUM_HAL_INTERRUPT_GRAY));
#elif defined _DICE2_FPGA
	return FALSE;
#endif
}

/*********************************************************
	gray interrupt handler thread
*/
void grayIntThread(void *dummy)
{
	UNUSED_ARG(dummy);

	DO_FOREVER
	{
		if (TCInterruptPollGray())
		{
#ifdef _GRAYINT_DEFERRED
			// put gray event to gray event handler threads messageQueue
			grayEventHandlerDeferredPolling();
#else //_GRAYINT_DEFERRED
			// call gray event handler directly
			grayEventHandlerDirectPolling();
#endif //_GRAYINT_DEFERRED
		}
		TCTaskWait(50);
	}
}
#endif //_GRAYINT_POLLING

/*********************************************************
	Create message queues for this module
*/
HRESULT grayEventCreateMessageQueues(void)
{
	HRESULT		hResult = NO_ERROR;

#ifdef _GRAYINT_DEFERRED
	// gray deferred event handler thread message queue
	hResult = TCMsgQueueOpen(&grayDeferredQueueID,				// the queue id
							1,									// size of queue element = 1 quadlet
							&grayDeferredEventQueueMemory,		// the memory to use
							GRAY_DEFERRED_QUEUE_SIZE,			// how big the queue is (in bytes)
							TRUE);
#endif //_GRAYINT_DEFERRED
	
	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT grayEventResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

#ifdef _GRAYINT_DEFERRED
	// gray deferred event handler thread
	hResult = TCTaskResume(grayDeferredThreadID);
	if (hResult != NO_ERROR) return hResult;
#endif //_GRAYINT_DEFERRED

#ifdef _GRAYINT_POLLING
	if (grayEventPollingEnable == TRUE)
	{
		// gray polling interrupt handler thread
		hResult = TCTaskResume(grayIntThreadID);
		if (hResult != NO_ERROR) return hResult;
	}
#endif //_GRAYINT_POLLING

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT grayEventSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

#ifdef _GRAYINT_DEFERRED
	// gray deferred event handler thread
	hResult = TCTaskSuspend(grayDeferredThreadID);
	if (hResult != NO_ERROR) return hResult;
#endif //_GRAYINT_DEFERRED

#ifdef _GRAYINT_POLLING
	if (grayEventPollingEnable == TRUE)
	{
		// gray polling interrupt handler thread
		hResult = TCTaskSuspend(grayIntThreadID);
		if (hResult != NO_ERROR) return hResult;
	}
#endif //_GRAYINT_POLLING

	return hResult;
}

HRESULT grayEventInitializeTasks(void)
{	
	HRESULT hResult = NO_ERROR;
	
#ifdef _GRAYINT_DEFERRED
	// gray deferred event handler thread
	hResult = TCTaskCreate(&grayDeferredThreadID, grayDeferredThreadName, grayDeferredThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;
#endif //_GRAYINT_DEFERRED

#ifdef _GRAYINT_POLLING
	if (grayEventPollingEnable == TRUE)
	{
		// gray polling interrupt handler thread
		hResult = TCTaskCreate(&grayIntThreadID, grayIntThreadName, grayIntThread, TASK_PRIORITY_BELOW_NORMAL, TASK_STACKSIZE_DEFAULT);
		if (hResult != NO_ERROR) return hResult;
	}
#endif //_GRAYINT_POLLING

	return hResult;
}

HRESULT grayEventSetPollEnable(BOOL bEnable)
{	
	HRESULT hResult = NO_ERROR;

	if (grayEventInitialized == TRUE)
	{
		hResult = E_GRAY_EVENT_INITIALIZE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

#ifdef _GRAYINT_POLLING
	grayEventPollingEnable = bEnable;
#else //_GRAYINT_POLLING
	if (bEnable)
	{
		hResult = E_GRAY_EVENT_INITIALIZE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
#endif //_GRAYINT_POLLING

	return hResult;
}

BOOL grayEventGetPollEnable(void)
{
	BOOL bEnable = FALSE;

#ifdef _GRAYINT_POLLING
	bEnable = grayEventPollingEnable;
#endif //_GRAYINT_POLLING

	return bEnable;
}

HRESULT grayEventInitialize(void)
{	
	HRESULT hResult = NO_ERROR;
	

	hResult = grayEventInitCallbacks();
	if (hResult != NO_ERROR) return hResult;

	hResult = grayEventCreateMessageQueues();
	if (hResult != NO_ERROR) return hResult;

	// Start gray threads
	grayEventResumeTasks();

#ifdef _GRAYINT_POLLING
	if (grayEventPollingEnable == FALSE)
#endif //_GRAYINT_POLLING
	{
		//hResult = TCInterruptCreateGray(&grayIntEventHandler);
		TCInterruptInstallHandler(CYGNUM_HAL_INTERRUPT_GRAY, NULL, &grayIntEventHandler);
		if (hResult != NO_ERROR) return hResult;
	}

	grayEventInitialized = TRUE;

	return hResult;
}

