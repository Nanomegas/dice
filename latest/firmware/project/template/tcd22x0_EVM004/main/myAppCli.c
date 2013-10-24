//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2005 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
	
	Purpose:	myAppCli.c - Initialization of OEMs Command Line Interface
	
	Revisions:
		created 02/02/2005 arg

****************************************************************************/
#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_MYAPP
#include "TCTasking.h"
#include "myApp.h"
#include "myAppCli.h"
#include "cli.h"
#include "targetBoard.h"
#include "mymodes.h"




static  char	evmMidi_UsageString[]=\
				"evm.midibp: <on|off>\n\r"\
				"use: evm.midibp\n\r"\
				"<on|off>: If on 1394 MIDI is bypassed internally\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set the MIDI bypass mode\n\r"\
				"";

static	CLIDescriptor	evmMidi_Descriptor[]=
{
	{kCLI_Name,				(uint32)"midibp"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&myModeBypassMidi},
	{kCLI_UsageString, 		(uint32)evmMidi_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},					
	{kCLI_ConstantsString,	(uint32)"on=1,off=0"},
	{kCLI_ModuleName,		(uint32)"EVM Set MIDI bypass mode"},
	{kCLI_GroupName,		(uint32)"evm"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void myAppCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	// This is where the developer will put her CLI tool installs
	// for example:
	//	hResult += cliInstallCLIDescriptor(evmSpi_Descriptor);
	//  ...
	
	hResult += cliInstallCLIDescriptor(evmMidi_Descriptor);
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing myApp cli tools");
	}
}

#endif //_CLI_TOOL_MYAPP
#endif //_CLI
