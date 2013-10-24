//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2005 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
	
	Purpose:	myAppCli.h - Initialization of OEMs Command Line Interface
	
	Revisions:
		created 02/02/2005 arg

****************************************************************************/
#ifndef _MYAPPCLI_H
#define _MYAPPCLI_H

#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_MYAPP

void		myAppCliInstallTools(void);
#endif //_CLI_TOOL_MYAPP
#endif //_CLI
#endif //_MYAPPCLI_H
