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
* DESCRIPTION: Enable DICE timer 2

	Purpose:	Provide an acurate timer for system (in 1/49.152M ticks)
	
	Revisions:
		created 08/27/06 by Zhao Luo

****************************************************************************/

#include "TCTasking.h"
#include <cyg/hal/plf_io.h>

#ifndef _LEAVE_TIMER2

uint32					diceTimer2ThreadID = 0;
const char*				diceTimer2ThreadName = "diceTimer2Thread";

static uint32	hi_counter = 0;
static uint32   low_counter = 0;
static uint32	previous_bt_state = 0;

static void updateDiceTimer2(void)
{
	uint32 dummy;

	dummy = *((volatile uint32 *)TIMER2_CUR_VAL);
	low_counter = (uint32)0xFFFFFFFF - dummy;
	if(previous_bt_state && ((low_counter & 0x80000000) == 0)) {
		hi_counter ++;
	}
	previous_bt_state = low_counter & 0x80000000; //set the new hi
}

uint32 diceTimer2GetLow(void)
{
	return (uint32)0xFFFFFFFF - *((volatile uint32 *)TIMER2_CUR_VAL);
}

void diceTimer2Get(uint32 *high, uint32 *low)
{
	updateDiceTimer2();
	if(high) *high = hi_counter;
	if(low) *low = low_counter;
}

void diceTimer2Reset(void)
{
	//disable timer2
	*((volatile uint32 *)TIMER2_CTRL_REG) = 0;
	hi_counter = 0;
	low_counter = 0;
	//set timer2 to free running, masked. 
	*((volatile uint32 *)TIMER2_CTRL_REG) = 5;
}

/*********************************************************
	Thread watching system
*/
void diceTimer2Thread(void *dummy)
{
	//diceTimer2Reset();
	TCTaskWait(10); //Wait for timer get stable
	DO_FOREVER
	{
		uint32 hi, low;
		updateDiceTimer2();
		diceTimer2Get(&hi, &low);
		TCTaskWait(10000); //every 10 seconds
	}
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT diceTimer2ResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// thread watching system
	hResult = TCTaskResume(diceTimer2ThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT diceTimer2SuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// thread watching system
	hResult = TCTaskSuspend(diceTimer2ThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Create tasks for this module
*/
HRESULT diceTimer2InitializeTask(void)
{
	HRESULT hResult = NO_ERROR;

	// thread watching system
	hResult = TCTaskCreate(&diceTimer2ThreadID, 
						   diceTimer2ThreadName, 
						   diceTimer2Thread, 
						   TASK_PRIORITY_DEFAULT, 
						   TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT diceTimer2Initialize(void)
{
	diceTimer2Reset(); 
	return diceTimer2ResumeTasks();
}

#else

HRESULT diceTimer2InitializeTask(void)
{
	return NO_ERROR;
}

HRESULT diceTimer2Initialize(void)
{
	return NO_ERROR;
}
void diceTimer2Reset(void)
{
}

/// Get timer2's value, 
void diceTimer2Get(uint32 *high, uint32 *low)
{
	if (high) *high=0;
	if (low) *low=0;
}

uint32 diceTimer2GetLow(void)
{
	return 0;
}


#endif
