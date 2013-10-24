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
  * DESCRIPTION: Sys Debug Command Line Interface

		sys debug CLI Tools and Variables
	
		These are used by the CLI to view sys debug info.

	Revisions:
		created 10/25/2002 lm

****************************************************************************/
#ifdef _SYSDEBUG
#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_SYS
#include "TCTasking.h"
#include "sysDebug.h"
#include "sysCli.h"
#include "cli.h"

/***********************************************************************/
#ifdef _SYSDEBUGPRINTF_LOG
static  char 	sysPrintf_UsageString[]=\
				"sys:logprintf: \n\r"\
				"use: sys:logprintf <on | off>\n\r"\
				"------------------------------------------------------------\n\r"\
				"set sysPrintf logging mode";
static	CLIDescriptor	sysPrintf_Descriptor[]=
{
	{kCLI_Name,				(uint32)"logprintf"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&sysSetDebugPrintfLogEnabled},
	{kCLI_UsageString, 		(uint32)sysPrintf_UsageString},
	{kCLI_ArgumentPatternString,	(uint32)"i"},
	{kCLI_ConstantsString,  (uint32)""},
	{kCLI_ModuleName,		(uint32)"sysPrintf log mode"},
	{kCLI_GroupName,		(uint32)"sys"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};
#endif //_SYSDEBUGPRINTF_LOG

/***********************************************************************/
#ifdef _SYSLOGERROR_LOG
static  char 	sysLogDisp_UsageString[]=\
				"sys:syslog: \n\r"\
				"use: syslog\n\r"\
				"------------------------------------------------------------\n\r"\
				"display all logged sysLogErrors";
static	CLIDescriptor	sysLogDisp_Descriptor[]=
{
	{kCLI_Name,				(uint32)"syslog"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&sysLogErrorLogItemsDisplay},
	{kCLI_UsageString, 		(uint32)sysLogDisp_UsageString},
	{kCLI_ArgumentPatternString,	(uint32)""},
	{kCLI_ConstantsString,  (uint32)""},
	{kCLI_ModuleName,		(uint32)"display sysLogErrors"},
	{kCLI_GroupName,		(uint32)"sys"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/***********************************************************************/
static  char 	sysLogError_UsageString[]=\
				"sys:logmode: \n\r"\
				"use: sys:logmode <on | off>\n\r"\
				"------------------------------------------------------------\n\r"\
				"set sysLogErrors logging mode";
static	CLIDescriptor	sysLogError_Descriptor[]=
{
	{kCLI_Name,				(uint32)"logmode"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&sysSetLogErrorLogEnabled},
	{kCLI_UsageString, 		(uint32)sysLogError_UsageString},
	{kCLI_ArgumentPatternString,	(uint32)"i"},
	{kCLI_ConstantsString,  (uint32)""},
	{kCLI_ModuleName,		(uint32)"sysLogErrors logging mode"},
	{kCLI_GroupName,		(uint32)"sys"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};
#endif //_SYSLOGERROR_LOG


/***********************************************************************/
#ifdef _SYSTIMESTAMP
static  char 	sysTimestamp_UsageString[]=\
				"sys:timestamp: \n\r"\
				"use: sys:timestamp <on | off>\n\r"\
				"------------------------------------------------------------\n\r"\
				"set syslog timestamp mode";
static	CLIDescriptor	sysTimestamp_Descriptor[]=
{
	{kCLI_Name,				(uint32)"timestamp"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&sysSetTimeStampEnabled},
	{kCLI_UsageString, 		(uint32)sysTimestamp_UsageString},
	{kCLI_ArgumentPatternString,	(uint32)"i"},
	{kCLI_ConstantsString,  (uint32)""},
	{kCLI_ModuleName,		(uint32)"syslog timestamp mode"},
	{kCLI_GroupName,		(uint32)"sys"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};
#endif //_SYSTIMESTAMP

/***********************************************************************/
#if defined (_SYSDEBUG) || defined (_SYSDEBUGERROR)
static HRESULT sysDebugSetMask(uint32 mask)
{
	sysDebugSetEnabled(mask);

	return NO_ERROR;
}

static  char	sysSetMask_UsageString[]=\
				"sys:setmask \n\r"\
				"use: sys:setmask <mask>\n\r"\
				"------------------------------------------------------------\n\r"\
				"set debug trace mask";

static	CLIDescriptor	sysSetMask_Descriptor[]=
{
	{kCLI_Name,				(uint32)"setmask"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&sysDebugSetMask},
	{kCLI_UsageString, 		(uint32)sysSetMask_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"set debug trace mask"},
	{kCLI_GroupName,		(uint32)"sys"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/***********************************************************************/
static HRESULT sysDebugGetMask(void)
{
	cliPrintf("mask: 0x%08x\n\r", sysDebugGetEnabled());

	return NO_ERROR;
}

static  char	sysGetMask_UsageString[]=\
				"sys:getmask \n\r"\
				"use: sys:getmask\n\r"\
				"------------------------------------------------------------\n\r"\
				"get debug trace mask";

static	CLIDescriptor	sysGetMask_Descriptor[]=
{
	{kCLI_Name,				(uint32)"getmask"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&sysDebugGetMask},
	{kCLI_UsageString, 		(uint32)sysGetMask_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"get debug trace mask"},
	{kCLI_GroupName,		(uint32)"sys"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/***********************************************************************/
static HRESULT sysDebugIsMask(uint32 mask)
{
	cliPrintf("mask: 0x%08x enabled: %s\n\r", mask, (sysDebugIsEnabled(mask)) ? "TRUE" : "FALSE");

	return NO_ERROR;
}

static  char	sysIsMask_UsageString[]=\
				"sys:ismask \n\r"\
				"use: sys:ismask <mask>\n\r"\
				"------------------------------------------------------------\n\r"\
				"is debug trace mask enabled";

static	CLIDescriptor	sysIsMask_Descriptor[]=
{
	{kCLI_Name,				(uint32)"ismask"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&sysDebugIsMask},
	{kCLI_UsageString, 		(uint32)sysIsMask_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"is debug trace mask enabled"},
	{kCLI_GroupName,		(uint32)"sys"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/***********************************************************************/
static HRESULT sysDebugShowMasks(void)
{
	uint32 index;

	cliPrintf("available debug trace masks\n\r");

	for (index = 0; index < SYSDEBUG_TRACE_ITEMS; index++)
	{
		cliPrintf("%s\n\r", sysDebugGetTraceName(index));
	}

	return NO_ERROR;
}

static  char	sysShowMasks_UsageString[]=\
				"sys:showmasks \n\r"\
				"use: sys:showmasks\n\r"\
				"------------------------------------------------------------\n\r"\
				"show available debug trace masks";

static	CLIDescriptor	sysShowMasks_Descriptor[]=
{
	{kCLI_Name,				(uint32)"showmasks"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&sysDebugShowMasks},
	{kCLI_UsageString, 		(uint32)sysShowMasks_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"show available debug trace masks"},
	{kCLI_GroupName,		(uint32)"sys"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};
#endif //_SYSDEBUG || _SYSDEBUGERROR

/***********************************************************************/
#ifdef _SYS_TRACE
static HRESULT _sysTrace(uint32 command)
{
	switch(command) {
	case 0:
		sysTraceStart();
		break;
	case 1:
		sysTraceStop();
		break;
	case 2:
		sysTraceDump();
		break;
	default:
		break;
	}
	return NO_ERROR;
}

static  char	trace_UsageString[]=\
                   "sys:trace start|stop|dump \n\r";

static	CLIDescriptor	sysTrace_Descriptor[]=
{
	{kCLI_Name,				(uint32)"trace"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&_sysTrace},
	{kCLI_UsageString, 		(uint32)trace_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ConstantsString,	(uint32)"start=0,stop=1,dump=2"},
	{kCLI_ModuleName,		(uint32)"trace functions"},
	{kCLI_GroupName,		(uint32)"sys"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};
#endif //_SYS_TRACE

/***********************************************************************/

void sysCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

#ifdef _SYSTIMESTAMP
	hResult += cliInstallCLIDescriptor(sysTimestamp_Descriptor);
#endif //_SYSTIMESTAMP
#ifdef _SYSDEBUGPRINTF_LOG
	hResult += cliInstallCLIDescriptor(sysPrintf_Descriptor);
#endif //_SYSDEBUGPRINTF_LOG
#ifdef _SYSLOGERROR_LOG
	hResult += cliInstallCLIDescriptor(sysLogDisp_Descriptor);
	hResult += cliInstallCLIDescriptor(sysLogError_Descriptor);
#endif //_SYSLOGERROR_LOG

#if defined (_SYSDEBUG) || defined (_SYSDEBUGERROR)
	hResult += cliInstallCLIDescriptor(sysSetMask_Descriptor);
	hResult += cliInstallCLIDescriptor(sysGetMask_Descriptor);
	hResult += cliInstallCLIDescriptor(sysIsMask_Descriptor);
	hResult += cliInstallCLIDescriptor(sysShowMasks_Descriptor);
#endif //_SYSDEBUG || _SYSDEBUGERROR

#ifdef _SYS_TRACE
	hResult += cliInstallCLIDescriptor(sysTrace_Descriptor);
#endif //_SYS_TRACE

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing sys cli tools");
	}

#if 0 //def _DEBUG
	{
		char debugPCoreDebugStr[64];
		sprintf(debugPCoreDebugStr, "sys:setmask 0xffffffff");
		cliProcessCommand(debugPCoreDebugStr);
	}
#endif //_DEBUG
}

#endif //_CLI_TOOL_SYS
#endif //_CLI
#endif //_SYSDEBUG
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
