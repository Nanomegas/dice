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

#ifndef _TC_SEMAPHORE_H_
#define _TC_SEMAPHORE_H_

#include "TCTypes.h"
#include "ErrorCodes.h"

/** @addtogroup RTOSSemaphore
	Provides RTOS semaphores.
	@{
*/

/** Create a semaphore.
	@param semID	Address to place the ID of the semaphore in.
	@param count	The initial count of the semaphore.
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT				TCSemaphoreOpen (uint32 *semID, int32 count);
HRESULT				TCSemaphoreOpenIRQ (uint32 *semID, int32 count);

/** Close a sempahore
	@param semID	The ID of the semaphore to close.
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT				TCSemaphoreClose (uint32 semID);

/** Reset a semaphore
	@param semID	The ID of the semaphore to reset.
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/	
HRESULT				TCSemaphoreReset (uint32 semID);

/** Signals a sempahore
	@param semID	The ID of the semaphore to signal.
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT				TCSemaphoreSignal (uint32 semID);

#pragma long_calls
HRESULT		_lTCSemaphoreSignal (uint32 semID);
#pragma long_calls_off

/** Wait for a semaphore
	@param semID	The ID of the semaphore to wait on.
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT				TCSemaphoreWait (uint32 semID);

/** Wait for a sempahore with timeout
	@param semID	The ID of the semaphore to wait on.
	@param msecs	TC_WAIT_FOREVER, TC_NO_WAIT or timeout time in milliseconds.
	@return         NO_ERROR (0) if successful, E_SEMAPHORE_TIMEOUT if a timeout occured.
*/
HRESULT				TCSemaphoreWaitTimeout (uint32 semID, uint32 msecs);

// TCKernel implements two mutual exclusion primitives:
//
// A. mutexes used to ensure exclusive access 
//    when the kernel scheduling is preemptive (e.g. time-slicing is enabled)
//    These mutexes should be used when called from tasks which never yield while locking the mutex
//    if kernel scheduling is non-preemptive (e.g. time-slicing is disabled)
//    these calls to handle mutual exclusion can be optimized away
//    This is done by defining the pre-compiler flag _NO_MUTEX
//    The function implementing these mutexes are TCMutexOpen,TCMutexLock,..

// B. mutexes used to ensure exclusive access 
//    no matter whether kernel scheduling is preemptive or non-preemtive
//    These mutexes should be used when called from tasks which want to yield while locking the mutex
//    The function implementing these mutexes are TCMutexAppOpen,TCMutexAppLock,..


#ifdef _NO_MUTEX
_TC_INLINE HRESULT	TCMutexOpen(uint32 *semID) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	TCMutexOpen(uint32 *semID) {*semID = 0; return NO_ERROR;}
#define				TCMutexClose(semID)		NO_ERROR
#define				TCMutexUnlock(semID)
#define				TCMutexLock(semID)		NO_ERROR
#else //_NO_MUTEX
#define				TCMutexOpen(semID)		TCSemaphoreOpen (semID, 1)
#define				TCMutexClose			TCSemaphoreClose
#define				TCMutexUnlock			TCSemaphoreSignal
#define				TCMutexLock				TCSemaphoreWait
#endif //_NO_MUTEX

#define				TCMutexAppOpen(semID)	TCSemaphoreOpen (semID, 1)
#define				TCMutexAppClose			TCSemaphoreClose
#define				TCMutexAppUnlock		TCSemaphoreSignal
#define				TCMutexAppLock			TCSemaphoreWait
#define				TCMutexAppLockTimeout	TCSemaphoreWaitTimeout

#endif
