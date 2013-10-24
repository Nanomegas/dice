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
	
	Purpose:	DICE - InS receiver and transmitter API
	
	Revisions:
		created 01/02/2007 ml

****************************************************************************/

#include "TCTypes.h"
#include "TCDebug.h"
#include "ErrorCodes.h"
#include "diceDefs.h"
#include "diceInS.h"

static char 	moduleName[] = __FILE__ ;


static uInS_CLK_SETUP * insClkSetup(uint8 insID)
{
	switch (insID)
	{
		case INS_ID0:
			return (uInS_CLK_SETUP *) (DICEBASE + DICE_INS0TX_BASE+DICE_INSTX_CLK);
		case INS_ID1:
			return (uInS_CLK_SETUP *) (DICEBASE + DICE_INS1TX_BASE+DICE_INSTX_CLK);
	}
	return 0;
}

static uInSTX_CH_MUTE * insTxMuteSetup(uint8 insID)
{
	switch (insID)
	{
		case INS_ID0:
			return (uInSTX_CH_MUTE *) (DICEBASE + DICE_INS0TX_BASE + DICE_INSTX_MUTE);
		case INS_ID1:
			return (uInSTX_CH_MUTE *) (DICEBASE + DICE_INS1TX_BASE + DICE_INSTX_MUTE);
	}
	return 0;
}

static uInS_LINE_SETUP * insTxLineSetup(uint8 insID, uint8 insLine)
{
	switch (insID)
	{
		case INS_ID0:
			return (uInS_LINE_SETUP *) (DICEBASE + DICE_INS0TX_BASE + insLine*DICE_INSTX_LINOFS);
		case INS_ID1:
			return (uInS_LINE_SETUP *) (DICEBASE + DICE_INS1TX_BASE + insLine*DICE_INSTX_LINOFS);
	}
	return 0;
}
			


static uInS_LINE_SETUP * insRxLineSetup(uint8 insID, uint8 insLine)
{
	switch (insID)
	{
		case INS_ID0:
			return (uInS_LINE_SETUP *) (DICEBASE + DICE_INS0RX_BASE + insLine*DICE_INSRX_LINOFS);
		case INS_ID1:
			return (uInS_LINE_SETUP *) (DICEBASE + DICE_INS1RX_BASE + insLine*DICE_INSRX_LINOFS);
	}
	return 0;
}



// Turn the ports off 
HRESULT insInitialize(void)
{
	HRESULT hResult = NO_ERROR;
	uint8 i,j;	
	
	for (i = 0; i < NUM_ID_INS; i++)
	{
		insSetClockPortI2S (i, INS_MCK_512BR, false);
		for (j = 0; j < NUM_INS_LINES; j++)
		{
			insRxSetDataFormatI2S(i, j, false);
			insTxSetDataFormatI2S(i, j, false);
		}
	}
	return hResult;
}



HRESULT insSetClockPort (uint8 insId, uint8 mck_rate, uint8 bck_rate, uint8 fsy_len,
                         BOOL mck_inv, BOOL bck_inv, BOOL fsy_inv, BOOL enable)
{
	HRESULT hResult = NO_ERROR;
	uInS_CLK_SETUP clk_set;
	
	if (insId < NUM_ID_INS)
	{
		clk_set = *insClkSetup(insId);
		clk_set.bit.mck_rate = mck_rate;
		clk_set.bit.bck_rate = bck_rate;
		clk_set.bit.fsy_len = fsy_len;
		clk_set.bit.mck_inv = mck_inv;
		clk_set.bit.bck_inv = bck_inv;
		clk_set.bit.fsy_inv = fsy_inv;
		clk_set.bit.enable = enable;
		*insClkSetup(insId) = clk_set;
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}


HRESULT insSetClockPortI2S (uint8 insId, uint8 mck_rate, BOOL enable)
{
	return insSetClockPort (insId, mck_rate, INS_BCK_64FS, INS_FSYN_LEN_32BIT, 0, 1, 1, enable);
}


HRESULT insRxSetDataFormat(uint8 insId, uint8 line, uint8 mode, uint8 delay, uint8 shuffle, BOOL enable)
{
	HRESULT hResult = NO_ERROR;
	uInS_LINE_SETUP line_set;
	
	if ((insId >= NUM_ID_INS) || (line >=NUM_INS_LINES))
	{
		hResult = E_DICE_BAD_INPUT_PARAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	line_set = *insRxLineSetup(insId, line);
	line_set.bit.delay = delay;
	line_set.bit.enable = enable;
	line_set.bit.mode = mode;
	line_set.bit.shuffle = shuffle;
	*insRxLineSetup(insId, line) = line_set;
	return hResult;
}

	
HRESULT insRxSetDataFormatI2S(uint8 insId, uint8 line, BOOL enable)
{
	return insRxSetDataFormat(insId, line, INS_MODE_I2S, 1, INS_SHFL_24MSB_FRST_LJST, enable);
}

HRESULT insTxSetDataFormat(uint8 insId, uint8 line, uint8 mode, uint8 delay, uint8 shuffle, BOOL enable)
{
	HRESULT hResult = NO_ERROR;
	uInS_LINE_SETUP line_set;
	
	if ((insId >= NUM_ID_INS) || (line >=NUM_INS_LINES))
	{
		hResult = E_DICE_BAD_INPUT_PARAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	line_set = *insTxLineSetup(insId, line);
	line_set.bit.delay = delay;
	line_set.bit.enable = enable;
	line_set.bit.mode = mode;
	line_set.bit.shuffle = shuffle;
	*insTxLineSetup(insId, line) = line_set;
	return hResult;
}


HRESULT insTxSetDataFormatI2S(uint8 insId, uint8 line, BOOL enable)
{
	return insTxSetDataFormat(insId, line, INS_MODE_I2S, 1, INS_SHFL_24MSB_FRST_LJST, enable);
}


HRESULT insTxSetMute(uint8 insId, uint8 insTxCh, BOOL mute)
{
	HRESULT hResult = NO_ERROR;	
	uint32 muteBit;
	uint32 muteBits;

	if (insId  < NUM_ID_INS)
	{
		muteBits = insTxMuteSetup(insId)->reg;

		if (insTxCh < NUM_MAX_INS_CHANNELS)
		{
			muteBit = 1 << insTxCh;
		}
		else
		{
			muteBit = 0xffff;
		}

		if (mute)
			muteBits |= muteBit;  // Set mute
		else
			muteBits &= ~muteBit; // Clear mute
		insTxMuteSetup(insId)->reg = muteBits;
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}
