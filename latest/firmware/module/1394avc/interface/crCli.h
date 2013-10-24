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

 DESCRIPTION: 

		Implementation of CLI for a global callback registry.
		
  Revisions:
		created 21/03/2003 LM

****************************************************************************/
#ifndef _CRCLI_H
#define _CRCLI_H
#ifdef _CLI

#include "cliToolDefs.h"
#include "keyValue.h"

#ifdef _CLI_TOOL_AVC_CR

HRESULT crCliList(void);
HRESULT crCliShow(KEYVALUE* descriptor);

void	crCliInstallTools(void);

#endif //_CLI_TOOL_AVC_CR
#endif	//_CLI
#endif	//_CRCLI_H

