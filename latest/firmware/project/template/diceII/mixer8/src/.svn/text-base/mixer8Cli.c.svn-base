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
   * DESCRIPTION:

	Purpose: 

		Implementation of CLI for mixer8
	
  Revisions:
		created 09/03/2005 alia
****************************************************************************/

#ifdef _MIXER8
#ifdef _CLI

#include "cliToolDefs.h"
#define _CLI_TOOL_MIXER8

#ifdef _CLI_TOOL_MIXER8
#include "TCTasking.h"
#include "TCInterrupt.h"
#include "cli.h"
#include "mixer8.h"
#include "mixer8Cli.h"

static HRESULT mixer8GetMaster_Tool(uint32 reg)
{
	HRESULT		hResult = NO_ERROR;
	
	switch(	reg )
	{
	case 0:
		cliPrintf("mixer8.MasterEnable = %08X \n\r", mixer8.Enable);
		break;
	case 1:
		cliPrintf("mixer8.MasterCapabilities = %08X \n\r", mixer8.SupportedConfiguration);
		break;
	case 2:
		cliPrintf("mixer8.CurrentConfiguration = %08X \n\r", mixer8.CurrentConfiguration);
		break;
	case 3:
		cliPrintf("mixer8.MasterGain = %08X \n\r", mixer8.MasterGain);
		break;
	case 4:
		cliPrintf("mixer8.MasterMute = %08X \n\r", mixer8.MasterMute);
		break;
	case 5:
		cliPrintf("mixer8.MasterPan = %08X \n\r", mixer8.MasterPan);
		break;
	}	
	return hResult;
}

static  char	mixer8GetMaster_UsageString[]=\
				"Get: \n\r"\
				"use: mixer8.getmaster <register>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Get register contents for master: \n\r"\
				"<register>: enable, cap, curconf \n\r";
								
static	CLIDescriptor	mixer8GetMaster_Descriptor[]=
{
	{kCLI_Name,				(uint32)"getmaster"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&mixer8GetMaster_Tool},
	{kCLI_UsageString, 		(uint32)mixer8GetMaster_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"enable=0,cap=1,curconf=2,gain=3,mute=4,pan=5"},
	{kCLI_ModuleName,		(uint32)"get master register contents"},
	{kCLI_GroupName,		(uint32)"mixer8"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT mixer8SetMaster_Tool(uint32 reg, uint32 value)
{
	HRESULT		hResult = NO_ERROR;
	switch(	reg)
	{
	case 0:
		mixer8.Enable = value;			
		break;
	case 1:
		if(mixer8CheckValidConfiguration(value))
			mixer8.CurrentConfiguration = value;
		else
			hResult = E_BAD_INPUT_PARAMETERS;
		break;
	case 2:
		mixer8.MasterGain = value;
		break;
	case 3:
		mixer8.MasterMute = value;
		break;
	case 4:
		mixer8.MasterPan = value;
		break;
	}
	TCSemaphoreSignal(mixer8SemID);
	return hResult;
}
static  char	mixer8SetMaster_UsageString[]=\
				"Get: \n\r"\
				"use: mixer8.setmaster <register> <value>\n\r"\
				"------------------------------------------------------------\n\r"\
				"<register>: enable, curconf\n\r";
								
static	CLIDescriptor	mixer8SetMaster_Descriptor[]=
{
	{kCLI_Name,				(uint32)"setmaster"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&mixer8SetMaster_Tool},
	{kCLI_UsageString, 		(uint32)mixer8SetMaster_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ki"},
	{kCLI_ConstantsString,	(uint32)"enable=0,curconf=1,gain=2,mute=3,pan=4"},
	{kCLI_ModuleName,		(uint32)"set master register contents"},
	{kCLI_GroupName,		(uint32)"mixer8"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT mixer8GetChannel_Tool(uint32 index, uint32 reg)
{
	HRESULT		hResult = NO_ERROR;
	
	if( (index < 1) || (index > 8 ) )
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		return hResult;
	}
	index--;
	switch(	reg )
	{
		case 0:
			cliPrintf("mixer8.Channel[%d].Mute = %08X \n\r", index, mixer8.ChannelParameters[index].Mute);
		break;
		case 1:
			cliPrintf("mixer8.Channel[%d].Solo = %08X \n\r", index, mixer8.ChannelParameters[index].Solo);
		break;
		case 2:
			cliPrintf("mixer8.Channel[%d].Fader = %08X \n\r", index, mixer8.ChannelParameters[index].Fader);
		break;
		case 3:
			cliPrintf("mixer8.Channel[%d].Pan = %08X \n\r", index, mixer8.ChannelParameters[index].Pan);
		break;
		case 4:
			cliPrintf("mixer8.Channel[%d].Coupled = %08X \n\r", index, mixer8.ChannelParameters[index].Coupled);
		break;
	}	
	return hResult;
}

static  char	mixer8GetChannel_UsageString[]=\
				"Get: \n\r"\
				"use: mixer8.getchan <index> <register>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Get register contents for channel registers: \n\r"\
				"<register>: mute, solo, fader, pan, coupled\n\r";
								
static	CLIDescriptor	mixer8GetChannel_Descriptor[]=
{
	{kCLI_Name,				(uint32)"getchan"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&mixer8GetChannel_Tool},
	{kCLI_UsageString, 		(uint32)mixer8GetChannel_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ik"},
	{kCLI_ConstantsString,	(uint32)"mute=0,solo=1,fader=2,pan=3,coupled=5"},
	{kCLI_ModuleName,		(uint32)"get channel register contents"},
	{kCLI_GroupName,		(uint32)"mixer8"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT mixer8SetChannel_Tool(uint32 index, uint32 reg, uint32 value)
{
	HRESULT		hResult = NO_ERROR;
	
	if( (index < 1) || (index > 8 ) )
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		return hResult;
	}
	else
		index--;
	switch(	reg )
	{
		case 0:
			mixer8.ChannelParameters[index].Mute = value;
		break;
		case 1:
			mixer8.ChannelParameters[index].Solo = value;
		break;
		case 2:
			mixer8.ChannelParameters[index].Fader = value;
		break;
		case 3:
			mixer8.ChannelParameters[index].Pan = value;
		break;
		case 4:
			mixer8.ChannelParameters[index].Coupled = value;
		break;
	}
	TCSemaphoreSignal(mixer8SemID);
	return hResult;
}

static  char	mixer8SetChannel_UsageString[]=\
				"Get: \n\r"\
				"use: mixer8.setchan <index> <register> <value>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set register contents for channel registers: \n\r"\
				"<register>: mute, solo, fader, pan, coupled\n\r";
												
static	CLIDescriptor	mixer8SetChannel_Descriptor[]=
{
	{kCLI_Name,				(uint32)"setchan"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&mixer8SetChannel_Tool},
	{kCLI_UsageString, 		(uint32)mixer8SetChannel_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iki"},
	{kCLI_ConstantsString,	(uint32)"mute=0,solo=1,fader=2,pan=3,coupled=5"},
	{kCLI_ModuleName,		(uint32)"set channel register contents"},
	{kCLI_GroupName,		(uint32)"mixer8"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT mixer8GetAux_Tool(uint32 ch_index, uint32 aux_index, uint32 reg)
{
	HRESULT		hResult = NO_ERROR;
	
	if( (ch_index < 1) || (ch_index > 8 )  || (aux_index < 1) || (aux_index > 6 ) )
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		return hResult;
	}
	ch_index--;
	aux_index--;
	switch(reg)
	{
		case 0:
			cliPrintf("mixer8.Channel[%d].Aux[%d].On = %08X \n\r",
				ch_index, aux_index, mixer8.ChannelParameters[ch_index].Aux[aux_index].On);
		break;
		case 1:
			cliPrintf("mixer8.Channel[%d].Aux[%d].PrePost = %08X \n\r",
				ch_index, aux_index, mixer8.ChannelParameters[ch_index].Aux[aux_index].PrePost);
		break;
		case 2:
			cliPrintf("mixer8.Channel[%d].Aux[%d].Value = %08X \n\r",
				ch_index, aux_index, mixer8.ChannelParameters[ch_index].Aux[aux_index].Value);
		break;
	}
	return hResult;
}

static  char	mixer8GetAux_UsageString[]=\
				"Get: \n\r"\
				"use: mixer8.getaux <chindex> <auxindex> <register>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Get register contents for aux registers: \n\r"\
				"<register>: on, pre, value\n\r";
								
static	CLIDescriptor	mixer8GetAux_Descriptor[]=
{
	{kCLI_Name,				(uint32)"getaux"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&mixer8GetAux_Tool},
	{kCLI_UsageString, 		(uint32)mixer8GetAux_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iik"},
	{kCLI_ConstantsString,	(uint32)"on=0,pre=1,value=2"},
	{kCLI_ModuleName,		(uint32)"get aux register contents"},
	{kCLI_GroupName,		(uint32)"mixer8"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT mixer8SetAux_Tool(uint32 ch_index, uint32 aux_index, uint32 reg, uint32 value)
{
	HRESULT		hResult = NO_ERROR;
	
	if( (ch_index < 1) || (ch_index > 8 )  || (aux_index < 1) || (aux_index > 6 ) )
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		return hResult;
	}
	ch_index--;
	aux_index--;
	switch (reg)
	{
		case 0:
			mixer8.ChannelParameters[ch_index].Aux[aux_index].On = value;
		break;
		case 1:
			mixer8.ChannelParameters[ch_index].Aux[aux_index].PrePost = value;
		break;
		case 2:
			mixer8.ChannelParameters[ch_index].Aux[aux_index].Value = value;
		break;
	}
	TCSemaphoreSignal(mixer8SemID);
	return hResult;
}

static  char	mixer8SetAux_UsageString[]=\
				"Set: \n\r"\
				"use: mixer8.setaux <chindex> <auxindex> <register> <value>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set register contents for aux registers: \n\r"\
				"<register>: on, pre, value\n\r";
								
static	CLIDescriptor	mixer8SetAux_Descriptor[]=
{
	{kCLI_Name,				(uint32)"setaux"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&mixer8SetAux_Tool},
	{kCLI_UsageString, 		(uint32)mixer8SetAux_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iiki"},
	{kCLI_ConstantsString,	(uint32)"on=0,pre=1,value=2"},
	{kCLI_ModuleName,		(uint32)"set aux register contents"},
	{kCLI_GroupName,		(uint32)"mixer8"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT mixer8SetCoef_Tool(uint32 index, uint32 value)
{
	HRESULT		hResult = NO_ERROR;
	
	if( (index < 1) || (index > 64 ) )
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		return hResult;
	}

	mixer8.ShadowCoeffs[index-1] = value;
	TCSemaphoreSignal(mixer8SemID);
	return hResult;
}

static  char	mixer8SetCoef_UsageString[]=\
				"Set: \n\r"\
				"use: mixer8.setcoef <index> <value>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set mixer coefs directly: \n\r"\
				"<index>: 1,..,64\n\r";
								
static	CLIDescriptor	mixer8SetCoef_Descriptor[]=
{
	{kCLI_Name,				(uint32)"setcoef"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&mixer8SetCoef_Tool},
	{kCLI_UsageString, 		(uint32)mixer8SetCoef_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ii"},
	{kCLI_ModuleName,		(uint32)"set mixer coefs"},
	{kCLI_GroupName,		(uint32)"mixer8"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT mixer8Dump(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i;
	
	cliPrintf("mixer8 coeff:\n\r");

	for (i = 0; i < 8; i++)
	{
		cliPrintf("%08X %08X %08X %08X %08X %08X %08X %08X\n\r",
			mixer8.ShadowCoeffs[i*8+0],
			mixer8.ShadowCoeffs[i*8+1],
			mixer8.ShadowCoeffs[i*8+2],
			mixer8.ShadowCoeffs[i*8+3],
			mixer8.ShadowCoeffs[i*8+4],
			mixer8.ShadowCoeffs[i*8+5],
			mixer8.ShadowCoeffs[i*8+6],
			mixer8.ShadowCoeffs[i*8+7]);
	}

	cliPrintf("\n\r");

	return hResult;
}

static  char	mixer8Dump_UsageString[]=\
				"Set: \n\r"\
				"use: mixer8.dump \n\r"\
				"------------------------------------------------------------\n\r"\
				"Dump Whole mixer: \n\r";
								
static	CLIDescriptor	mixer8Dump_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dump"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&mixer8Dump},
	{kCLI_UsageString, 		(uint32)mixer8Dump_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"dump mixer8"},
	{kCLI_GroupName,		(uint32)"mixer8"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT mixer8DumpChannel(uint32 index)
{
	HRESULT		hResult = NO_ERROR;
	
	if((index < 1) || (index > MAX_MIXER_CHANNELS))
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		return hResult;
	}
	index--;
	cliPrintf("mixer8.ChannelParameters[%d]\n\r\
		\tMute    = %08X\n\r\
		\tSolo    = %08X\n\r\
		\tFader   = %08X\n\r\
		\tPan     = %08X\n\r\
		\tCoupled = %08X\n\r", 
		index+1,
		mixer8.ChannelParameters[index].Mute,
		mixer8.ChannelParameters[index].Solo,
		mixer8.ChannelParameters[index].Fader,
		mixer8.ChannelParameters[index].Pan,
		mixer8.ChannelParameters[index].Coupled);
	return hResult;
}

static  char	mixer8DumpChannel_UsageString[]=\
				"Set: \n\r"\
				"use: mixer8.dumpchan <index>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Dump Whole mixer8 channel: \n\r";
								
static	CLIDescriptor	mixer8DumpChannel_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dumpchan"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&mixer8DumpChannel},
	{kCLI_UsageString, 		(uint32)mixer8DumpChannel_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"dump mixer8 channel"},
	{kCLI_GroupName,		(uint32)"mixer8"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT mixer8CliReadOverFlow(void)
{
	HRESULT		hResult = NO_ERROR;
	
	sysDebugPrintf("OverFlow = %X\n\r",mixer8ReadOverFlow());

	return hResult;
}

static  char	mixer8ReadOverFlow_UsageString[]=\
				"Set: \n\r"\
				"use: mixer8.readoverflow <index>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Read mixer8 overflow: \n\r";
								
static	CLIDescriptor	mixer8ReadOverFlow_Descriptor[]=
{
	{kCLI_Name,				(uint32)"readoverflow"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&mixer8CliReadOverFlow},
	{kCLI_UsageString, 		(uint32)mixer8ReadOverFlow_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"readoverflow"},
	{kCLI_GroupName,		(uint32)"mixer8"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void mixer8CliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(mixer8GetMaster_Descriptor);
	hResult += cliInstallCLIDescriptor(mixer8SetMaster_Descriptor);
	hResult += cliInstallCLIDescriptor(mixer8GetChannel_Descriptor);
	hResult += cliInstallCLIDescriptor(mixer8SetChannel_Descriptor);
	hResult += cliInstallCLIDescriptor(mixer8SetAux_Descriptor);
	hResult += cliInstallCLIDescriptor(mixer8GetAux_Descriptor);
	hResult += cliInstallCLIDescriptor(mixer8SetCoef_Descriptor);
	hResult += cliInstallCLIDescriptor(mixer8Dump_Descriptor);
	hResult += cliInstallCLIDescriptor(mixer8DumpChannel_Descriptor);
	hResult += cliInstallCLIDescriptor(mixer8ReadOverFlow_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing mixer8 cli tools");
	}
}

#endif //_CLI_TOOL_MIXER8
#endif //_CLI
#endif //_MIXER8


