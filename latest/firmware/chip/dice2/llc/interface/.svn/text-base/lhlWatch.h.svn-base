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
 * DESCRIPTION: Link Hardware Abstraction Layer (Watching system)

	Purpose:	Link Hardware Abstraction Layer 
				Watch System helper functions
	
	Revisions:
		created 04/12/2002 lm

****************************************************************************/

#ifndef _LHLWATCH_H
#define _LHLWATCH_H

#include "TCTypes.h"
#include "bmdefs.h"

void		lhlWatchSetProcessEnabled(BOOL bEnabled);

void		lhlWatchThread(void *dummy);						// Thread watching overall system
															
HRESULT		lhlWatchResumeTasks(void);							// Resume tasks for this module
HRESULT		lhlWatchSuspendTasks(void);							// Suspend tasks for this module
															
HRESULT		lhlWatchInitialize(void);							// Create tasks for this module
RCODE_1394	lhlWatchTransRead(uint32 offset, uint32 *readData);  // Read non-irm serial bus csr register
RCODE_1394	lhlWatchTransWrite(uint32 offset, uint32 writeData);// Write non-irm serial bus csr register

#endif
