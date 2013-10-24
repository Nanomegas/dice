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
 * DESCRIPTION: AvcDriver

	Purpose:	AvcDriver Cli
				

	Revisions:
		created 12/12/2005 lm

****************************************************************************/


#ifdef _CLI
#include "cliToolDefs.h"
#include "TCTasking.h"
#include "avcDriver.h"
#include "avcDriverCli.h"
#include "avcDriverInterface.h"
#include "cli.h"

#define AVCDRIVER_SPEEDNAME_ITEMS	5

const char *avcDriverCliSpeedName[AVCDRIVER_SPEEDNAME_ITEMS] =
{
	"S100",
	"S200",
	"S400",
	"reserved",
	"unknown"
};

static const char* avcDriverCliGetSpeedName(uint32 speed)
{
	return avcDriverCliSpeedName[(speed < AVCDRIVER_SPEEDNAME_ITEMS) ? speed : AVCDRIVER_SPEEDNAME_ITEMS - 1];
}

static void avcDriverCliPrintAudioNames(uint32 items, char *names)
{
	uint32	i;
	char	*pc1;
	char	*pc2;

	pc1 = names;
	if (pc1 == '\0') return;

	for (i = 0; i < items; i++)
	{
		pc2 = pc1;
		while (*pc2 != '\\') 
		{
			if (pc2 == '\0') return;
			pc2 += 1;
		}
		cliPrintf("    [%i]: %.*s\n\r", i, (pc2-pc1), pc1);
		pc1 = pc2 + 1;
	}
}

HRESULT	avcDriverCliDump(uint32 mode)
{
	HRESULT					hResult = NO_ERROR;
	AVC_DRIVER_STRUCT*		pAvcDriver = avcDriverGetState();
	uint32					i;

	UNUSED_ARG(mode);

	cliPrintf("\n\ravcDriver GlobalParameters:\n\r");

	cliPrintf("avcDriver state:\n\r");
	cliPrintf("  GlobalParametersOffSet\t: %i\n\r",		pAvcDriver->GlobalParametersOffSet);
	cliPrintf("  GlobalParametersSize\t\t: %i\n\r",		pAvcDriver->GlobalParametersSize);
	cliPrintf("  TxParametersOffSet\t\t: %i\n\r",		pAvcDriver->TxParametersOffSet);
	cliPrintf("  TxParametersSize\t\t: %i\n\r",			pAvcDriver->TxParametersSize);
	cliPrintf("  RxParametersOffSet\t\t: %i\n\r",		pAvcDriver->RxParametersOffSet);
	cliPrintf("  RxParametersSize\t\t: %i\n\r",			pAvcDriver->RxParametersSize);
	cliPrintf("  Dummy1ParametersOffSet\t: %i\n\r",		pAvcDriver->Dummy1ParametersOffSet);
	cliPrintf("  Dummy1ParametersSize\t\t: %i\n\r",		pAvcDriver->Dummy1ParametersSize);
	cliPrintf("  Dummy2UpdateSpaceOffSet\t: %i\n\r",	pAvcDriver->Dummy2UpdateSpaceOffSet);
	cliPrintf("  Dummy2UpdateSpaceSize\t\t: %i\n\r",	pAvcDriver->Dummy2UpdateSpaceSize);

	cliPrintf("\n\ravcDriver GlobalParameters:\n\r");
	cliPrintf("  Owner\t\t\t\t: %08x %08x\n\r",			pAvcDriver->GlobalParameters.Owner.High, pAvcDriver->GlobalParameters.Owner.Low);
	cliPrintf("  Notification\t\t\t: %08x\n\r",			pAvcDriver->GlobalParameters.Notification);
	cliPrintf("  NickName\t\t\t: %s\n\r",				pAvcDriver->GlobalParameters.NickName);
	cliPrintf("  ClockSelect\t\t\t: %i %i\n\r",			(pAvcDriver->GlobalParameters.ClockSelect>>8)&0xff, pAvcDriver->GlobalParameters.ClockSelect&0xff);
	cliPrintf("  Enable\t\t\t: %x\n\r",					pAvcDriver->GlobalParameters.Enable);
	cliPrintf("  Status\t\t\t: %x\n\r",					pAvcDriver->GlobalParameters.Status);
	cliPrintf("  ExtendedStatus\t\t: %x\n\r",			pAvcDriver->GlobalParameters.ExtendedStatus);
	cliPrintf("  SampleRate\t\t\t: %i\n\r",				pAvcDriver->GlobalParameters.SampleRate);
	cliPrintf("  Version\t\t\t: %x\n\r",				pAvcDriver->GlobalParameters.Version);

	cliPrintf("\n\ravcDriver TxParameters:\n\r");
	cliPrintf("  NumIsocTransmitter\t\t: %i\n\r",		pAvcDriver->TxParameters.NumIsocTransmitter);
	cliPrintf("  IsocTransmitterSize\t\t: %i\n\r",		pAvcDriver->TxParameters.IsocTransmitterSize);
	for (i = 0; i < pAvcDriver->TxParameters.NumIsocTransmitter; i++)
	{
		cliPrintf("  IsochTx[%i]\n\r", i);
		cliPrintf("    ChannelNumber\t\t: %i\n\r",		pAvcDriver->TxParameters.IsocTransmitter[i].ChannelNumber);
		cliPrintf("    NumAudioSequences\t\t: %i\n\r",	pAvcDriver->TxParameters.IsocTransmitter[i].NumAudioSequences);
		cliPrintf("    NumMidiSequences\t\t: %i\n\r",	pAvcDriver->TxParameters.IsocTransmitter[i].NumMidiSequences);
		cliPrintf("    Speed\t\t\t: %s\n\r",			avcDriverCliGetSpeedName(pAvcDriver->TxParameters.IsocTransmitter[i].Speed));
		cliPrintf("    Names\n\r");
		avcDriverCliPrintAudioNames(pAvcDriver->TxParameters.IsocTransmitter[i].NumAudioSequences, pAvcDriver->TxParameters.IsocTransmitter[i].Names);
		cliPrintf("    AC3Capabilities\t\t: %i\n\r", pAvcDriver->TxParameters.IsocTransmitter[i].AC3Capabilities);
		cliPrintf("    AC3Enable\t\t\t: %i\n\r",  pAvcDriver->TxParameters.IsocTransmitter[i].AC3Enable);
	}

	cliPrintf("\n\ravcDriver RxParameters:\n\r");
	cliPrintf("  NumIsocReceiver\t\t: %i\n\r",			pAvcDriver->RxParameters.NumIsocReceiver);
	cliPrintf("  IsocReceiverSize\t\t: %i\n\r",			pAvcDriver->RxParameters.IsocReceiverSize);
	for (i = 0; i < pAvcDriver->RxParameters.NumIsocReceiver; i++)
	{
		cliPrintf("  IsochRx[%i]\n\r", i);
		cliPrintf("    ChannelNumber\t\t: %i\n\r",		pAvcDriver->RxParameters.IsocReceiver[i].ChannelNumber);
		cliPrintf("    FirstSequenceNumber\t\t: %i\n\r",pAvcDriver->RxParameters.IsocReceiver[i].FirstSequenceNumber);
		cliPrintf("    NumAudioSequences\t\t: %i\n\r",	pAvcDriver->RxParameters.IsocReceiver[i].NumAudioSequences);
		cliPrintf("    NumMidiSequences\t\t: %i\n\r",	pAvcDriver->RxParameters.IsocReceiver[i].NumMidiSequences);
		cliPrintf("    AC3Capabilities\t\t: %i\n\r", pAvcDriver->RxParameters.IsocReceiver[i].AC3Capabilities);
		cliPrintf("    AC3Enable\t\t\t: %i\n\r",  pAvcDriver->RxParameters.IsocReceiver[i].AC3Enable);
	}

	return hResult;
}


static  char	avcDriverCliDump_UsageString[]=\
				"avcdriver.dump: \n\r"\
				"use: dump\n\r"\
				"------------------------------------------------------------\n\r"\
				"dump avcdriver state\n\r";
static	CLIDescriptor	avcDriverCliDump_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dump"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcDriverCliDump},
	{kCLI_UsageString, 		(uint32)avcDriverCliDump_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"K"},
	{kCLI_ConstantsString,	(uint32)"all=0"},
	{kCLI_ModuleName,		(uint32)"dump avcdriver state"},
	{kCLI_GroupName,		(uint32)"avcdriver"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void avcDriverCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(avcDriverCliDump_Descriptor);
	
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing avs avcDriver tools");
	}
}

#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
