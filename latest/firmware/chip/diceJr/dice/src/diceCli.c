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
	
	Purpose:	DICE - CLI
	
	Revisions:
		created 12/06/2003 kbj

****************************************************************************/

#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_DICE
#include "TCTasking.h"
#include "dice.h"
#include "diceRouterReg.h"
#include "diceCli.h"
#include "cli.h"

static void dumpHPLL(DICE_HPLL *pDiceHPLL)
{
	cliPrintf("HPLL registers:\n\r");
	cliPrintf("\tcaf_enable  : %3i",		(unsigned long)pDiceHPLL->caf_enable);
	cliPrintf("\tfract_res   : %3i\n\r",	pDiceHPLL->fract_res);
	cliPrintf("\tcaf_select  : %3i",		pDiceHPLL->caf_select);
	cliPrintf("\tburst_len   : %3i\n\r",	pDiceHPLL->burst_len);
	cliPrintf("\tu_treshold  : %3i",		pDiceHPLL->u_treshold);
	cliPrintf("\tx1x2_mode   : %3i\n\r",	pDiceHPLL->x1x2_mode);
	cliPrintf("\tbandwidth_f : %3i",		pDiceHPLL->bandwidth_f);
	cliPrintf("\tsink_e      : %3i\n\r",	pDiceHPLL->sink_e);
	cliPrintf("\tbandwidth_c : %3i",		pDiceHPLL->bandwidth_c);
	cliPrintf("\tanchor_e    : %3i\n\r",	pDiceHPLL->anchor_e);
	cliPrintf("\tshape_f     : %3i",		pDiceHPLL->shape_f);
	cliPrintf("\te_anc_val   : %3i\n\r",	pDiceHPLL->e_anc_val);
	cliPrintf("\tshape_v     : %3i",		pDiceHPLL->shape_v);
	cliPrintf("\tedet_x1     : %3i\n\r",	pDiceHPLL->edet_x1);
	cliPrintf("\tmax_slew_f  : %3i",		pDiceHPLL->max_slew_f);
	cliPrintf("\tedet_x2     : %3i\n\r",	pDiceHPLL->edet_x2);
	cliPrintf("\tmax_slew_v  : %3i",		pDiceHPLL->max_slew_v);
	cliPrintf("\tediv_c      : %3i\n\r",	pDiceHPLL->ediv_c);
	cliPrintf("\tloos_thr    : %3i",		pDiceHPLL->loos_thr);
	cliPrintf("\tediv_f      : %3i\n\r",	pDiceHPLL->ediv_f);
	cliPrintf("\tmin_period  : %3i",		pDiceHPLL->min_period);
	cliPrintf("\tediv_s      : %3i\n\r",	pDiceHPLL->ediv_s);
	cliPrintf("\tmax_period  : %3i",		pDiceHPLL->max_period);
	cliPrintf("\tinvert_cde  : %3i\n\r",	pDiceHPLL->invert_cde);
	cliPrintf("\tndiv_f      : %3i",		pDiceHPLL->ndiv_f);
	cliPrintf("\thobble_cde  : %3i\n\r",	pDiceHPLL->hobble_cde);
	cliPrintf("\tndiv_e      : %3i",		pDiceHPLL->ndiv_e);
	cliPrintf("\tdivide_cj   : %3i\n\r",	pDiceHPLL->divide_cj);
	cliPrintf("\tndiv_b      : %3i",		pDiceHPLL->ndiv_b);
	cliPrintf("\tinvert_cj   : %3i\n\r",	pDiceHPLL->invert_cj);
	cliPrintf("\tphase_lag   : %3i",		pDiceHPLL->phase_lag);
	cliPrintf("\tmain_status : 0x%04x\n\r",	pDiceHPLL->main_status);
	cliPrintf("\tinstance_id : 0x%04x",		pDiceHPLL->instance_id);
	cliPrintf("\trevision_id : 0x%04x\n\r",	pDiceHPLL->revision_id);

}

static void dumpClock(void)
{
	cliPrintf("Clock registers:\n\r");
	cliPrintf("\tSYNC_CLK_SEL   : 0x%04x\n\r", pDiceClock->syncClkSel.reg);
	cliPrintf("\tROUTER_CLK_SEL : 0x%04x\n\r", pDiceClock->routerClkSel.reg);
	cliPrintf("\tWC_EC_SEL      : 0x%04x\n\r", pDiceClock->wcEcSel.reg);
	cliPrintf("\tBLOCK_SYNC     : 0x%04x\n\r", pDiceClock->blockSync.reg);
	cliPrintf("\tHPLL_CLK_SEL   : 0x%04x\n\r", pDiceClock->hpllClkSel.reg);
	cliPrintf("\tSR_CNT_SEL     : 0x%04x\n\r", pDiceClock->srCntSel.reg);
	cliPrintf("\tSR_MODE_SEL    : 0x%04x\n\r", pDiceClock->srModeSel.reg);
	cliPrintf("\tAESRX_MCK_SEL  : 0x%04x\n\r", pDiceClock->aesRxMckSel.reg);
	cliPrintf("\tPRESCALER_1    : 0x%04x\n\r", pDiceClock->prescaler1Sel);
	cliPrintf("\tHYBRID_PLL_SET : 0x%04x\n\r", pDiceClock->hybridPllSetup);
	cliPrintf("\tSR_COUNT_1     : 0x%04x\n\r", pDiceClock->srCount1);
	cliPrintf("\tSR_COUNT_2     : 0x%04x\n\r", pDiceClock->srCount2);
	cliPrintf("\tSR_COUNT_1MAX  : 0x%04x\n\r", pDiceClock->srCount1Max);
	cliPrintf("\tSR_COUNT_2MAX  : 0x%04x\n\r", pDiceClock->srCount2Max);
}

static const char * srcNames[16] = 
{
	"AES",  //DEV_AES_RX,
	"ADAT", //DEV_ADAT_RX,
	"MIX",  //DEV_MIXER_RX,
	"NA",
	"INS0", //DEV_INS_RX0=4,
	"INS1", //DEV_INS_RX1,
	"NA",
	"NA",
	"NA",
	"NA",
	"ARM",  //DEV_APB_RX=10,
	"AVS1", //DEV_AVS_ADO1,
	"AVS2", //DEV_AVS_ADO2,
	"NA",
	"NA",
	"MUTE"
};

static const char * dstNames[16] = 
{
	"AES",  //DEV_AES_TX,
	"ADAT", //DEV_ADAT_TX,
	"MIX0", //DEV_MIXER0_TX,
	"MIX1", //DEV_MIXER0_TX,
	"INS0", //DEV_INS_TX0,
	"INS1", //DEV_INS_TX1,
	"NA",
	"NA",
	"NA",
	"NA",
	"ARM",  //DEV_APB_TX=10,
	"AVS1", //DEV_AVS_ADI1,
	"AVS2", //DEV_AVS_ADI2,
	"NA",
	"NA",
	"MUTE"
};

static void dumpRouter(void)
{
	uROUTER_SETUP routerSetup;
	uROUTER_ENTRY routerEntry;
	uint32	i;

	cliPrintf("Router 1 registers:\n\r");
	
	diceRegRead(DICE_ROUTER1_SETUP, &routerSetup.reg);
	cliPrintf("\tROUTER1_SETUP       : 0x%04x\n\r", routerSetup.reg);
	
	for (i = 0; i < routerSetup.bit.numEntry+1; i++)
	{
		diceRegRead((DICE_ROUTER1_ENTRY+(i*4)), &routerEntry.reg);
		cliPrintf("\tROUTER1_ENTRY[%03i]  : 0x%03x 0x%04x %4s ch %-2i -> %4s ch %-2i\n\r", i, (routerEntry.reg>>16)&0xfff,routerEntry.reg&0xffff,srcNames[routerEntry.bit.srcBlkID],routerEntry.bit.srcCh,
			                                                                                       dstNames[routerEntry.bit.dstBlkID],routerEntry.bit.dstCh);
	}
}

static void dumpAESRx(void)
{
	int ch, csb;

	cliPrintf("AES Rx registers:\n\r");
	cliPrintf("\tSETUP          : 0x%04x\n\r", pDiceAESRx->setup.reg);
	cliPrintf("\tPLL_PLS_WIDTH  : 0x%04x\n\r", pDiceAESRx->pllPulse.reg);
	cliPrintf("\tFORCE_VCO      : 0x%04x\n\r", pDiceAESRx->forceVCO.reg);
	cliPrintf("\tVCO_MIN_FQ_LSB : 0x%04x\n\r", pDiceAESRx->minVCOFqLSB);
	cliPrintf("\tVCO_MIN_FQ_MSB : 0x%04x\n\r", pDiceAESRx->minVCOFqMSB);

	cliPrintf("AES Rx channel status bytes:\n\r");
	for (ch = 0; ch < NUM_AES_CHANNELS; ch++)
	{
		cliPrintf("\tCh[%i] ", ch);
		for (csb = 0; csb < NUM_AES_CS_BYTES; csb++)
		{
			cliPrintf("0x%02x ", pDiceAESRx->csChannel[ch][csb]);
		}
		cliPrintf("\n\r"); // Next line
	}

	cliPrintf("AES Rx channel setup status bytes:\n\r");
	for (csb = 0; csb < NUM_AES_SEL_CS_BYTES; )
	{
		if (csb%8 == 0) cliPrintf("\t");
		cliPrintf("0x%02x ", pDiceAESRx->csChannelSel[csb++]);
		if (csb%8 == 0) cliPrintf("\n\r"); // Next line
	}
}

static void dumpAESState(void)
{
	BOOL	bEnable;
	BOOL	bLocked;
	uint8	master;
	uint16	aesStatus;
	
	aesRxGetEnable (&bEnable);
	bLocked = aesIsMasterLocked ();
	aesGetMasterClk(&master);
	aesGetStatus (&aesStatus);

	cliPrintf("AES State:\n\r");
	cliPrintf("\tMaster          : AES%i\n\r",master);
	cliPrintf("\tState           : %s\n\r", bEnable ? (bLocked ? "Locked" : "Unlocked") : "Disabled");	
	cliPrintf("\tAES0            : %s V=%i\n\r", (aesStatus & AES_STAT_LOCK0) ? "Locked  " : "Unlocked", (aesStatus & AES_STAT_VALIDITY0) ? 1 : 0);
	cliPrintf("\tAES1            : %s V=%i\n\r", (aesStatus & AES_STAT_LOCK1) ? "Locked  " : "Unlocked", (aesStatus & AES_STAT_VALIDITY1) ? 1 : 0);
	cliPrintf("\tAES2            : %s V=%i\n\r", (aesStatus & AES_STAT_LOCK2) ? "Locked  " : "Unlocked", (aesStatus & AES_STAT_VALIDITY2) ? 1 : 0);
	cliPrintf("\tAES3            : %s V=%i\n\r", (aesStatus & AES_STAT_LOCK3) ? "Locked  " : "Unlocked", (aesStatus & AES_STAT_VALIDITY3) ? 1 : 0);
}

static void dumpAESError(void)
{
	uint8	i;
	uAESRX_ERROR error[4];
	
	for (i=0; i<4; i++)	
		aesGetRxError(i, &error[i], TRUE);
	
	
	cliPrintf("AES Errors  R=Repeat, S=Slip, P=Parity, C=CRC:\n\r");
	cliPrintf("\tAES0            : %c%c%c%c\n\r", error[0].bit.u_run ? 'R' : ' ', error[0].bit.o_run ? 'S' : ' ',error[0].bit.pty_err ? 'P' : ' ', error[0].bit.crc_err ? 'C' : ' ');
	cliPrintf("\tAES1            : %c%c%c%c\n\r", error[1].bit.u_run ? 'R' : ' ', error[1].bit.o_run ? 'S' : ' ',error[1].bit.pty_err ? 'P' : ' ', error[1].bit.crc_err ? 'C' : ' ');	
	cliPrintf("\tAES2            : %c%c%c%c\n\r", error[2].bit.u_run ? 'R' : ' ', error[2].bit.o_run ? 'S' : ' ',error[2].bit.pty_err ? 'P' : ' ', error[2].bit.crc_err ? 'C' : ' ');
	cliPrintf("\tAES3            : %c%c%c%c\n\r", error[3].bit.u_run ? 'R' : ' ', error[3].bit.o_run ? 'S' : ' ',error[3].bit.pty_err ? 'P' : ' ', error[3].bit.crc_err ? 'C' : ' ');
}

static void dumpAESTx(void)
{
	int ch, csb;

	cliPrintf("AES Tx registers:\n\r");
	cliPrintf("\tMODE_SETUP     : 0x%04x\n\r", pDiceAESTx->modeSetup.reg);
	cliPrintf("\tCBL_SETUP      : 0x%04x\n\r", pDiceAESTx->cblSetup.reg);
	cliPrintf("\tCH_MUTE        : 0x%04x\n\r", pDiceAESTx->chMute.reg);
	cliPrintf("\tCH_VALIDITY    : 0x%04x\n\r", pDiceAESTx->chValidity.reg);
	cliPrintf("\tUSER_SEL[0-7]  : 0x%02x 0x%02x 0x%02x 0x%02x\n\r", 
								  pDiceAESTx->userSelCh[0].reg,
								  pDiceAESTx->userSelCh[1].reg,
								  pDiceAESTx->userSelCh[2].reg,
								  pDiceAESTx->userSelCh[3].reg);

	cliPrintf("AES Tx channel status bytes:\n\r");
	for (ch = 0; ch < NUM_AES_CHANNELS; ch++)
	{
		cliPrintf("\tCh[%i] ", ch);
		for (csb = 0; csb < NUM_AES_CS_BYTES; csb++)
		{
			cliPrintf("0x%02x ", pDiceAESTx->csChannel[ch][csb]);
		}
		cliPrintf("\n\r"); // Next line
	}

	cliPrintf("AES Tx channel setup status bytes:\n\r");
	for (csb = 0; csb < NUM_AES_SEL_CS_BYTES; )
	{
		if (csb%8 == 0) cliPrintf("\t");
		cliPrintf("0x%02x ", pDiceAESTx->csChannelSel[csb++]);
		if (csb%8 == 0) cliPrintf("\n\r"); // Next line
	}
}


static void dumpADAT(void)
{
	BOOL	bEnable;
	BOOL	bLocked;
	BOOL	bLocked_aux;
	uADATRX_STATUS error, error_aux;
	
	adatRxGetEnable (&bEnable);
	bLocked = adatIsLocked (0);
	bLocked_aux = adatIsLocked (1);
	adatRxGetError(0,&error, TRUE);
	adatRxGetError(1,&error_aux, TRUE);
	
	cliPrintf("ADAT Rx:\n\r");
	cliPrintf("\tState           : %s\n\r", bEnable ? (bLocked ? "Locked" : "Unlocked") : "Disabled");	
	cliPrintf("ADAT Errors  L=No Lock, S=No Sync\n\r");
	cliPrintf("\tErrors          : %c%c%\n\r", error.bit.no_lock ? 'L' : ' ', error.bit.no_sync ? 'S' : ' ');
	cliPrintf("ADAT Rx registers:\n\r");
	cliPrintf("\tSTATUS         : 0x%04x\n\r", pDiceADATRx->status.reg);
	cliPrintf("ADAT Tx registers:\n\r");
	cliPrintf("\tUSER_SETUP     : 0x%04x\n\r", pDiceADATTx->userSetup.reg & 0x0f);
	cliPrintf("\tCH_MUTE        : 0x%04x\n\r", pDiceADATTx->chMute.reg);	

	cliPrintf("ADAT AUX Rx:\n\r");
	cliPrintf("\tState           : %s\n\r", bEnable ? (bLocked_aux ? "Locked" : "Unlocked") : "Disabled");	
	cliPrintf("ADAT AUX Errors  L=No Lock, S=No Sync\n\r");
	cliPrintf("\tErrors          : %c%c%\n\r", error_aux.bit.no_lock ? 'L' : ' ', error_aux.bit.no_sync ? 'S' : ' ');
	cliPrintf("ADAT AUX Rx registers:\n\r");
	cliPrintf("\tSTATUS         : 0x%04x\n\r", pDiceADATRx->statusAux.reg);
	cliPrintf("ADAT AUX Tx registers:\n\r");
	cliPrintf("\tUSER_SETUP     : 0x%04x\n\r", (pDiceADATTx->userSetup.reg>>4));
	cliPrintf("\tCH_MUTE        : 0x%04x\n\r", pDiceADATTx->chMuteAux.reg);	

}


HRESULT	diceDump_Tool(uint32 dump)
{
	switch (dump)
	{
		case 0: // Clock
			dumpClock();
			break;

		case 1: // AES
			dumpAESState();
			dumpAESError();
			break;

		case 2: // Router
			dumpRouter();
			break;

		case 4: // ADAT
			dumpADAT();
			break;


		case 7: // HPLL1
			dumpHPLL(pDiceHPLL1);
			break;


		case 9: // AESRX
			dumpAESRx();
			break;

		case 10: // AESTX
			dumpAESTx();
			break;
	}

	return NO_ERROR;
}

static  char	Dump_UsageString[]=\
				"dump: \n\r"\
				"use: dice:dump <module>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Dump register contents for module: \n\r"\
				"clock, aes, router, adat, hpll, hpll1, aesRx, aesTx\n\r";
static	CLIDescriptor	Dump_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dump"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&diceDump_Tool},
	{kCLI_UsageString, 		(uint32)Dump_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"clock=0,aes=1,router=2,adat=4,hpll1=7,hpll=7,aesRx=9,aesTx=10"},
	{kCLI_ModuleName,		(uint32)"Dump register contents"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	clkSrc_UsageString[]=\
				"clksrc: \n\r"\
				"use: dice:clksrc <source>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Select hybrid pll input clock source: \n\r"\
				"aes,adat0,adat1,wc,avs1,avs2,pre,ext\n\r";
static	CLIDescriptor	clkSrc_Descriptor[]=
{
	{kCLI_Name,				(uint32)"clksrc"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&clkSetInHybridPll},
	{kCLI_UsageString, 		(uint32)clkSrc_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"aes=0,adat0=1,adat1=2,wc=3,avs1=4,avs2=5,pre=8,ext=9"},
	{kCLI_ModuleName,		(uint32)"Set input clock source"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};



HRESULT aesSetMode(uint32 mode)
{

	switch (mode)
	{
		case 0: // Master mode
			aesTxSetMasterMode(FALSE);
			break;
		case 1: // Slave mode, block sync from CH_ID0
			aesTxSetSlaveMode(CH_ID0);
			break;
	}

	return NO_ERROR;
}

static  char	AES_UsageString[]=\
				"aes: \n\r"\
				"use: dice:aes <master|slave>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set mode for AES communication: \n\r"\
				"master or slave clock mode\n\r";
static	CLIDescriptor	AES_Descriptor[]=
{
	{kCLI_Name,				(uint32)"aes"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&aesSetMode},
	{kCLI_UsageString, 		(uint32)AES_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ConstantsString,	(uint32)"master=0,slave=1"},
	{kCLI_ModuleName,		(uint32)"Set AES communication mode"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


HRESULT aesSetProMode(BOOL proMode)
{
	return aesTxCSSetMode (proMode);
}

static  char	AES_ProMode_UsageString[]=\
				"aespm: \n\r"\
				"use: dice:aespm <on|off>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set professional or consumer mode for AES communication: \n\r"\
				"on selects pro mode, off sets consumer mode\n\r";

static	CLIDescriptor	AES_ProModeDescriptor[]=
{
	{kCLI_Name,				(uint32)"aespm"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&aesSetProMode},
	{kCLI_UsageString, 		(uint32)AES_ProMode_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ConstantsString,	(uint32)"off=0,on=1"},
	{kCLI_ModuleName,		(uint32)"Set AES pro or consumer mode"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

HRESULT aesSetCopyProtect(uint32 cpy)
{
	return aesTxCSSetCpy ((uint8)cpy);
}

static  char	AES_CopyProtect_UsageString[]=\
				"aescp: \n\r"\
				"use: dice:aescp <inf|one|none>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set copy protection for AES communication: \n\r"\
				"inf, one, none\n\r";

static	CLIDescriptor	AES_CopyProtectDescriptor[]=
{
	{kCLI_Name,				(uint32)"aescp"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&aesSetCopyProtect},
	{kCLI_UsageString, 		(uint32)AES_CopyProtect_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ConstantsString,	(uint32)"inf=0,one=1,none=3"},
	{kCLI_ModuleName,		(uint32)"Set AES copy protection"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


HRESULT aesSetPreEmphasis(uint32 pe)
{
	return aesTxCSSetPreEmphasis ((uint8)pe);
}

static  char	AES_PreEmphasis_UsageString[]=\
				"aespe: \n\r"\
				"use: dice:aespe <nind|none|50_15|j17>\n\r"\
				" nind = not indicated\n\r"\
				" none = no emphasis\n\r"\
				" 50_15 = 50us + 15us emphasis\n\r"\
				" j17 = ITU J.17\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set pre emphasis state for AES communication: \n\r";

static	CLIDescriptor	AES_PreEmphasisDescriptor[]=
{
	{kCLI_Name,				(uint32)"aespe"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&aesSetPreEmphasis},
	{kCLI_UsageString, 		(uint32)AES_PreEmphasis_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ConstantsString,	(uint32)"nind=0,none=1,50_15=2,j17=3"},
	{kCLI_ModuleName,		(uint32)"Set AES Emphasis"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


HRESULT aesSetCategory(uint32 cat)
{
	return aesTxCSSetCategory ((uint8)cat);
}

static  char	AES_Category_UsageString[]=\
				"aescat: \n\r"\
				"use: dice:aescat <category>\n\r"\
				" <category>: one of\n\r"\
				"   gen\n\r"\
				"   cd\n\r"\
				"   other\n\r"\
				"   minidisc\n\r"\
				"   dvd\n\r"\
				"   pcm\n\r"\
				"   mixer\n\r"\
				"   src\n\r"\
				"   smp\n\r"\
				"   proc\n\r"\
				"   tape_dat\n\r"\
				"   tape_video\n\r"\
				"   tape_dcc\n\r"\
				"   synth\n\r"\
				"   mic\n\r"\
				"   ad\n\r"\
				"   ad_cpy\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set category for AES communication: \n\r";

static	CLIDescriptor	AES_CategoryDescriptor[]=
{
	{kCLI_Name,				(uint32)"aescat"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&aesSetCategory},
	{kCLI_UsageString, 		(uint32)AES_Category_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ConstantsString,	(uint32)"gen=0,cd=1,other=9,minidisc=73,dvd=25,pcm=2,mixer=24,src=26,smp=34,proc=42,tape_dat=3,tape_video=11,tape_dcc=67,synth=5,mic=13,ad=6,ad_cpy=22"},
	{kCLI_ModuleName,		(uint32)"Set AES category"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};




HRESULT blockSync_Tool(uint32 syncDst, uint32 syncSrc)
{

	switch(syncDst)
	{
		case 0: // Extern
			return clkSetExternBlocksync(syncSrc);
		case 1: // AES
			return clkSetAESTxBlocksync(syncSrc);
		case 2: // AVS
			return clkSetTXDI1Blocksync(syncSrc); 
		case 3: //
			return clkSetTXDI2Blocksync(syncSrc);
	}

	return E_FAIL;
}

static  char	BLOCKSYNC_UsageString[]=\
				"blockSync: \n\r"\
				"use: dice:blockSync <dst> <src>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Select source for block sync to destination:\n\r"\
				"<dst>: blko,aestx,txdi1,txdi2\n\r"\
				"<src>: aesRx,avsRx1,avsRx2,aesTx1,avsTx1,avsTx2\n\r";
static	CLIDescriptor	BLOCKSYNC_Descriptor[]=
{
	{kCLI_Name,				(uint32)"blockSync"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&blockSync_Tool},
	{kCLI_UsageString, 		(uint32)BLOCKSYNC_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"kk"},
	{kCLI_ConstantsString,	(uint32)"blko=0,aestx=1,txdi1=2,txdi2=3,aesRx=0,avsRx1=1,avsRx2=2,aesTx1=5,avsTx1=6,avsTx2=7"},
	{kCLI_ModuleName,		(uint32)"Select source for block sync"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT diceClkSync_Tool(uint32 value)
{
	return clkSetInSync(CLK_SYNC_ID1, value);
}
static  char	diceClkSync_UsageString[]=\
				"diceSync: \n\r"\
				"use: dice:sync <value>\n\r"\
				"------------------------------------------------------------\n\r"\
				"<value>: aes, slave, HPLL\n\r";
static	CLIDescriptor	diceClkSync_Descriptor[]=
{
	{kCLI_Name,				(uint32)"sync"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&diceClkSync_Tool},
	{kCLI_UsageString, 		(uint32)diceClkSync_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"aes=0,slave=2,HPLL=3"},
	{kCLI_ModuleName,		(uint32)"Set synchronizer source"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT diceClkMode_Tool(uint32 value)
{
	return clkSetInRouter(ROUTER_ID1, value);
}
static  char	diceClkMode_UsageString[]=\
				"dice:mode \n\r"\
				"use: dice:mdoe <value>\n\r"\
				"------------------------------------------------------------\n\r"\
				"<value>: 1x, 2x, 4x\n\r";
static	CLIDescriptor	diceClkMode_Descriptor[]=
{
	{kCLI_Name,				(uint32)"mode"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&diceClkMode_Tool},
	{kCLI_UsageString, 		(uint32)diceClkMode_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"1x=0,2x=1,4x=2"},
	{kCLI_ModuleName,		(uint32)"Set FS source for router"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT diceClkEClk_Tool(uint32 clkId, uint32 value)
{
	//zluo, hack for jr build
 	//return clkSetInExtern((uint8)clkId, (uint8)value);
	return NO_ERROR;
}
static  char	diceClkEClk_UsageString[]=\
				"dice:eclk \n\r"\
				"use: dice:eclk <clkId> <value>\n\r"\
				"------------------------------------------------------------\n\r"\
				"<clkId>: wclk0,wclk1,extbr\n\r"\
				"<value>: off,1fb,2fb,4fb, sys1fs, extfbr\n\r";
static	CLIDescriptor	diceClkEClk_Descriptor[]=
{
	{kCLI_Name,				(uint32)"eclk"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&diceClkEClk_Tool},
	{kCLI_UsageString, 		(uint32)diceClkEClk_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"kk"},
	{kCLI_ConstantsString,	(uint32)"wclk0=0,extbr=0,wclk1=1,off=0,1fb=1,2fb=2,4fb=3,sys1fs=4,extfbr=5"},
	{kCLI_ModuleName,		(uint32)"Set external clock"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT diceClkReset_Tool(void)
{
 	return clkInitialize();
}

static  char	diceClkReset_UsageString[]=\
				"dice:clkReset \n\r"\
				"use: dice:clkReset\n\r"\
				"------------------------------------------------------------\n\r"\
				"Reset dice clock\n\r";

static	CLIDescriptor	diceClkReset_Descriptor[]=
{
	{kCLI_Name,				(uint32)"clkReset"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&diceClkReset_Tool},
	{kCLI_UsageString, 		(uint32)diceClkReset_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"Reset clock"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	diceAESEnable_UsageString[]=\
				"diceAes:enable \n\r"\
				"use: diceaes:enable <bEnable>\n\r"\
				"------------------------------------------------------------\n\r"\
				"bEnable: off, on \n\r";

static	CLIDescriptor	diceAESEnable_Descriptor[]=
{
	{kCLI_Name,				(uint32)"enable"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&aesRxEnable},
	{kCLI_UsageString, 		(uint32)diceAESEnable_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"off=0,on=1"},
	{kCLI_ModuleName,		(uint32)"Enable AES"},
	{kCLI_GroupName,		(uint32)"diceaes"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	diceAESSetMasterClk_UsageString[]=\
				"diceAes:mClk <src> \n\r"\
				"use: diceaes:mclk <src>\n\r"\
				"------------------------------------------------------------\n\r"\
				"<src>: aes0, aes1, aes2, aes3, aesany\n\r";

static	CLIDescriptor	diceAESSetMasterClk_Descriptor[]=
{
	{kCLI_Name,				(uint32)"mclk"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&aesSetMasterClk},
	{kCLI_UsageString, 		(uint32)diceAESSetMasterClk_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"aes0=0,aes1=1,aes2=2,aes3=3,aesany=4"},
	{kCLI_ModuleName,		(uint32)"AES set master clock"},
	{kCLI_GroupName,		(uint32)"diceaes"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	diceAESDualwire_UsageString[]=\
				"diceAes:dw \n\r"\
				"use: diceaes:dw <bEnable>\n\r"\
				"------------------------------------------------------------\n\r"\
				"bEnable: off, on \n\r";

static	CLIDescriptor	diceAESDualwire_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dw"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&aesRxSetDualwire},
	{kCLI_UsageString, 		(uint32)diceAESDualwire_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"off=0,on=1"},
	{kCLI_ModuleName,		(uint32)"Set dual wire mode"},
	{kCLI_GroupName,		(uint32)"diceaes"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static  char	diceAdatEnable_UsageString[]=\
				"diceAdat:enable \n\r"\
				"use: diceAdat:enable <bEnable>\n\r"\
				"------------------------------------------------------------\n\r"\
				"bEnable: off, on \n\r";

static	CLIDescriptor	diceAdatEnable_Descriptor[]=
{
	{kCLI_Name,				(uint32)"enable"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&adatRxEnable},
	{kCLI_UsageString, 		(uint32)diceAdatEnable_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"off=0,on=1"},
	{kCLI_ModuleName,		(uint32)"Enable AES"},
	{kCLI_GroupName,		(uint32)"diceadat"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};



void hpllCliInstallTools(void);

void diceCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(clkSrc_Descriptor);
	hResult += cliInstallCLIDescriptor(Dump_Descriptor);
	hResult += cliInstallCLIDescriptor(AES_Descriptor);
	hResult += cliInstallCLIDescriptor(AES_ProModeDescriptor);
	hResult += cliInstallCLIDescriptor(AES_CopyProtectDescriptor);
	hResult += cliInstallCLIDescriptor(AES_PreEmphasisDescriptor);
	hResult += cliInstallCLIDescriptor(AES_CategoryDescriptor);
	hResult += cliInstallCLIDescriptor(BLOCKSYNC_Descriptor);
	hResult += cliInstallCLIDescriptor(diceClkSync_Descriptor);
	hResult += cliInstallCLIDescriptor(diceClkMode_Descriptor);
	hResult += cliInstallCLIDescriptor(diceClkEClk_Descriptor);
	hResult += cliInstallCLIDescriptor(diceClkReset_Descriptor);
	hResult += cliInstallCLIDescriptor(diceAESEnable_Descriptor);
	hResult += cliInstallCLIDescriptor(diceAESSetMasterClk_Descriptor);
	hResult += cliInstallCLIDescriptor(diceAESDualwire_Descriptor);
	hResult += cliInstallCLIDescriptor(diceAdatEnable_Descriptor);
	
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing dice cli tools");
	}

	//This is a hack, zluo
	hpllCliInstallTools();
}

static HRESULT hpllDivider_Tool(uint32 preDiv, uint32 addDiv)
{
 	return hpllSetDividers(0, preDiv, addDiv);
}

static  char	hpllDivider_UsageString[]=\
				"hpll:divider \n\r"\
				"use: hpll:divider preDiv addDiv\n\r"\
				"------------------------------------------------------------\n\r"\
				"\n\r";

static	CLIDescriptor	hpllDivider_Descriptor[]=
{
	{kCLI_Name,				(uint32)"divider"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&hpllDivider_Tool},
	{kCLI_UsageString, 		(uint32)hpllDivider_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ii"},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"Set HPLL Divider"},
	{kCLI_GroupName,		(uint32)"hpll"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT hpllPhaseOffset_Tool(uint32 phase)
{
 	return hpllSetPhaseOffset(0, phase);
}

static  char	hpllPhaseOffset_UsageString[]=\
				"hpll:phase \n\r"\
				"use: hpll:phase <value>\n\r"\
				"------------------------------------------------------------\n\r"\
				"\n\r";

static	CLIDescriptor	hpllPhaseOffset_Descriptor[]=
{
	{kCLI_Name,				(uint32)"phase"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&hpllPhaseOffset_Tool},
	{kCLI_UsageString, 		(uint32)hpllPhaseOffset_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"Set HPLL phase offset"},
	{kCLI_GroupName,		(uint32)"hpll"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT hpllJitterBW_Tool(uint32 floor, uint32 ceiling)
{
 	return hpllSetJitterBW(0, floor, ceiling);
}

static  char	hpllJitterBW_UsageString[]=\
				"hpll:jitter floor ceiling \n\r"\
				"use: hpll:jitter floor ceiling\n\r"\
				"------------------------------------------------------------\n\r"\
				"\n\r";

static	CLIDescriptor	hpllJitterBW_Descriptor[]=
{
	{kCLI_Name,				(uint32)"jitter"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&hpllJitterBW_Tool},
	{kCLI_UsageString, 		(uint32)hpllJitterBW_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ii"},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"Set HPLL jitter bandwidth"},
	{kCLI_GroupName,		(uint32)"hpll"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT hpllMaxGliderRate_Tool(uint32 locked, uint32 unlocked)
{
 	return hpllSetMaxGlideRate(0, locked, unlocked);
}

static  char	hpllMaxGliderRate_UsageString[]=\
				"hpll:glider locked unlocked \n\r"\
				"use: hpll:glider locked unlock\n\r"\
				"------------------------------------------------------------\n\r"\
				"\n\r";

static	CLIDescriptor	hpllMaxGliderRate_Descriptor[]=
{
	{kCLI_Name,				(uint32)"glider"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&hpllMaxGliderRate_Tool},
	{kCLI_UsageString, 		(uint32)hpllMaxGliderRate_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ii"},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"Set HPLL max glider rate"},
	{kCLI_GroupName,		(uint32)"hpll"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	hpllClock_UsageString[]=\
				"hpll:clock <value> \n\r"\
				"------------------------------------------------------------\n\r"\
				"\n\r";

static	CLIDescriptor	hpllClock_Descriptor[]=
{
	{kCLI_Name,				(uint32)"clock"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&hpllSetClock},
	{kCLI_UsageString, 		(uint32)hpllClock_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"Set HPLL clock"},
	{kCLI_GroupName,		(uint32)"hpll"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT hpllCoast_Tool(uint32 bCoast)
{
 	return hpllCoast(0, bCoast);
}

static  char	hpllCoast_UsageString[]=\
				"hpll:coast \n\r"\
				"use: hpll:coast <bCoast>\n\r"\
				"------------------------------------------------------------\n\r"\
				"bCoast: off, on\n\r";

static	CLIDescriptor	hpllCoast_Descriptor[]=
{
	{kCLI_Name,				(uint32)"coast"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&hpllCoast_Tool},
	{kCLI_UsageString, 		(uint32)hpllCoast_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"off=0,on=1"},
	{kCLI_ModuleName,		(uint32)"Set HPLL coast"},
	{kCLI_GroupName,		(uint32)"hpll"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static HRESULT hpllReference_Tool(uint32 bCoast)
{
 	return hpllSetReference(0, bCoast);
}

static  char	hpllReference_UsageString[]=\
				"hpll:ref \n\r"\
				"use: hpll:ref <value>\n\r"\
				"------------------------------------------------------------\n\r"\
				"value: 8 bit value \n\r";

static	CLIDescriptor	hpllReference_Descriptor[]=
{
	{kCLI_Name,				(uint32)"ref"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&hpllReference_Tool},
	{kCLI_UsageString, 		(uint32)hpllReference_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"Set HPLL reference"},
	{kCLI_GroupName,		(uint32)"hpll"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT hpllEnable_Tool(uint32 bEnable)
{
 	return hpllEnableClocks(0, bEnable);
}

static  char	hpllEnable_UsageString[]=\
				"hpll:enable \n\r"\
				"use: hpll:ref <bEnable>\n\r"\
				"------------------------------------------------------------\n\r"\
				"bEnable: off, on \n\r";

static	CLIDescriptor	hpllEnable_Descriptor[]=
{
	{kCLI_Name,				(uint32)"enable"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&hpllEnable_Tool},
	{kCLI_UsageString, 		(uint32)hpllEnable_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"off=0,on=1"},
	{kCLI_ModuleName,		(uint32)"Enable HPLL clock"},
	{kCLI_GroupName,		(uint32)"hpll"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	hpllReset_UsageString[]=\
				"hpll:reset \n\r"\
				"use: hpll:reset\n\r"\
				"------------------------------------------------------------\n\r"\
				"\n\r";

static	CLIDescriptor	hpllReset_Descriptor[]=
{
	{kCLI_Name,				(uint32)"reset"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&hpllInitialize},
	{kCLI_UsageString, 		(uint32)hpllReset_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"Reset HPLL"},
	{kCLI_GroupName,		(uint32)"hpll"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static HRESULT hpllDump_Tool(void)
{
	HRESULT hResult = NO_ERROR;
	uint32 v1, v2;
	if(hpllIsLocked(0)) 
		cliPrintf("HPLL is locked.\n\r");
	else
		cliPrintf("HPLL is not locked.\n\r");

	hResult += hpllGetClock(&v1);
	cliPrintf("\tclock: %d\n\r", v1);

	hResult += hpllGetDividers(0, &v1, &v2);
	cliPrintf("\tpreDiv: %d, addDiv: %d\n\r", v1, v2);
	
	hResult += hpllGetPeriod(0, (uint16*)&v1, (uint16*)&v2);
	cliPrintf("\tcount: %d, diff: %d\n\r", v1, v2);

 	return hResult;
}

static  char	hpllDump_UsageString[]=\
				"hpll:dump \n\r"\
				"use: hpll:dump\n\r"\
				"------------------------------------------------------------\n\r"\
				"\n\r";

static	CLIDescriptor	hpllDump_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dump"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&hpllDump_Tool},
	{kCLI_UsageString, 		(uint32)hpllDump_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"Dump HPLL status"},
	{kCLI_GroupName,		(uint32)"hpll"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void hpllCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;
	hResult += cliInstallCLIDescriptor(hpllDivider_Descriptor);
	hResult += cliInstallCLIDescriptor(hpllPhaseOffset_Descriptor);
	hResult += cliInstallCLIDescriptor(hpllJitterBW_Descriptor);
	hResult += cliInstallCLIDescriptor(hpllMaxGliderRate_Descriptor);
	hResult += cliInstallCLIDescriptor(hpllClock_Descriptor);
	hResult += cliInstallCLIDescriptor(hpllCoast_Descriptor);
	hResult += cliInstallCLIDescriptor(hpllReference_Descriptor);
	hResult += cliInstallCLIDescriptor(hpllEnable_Descriptor);
	hResult += cliInstallCLIDescriptor(hpllDump_Descriptor);
	hResult += cliInstallCLIDescriptor(hpllReset_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing hpll cli tools");
	}
}


#endif //_CLI_TOOL_DICE
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
