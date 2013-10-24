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
#include "display.h"

// Below is a sample CLI descriptor.	SEE THE RULES IN cli.h	See more examples in other module's Cli files.



static HRESULT cliLED (uint32 bit, uint32 val)
{
	amDisplayLed(bit, !val); 
	return NO_ERROR;
}

// My Application's CLI Useage String (help for the end user)
static  char	evmLed_UsageString[]=\
				"evm.led: \n\r"\
				"use: evm.led <bit> <state>\n\r"\
				"<bit>: 0..31, codnrst, phynrst, dspnrst, phantom\n\r"\
				"<state>: on,off\n\r"\
				"------------------------------------------------------------\n\r"\
				"Change a single bit and sends it out through SPI\n\r"\
				"";

static	CLIDescriptor	evmLed_Descriptor[]=
{
	{kCLI_Name,				(uint32)"led"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cliLED},
	{kCLI_UsageString, 		(uint32)evmLed_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ik"},					
	{kCLI_ConstantsString,	(uint32)"codecnrst=12,phynrst=13,dspnrst=14,phantom=15,on=1,off=0"},
	{kCLI_ModuleName,		(uint32)"EVM LED command"},
	{kCLI_GroupName,		(uint32)"evm"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};





static HRESULT cliSpi(uint32 dev, uint32 w)
{
	if (dev==0)
		targetWriteCodecSPIWord (w);
	else if (dev==1)
		targetWriteLEDSPIWord(w);
	else if (dev==2)
	  targetWriteDisplaySPIWord(w);
	else if (dev==3)
	  targetWriteDspSPIWord(w);
	return NO_ERROR;
}


// My Application's CLI Useage String (help for the end user)
static  char	evmSpi_UsageString[]=\
				"evm.spi: \n\r"\
				"use: evm.spi <dst> <word>\n\r"\
				"<dst>: codec,led,disp,dsp\n\r"\
				"------------------------------------------------------------\n\r"\
				"Sends the word through SPI to the Codecs or Display\n\r"\
				"";

static	CLIDescriptor	evmSpi_Descriptor[]=
{
	{kCLI_Name,				(uint32)"spi"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cliSpi},
	{kCLI_UsageString, 		(uint32)evmSpi_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ki"},					
	{kCLI_ConstantsString,	(uint32)"codec=0,led=1,disp=2,dsp=3"},
	{kCLI_ModuleName,		(uint32)"EVM SPI command"},
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
	hResult += cliInstallCLIDescriptor(evmMidi_Descriptor);
	hResult += cliInstallCLIDescriptor(evmLed_Descriptor);
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing myApp cli tools");
	}
}

#endif //_CLI_TOOL_MYAPP
#endif //_CLI
