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
	Purpose:	Implementing Semaphore Platform for threadX
				for the windows platform
	
	Revisions:
		created 12/18/2002 kbj

****************************************************************************/

#include <cyg/kernel/kapi.h>
#include "TCTasking.h"

// True when tasking started
extern BOOL gbTaskingStarted; 
// returns number of ticks (rounded up)
extern uint32 msecs2ThreadXTicks (uint32 msecs);		

static char 		moduleName[] = __FILE__ ;

#define NO_OF_SEMAPHORE			96	//LM??? 64 Maximum number of semaphores

static cyg_sem_t coreSemaphore[NO_OF_SEMAPHORE];
static uint16 gbFreeSemaId = 0;   // Next free semaphore

HRESULT TCSemaphoreOpen (uint32 *semID, int32 count) 
{
	if (gbFreeSemaId >= NO_OF_SEMAPHORE) 
	{
		sysLogError(E_SEMAPHORE_OPEN_ERROR, __LINE__, moduleName);
		return E_SEMAPHORE_OPEN_ERROR;
	}

	//sprintfsemaNames[gbFreeSemaId], "Sema%i", gbFreeSemaId+1);

	cyg_semaphore_init(&coreSemaphore[gbFreeSemaId], count);

	gbFreeSemaId++;
	*semID = gbFreeSemaId;  
	return NO_ERROR;
}

HRESULT TCSemaphoreOpenIRQ (uint32 *semID, int32 count) 
{
	return TCSemaphoreOpen (semID, count);
}

HRESULT TCSemaphoreClose (uint32 semID) 
{
	semID--;
	if (semID < gbFreeSemaId)
	{
		cyg_semaphore_destroy(&coreSemaphore[semID]);
		return NO_ERROR;
	}
	sysLogError(E_SEMAPHORE_ID_NOT_VALID, __LINE__, moduleName);
	return E_SEMAPHORE_ID_NOT_VALID;
}

HRESULT TCSemaphoreReset (uint32 semID)
{
        cyg_count32 val;
	semID--; // SemaIDs 1-96
	if (semID < gbFreeSemaId)
	{
	  //zluo, keep it this way to see if this is really needed.coreSemaphore[semID].tx_semaphore_count = 0;
	  cyg_semaphore_peek(&coreSemaphore[semID], &val);
	  //if(val == 0) return NO_ERROR;

	  //printf("TCSemaphoreReset, value = %d\n", val);
	  TCSemaphoreClose (semID+1); // delete the semaphore, 
	  cyg_semaphore_init(&coreSemaphore[semID], 0);
	  return NO_ERROR;
	}
	sysLogError(E_SEMAPHORE_ID_NOT_VALID, __LINE__, moduleName);
	return E_SEMAPHORE_ID_NOT_VALID;

}

HRESULT TCSemaphoreSignal (uint32 semID) 
{
	semID--; // SemaIDs 1-16
	if (semID < gbFreeSemaId)
	{
		if (!gbTaskingStarted) return NO_ERROR;
		cyg_semaphore_post(&coreSemaphore[semID]);
		return NO_ERROR;
	}
	sysLogError(E_SEMAPHORE_ID_NOT_VALID, __LINE__, moduleName);
	return E_SEMAPHORE_ID_NOT_VALID;
}

#pragma long_calls
HRESULT		_lTCSemaphoreSignal (uint32 semID)
{
	return TCSemaphoreSignal (semID);
}
#pragma long_calls_off


HRESULT TCSemaphoreWait (uint32 semID) 
{
	semID--; // SemaIDs 1-16
	if (semID < gbFreeSemaId)
	{
		if (!gbTaskingStarted) return NO_ERROR;
		cyg_semaphore_wait(&coreSemaphore[semID]);
		return NO_ERROR;
	}
	sysLogError(E_SEMAPHORE_ID_NOT_VALID, __LINE__, moduleName);
	return E_SEMAPHORE_ID_NOT_VALID;
}

HRESULT TCSemaphoreWaitTimeout (uint32 semID, uint32 msec)
{
	BOOL hResult;
	uint32 ticks;
	semID--; // SemaIDs 1-16
	if (semID < gbFreeSemaId)
	{
		if (!gbTaskingStarted) return NO_ERROR;
		ticks = msecs2ThreadXTicks(msec);
		ticks += cyg_current_time();
		hResult = cyg_semaphore_timed_wait(&coreSemaphore[semID], ticks);

		if (hResult) {	

		    return NO_ERROR;
		}
		else {
		    return E_SEMAPHORE_TIMEOUT;
		}
	}
	sysLogError(E_SEMAPHORE_ID_NOT_VALID, __LINE__, moduleName);
	return E_SEMAPHORE_ID_NOT_VALID;
}
