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

static DICE_ADATRX diceADATRxShadow;
static DICE_ADATTX diceADATTxShadow;


typedef struct 
{
	uint8			enabled;
	uint8			locked;
	uint8			synced;
	uint8			locked_aux;
	uint8			synced_aux;
	uADATRX_STATUS	status;		// sticky 
	uADATRX_STATUS	_status;	// non-sticky 
	uADATRX_STATUS	status_aux;		// sticky 
	uADATRX_STATUS	_status_aux;	// non-sticky 
	uint32			lstUsrData;	// last read user data to monitor change
	uint32			lstUsrData_aux;	// last read user data to monitor change
} ADAT_RX_FSM;


static ADAT_RX_FSM adatFSM;




void adatRxStateMachine (uint32 * pEvent)
{
	*pEvent = 0;
	
	if (!adatFSM.enabled) return;
	
	adatFSM._status.reg = diceADATRxShadow.status.reg = pDiceADATRx->status.reg;
	
	if (adatFSM.synced)
	{
		if (adatFSM._status.bit.no_sync || !adatFSM._status.bit.sync)
		{
			*pEvent |= EVT_ADAT_SYNC_LOST;
			*pEvent |= EVT_ADAT_LOCK_LOST;
			adatFSM.locked = false;
			adatFSM.synced = false;
		}
	}
	else //not locked
	{
		if ((!adatFSM._status.bit.no_sync) || adatFSM._status.bit.sync)
		{
			*pEvent |= EVT_ADAT_SYNC_ACQUIRED;
			adatFSM.synced = true;
		}
	}
	
	if (adatFSM.locked)
	{
		if (adatFSM._status.bit.no_lock || !adatFSM._status.bit.lock)
		{
			*pEvent |= EVT_ADAT_LOCK_LOST;
			adatFSM.locked = false;
		}
	}
	else //not locked
	{
		if ((!adatFSM._status.bit.no_lock) || adatFSM._status.bit.lock)
		{
			*pEvent |= EVT_ADAT_LOCK_ACQUIRED;
			adatFSM.locked = true;
		}
	}

	adatFSM.status.reg |= adatFSM._status.reg;
	
	if (adatFSM.locked)
	{
		if (adatFSM.lstUsrData != diceADATRxShadow.status.bit.user_data)
		{
			adatFSM.lstUsrData = diceADATRxShadow.status.bit.user_data;
			*pEvent |= EVT_ADAT_USR_CHANGE;
		}
	}
	
	
	
	adatFSM._status_aux.reg = diceADATRxShadow.statusAux.reg = pDiceADATRx->statusAux.reg;
	if (adatFSM.synced_aux)
	{
		if (adatFSM._status_aux.bit.no_sync || !adatFSM._status_aux.bit.sync)
		{
			*pEvent |= (EVT_ADAT_SYNC_LOST<<16);
			*pEvent |= (EVT_ADAT_LOCK_LOST<<16);
			adatFSM.locked_aux = false;
			adatFSM.synced_aux = false;
		}
	}
	else //not locked
	{
		if ((!adatFSM._status_aux.bit.no_sync) || adatFSM._status_aux.bit.sync)
		{
			*pEvent |= (EVT_ADAT_SYNC_ACQUIRED<<16);
			adatFSM.synced_aux = true;
		}
	}


	if (adatFSM.locked_aux)
	{
		if (adatFSM._status_aux.bit.no_lock || !adatFSM._status_aux.bit.lock)
		{
			*pEvent |= (EVT_ADAT_LOCK_LOST<<16);
			adatFSM.locked_aux = false;
		}
	}
	else //not locked
	{
		if ((!adatFSM._status_aux.bit.no_lock) || adatFSM._status_aux.bit.lock)
		{
			*pEvent |= (EVT_ADAT_LOCK_ACQUIRED<<16);
			adatFSM.locked_aux = true;
		}
	}
	adatFSM.status_aux.reg |= adatFSM._status_aux.reg;
	
	if (adatFSM.locked_aux)
	{
		if (adatFSM.lstUsrData_aux != diceADATRxShadow.statusAux.bit.user_data)
		{
			adatFSM.lstUsrData_aux = diceADATRxShadow.statusAux.bit.user_data;
			*pEvent |= (EVT_ADAT_USR_CHANGE<<16);
		}
	}
}

HRESULT adatRxEnable (BOOL bEnable)
{
	adatFSM.enabled = bEnable;
	return NO_ERROR;
}

HRESULT adatRxGetEnable (BOOL * bEnable)
{
	*bEnable = adatFSM.enabled;
	return NO_ERROR;
}

HRESULT adatRxGetUserData(uint8 rxid, uint8 *data)
{
	HRESULT hResult = NO_ERROR;

	if (!rxid)
		*data = adatFSM.lstUsrData;
	else
		*data = adatFSM.lstUsrData_aux;
	return hResult;
}


HRESULT adatRxGetError(uint8 rxid, uADATRX_STATUS *pRxError, BOOL clearCondition)
{
	HRESULT hResult = NO_ERROR;

	if (!rxid)
	{
		pRxError->reg = adatFSM.status.reg;
		if (clearCondition) adatFSM.status.reg = 0;
	}
	else
	{
		pRxError->reg = adatFSM.status_aux.reg;
		if (clearCondition) adatFSM.status_aux.reg = 0;
	}
	
	return hResult;
}


// Check if ADAT Rx is in locked state
BOOL adatIsLocked(uint8  rxid)
{
	if (!rxid)
		return (adatFSM.locked);
	else
		return (adatFSM.locked_aux);
}

BOOL adatIsSynced(uint8  rxid)
{
	if (!rxid)
		return (adatFSM.synced);
	else
		return (adatFSM.synced_aux);
}


HRESULT adatTxSetUser(uint8  txid, uint8 userData)
{
	HRESULT hResult = NO_ERROR;

	if (!txid)
	{
		diceADATTxShadow.userSetup.bit.user_data = userData;
		pDiceADATTx->userSetup.reg = diceADATTxShadow.userSetup.reg;
		return hResult;
	}
	else
	{
		diceADATTxShadow.userSetup.bit.user_data_aux = userData;
		pDiceADATTx->userSetup.reg = diceADATTxShadow.userSetup.reg;
		return hResult;
	}
}



// Set ADAT transmitter mute for channel 0-7, if ALL_CH then mute/unmute all
HRESULT adatTxSetMute(uint8  txid, uint8 adatTxCh, BOOL mute)
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

	if (!txid)
	{
		if (mute)
			diceADATTxShadow.chMute.reg |= muteBit;  // Set mute
		else
			diceADATTxShadow.chMute.reg &= ~muteBit; // Clear mute
		pDiceADATTx->chMute.reg = diceADATTxShadow.chMute.reg;
    } 
    else
	{
		if (mute)
			diceADATTxShadow.chMuteAux.reg |= muteBit;  // Set mute
		else
			diceADATTxShadow.chMuteAux.reg &= ~muteBit; // Clear mute
		pDiceADATTx->chMuteAux.reg = diceADATTxShadow.chMuteAux.reg;
    } 
    
	return hResult;
}

HRESULT adatInitialize(void)
{
	HRESULT hResult = NO_ERROR;


	// Set ADAT tx default
	diceADATTxShadow.userSetup.reg = 0;
	// Mute all transmitters
	diceADATTxShadow.chMute.reg = 0xFF;
	diceADATTxShadow.chMuteAux.reg = 0xFF;
	
	pDiceADATTx->userSetup.reg = diceADATTxShadow.userSetup.reg;
	pDiceADATTx->chMute.reg    = diceADATTxShadow.chMute.reg;
	pDiceADATTx->chMuteAux.reg = diceADATTxShadow.chMuteAux.reg;

	return hResult;
}
