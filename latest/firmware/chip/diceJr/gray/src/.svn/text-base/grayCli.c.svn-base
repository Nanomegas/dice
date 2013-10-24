/*===================================================================*
  Copyright (c) 2002 TC Applied Technologies Ltd.
  -------------------------------------------------------------------
  $Header: /usr/local/cvsroot/firmware/chip/diceJr/gray/src/grayCli.c,v 1.2 2007/02/08 17:59:33 zluo Exp $
 *===================================================================*/ 

 /*********************************************************************
 * DESCRIPTION: 

	Gray Test Module CLI for testing Gray encoder interface
		
	Revisions:
		created 12/11/2004 lm

***********************************************************************/

#ifdef _CLI
#include "cliToolDefs.h"
#include "TCTasking.h"
#include "coreDefs.h"
#include "cli.h"
//#include "dspCli.h"
#include "gray.h"
#include "grayCli.h"

//static char 		moduleName[] = __FILE__;

HRESULT	grayCliInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

#ifdef _CLI_TOOL_GRAY
	grayCliInstallTools();
#endif //_CLI_TOOL_GRAY

	return hResult;
}

HRESULT	grayCliSetup(BOOL bPollEnable)
{
	HRESULT		hResult = NO_ERROR;
	
	hResult = grayEventSetPollEnable(bPollEnable);
	if (hResult != NO_ERROR) return hResult;

	hResult = grayEventInitializeTasks();
	if (hResult != NO_ERROR) return hResult;

	hResult = grayEventInitialize();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT	grayCliSetMode(uint32 encId, uint32 mode)
{
	HRESULT		hResult = NO_ERROR;
	uint32		enc;
	uint32		all = GRAY_ENC_ITEMS;

	for (enc = 0; enc < GRAY_ENC_ITEMS; enc++)
	{
		if (encId == all ||
			encId == enc)
		{
			switch (mode)
			{
				case GRAY_ENC_MODE_DIRECT:
				case GRAY_ENC_MODE_DEFERRED:
					break;
				default:
					mode = GRAY_ENC_MODE_DEFAULT;
					break;
			}

			hResult = grayEventSetMode(enc, mode);
			if (hResult != NO_ERROR) return hResult;
		}
	}

	return hResult;
}

HRESULT	grayCliGetMode(uint32 encId)
{
	HRESULT		hResult = NO_ERROR;
	uint32		enc;
	uint32		mode;
	char		pollStr[16];
	char		modeStr[16];
	uint32		all = GRAY_ENC_ITEMS;

//	cliPrintf("enc default mode: %i\n\r", GRAY_ENC_MODE_DEFAULT);

	sprintf(pollStr, "%s", grayEventGetPollEnable() ? "polling" : "interrupt");

	for (enc = 0; enc < GRAY_ENC_ITEMS; enc++)
	{
		if (encId == all ||
			encId == enc)
		{
			hResult = grayEventGetMode(enc, &mode);
			if (hResult != NO_ERROR) return hResult;

			switch (mode)
			{
				case GRAY_ENC_MODE_DIRECT:
					sprintf(modeStr, "direct");
					break;
				case GRAY_ENC_MODE_DEFERRED:
					sprintf(modeStr, "deferred");
					break;
				default:
					sprintf(modeStr, "unknown");
					break;
			}

			cliPrintf("enc%i mode: %s %s (%i)\n\r", enc, pollStr, modeStr, mode);
		}
	}

	return hResult;
}


void grayCliAccCustomCallback(int8 *change, uint32 timeDiff)
{
	// custom callback using timeDiff (e.g. interrupt mode)
	if (timeDiff <= 10)
	{
		*change = *change * 20;
	}
	else if (timeDiff <= 20)
	{
		*change = *change * 10;
	}
	else if (timeDiff <= 50)
	{
		*change = *change * 5;
	}
}

HRESULT	grayCliSetAccMode(uint32 encId, uint32 accMode)
{
	HRESULT				hResult = NO_ERROR;
	uint32				enc;
	uint32				all = GRAY_ENC_ITEMS;
	GRAYACC_CALLBACK	accCustomCallback = NULL;

	for (enc = 0; enc < GRAY_ENC_ITEMS; enc++)
	{
		if (encId == all ||
			encId == enc)
		{
			switch (accMode)
			{
				case GRAY_ENC_ACC_MODE_NONE:
				case GRAY_ENC_ACC_MODE_TYPE1:
				case GRAY_ENC_ACC_MODE_TYPE2:
					break;
				case GRAY_ENC_ACC_MODE_CUSTOM:
					accCustomCallback = grayCliAccCustomCallback;
					break;
				default:
					accMode = GRAY_ENC_ACC_MODE_DEFAULT;
					break;
			}

			hResult = grayEventSetAccMode(enc, accMode, accCustomCallback);
			if (hResult != NO_ERROR) return hResult;
		}
	}

	return hResult;
}

HRESULT	grayCliGetAccMode(uint32 encId)
{
	HRESULT		hResult = NO_ERROR;
	uint32		enc;
	uint32		accMode;
	char		accModeStr[16];
	uint32		all = GRAY_ENC_ITEMS;

//	cliPrintf("enc default accMode: %i\n\r", GRAY_ENC_ACC_MODE_DEFAULT);

	for (enc = 0; enc < GRAY_ENC_ITEMS; enc++)
	{
		if (encId == all ||
			encId == enc)
		{
			hResult = grayEventGetAccMode(enc, &accMode);
			if (hResult != NO_ERROR) return hResult;

			switch (accMode)
			{
				case GRAY_ENC_ACC_MODE_NONE:
					sprintf(accModeStr, "none");
					break;
				case GRAY_ENC_ACC_MODE_TYPE1:
					sprintf(accModeStr, "type1");
					break;
				case GRAY_ENC_ACC_MODE_TYPE2:
					sprintf(accModeStr, "type2");
					break;
				case GRAY_ENC_ACC_MODE_CUSTOM:
					sprintf(accModeStr, "custom");
					break;
				default:
					sprintf(accModeStr, "unknown");
					break;
			}

			cliPrintf("enc%i mode: accMode:%s (%i)\n\r", enc, accModeStr, accMode);
		}
	}

	return hResult;
}

enum
{
	GRAY_CALLBACK_REMOVE = 0,	
//	GRAY_CALLBACK_DSPGAIN,
	GRAY_CALLBACK_PRINTVALUE
};

/*
void grayCliCallbackDSPGain(uint8 enc, int8 change)
{
	uint32		oldvalue = 0;
	int32		newvalue = 0;

	dspCliGetGainValue(&oldvalue);

	newvalue = oldvalue + change * 0x20000;
	if (newvalue < 0x000000) newvalue = 0x000000;
	if (newvalue > 0x7fffff) newvalue = 0x7fffff;

	if (oldvalue != newvalue)
	{
		dspCliSetGainValue((uint32) newvalue);
	}

//	cliPrintf("callBackDSPGain%i: change:%i\n\r", enc, change);
//	cliPrintf("callBackDSPGain%i: oldvalue:0x%06x newvalue:0x%06x\n\r", enc, oldvalue, newvalue);
}
*/


static uint32 grayCliPrintValue;

void grayCliCallbackPrintValue(uint8 enc, int8 change)
{
	uint32		oldvalue = 0;
	int32		newvalue = 0;

	oldvalue = grayCliPrintValue;

	newvalue = oldvalue + change;
	if (newvalue < 0x000000) newvalue = 0x000000;
	if (newvalue > 1000) newvalue = 1000;

	grayCliPrintValue = newvalue;

	cliPrintf("callBackPrintValue%i: change:%i oldvalue:%i newvalue:%i\n\r", enc, change, oldvalue, newvalue);
//	cliPrintf("callBackPrintValue%i: value:%i\n\r", enc, newvalue);
}

HRESULT	grayCliCallback(uint32 enc, uint32 mode)
{
	HRESULT		hResult = NO_ERROR;

	if (enc >= GRAY_ENC_ITEMS)
	{
		return E_BAD_INPUT_PARAMETERS;
	}

	switch (mode)
	{
//		case GRAY_CALLBACK_DSPGAIN:
//			hResult = grayEventAddCallback(enc, grayCliCallbackDSPGain);
//			break;

		case GRAY_CALLBACK_PRINTVALUE:
			hResult = grayEventAddCallback(enc, grayCliCallbackPrintValue);
			break;

		case GRAY_CALLBACK_REMOVE:
			hResult = grayEventRemoveCallback(enc);
			break;
	}

	return hResult;
}

#define MODE_DUMP				0x0
#define MODE_DUMP_IFSET			0x1
#define MODE_DUMP_IFENABLE		0x2
#define MODE_CLEAR				0x3
#define MODE_REGS				0x4

HRESULT	grayCliIntLogDump(uint32 mode)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i;
	uint32		count = 0;
	BOOL		bEnable = FALSE;

	count = grayEventGetIntCounts();
	cliPrintf("IntLog Total Events [%i]: %8i Enabled\n\r", GRAY_ENC_ITEMS, count);
	for (i = 0; i < GRAY_ENC_ITEMS; i++)
	{
		count = grayEventGetIntCount(i);
		bEnable = grayEventIsIntEnabled(i);

		if ((mode == MODE_DUMP) ||
			(mode == MODE_DUMP_IFSET && count) ||
			(mode == MODE_DUMP_IFENABLE && bEnable))
		{
			cliPrintf("Gray Rotary Encoder [%i]: %8i %s\n\r", 
				i, 
				count, 
				(bEnable) ? "+" : " ");
		}
	}

	return hResult;
}

HRESULT	grayCliIntLogClear(void)
{
	HRESULT		hResult = NO_ERROR;

	grayEventClearIntCounts();

	return hResult;
}

HRESULT	grayCliIntLogRegs(void)
{
	HRESULT		hResult = NO_ERROR;

	cliPrintf("GRAY_INT_SETUP : 0x%08x\n\r", *((volatile uint32 *) GRAY_INT_SETUP));
	cliPrintf("GRAY_INT_STATUS: 0x%08x\n\r", *((volatile uint32 *) GRAY_INT_STATUS));
	cliPrintf("GRAY_COUNTERS:   0x%08x\n\r", *((volatile uint32 *) GRAY_COUNTERS));

	return hResult;
}

HRESULT	grayCliIntLog(uint32 mode)
{
	HRESULT		hResult = NO_ERROR;

	switch(mode)
	{
		default:
		case MODE_DUMP:
		case MODE_DUMP_IFSET:
		case MODE_DUMP_IFENABLE:
			hResult = grayCliIntLogDump(mode);
			break;
		case MODE_CLEAR:
			hResult = grayCliIntLogClear();
			break;
		case MODE_REGS:
			hResult = grayCliIntLogRegs();
			break;
	}

	return hResult;
}

/*
HRESULT	grayCliVolume(void)
{
	HRESULT		hResult = NO_ERROR;

	//gray encoder dsp volume test

	//initialize dsp hi08 interface
	//load dsp test code
	//setup dice dsai
	hResult = dspCliSetup(TRUE);
	if (hResult != NO_ERROR) return hResult;

	//setup i2c fpga for gray encoder
	//setup gray encoder event handling
	hResult = grayCliSetup(FALSE);
	if (hResult != NO_ERROR) return hResult;

	//install gray encoder 1 callback for controlling dsp gain
	hResult = grayCliCallback(GRAY_ENC_0, GRAY_CALLBACK_DSPGAIN);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

*/
#ifdef _CLI_TOOL_GRAY

static  char	grayCliSetup_UsageString[]=\
				"gray.setup \n\r"\
				"use: setup <polling>\n\r"\
				"<polling>: on|off\n\r"\
				"------------------------------------------------------------\n\r"\
				"setup gray encoders\n\r"\
				"set mode polling/interrupt for gray encoder handling\n\r";

static	CLIDescriptor	grayCliSetup_Descriptor[]=
{
	{kCLI_Name,				(uint32)"setup"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&grayCliSetup},
	{kCLI_UsageString, 		(uint32)grayCliSetup_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"setup gray handling"},
	{kCLI_GroupName,		(uint32)"gray"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	grayCliSetMode_UsageString[]=\
				"gray.setmode \n\r"\
				"use: setmode <enc> <mode>\n\r"\
				"<enc>: 0,..,3,all\n\r"\
				"<mode>: default,direct,deferred\n\r"\
				"------------------------------------------------------------\n\r"\
				"set gray encoders mode\n\r";

static	CLIDescriptor	grayCliSetMode_Descriptor[]=
{
	{kCLI_Name,				(uint32)"setmode"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&grayCliSetMode},
	{kCLI_UsageString, 		(uint32)grayCliSetMode_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ik"},
	{kCLI_ConstantsString,	(uint32)"direct=0x0,deferred=0x1,default=0x2,all=0x4"},
	{kCLI_ModuleName,		(uint32)"set gray enc mode"},
	{kCLI_GroupName,		(uint32)"gray"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	grayCliGetMode_UsageString[]=\
				"gray.getmode \n\r"\
				"use: getmode <enc>\n\r"\
				"<enc>: 0,..,3,all\n\r"\
				"------------------------------------------------------------\n\r"\
				"get gray encoders mode\n\r";

static	CLIDescriptor	grayCliGetMode_Descriptor[]=
{
	{kCLI_Name,				(uint32)"getmode"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&grayCliGetMode},
	{kCLI_UsageString, 		(uint32)grayCliGetMode_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ConstantsString,	(uint32)"all=0x4"},
	{kCLI_ModuleName,		(uint32)"get gray enc mode"},
	{kCLI_GroupName,		(uint32)"gray"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	grayCliSetAccMode_UsageString[]=\
				"gray.setaccmode \n\r"\
				"use: setaccmode <enc> <accMode>\n\r"\
				"<enc>: 0,..,3,all\n\r"\
				"<accMode>: default,none,type1,type2,custom\n\r"\
				"   custom: for adding own acc handling callback\n\r"\
				"           here using a fixed test interrupt version\n\r"\
				"------------------------------------------------------------\n\r"\
				"set gray encoders acc mode\n\r";

static	CLIDescriptor	grayCliSetAccMode_Descriptor[]=
{
	{kCLI_Name,				(uint32)"setaccmode"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&grayCliSetAccMode},
	{kCLI_UsageString, 		(uint32)grayCliSetAccMode_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ik"},
	{kCLI_ConstantsString,	(uint32)"none=0x0,type1=0x1,type2=0x2,custom=0x3,default=0x4,all=0x4"},
	{kCLI_ModuleName,		(uint32)"set gray enc acc mode"},
	{kCLI_GroupName,		(uint32)"gray"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	grayCliGetAccMode_UsageString[]=\
				"gray.getaccmode \n\r"\
				"use: getaccmode <enc>\n\r"\
				"<enc>: 0,..,3,all\n\r"\
				"------------------------------------------------------------\n\r"\
				"get gray encoders acc mode\n\r";

static	CLIDescriptor	grayCliGetAccMode_Descriptor[]=
{
	{kCLI_Name,				(uint32)"getaccmode"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&grayCliGetAccMode},
	{kCLI_UsageString, 		(uint32)grayCliGetAccMode_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ConstantsString,	(uint32)"all=0x4"},
	{kCLI_ModuleName,		(uint32)"get gray enc acc mode"},
	{kCLI_GroupName,		(uint32)"gray"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	grayCliCallback_UsageString[]=\
				"gray.callback \n\r"\
				"use: callback <enc> <mode>\n\r"\
				"<enc>: 0,..,3\n\r"\
				"<mode>: remove  - remove installed callback\n\r"\
				"        dspgain - (not implemented)\n\r"\
				"        print   - install print value callback\n\r"\
				"------------------------------------------------------------\n\r"\
				"install/remove gray encoder callback to dsp gain\n\r";

static	CLIDescriptor	grayCliCallback_Descriptor[]=
{
	{kCLI_Name,				(uint32)"callback"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&grayCliCallback},
	{kCLI_UsageString, 		(uint32)grayCliCallback_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ik"},
	{kCLI_ConstantsString,	(uint32)"remove=0x0,dspgain=0x1,print=0x2"},
	{kCLI_ModuleName,		(uint32)"setup gray callback"},
	{kCLI_GroupName,		(uint32)"gray"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	grayCliIntLog_UsageString[]=\
				"gray.intlog: \n\r"\
				"use: gray.intlog <mode>\n\r"\
				"<mode>: dump,dumpifset,dumpifenable,clear,regs\n\r"\
				"------------------------------------------------------------\n\r"\
				"gray interrupt log";

static	CLIDescriptor	grayCliIntLog_Descriptor[]=
{
	{kCLI_Name,				(uint32)"intlog"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&grayCliIntLog},
	{kCLI_UsageString, 		(uint32)grayCliIntLog_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"dump=0x0,dumpifset=0x1,dumpifenable=0x2,clear=0x3,regs=0x4"},
	{kCLI_ModuleName,		(uint32)"gray interrupt log"},
	{kCLI_GroupName,		(uint32)"gray"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/*
static  char	grayCliVolume_UsageString[]=\
				"gray.volume: \n\r"\
				"use: gray.volume\n\r"\
				"------------------------------------------------------------\n\r"\
				"gray encoder dsp volume test\n\r"\
				"dsp: initialize hi08 interface\n\r"\
				"dsp: load dsp appl code\n\r"\
				"dsai: setup dice dsai\n\r"\
				"gray: setup i2c fpga for gray encoder (VB board only)\n\r"\
				"gray: setup gray encoder event handling\n\r"\
				"gray: install gray encoder 1 callback for controlling dsp gain";

static	CLIDescriptor	grayCliVolume_Descriptor[]=
{
	{kCLI_Name,				(uint32)"volume"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&grayCliVolume},
	{kCLI_UsageString, 		(uint32)grayCliVolume_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"gray volume test"},
	{kCLI_GroupName,		(uint32)"gray"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};
*/

void grayCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(grayCliSetup_Descriptor);
	hResult += cliInstallCLIDescriptor(grayCliSetMode_Descriptor);
	hResult += cliInstallCLIDescriptor(grayCliGetMode_Descriptor);
	hResult += cliInstallCLIDescriptor(grayCliSetAccMode_Descriptor);
	hResult += cliInstallCLIDescriptor(grayCliGetAccMode_Descriptor);
	hResult += cliInstallCLIDescriptor(grayCliCallback_Descriptor);
	hResult += cliInstallCLIDescriptor(grayCliIntLog_Descriptor);
	//hResult += cliInstallCLIDescriptor(grayCliVolume_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing gray cli tools");
	}
}

#endif //_CLI_TOOL_GRAY
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
