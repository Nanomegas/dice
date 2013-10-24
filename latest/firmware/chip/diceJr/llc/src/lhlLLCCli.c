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
 * DESCRIPTION: Link Hardware Abstraction Layer Command Line Interface

		lhl LLC CLI Tools and Variables
	
		These are used by the CLI to view lhl LLC specific info.

	Revisions:
		created 12/17/2002 kbj

****************************************************************************/

#ifdef _CLI
#include "TCTasking.h"
#include "lhl.h"
#include "lhlCli.h"
#include "lhlLLCCli.h"
#include "lhlLLCDefs.h"
#include "llcInt.h"
#include "cli.h"

//static char 		moduleName[] = __FILE__ ;

HRESULT lhlLLCCliPrintConfCode(uint32 conf)	// for handling LLC specific conf cases
{
	HRESULT		hResult = NO_ERROR;

	switch (conf)
	{
		/* DMA ContextControl.event register values */

		case LLC_EVENT_INIT:
			cliPrintf("terminate (ack) bcast etc.\n\r");
			break;

		case LLC_EVT_TCODE_ERR:
			cliPrintf("event tCode error\n\r");
			break;

		case LLC_ACK_MISSING_ERR:
			cliPrintf("event ack missing\n\r");
			break;

		case LLC_EVT_UNDERRUN:
			cliPrintf("event underrun\n\r");
			break;

		case LLC_EVT_PAR_ERROR:
			cliPrintf("event parity error\n\r");
			break;

		case LLC_EVT_RETRY_FAIL:
			cliPrintf("event retry fail\n\r");
			break;

		case LLC_EVT_ABORT_RTRY:
			cliPrintf("event abort retry\n\r");
			break;

		case LLC_EVT_FLUSHED:
			cliPrintf("event flushed (Bus reset)\n\r");
			break;

		default:
			hResult = lhlCli1394PrintConfCode(conf); // general conf codes
			break;
	}

	return hResult;
}

HRESULT lhlLLCCliPrintTCode(uint32 tCode)	// for handling LLC specific tcode cases
{
	HRESULT		hResult = NO_ERROR;

	switch (tCode)
	{
		case TCODE_PHY_PACKET:
			cliPrintf("phy packet");
			break;

		default:
			hResult = lhlCli1394PrintTCode(tCode); // general tcodes
			break;
	}

	return hResult;
}

#ifdef _CLI_TOOL_LHL

#define MODE_DUMP				0x0
#define MODE_DUMP_IFSET			0x1
#define MODE_DUMP_IFENABLE		0x2
#define MODE_CLEAR				0x3

HRESULT	lhlCliIntLogDump(uint32 mode)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i;
	uint32		count = 0;
	BOOL		bEnable = FALSE;

	count = llcIntEventGetIntCounts();
	cliPrintf("IntLog Total Events   [%2i]: %8i Enabled\n\r", LLC_INT_ITEMS, count);
	for (i = 0; i < LLC_INT_ITEMS; i++)
	{
		count = llcIntEventGetIntCount(i);
		bEnable = llcIntEventIsIntEnabled(i);

		if ((mode == MODE_DUMP) ||
			(mode == MODE_DUMP_IFSET && count) ||
			(mode == MODE_DUMP_IFENABLE && bEnable))
		{
			cliPrintf("%s[%2i]: %8i %s\n\r", 
				llcIntEventGetIntName(i), 
				i, 
				count, 
				(bEnable) ? "+" : " ");
		}
	}

	return hResult;
}

HRESULT	lhlCliIntLogClear(void)
{
	HRESULT		hResult = NO_ERROR;

	llcIntEventClearIntCounts();

	return hResult;
}

HRESULT	lhlCliIntLog(uint32 mode)
{
	HRESULT		hResult = NO_ERROR;

	switch(mode)
	{
		default:
		case MODE_DUMP:
		case MODE_DUMP_IFSET:
		case MODE_DUMP_IFENABLE:
			hResult = lhlCliIntLogDump(mode);
			break;
		case MODE_CLEAR:
			hResult = lhlCliIntLogClear();
			break;
	}

	return hResult;
}

static  char	lhlCliIntLog_UsageString[]=\
				"lhl.intlog: \n\r"\
				"use: lhl.intlog <mode>\n\r"\
				"<mode>: dump,dumpifset,dumpifenable,clear\n\r"\
				"------------------------------------------------------------\n\r"\
				"lhl interrupt log";

static	CLIDescriptor	lhlCliIntLog_Descriptor[]=
{
	{kCLI_Name,				(uint32)"intlog"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lhlCliIntLog},
	{kCLI_UsageString, 		(uint32)lhlCliIntLog_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"dump=0x0,dumpifset=0x1,dumpifenable=0x2,clear=0x3"},
	{kCLI_ModuleName,		(uint32)"lhl interrupt log"},
	{kCLI_GroupName,		(uint32)"lhl"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void lhlLLCCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(lhlCliIntLog_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing lhl LLC cli tools");
	}
}

#endif //_CLI_TOOL_LHL
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
