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
 * DESCRIPTION: Node Controller Interface Command Line Interface

		nci CLI Tools and Variables
	
		These are used by the CLI to view nci info.

	Revisions:
		created 05/01/2002 lm
****************************************************************************/

#ifndef _NCICLI_H
#define _NCICLI_H
#ifdef _CLI
#include "cliToolDefs.h"

#include "TCTypes.h"
#include "lalDefs.h"

HRESULT			nciCliGetBusInfo(void);
HRESULT			nciCliGetSpeedTxt(uint32 speed, char *buf);
HRESULT			nciCliGetNodeInfo(uint16 nodeIndex);

#ifdef _CLI_TOOL_NCI
HRESULT			nciCliDumpHandleTable(void);
HRESULT			nciCliDumpAllNodeInfo(void);
HRESULT			nciCliDumpLocalNodeInfo(void);

HRESULT 		nciCliDisplaySpeed(uint32 speed);

HRESULT			nciCliCreateHandle(uint32 wwuid_hi, uint32 wwuid_lo);
HRESULT			nciCliCloseHandle(LAL_NODE_HANDLE handle);

void			nciCliInstallTools(void);

#endif //_CLI_TOOL_NCI
#endif //_CLI
#endif //_NCICLI_H
