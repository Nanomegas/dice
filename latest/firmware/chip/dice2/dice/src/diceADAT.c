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
	
	Purpose:	DICE - ADAT receiver and transmitter API
	
	Revisions:
		created 26/06/2003 kbj

****************************************************************************/

#include "TCTypes.h"
#include "TCDebug.h"
#include "TCTasking.h"
#include "ErrorCodes.h"
#include "diceDefs.h"
#include "diceADAT.h"
#include "diceClock.h"
#include "targetBoard.h" //for LED

//static char 	moduleName[] = __FILE__ ;

DICE_ADATRX *pDiceADATRx = (DICE_ADATRX *)(DICEBASE + DICE_ADATRX_BASE);
DICE_ADATTX *pDiceADATTx = (DICE_ADATTX *)(DICEBASE + DICE_ADATTX_BASE);

/************************* LOW LEVEL FUNCTIONS *****************************/
HRESULT adatRxSetPLLSync(uint8 selection)
{
	HRESULT hResult = NO_ERROR;

	pDiceADATRx->setup.bit.pll_sel = selection;
	return hResult;
}


/************************* HIGH LEVEL FUNCTIONS *****************************/
// ADAT Rx State machine functions.
// Lock state machine
// The lock state machine monitors the TDIF PLL and makes sure that lock is
// obtained by pumping the PLL.
// adatRxStateMachine function should be called at least every 20ms.

#define FSM_TICKSIZE	20  // run every 20ms
#define FSM_FORCE_UP	20/FSM_TICKSIZE //force up for 100ms
#define FSM_FORCE_WAIT	100/FSM_TICKSIZE //after force wait for 100ms

typedef enum
{
	ADAT_STATE_DISABLED,  //The ADAT Rx is not started
	ADAT_STATE_FORCE_PLL, //The ADAT PLL is being forced
	ADAT_STATE_CHECK,     //The ADAT PLL is not forced, check for lock
	ADAT_STATE_LOCKED     //The Rx or Tx is enabled and running
} ADAT_STATE;


typedef struct 
{
	ADAT_STATE		state;
	uADATRX_STATUS	status;		// sticky 
	uADATRX_STATUS	_status;	// non-sticky 
	uint32			stateVal;	// countig sub states
	uint32			lstUsrData;	// last read user data to monitor change
	uint32			lastTime;
} ADAT_RX_FSM;


static ADAT_RX_FSM adatFSM;

#define DIRECT_EV_MASK  0x60


static void adatRxSetState (ADAT_STATE state)
{
	if (adatFSM.state == state) return;

	switch (state)
	{
		case ADAT_STATE_DISABLED:
			break;

		case ADAT_STATE_FORCE_PLL:
			clkSetSignalPolarity(DEV_ADAT_RX, INVERT_VCO_UP_SIGNAL);
			adatFSM.stateVal = FSM_FORCE_UP;
			break;       

		case ADAT_STATE_CHECK:
			clkSetSignalPolarity(DEV_ADAT_RX, NORMAL_VCO_UP_SIGNAL);
			adatFSM.stateVal = FSM_FORCE_WAIT; 
			break;
	    
		case ADAT_STATE_LOCKED:
			break;
	}   
	adatFSM.state = state;
	if (adatFSM.state == ADAT_STATE_LOCKED)
		LED_ON(ADAT_LOCK_LED_ID);
	else
		LED_OFF(ADAT_LOCK_LED_ID);

}

#define ISLOCKED(var) ((var.bit.lock==1) && (var.bit.lock_sel == 1))

void adatRxStateMachine (uint32 * pEvent)
{
	*pEvent = 0;
	
	if (TCTimeElapsed(adatFSM.lastTime) < FSM_TICKSIZE) return;
	adatFSM.lastTime = TCTimeGet();	
	
	if (adatFSM.state == ADAT_STATE_DISABLED) return;

	adatFSM._status = pDiceADATRx->status;
	if (pDiceADATRx->setup.bit.s_mux_en)
		adatFSM._status.reg &= ~(EVT_ADAT_SLIP|EVT_ADAT_RPT);
	adatFSM.status.reg |= adatFSM._status.reg;	

	switch (adatFSM.state)
	{
		case ADAT_STATE_FORCE_PLL:
			if (!adatFSM.stateVal)
			{
				adatRxSetState (ADAT_STATE_CHECK);
			}
			else
			{
				adatFSM.stateVal--;
			}
			break;

		case ADAT_STATE_CHECK:
			if (!adatFSM.stateVal)
			{
				if (ISLOCKED(adatFSM._status)) 
				{
					adatRxSetState (ADAT_STATE_LOCKED);
					*pEvent |= EVT_ADAT_LOCK_ACQUIRED;
				}
				else
				{
					// we need to pump again or try another source
					adatRxSetState (ADAT_STATE_FORCE_PLL);
				}
			}
			else
			{
				adatFSM.stateVal--;
			}
			break;      
		    
		case ADAT_STATE_LOCKED:
			if ((adatFSM._status.bit.no_lock == 1) || !ISLOCKED(adatFSM._status))
			{
				adatRxSetState (ADAT_STATE_FORCE_PLL);
				*pEvent |= EVT_ADAT_LOCK_LOST;
			}
			else
			{
				*pEvent |= adatFSM._status.reg & DIRECT_EV_MASK;
				if (adatFSM.lstUsrData != pDiceADATRx->setup.bit.user_data)
				{
					adatFSM.lstUsrData = pDiceADATRx->setup.bit.user_data;
					*pEvent |= EVT_ADAT_USR_CHANGE;
				}
			}
			break;
	}
}

HRESULT adatRxEnable (BOOL bEnable)
{
	if (bEnable)
	{
		adatRxSetState (ADAT_STATE_FORCE_PLL);
		clkSetVCO(DEV_ADAT_RX, ENABLE_RX_VCO, INTERNAL_VCO);		
	}
	else
	{
		adatRxSetState (ADAT_STATE_DISABLED);
		clkSetVCO(DEV_ADAT_RX, DISABLE_RX_VCO, INTERNAL_VCO);
	}			
	return NO_ERROR;
}

HRESULT adatRxGetEnable (BOOL * bEnable)
{
	*bEnable = (adatFSM.state != ADAT_STATE_DISABLED);
	return NO_ERROR;
}

HRESULT adatRxGetUserData(uint8 *data)
{
	HRESULT hResult = NO_ERROR;

	*data = pDiceADATRx->setup.bit.user_data;
	return hResult;
}


HRESULT adatRxSetMux(BOOL enable)
{
	HRESULT hResult = NO_ERROR;

	if (enable) 
		pDiceADATRx->setup.bit.s_mux_en = 1;
	else
		pDiceADATRx->setup.bit.s_mux_en = 0;
	return hResult;
}

HRESULT adatRxGetError(uADATRX_STATUS *pRxError, BOOL clearCondition)
{
	HRESULT hResult = NO_ERROR;

	pRxError->reg = adatFSM.status.reg;
	if (clearCondition) adatFSM.status.reg = 0;

	return hResult;
}


// Check if ADAT Rx is in locked state
BOOL adatIsLocked(void)
{
	return (adatFSM.state == ADAT_STATE_LOCKED);
}



HRESULT adatTxSetUser(uint8 userData)
{
	HRESULT hResult = NO_ERROR;

	pDiceADATTx->userSetup.bit.user_data = userData;
	return hResult;
}

HRESULT adatTxSetLoopUser(BOOL enable, uint8 channel)
{
	HRESULT hResult = NO_ERROR;

	pDiceADATTx->userSetup.bit.user_ch = channel;

	if (enable)
		pDiceADATTx->userSetup.bit.loop_user_data = 1;
	else
		pDiceADATTx->userSetup.bit.loop_user_data = 0;


	return hResult;
}

HRESULT adatTxSetMux(BOOL enable)
{
	HRESULT hResult = NO_ERROR;

	if (enable)
		pDiceADATTx->muxSetup.bit.s_mux_en = 1;
	else
		pDiceADATTx->muxSetup.bit.s_mux_en = 0;

	return hResult;
}

// Set ADAT transmitter mute for channel 0-7, if ALL_CH then mute/unmute all
HRESULT adatTxSetMute(uint8 adatTxCh, BOOL mute)
{
	HRESULT hResult = NO_ERROR;
	uint32 muteBit;

	if (adatTxCh < NUM_ADAT_CHANNELS)
	{
		muteBit = 1 << adatTxCh;
	}
	else
	{
		muteBit = 0xff;
	}

	if (mute)
		pDiceADATTx->chMute.reg |= muteBit;  // Set mute
	else
		pDiceADATTx->chMute.reg &= ~muteBit; // Clear mute


	return hResult;
}

HRESULT adatInitialize(void)
{
	HRESULT hResult = NO_ERROR;

	//all the shadows are initialized to 0. 
	// Set ADAT rx default
	pDiceADATRx->setup.reg = 0;

	// Set ADAT tx default
	pDiceADATTx->userSetup.reg = 0;
	pDiceADATTx->userSetup.bit.user_data = 0;
	pDiceADATTx->muxSetup.bit.s_mux_en = 0; 

	// Mute all transmitters
	pDiceADATTx->chMute.reg = 0xFF;

	adatRxSetPLLSync(PLL_SEL_SYNC_1);
	
	adatFSM.state      = ADAT_STATE_DISABLED;
	adatFSM.stateVal   = 0;
	adatFSM.lastTime   = TCTimeGet();	
	adatFSM.lstUsrData = 0;
	return hResult;
}
