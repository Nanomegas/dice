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

		This file describes the Command Line Interpreter (cli) CB Cli module
	
  Revisions:
		created 22/10/2002 lm
****************************************************************************/

#ifndef _CLICBCLI_H
#define _CLICBCLI_H

#include "TCTypes.h"
#include "lal.h"

#ifdef _CLI
#ifdef _CLI_CB

HRESULT			cliCBHandleCall(LAL_NODE_HANDLE handle, char *cliStr);
HRESULT			cliCBNodeIdCall(uint32 nodeId, char *cliStr);
HRESULT			cliCBNodeAddrCall(uint32 nodeAddr, char *cliStr);

void			cliCBCliInstallTools(void);

#endif //_CLI_CB
#endif //_CLI

#endif //_CLICBCLI_H

