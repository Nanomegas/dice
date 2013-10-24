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
	
	Revisions:
		created 04/03/2002 lm

****************************************************************************/

#ifndef _TC_TIME_H_
#define _TC_TIME_H_

#include "TCTypes.h"

/** @addtogroup RTOSTime
	This modules provides functions for obtianing timing information.
	@{
*/

/** Get the current system time.  This is the time in msecs since the system
    was started.
	@return				The time in milliseconds since the system was started.
*/
uint32	TCTimeGet(void);										// (in msecs)

/** Obtain the elapsed time in milliseconds, checking for wrap-around.
	@param starttime	The start time.
	@return				The elapsed time.
*/
uint32	TCTimeElapsed(uint32 starttime);						// (in msecs)

/** Obtain the time difference between a start tiem and an end time, checkinf
    for wrap-around.
	@param endtime		The end time.
	@param starttime	The start time.
	@return				The elapsed time.
*/
uint32	TCTimeDifference(uint32 endtime, uint32 starttime);		// (in msecs)

#ifdef _PROFILE
void	TCTimeGetUSecs(uint32 *msecs, uint32 *usecs);
#endif //_PROFILE

//! @}
#endif
