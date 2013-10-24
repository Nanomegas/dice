/*===================================================================*
  Copyright (c) 2002 TC Applied Technologies Ltd.
  -------------------------------------------------------------------
  $Header: /usr/local/cvsroot/firmware/project/projectEVM/dsp/src/dspCli.c,v 1.2 2007/02/08 17:59:33 zluo Exp $
  ===================================================================*/ 

 /*********************************************************************
 * DESCRIPTION: 

	DSP Test Module CLI for testing DSP HIO8 interface
		
	Revisions:
		created 12/11/2004 lm

***********************************************************************/

#ifdef _CLI
#include "cliToolDefs.h"
#include "TCTasking.h"
#include "cli.h"
#include "dsp.h"
#include "dspCli.h"
#include "dspAppl.h"
#include "diceDefs.h"
#include "diceDSAI.h"

//static char 		moduleName[] = __FILE__;

/* *************************************************************** */

// samples are multiplied with both dspappl_gainX and dspappl_gainY

static uint32	dspMem = Y_MEM;
static uint32	dspAddr = 0;
static uint32	dspValue = 0x7fffff;

HRESULT dspCliSetGain(uint32 value)
{
	HRESULT		hResult = NO_ERROR;

	dspValue = value;

	hResult = dspWriteMem(dspMem, dspAddr, dspValue);

	return hResult;
}

HRESULT dspCliGetGain(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		value;

	hResult = dspReadMem(dspMem, dspAddr, &value);

	cliPrintf("Gain read: 0x%x, expected: 0x%x\n\r", value, dspValue);

	return hResult;
}

HRESULT dspCliSetGainValue(uint32 value)
{
	return dspCliSetGain(value);
}

HRESULT dspCliGetGainValue(uint32 *value)
{
	HRESULT		hResult = NO_ERROR;

	*value = dspValue;

	return hResult;
}

HRESULT	dspCliTestGain(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i;
	uint32		value = 0x7fffff;

	for (i = 0; i < 16; i++)
	{
		dspCliSetGainValue(value);
		value -= 0x80000;
		TCTaskWait(200);
	}
	for (i = 0; i < 16; i++)
	{
		dspCliSetGainValue(value);
		value += 0x80000;
		TCTaskWait(200);
	}

	return hResult;
}

HRESULT dspCliSetValue(uint32 mem, uint32 addr, uint32 value)
{
	HRESULT		hResult = NO_ERROR;

	dspMem = mem;
	dspAddr = addr;
	dspValue = value;

	hResult = dspWriteMem(dspMem, dspAddr, dspValue);

	return hResult;
}

HRESULT dspCliGetValue(uint32 mem, uint32 addr)
{
	HRESULT		hResult = NO_ERROR;
	uint32		value;

	hResult = dspReadMem(mem, addr, &value);

	switch (dspMem)
	{
		case X_MEM: cliPrintf("x:0x%x : 0x%x\n\r", addr, value); break;
		case Y_MEM: cliPrintf("y:0x%x : 0x%x\n\r", addr, value); break;
		case P_MEM: cliPrintf("p:0x%x : 0x%x\n\r", addr, value); break;
		default: 
			cliPrintf("mem: unknown, addr: 0x%x\n\r", addr);
			hResult = E_FAIL;
			break;
	}

	return hResult;
}

HRESULT dspCliSetAddr(uint32 mem, uint32 addr)
{
	HRESULT		hResult = NO_ERROR;

	dspMem = mem;
	dspAddr = addr;

	return hResult;
}

HRESULT dspCliGetAddr(void)
{
	HRESULT		hResult = NO_ERROR;

	switch (dspMem)
	{
		case X_MEM: cliPrintf("dspMem: x, dspAddr: 0x%x\n\r", dspAddr); break;
		case Y_MEM: cliPrintf("dspMem: y, dspAddr: 0x%x\n\r", dspAddr); break;
		case P_MEM: cliPrintf("dspMem: p, dspAddr: 0x%x\n\r", dspAddr); break;
		default: 
			cliPrintf("dspMem: unknown, dspAddr: 0x%x\n\r", dspAddr);
			hResult = E_FAIL;
			break;
	}
	
	return hResult;
}

HRESULT dspCliSetHi08Value(uint32 offset, uint32 value)
{
	HRESULT		hResult = NO_ERROR;

	hi08Write(offset, value);

	return hResult;
}

HRESULT dspCliGetHi08Value(uint32 offset)
{
	HRESULT		hResult = NO_ERROR;
	uint32		value = 0;

	value = hi08Read(offset);

	cliPrintf("hi08[%i] : 0x%02x\n\r", offset, value);

	return hResult;
}

HRESULT dspCliDumpHi08Value(void)
{
	HRESULT		hResult = NO_ERROR;

	cliPrintf("ICR [%i] : 0x%02x\n\r", DSP_ICR_REG, hi08Read(DSP_ICR_REG));
	cliPrintf("CVR [%i] : 0x%02x\n\r", DSP_CVR_REG, hi08Read(DSP_CVR_REG));
	cliPrintf("ISR [%i] : 0x%02x\n\r", DSP_ISR_REG, hi08Read(DSP_ISR_REG));
	cliPrintf("IVR [%i] : 0x%02x\n\r", DSP_IVR_REG, hi08Read(DSP_IVR_REG));
	cliPrintf("H   [%i] : 0x%02x\n\r", DSP_H_REG, hi08Read(DSP_H_REG));
	cliPrintf("M   [%i] : 0x%02x\n\r", DSP_M_REG, hi08Read(DSP_M_REG));
	cliPrintf("L   [%i] : 0x%02x\n\r", DSP_L_REG, hi08Read(DSP_L_REG));

	return hResult;
}

HRESULT	dspCliLoadDspCode(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = dspLoadCode((BYTE *) dspappl_code, dspappl_size, 0);

	return hResult;
}

void dspCliSetupDSAI(void)
{
    //---------- Setup DSAI receiver and transmitter ----------
	// Mode = master and 8 channels
	dsaiSetChMode(DSAI_ID1, DSAI_RX | DSAI_TX, MODE_8_CH);

#if 0 //test for 1 clock or 32 clock of frame sync (need to be identical in dsp dsai configuration)
	dsaiSetSyncLen(DSAI_ID1, DSAI_RX | DSAI_TX, SYNC_LEN_1_CLK);
	dsaiSetDataDelay(DSAI_ID1, DSAI_RX | DSAI_TX, DELAY_1_CLK);
#else
	dsaiSetSyncLen(DSAI_ID1, DSAI_RX | DSAI_TX, SYNC_LEN_32_CLK);
	dsaiSetDataDelay(DSAI_ID1, DSAI_RX | DSAI_TX, DELAY_0_CLK);
#endif

	dsaiSetDataShuffle(DSAI_ID1, DSAI_RX | DSAI_TX, SHUFFLE_M0);

#if 1 //test for clock and frame sync master or slave
	dsaiSetMaster(DSAI_ID1, DSAI_RX | DSAI_TX, TRUE);
#else
	dsaiSetMaster(DSAI_ID1, DSAI_RX | DSAI_TX, FALSE);
#endif

	*((volatile uint32 *) GPCSR_DSAI_SEL) = 0xf000; // set all dsai's as master
}

HRESULT	dspCliDiceSetup(void)
{
	HRESULT		hResult = NO_ERROR;

	dspCliSetupDSAI();

	return hResult;
}

HRESULT	dspCliSetup(BOOL bLoad)
{
	HRESULT		hResult = NO_ERROR;

	hResult = dspInitialize();
	if (hResult != NO_ERROR) return hResult;

	if (bLoad)
	{
		hResult = dspCliLoadDspCode();
		if (hResult != NO_ERROR) return hResult;
	}

	hResult = dspCheckPresentAndRunning();
	if (hResult != NO_ERROR) return hResult;
	
	hResult = dspCliDiceSetup();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT	dspCliInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

	dspMem = dspGetMem(dspappl_gainY);
	dspAddr = dspGetAddr(dspappl_gainY);

#ifdef _CLI_TOOL_DSP
	dspCliInstallTools();
#endif //_CLI_TOOL_DSP

	return hResult;
}

#ifdef _CLI_TOOL_DSP

static  char	dspCliSetup_UsageString[]=\
				"dsp.setup \n\r"\
				"use: setup <bLoad:on|off>\n\r"\
				"<bLoad>: load dsp code (off if used with dsp debugger)\n\r"\
				"------------------------------------------------------------\n\r"\
				"setup hi08, load dsp code and setup dice dsai and aes\n\r";

static	CLIDescriptor	dspCliSetup_Descriptor[]=
{
	{kCLI_Name,				(uint32)"setup"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&dspCliSetup},
	{kCLI_UsageString, 		(uint32)dspCliSetup_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"setup dsp test"},
	{kCLI_GroupName,		(uint32)"dsp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	dspCliGetGain_UsageString[]=\
				"dsp.getgain \n\r"\
				"use: getgain\n\r"\
				"------------------------------------------------------------\n\r"\
				"get dsp gain value\n\r";

static	CLIDescriptor	dspCliGetGain_Descriptor[]=
{
	{kCLI_Name,				(uint32)"getgain"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&dspCliGetGain},
	{kCLI_UsageString, 		(uint32)dspCliGetGain_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"get dsp gain"},
	{kCLI_GroupName,		(uint32)"dsp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	dspCliSetGain_UsageString[]=\
				"dsp.setgain \n\r"\
				"use: setgain <gain>\n\r"\
				"------------------------------------------------------------\n\r"\
				"set dsp gain value using default mem and address\n\r";

static	CLIDescriptor	dspCliSetGain_Descriptor[]=
{
	{kCLI_Name,				(uint32)"setgain"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&dspCliSetGain},
	{kCLI_UsageString, 		(uint32)dspCliSetGain_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"set dsp gain value"},
	{kCLI_GroupName,		(uint32)"dsp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	dspCliTestGain_UsageString[]=\
				"dsp.testgain \n\r"\
				"use: testgain\n\r"\
				"------------------------------------------------------------\n\r"\
				"dsp testing gain by lowering/increasing gain\n\r";

static	CLIDescriptor	dspCliTestGain_Descriptor[]=
{
	{kCLI_Name,				(uint32)"testgain"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&dspCliTestGain},
	{kCLI_UsageString, 		(uint32)dspCliTestGain_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"dsp testing gain"},
	{kCLI_GroupName,		(uint32)"dsp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	dspCliGetAddr_UsageString[]=\
				"dsp.getaddr \n\r"\
				"use: getaddr \n\r"\
				"------------------------------------------------------------\n\r"\
				"get default dsp mem and addr setting used for getgain/setgain\n\r";

static	CLIDescriptor	dspCliGetAddr_Descriptor[]=
{
	{kCLI_Name,				(uint32)"getaddr"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&dspCliGetAddr},
	{kCLI_UsageString, 		(uint32)dspCliGetAddr_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"get default dsp mem addr"},
	{kCLI_GroupName,		(uint32)"dsp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	dspCliSetAddr_UsageString[]=\
				"dsp.setaddr \n\r"\
				"use: setaddr <mem> <addr>\n\r"\
				"<mem>: x,y,p\n\r"\
				"------------------------------------------------------------\n\r"\
				"set default dsp mem and addr setting used for getgain/setgain\n\r";

static	CLIDescriptor	dspCliSetAddr_Descriptor[]=
{
	{kCLI_Name,				(uint32)"setaddr"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&dspCliSetAddr},
	{kCLI_UsageString, 		(uint32)dspCliSetAddr_UsageString},
	{kCLI_ConstantsString,	(uint32)"x=2,y=4,p=0"},
	{kCLI_ArgumentPatternString, (uint32)"ki"},
	{kCLI_ModuleName,		(uint32)"set default dsp mem addr"},
	{kCLI_GroupName,		(uint32)"dsp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	dspCliGetValue_UsageString[]=\
				"dsp.getvalue \n\r"\
				"use: getvalue <mem> <addr>\n\r"\
				"<mem>: x,y,p\n\r"\
				"------------------------------------------------------------\n\r"\
				"get dsp value from mem and addr setting\n\r";

static	CLIDescriptor	dspCliGetValue_Descriptor[]=
{
	{kCLI_Name,				(uint32)"getvalue"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&dspCliGetValue},
	{kCLI_UsageString, 		(uint32)dspCliGetValue_UsageString},
	{kCLI_ConstantsString,	(uint32)"x=2,y=4,p=0"},
	{kCLI_ArgumentPatternString, (uint32)"ki"},
	{kCLI_ModuleName,		(uint32)"get dsp value"},
	{kCLI_GroupName,		(uint32)"dsp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	dspCliSetValue_UsageString[]=\
				"dsp.setvalue \n\r"\
				"use: setvalue <mem> <addr> <value>\n\r"\
				"<mem>: x,y,p\n\r"\
				"------------------------------------------------------------\n\r"\
				"set dsp value from mem and addr setting\n\r";

static	CLIDescriptor	dspCliSetValue_Descriptor[]=
{
	{kCLI_Name,				(uint32)"setvalue"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&dspCliSetValue},
	{kCLI_UsageString, 		(uint32)dspCliSetValue_UsageString},
	{kCLI_ConstantsString,	(uint32)"x=2,y=4,p=0"},
	{kCLI_ArgumentPatternString, (uint32)"kii"},
	{kCLI_ModuleName,		(uint32)"set dsp value"},
	{kCLI_GroupName,		(uint32)"dsp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	dspCliGetHi08Value_UsageString[]=\
				"dsp.gethi08 \n\r"\
				"use: gethi08 <register>\n\r"\
				"<register>: 0,..,7\n\r"\
				"------------------------------------------------------------\n\r"\
				"get dsp hi08 register value\n\r";

static	CLIDescriptor	dspCliGetHi08Value_Descriptor[]=
{
	{kCLI_Name,				(uint32)"gethi08"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&dspCliGetHi08Value},
	{kCLI_UsageString, 		(uint32)dspCliGetHi08Value_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"get dsp hi08 value"},
	{kCLI_GroupName,		(uint32)"dsp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	dspCliSetHi08Value_UsageString[]=\
				"dsp.sethi08 \n\r"\
				"use: sethi08 <register> <value>\n\r"\
				"<register>: 0,..,7\n\r"\
				"------------------------------------------------------------\n\r"\
				"set dsp hi08 register value\n\r";

static	CLIDescriptor	dspCliSetHi08Value_Descriptor[]=
{
	{kCLI_Name,				(uint32)"sethi08"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&dspCliSetHi08Value},
	{kCLI_UsageString, 		(uint32)dspCliSetHi08Value_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ii"},
	{kCLI_ModuleName,		(uint32)"set dsp hi08 value"},
	{kCLI_GroupName,		(uint32)"dsp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	dspCliDumpHi08Value_UsageString[]=\
				"dsp.dumphi08 \n\r"\
				"use: dumphi08 \n\r"\
				"------------------------------------------------------------\n\r"\
				"dump dsp hi08 registers\n\r";

static	CLIDescriptor	dspCliDumpHi08Value_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dumphi08"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&dspCliDumpHi08Value},
	{kCLI_UsageString, 		(uint32)dspCliDumpHi08Value_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"dump dsp hi08"},
	{kCLI_GroupName,		(uint32)"dsp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void dspCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(dspCliSetup_Descriptor);
	hResult += cliInstallCLIDescriptor(dspCliGetValue_Descriptor);
	hResult += cliInstallCLIDescriptor(dspCliSetValue_Descriptor);
	hResult += cliInstallCLIDescriptor(dspCliGetGain_Descriptor);
	hResult += cliInstallCLIDescriptor(dspCliSetGain_Descriptor);
	hResult += cliInstallCLIDescriptor(dspCliTestGain_Descriptor);
	hResult += cliInstallCLIDescriptor(dspCliGetAddr_Descriptor);
	hResult += cliInstallCLIDescriptor(dspCliSetAddr_Descriptor);
	hResult += cliInstallCLIDescriptor(dspCliGetHi08Value_Descriptor);
	hResult += cliInstallCLIDescriptor(dspCliSetHi08Value_Descriptor);
	hResult += cliInstallCLIDescriptor(dspCliDumpHi08Value_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing dsp cli tools");
	}
}

#endif //_CLI_TOOL_DSP
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
