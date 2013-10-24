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
 * DESCRIPTION:
	Purpose:	
	
	Revisions:
		created 04/03/2002 lm

****************************************************************************/

#ifndef _TC_DEBUG_H_
#define _TC_DEBUG_H_

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include "TCTypes.h"
#include "sysDebug.h"

#if 1 //LM??? fflush is added to sysPrintf (just as efficient as printf)
void			sysPrintfSetEnable(BOOL bEnable);
void			sysPrintf(const char * format, ...);
#define			sysDebugPrintf sysPrintf
#define			os_printf sysPrintf
#elif defined (_SYSDEBUGPRINTF_LOG)
HRESULT			sysDebugPrintf(const char * format, ...);
#define			os_printf sysDebugPrintf
#elif defined (_DICE2)
#define			os_printf printf
#define			sysDebugPrintf printf
#else
#define			sysDebugPrintf //
#define			os_printf sysDebugPrintf
#endif

void			sysLogError(HRESULT hResult, int32 errorParm, char *errText);
void			sysLogErrorISR(HRESULT hResult, int32 errorParm,	char *errText);

#endif
