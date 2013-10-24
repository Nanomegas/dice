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
 * DESCRIPTION: Connection Management Procedures (CMP) CLI

	This module implements the 
		
		  Connection Management Procedures Command Line Interface

	Revisions:
		created 09/17/2002 lm

****************************************************************************/

#ifndef _CMPCLI_H
#define _CMPCLI_H
#ifdef _CLI

#include "cliToolDefs.h"

#ifdef _CLI_TOOL_CMP

#include "packetBlock.h"
#include "TCTypes.h"
#include "lalDefs.h"

HRESULT		cmpCliiMPRVarCB(uint32 operation, char* varname, uint32	oldValue, uint32* newValue);
HRESULT		cmpClioMPRVarCB(uint32 operation, char* varname, uint32	oldValue, uint32* newValue);
HRESULT		cmpCliiPCR0VarCB(uint32 operation, char* varname, uint32 oldValue, uint32* newValue);
HRESULT		cmpClioPCR0VarCB(uint32 operation, char* varname, uint32 oldValue, uint32* newValue);

void		cmpCliDisplayDataRate(uint32 rate);
void		cmpCliDisplayPlugState(uint32 state);

HRESULT		cmpCliiMPRRead(uint32 iNodeAddr);
HRESULT		cmpClioMPRRead(uint32 oNodeAddr);
HRESULT		cmpCliiPCRRead(uint32 iPCRNumber, uint32 iNodeAddr);
HRESULT		cmpClioPCRRead(uint32 oPCRNumber, uint32 oNodeAddr);
HRESULT		cmpCliiPCRReadAll(uint32 iNodeAddr);
HRESULT		cmpClioPCRReadAll(uint32 oNodeAddr);
HRESULT		cmpCliDisplayInfo(uint32 nodeAddr);

HRESULT		cmpCliBCInConnectionEstablish(uint32 iPCRNumber);
HRESULT		cmpCliBCInConnectionBreak(uint32 iPCRNumber);
HRESULT		cmpBCCliOutConnectionEstablish(uint32 oPCRNumber);
HRESULT		cmpCliBCOutConnectionBreak(uint32 iPCRNumber);
HRESULT		cmpCliP2PInConnectionEstablish(uint32 iPCRNumber, uint32 oPCRNumber, uint16 oNodeAddr);
HRESULT		cmpCliP2PInConnectionBreak(uint32 iPCRNumber, uint32 oPCRNumber, uint16 oNodeAddr);

void		cmpCliInstallTools(void);

#endif //_CLI_TOOL_CMP
#endif //_CLI
#endif //_CMPCLI_H

