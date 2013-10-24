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

		lhl CLI Tools and Variables
	
		These are used by the CLI to view lhl info.

	Revisions:
		created 05/14/2002 lm

****************************************************************************/

#ifndef _LHLCLI_H
#define _LHLCLI_H
#ifdef _CLI
#include "cliToolDefs.h"
#include "TCTypes.h"
#include "lhlStatistics.h"

HRESULT		lhlCliPrintTCode(uint32 tCode);			// print tCode
HRESULT		lhlCli1394PrintTCode(uint32 tCode);		// must be called from lhlLLCCliPrintTCode (for handling LLC specific cases)
HRESULT		lhlCliPrintConfCode(uint32 conf);		// print confirmation code
HRESULT		lhlCli1394PrintConfCode(uint32 conf);	// must be called from lhlLLCCliPrintConfCode (for handling LLC specific cases)

#ifdef _CLI_TOOL_LHL

void		lhlCliInstallTools(void);

#endif //_CLI_TOOL_LHL
#endif //_CLI
#endif //_LHLCLI_H
