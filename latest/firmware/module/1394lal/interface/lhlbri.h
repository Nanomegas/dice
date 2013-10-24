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
 * DESCRIPTION: Link Hardware Abstraction Layer (Bus Reset Interface)

	Purpose:	Link Hardware Abstraction Layer 
				Bus Reset Interface helper functions
	
	Revisions:
		created 04/12/2002 lm

****************************************************************************/

#ifndef _LHLBRI_H
#define _LHLBRI_H

#include "TCTypes.h"
#include "bmdefs.h"

HRESULT		lhlBriForceBusReset(void);
HRESULT		lhlBriForceRoot(void);
BOOL		lhlBriIsRoot(void);									// returns TRUE if local node is root

HRESULT		lhlBriGetCycleTime(uint32* time);					// get the current bus cycle time
HRESULT		lhlBriSetCycleMaster(BOOL enabled);					// set Cycle Master function of the Link chip
BOOL		lhlBriIsCycleMaster(void);							// returns TRUE if local node is CM

HRESULT		lhlBriBusResetIndication(void);						// when a bus reset occurs, a number of Link housekeeping actions are performed
HRESULT		lhlBriBusResetInitLink(void);						// Stop transmit and receive DMA controllers
HRESULT		lhlBriGetSelfIDBundle(void);						// Get the self-id bundles (check if self id data)

BOOL		lhlBriInBusReset(void);								// Checking low level bus reset state (in bus reset)
BOOL		lhlBriNewBusResetDetected(BOOL bDuringCriticalBR);	// called during bus reset process (checking for new bus resets and yielding)

HRESULT		lhlBriHandleBusResetPart1(void);					// called when an bus reset process is started (selfId complete)
																// Part1: BR critical part (ends by clearing BR and SelfIdComplete Bits)
HRESULT		lhlBriHandleBusResetPart2(void);					// called when an bus reset process is started (selfId complete)
																// Part2: BR non-critical part (after clearing BR and SelfIdComplete Bits)
BOOL		lhlBriHandlesAreValid(void);						// False after bus reset, TRUE after bus info block scan succeeds

void		lhlBriIntHandler(void);								// called when an bus reset interrupt occures
void		lhlBriSelfIdIntHandler(void);						// called when an selfId Complete interrupt occures
void		lhlBriThread(void *dummy);							// Thread waiting for bus reset process
															
HRESULT		lhlBriCreateSemaphores(void);						// Create semaphores for this module
HRESULT		lhlBriResumeIntHandlers(void);						// Resume interrupt handlers for this module
HRESULT		lhlBriSuspendIntHandlers(void);						// Suspend interrupt handlers for this module
HRESULT		lhlBriResumeTasks(void);							// Resume tasks for this module
HRESULT		lhlBriSuspendTasks(void);							// Suspend tasks for this module
															
HRESULT		lhlBriInitialize(void);								// Create tasks for this module

#endif
