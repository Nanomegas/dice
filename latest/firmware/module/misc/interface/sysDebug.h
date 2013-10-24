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
 /***************************************************************************
  * DESCRIPTION: Sys Debug Module

		sys debug Tools for logging debugging info
	
	Revisions:
		created 10/25/2002 lm
*****************************************************************************/

#ifndef _SYSDEBUG_H
#define _SYSDEBUG_H

#include "TCTypes.h"

#define			SYS_MAX_VSPRINTF_BUF_CHARS		128 //longest know line is 72, additional space for timestamp and index counter
#define			SYS_MAX_VSPRINTF_ITEMS			100 //increase if needed
#define			SYS_VSPRINTF_AREAS				10	//items are split into areas for faster debugging

typedef enum
{
	SYSDEBUG_TRACE_NONE			= 0x00000000,
	SYSDEBUG_TRACE_INIT			= 0x00000001,
	SYSDEBUG_TRACE_BUSRESET		= 0x00000002,
	SYSDEBUG_TRACE_WARNINGS		= 0x00000004,
	SYSDEBUG_TRACE_PACKETS		= 0x00000008,
	SYSDEBUG_TRACE_UPPERLAYER	= 0x00000010,
	SYSDEBUG_TRACE_TESTS		= 0x00000020,
	SYSDEBUG_TRACE_LLC			= 0x00000040,
	SYSDEBUG_TRACE_ERRORS		= 0x00000080,
	SYSDEBUG_TRACE_FCP			= 0x00000100,
	SYSDEBUG_TRACE_CMP			= 0x00000200,
	SYSDEBUG_TRACE_AVC			= 0x00000400,
	SYSDEBUG_TRACE_DRD			= 0x00000800,
	SYSDEBUG_TRACE_DTCP			= 0x00001000,
	SYSDEBUG_TRACE_AKE			= 0x00002000,
	SYSDEBUG_TRACE_DAL			= 0x00004000,
	SYSDEBUG_TRACE_AVS			= 0x00008000,
	SYSDEBUG_TRACE_MLAN			= 0x00010000,
	SYSDEBUG_TRACE_FIS			= 0x00020000,
	SYSDEBUG_TRACE_AVC_MUSIC	= 0x00040000,
	SYSDEBUG_TRACE_RCDVD		= 0x00100000,
	SYSDEBUG_TRACE_TCFILE		= 0x00200000,
	SYSDEBUG_TRACE_CLICB		= 0x00400000,
	SYSDEBUG_TRACE_I2C			= 0x00800000,
	SYSDEBUG_TRACE_AML			= 0x01000000,
	SYSDEBUG_TRACE_SPI			= 0x02000000,
	SYSDEBUG_TRACE_SPS			= 0x04000000,
	SYSDEBUG_TRACE_SERIOUS		= 0x08000000,
	SYSDEBUG_TRACE_CRB			= 0x10000000,
	SYSDEBUG_TRACE_GENERAL		= 0x80000000,
	SYSDEBUG_TRACE_ALL			= 0xFFFFFFFF, // update SYSDEBUG_TRACE_ITEMS when adding new

	SYSDEBUG_TRACE_ITEMS		= 31 // !!! number of items above
} SYSDEBUG_TRACE;

#if defined (_SYSDEBUG) || defined (_SYSDEBUGERROR)

#ifdef _SYSTIMESTAMP
HRESULT			sysSetTimeStampEnabled(BOOL bEnabled);
#endif //_SYSTIMESTAMP

#ifdef _SYSDEBUGPRINTF_LOG
void			sysDebugPrintfLogItem(char *text);
void			sysDebugPrintfGetStrCurPos(uint32 *cur, uint32 *pos);
char*			sysDebugPrintfGetStr(uint32 pos);
#endif //_SYSDEBUGPRINTF_LOG

#ifdef _SYSLOGERROR_LOG
void			sysLogErrorLogItem(char *text);
HRESULT			sysLogErrorLogItemsDisplay(void);
#endif //_SYSLOGERROR_LOG

HRESULT			sysSetDebugPrintfLogEnabled(BOOL bEnabled);
HRESULT			sysSetLogErrorLogEnabled(BOOL bEnabled);

BOOL			sysDebugIsEnabled(uint32 mask);
uint32			sysDebugGetEnabled(void);
void			sysDebugSetEnabled(uint32 mask);
const char*		sysDebugGetTraceName(uint32 index);

void			sysGetTimeStr(uint32 time, char *timestr);
void			sysGetCurTimeStr(char *timestr);
void			sysPrintTime(uint32 time);
void			sysPrintElapsedTime(uint32 time);
void			sysPrintTimeDifference(uint32 endtime, uint32 starttime);
void			sysPrintCurTime(void);

HRESULT			sysInitialize(void);
BOOL			sysDebugOrEnabled(uint32 mask);

#endif //_SYSDEBUG || _SYSDEBUGERROR


//##__VA_ARGS__ is a GCC extension, the normal __VA_ARGS__ wont' handle 0 arg
#ifdef _SYSDEBUG
#define SYS_DEBUG(mask, format, ...) \
		if (sysDebugOrEnabled(mask)) {\
			sysDebugPrintf(format, ##__VA_ARGS__); } 
#else 
#define SYS_DEBUG(mask, format, ...) {} 
#endif


/*
	The sysTrace facility is used to record program activities for debugging that are context sensitive 
	and time sensitive. It has a much more accurate timer than RTC (in the resolution of system clocks 
	49.152Mhz), can be used in any context (isr/dsr) and itself has low overhead. It can be conditionally
	compiled in/out by compile flags _SYS_TRACE. The mask field in SYS_TRACe* is the same as in SYS_DEBUG.
*/
#ifndef _SYS_TRACE

#define SYS_TRACE0(mask) {}
#define SYS_TRACE1(mask, arg0) {}
#define SYS_TRACE2(mask, arg0, arg1) {}
#define SYS_TRACE3(mask, arg0, arg1, arg2) {}

#define fSYS_TRACE0(mask) {}
#define fSYS_TRACE1(mask, arg0) {}
#define fSYS_TRACE2(mask, arg0, arg1) {}
#define fSYS_TRACE3(mask, arg0, arg1, arg2) {}

#else

#define TRACE_BUF_SIZE    0x1000

void sysTrace(uint32 mask, char *file_name, uint32 line_number, 
		   uint32 arg_num, uint32 arg0, uint32 arg1, uint32 arg2);

#define SYS_TRACE0(mask)                   sysTrace(mask, (char *)__func__, __LINE__, 0, 0, 0, 0)
#define SYS_TRACE1(mask, arg0)             sysTrace(mask, (char *)__func__, __LINE__, 1, (uint32)arg0, 0, 0)
#define SYS_TRACE2(mask, arg0, arg1)       sysTrace(mask, (char *)__func__, __LINE__, 2, (uint32)arg0, (uint32)arg1, 0)
#define SYS_TRACE3(mask, arg0, arg1, arg2) sysTrace(mask, (char *)__func__, __LINE__, 3, (uint32)arg0, (uint32)arg1, (uint32)arg2)

//create far versions of these using a pointer.
typedef void (*SYS_TRACE_FUNC)(uint32 mask, char *file_name, uint32 line_number, 
		   uint32 arg_num, uint32 arg0, uint32 arg1, uint32 arg2);

extern SYS_TRACE_FUNC fsysTrace;

#define fSYS_TRACE0(mask)                   fsysTrace(mask, (char *)__func__, __LINE__, 0, 0, 0, 0)
#define fSYS_TRACE1(mask, arg0)             fsysTrace(mask, (char *)__func__, __LINE__, 1, (uint32)arg0, 0, 0)
#define fSYS_TRACE2(mask, arg0, arg1)       fsysTrace(mask, (char *)__func__, __LINE__, 2, (uint32)arg0, (uint32)arg1, 0)
#define fSYS_TRACE3(mask, arg0, arg1, arg2) fsysTrace(mask, (char *)__func__, __LINE__, 3, (uint32)arg0, (uint32)arg1, (uint32)arg2)


//cli functions declarations:
void sysTraceStart(void);
void sysTraceStop(void);
void sysTraceDump(void);


#endif //_SYS_TRACE

#endif
