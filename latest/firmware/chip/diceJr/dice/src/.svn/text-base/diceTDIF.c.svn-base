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
	
	Purpose:	DICE - TDIF receiver and transmitter API
	
	Revisions:
		created 26/06/2003 kbj

****************************************************************************/

#include "TCTypes.h"
#include "TCDebug.h"
#include "TCTasking.h"
#include "ErrorCodes.h"
#include "diceDefs.h"
#include "diceTDIF.h"
#include "diceClock.h"
#include "targetBoard.h" //for LED

static char 	moduleName[] = __FILE__ ;

DICE_TDIFRX *pDiceTDIFRx = (DICE_TDIFRX *)(DICEBASE + DICE_TDIFRX_BASE);
DICE_TDIFTX *pDiceTDIFTx = (DICE_TDIFTX *)(DICEBASE + DICE_TDIFTX_BASE);


/************************* LOW LEVEL FUNCTIONS *****************************/

// Setup receiver lock range
HRESULT tdifRxSetSetup(uint8 lockRange)
{
	HRESULT hResult = NO_ERROR;

	pDiceTDIFRx->setup.bit.lock_range = lockRange;
	return hResult;
}

/************************* HIGH LEVEL FUNCTIONS *****************************/
// TDIF Rx State machine functions.
// Lock state machine
// The lock state machine monitors the TDIF PLL and makes sure that lock is
// obtained by pumping the PLL.
// tdifRxStateMachine function should be called at least every 20ms.

#define FSM_TICKSIZE	20  // run every 20ms
#define FSM_FORCE_UP	20/FSM_TICKSIZE //force up for 100ms
#define FSM_FORCE_WAIT	100/FSM_TICKSIZE //after force wait for 100ms


typedef enum
{
	TDIF_STATE_DISABLED,  //The TDIF Rx is not started
	TDIF_STATE_FORCE_PLL, //The TDIF PLL is being forced
	TDIF_STATE_CHECK,     //The TDIF PLL is not forced, check for lock
	TDIF_STATE_LOCKED     //The Rx or Tx is enabled and running
} TDIF_STATE;


typedef struct 
{
	TDIF_STATE		state;
	uTDIFRX_STATUS	status;               //sticky 
	uTDIFRX_STATUS	_status;               // non-sticky 
	uint32			stateVal;    //countig sub states
	uint32			lastTime;
} TDIF_RX_FSM;

static TDIF_RX_FSM tdifFSM;


#define DIRECT_EV_MASK  0x3FF

static void tdifRxSetState (TDIF_STATE state)
{
	if (tdifFSM.state == state) return;
  
	switch (state)
	{
		case TDIF_STATE_DISABLED:
			break;
    
		case TDIF_STATE_FORCE_PLL:
			clkSetSignalPolarity(DEV_TDIF_RX, INVERT_VCO_UP_SIGNAL);
			tdifFSM.stateVal = FSM_FORCE_UP;
			break;       
    
		case TDIF_STATE_CHECK:
			clkSetSignalPolarity(DEV_TDIF_RX, NORMAL_VCO_UP_SIGNAL);
			tdifFSM.stateVal = FSM_FORCE_WAIT; 
			break;
        
		case TDIF_STATE_LOCKED:
			break;
	}
	tdifFSM.state = state;
    //ML:140607, removed, was used for debugging, use DAL callback for updating LOCK STATUS
  	//if (tdifFSM.state == TDIF_STATE_LOCKED)
	//	LED_ON(TDIF_LOCK_LED_ID);
	//else
	//	LED_OFF(TDIF_LOCK_LED_ID);

}

void tdifRxStateMachine (uint32 * pEvent)
{
	*pEvent = 0;
	
	if (TCTimeElapsed(tdifFSM.lastTime) < FSM_TICKSIZE) return;
	tdifFSM.lastTime = TCTimeGet();	
	if (tdifFSM.state == TDIF_STATE_DISABLED) return;

	tdifFSM._status = pDiceTDIFRx->status;
	tdifFSM.status.reg |= tdifFSM._status.reg;
  
		
	switch (tdifFSM.state)
	{
		case TDIF_STATE_FORCE_PLL:
			if (!tdifFSM.stateVal)
			{
				tdifRxSetState (TDIF_STATE_CHECK);
			}
			else
			{
				tdifFSM.stateVal--;
			}
			break;

		case TDIF_STATE_CHECK:
			if (!tdifFSM.stateVal)
			{
				if (tdifFSM._status.bit.lock == 1) 
				{
					tdifRxSetState (TDIF_STATE_LOCKED);
					*pEvent |= EVT_TDIF_LOCK_ACQUIRED;
				}
				else
				{
					// we need to pump again or try another source
					tdifRxSetState (TDIF_STATE_FORCE_PLL);
				}
			}
			else
			{
				tdifFSM.stateVal--;
			}
			break;      
			
		case TDIF_STATE_LOCKED:
			if (tdifFSM._status.bit.no_lock == 0)
			{
				tdifRxSetState (TDIF_STATE_FORCE_PLL);
				*pEvent |= EVT_TDIF_LOCK_LOST;
			} 
			else
			{
				*pEvent |= tdifFSM._status.reg & DIRECT_EV_MASK;
			}
			break;
	}
	
}

HRESULT tdifRxEnable (BOOL bEnable)
{
	if (bEnable)
	{
		tdifRxSetState (TDIF_STATE_FORCE_PLL);
		clkSetVCO(DEV_TDIF_RX, ENABLE_RX_VCO, INTERNAL_VCO);
	}
	else
	{
		tdifRxSetState (TDIF_STATE_DISABLED);
		clkSetVCO(DEV_TDIF_RX, DISABLE_RX_VCO, INTERNAL_VCO);
	}			
	return NO_ERROR;
}


// Get receiver status: rate, emphasis on/off
HRESULT tdifRxGetGlobalStatus(uint8 *rate, BOOL *emphasis)
{
	HRESULT hResult = NO_ERROR;

	*emphasis = pDiceTDIFRx->setup.bit.emphasis;
	*rate = (pDiceTDIFRx->setup.bit.fs1 << 1) | pDiceTDIFRx->setup.bit.fs0;
	return hResult;
}




// Get receiver channel status: user data, word length, emphasis on/off
HRESULT tdifRxGetChStatus(uint8 channel, uint8 *userData, uint8 *wdLen, BOOL *emphasis)
{
	HRESULT hResult = NO_ERROR;

	if (channel < NUM_TDIF_CHANNELS)
	{
		uint8 offset;
		uint8 even;
		offset = channel / 2;
		even = channel % 2;
		if (even)
		{
			*userData = pDiceTDIFRx->chSetup[offset].bit.user_data2;
			*wdLen = pDiceTDIFRx->chSetup[offset].bit.word_len2;
			*emphasis = pDiceTDIFRx->chSetup[offset].bit.emphasis2;
		}
		else
		{
			*userData = pDiceTDIFRx->chSetup[offset].bit.user_data1;
			*wdLen = pDiceTDIFRx->chSetup[offset].bit.word_len1;
			*emphasis = pDiceTDIFRx->chSetup[offset].bit.emphasis1;
		}
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

HRESULT tdifRxGetError(uTDIFRX_STATUS *pRxError, BOOL clearCondition)
{
	HRESULT hResult = NO_ERROR;

	pRxError->reg = tdifFSM.status.reg;
	if (clearCondition) tdifFSM.status.reg = 0;

	return hResult;
}



BOOL tdifIsLocked(void)
{
	return (tdifFSM.state == TDIF_STATE_LOCKED);
}

// Read phase difference between sys_1fs and tdif_1fs
HRESULT tdifRxGetPhaseDiff(uint16 *phaseDiff)
{
	HRESULT hResult = NO_ERROR;

	*phaseDiff = pDiceTDIFRx->phaseDiff;
	return hResult;
}

// Setup transmitter: rate, emphasis on/off, 
HRESULT tdifTxSetSetup(uint8 rate, BOOL emphasis)
{
	HRESULT hResult = NO_ERROR;
	
	pDiceTDIFTx->chSetup[0].bit.efs0_1 = emphasis; // Set emphasis
	pDiceTDIFTx->chSetup[1].bit.efs0_1 = rate & 0x1; // Set fs0
	pDiceTDIFTx->chSetup[2].bit.efs0_1 = (rate >> 1) & 0x1; // Set fs1
	return hResult;
}

// Setup transmit channel: user data, word length, emphasis on/off
HRESULT tdifTxSetChStatus(uint8 channel, uint8 userData, uint8 wdLen, BOOL emphasis)
{
	HRESULT hResult = NO_ERROR;

	if (channel < NUM_TDIF_CHANNELS)
	{
		pDiceTDIFTx->chSetup[channel].bit.user_data = userData;
		pDiceTDIFTx->chSetup[channel].bit.word_len = wdLen;
		pDiceTDIFTx->chSetup[channel].bit.emphasis = emphasis;
		return hResult;
	}
	
	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set TDIF transmitter mute for channel 0-7, if ALL_CH then mute/unmute all
HRESULT tdifTxSetMute(uint8 tdifTxCh, BOOL mute)
{
	HRESULT hResult = NO_ERROR;
	uint32 muteBit;

	if (tdifTxCh < NUM_TDIF_CHANNELS)
	{
		muteBit = 1 << tdifTxCh;
	}
	else
	{
		muteBit = 0xff;
	}

	if (mute)
		pDiceTDIFTx->chMute.reg |= muteBit;  // Set mute
	else
		pDiceTDIFTx->chMute.reg &= ~muteBit; // Clear mute
	return hResult;
}


HRESULT tdifInitialize(void)
{
	HRESULT hResult = NO_ERROR;
	uint8 i;

	// Set TDIF rx default
	pDiceTDIFRx->setup.reg = 0;
	// lock range er hermed 20ns
	pDiceTDIFRx->setup.bit.lock_range = 1;

	for (i = 0; i < NUM_TDIF; i++)
	{
		pDiceTDIFRx->chSetup[i].reg = 0;
	}
	
	pDiceTDIFRx->phaseDiff = 0;

	// Set TDIF tx default
	for (i = 0; i < NUM_TDIF_CHANNELS; i++)
	{
		pDiceTDIFTx->chSetup[i].reg = 0;
	}
	// Mute all transmitters
	pDiceTDIFTx->chMute.reg = 0xff;

	tdifFSM.state      = TDIF_STATE_FORCE_PLL;
	tdifFSM.stateVal   = 0;
	tdifFSM.lastTime   = TCTimeGet();	
	tdifTxSetSetup(TDIF_RATE_48K, FALSE);
	tdifTxSetChStatus(TDIF_CH1, 0, TDIF_WDLEN_24B, FALSE);
	tdifTxSetChStatus(TDIF_CH2, 0, TDIF_WDLEN_24B, FALSE);
	tdifTxSetChStatus(TDIF_CH3, 0, TDIF_WDLEN_24B, FALSE);
	tdifTxSetChStatus(TDIF_CH4, 0, TDIF_WDLEN_24B, FALSE);
	tdifTxSetChStatus(TDIF_CH5, 0, TDIF_WDLEN_24B, FALSE);
	tdifTxSetChStatus(TDIF_CH6, 0, TDIF_WDLEN_24B, FALSE);
	tdifTxSetChStatus(TDIF_CH7, 0, TDIF_WDLEN_24B, FALSE);
	tdifTxSetChStatus(TDIF_CH8, 0, TDIF_WDLEN_24B, FALSE);
	return hResult;
}



