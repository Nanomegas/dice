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
 * DESCRIPTION: Pioneer DVD Test Module CLI

	This module implements the 
		
	Pioneer DVD Test Module CLI for testing CMP AVC and AKE
		
	Revisions:
		created 07/08/2003 lm

*****************************************************************************/

#ifndef _PIONEERDVDTEST_H
#define _PIONEERDVDTEST_H

#include "packetBlock.h"
#include "TCTypes.h"
#include "lalDefs.h"

HRESULT		pioneerDVDInitialize(void);

#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_PIONEER_DVD

HRESULT		pioneerDVDStart(void);
HRESULT		pioneerDVDStop(BOOL bStopMessage);
HRESULT		pioneerDVDEstablish(void);
HRESULT		pioneerDVDBreak(void);

HRESULT		pioneerDVDGetHandle(LAL_NODE_HANDLE *pHandle);
HRESULT		pioneerDVDGetNodeAddr(uint32 *nodeAddr);
HRESULT		pioneerDVDIsEstablished(BOOL *bEstablished, BYTE *channel, BYTE *plugID);
HRESULT		pioneerDVDSetAsAVCTarget(void);

void		pioneerDVDCliInstallTools(void);

#endif //_CLI_TOOL_PIONEER_DVD
#endif //_CLI
#endif //_PIONEERDVDTEST_H

