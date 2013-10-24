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

#define   EVT_ADAT_SLIP				0x00000020
#define   EVT_ADAT_RPT				0x00000040
#define   EVT_ADAT_LOCK_ACQUIRED	0x00000100
#define   EVT_ADAT_LOCK_LOST		0x00000200
#define   EVT_ADAT_USR_CHANGE		0x00001000

void adatRxStateMachine (uint32 * pEvent);

HRESULT adatRxGetError(uADATRX_STATUS *pRxError, BOOL clearCondition);

// Check if ADAT Rx is in locked state
BOOL adatIsLocked(void);

HRESULT adatRxEnable (BOOL bEnable);

HRESULT adatRxGetEnable (BOOL * bEnable);

// Get user data
HRESULT adatRxGetUserData(uint8 *data);


// Enable/disable mux (Enable for 96kHz)
HRESULT adatRxSetMux(BOOL enable);


// Set user status bits
HRESULT adatTxSetUser(uint8 userData);

// Set to loop user status bits from ADAT receiver channel 0-7
HRESULT adatTxSetLoopUser(BOOL enable, uint8 channel);

// Enable/disable mux (Enable for 96kHz)
HRESULT adatTxSetMux(BOOL enable);

// Set ADAT transmitter mute for channel 0-7, if ALL_CH then mute/unmute all
HRESULT adatTxSetMute(uint8 adatTxCh, BOOL mute);



/************************* LOW LEVEL FUNCTIONS *****************************/

// Set PLL selection
HRESULT adatRxSetPLLSync(uint8 selection);

// Values for selection, number of sync's
#define PLL_SEL_SYNC_1	0
#define PLL_SEL_SYNC_2	1
#define PLL_SEL_SYNC_3	2
#define PLL_SEL_SYNC_4	3




#endif
