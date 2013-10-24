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
//===========================================================================
/****************************************************************************
	
	Purpose:	AVS interrupt and event thread 
				for handling of events from the AVS system
	
	Revisions:
		created 12/06/2003 kbj


****************************************************************************/

#ifndef _AVSINT_H
#define _AVSINT_H

#include "avsEnvDefs.h"
														// Interrupt event register structure for counters and debug texts
typedef struct
{
	uint32	avsIntEventCopy;							// Copy of avsIntEvent register 
	uint32	numEventItems;								// Number of AVS events for interrupt register
	uint32	*avsIntEvents;								// Pointer to list enabled event bits
	uint32	*avsIntCounter;								// Pointer to array that counts the number of events
	char	***avsIntTexts;								// Pointer to array of texts with events
} AVS_EVENTS;

extern AVS_EVENTS avsIntEventState[NUM_AVS_EVENTS];		// AVS interrupt status array structure

BOOL	avsIntGetPollEnable(void);

HRESULT	avsIntInitialize(void);							// AVS interrupt initialization
HRESULT	avsIntInitializeTasks(void);

void	avsIntClearCounters(void);						// clear all interrupt events counters

HRESULT avsIntSignalSemaphore(void);					// signal avs int event semaphore for this module

HRESULT	avsIntCreateSemaphores(void);
HRESULT	avsIntResumeTasks(void);
HRESULT	avsIntSuspendTasks(void);

#endif
