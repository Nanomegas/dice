//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2010 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
	
	Purpose:	eapCli.c - Initialization of EAPs Command Line Interface
	
	Revisions:
		created 03/30/2010 bk

****************************************************************************/
#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_EAP
#include "TCTasking.h"
#include "coreDefs.h"
#include "diceDefs.h"
#include "eapCli.h"
#include "cli.h"

typedef struct
{
	uint32 coeff[16][18];
} DICE_MIXER;

DICE_MIXER *pDiceMixer = (DICE_MIXER *)(DICEBASE + DICE_MIXER_BASE+0x800);

static void dumpMixer(void)
{
	int out, in;

	cliPrintf("Dice mixer coefficients:\n\r");
	cliPrintf("out0    1    2    3    4    5    6    7    8    9   10   11   12   13   14   15\n\r");
	for (out=0; out<18; out++)
	{
		for (in=0; in<16 ;in++)
		{
			cliPrintf("%04x ", pDiceMixer->coeff[out][in]);   
		}
		cliPrintf("\n\r");
	}
}

HRESULT	eapDump_Tool(uint32 dump)
{
	switch (dump)
	{
		case 0: // mixer coeff's
			dumpMixer();
			break;
		default:
			break;
	}

	return NO_ERROR;
}

static  char	Dump_UsageString[]=\
				"dump: \n\r"\
				"use: eap.dump <module>\n\r"\
				"<module>: mixer\n\r"\
				"------------------------------------------------------------\n\r"\
				"Dump register contents for module \n\r"\
				"mixer\n\r";

static	CLIDescriptor	Dump_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dump"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&eapDump_Tool},
	{kCLI_UsageString, 		(uint32)Dump_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"mixer=0"},
	{kCLI_ModuleName,		(uint32)"Dump register contents"},
	{kCLI_GroupName,		(uint32)"eap"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void eapCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;
	
	hResult += cliInstallCLIDescriptor(Dump_Descriptor);
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing EAP cli tools");
	}
}

#endif //_CLI_TOOL_EAP
#endif //_CLI
