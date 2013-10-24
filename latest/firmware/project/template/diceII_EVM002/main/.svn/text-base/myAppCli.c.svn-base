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

// Below is a sample CLI descriptor.	SEE THE RULES IN cli.h	See more examples in other module's Cli files.


static void cliSpi(uint32 dev, uint32 w)
{
	if (dev==0)
		targetWriteCodecSPIWord (w);
	else
	{
		w = targetReadCPLDSPIWord (w);
		cliPrintf("Return: 0x%08x\n\r", w);
	}
}

// My Application's CLI Useage String (help for the end user)
static  char	evmSpi_UsageString[]=\
				"evm.spi: \n\r"\
				"use: evm.spi <dst> <word>\n\r"\
				"<dst>: codec,cpld\n\r"\
				"------------------------------------------------------------\n\r"\
				"Sends the word through SPI to the Codecs or CPLD\n\r"\
				"";

static	CLIDescriptor	evmSpi_Descriptor[]=
{
	{kCLI_Name,				(uint32)"spi"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cliSpi},
	{kCLI_UsageString, 		(uint32)evmSpi_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ki"},					
	{kCLI_ConstantsString,	(uint32)"codec=0,cpld=1"},
	{kCLI_ModuleName,		(uint32)"EVM SPI command"},
	{kCLI_GroupName,		(uint32)"evm"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	evmModes_UsageString[]=\
				"evm.modes: \n\r"\
				"use: evm.modes\n\r"\
				"------------------------------------------------------------\n\r"\
				"List the available modes\n\r"\
				"";

static	CLIDescriptor	evmModes_Descriptor[]=
{
	{kCLI_Name,				(uint32)"modes"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&myModesDump},
	{kCLI_UsageString, 		(uint32)evmModes_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},					
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"EVM Modes dump"},
	{kCLI_GroupName,		(uint32)"evm"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	evmMode_UsageString[]=\
				"evm.mode: <mode>\n\r"\
				"use: evm.mode\n\r"\
				"<mode>: id of the mode to set\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set the current config mode\n\r"\
				"";

static	CLIDescriptor	evmMode_Descriptor[]=
{
	{kCLI_Name,				(uint32)"mode"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&myAppChangeConfig},
	{kCLI_UsageString, 		(uint32)evmMode_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},					
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"EVM Set Mode Command"},
	{kCLI_GroupName,		(uint32)"evm"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	evmAC3out_UsageString[]=\
				"evm.ac3out: <isoc> <mask>\n\r"\
				"use: evm.ac3out\n\r"\
				"<isoc>: id stream (0/1)\n\r"\
				"<mask>: mask 16 bit\n\r"\
				"------------------------------------------------------------\n\r"\
				"Configure Mode6/7 AC3 out flags\n\r"\
				"";

static	CLIDescriptor	evmAC3out_Descriptor[]=
{
	{kCLI_Name,				(uint32)"ac3out"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&myAppChangeAC3},
	{kCLI_UsageString, 		(uint32)evmAC3out_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ii"},					
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"EVM change AC3 Out"},
	{kCLI_GroupName,		(uint32)"evm"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};



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
	
	hResult += cliInstallCLIDescriptor(evmSpi_Descriptor);
	hResult += cliInstallCLIDescriptor(evmModes_Descriptor);
	hResult += cliInstallCLIDescriptor(evmMode_Descriptor);
	hResult += cliInstallCLIDescriptor(evmMidi_Descriptor);
	hResult += cliInstallCLIDescriptor(evmAC3out_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing myApp cli tools");
	}
}

#endif //_CLI_TOOL_MYAPP
#endif //_CLI
