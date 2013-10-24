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
* DESCRIPTION: Link Hardware Abstraction Layer (Watching system)

	Purpose:	Link Hardware Abstraction Layer 
				Watch System helper functions
	
	Revisions:
		created 12/17/2002 kbj

****************************************************************************/

#include "TCTasking.h"
#include "lhlDefs.h"
#include "lhlPendingTx.h"
#include "lhlService.h"
#include "lhlStatistics.h"
#include "lhlWatch.h"
#include "mpmCli.h"
#include "bri.h"

#include "csr.h"
#include "llcReg.h"
#include "llcDefs.h"
#include "lal.h" //lalIsThisNodeRoot

//static char 		moduleName[] = __FILE__ ;

// task (thread) IDs for this module
uint32					lhlWatchThreadID = 0;				// watch system thread
const char*				lhlWatchThreadName = "lhlWatchThread";

#define	LHL_WATCH_BR_MSEC			100
#define	LHL_WATCH_IDLE_MSEC			500

static uint32	bus_time = 0;
static uint32	previous_bt_state = 0;

static uint32 updateBusTimer(void)
{
	uint32 dummy;

	llcLinkRegRead(CY_TMR_REG_DP, &dummy);
	//if second counter flipped, increment bus_time 7bit;
	if(previous_bt_state && ((dummy & 0x80000000) == 0)) {
		bus_time += 0x80;
	}
	previous_bt_state = dummy & 0x80000000; //set the new bus time state
	//return current bus time.
	return bus_time + ((dummy & 0xFE000000) >> 25);
}

RCODE_1394	lhlWatchTransRead(uint32 offset, uint32 *readData)
{
	RCODE_1394	rCode = RSP_COMPLETE;

	switch (offset)
	{
		case CSR_SB_CYCLE_TIME:
			llcLinkRegRead(CY_TMR_REG_DP, readData);
			break;
		case CSR_SB_BUS_TIME:
			*readData = updateBusTimer();
			break;
		case CSR_SB_BUSY_TIME_OUT:
			llcLinkRegRead(RET_CT_REG_DP, readData);
			break;

		default:
			rCode = RSP_ADDRESS_ERROR;
			break;
	}

	return rCode;
}

RCODE_1394	lhlWatchTransWrite(uint32 offset, uint32 writeData)
{
	RCODE_1394	rCode = RSP_COMPLETE;

	switch (offset)
	{
		case CSR_SB_CYCLE_TIME:
			if(lalIsThisNodeRoot()) {
				llcLinkRegWrite(CY_TMR_REG_DP, writeData);
			} else 
				rCode = RSP_DATA_ERROR;
			break;
		case CSR_SB_BUS_TIME:
			if(lalIsThisNodeRoot()) {
				bus_time = writeData & 0xFFFFFF80; //lower 7 bit is decided by cycle timer
			} else 
				rCode = RSP_DATA_ERROR;
			break;
		case CSR_SB_BUSY_TIME_OUT:
			llcLinkRegWrite(RET_CT_REG_DP, writeData);
			break;

		default:
			rCode = RSP_ADDRESS_ERROR;
			break;
	}

	return rCode;
}
void lhlWatchThreadTest(void *dummy)
{
	UNUSED_ARG(dummy);

	DO_FOREVER
	{
		TCTaskWait(100);
	}
}

/*********************************************************
	Thread watching system
*/
void lhlWatchThread(void *dummy)
{
	HRESULT	hResult = NO_ERROR;
	uint32	elapsedTime;
	uint32	ageTime;
	uint32	briTime;
	uint32	taskWaitTime;

	UNUSED_ARG(dummy);

	ageTime = TCTimeGet();
	briTime = TCTimeGet();
	taskWaitTime = LHL_WATCH_BR_MSEC;

	DO_FOREVER
	{
		hResult = briElapsedTimeSinceResetCompletion(&elapsedTime);
		if (hResult == E_BRI_NEW_BUS_RESET ||
			elapsedTime <= 3000)
		{
			if (lhlServiceInToggleBRLed() == FALSE)
			{
				lhlServiceEnterToggleBRLed();
				taskWaitTime = LHL_WATCH_BR_MSEC;
			}
			lhlServiceToggleBRLed();
		}
		else if (lhlServiceInToggleBRLed() == TRUE)
		{
			lhlServiceLeaveToggleBRLed();
			taskWaitTime = LHL_WATCH_IDLE_MSEC;
		}

		elapsedTime = TCTimeElapsed(ageTime);
		if (elapsedTime >= 1000)
		{
			lhlPendingTxAgeout();
			ageTime = TCTimeGet();
		}

#if defined (_STATICTICS) && defined (_CLI)
		//LM??? check memory etc.
		if (lhlStatistics.pbAllocBlock >= 8) //LM???
		{
			mpmCliDump(DUMP_INUSE_PB);
		}
#endif //_STATICTICS && _CLI

		if (briIsSelfIDsValid() == FALSE)
		{
			//LM??? set a timer for the case where a self-id never shows up
			// this case has not been seen before, but one never knows...
		}

		//Update bus time register
		updateBusTimer();
		TCTaskWait(taskWaitTime);

#ifdef _STATISTICS
//		lhlStats.lhlWatch++;
#endif //_STATISTICS
	}
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT lhlWatchResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// thread watching system
	hResult = TCTaskResume(lhlWatchThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT lhlWatchSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// thread watching system
	hResult = TCTaskSuspend(lhlWatchThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Create tasks for this module
*/
HRESULT lhlWatchInitialize(void)
{
	HRESULT hResult = NO_ERROR;

	// thread watching system
	hResult = TCTaskCreate(&lhlWatchThreadID, lhlWatchThreadName, lhlWatchThread, TASK_PRIORITY_DEFAULT /*TASK_PRIORITY_WATCH*/, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}
