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
 	Purpose:	Implementing Tasking Platform for threadX

	Revisions:
		created 12/18/2002 kbj

****************************************************************************/

#include <cyg/kernel/kapi.h>
#include <stdio.h>
#include "TCTasking.h"

// returns number of ticks (rounded up)
extern uint32 msecs2ThreadXTicks (uint32 msecs);

extern HRESULT mainInitialize(void);

BOOL gbTaskingStarted; // True when tasking started

static char 		moduleName[] = __FILE__ ;

// Maximum number of threads

typedef struct {
    char 				*threadName;
    cyg_thread_entry_t	*entry;
    cyg_addrword_t		input;
} t_ThreadDefinition;

#define MAX_NUM_TASKS	32

// Table with thread names 
// taskID of the thread is the entry for this table
static t_ThreadDefinition	coreThreadDefs[MAX_NUM_TASKS];
static cyg_thread			coreThreads[MAX_NUM_TASKS];
static cyg_handle_t   		coreThreadHandles[MAX_NUM_TASKS];							// task list identified resumed tasks
static BOOL					coreThreadResumed[MAX_NUM_TASKS];							// task list identified resumed tasks
static char					coreThreadStacks[MAX_NUM_TASKS][10240];						// stacks for threads;

HRESULT TCTaskingInitialize (void) 
{
	cyg_uint16 taskID;
	for (taskID = 0; taskID < MAX_NUM_TASKS; taskID++) 
	{ 
		//zluo, no direct state for threads, coreThreads[taskID].tx_state = TX_TERMINATED;
		coreThreadResumed[taskID] = FALSE;
		coreThreadHandles[taskID] = 0xFFFFFFFF;

		coreThreadDefs[taskID].threadName = 0;
		coreThreadDefs[taskID].entry = NULL;
		coreThreadDefs[taskID].input = 0;
	}
	gbTaskingStarted = FALSE;

	return NO_ERROR;
}

HRESULT TCTaskingStart (void) 
{
	uint16 taskID;

	gbTaskingStarted = TRUE;

	for (taskID = 0; taskID < MAX_NUM_TASKS; taskID++)
	{
		if (coreThreadResumed[taskID] == TRUE)
		{
			if (coreThreadHandles[taskID] != 0xFFFFFFFF)
			{
				cyg_thread_resume(coreThreadHandles[taskID]);
			}
		}
	}

	return NO_ERROR;
}

HRESULT TCTaskingStop (void) 
{
	uint16 taskID;

	gbTaskingStarted = FALSE;
	for (taskID = 0; taskID < MAX_NUM_TASKS; taskID++)
	{
		coreThreadResumed[taskID] = FALSE;

		if ((coreThreadHandles[taskID] != 0xFFFFFFFF))
		{
			cyg_thread_suspend(coreThreadHandles[taskID]);
		}
	}
	return NO_ERROR;
}

// push implementation down one layer so we don't mess w/ the
// original TCTaskCreate
// HNE. 
HRESULT TCTaskCreateInternal (uint32 *taskID, const char *taskName, void (*taskEntry)(void*), void *taskData, uint32 priority, uint32 stackSize)
{
	HRESULT		hResult = NO_ERROR;
	uint16		tskID;

	*taskID = MAX_NUM_TASKS;

	for (tskID = 0; tskID < MAX_NUM_TASKS; tskID++)
	{
		if (coreThreadHandles[tskID] == 0xFFFFFFFF) // Check for free entry
		{
			coreThreadDefs[tskID].entry = (cyg_thread_entry_t*) taskEntry;
			coreThreadDefs[tskID].threadName = (char *) taskName;

			// ADDED By HNE, initialize so that we can use a class member
			// function as a thread function. 
			coreThreadDefs[tskID].input = (cyg_addrword_t)taskData;

			coreThreadResumed[tskID] = FALSE;

			cyg_thread_create(priority, 
					  coreThreadDefs[tskID].entry, 
					  coreThreadDefs[tskID].input,  
					  coreThreadDefs[tskID].threadName, 
					  &coreThreadStacks[tskID][0], stackSize,
					  &coreThreadHandles[tskID],
					  &coreThreads[tskID]);
			*taskID = tskID;
			return hResult;
		}
	}

	// Entry not found
	hResult = E_TASKING_CREATE_ERROR;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Keep orig TCTaskCreate
HRESULT TCTaskCreate(uint32 *taskID, const char *taskName, void (*taskEntry)(void*),  uint32 priority, uint32 stackSize){
	return TCTaskCreateInternal(taskID,taskName,taskEntry,NULL, priority, stackSize);
}
//Extend TCTaskCreate to handle object pointers 
HRESULT TCTaskCreateEx(uint32 *taskID, const char *taskName, void (*taskEntry)(void*), void *taskData, uint32 priority, uint32 stackSize){
	return TCTaskCreateInternal(taskID,taskName,taskEntry, taskData, priority, stackSize);
}

HRESULT TCTaskResume (uint32 taskID) 
{
	HRESULT hResult = NO_ERROR;
	
	if ((taskID >= MAX_NUM_TASKS))
	{
		sysLogError(E_TASK_WRONG_TASKID, __LINE__, moduleName);
		return E_TASK_WRONG_TASKID;
	}

	coreThreadResumed[taskID] = TRUE;

	if (gbTaskingStarted == TRUE)
	{
		cyg_thread_resume(coreThreadHandles[taskID]);
	}
	return hResult;
}

HRESULT TCTaskSuspend (uint32 taskID) 
{
	if ((taskID >= MAX_NUM_TASKS))
	{
		sysLogError(E_TASK_WRONG_TASKID, __LINE__, moduleName);
		return E_TASK_WRONG_TASKID;
	}

	coreThreadResumed[taskID] = FALSE;

	if (gbTaskingStarted == TRUE)
	{
		cyg_thread_suspend(coreThreadHandles[taskID]);
	}
	return NO_ERROR;
}

void TCTaskYield (void) 
{
	cyg_thread_yield();
}

void TCTaskWait(uint32 msecs)
{
	uint32 ticks = msecs2ThreadXTicks(msecs);
	cyg_thread_delay(ticks);
}

uint32 TCTaskGetMyID(void)
{
	uint16 tskID;
	cyg_handle_t pThreadHandle;

	pThreadHandle = cyg_thread_self();
	for (tskID = 0; tskID < MAX_NUM_TASKS; tskID++)
	{
		if (coreThreadHandles[tskID] == pThreadHandle) return tskID;
	}
#ifndef _AVC	// bk: AVC must add tasks early in init order, when this is not an error
	sysLogError(E_TASK_WRONG_TASKID, __LINE__, moduleName);
#endif
	return MAX_NUM_TASKS;
}

uint32 TCTaskingGetMaxNumTasks(void)
{
	return MAX_NUM_TASKS;
}

uint32 TCTaskingGetNumTasks(void)
{
	uint32 items = 0;
	uint16 tskID;

	for (tskID = 0; tskID < MAX_NUM_TASKS; tskID++)
	{
		if (coreThreadHandles[tskID] != 0xFFFFFFFF)
		{
			items++;
		}
	}

	return items;
}

char* TCTaskingGetTaskName(uint32 taskID)
{
	char *taskName = NULL;

	if (coreThreadHandles[taskID] != 0xFFFFFFFF)
	{
		taskName = coreThreadDefs[taskID].threadName;
	}

	return taskName;
}

BOOL TCTaskingGetTaskResumeState(uint32 taskID)
{
	BOOL bResumed = FALSE;

	if (coreThreadHandles[taskID] != 0xFFFFFFFF)
	{
		bResumed = coreThreadResumed[taskID];
	}

	return bResumed;
}

BOOL	TCTaskStarted (void)
{
	return gbTaskingStarted;
}
