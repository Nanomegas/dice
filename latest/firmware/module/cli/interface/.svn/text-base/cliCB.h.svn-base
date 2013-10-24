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
  * DESCRIPTION: Command Line Interface - CB (for remote CLI Calls)

	Purpose: 

		This file describes the Command Line Interpreter (cli) CB
	
  Revisions:
		created 22/10/2002 lm
****************************************************************************/

#ifndef _CLICB_H
#define _CLICB_H

#ifdef _CLI
#ifdef _CLI_CB

#include "TCTypes.h"
#include "1394defs.h"
#include "packetBlock.h"
#include "lal.h"
#include "cliSIO.h"


#define CLI_CONTROLLER_HREG		0x0						// Register used to set the CLI controller Node Address. Only Compare-Swap locks allowed.
#define CLI_CONTROLLER_LREG		0x4						// Register used to set the CLI controller Node Address. A copy of the CLI_RESP_COUNT is sent to this address.
#define CLI_RESP_CLEAR_REG		0x8						// Register used to clear the response area. Must be written to prior to doing a remote call.
#define CLI_RESP_COUNT_REG		0xC						// Register that records the length of the response, including zero termination. Set to 1 for no reponse.
#define CLI_RESULT_REG			0x10					// The status of the command result code of the last CLI command executed (2 quadlets)
#define CLI_CALL_AREA			0x100					// Write remote call text to this area
#define CLI_RESPONSE_AREA		0x1000					// Area used to hold responses to remote calls

#define	CLI_MAX_MSG_SIZE		0x800					// Maximum length of CLI output buffered
#if 1
#define CLI_CB_BASE_HI			0x0000					// Changed to 0000 from FFFF because I cant make the windows machine generate reads to FFFF	
#else
#define CLI_CB_BASE_HI			0xFFFF
#endif
#define CLI_CB_BASE_START		0xF0020000	
#define CLI_CB_BASE_END			(CLI_CB_BASE_START+CLI_RESPONSE_AREA+CLI_MAX_MSG_SIZE)

extern const OFFSET_1394 cliDestOffsetHi;
extern const OFFSET_1394 cliDestOffsetLo;

//LM??? subscribe as a callback to sysLogError instead
#define _CLICB_SYSLOGERROR
#ifdef _CLICB_SYSLOGERROR
extern void		cliCBSysErrorLogItem(char *text);
#endif //_CLICB_SYSLOGERROR

HRESULT			cliCBNodeVerbose(BOOL bVerbose);
HRESULT			cliCBNodeDeferred(BOOL bDeferred);
HRESULT			cliCBHandleCommand(char *cliCommandStr);

HRESULT			cliCBCallback(PB * incomingPacket);

HRESULT			cliCBInitialize(void);
HRESULT			cliCBInitializeTasks(void);				// Initialize tasks for this module
HRESULT			cliCBResumeTasks(void);					// Resume tasks for this module
HRESULT			cliCBSuspendTasks(void);				// Suspend tasks for this module

#endif //_CLI_CB
#endif //_CLI

#endif //_CLICB_H

