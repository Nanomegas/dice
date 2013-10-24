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
 	Purpose:	Implementing Time Platform Generic layer
				for the windows platform

	Revisions:
		created 12/18/2002 kbj

****************************************************************************/

#include <limits.h>
#include <cyg/kernel/kapi.h>
#include "TCTime.h"

#define	MSECS_PER_THREADX_TICK	10	// depending on timer 1 interrupt setting

/*****************************************************************************

	Timer utilities

*****************************************************************************/

int32 threadxTicks2Msecs (uint32 ticks)
{
	return(ticks * MSECS_PER_THREADX_TICK);
}	

/***************/

#define TX_NO_WAIT		0
#define TX_WAIT_FOREVER	0xFFFFFFFF

uint32 msecs2ThreadXTicks (uint32 msecs)		// returns number of ticks (rounded up)
{

	if ((msecs == TX_NO_WAIT) ||
		(msecs == TX_WAIT_FOREVER))
	{
		return (msecs) ;
	}
	else
	{
		/* The (not so) fancy math gives a result which is rounded up.
				Example for 5 msecs_per_tick:
					4 msecs = 1 tick
					5 msecs = 1 tick
					6 msecs = 2 ticks
					etc
		*/
		return (((uint32)(msecs + MSECS_PER_THREADX_TICK - 1) / MSECS_PER_THREADX_TICK ));
	}
}

#if 1 //LM???
uint32 TCTimeGet(void)
{
	// tx time functions returns in timer ticks
	return threadxTicks2Msecs(cyg_current_time());
}

uint32 TCTimeDifference(uint32 endtime, uint32 starttime)
{
	if (endtime >= starttime)
	{
		return (endtime - starttime);
	}
	else // timer wrap-around
	{
		return ((0xFFFFFFFF - starttime) + endtime + 1);
	}
}

uint32 TCTimeElapsed(uint32 starttime)
{
	return TCTimeDifference (TCTimeGet(), starttime);
}

#else

// time functions returns in timer ticks
uint32 TCTimeGet(void)
{
	return cyg_current_time();
}

// time input in timer ticks returns differencs in ms
uint32 TCTimeElapsed(uint32 starttime)
{
	uint32	threadXCurrentTicks = cyg_current_time() ;
	uint32	elapsedTicks ;
	int32	elapsedMsecs ;

	/*  Since OS ticks are unsigned, if start > current then timer has rolled over.
		Also, there is a hard-coded equate here:
			 ULONG == uint32
			 LONG  == int32 */

	if (starttime > threadXCurrentTicks)
	{
		elapsedTicks = (ULONG_MAX - starttime) + threadXCurrentTicks ;
	}
	else
	{
		elapsedTicks = threadXCurrentTicks - starttime ;
	}

	elapsedMsecs = threadxTicks2Msecs (elapsedTicks) ;
	if (elapsedMsecs < 0)
	{
		// some sort of scaling problem likely
		elapsedMsecs = LONG_MAX ;		
	}
	return (elapsedMsecs) ;
}
#endif
