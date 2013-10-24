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
 	Purpose:	Implementing Tasking Platform Generic layer
	
	Revisions:
		created 04/03/2002 lm

****************************************************************************/

#ifndef _TC_TASKING_H_
#define _TC_TASKING_H_

#include "ErrorCodes.h"
#include "TCTypes.h"
#include "TCTaskingDefs.h"
#include "TCSemaphore.h"
#include "TCMsgQueue.h"
#include "TCTime.h"
#include "TCMemory.h"
#include "TCDebug.h"

/** @addtogroup RTOSTask
	This modules provides functions for creating and manipulating tasks/threads.
	@{
*/

HRESULT	TCTaskingInitialize (void);
HRESULT	TCTaskingStart (void);
HRESULT	TCTaskingStop (void);

/** Create a task.
	@param taskID		The address to place the taskID.
	@param taskName		The name for the task.
	@param taskEntry	The entry point for the task.
	@param priority		The priority for the task.  See TCTaskingDefs.h
	@param stackSize	The stack size to use.  See TASK_STACKSIZE_DEFAULT in TCTaskingDefs.h
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	TCTaskCreate (uint32 *taskID, const char *taskName, void (*taskEntry)(void*), uint32 priority, uint32 stackSize);

/** Create a task, extended version.
	@param taskID		The address to place the taskID.
	@param taskName		The name for the task.
	@param taskEntry	The entry point for the task.
	@param taskData		void * to be passed into the entry point.... usually an obj pointer.
	@param priority		The priority for the task.  See TCTaskingDefs.h
	@param stackSize	The stack size to use.  See TASK_STACKSIZE_DEFAULT in TCTaskingDefs.h
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	TCTaskCreateEx (uint32 *taskID, const char *taskName, void (*taskEntry)(void*), void *taskData, uint32 priority, uint32 stackSize);

/** Resume a suspended task.
	@param taskID		The ID of the task to resume.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	TCTaskResume (uint32 taskID);

/** Suspend a task.
	@param taskID		The ID of the task to resume.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	TCTaskSuspend (uint32 taskID);

/** Enables a task to obtain its task ID.
	@return				The ID of the calling task.
*/
uint32	TCTaskGetMyID (void);

/** Yield to other tasks.
*/
void 	TCTaskYield (void);

/** Wait for the specified number of millisecondss.
	@param msecs	The time to wait in milliseconds.
*/
void 	TCTaskWait (uint32 msecs);

/** Are all task started?
	@return				false (0) for not started or true (1) for started.
*/
BOOL	TCTaskStarted (void);

uint32	TCTaskingGetMaxNumTasks(void);
uint32	TCTaskingGetNumTasks(void);
char*	TCTaskingGetTaskName(uint32 taskID);
BOOL	TCTaskingGetTaskResumeState(uint32 taskID);

//! @}
#endif
