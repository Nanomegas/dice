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

#ifndef _DICEADAT_H
#define _DICEADAT_H

#include "diceADATReg.h"

HRESULT adatInitialize(void);

#define   EVT_ADAT_SYNC_ACQUIRED	0x00000020
#define   EVT_ADAT_SYNC_LOST		0x00000040
#define   EVT_ADAT_LOCK_ACQUIRED	0x00000100
#define   EVT_ADAT_LOCK_LOST		0x00000200
#define   EVT_ADAT_USR_CHANGE		0x00001000

void adatRxStateMachine (uint32 * pEvent);

HRESULT adatRxGetError(uint8 rxid, uADATRX_STATUS *pRxError, BOOL clearCondition);

// Check if ADAT Rx is in locked state
BOOL adatIsLocked(uint8  rxid);

BOOL adatIsSynced(uint8  rxid);

HRESULT adatRxEnable (BOOL bEnable);

HRESULT adatRxGetEnable (BOOL * bEnable);

// Get user data
HRESULT adatRxGetUserData(uint8  rxid, uint8 *data);


// Set user status bits
HRESULT adatTxSetUser(uint8  txid, uint8 userData);


// Set ADAT transmitter mute for channel 0-7, if ALL_CH then mute/unmute all
HRESULT adatTxSetMute(uint8  txid, uint8 adatTxCh, BOOL mute);




#endif
