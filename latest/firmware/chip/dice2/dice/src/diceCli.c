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
	cliPrintf("\tRX_CLK_DMN_SEL : 0x%04x\n\r", pDiceClock->rxClkDmnSel.reg);
	cliPrintf("\tTX_CLK_DMN_SEL : 0x%04x\n\r", pDiceClock->txClkDmnSel.reg);
	cliPrintf("\tAESRX_MCK_SEL  : 0x%04x\n\r", pDiceClock->aesRxMckSel.reg);
	cliPrintf("\tADATRX_MCK_SEL : 0x%04x\n\r", pDiceClock->adatRxMckSel.reg);
	cliPrintf("\tTDIFRX_MCK_SEL : 0x%04x\n\r", pDiceClock->tdifRxMckSel.reg);
	cliPrintf("\tADAT_SMUX_SEL  : 0x%04x\n\r", pDiceClock->adatSMuxSel.reg);
	cliPrintf("\tPRESCALER_1    : 0x%04x\n\r", pDiceClock->prescaler1Sel);
	cliPrintf("\tPRESCALER_2    : 0x%04x\n\r", pDiceClock->prescaler2Sel);
	cliPrintf("\tHYBRID_PLL_SET : 0x%04x\n\r", pDiceClock->hybridPllSetup);
	cliPrintf("\tSR_COUNT_1     : 0x%04x\n\r", pDiceClock->srCount1);
	cliPrintf("\tSR_COUNT_2     : 0x%04x\n\r", pDiceClock->srCount2);
	cliPrintf("\tSR_COUNT_1MAX  : 0x%04x\n\r", pDiceClock->srCount1Max);
	cliPrintf("\tSR_COUNT_2MAX  : 0x%04x\n\r", pDiceClock->srCount2Max);
}

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
		cliPrintf("\tROUTER1_ENTRY[%03i]  : 0x%04x\n\r", i, routerEntry.reg);
	}

	cliPrintf("Router 2 registers:\n\r");
	
	diceRegRead(DICE_ROUTER2_SETUP, &routerSetup.reg);
	cliPrintf("\tROUTER2_SETUP       : 0x%04x\n\r", routerSetup.reg);
	
	for (i = 0; i < routerSetup.bit.numEntry+1; i++)
	{
		diceRegRead((DICE_ROUTER2_ENTRY+(i*4)), &routerEntry.reg);
		cliPrintf("\tROUTER2_ENTRY[%03i]  : 0x%04x\n\r", i, routerEntry.reg);
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
		cliPrintf("0x%02x ", pDiceAESRx->csChannel[ch][csb++]);
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
		cliPrintf("0x%02x ", pDiceAESTx->csChannel[ch][csb++]);
		if (csb%8 == 0) cliPrintf("\n\r"); // Next line
	}
}

static void dumpADAT(void)
{
	BOOL	bEnable;
	BOOL	bLocked;
	uADATRX_STATUS error;
	
	adatRxGetEnable (&bEnable);
	bLocked = adatIsLocked ();
	adatRxGetError(&error, TRUE);
	
	cliPrintf("ADAT Rx:\n\r");
	cliPrintf("\tState           : %s\n\r", bEnable ? (bLocked ? "Locked" : "Unlocked") : "Disabled");	
	cliPrintf("ADAT Errors  R=Repeat, S=Slip\n\r");
	cliPrintf("\tErrors          : %c%c%\n\r", error.bit.u_run ? 'R' : ' ', error.bit.o_run ? 'S' : ' ');
	cliPrintf("ADAT Rx registers:\n\r");
	cliPrintf("\tSETUP          : 0x%04x\n\r", pDiceADATRx->setup.reg);
	cliPrintf("\tSTATUS         : 0x%04x\n\r", pDiceADATRx->status.reg);
	cliPrintf("ADAT Tx registers:\n\r");
	cliPrintf("\tUSER_SETUP     : 0x%04x\n\r", pDiceADATTx->userSetup.reg);
	cliPrintf("\tMUX_SETUP      : 0x%04x\n\r", pDiceADATTx->muxSetup.reg);
	cliPrintf("\tCH_MUTE        : 0x%04x\n\r", pDiceADATTx->chMute.reg);	
}


static void dumpTDIF(void)
{
	cliPrintf("TDIF Rx registers:\n\r");
	cliPrintf("\tSETUP          : 0x%04x\n\r", pDiceTDIFRx->setup.reg);
	cliPrintf("\tCH_SETUP[0]    : 0x%04x\n\r", pDiceTDIFRx->chSetup[0].reg);
	cliPrintf("\tCH_SETUP[1]    : 0x%04x\n\r", pDiceTDIFRx->chSetup[1].reg);
	cliPrintf("\tCH_SETUP[2]    : 0x%04x\n\r", pDiceTDIFRx->chSetup[2].reg);
	cliPrintf("\tCH_SETUP[3]    : 0x%04x\n\r", pDiceTDIFRx->chSetup[3].reg);
	cliPrintf("\tSTATUS         : 0x%04x\n\r", pDiceTDIFRx->status.reg);
	cliPrintf("\tPHASE_DIFF     : 0x%04x\n\r", pDiceTDIFRx->phaseDiff);
	cliPrintf("\tINVERSION      : 0x%04x\n\r", pDiceTDIFRx->inversion.reg);
	cliPrintf("TDIF Tx registers:\n\r");
	cliPrintf("\tCH_SETUP[0]    : 0x%04x\n\r", pDiceTDIFTx->chSetup[0].reg);
	cliPrintf("\tCH_SETUP[1]    : 0x%04x\n\r", pDiceTDIFTx->chSetup[1].reg);
	cliPrintf("\tCH_SETUP[2]    : 0x%04x\n\r", pDiceTDIFTx->chSetup[2].reg);
	cliPrintf("\tCH_SETUP[3]    : 0x%04x\n\r", pDiceTDIFTx->chSetup[3].reg);
	cliPrintf("\tCH_SETUP[4]    : 0x%04x\n\r", pDiceTDIFTx->chSetup[4].reg);
	cliPrintf("\tCH_SETUP[5]    : 0x%04x\n\r", pDiceTDIFTx->chSetup[5].reg);
	cliPrintf("\tCH_SETUP[6]    : 0x%04x\n\r", pDiceTDIFTx->chSetup[6].reg);
	cliPrintf("\tCH_SETUP[7]    : 0x%04x\n\r", pDiceTDIFTx->chSetup[7].reg);
	cliPrintf("\tCH_MUTE        : 0x%04x\n\r", pDiceTDIFTx->chMute.reg);
	cliPrintf("\tINVERSION      : 0x%04x\n\r", pDiceTDIFTx->inversion.reg);
}

static void dumpDSAI(void)
{
	cliPrintf("DSAI0 Rx registers:\n\r");
	cliPrintf("\tSETUP          : 0x%04x\n\r", pDiceDSAIRx0->setup.reg);
	cliPrintf("\tMASTER_SETUP   : 0x%04x\n\r", pDiceDSAIRx0->masterSetup.reg);
	cliPrintf("\tSAMPLE_ERROR   : 0x%04x\n\r", pDiceDSAIRx0->sampleError);
	cliPrintf("DSAI0 Tx registers:\n\r");
	cliPrintf("\tSETUP          : 0x%04x\n\r", pDiceDSAITx0->setup.reg);
	cliPrintf("\tMASTER_SETUP   : 0x%04x\n\r", pDiceDSAITx0->masterSetup.reg);
	cliPrintf("\tSAMPLE_ERROR   : 0x%04x\n\r", pDiceDSAITx0->sampleError);
	cliPrintf("\tCH_MUTE        : 0x%04x\n\r", pDiceDSAITx0->txMuteCh.reg);
}

static void dumpI2S(void)
{
	cliPrintf("I2S0 Rx registers:\n\r");
	cliPrintf("\tCLK_SETUP      : 0x%04x\n\r", pDiceI2SRx0->clkSetup.reg);
	cliPrintf("\tCH_SETUP[0]    : 0x%04x\n\r", pDiceI2SRx0->chSetup[0].reg);
	cliPrintf("\tCH_SETUP[1]    : 0x%04x\n\r", pDiceI2SRx0->chSetup[1].reg);
	cliPrintf("\tCH_SETUP[2]    : 0x%04x\n\r", pDiceI2SRx0->chSetup[2].reg);
	cliPrintf("\tCH_SETUP[3]    : 0x%04x\n\r", pDiceI2SRx0->chSetup[3].reg);
	cliPrintf("I2S0 Tx registers:\n\r");
	cliPrintf("\tCLK_SETUP      : 0x%04x\n\r", pDiceI2STx0->clkSetup.reg);
	cliPrintf("\tCH_SETUP[0]    : 0x%04x\n\r", pDiceI2STx0->chSetup[0].reg);
	cliPrintf("\tCH_SETUP[1]    : 0x%04x\n\r", pDiceI2STx0->chSetup[1].reg);
	cliPrintf("\tCH_SETUP[2]    : 0x%04x\n\r", pDiceI2STx0->chSetup[2].reg);
	cliPrintf("\tCH_SETUP[3]    : 0x%04x\n\r", pDiceI2STx0->chSetup[3].reg);
	cliPrintf("\tCH_MUTE        : 0x%04x\n\r", pDiceI2STx0->chMute.reg);
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

		case 3: // DSAI
			dumpDSAI();
			break;

		case 4: // ADAT
			dumpADAT();
			break;

		case 5: // TDIF
			dumpTDIF();
			break;

		case 6: // I2S
			dumpI2S();
			break;

		case 7: // HPLL1
			dumpHPLL(pDiceHPLL1);
			break;

		case 8: // HPLL2
			dumpHPLL(pDiceHPLL2);
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




HRESULT blockSync_Tool(uint32 syncDst, uint32 syncSrc)
{

	switch(syncDst)
	{
		case 0: // Extern
			return clkSetExternBlocksync(syncSrc);
		case 1: // AES
			return clkSetAESTxBlocksync(syncSrc);
		case 2: // AVS
			return clkSetAVSTxBlocksync(0, syncSrc); // AVS1 as default
	}

	return E_FAIL;
}


HRESULT dsaiSetMode(uint32 mode)
{

	switch (mode)
	{
		case 0: // 1Bit
			dsaiSetSyncLen(DSAI_ID0, DSAI_RX | DSAI_TX, SYNC_LEN_1_CLK);
			dsaiSetDataDelay(DSAI_ID0, DSAI_RX | DSAI_TX, DELAY_1_CLK);
			break;
		case 1: // 32Bit
			dsaiSetSyncLen(DSAI_ID0, DSAI_RX | DSAI_TX, SYNC_LEN_32_CLK);
			dsaiSetDataDelay(DSAI_ID0, DSAI_RX | DSAI_TX, DELAY_0_CLK);
			break;
		case 2:	// Master
			dsaiSetMaster(DSAI_ID0, DSAI_RX | DSAI_TX, TRUE);
			break;
		case 3: // Slave
			dsaiSetMaster(DSAI_ID0, DSAI_RX | DSAI_TX, FALSE);
			break;
	}

	return NO_ERROR;
}

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


static  char	Sync_UsageString[]=\
				"sync: \n\r"\
				"use: dice:sync <0|1> <source>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Select synchronizer input clock source: \n\r"\
				"44k, 48k, ext, hpll, aes, adat, tdif \n\r";
static	CLIDescriptor	Sync_Descriptor[]=
{
	{kCLI_Name,				(uint32)"sync"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&clkSetInSync},
	{kCLI_UsageString, 		(uint32)Sync_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"dk"},
	{kCLI_ConstantsString,	(uint32)"44k=0,48k=1,ext=2,hpll=3,aes=4,adat=5,tdif=6"},
	{kCLI_ModuleName,		(uint32)"Sync input clock source"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	HPLL_UsageString[]=\
				"hpll: \n\r"\
				"use: dice:hpll <0|1> <source>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Select hybrid pll input clock source: \n\r"\
				"aes, adat, tdif, wc, avs1, pre, ext, dsaitx0, dsairx0 \n\r";
static	CLIDescriptor	HPLL_Descriptor[]=
{
	{kCLI_Name,				(uint32)"hpll"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&clkSetInHybridPll},
	{kCLI_UsageString, 		(uint32)HPLL_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"di"},
	{kCLI_ConstantsString,	(uint32)"aes=0,adat=1,tdif=2,wc=3,avs1=4,pre=8,ext=9,dsairx0=10,dsaitx0=14"},
	{kCLI_ModuleName,		(uint32)"Hybrid PLL input clock source"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static  char	Dump_UsageString[]=\
				"dump: \n\r"\
				"use: dice:dump <module>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Dump register contents for module: \n\r"\
				"clock, aes, router, dsai, adat, tdif, i2s, hpll1, hpll2, aesRx, aesTx\n\r";
static	CLIDescriptor	Dump_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dump"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&diceDump_Tool},
	{kCLI_UsageString, 		(uint32)Dump_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"clock=0,aes=1,router=2,dsai=3,adat=4,tdif=5,i2s=6,hpll1=7,hpll2=8,aesRx=9,aesTx=10,clkState=11"},
	{kCLI_ModuleName,		(uint32)"Dump register contents"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static  char	DSAI_UsageString[]=\
				"dsai: \n\r"\
				"use: dice:dsai <1bit|32bit|master|slave>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set mode for DSAI communication: \n\r"\
				"1-bit or 32-bit frame sync, master or slave clock mode\n\r";
static	CLIDescriptor	DSAI_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dsai"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&dsaiSetMode},
	{kCLI_UsageString, 		(uint32)DSAI_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"k"},
	{kCLI_ConstantsString,	(uint32)"1bit=0,32bit=1,master=2,slave=3"},
	{kCLI_ModuleName,		(uint32)"Set DSAI communication mode"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

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


static  char	BLOCKSYNC_UsageString[]=\
				"blockSync: \n\r"\
				"use: dice:blockSync <dst> <src>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Select source for block sync to destination:\n\r"\
				"<dst>: ext, aes, avs\n\r"\
				"<src>: aesRx, avsRx, aesTx, avsTx, extIn\n\r";
static	CLIDescriptor	BLOCKSYNC_Descriptor[]=
{
	{kCLI_Name,				(uint32)"blockSync"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&blockSync_Tool},
	{kCLI_UsageString, 		(uint32)BLOCKSYNC_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"kk"},
	{kCLI_ConstantsString,	(uint32)"ext=0,aes=1,avs=2,aesRx=0,avsRx=1,aesTx=5,avsTx=6,extIn=5"},
	{kCLI_ModuleName,		(uint32)"Select source for block sync"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static  char	I2S_RSMF_UsageString[]=\
				"rsmf: \n\r"\
				"use: dice:rsmf i2sId on/off \n\r"\
				"------------------------------------------------------------\n\r"\
				"Rx Set Master Frequency\n\r";
static	CLIDescriptor	I2S_RSMF_Descriptor[]=
{
	{kCLI_Name,				(uint32)"rsmf"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&i2sRxSetMckFreq},
	{kCLI_UsageString, 		(uint32)I2S_RSMF_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"dk"},
	{kCLI_ConstantsString,	(uint32)"off=0,on=1"},
	{kCLI_ModuleName,		(uint32)"I2S Rx Set Master Frequency"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	I2S_RS192_UsageString[]=\
				"rs192: \n\r"\
				"use: dice:rs192 i2sId on/off \n\r"\
				"------------------------------------------------------------\n\r"\
				"Rx Set Rx 192KHz mode\n\r";
static	CLIDescriptor	I2S_RS192_Descriptor[]=
{
	{kCLI_Name,				(uint32)"rs192"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&i2sRxSetM192},
	{kCLI_UsageString, 		(uint32)I2S_RS192_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"dk"},
	{kCLI_ConstantsString,	(uint32)"off=0,on=1"},
	{kCLI_ModuleName,		(uint32)"I2S Rx Set 192KHz mode"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};



static  char	I2S_RSC_UsageString[]=\
				"rsc: \n\r"\
				"use: dice:rsc i2sId on/off mckOutDiv\n\r"\
				"------------------------------------------------------------\n\r"\
				"Enable mast clock output and set divider\n\r";
static	CLIDescriptor	I2S_RSC_Descriptor[]=
{
	{kCLI_Name,				(uint32)"rsc"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&i2sRxSetClock},
	{kCLI_UsageString, 		(uint32)I2S_RSC_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"dkd"},
	{kCLI_ConstantsString,	(uint32)"off=0,on=1"},
	{kCLI_ModuleName,		(uint32)"I2S Rx enable Master Clock output and set divider"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static  char	I2S_RSCI_UsageString[]=\
				"rsci: \n\r"\
				"use: dice:rsci i2sId on/off bickInv\n\r"\
				"------------------------------------------------------------\n\r"\
				"I2S Invert clock signal\n\r";
static	CLIDescriptor	I2S_RSCI_Descriptor[]=
{
	{kCLI_Name,				(uint32)"rsci"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&i2sRxSetClockInv},
	{kCLI_UsageString, 		(uint32)I2S_RSCI_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"dkd"},
	{kCLI_ConstantsString,	(uint32)"off=0,on=1"},
	{kCLI_ModuleName,		(uint32)"I2S Rx Invert Clock signal"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static  char	I2S_RSDF_UsageString[]=\
				"rsdf: \n\r"\
				"use: dice:rsdf i2sId i2sRxCh msbfirst[on/off] lJustify[on/off] \n\r"\
				"               dataSize[24b/32b] dataDelay[on/off]             \n\r"\
				"------------------------------------------------------------\n\r"\
				"I2S Rx Set Data Format, L/R justify, data size & data delay \n\r";
static	CLIDescriptor	I2S_RSDF_Descriptor[]=
{
	{kCLI_Name,				(uint32)"rsdf"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&i2sRxSetDataFormat},
	{kCLI_UsageString, 		(uint32)I2S_RSDF_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ddkkkk"},
	{kCLI_ConstantsString,	(uint32)"off=0,on=1,24b=0,32b=1"},
	{kCLI_ModuleName,		(uint32)"I2S Rx Set Data Format and more"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static  char	I2S_TSMF_UsageString[]=\
				"tsmf: \n\r"\
				"use: dice:tsmf i2sId on/off \n\r"\
				"------------------------------------------------------------\n\r"\
				"Tx Set Master Frequency\n\r";
static	CLIDescriptor	I2S_TSMF_Descriptor[]=
{
	{kCLI_Name,				(uint32)"tsmf"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&i2sTxSetMckFreq},
	{kCLI_UsageString, 		(uint32)I2S_TSMF_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"dk"},
	{kCLI_ConstantsString,	(uint32)"off=0,on=1"},
	{kCLI_ModuleName,		(uint32)"I2S Tx Set Master Frequency"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	I2S_TS192_UsageString[]=\
				"ts192: \n\r"\
				"use: dice:ts192 i2sId on/off \n\r"\
				"------------------------------------------------------------\n\r"\
				"Rx Set Tx 192KHz mode\n\r";
static	CLIDescriptor	I2S_TS192_Descriptor[]=
{
	{kCLI_Name,				(uint32)"ts192"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&i2sTxSetM192},
	{kCLI_UsageString, 		(uint32)I2S_TS192_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"dk"},
	{kCLI_ConstantsString,	(uint32)"off=0,on=1"},
	{kCLI_ModuleName,		(uint32)"I2S Tx Set 192KHz mode"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};



static  char	I2S_TSC_UsageString[]=\
				"tsc: \n\r"\
				"use: dice:tsc i2sId on/off mckOutDiv\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set master Clock output and set divider\n\r";
static	CLIDescriptor	I2S_TSC_Descriptor[]=
{
	{kCLI_Name,				(uint32)"tsc"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&i2sTxSetClock},
	{kCLI_UsageString, 		(uint32)I2S_TSC_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"dkd"},
	{kCLI_ConstantsString,	(uint32)"off=0,on=1"},
	{kCLI_ModuleName,		(uint32)"I2S Tx Set master Clock output and set divider"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static  char	I2S_TSCI_UsageString[]=\
				"tsci: \n\r"\
				"use: dice:tsci i2sId on/off bickInv\n\r"\
				"------------------------------------------------------------\n\r"\
				"I2S Invert clock signal\n\r";
static	CLIDescriptor	I2S_TSCI_Descriptor[]=
{
	{kCLI_Name,				(uint32)"tsci"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&i2sTxSetClockInv},
	{kCLI_UsageString, 		(uint32)I2S_TSCI_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"dkd"},
	{kCLI_ConstantsString,	(uint32)"off=0,on=1"},
	{kCLI_ModuleName,		(uint32)"I2S Tx Invert Clock signal"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static  char	I2S_TSDF_UsageString[]=\
				"tsdf: \n\r"\
				"use: dice:tsdf i2sId i2sRxCh msbfirt[on/off] lJustify[on/off] \n\r"\
				"               dataSize[24b/32b] dataDelay[on/off]             \n\r"\
				"------------------------------------------------------------\n\r"\
				"I2S Tx Set Data Format, L/R justify, data size & data delay \n\r";
static	CLIDescriptor	I2S_TSDF_Descriptor[]=
{
	{kCLI_Name,				(uint32)"tsdf"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&i2sTxSetDataFormat},
	{kCLI_UsageString, 		(uint32)I2S_TSDF_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ddkkkk"},
	{kCLI_ConstantsString,	(uint32)"off=0,on=1,24b=0,32b=1"},
	{kCLI_ModuleName,		(uint32)"I2S Tx Set Data Format and more"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

//This function contains the default I2S setting for EVM.
static HRESULT diceI2sCliSetDefault(void)
{
	// I2S Receivers
	i2sRxSetMckFreq		(I2S_ID0, 0);
	i2sRxSetM192		(I2S_ID0, 0);	
	i2sRxSetClock		(I2S_ID0, TRUE, 0);
	i2sRxSetClockInv	(I2S_ID0, 0, 0);
	i2sRxSetDataFormat	(I2S_ID0, I2S_CH0, 1, 1, 0, 0);
	i2sRxSetDataFormat	(I2S_ID0, I2S_CH1, 1, 1, 0, 0);
	i2sRxSetDataFormat	(I2S_ID0, I2S_CH2, 1, 1, 0, 0);
	i2sRxSetDataFormat	(I2S_ID0, I2S_CH3, 1, 1, 0, 0);
	
	i2sRxSetMckFreq		(I2S_ID1, 0);
	i2sRxSetM192		(I2S_ID1, 0);	
	i2sRxSetClock		(I2S_ID1, TRUE, 0);
	i2sRxSetClockInv	(I2S_ID1, 0, 0);
	i2sRxSetDataFormat	(I2S_ID1, I2S_CH0, 1, 1, 0, 0);
	i2sRxSetDataFormat	(I2S_ID1, I2S_CH1, 1, 1, 0, 0);

	i2sRxSetMckFreq		(I2S_ID2, 0);
	i2sRxSetM192		(I2S_ID2, 0);	
	i2sRxSetClock		(I2S_ID2, TRUE, 0);
	i2sRxSetClockInv	(I2S_ID2, 0, 0);
	i2sRxSetDataFormat	(I2S_ID2, I2S_CH0, 1, 1, 0, 0);
	i2sRxSetDataFormat	(I2S_ID2, I2S_CH1, 1, 1, 0, 0);

	// I2S Transmitters
	i2sTxSetMckFreq		(I2S_ID0, 0);
	i2sTxSetM192		(I2S_ID0, 0);
	i2sTxSetClock		(I2S_ID0, TRUE, 0);
	i2sTxSetClockInv	(I2S_ID0, 0, 0);
	i2sTxSetDataFormat	(I2S_ID0, I2S_CH0, 1, 1, 0, 0);
	i2sTxSetDataFormat	(I2S_ID0, I2S_CH1, 1, 1, 0, 0);
	i2sTxSetDataFormat	(I2S_ID0, I2S_CH2, 1, 1, 0, 0);
	i2sTxSetDataFormat	(I2S_ID0, I2S_CH3, 1, 1, 0, 0);

	i2sTxSetMckFreq		(I2S_ID1, 0);
	i2sTxSetM192		(I2S_ID1, 0);
	i2sTxSetClock		(I2S_ID1, TRUE, 0);
	i2sTxSetClockInv	(I2S_ID1, 0, 0);
	i2sTxSetDataFormat	(I2S_ID1, I2S_CH0, 1, 1, 0, 0);
	i2sTxSetDataFormat	(I2S_ID1, I2S_CH1, 1, 1, 0, 0);

	i2sTxSetMckFreq		(I2S_ID2, 0);
	i2sTxSetM192		(I2S_ID2, 0);
	i2sTxSetClock		(I2S_ID2, TRUE, 0);
	i2sTxSetClockInv	(I2S_ID2, 0, 0);
	i2sTxSetDataFormat	(I2S_ID2, I2S_CH0, 1, 1, 0, 0);
	i2sTxSetDataFormat	(I2S_ID2, I2S_CH1, 1, 1, 0, 0);

	return NO_ERROR;
}
static  char	I2S_DEFAULT_UsageString[]=\
				"i2s_default: \n\r"\
				"use: dice:i2s_default       \n\r"\
				"------------------------------------------------------------\n\r"\
				"Default I2S setting \n\r";
static	CLIDescriptor	I2S_DEFAULT_Descriptor[]=
{
	{kCLI_Name,				(uint32)"i2s_default"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&diceI2sCliSetDefault},
	{kCLI_UsageString, 		(uint32)I2S_DEFAULT_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"I2S default setting"},
	{kCLI_GroupName,		(uint32)"dice"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void diceCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(Sync_Descriptor);
	hResult += cliInstallCLIDescriptor(HPLL_Descriptor);
	hResult += cliInstallCLIDescriptor(Dump_Descriptor);
	hResult += cliInstallCLIDescriptor(DSAI_Descriptor);
	hResult += cliInstallCLIDescriptor(AES_Descriptor);
	hResult += cliInstallCLIDescriptor(BLOCKSYNC_Descriptor);
	hResult += cliInstallCLIDescriptor(I2S_RSMF_Descriptor);
	hResult += cliInstallCLIDescriptor(I2S_RS192_Descriptor);
	hResult += cliInstallCLIDescriptor(I2S_RSC_Descriptor);
	hResult += cliInstallCLIDescriptor(I2S_RSCI_Descriptor);
	hResult += cliInstallCLIDescriptor(I2S_RSDF_Descriptor);
	hResult += cliInstallCLIDescriptor(I2S_TSMF_Descriptor);
	hResult += cliInstallCLIDescriptor(I2S_TS192_Descriptor);
	hResult += cliInstallCLIDescriptor(I2S_TSC_Descriptor);
	hResult += cliInstallCLIDescriptor(I2S_TSCI_Descriptor);
	hResult += cliInstallCLIDescriptor(I2S_TSDF_Descriptor);
	hResult += cliInstallCLIDescriptor(I2S_DEFAULT_Descriptor);
	
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing avs cli tools");
	}
}

#endif //_CLI_TOOL_DICE
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
