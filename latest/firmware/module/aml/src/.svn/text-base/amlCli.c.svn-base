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
	
	Purpose:	Abstract MIDI Layer
  
  Description:       
  
	Revisions:
		created 09/18/2006 zluo

****************************************************************************/
#include "TCTypes.h"

#include "aml.h"
#include "TCSemaphore.h"
#include "TCDebug.h"
#include "diceTimer2.h"

#include "avsAml.h"
#include "uartAml.h"

#ifdef _CLI
#include "cli.h"

static HRESULT amlCliListDev(void)
{
	int i;
	cliPrintf("      NAME         ID        TYPE      CB address   Dest Mon  DrvHndl TxErr\n");	
	for (i = 0; i < AML_DEV_NUM; i++) 
		amlDisplayDev(i);
	return NO_ERROR;
}

static  char	aml_list_UsageString[]=\
				"aml.list: \n\r"\
				"use: list \n\r"\
				"------------------------------------------------------------\n\r"\
				"list current aml device\n\r";
static	CLIDescriptor	aml_list_Descriptor[]=
{
	{kCLI_Name,				(uint32)"list"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&amlCliListDev},
	{kCLI_UsageString, 		(uint32)aml_list_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"list aml device"},
	{kCLI_GroupName,		(uint32)"aml"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static  char	aml_dest_UsageString[]=\
				"aml.dest source_dev_id dest_dev_id\n\r"\
				"------------------------------------------------------------\n\r"\
				"set the destination of a RX device\n\r";
static	CLIDescriptor	aml_dest_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dest"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&amlSetDestination},
	{kCLI_UsageString, 		(uint32)aml_dest_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ii"},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"set destination of an aml device"},
	{kCLI_GroupName,		(uint32)"aml"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT amlSendChar(uint32 midi_dev_id, uint32 c)
{
	return amlAppPutByte(midi_dev_id, c);
}
static  char	aml_put_UsageString[]=\
				"aml.put <dev_id> <char>\n\r"\
				"use: dev_id: device id get from aml.list \n\r"\
				"use: char: charactor to send, in hex format \n\r"\
				"------------------------------------------------------------\n\r"\
				"Send a charactor to a monitor enabled device\n\r";
static	CLIDescriptor	aml_put_Descriptor[]=
{
	{kCLI_Name,				(uint32)"put"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&amlSendChar},
	{kCLI_UsageString, 		(uint32)aml_put_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ii"},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"send a byte to device"},
	{kCLI_GroupName,		(uint32)"aml"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static HRESULT amlGetChar(uint32 midi_dev_id)
{
	HRESULT		hResult;
	uint8		c;
	hResult = amlAppGetByte(midi_dev_id, &c);
	if(hResult == NO_ERROR)
		cliPrintf("0x%x\n", c);
	return hResult;
}

static  char	aml_get_UsageString[]=\
				"aml.get <dev_id>\n\r"\
				"use: dev_id: device id get from aml.list \n\r"\
				"------------------------------------------------------------\n\r"\
				"Get a charactor to a monitor enabled device\n\r";
static	CLIDescriptor	aml_get_Descriptor[]=
{
	{kCLI_Name,				(uint32)"get"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&amlGetChar},
	{kCLI_UsageString, 		(uint32)aml_get_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"Get a byte to device"},
	{kCLI_GroupName,		(uint32)"aml"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};



HRESULT amlCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(aml_list_Descriptor);
	hResult += cliInstallCLIDescriptor(aml_dest_Descriptor);
	hResult += cliInstallCLIDescriptor(aml_get_Descriptor);
	hResult += cliInstallCLIDescriptor(aml_put_Descriptor);
	
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing avs diceDriver tools");
	}
	return hResult;
}

#endif //_CLI
