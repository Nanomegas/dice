//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2010 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
	
	Purpose:	eapCli.h - Initialization of EAPs Command Line Interface
	
	Revisions:
		created 03/30/2010 bk

****************************************************************************/
#ifndef _EAPCLI_H
#define _EAPCLI_H

#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_EAP
void		eapCliInstallTools(void);
#endif //_CLI_TOOL_EAP
#endif //_CLI

#endif //_EAPCLI_H
