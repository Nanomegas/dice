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
 * DESCRIPTION: Link Hardware Abstraction Layer Command Line Interface

		lhl LLC CLI Tools and Variables
	
		These are used by the CLI to view lhl LLC specific info.

	Revisions:
		created 05/14/2002 lm

****************************************************************************/

#ifndef _LHLLLCCLI_H
#define _LHLLLCCLI_H
#ifdef _CLI
#include "cliToolDefs.h"
#include "TCTypes.h"

HRESULT		lhlLLCCliPrintConfCode(uint32 conf);	// for handling LLC specific conf cases
HRESULT		lhlLLCCliPrintTCode(uint32 tCode);		// for handling LLC specific tcode cases

#ifdef _CLI_TOOL_LHL

void		lhlLLCCliInstallTools(void);

#endif //_CLI_TOOL_LHL
#endif //_CLI
#endif //_LHLLLCCLI_H
