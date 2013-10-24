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
	
	Purpose:	DICE - DSAI1 receiver and transmitter API
	
	Revisions:
		created 26/06/2003 kbj

****************************************************************************/

#include "TCTypes.h"
#include "TCDebug.h"
#include "ErrorCodes.h"
#include "diceDefs.h"
#include "diceDSAI.h"

static char 	moduleName[] = __FILE__ ;

DICE_DSAI *pDiceDSAIRx0 = (DICE_DSAI *)(DICEBASE + DICE_DSAI0RX_BASE);
DICE_DSAI *pDiceDSAITx0 = (DICE_DSAI *)(DICEBASE + DICE_DSAI0TX_BASE);
DICE_DSAI *pDiceDSAIRx1 = (DICE_DSAI *)(DICEBASE + DICE_DSAI1RX_BASE);
DICE_DSAI *pDiceDSAITx1 = (DICE_DSAI *)(DICEBASE + DICE_DSAI1TX_BASE);
DICE_DSAI *pDiceDSAIRx2 = (DICE_DSAI *)(DICEBASE + DICE_DSAI2RX_BASE);
DICE_DSAI *pDiceDSAITx2 = (DICE_DSAI *)(DICEBASE + DICE_DSAI2TX_BASE);
DICE_DSAI *pDiceDSAIRx3 = (DICE_DSAI *)(DICEBASE + DICE_DSAI3RX_BASE);
DICE_DSAI *pDiceDSAITx3 = (DICE_DSAI *)(DICEBASE + DICE_DSAI3TX_BASE);

static DICE_DSAI *dsaiTx(uint8 dsaiID)
{
	switch (dsaiID)
	{
		case DSAI_ID0:
			return pDiceDSAITx0;
		case DSAI_ID1:
			return pDiceDSAITx1;
		case DSAI_ID2:
			return pDiceDSAITx2;
		case DSAI_ID3:
			return pDiceDSAITx3;
	}
	return 0;
}

static DICE_DSAI *dsaiRx(uint8 dsaiID)
{
	switch (dsaiID)
	{
		case DSAI_ID0:
			return pDiceDSAIRx0;
		case DSAI_ID1:
			return pDiceDSAIRx1;
		case DSAI_ID2:
			return pDiceDSAIRx2;
		case DSAI_ID3:
			return pDiceDSAIRx3;
	}
	return 0;
}

static void setDSAIDefault(DICE_DSAI *pDSAI)
{
	// Default init of DSAI
	pDSAI->setup.reg = 0;
	pDSAI->masterSetup.reg = 0;
	// Mute all transmitters
	pDSAI->txMuteCh.reg = 0xff;

	// Ouput enable clock and sync
	pDSAI->masterSetup.bit.clk_oe = 0;
	pDSAI->masterSetup.bit.sync_oe = 0;
}

// Clear everything 
HRESULT dsaiInitialize(void)
{
	HRESULT hResult = NO_ERROR;
	
	setDSAIDefault(pDiceDSAIRx0);
	setDSAIDefault(pDiceDSAITx0);

	setDSAIDefault(pDiceDSAIRx1);
	setDSAIDefault(pDiceDSAITx1);
	
	setDSAIDefault(pDiceDSAIRx2);
	setDSAIDefault(pDiceDSAITx2);
	
	setDSAIDefault(pDiceDSAIRx3);
	setDSAIDefault(pDiceDSAITx3);

	return hResult;
}

// Set DSAI Rx and Tx channel mode for 8 or 16 slots
HRESULT dsaiSetChMode(uint8 dsaiId, uint8 rxTxMask, uint8 chMode)
{
	HRESULT hResult = NO_ERROR;

	if (dsaiId < NUM_DSAI)
	{
		DICE_DSAI *pDsai;
	
		if (rxTxMask & DSAI_TX)
		{
			pDsai = dsaiTx(dsaiId);
			pDsai->setup.bit.ch_mode = chMode;
		}
		if (rxTxMask & DSAI_RX)
		{
			pDsai = dsaiRx(dsaiId);
			pDsai->setup.bit.ch_mode = chMode;
		}
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set DSAI Rx and Tx sync length for 1 clock or 32 clocks
HRESULT dsaiSetSyncLen( uint8 dsaiId, uint8 rxTxMask, uint8 syncLen)
{
	HRESULT hResult = NO_ERROR;

	if (dsaiId < NUM_DSAI)
	{
		DICE_DSAI *pDsai;
	
		if (rxTxMask & DSAI_TX)
		{
			pDsai = dsaiTx(dsaiId);
			pDsai->setup.bit.sync_length = syncLen;
		}
		if (rxTxMask & DSAI_RX)
		{
			pDsai = dsaiRx(dsaiId);
			pDsai->setup.bit.sync_length = syncLen;
		}
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}


// Set DSAI Rx and Tx delay of data after sync
HRESULT dsaiSetDataDelay(uint8 dsaiId, uint8 rxTxMask, uint8 delay)
{
	HRESULT hResult = NO_ERROR;

	if (dsaiId < NUM_DSAI)
	{
		DICE_DSAI *pDsai;
	
		if (rxTxMask & DSAI_TX)
		{
			pDsai = dsaiTx(dsaiId);
			pDsai->setup.bit.data_delay = delay;
		}
		if (rxTxMask & DSAI_RX)
		{
			pDsai = dsaiRx(dsaiId);
			pDsai->setup.bit.data_delay = delay;
		}
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set DSAI Rx and Tx to be master - clock and sync provider
HRESULT dsaiSetMaster(uint8 dsaiId, uint8 rxTxMask, BOOL master)
{
	HRESULT hResult = NO_ERROR;

	if (dsaiId < NUM_DSAI)
	{
		DICE_DSAI *pDsai;
		uint8 enable = 0;
		
		if (master) enable = 1;
	
		if (rxTxMask & DSAI_TX)
		{
			pDsai = dsaiTx(dsaiId);
			pDsai->masterSetup.bit.clk_oe = enable;
			pDsai->masterSetup.bit.sync_oe = enable;
		}
		if (rxTxMask & DSAI_RX)
		{
			pDsai = dsaiRx(dsaiId);
			pDsai->masterSetup.bit.clk_oe = enable;
			pDsai->masterSetup.bit.sync_oe = enable;
		}
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set DSAI Rx and Tx data shuffle mode
HRESULT dsaiSetDataShuffle(uint8 dsaiId, uint8 rxTxMask, uint8 shuffle)
{
	HRESULT hResult = NO_ERROR;

	if (dsaiId < NUM_DSAI)
	{
		DICE_DSAI *pDsai;
	
		if (rxTxMask & DSAI_TX)
		{
			pDsai = dsaiTx(dsaiId);
			pDsai->setup.bit.shuffle = shuffle;
		}
		if (rxTxMask & DSAI_RX)
		{
			pDsai = dsaiRx(dsaiId);
			pDsai->setup.bit.shuffle = shuffle;
		}
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Get DSAI sample error status, specifie either Rx or Tx bit
HRESULT dsaiGetStatus(uint8 dsaiId, uint8 rxTxMask, uint8 *status)
{
	HRESULT hResult = NO_ERROR;

	if (dsaiId < NUM_DSAI)
	{
		DICE_DSAI *pDsai;
	
		if (rxTxMask & DSAI_TX)
		{
			pDsai = dsaiTx(dsaiId);
			*status = pDsai->sampleError;
			return hResult;
		}
		if (rxTxMask & DSAI_RX)
		{
			pDsai = dsaiRx(dsaiId);
			*status = pDsai->sampleError;
			return hResult;
		}
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set DSAI transmitter mute for channel 0-7, if ALL_CH then mute/unmute all
HRESULT dsaiTxSetMute(uint8 dsaiId, uint8 dsaiTxCh, BOOL mute)
{
	HRESULT hResult = NO_ERROR;
	DICE_DSAI *pDsai;
	uint32 muteBit;

	if (dsaiId < NUM_DSAI)
	{
		pDsai = dsaiTx(dsaiId);

		if (dsaiTxCh < NUM_DSAI_CHANNELS)
		{
			muteBit = 1 << dsaiTxCh;
		}
		else
		{
			muteBit = 0xff;
		}

		if (mute)
			pDsai->txMuteCh.reg |= muteBit;  // Set mute
		else
			pDsai->txMuteCh.reg &= ~muteBit; // Clear mute
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}
