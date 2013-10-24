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
	
	Purpose:	DICE - I2S receiver and transmitter API
	
	Revisions:
		created 26/06/2003 kbj

****************************************************************************/

#include "TCTypes.h"
#include "TCDebug.h"
#include "ErrorCodes.h"
#include "diceDefs.h"
#include "diceI2S.h"

static char 	moduleName[] = __FILE__ ;

DICE_I2SRX *pDiceI2SRx0 = (DICE_I2SRX *)(DICEBASE + DICE_I2S0RX_BASE);
DICE_I2STX *pDiceI2STx0 = (DICE_I2STX *)(DICEBASE + DICE_I2S0TX_BASE);
DICE_I2SRX *pDiceI2SRx1 = (DICE_I2SRX *)(DICEBASE + DICE_I2S1RX_BASE);
DICE_I2STX *pDiceI2STx1 = (DICE_I2STX *)(DICEBASE + DICE_I2S1TX_BASE);
DICE_I2SRX *pDiceI2SRx2 = (DICE_I2SRX *)(DICEBASE + DICE_I2S2RX_BASE);
DICE_I2STX *pDiceI2STx2 = (DICE_I2STX *)(DICEBASE + DICE_I2S2TX_BASE);

static DICE_I2SRX diceI2SRxShadow[NUM_ID_I2S];
static DICE_I2STX diceI2STxShadow[NUM_ID_I2S];

static DICE_I2STX *i2sTx(uint8 i2sID)
{
	switch (i2sID)
	{
		case I2S_ID0:
			return pDiceI2STx0;
		case I2S_ID1:
			return pDiceI2STx1;
		case I2S_ID2:
			return pDiceI2STx2;
	}
	return 0;
}

static DICE_I2SRX *i2sRx(uint8 i2sID)
{
	switch (i2sID)
	{
		case I2S_ID0:
			return pDiceI2SRx0;
		case I2S_ID1:
			return pDiceI2SRx1;
		case I2S_ID2:
			return pDiceI2SRx2;
	}
	return 0;
}

static uint8 numI2sLines (uint8 i2sID)
{
	switch (i2sID)
	{
		case I2S_ID0:
			return 4;
		case I2S_ID1:
			return 2;
		case I2S_ID2:
			return 2;
	}
	return 0;
}


static void setI2SRXDefault(uI2SRX_CH_SETUP *pChSetup)
{
	// Default init of receiver
	pChSetup->bit.mclk_out_en = 1;
	pChSetup->bit.msb_first = 1;
	pChSetup->bit.l_justify = 1;
	pChSetup->bit.data_in_size = 1;
	pChSetup->bit.data_delay = 1;
	pChSetup->bit.lrck_out_inv = 0;
	pChSetup->bit.bick_inv = 1;
	pChSetup->bit.mclk_out_div = 0;
}

static void setI2STXDefault(uI2STX_CH_SETUP *pChSetup)
{
	// Default init of transmitter
	pChSetup->bit.mclk_out_en = 1;
	pChSetup->bit.msb_first = 1;
	pChSetup->bit.l_justify = 1;
	pChSetup->bit.data_in_size = 1;
	pChSetup->bit.data_delay = 1;
	pChSetup->bit.lrck_out_inv = 0;
	pChSetup->bit.bick_inv = 1;
	pChSetup->bit.mclk_out_div = 0;
}

// Clear everything 
HRESULT i2sInitialize(void)
{
	HRESULT hResult = NO_ERROR;
	uint8 i;
	
	
	for (i = 0; i < NUM_ID_I2S; i++)
	{
		memset (&diceI2SRxShadow[i],0,sizeof(DICE_I2SRX));
		memset (&diceI2STxShadow[i],0,sizeof(DICE_I2STX));
		setI2SRXDefault(&diceI2SRxShadow[i].chSetup[0]);
		setI2SRXDefault(&diceI2SRxShadow[i].chSetup[1]);
		setI2SRXDefault(&diceI2SRxShadow[i].chSetup[2]);
		setI2SRXDefault(&diceI2SRxShadow[i].chSetup[3]);
		setI2STXDefault(&diceI2STxShadow[i].chSetup[0]);
		setI2STXDefault(&diceI2STxShadow[i].chSetup[1]);
		setI2STXDefault(&diceI2STxShadow[i].chSetup[2]);
		setI2STXDefault(&diceI2STxShadow[i].chSetup[3]);
		memcpy (i2sRx(i), &diceI2SRxShadow[i], sizeof(DICE_I2SRX));
		memcpy (i2sTx(i), &diceI2STxShadow[i], sizeof(DICE_I2STX));
	}	
	return hResult;
}

// Set master freq.
HRESULT i2sRxSetMckFreq(uint8 i2sId, BOOL mckFreq)
{
	HRESULT hResult = NO_ERROR;
	
	if (i2sId < NUM_ID_I2S)
	{
		DICE_I2SRX *pI2S = i2sRx(i2sId);
		diceI2SRxShadow[i2sId].clkSetup.bit.mck_in_freq = mckFreq;
		pI2S->clkSetup.reg = diceI2SRxShadow[i2sId].clkSetup.reg;
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

HRESULT i2sRxSetM192(uint8 i2sId, BOOL m192)
{
	HRESULT hResult = NO_ERROR;
	
	if (i2sId < NUM_ID_I2S)
	{
		DICE_I2SRX *pI2S = i2sRx(i2sId);
		diceI2SRxShadow[i2sId].clkSetup.bit.m192 = m192;
		pI2S->clkSetup.reg = diceI2SRxShadow[i2sId].clkSetup.reg;
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}


// Enable master clock output and set divider
HRESULT i2sRxSetClock(uint8 i2sId, BOOL mckOutEn, uint8 mckOutDiv)
{
	HRESULT hResult = NO_ERROR;
	
	if (i2sId < NUM_ID_I2S)
	{
		DICE_I2SRX *pI2S = i2sRx(i2sId);
		diceI2SRxShadow[i2sId].chSetup[0].bit.mclk_out_en = mckOutEn;
		diceI2SRxShadow[i2sId].chSetup[0].bit.mclk_out_div = mckOutDiv;
		pI2S->chSetup[0].reg = diceI2SRxShadow[i2sId].chSetup[0].reg;
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Invert clock signal
HRESULT i2sRxSetClockInv(uint8 i2sId, BOOL lrckOutInv, uint8 bickInv)
{
	HRESULT hResult = NO_ERROR;
	
	if (i2sId < NUM_ID_I2S)
	{
		DICE_I2SRX *pI2S = i2sRx(i2sId);
		diceI2SRxShadow[i2sId].chSetup[0].bit.lrck_out_inv = lrckOutInv;
		diceI2SRxShadow[i2sId].chSetup[0].bit.bick_inv = bickInv;	
		pI2S->chSetup[0].reg = diceI2SRxShadow[i2sId].chSetup[0].reg;
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set data format, MSB/LSB first, Left/Right justity, Data size and data delay
HRESULT i2sRxSetDataFormat(uint8 i2sId, uint8 i2sRxCh, BOOL msbFirst, BOOL lJustify, BOOL dataSize, BOOL dataDelay)
{
	HRESULT hResult = NO_ERROR;
	
	if ((i2sId < NUM_ID_I2S) && (i2sRxCh < numI2sLines (i2sId)))
	{
		DICE_I2SRX *pI2S = i2sRx(i2sId);
		diceI2SRxShadow[i2sId].chSetup[i2sRxCh].bit.msb_first = msbFirst;
		diceI2SRxShadow[i2sId].chSetup[i2sRxCh].bit.l_justify = lJustify;
		diceI2SRxShadow[i2sId].chSetup[i2sRxCh].bit.data_in_size = dataSize;
		diceI2SRxShadow[i2sId].chSetup[i2sRxCh].bit.data_delay = dataDelay;
		pI2S->chSetup[i2sRxCh].reg = diceI2SRxShadow[i2sId].chSetup[i2sRxCh].reg;
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set master freq.
HRESULT i2sTxSetMckFreq(uint8 i2sId, BOOL mckFreq)
{
	HRESULT hResult = NO_ERROR;
	
	if (i2sId < NUM_ID_I2S)
	{
		DICE_I2STX *pI2S = i2sTx(i2sId);
		diceI2STxShadow[i2sId].clkSetup.bit.sys_mck_freq = mckFreq;
		pI2S->clkSetup.reg = diceI2STxShadow[i2sId].clkSetup.reg;
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

HRESULT i2sTxSetM192(uint8 i2sId, BOOL m192)
{
	HRESULT hResult = NO_ERROR;
	
	if (i2sId < NUM_ID_I2S)
	{
		DICE_I2STX *pI2S = i2sTx(i2sId);
		diceI2STxShadow[i2sId].clkSetup.bit.m192 = m192;
		pI2S->clkSetup.reg = diceI2STxShadow[i2sId].clkSetup.reg;
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}


// Enable master clock output and set divider
HRESULT i2sTxSetClock(uint8 i2sId, BOOL mckOutEn, uint8 mckOutDiv)
{
	HRESULT hResult = NO_ERROR;
	
	if (i2sId < NUM_ID_I2S)
	{
		DICE_I2STX *pI2S = i2sTx(i2sId);
		diceI2STxShadow[i2sId].chSetup[0].bit.mclk_out_en = mckOutEn;
		diceI2STxShadow[i2sId].chSetup[0].bit.mclk_out_div= mckOutDiv;
		pI2S->chSetup[0].reg = diceI2STxShadow[i2sId].chSetup[0].reg;
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Invert clock signal
HRESULT i2sTxSetClockInv(uint8 i2sId, BOOL lrckOutInv, uint8 bickInv)
{
	HRESULT hResult = NO_ERROR;
	
	if (i2sId < NUM_ID_I2S)
	{
		DICE_I2STX *pI2S = i2sTx(i2sId);
		diceI2STxShadow[i2sId].chSetup[0].bit.lrck_out_inv =lrckOutInv;
		diceI2STxShadow[i2sId].chSetup[0].bit.bick_inv = bickInv;
		pI2S->chSetup[0].reg = diceI2STxShadow[i2sId].chSetup[0].reg;
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set data format, MSB/LSB first, Left/Right justity, Data size and data delay
HRESULT i2sTxSetDataFormat(uint8 i2sId, uint8 i2sTxCh, BOOL msbFirst, BOOL lJustify, BOOL dataSize, BOOL dataDelay)
{
	HRESULT hResult = NO_ERROR;
	
	if ((i2sId < NUM_ID_I2S) && (i2sTxCh < numI2sLines (i2sId)))
	{
		DICE_I2STX *pI2S = i2sTx(i2sId);
		diceI2STxShadow[i2sId].chSetup[i2sTxCh].bit.msb_first = msbFirst;
		diceI2STxShadow[i2sId].chSetup[i2sTxCh].bit.l_justify = lJustify;
		diceI2STxShadow[i2sId].chSetup[i2sTxCh].bit.data_in_size = dataSize;
		diceI2STxShadow[i2sId].chSetup[i2sTxCh].bit.data_delay = dataDelay;
		pI2S->chSetup[i2sTxCh].reg = diceI2STxShadow[i2sId].chSetup[i2sTxCh].reg;
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set DSAI transmitter mute for channel 0-7, if ALL_CH then mute/unmute all
HRESULT i2sTxSetMute(uint8 i2sId, uint8 i2sTxCh, BOOL mute)
{
	HRESULT hResult = NO_ERROR;	
	uint32 muteBit;

	if (i2sId  < NUM_ID_I2S)
	{
		DICE_I2STX *pI2S = i2sTx(i2sId);

		if (i2sTxCh < NUM_I2S_CHANNELS)
		{
			muteBit = 1 << i2sTxCh;
		}
		else
		{
			muteBit = 0xff;
		}

		if (mute)
			diceI2STxShadow[i2sId].chMute.reg |= muteBit;  // Set mute
		else
			diceI2STxShadow[i2sId].chMute.reg &= ~muteBit; // Clear mute
		pI2S->chMute.reg = diceI2STxShadow[i2sId].chMute.reg;
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}
