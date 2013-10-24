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

	Implementation of CLI for the drd module
	
  Revisions:
		created 08/04/2003 LM

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#ifdef _CLI
#include "cliToolDefs.h"
#include "TCTasking.h"
#include "cli.h"
#include "drdCli.h"

enum
{
	DRD_CLI_POWER_ON		= 0,
	DRD_CLI_POWER_STANDBY	= 1,
	DRD_CLI_POWER_STATE		= 2
};

HRESULT drdCliDisplayPowerState(DRD_HOST_POWER_STATE state)
{
	HRESULT			hResult = NO_ERROR;	

	cliOutputLine("");
	cliOutputLine("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
	cliOutputLine("DRD Module");
	cliOutputLine("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
	cliPrintf("HOST_POWER_STATE: %s\n\r", (state == HOST_POWER_ON) ? "HOST_POWER_ON" : "HOST_POWER_STANDBY");
	cliOutputLine("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");

	return hResult;
}

#ifdef _CLI_TOOL_DRD

HRESULT drdCliSetPowerState(uint32 type)
{
	HRESULT					hResult = NO_ERROR;
	DRD_HOST_POWER_STATE	state = HOST_POWER_STANDBY;
	switch (type)
	{
		case DRD_CLI_POWER_ON:
			hResult = drdSetPowerState(HOST_POWER_ON);
			break;

		case DRD_CLI_POWER_STANDBY:
			hResult = drdSetPowerState(HOST_POWER_STANDBY);
			break;
		
		case DRD_CLI_POWER_STATE:
			hResult = drdGetPowerState(&state);
			drdCliDisplayPowerState(state);
			break;
	}

	return hResult;
}

// AV/C Power Command Tool
static  char	drdSetPowerState_UsageString[]=\
				"drd:power: \n\r"\
				"use: drd:power <type>\n\r"\
				"type: on:0, standby:1, state:2\n\r"\
				"------------------------------------------------------------\n\r"\
				"set drd powerstate\n\r";

static	CLIDescriptor	drdSetPowerState_Descriptor[]=
{
	{kCLI_Name,				(uint32)"power"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&drdCliSetPowerState},
	{kCLI_UsageString, 		(uint32)drdSetPowerState_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"set drd powerstate"},
	{kCLI_GroupName,		(uint32)"drd"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void drdCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(drdSetPowerState_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing drd cli tools");
	}
}

#endif //_CLI_TOOL_DRD
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
