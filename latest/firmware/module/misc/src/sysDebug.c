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
  * DESCRIPTION: Sys Debug Module

		sys debug Tools for logging debugging info
	
	Revisions:
		created 10/25/2002 lm
****************************************************************************/

#include "TCTasking.h"
#include "sysDebug.h"
#include "sysCli.h"

#ifdef _DICE_TIMER2
#include "diceTimer2.h"
#endif

#if defined (_SYSDEBUG) || defined (_SYSDEBUGERROR)
HRESULT sysInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

#ifdef _CLI_TOOL_SYS
	sysCliInstallTools();
#endif //_CLI_TOOL_SYS

	return hResult;
}

void sysGetTimeStr(uint32 time, char *timestr)
{
	uint32 hours, mins, secs, msecs;
	
	msecs = time;
	secs = msecs / 1000; 
	msecs = msecs % 1000;
	mins = secs / 60; 
	secs = secs % 60;
	hours = mins / 60; 
	mins = mins % 60;
	
	sprintf(timestr, "[%i.%02i.%02i.%03i]:", hours, mins, secs, msecs);
}

void sysGetCurTimeStr(char *timestr)
{
	sysGetTimeStr(TCTimeGet(), timestr);
}

void sysPrintTime(uint32 time)
{
	char timestr[32];
	sysGetTimeStr(time, timestr);
	sysDebugPrintf(timestr);
}

void sysPrintElapsedTime(uint32 time)
{
	char timestr[32];
	sysGetTimeStr(TCTimeElapsed(time), timestr);
	sysDebugPrintf(timestr);
}

void sysPrintTimeDifference(uint32 endtime, uint32 starttime)
{
	sysPrintTime(TCTimeDifference(endtime, starttime));
}

void sysPrintCurTime(void)
{
	sysPrintTime(TCTimeGet());
}

#ifdef _SYSTIMESTAMP
static BOOL		sysTimeStampEnabled = TRUE;

HRESULT sysSetTimeStampEnabled(BOOL bEnabled)
{
	HRESULT	hResult = NO_ERROR;

	sysTimeStampEnabled = bEnabled;

	return hResult;
}
#endif //_SYSTIMESTAMP
#endif //_SYSDEBUG || _SYSDEBUGERROR

#if defined (_SYSDEBUGPRINTF_LOG) || defined (_SYSLOGERROR_LOG)
static void sysPrintfLog(char *text, uint32 textlen, char *buf, uint32 *cur, uint32 *pos)
{
	uint32	maxlen = 0;
	uint32	restlen = 0;
	uint32	cpylen = 0;
	BOOL	bNewLine = FALSE;

	cpylen = textlen;
	maxlen = SYS_MAX_VSPRINTF_BUF_CHARS - 1; // always space for null character
	restlen = maxlen;
	restlen -= *pos;
	
	if (cpylen > restlen) cpylen = restlen;

	if (*pos == 0)
	{
		memset(buf, 0, SYS_MAX_VSPRINTF_BUF_CHARS);
#ifdef _SYSTIMESTAMP
		if (sysTimeStampEnabled)
		{
			char timestr[32];
			uint32 timeStrLen;

			sysGetCurTimeStr(timestr);
			timeStrLen = strlen(timestr);
			restlen -= timeStrLen;
			if (cpylen > restlen) cpylen = restlen;

			strncpy(buf, timestr, timeStrLen);
			strncat(buf, text, cpylen);
			cpylen += timeStrLen;
		}
#else //_SYSTIMESTAMP
		strncpy(buf, text, cpylen);
#endif //_SYSTIMESTAMP
	}
	else
	{
		strncat(buf, text, cpylen);
	}

	*pos += cpylen;
	if (*pos == maxlen)
	{
		buf[maxlen] = 0;
		bNewLine = TRUE;

	}
	else if ((*pos >= 2) &&
			 (buf[*pos - 2] == '\n') &&
			 (buf[*pos - 1] == '\r')) // only expecting \n\r at very end of buffer
	{
		bNewLine = TRUE;
	}
	if (bNewLine == TRUE)
	{
		*pos = 0;
		*cur = (*cur + 1) % SYS_MAX_VSPRINTF_ITEMS; //wrap-around
	}
}
#endif //_SYSDEBUGPRINTF_LOG || _SYSLOGERROR_LOG

#ifdef _SYSDEBUGPRINTF_LOG
static char		sysDebugPrintfLogBuf[SYS_VSPRINTF_AREAS][SYS_MAX_VSPRINTF_ITEMS / SYS_VSPRINTF_AREAS][SYS_MAX_VSPRINTF_BUF_CHARS]; // faster debugging
static uint32	sysDebugPrintfLogCur;
static uint32	sysDebugPrintfLogPos;
static BOOL		sysDebugPrintfLogEnabled = TRUE;

void sysDebugPrintfLogItem(char *text)
{
	if (sysDebugPrintfLogEnabled)
	{
#if 1
		char*	p = text;
		uint32	textlen = strlen(text);
		uint32	len = 0;
		uint32	i = 0;

		for (i = 0; i < textlen; i++)
		{
			len++;
			if ((i >= 1) &&
				(text[i - 1] == '\n') &&
				(text[i] == '\r'))
			{
				sysPrintfLog(	p, 
							len,
							sysDebugPrintfLogBuf[sysDebugPrintfLogCur / SYS_VSPRINTF_AREAS]
												[sysDebugPrintfLogCur % SYS_VSPRINTF_AREAS], 
							&sysDebugPrintfLogCur, 
							&sysDebugPrintfLogPos);
				p = text + i  + 1;
				len = 0;
			}
		}	
		if (len != 0)
		{
			sysPrintfLog(	p, 
						len,
						sysDebugPrintfLogBuf[sysDebugPrintfLogCur / SYS_VSPRINTF_AREAS]
											[sysDebugPrintfLogCur % SYS_VSPRINTF_AREAS], 
						&sysDebugPrintfLogCur, 
						&sysDebugPrintfLogPos);
		}
#else
		sysPrintfLog(	text, 
					textlen,
					sysDebugPrintfLogBuf[sysDebugPrintfLogCur / SYS_VSPRINTF_AREAS]
										[sysDebugPrintfLogCur % SYS_VSPRINTF_AREAS], 
					&sysDebugPrintfLogCur, 
					&sysDebugPrintfLogPos);
#endif
	}
}

void sysDebugPrintfGetStrCurPos(uint32 *cur, uint32 *pos)
{
	*cur = sysDebugPrintfLogCur;
	*pos = sysDebugPrintfLogPos;
}

char* sysDebugPrintfGetStr(uint32 pos)
{
	if (pos + 1 > SYS_MAX_VSPRINTF_ITEMS)
	{
		pos = 0;
	}
	return sysDebugPrintfLogBuf[pos / SYS_VSPRINTF_AREAS][pos % SYS_VSPRINTF_AREAS];
}

HRESULT sysSetDebugPrintfLogEnabled(BOOL bEnabled)
{
	HRESULT	hResult = NO_ERROR;

	sysDebugPrintfLogEnabled = bEnabled;

	return hResult;
}

#endif //_SYSDEBUGPRINTF_LOG

#ifdef _SYSLOGERROR_LOG
static char		sysLogErrorLogBuf[SYS_VSPRINTF_AREAS][SYS_MAX_VSPRINTF_ITEMS / SYS_VSPRINTF_AREAS][SYS_MAX_VSPRINTF_BUF_CHARS]; // faster debugging
static uint32	sysLogErrorLogCur;
static uint32	sysLogErrorLogPos;
static BOOL		sysLogErrorLogEnabled = TRUE;

void sysLogErrorLogItem(char *text)
{
	if (sysLogErrorLogEnabled)
	{
		sysPrintfLog(	text, 
					strlen(text),
					sysLogErrorLogBuf[sysLogErrorLogCur / SYS_VSPRINTF_AREAS]
									 [sysLogErrorLogCur % SYS_VSPRINTF_AREAS], 
					&sysLogErrorLogCur, 
					&sysLogErrorLogPos);
	}
}

HRESULT sysLogErrorLogItemsDisplay(void)
{
	HRESULT	hResult = NO_ERROR;
	uint32	i;
	uint32	pos;
	char	*text;

	for (i = 0; i < SYS_MAX_VSPRINTF_ITEMS; i++)
	{
		pos = (sysLogErrorLogCur + i) % SYS_MAX_VSPRINTF_ITEMS; //wrap-around
		text = sysLogErrorLogBuf[pos / SYS_VSPRINTF_AREAS][pos % SYS_VSPRINTF_AREAS];
		if (text[0] != 0)
		{
			sysDebugPrintf(text);
		}
	}

	return hResult;
}

HRESULT sysSetLogErrorLogEnabled(BOOL bEnabled)
{
	HRESULT	hResult = NO_ERROR;

	sysLogErrorLogEnabled = bEnabled;

	return hResult;
}
#endif //_SYSLOGERROR_LOG

#if defined (_SYSDEBUG) || defined (_SYSDEBUGERROR)
static uint32 sysDebugEnabledTrace = SYSDEBUG_TRACE_SERIOUS;

static const char sysDebugEnabledTraceNames[SYSDEBUG_TRACE_ITEMS][64] =
{
	"SYSDEBUG_TRACE_NONE         = 0x00000000",
	"SYSDEBUG_TRACE_INIT         = 0x00000001",
	"SYSDEBUG_TRACE_BUSRESET     = 0x00000002",
	"SYSDEBUG_TRACE_WARNINGS     = 0x00000004",
	"SYSDEBUG_TRACE_PACKETS      = 0x00000008",
	"SYSDEBUG_TRACE_UPPERLAYER   = 0x00000010",
	"SYSDEBUG_TRACE_TESTS        = 0x00000020",
	"SYSDEBUG_TRACE_LLC          = 0x00000040",
	"SYSDEBUG_TRACE_ERRORS       = 0x00000080",
	"SYSDEBUG_TRACE_FCP          = 0x00000100",
	"SYSDEBUG_TRACE_CMP          = 0x00000200",
	"SYSDEBUG_TRACE_AVC          = 0x00000400",
	"SYSDEBUG_TRACE_DRD          = 0x00000800",
	"SYSDEBUG_TRACE_DTCP         = 0x00001000",
	"SYSDEBUG_TRACE_AKE          = 0x00002000",
	"SYSDEBUG_TRACE_DAL          = 0x00004000",
	"SYSDEBUG_TRACE_AVS          = 0x00008000",
	"SYSDEBUG_TRACE_MLAN         = 0x00010000",
	"SYSDEBUG_TRACE_FIS          = 0x00020000",
	"SYSDEBUG_TRACE_AVC_MUSIC    = 0x00040000",
	"SYSDEBUG_TRACE_RCDVD        = 0x00100000",
	"SYSDEBUG_TRACE_TCFILE       = 0x00200000",
	"SYSDEBUG_TRACE_CLICB        = 0x00400000",
	"SYSDEBUG_TRACE_I2C          = 0x00800000",
	"SYSDEBUG_TRACE_AML          = 0x01000000",
	"SYSDEBUG_TRACE_SPI          = 0x02000000",
	"SYSDEBUG_TRACE_SPS          = 0x04000000",
	"SYSDEBUG_TRACE_SPS          = 0x04000000",
	"SYSDEBUG_TRACE_CRB          = 0x10000000",
	"SYSDEBUG_TRACE_GENERAL      = 0x80000000",
	"SYSDEBUG_TRACE_ALL          = 0xFFFFFFFF"
};

BOOL sysDebugIsEnabled(uint32 mask)
{
	return ((mask & sysDebugEnabledTrace) == mask);
}

uint32 sysDebugGetEnabled(void)
{
	return sysDebugEnabledTrace;
}

void sysDebugSetEnabled(uint32 mask)
{
	sysDebugEnabledTrace = mask;
}

const char *sysDebugGetTraceName(uint32 index)
{
	if (index >= SYSDEBUG_TRACE_ITEMS)
	{
		index = 0;
	}

	return sysDebugEnabledTraceNames[index];
}

//return true if any flag in mask is turned on
BOOL sysDebugOrEnabled(uint32 mask)
{
	return ((mask & sysDebugEnabledTrace) != 0);
}
#endif //_SYSDEBUG || _SYSDEBUGERROR


#ifdef _SYS_TRACE

typedef struct {
	char *file_name;
	uint32 time_stamp;
	uint16 line_number;
	uint16 entry_size;
	uint32 arg0, arg1, arg2;
} trace_entry_t;

static trace_entry_t trace_buffer[TRACE_BUF_SIZE];
static uint32 trace_buffer_pointer;
//static bool trace_wrap_enable = false;
static bool trace_enabled = true;

void sysTrace(uint32 mask, char *file_name, uint32 line_number, 
		   uint32 arg_num, uint32 arg0, uint32 arg1, uint32 arg2)
{
	if(trace_enabled && (mask & sysDebugEnabledTrace)) { 
		trace_buffer[trace_buffer_pointer].time_stamp = diceTimer2GetLow();
		trace_buffer[trace_buffer_pointer].file_name = file_name;
		trace_buffer[trace_buffer_pointer].line_number = line_number;
		trace_buffer[trace_buffer_pointer].entry_size = arg_num;
		trace_buffer[trace_buffer_pointer].arg0 = arg0;
		trace_buffer[trace_buffer_pointer].arg1 = arg1;
		trace_buffer[trace_buffer_pointer].arg2 = arg2;

		trace_buffer_pointer = (trace_buffer_pointer + 1) % TRACE_BUF_SIZE;
	}
} 

void sysTraceStart(void)
{
	trace_buffer_pointer = 0;
	trace_enabled = true;
#ifndef _DICE_TIMER2 //No timer2 defined, then we have to reset to start 
	diceTimer2Reset();
#endif
	//Otherwise, timer2 is already running and don't disturb.
}

void sysTraceStop(void)
{
	trace_enabled = false;
}

void sysTraceDump(void)
{
	int i;
	
	sysDebugPrintf("trace buffer = %x, #%d of items \n", trace_buffer, trace_buffer_pointer); 
	sysDebugPrintf("   #    Time (us)   File name         ln  arg0     arg1     arg2\n");
	for(i = 0; i < trace_buffer_pointer; i++) {
		sysDebugPrintf("%5d%10d %20s %4d", i, 
			   (i == 0) ? ((trace_buffer[i].time_stamp)/49) :
						  ((trace_buffer[i].time_stamp - trace_buffer[i-1].time_stamp)/49),
			   trace_buffer[i].file_name, 
			   trace_buffer[i].line_number
			   );
		switch (trace_buffer[i].entry_size) {
		case 3:
			sysDebugPrintf(" %08x", trace_buffer[i].arg0);
			sysDebugPrintf(" %08x", trace_buffer[i].arg1); 
			sysDebugPrintf(" %08x", trace_buffer[i].arg2); 
			break;
		case 2:
			sysDebugPrintf(" %08x", trace_buffer[i].arg0);
			sysDebugPrintf(" %08x", trace_buffer[i].arg1); 
			break;
		case 1:
			sysDebugPrintf(" %08x", trace_buffer[i].arg0);
			break;
		default:
			break;
		}
		sysDebugPrintf("\n");
	}
}

SYS_TRACE_FUNC fsysTrace = &sysTrace;

#endif // _SYS_TRACE
