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

		Implementation of CLI for the global callback registry.

  Revisions:
		created 21/03/2003 LM

****************************************************************************/

#ifdef _CLI
#include "cliToolDefs.h"
#if 1 //def _CLI_TOOL_AVC_DS
#include "TCTypes.h"
#include "TCDebug.h"
#include "TCTasking.h"
#include "datastream.h"
#include "dsbuffer.h"
#include "cli.h"

//static char 		moduleName[] = __FILE__ ;

#if 0 //def _SYSDEBUG
extern BOOL dataStreamLogging;
#endif //_SYSDEBUG

HRESULT dsCliShow(void)
{
	HRESULT			hResult = NO_ERROR;

#ifdef _LITTLE_ENDIAN
	cliPrintf("_LITTLE_ENDIAN:\t\tdefined\n\r");
#else //_LITTLE_ENDIAN
	cliPrintf("_LITTLE_ENDIAN:\t\tundefined\n\r");
#endif //_LITTLE_ENDIAN

	cliPrintf("dsMODE_PLATFORMENDIAN:\t%i\n\r", dsMODE_PLATFORMENDIAN);
	cliPrintf("dsMODE_NATIVE:\t\t%i\n\r", dsMODE_NATIVE);
	cliPrintf("dsMODE_BIGENDIAN:\t%i\n\r", dsMODE_BIGENDIAN);
	cliPrintf("dsMODE_LITTLEENDIAN:\t%i\n\r", dsMODE_LITTLEENDIAN);

	return hResult;
}

HRESULT dsCliTest(uint32 mode)
{
	HRESULT			hResult = NO_ERROR;
	DataStream		ds;
	uint8			data[4] = {0x12,0x34,0x56,0x78};
	uint32			read[4];
	uint32			i;
	BOOL			bEncode;
#if 0 //def _SYSDEBUG
	BOOL			dataStreamLoggingSafe = dataStreamLogging;

	dataStreamLogging = TRUE;
#endif //_SYSDEBUG

	cliPrintf("dsCliTest\n\r");
	cliPrintf("data:\t\t");
	for (i = 0; i < 4; i++) cliPrintf("0x%02x ", data[i]);
	cliPrintf("\n\r");

	hResult = dsOpenStream(&ds, data, sizeof(data), dsMODE_READ | mode);
	if (hResult != NO_ERROR) return hResult;

	bEncode = FALSE;

	hResult = dsHandleBits(&ds, 4, &read[0], bEncode);
	if (hResult != NO_ERROR) return hResult;
	hResult = dsHandleBits(&ds, 4, &read[1], bEncode);
	if (hResult != NO_ERROR) return hResult;
	hResult = dsHandleBits(&ds, 8, &read[2], bEncode);
	if (hResult != NO_ERROR) return hResult;
	hResult = dsHandleBits(&ds, 16, &read[3], bEncode);
	if (hResult != NO_ERROR) return hResult;

	dsCloseStream(&ds);

#if 0 //def _SYSDEBUG
	dataStreamLogging = dataStreamLoggingSafe;
#endif //_SYSDEBUG

	cliPrintf("dsCliTest\n\r");
	cliPrintf("read:\t\t");
	for (i = 0; i < 4; i++) cliPrintf("0x%08x ", read[i]);
	cliPrintf("\n\r");

	return hResult;
}

// Show Tool
static  char	dsCliShow_UsageString[]=\
				"ds.show: \n\r"\
				"use: ds.show <mode>\n\r"\
				"------------------------------------------------------------\n\r"\
				"show modes info for datastream and dsbuffer";
static	CLIDescriptor	dsCliShow_Descriptor[]=
{
	{kCLI_Name,				(uint32)"show"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&dsCliShow},
	{kCLI_UsageString, 		(uint32)dsCliShow_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"show modes info"},
	{kCLI_GroupName,		(uint32)"ds"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// Test Tool
static  char	dsCliTest_UsageString[]=\
				"ds.test: \n\r"\
				"use: ds.test <mode>\n\r"\
				"------------------------------------------------------------\n\r"\
				"test datastream and dsbuffer";
static	CLIDescriptor	dsCliTest_Descriptor[]=
{
	{kCLI_Name,				(uint32)"test"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&dsCliTest},
	{kCLI_UsageString, 		(uint32)dsCliTest_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"test datastream and dsbuffer"},
	{kCLI_GroupName,		(uint32)"ds"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void dsCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(dsCliShow_Descriptor);
	hResult += cliInstallCLIDescriptor(dsCliTest_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing ds cli tools");
	}
}
#endif //_CLI_TOOL_AVC_DS
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
