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
 * DESCRIPTION: Link Hardware Abstraction Layer service handling
				of IntEvent interrupts

	Purpose:	Handling of errors signaled from the LLC
	
	Revisions:
		created 05/08/2002 kbj

****************************************************************************/

#ifndef _LHLSERVICE_H
#define _LHLSERVICE_H

#include "TCTypes.h"
#include "bmdefs.h"

HRESULT lhlServiceInitialize(void);				// Create tasks for this module, interrupt handlers and sema.

HRESULT lhlServiceResumeTasks(void);			// Resume tasks for this module
HRESULT lhlServiceSuspendTasks(void);			// Suspend tasks for this module
HRESULT lhlServiceResumeIntHandlers(void);		// Resume interrupt handlers for this module
HRESULT lhlServiceSuspendIntHandlers(void);		// Suspend interrupt handlers for this module

HRESULT lhlServiceResumeAllIntHandlers(void);	// Resume all link layer interrupt handlers
HRESULT lhlServiceSuspendAllIntHandlers(void);	// Suspend all link layer interrupt handlers

HRESULT lhlServiceDisableLinkController(void);	// Disabling of Link Layer controller
HRESULT lhlServiceResetLinkController(void);	// Make software reset of Link Layer controller

HRESULT lhlServiceCreateSemaphores(void);
void	lhlServiceThread(void *dummy);

// LED Helper Functions
BOOL	lhlServiceInToggleBRLed(void);
void	lhlServiceEnterToggleBRLed(void);
void	lhlServiceLeaveToggleBRLed(void);
void	lhlServiceToggleBRLed(void);
void	lhlServiceToggleCycleLed(void);
void	lhlServiceToggleLedMode(void);
void	lhlServiceSetLedMode(BOOL on);

#endif
