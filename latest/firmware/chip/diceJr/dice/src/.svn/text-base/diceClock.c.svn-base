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
	
	Purpose:	DICE clock module API
	
	Revisions:
		created 12/06/2003 kbj

****************************************************************************/

#include <string.h>
#include "TCTypes.h"
#include "TCDebug.h"
#include "ErrorCodes.h"
#include "diceDefs.h"
#include "diceClock.h"

static char 	moduleName[] = __FILE__ ;

DICE_CLOCK *pDiceClock = (DICE_CLOCK *)(DICEBASE + DICE_CLOCK_BASE);

static DICE_CLOCK diceClockShadow;

#include "sysCli.h"

#ifdef _SYS_VERBOSE
_TC_INLINE_STATIC void clkRegRead(uint32 *addr, uint32 *value) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void clkRegRead(uint32 *addr, uint32 *value)
// Read a value from the CLK
{
	*value = *addr;

	if (sysCliVerboseIsEnabled(SYS_VERBOSE_CLK_RD))
	{
		sysDebugPrintf("clk get 0x%08x:0x%08x\n\r", addr, *value);
	}
}

_TC_INLINE_STATIC void clkRegWrite(uint32 *addr, uint32 value) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void clkRegWrite(uint32 *addr, uint32 value)
// Write a value to the CLK
{
	if (sysCliVerboseIsEnabled(SYS_VERBOSE_CLK_WR))
	{
		if (sysCliVerboseIsEnabled(SYS_VERBOSE_CLK_DIFF))
		{
			uint32 old = *addr;
			*addr = value;
			sysDebugPrintf("clk set 0x%08x:0x%08x\t(old/new 0x%08x:0x%08x)\n\r", addr, value, old, *addr);
		}
		else
		{
			*addr = value;
			sysDebugPrintf("clk set 0x%08x:0x%08x\n\r", addr, value);
		}
	}
	else
	{
		*addr = value;
	}
}

_TC_INLINE_STATIC void clkBlkWrite(uint32 *dst, uint32 *src, uint32 size) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void clkBlkWrite(uint32 *dst, uint32 *src, uint32 size)
// Write a block to the CLK
{
	if (sysCliVerboseIsEnabled(SYS_VERBOSE_CLK_WR))
	{
		uint32 i;

		for (i = 0; i < size / 4; i++)
		{
			uint32 *addr = &(dst[i]);
			uint32 value = src[i];

			if (sysCliVerboseIsEnabled(SYS_VERBOSE_CLK_DIFF))
			{
				uint32 old = *addr;
				*addr = value;
				sysDebugPrintf("clk set 0x%08x:0x%08x\t(old/new 0x%08x:0x%08x)\n\r", addr, value, old, *addr);
			}
			else
			{
				*addr = value;
				sysDebugPrintf("clk set 0x%08x:0x%08x\n\r", addr, value);
			}
		}
	}
	else
	{
		memcpy(dst, src, size);
	}
}

#else //_SYS_VERBOSE
_TC_INLINE_STATIC void clkRegRead(uint32 *addr, uint32 *value) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void clkRegRead(uint32 *addr, uint32 *value)
// Read a value from the CLK
{
	*value = *addr;
}

_TC_INLINE_STATIC void clkRegWrite(uint32 *addr, uint32 value) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void clkRegWrite(uint32 *addr, uint32 value)
// Write a value to the CLK
{
	*addr = value;
}

_TC_INLINE_STATIC void clkBlkWrite(uint32 *dst, uint32 *src, uint32 size) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void clkBlkWrite(uint32 *dst, uint32 *src, uint32 size)
// Write a block to the CLK
{
	memcpy(dst, src, size);
}
#endif //_SYS_VERBOSE


#define CLOCKDOUBLER_SETTING			0x19402	// Selects clock doubler as clock
#define PHYCLOCK_SETTING				0x19403	// Selects clock doubler as clock
#ifdef _PLLE_IS_HIGH
#define HPLL_INIT_SETTING				CLOCKDOUBLER_SETTING
#else
#define HPLL_INIT_SETTING				PHYCLOCK_SETTING
#endif

// Clear everything 
HRESULT clkInitialize(void)
{
	HRESULT hResult = NO_ERROR;

#if 0 //def _SYS_VERBOSE
	sysCliSetVerbose(SYS_VERBOSE_CLK_ALL);
#endif //_SYS_VERBOSE

	memset(&diceClockShadow, 0, sizeof(DICE_CLOCK));
	diceClockShadow.hybridPllSetup = HPLL_INIT_SETTING;
	
	diceClockShadow.srCount1Max = diceClockShadow.srCount2Max = 0xFFFF;
	diceClockShadow.syncClkSel.bit.sync1_in_sel = SYNC_CLK_HYBRID_PLL;
	//diceClockShadow.syncClkSel.bit.sync2_in_sel = SYNC_CLK_HYBRID_PLL;
	diceClockShadow.hpllClkSel.bit.ref_event1_sel = HPLL_CLK_AESRX;
	//diceClockShadow.hpllClkSel.bit.ref_event2_sel = HPLL_CLK_PRESCALER;
	diceClockShadow.routerClkSel.bit.fs1_router1_sel = LOW_RATE;
	//diceClockShadow.routerClkSel.bit.fs256_router1_sel = ROUTER_CLK_48K_SAME;
	//diceClockShadow.routerClkSel.bit.fs1_router2_sel = ROUTER_CLK_48K_SAME;
	//diceClockShadow.routerClkSel.bit.fs256_router2_sel = ROUTER_CLK_48K_SAME;
	diceClockShadow.wcEcSel.bit.extbr_wclk0 = 0x5;
	diceClockShadow.wcEcSel.bit.wclk1 = 0x4;


	diceClockShadow.aesRxMckSel.bit.regen_vco_ext_clk_sel = INTERNAL_VCO;
	diceClockShadow.aesRxMckSel.bit.regen_vco_pwrdn = DISABLE_RX_VCO;
	diceClockShadow.aesRxMckSel.bit.regen_vco_pre_scaler = 0x3e;
	diceClockShadow.aesRxMckSel.bit.regen_vco_main_divider = 0xf8;
	diceClockShadow.aesRxMckSel.bit.regen_vco_post_scaler = POST_DIV_BY_2;
	diceClockShadow.aesRxMckSel.bit.regen_vco_up_pol = SIGNAL_ACTIVE_HIGH;

	
	clkBlkWrite((uint32 *) pDiceClock, (uint32 *) &diceClockShadow, sizeof(DICE_CLOCK));

	return hResult;
}

// Clock routing for synchronizer 1,2
HRESULT clkSetInSync(uint8 clkSyncId, uint8 syncClk)
{
	HRESULT hResult = NO_ERROR;
	
	switch (clkSyncId)
	{
		case CLK_SYNC_ID1:
			diceClockShadow.syncClkSel.bit.sync1_in_sel = syncClk;
			break;
		default:
			hResult = E_DICE_BAD_INPUT_PARAM;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	clkRegWrite(&(pDiceClock->syncClkSel.reg), diceClockShadow.syncClkSel.reg);

	return hResult;
}

HRESULT clkGetInSync(uint8 clkSyncId, uint8 *syncClk)
{
	HRESULT hResult = NO_ERROR;
	
	clkRegRead(&(pDiceClock->syncClkSel.reg), &(diceClockShadow.syncClkSel.reg));

	switch (clkSyncId)
	{
		case CLK_SYNC_ID1:
			*syncClk = diceClockShadow.syncClkSel.bit.sync1_in_sel;
			break;
		default:
			hResult = E_DICE_BAD_INPUT_PARAM;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	return hResult;
}

// Clock routing for hybrid pll 1,2
HRESULT clkSetInHybridPll(uint8 hpllClk)
{
	HRESULT hResult = NO_ERROR;
	
	diceClockShadow.hpllClkSel.bit.ref_event1_sel = hpllClk;
	clkRegWrite(&(pDiceClock->hpllClkSel.reg), diceClockShadow.hpllClkSel.reg);

	return hResult;
}

HRESULT clkGetInHybridPll(uint8 *hpllClk)
{
	HRESULT hResult = NO_ERROR;
	
	clkRegRead(&(pDiceClock->hpllClkSel.reg), &(diceClockShadow.hpllClkSel.reg));

	*hpllClk = diceClockShadow.hpllClkSel.bit.ref_event1_sel;

	return hResult;
}

// Clock routing for router 1,2
HRESULT clkSetInRouter(uint8 routerId, uint8 routerClk)
{
	HRESULT hResult = NO_ERROR;
	
	switch (routerId)
	{
		case ROUTER_ID1:
			diceClockShadow.routerClkSel.bit.fs1_router1_sel = routerClk;
			break;
		default:
			hResult = E_DICE_BAD_INPUT_PARAM;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	clkRegWrite(&(pDiceClock->routerClkSel.reg), diceClockShadow.routerClkSel.reg);

	return hResult;
}

HRESULT clkGetInRouter(uint8 routerId, uint8 *routerClk)
{	
	HRESULT hResult = NO_ERROR;
	
	clkRegRead(&(pDiceClock->routerClkSel.reg), &(diceClockShadow.routerClkSel.reg));

	switch (routerId)
	{
		case ROUTER_ID1:
			*routerClk = diceClockShadow.routerClkSel.bit.fs1_router1_sel;
			break;
		default:
			hResult = E_DICE_BAD_INPUT_PARAM;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	return hResult;
}

// Set clock routing for receive device 
HRESULT clkSetInRxDev(DEV_RX_TYPES rxDevType, uint8 routerId)
{
	HRESULT hResult = E_GEN_ILLEGAL_PARAM;
	return hResult;
}

// Set clock routing for transmit device 
HRESULT clkSetInTxDev(DEV_TX_TYPES txDevType, uint8 routerId)
{
	HRESULT hResult = E_GEN_ILLEGAL_PARAM;
	return hResult;
}

// Set pre scale clock divider for device only AES, ADAT and TDIF
HRESULT clkSetPreDivider(DEV_RX_TYPES rxDevType, uint8 preDivider)
{
	HRESULT hResult = NO_ERROR;

	if ((preDivider < MIN_PRE_DIV) || 
	    (preDivider > MAX_PRE_DIV))
	{
		hResult = E_DICE_BAD_INPUT_PARAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	switch (rxDevType)
	{
		case DEV_AES_RX:
			diceClockShadow.aesRxMckSel.bit.regen_vco_pre_scaler = preDivider;
			clkRegWrite(&(pDiceClock->aesRxMckSel.reg), diceClockShadow.aesRxMckSel.reg);
			break;
		default:
			hResult = E_DICE_BAD_INPUT_PARAM;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	return hResult;
}
// Set main clock divider for device only AES, ADAT and TDIF
HRESULT clkSetMainDivider(DEV_RX_TYPES rxDevType, uint8 mainDivider)
{
	HRESULT hResult = NO_ERROR;
	
	if ((mainDivider < MIN_MAIN_DIV) || 
	    (mainDivider > MAX_MAIN_DIV))
	{
		hResult = E_DICE_BAD_INPUT_PARAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	switch (rxDevType)
	{
		case DEV_AES_RX:
			diceClockShadow.aesRxMckSel.bit.regen_vco_main_divider = mainDivider;
			clkRegWrite(&(pDiceClock->aesRxMckSel.reg), diceClockShadow.aesRxMckSel.reg);
			break;
		default:
			hResult = E_DICE_BAD_INPUT_PARAM;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	return hResult;
}

// Set post scale clock divider for device only AES, ADAT and TDIF
HRESULT clkSetPostDivider(DEV_RX_TYPES rxDevType, uint8 postDivider)
{
	HRESULT hResult = NO_ERROR;
	
	switch (rxDevType)
	{
		case DEV_AES_RX:
			diceClockShadow.aesRxMckSel.bit.regen_vco_post_scaler = postDivider;
			clkRegWrite(&(pDiceClock->aesRxMckSel.reg), diceClockShadow.aesRxMckSel.reg);
			break;
		default:
			hResult = E_DICE_BAD_INPUT_PARAM;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	return hResult;
}

// Set polarity of UP/Down signal to onchip VCO
HRESULT clkSetSignalPolarity(DEV_RX_TYPES rxDevType, uint8 mode)
{
	HRESULT hResult = NO_ERROR;
	uint32	up_pol = SIGNAL_ACTIVE_HIGH;
	uint32	down_pol = SIGNAL_ACTIVE_LOW;

	switch (mode)
	{
		case NORMAL_VCO_UP_SIGNAL:
			up_pol = SIGNAL_ACTIVE_HIGH;
			break;
		case INVERT_VCO_UP_SIGNAL:
			up_pol = SIGNAL_ACTIVE_LOW;
			break;
		default:
			hResult = E_DICE_BAD_INPUT_PARAM;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
	}

	switch (rxDevType)
	{
		case DEV_AES_RX:
			diceClockShadow.aesRxMckSel.bit.regen_vco_up_pol = up_pol;
			diceClockShadow.aesRxMckSel.bit.regen_vco_down_pol = down_pol;
			clkRegWrite(&(pDiceClock->aesRxMckSel.reg), diceClockShadow.aesRxMckSel.reg);
			return hResult;
		default:
			hResult = E_DICE_BAD_INPUT_PARAM;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
	}

	return hResult;
}

// Selects external or internal VCO for rx device only AES, ADAT and TDIF
// Enable internal receiver VCO for rx device only AES, ADAT and TDIF
HRESULT clkSetVCO(DEV_RX_TYPES rxDevType, uint8 enableRx, uint8 external)
{
	HRESULT hResult = NO_ERROR;

	switch (rxDevType)
	{
		case DEV_AES_RX:
			diceClockShadow.aesRxMckSel.bit.regen_vco_pwrdn = enableRx;
			diceClockShadow.aesRxMckSel.bit.regen_vco_ext_clk_sel = external;
			clkRegWrite(&(pDiceClock->aesRxMckSel.reg), diceClockShadow.aesRxMckSel.reg);
			break;
		default:
			hResult = E_DICE_BAD_INPUT_PARAM;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	return hResult;
}

// Setting ADAT S-Muz
// Selects whether to sync ADAT Transmitter to a received ADAT stream
HRESULT clkSetADATSync(BOOL bSynToStream)
{
	HRESULT hResult = E_DICE_BAD_INPUT_PARAM;
	return hResult;
}

// Hybrid PLL clock selection
HRESULT clkSetHybridPLL(uint32 hpllClk)
{
	HRESULT hResult = NO_ERROR;

	//LM??? function name change etc. (Clock doubler settings //LM???)
	clkRegWrite(&(pDiceClock->hybridPllSetup), hpllClk);
	
	return hResult;
}

// External clock input selection from synchronizer 1,2
HRESULT clkSetInExtern(uint8 clkId, uint8 value)
{
	HRESULT hResult = NO_ERROR;

	switch(clkId) {
	case 1:
		diceClockShadow.wcEcSel.bit.wclk1 = value;
		break;
	case 0:
		diceClockShadow.wcEcSel.bit.extbr_wclk0 = value;
		break;
	}
		
	clkRegWrite(&(pDiceClock->wcEcSel.reg), diceClockShadow.wcEcSel.reg);
	return hResult;
}

HRESULT clkGetInExtern(uint8 *syncId)
{
	HRESULT hResult = NO_ERROR;

	clkRegRead(&(pDiceClock->wcEcSel.reg), &(diceClockShadow.wcEcSel.reg));
	*syncId = diceClockShadow.wcEcSel.reg;

	return hResult;
}

// Word clock input selection from router 1,2
HRESULT clkSetOutWord(uint8 value)
{
	HRESULT hResult = NO_ERROR;

	diceClockShadow.wcEcSel.reg = value;

	clkRegWrite(&(pDiceClock->wcEcSel.reg), diceClockShadow.wcEcSel.reg);

	return hResult;
}

HRESULT clkGetOutWord(uint8 *routerId)
{
	HRESULT hResult = NO_ERROR;
	
	clkRegRead(&(pDiceClock->wcEcSel.reg), &(diceClockShadow.wcEcSel.reg));

	*routerId = diceClockShadow.wcEcSel.reg;
	
	return hResult;
}

// Selects source of the block sync to other devices
HRESULT clkSetExternBlocksync(uint8 bsSrc)
{
	HRESULT hResult = NO_ERROR;

	diceClockShadow.blockSync.bit.ext_bsync_out_sel = bsSrc;
	
	clkRegWrite(&(pDiceClock->blockSync.reg), diceClockShadow.blockSync.reg);

	return hResult;
}

// Selects source of the block sync to AES tx
HRESULT clkSetAESTxBlocksync(uint8 bsSrc)
{
	HRESULT hResult = NO_ERROR;

	diceClockShadow.blockSync.bit.aes_tx_bsync_sel = bsSrc;
	
	clkRegWrite(&(pDiceClock->blockSync.reg), diceClockShadow.blockSync.reg);

	return hResult;
}

// Selects source of the block sync to AVS ATX1
HRESULT clkSetTXDI1Blocksync(uint8 bsSrc)
{
	HRESULT hResult = NO_ERROR;

	//the error detect code can be put here.
	diceClockShadow.blockSync.bit.txdi1_bsync_out_sel = bsSrc;
	clkRegWrite(&(pDiceClock->blockSync.reg), diceClockShadow.blockSync.reg);

	return hResult;
}

HRESULT clkSetTXDI2Blocksync(uint8 bsSrc)
{
	HRESULT hResult = NO_ERROR;

	diceClockShadow.blockSync.bit.txdi2_bsync_out_sel = bsSrc;
	clkRegWrite(&(pDiceClock->blockSync.reg), diceClockShadow.blockSync.reg);

	return hResult;
}

// Setting prescale devisor for prescaler 1,2
HRESULT clkSetPrescaler(uint8 prescalerId, uint32 devisor)
{
	HRESULT hResult = NO_ERROR;
	
	switch (prescalerId)
	{
		case PRESCALER_ID1:
			clkRegWrite(&(pDiceClock->prescaler1Sel), devisor);
			break;
		default:
			hResult = E_DICE_BAD_INPUT_PARAM;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	return hResult;
}

// Configuration af sample rate counter
HRESULT clkSetSampleRateCntr(uint8 srCntrId, uint8 srCntInput1, uint8 srCntInput2)
{
	HRESULT hResult = NO_ERROR;

	switch (srCntrId)
	{
		case SR_CNTR_ID1:
			diceClockShadow.srCntSel.bit.sr_cnt1_in1_sel = srCntInput1;
			diceClockShadow.srCntSel.bit.sr_cnt1_in2_sel = srCntInput2;
			break;
		case SR_CNTR_ID2:
			diceClockShadow.srCntSel.bit.sr_cnt2_in1_sel = srCntInput1;
			diceClockShadow.srCntSel.bit.sr_cnt2_in2_sel = srCntInput2;
			break;
		default:
			hResult = E_DICE_BAD_INPUT_PARAM;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	clkRegWrite(&(pDiceClock->srCntSel.reg), diceClockShadow.srCntSel.reg);

	return hResult;
}

HRESULT clkGetSampleRateCntr(uint8 srCntrId, uint8 *srCntInput1, uint8 *srCntInput2)
{
	HRESULT hResult = NO_ERROR;

	clkRegRead(&(pDiceClock->srCntSel.reg), &(diceClockShadow.srCntSel.reg));

	switch (srCntrId)
	{
		case SR_CNTR_ID1:
			*srCntInput1 = diceClockShadow.srCntSel.bit.sr_cnt1_in1_sel;
			*srCntInput2 = diceClockShadow.srCntSel.bit.sr_cnt1_in2_sel;
			break;
		case SR_CNTR_ID2:
			*srCntInput1 = diceClockShadow.srCntSel.bit.sr_cnt2_in1_sel;
			*srCntInput2 = diceClockShadow.srCntSel.bit.sr_cnt2_in2_sel;
			break;
		default:
			hResult = E_DICE_BAD_INPUT_PARAM;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	return hResult;
}

// Seting SR Cntr as phase detector or sample rate counter
HRESULT clkSetSampleRateMode(uint8 srCntrId, uint8 srMode)
{
	HRESULT hResult = NO_ERROR;

	switch (srCntrId)
	{
		case SR_CNTR_ID1:
			diceClockShadow.srModeSel.bit.sr_mode1 = srMode;
			break;
		case SR_CNTR_ID2:
			diceClockShadow.srModeSel.bit.sr_mode2 = srMode;
			break;
		default:
			hResult = E_DICE_BAD_INPUT_PARAM;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	clkRegWrite(&(pDiceClock->srModeSel.reg), diceClockShadow.srModeSel.reg);

	return hResult;
}

// Get SR Cntr value and mode
HRESULT clkGetSampleRateValue(uint8 srCntrId, uint8 *srMode, uint32 *srCount)
{
	HRESULT hResult = NO_ERROR;

	clkRegRead(&(pDiceClock->srModeSel.reg), &(diceClockShadow.srModeSel.reg));

	switch (srCntrId)
	{
		case SR_CNTR_ID1:
			*srMode = diceClockShadow.srModeSel.bit.sr_mode1;
			clkRegRead(&(pDiceClock->srCount1), srCount);
			break;
		case SR_CNTR_ID2:
			*srMode = diceClockShadow.srModeSel.bit.sr_mode2;
			clkRegRead(&(pDiceClock->srCount2), srCount);
			break;
		default:
			hResult = E_DICE_BAD_INPUT_PARAM;
			sysLogError(hResult, __LINE__, moduleName);
			break;
	}

	return hResult;
}
