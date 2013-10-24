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
		created 05/01/2002 lm
****************************************************************************/

static char 		moduleName[] = __FILE__ ;

#ifdef _CLI

#include "TCTasking.h"
#include "cli.h"
#include "cliBuiltIn.h"
#include "cliSIO.h"
#include "cliSystem.h"

// task (thread) IDs for this module
uint32					cliSystemThreadID = 0;				// cli system thread
const char*				cliSystemThreadName = "cliSystemThread";

BOOL					cliSystembPromptOn = TRUE;

/**********************************************************************
	cliSystemPromptOn

	Turns on/off the printing of the prompt character

 **********************************************************************/
void cliSystemPromptOn(BOOL bPromptOn)
{
	cliSystembPromptOn = bPromptOn;
}

/**********************************************************************
	cliSystemIsPromptOn

	Return the printing of the prompt character mode

 **********************************************************************/
BOOL cliSystemIsPromptOn(void)
{
	return cliSystembPromptOn;
}

//	cliWaitForLine - a helper used to look for data from the serial port. 
static HRESULT cliSystemWaitForCommandLine(char* text, uint32 maxLen)
{
	HRESULT		hResult = NO_ERROR;
	uint32		len = 0;
	char*		pCommandString;
	
	pCommandString = cliSIOWaitForLine();

	len = strlen(pCommandString);
	
	if (len >= maxLen)
	{
		cliPrintf("error: input string too long");
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	strcpy(text, pCommandString);
	
	return hResult;
}

static HRESULT cliSystemPrepareCommandLine(char *command_line, uint32 len)
{
	HRESULT		hResult = NO_ERROR;

	memset(command_line, 0, len);

	if (cliSystemIsPromptOn())
	{
		cliOutputRaw(">");
	}

	return hResult;
}

static HRESULT cliSystemHandleCommandLine(char *command_line)
{
	HRESULT		hResult = NO_ERROR;
	char*		arglist[kMaxArgElements];
	uint32		argcount=0;

	if (strlen(command_line))
	{
		// This function does a split of all the arguments that are in the line.
		// it also does quote matching, so that string arguments are considered one argument,
		// even if the string contains spaces.
		hResult = cliStrToArgs(command_line, arglist, &argcount);
		if (hResult == NO_ERROR)
		{
			hResult = cliProcessArgs(argcount, arglist);
		}
	}

	return hResult;
}

/*********************************************************
	cli system thread
*/
void cliSystemThread(void *dummy)
{
	HRESULT		hResult = NO_ERROR;
	char        command_line[kMaxCommandLine];

	UNUSED_ARG(dummy);

	cliBuiltInTool_SPLASH();

	DO_FOREVER
	{
		cliSystemPrepareCommandLine(command_line, kMaxCommandLine);

		// Check to see if we have any input waiting for us at the serial interface
		hResult = cliSystemWaitForCommandLine(command_line, kMaxCommandLine);
		
		if (hResult == NO_ERROR)
		{
			cliSystemHandleCommandLine(command_line);
		}
	}
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT cliSystemResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// thread cli system
	hResult = TCTaskResume(cliSystemThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT cliSystemSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// thread cli system
	hResult = TCTaskSuspend(cliSystemThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Create tasks for this module
*/
HRESULT cliSystemInitializeTasks(void)
{
	HRESULT hResult = NO_ERROR;
#ifdef _WINMP //LM???
	uint32	priority = TASK_PRIORITY_DEFAULT;
#elif defined (_DICE2) || defined (_DICEJR)
	uint32	priority = TASK_PRIORITY_DEFAULT;
#else
	uint32	priority = TASK_PRIORITY_LOWEST;
#endif

	// thread cli system
	hResult = TCTaskCreate(&cliSystemThreadID, cliSystemThreadName, cliSystemThread, priority, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

#endif //_CLI
