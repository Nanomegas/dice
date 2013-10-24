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
 * DESCRIPTION: DiceDriver

	Purpose:	DiceDriver Cli
				

	Revisions:
		created 12/12/2005 lm

****************************************************************************/


#ifdef _CLI
#include "cliToolDefs.h"
#include "TCTasking.h"
#include "diceDriver.h"
#include "diceDriverCli.h"
#include "diceDriverInterface.h"
#include "cli.h"

#define DICEDRIVER_SPEEDNAME_ITEMS	5

const char *diceDriverCliSpeedName[DICEDRIVER_SPEEDNAME_ITEMS] =
{
	"S100",
	"S200",
	"S400",
	"S800",
	"unknown"
};

static const char* diceDriverCliGetSpeedName(uint32 speed)
{
	return diceDriverCliSpeedName[(speed < DICEDRIVER_SPEEDNAME_ITEMS) ? speed : DICEDRIVER_SPEEDNAME_ITEMS - 1];
}

static void diceDriverCliPrintAudioNames(uint32 items, char *names)
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
		cliPrintf("    [%2i]: \"%.*s\"\n\r", i, (pc2-pc1), pc1);
		pc1 = pc2 + 1;
	}
}

HRESULT	diceDriverCliDump(uint32 mode)
{
	HRESULT					hResult = NO_ERROR;
	DICE_DRIVER_STRUCT*		pDiceDriver = diceDriverGetState();
	uint32					i;

	UNUSED_ARG(mode);

	cliPrintf("diceDriver state:\n\r");
	cliPrintf("  GlobalParametersOffSet\t: %i\n\r",		pDiceDriver->GlobalParametersOffSet);
	cliPrintf("  GlobalParametersSize\t\t: %i\n\r",		pDiceDriver->GlobalParametersSize);
	cliPrintf("  TxParametersOffSet\t\t: %i\n\r",		pDiceDriver->TxParametersOffSet);
	cliPrintf("  TxParametersSize\t\t: %i\n\r",			pDiceDriver->TxParametersSize);
	cliPrintf("  RxParametersOffSet\t\t: %i\n\r",		pDiceDriver->RxParametersOffSet);
	cliPrintf("  RxParametersSize\t\t: %i\n\r",			pDiceDriver->RxParametersSize);
	cliPrintf("  Dummy1ParametersOffSet\t: %i\n\r",		pDiceDriver->Dummy1ParametersOffSet);
	cliPrintf("  Dummy1ParametersSize\t\t: %i\n\r",		pDiceDriver->Dummy1ParametersSize);
	cliPrintf("  Dummy2UpdateSpaceOffSet\t: %i\n\r",	pDiceDriver->Dummy2UpdateSpaceOffSet);
	cliPrintf("  Dummy2UpdateSpaceSize\t\t: %i\n\r",	pDiceDriver->Dummy2UpdateSpaceSize);

	cliPrintf("\n\rGlobalParameters:\n\r");
	cliPrintf("  Owner\t\t\t\t: %08x %08x\n\r",			pDiceDriver->GlobalParameters.Owner.High, pDiceDriver->GlobalParameters.Owner.Low);
	cliPrintf("  Notification\t\t\t: %08x\n\r",			pDiceDriver->GlobalParameters.Notification);
	cliPrintf("  NickName\t\t\t: \"%s\"\n\r",			pDiceDriver->GlobalParameters.NickName);
	cliPrintf("  ClockSelect\t\t\t: %i %i\n\r",			(pDiceDriver->GlobalParameters.ClockSelect>>8)&0xff, pDiceDriver->GlobalParameters.ClockSelect&0xff);
	cliPrintf("  Enable\t\t\t: %x\n\r",					pDiceDriver->GlobalParameters.Enable);
	cliPrintf("  Status\t\t\t: %08x\n\r",				pDiceDriver->GlobalParameters.Status);
	cliPrintf("  ExtendedStatus\t\t: %08x\n\r",			pDiceDriver->GlobalParameters.ExtendedStatus);
	cliPrintf("  SampleRate\t\t\t: %i\n\r",				pDiceDriver->GlobalParameters.SampleRate);
	cliPrintf("  Protocol Version\t\t: %i.%i.%i.%i\n\r",((pDiceDriver->GlobalParameters.Version >> 24) & 0xff),
														((pDiceDriver->GlobalParameters.Version >> 16) & 0xff),
														((pDiceDriver->GlobalParameters.Version >>  8) & 0xff),
														( pDiceDriver->GlobalParameters.Version        & 0xff));
	cliPrintf("  ClockSourceNames\t\t: \"%s\"\n\r",		pDiceDriver->GlobalParameters.ClockSourceNames);
	cliPrintf("  Capabilities\t\t\t: %08x\n\r",			pDiceDriver->GlobalParameters.Capabilities);
	cliPrintf("  MaxFwSpeed\t\t\t: %s\n\r",				diceDriverCliGetSpeedName(pDiceDriver->GlobalParameters.MaxFwSpeed));
	cliPrintf("  TxHwLatency\t\t\t: %i\n\r",			pDiceDriver->GlobalParameters.TxHwLatency);
	cliPrintf("  RxHwLatency\t\t\t: %i\n\r",			pDiceDriver->GlobalParameters.RxHwLatency);
	cliPrintf("  SortID\t\t\t: %i\n\r",					pDiceDriver->GlobalParameters.SortID);

	cliPrintf("\n\rTxParameters:\n\r");
	cliPrintf("  NumIsocTransmitter\t\t: %i\n\r",		pDiceDriver->TxParameters.NumIsocTransmitter);
	cliPrintf("  IsocTransmitterSize\t\t: %i\n\r",		pDiceDriver->TxParameters.IsocTransmitterSize);
	for (i = 0; i < pDiceDriver->TxParameters.NumIsocTransmitter; i++)
	{
		cliPrintf("  IsochTx[%i]\n\r", i);
		cliPrintf("    ChannelNumber\t\t: %i\n\r",		pDiceDriver->TxParameters.IsocTransmitter[i].ChannelNumber);
		cliPrintf("    NumAudioSequences\t\t: %i\n\r",	pDiceDriver->TxParameters.IsocTransmitter[i].NumAudioSequences);
		cliPrintf("    NumMidiSequences\t\t: %i\n\r",	pDiceDriver->TxParameters.IsocTransmitter[i].NumMidiSequences);
		cliPrintf("    Speed\t\t\t: %s\n\r",			diceDriverCliGetSpeedName(pDiceDriver->TxParameters.IsocTransmitter[i].Speed));
		cliPrintf("    Names\n\r");
		diceDriverCliPrintAudioNames(pDiceDriver->TxParameters.IsocTransmitter[i].NumAudioSequences, pDiceDriver->TxParameters.IsocTransmitter[i].Names);
		cliPrintf("    AC3Capabilities\t\t: %08x\n\r",	pDiceDriver->TxParameters.IsocTransmitter[i].AC3Capabilities);
		cliPrintf("    AC3Enable\t\t\t: %08x\n\r",		pDiceDriver->TxParameters.IsocTransmitter[i].AC3Enable);
	}

	cliPrintf("\n\rRxParameters:\n\r");
	cliPrintf("  NumIsocReceiver\t\t: %i\n\r",			pDiceDriver->RxParameters.NumIsocReceiver);
	cliPrintf("  IsocReceiverSize\t\t: %i\n\r",			pDiceDriver->RxParameters.IsocReceiverSize);
	for (i = 0; i < pDiceDriver->RxParameters.NumIsocReceiver; i++)
	{
		cliPrintf("  IsochRx[%i]\n\r", i);
		cliPrintf("    ChannelNumber\t\t: %i\n\r",		pDiceDriver->RxParameters.IsocReceiver[i].ChannelNumber);
		cliPrintf("    FirstSequenceNumber\t\t: %i\n\r",pDiceDriver->RxParameters.IsocReceiver[i].FirstSequenceNumber);
		cliPrintf("    NumAudioSequences\t\t: %i\n\r",	pDiceDriver->RxParameters.IsocReceiver[i].NumAudioSequences);
		cliPrintf("    NumMidiSequences\t\t: %i\n\r",	pDiceDriver->RxParameters.IsocReceiver[i].NumMidiSequences);
		cliPrintf("    Names\n\r");
		diceDriverCliPrintAudioNames(pDiceDriver->RxParameters.IsocReceiver[i].NumAudioSequences, pDiceDriver->RxParameters.IsocReceiver[i].Names);
		cliPrintf("    AC3Capabilities\t\t: %08x\n\r",	pDiceDriver->RxParameters.IsocReceiver[i].AC3Capabilities);
		cliPrintf("    AC3Enable\t\t\t: %08x\n\r",		pDiceDriver->RxParameters.IsocReceiver[i].AC3Enable);
	}

	return hResult;
}


static  char	diceDriverCliDump_UsageString[]=\
				"dicedriver.dump: \n\r"\
				"use: dump <mode>\n\r"\
				"<mode>: all (default)\n\r"\
				"------------------------------------------------------------\n\r"\
				"dump dicedriver state\n\r";
static	CLIDescriptor	diceDriverCliDump_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dump"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&diceDriverCliDump},
	{kCLI_UsageString, 		(uint32)diceDriverCliDump_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"K"},
	{kCLI_ConstantsString,	(uint32)"all=0"},
	{kCLI_ModuleName,		(uint32)"dump dicedriver state"},
	{kCLI_GroupName,		(uint32)"dicedriver"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


HRESULT	diceDriverCliNotify(uint32 notification)
{
	HRESULT					hResult = NO_ERROR;
	diceDriverNotifyHost (notification);
	return hResult;
}

static  char	diceDriverCliNotify_UsageString[]=\
				"dicedriver.notify: \n\r"\
				"use: notify <notification>\n\r"\
				"<notification>: bitmask\n\r"\
				"------------------------------------------------------------\n\r"\
				"send notification to host\n\r";
static	CLIDescriptor	diceDriverCliNotify_Descriptor[]=
{
	{kCLI_Name,				(uint32)"notify"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&diceDriverCliNotify},
	{kCLI_UsageString, 		(uint32)diceDriverCliNotify_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"send host notification"},
	{kCLI_GroupName,		(uint32)"dicedriver"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


HRESULT	diceDriverCliSetLatencies(uint32 input, uint32 output)
{
	diceDriverSetHardwareLatencies(input, output);
	diceDriverNotifyHost(DD_NOTIFY_RX_CFG_CHG_BIT | DD_NOTIFY_TX_CFG_CHG_BIT);
	return NO_ERROR;
}

static  char	diceDriverCliSetLatencies_UsageString[]=\
				"dicedriver.setLatencies: \n\r"\
				"use: setLatencies <in> <out>\n\r"\
				"------------------------------------------------------------\n\r"\
				"set audio in/out latencies and send notification to host\n\r";
static	CLIDescriptor	diceDriverCliSetLatencies_Descriptor[]=
{
	{kCLI_Name,				(uint32)"setLatencies"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&diceDriverCliSetLatencies},
	{kCLI_UsageString, 		(uint32)diceDriverCliSetLatencies_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ii"},
	{kCLI_ModuleName,		(uint32)"set audio latencies"},
	{kCLI_GroupName,		(uint32)"dicedriver"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


HRESULT	diceDriverCliSetSortID(uint32 sortID)
{
	diceDriverSetSortID(sortID);
	diceDriverNotifyHost(DD_NOTIFY_RX_CFG_CHG_BIT | DD_NOTIFY_TX_CFG_CHG_BIT);
	return NO_ERROR;
}

static  char	diceDriverCliSetSortID_UsageString[]=\
				"dicedriver.setSortID: \n\r"\
				"use: setSortID <sortID>\n\r"\
				"------------------------------------------------------------\n\r"\
				"set sort ID and send notification to host\n\r";
static	CLIDescriptor	diceDriverCliSetSortID_Descriptor[]=
{
	{kCLI_Name,				(uint32)"setSortID"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&diceDriverCliSetSortID},
	{kCLI_UsageString, 		(uint32)diceDriverCliSetSortID_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"set sort ID"},
	{kCLI_GroupName,		(uint32)"dicedriver"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


void diceDriverCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(diceDriverCliDump_Descriptor);
	hResult += cliInstallCLIDescriptor(diceDriverCliNotify_Descriptor);
	hResult += cliInstallCLIDescriptor(diceDriverCliSetLatencies_Descriptor);
	hResult += cliInstallCLIDescriptor(diceDriverCliSetSortID_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing diceDriver tools");
	}
}

#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
