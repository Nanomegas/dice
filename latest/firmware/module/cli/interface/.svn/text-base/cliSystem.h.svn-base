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
 * DESCRIPTION: Command Line Interface

	Purpose: 

		This file describes the Command Line Interpreter (cli) API.
	
  Revisions:
		created 11/14/2002 lm
****************************************************************************/

#ifndef _CLISYSTEM_H
#define _CLISYSTEM_H

#ifdef _CLI

#include "TCTypes.h"

#define CLI_IDLE_MSEC				500

#define CHAR_CR 					0x0d	// return/enter
#define CHAR_LF						0x0a	// linefeed

HRESULT			cliSystemInitializeTasks(void);						// Initialize tasks for this module
HRESULT			cliSystemResumeTasks(void);							// Resume tasks for this module
HRESULT			cliSystemSuspendTasks(void);						// Suspend tasks for this module
void			cliSystemPromptOn(BOOL bPromptOn);					// This function turns on/off the printing of the prompt character
BOOL			cliSystemIsPromptOn(void);							// This function return the printing of the prompt character mode

#endif //_CLI

#endif //_CLISYSTEM_H

