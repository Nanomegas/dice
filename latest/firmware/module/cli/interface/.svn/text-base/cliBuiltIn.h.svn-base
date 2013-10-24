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

		CLI Built In Tools
	
  Revisions:
		created 05/01/2002 lm
****************************************************************************/

#ifndef _CLIBUILTIN_H
#define _CLIBUILTIN_H

#ifdef _CLI

#include "TCTypes.h"

typedef HRESULT (*CLI_CALLBACK)(void);

void			cliBuiltInInstallTools(void);
HRESULT			cliBuiltInToolSetSplashCB(CLI_CALLBACK splashCB);
HRESULT			cliBuiltInToolSetResetCB(CLI_CALLBACK resetCB);

HRESULT			cliBuiltInTool_ENV(uint32 argc, char** argv);
HRESULT			cliBuiltInTool_SET(uint32 argc, char** argv);
HRESULT			cliBuiltInTool_GET(uint32 argc, char** argv);
HRESULT			cliBuiltInTool_SPLASH(void);
HRESULT			cliBuiltInTool_DUMP(void* address, uint32 unitsize, uint32 numberofitems, uint32 itemsperline);
HRESULT			cliBuiltInTool_EXIT(void);
HRESULT			cliBuiltInTool_RESET(void);

#endif //_CLI

#endif //_CLIBUILTIN_H

