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


static  char	evmSetStdAlone_UsageString[]=\
				"test.stdalone: <sync> <intRate>\n\r"\
				"<sync>    : int | spdif\n\r"\
				"<intRate> : 32k | 44k1 | 48k | 88k2 | 96k | 176.4k | 192k\n\r"\
				"------------------------------------------------------------\n\r"\
				"<intRate> only matters when 'int' sync source is selected\n\r"\
				"";

static	CLIDescriptor	evmSetStdAlone_Descriptor[]=
{
	{kCLI_Name,				(uint32)"stdalone"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&myAppSetStdAlone},
	{kCLI_UsageString, 		(uint32)evmSetStdAlone_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"kk"},					
	{kCLI_ConstantsString,	(uint32)"int=12,adat=5,wc=7,spdif=0,32k=0,44k1=1,48k=2,88k2=3,96k=4,176k4=5,192k=6,norm=0,II=1,IV=2,adda=0,addatos=1,ad=2,adtos=3"},
	{kCLI_ModuleName,		(uint32)"Set stand alone mode command"},
	{kCLI_GroupName,		(uint32)"test"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	evmDumpStdAlone_UsageString[]=\
				"test.dumpstda\n\r"\
				"------------------------------------------------------------\n\r"\
				"Dump the current stand alone setting\n\r"\
				"";

static	CLIDescriptor	evmDumpStdAlone_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dumpstda"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&myAppDumpStdAlone},
	{kCLI_UsageString, 		(uint32)evmDumpStdAlone_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},					
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"Dump stand alone mode command"},
	{kCLI_GroupName,		(uint32)"test"},
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
	hResult += cliInstallCLIDescriptor(evmSetStdAlone_Descriptor);
	hResult += cliInstallCLIDescriptor(evmDumpStdAlone_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing myApp cli tools");
	}
}

#endif //_CLI_TOOL_MYAPP
#endif //_CLI
