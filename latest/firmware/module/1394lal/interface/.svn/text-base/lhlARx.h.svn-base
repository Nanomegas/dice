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
 * DESCRIPTION: Link Hardware Abstraction Layer receive packet i/o

	Purpose:	Link Hardware Abstraction Layer receive packet i/o
						handling for asynchronous receive packets
	
	Revisions:
		created 04/03/2002 lm
****************************************************************************/

#ifndef _LHLARX_H
#define _LHLARX_H

#include "tctypes.h"
#include "packetBlock.h"


HRESULT		lhlRxCreateSemaphores(void);
HRESULT		lhlRxResumeIntHandlers(void);	// resume interrupt handlers for this module
HRESULT		lhlRxSuspendIntHandlers(void);	// suspend interrupt handlers for this module

HRESULT		lhlRxResumeTasks(void);			// resume tasks for this module
HRESULT		lhlRxSuspendTasks(void);		// suspend tasks for this module

HRESULT		lhlRxInitialize(void);			// create tasks (interrupt handlers, threads and semaphores) for this module

HRESULT		lhlRxDiscardTlabelAbove (uint32 tlabel); // The system will discard write response packets with tlable
																									 // above this number at interrupt level.

#endif
