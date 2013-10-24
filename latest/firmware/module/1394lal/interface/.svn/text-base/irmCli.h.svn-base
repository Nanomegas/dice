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
 * DESCRIPTION: Isochronous Resource Manager (IRM) CLI

	This module implements the 
		
		  Isochronous Resource Manager Command Line Interface

	Revisions:
		created 04/06/2003 lm

****************************************************************************/

#ifndef _IRMCLI_H
#define _IRMCLI_H
#ifdef _IRM
#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_IRM

#include "TCTypes.h"

HRESULT		irmCliAllocateChannelBandwidth(uint32 channel, uint32 bandwidth);
HRESULT		irmCliAllocateChannel(uint32 channel);
HRESULT		irmCliAllocateBandwidth(uint32 bandwidth);
HRESULT		irmCliFreeChannelBandwidth(uint32 channel, uint32 bandwidth);
HRESULT		irmCliFreeChannel(uint32 channel);
HRESULT		irmCliFreeBandwidth(uint32 bandwidth);

HRESULT		irmCliDisplayInfo(void);
HRESULT		irmCliDisplayChannel(void);
HRESULT		irmCliDisplayBandwidth(void);

void		irmCliInstallTools(void);

#endif //_CLI_TOOL_IRM
#endif //_CLI
#endif //_IRM
#endif //_IRMCLI_H

