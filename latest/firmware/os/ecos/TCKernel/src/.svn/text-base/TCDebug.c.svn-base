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

#include "TCTasking.h"
#include "ErrorCodes.h"
#include "ErrorText.h"

#ifdef _CLI
#include "cliCB.h"
#endif

#ifdef _STATISTICS
#include "lhlStatistics.h"
#endif //_STATISTICS

#ifdef _SYSDEBUGPRINTF_LOG
static char 		moduleName[] = __FILE__ ;
#endif //_SYSDEBUGPRINTF_LOG

//#define _SYSLOGSERROR_ISR 1

static char *sysExtractFilename(char *errText)
{
	char *nameText;
	int len = strlen(errText);

	// Remove path of module name
	nameText = &errText[len];
	while (len > 0)
	{
		nameText--;
		if (*nameText == '\\') // Search for backslash
		{
			nameText++;
			break;
		}
		len--;
	}	

	return nameText;
}

_TC_INLINE void sysLogErrorCommon(HRESULT hResult, int32 errorParm, char *errText, BOOL bISR) _TC_INLINE_ALWAYS;
_TC_INLINE void sysLogErrorCommon(HRESULT hResult, int32 errorParm, char *errText, BOOL bISR)
{
	char tempstring[SYS_MAX_VSPRINTF_BUF_CHARS];
	BOOL bPrintf = TRUE;

	sprintf(tempstring, "%s %#08x %s L%i %s\n\r", (bISR) ? "ERROR_ISR" : "ERROR", (int)hResult, errorText(hResult), 
		(int)errorParm, sysExtractFilename(errText));

	//LM??? subscribe as a callback to sysLogError instead
#ifdef _CLICB_SYSLOGERROR
	cliCBSysErrorLogItem(tempstring);
#endif //_CLICB_SYSLOGERROR

#ifdef _SYSLOGERROR_LOG
	sysLogErrorLogItem(tempstring);
#endif //_SYSLOGERROR_LOG

#ifdef _SYSLOGERROR_ISR
#else //_SYSLOGERROR_ISR
	bPrintf = (bISR != TRUE);
#endif //_SYSLOGERROR_ISR

	if (bPrintf == TRUE)
	{
#ifdef _SYSTIMESTAMP
		sysPrintCurTime();
#endif //_SYSTIMESTAMP
		os_printf(tempstring);
#ifdef _VISUAL_DEBUG_WINDOW
		{
			static char TraceBuffer[1024];
			sprintf(TraceBuffer, "%s(%i): %s %#08x %s\n\r", errText, errorParm, (bISR) ? "ERROR_ISR" : "ERROR", hResult, errorText(hResult));
			OutputDebugString(TraceBuffer);
		}
#endif //_VISUAL_DEBUG_WINDOW
	}
}

void sysLogError(HRESULT hResult, int32 errorParm, char *errText)
{
	sysLogErrorCommon(hResult, errorParm, errText, FALSE);
#ifdef _STATISTICS
	lhlStatistics.sysLogErr++;
#endif //_STATISTICS
}

void sysLogErrorISR(HRESULT hResult, int32 errorParm, char *errText)
{
	sysLogErrorCommon(hResult, errorParm, errText, TRUE);
#ifdef _STATISTICS
	lhlStatistics.sysLogISR++;
#endif //_STATISTICS
}	

static BOOL sysPrintfIsEnabled = TRUE;

void sysPrintfSetEnable(BOOL bEnable)
{
	sysPrintfIsEnabled = bEnable;
}

#if defined(_CLI) || defined (_SYSDEBUG) 
//need to prepare for non console mode
void sysPrintf(const char * format, ...)
{

	if (sysPrintfIsEnabled)
	{
		va_list		argList;

		va_start(argList, format);
		vfnprintf(stdout, INT_MAX, format, argList);
		fflush(stdout);
		va_end(argList);
	}
}
#else
void sysPrintf(const char * format, ...)
{
}
#endif

#ifdef _SYSDEBUGPRINTF_LOG
static char	sysVsprintfBuf[4*SYS_MAX_VSPRINTF_BUF_CHARS]; // just make a larger buffer for safety

HRESULT sysDebugPrintf(const char * format, ...)
{
	HRESULT		hResult = NO_ERROR;
	int16		len = 0;
	va_list		argList;

	va_start(argList, format);
	len = (int16) vsprintf(sysVsprintfBuf, format, argList);
	va_end(argList);

	if (len >= SYS_MAX_VSPRINTF_BUF_CHARS)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

#if 0
	printf(sysVsprintfBuf);
#endif

	sysDebugPrintfLogItem(sysVsprintfBuf); 

	return hResult;
}
#endif //_SYSDEBUGPRINTF_LOG
