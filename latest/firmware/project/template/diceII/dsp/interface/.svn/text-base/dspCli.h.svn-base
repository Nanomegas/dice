/*===================================================================*
  Copyright (c) 2002 TC Applied Technologies Ltd.
  -------------------------------------------------------------------
  $Header: /usr/local/cvsroot/firmware/project/projectEVM/dsp/interface/dspCli.h,v 1.2 2007/02/08 17:59:33 zluo Exp $
 *===================================================================*/ 

 /*********************************************************************
 * DESCRIPTION:

	This module implements the 
		
	DSP Test Module CLI for testing DSP HIO8 interface
		
	Revisions:
		created 12/11/2004 lm

***********************************************************************/

#ifndef _DSPCLI_H
#define _DSPCLI_H

#include "TCTypes.h"

HRESULT		dspCliInitialize(void);

HRESULT		dspCliSetGainValue(uint32 value);
HRESULT		dspCliGetGainValue(uint32 *value);

HRESULT		dspCliSetup(BOOL bLoad);

#ifdef _CLI
//#include "cliToolDefs.h"
#define _CLI_TOOL_DSP

#ifdef _CLI_TOOL_DSP

void		dspCliInstallTools(void);

#endif //_CLI_TOOL_DSP
#endif //_CLI
#endif //_DSPCLI_H

