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
 * DESCRIPTION: CSR Command Line Interface

	csr CLI Tools and Variables

	These are used by the CLI to view csr info.

	Revisions:
		created 10/22/2003 lm
****************************************************************************/

#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_CSR
#include "TCTasking.h"
#include "cli.h"
#include "csr.h"
#include "csrCli.h"
#include "lal.h"

//static char 		moduleName[] = __FILE__ ;

/*
	Configuration ROM CSR Display Tool
*/

static HRESULT csrCliShowCSR(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		offset = CSR_ROM_SPACE_START;
	uint32		i;

	cliPrintf("Config ROM, offset: 0x%08x\n\r", offset);

	for (i = 0; i < csrGetConfigROMQuadletSize(); i++)
	{
		offset = CSR_ROM_SPACE_START + i * 4;
		cliPrintf("0x%03X: 0x%08X\n\r", offset, csrReadConfigROMQuadlet(offset));
	}

	return hResult;
}

static  char	csrCliShowCSR_UsageString[]=\
				"csr:info: \n\r"\
				"use: csr:info\n\r"\
				"------------------------------------------------------------\n\r"\
				"display configuration ROM CSR";
static	CLIDescriptor	csrCliShowCSR_Descriptor[]=
{
	{kCLI_Name,				(uint32)"info"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&csrCliShowCSR},
	{kCLI_UsageString, 		(uint32)csrCliShowCSR_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"display configuration ROM CSR"},
	{kCLI_GroupName,		(uint32)"csr"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/*
	Configuration ROM CSR Update Tool
*/

static HRESULT csrCliUpdateCSR(uint32 index, uint32 value)
{
	HRESULT		hResult = NO_ERROR;
	uint32		offset = CSR_ROM_SPACE_START;
	uint32		i;
	uint32		data[CSR_ROM_MAX_SIZE/4];
	uint32		size = csrGetConfigROMQuadletSize();

	cliPrintf("Config ROM, offset: 0x%08x\n\r", offset);

	for (i = 0; i < size; i++)
	{
		offset = CSR_ROM_SPACE_START + i * 4;
		data[i] = csrReadConfigROMQuadlet(offset);
	}

	data[index] = value;

	hResult = csrUpdateConfigROM(data, size);

	return hResult;
}

static  char	csrCliUpdateCSR_UsageString[]=\
				"csr:update:\n\r"\
				"use: csr:update <index> <data>\n\r"\
				"------------------------------------------------------------\n\r"\
				"update configuration ROM CSR";
static	CLIDescriptor	csrCliUpdateCSR_Descriptor[]=
{
	{kCLI_Name,				(uint32)"update"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&csrCliUpdateCSR},
	{kCLI_UsageString, 		(uint32)csrCliUpdateCSR_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ii"},
	{kCLI_ModuleName,		(uint32)"update configuration ROM CSR"},
	{kCLI_GroupName,		(uint32)"csr"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/*
	Set WWUID Tool
*/
static HRESULT csrCliSetWWUID(uint32 wwuid_hi, uint32 wwuid_low)
{
	HRESULT		hResult = NO_ERROR;
	char		tempstring[40];

	sprintf(tempstring, "writing WWUID %02x %08x", wwuid_hi, wwuid_low);
	cliOutputLine (tempstring);

	lalSetWWUID((uint8)wwuid_hi, wwuid_low);

	return hResult;
}

static  char	csrCliSetWWUID_UsageString[]=\
				"csr:setwwuid: \n\r"\
				"use: csr:setwwuid <hi quad> <lo quad>\n\r"\
				"------------------------------------------------------------\n\r"\
				"set node WWUID";
static	CLIDescriptor	csrCliSetWWUID_Descriptor[]=
{
	{kCLI_Name,				(uint32)"setwwuid"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&csrCliSetWWUID},
	{kCLI_UsageString, 		(uint32)csrCliSetWWUID_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ii"},
	{kCLI_ModuleName,		(uint32)"set node wwuid"},
	{kCLI_GroupName,		(uint32)"csr"},
	{kCLI_Hidden,			0},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void csrCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(csrCliShowCSR_Descriptor);
	hResult += cliInstallCLIDescriptor(csrCliUpdateCSR_Descriptor);
	hResult += cliInstallCLIDescriptor(csrCliSetWWUID_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing csr cli tools");
	}
}

#endif //_CLI_TOOL_CSR
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
