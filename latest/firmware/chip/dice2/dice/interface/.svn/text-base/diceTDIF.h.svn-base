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

#ifndef _DICETDIF_H
#define _DICETDIF_H

#include "diceTDIFReg.h"

// TDIF receiver and transmitter channels 0-7
#define TDIF_CH1		0
#define TDIF_CH2		1
#define TDIF_CH3		2
#define TDIF_CH4		3
#define TDIF_CH5		4
#define TDIF_CH6		5
#define TDIF_CH7		6
#define TDIF_CH8		7

// Values for rate
#define TDIF_RATE_48K	0	// Sample rate 48 kHz or 96 kHz
#define TDIF_RATE_44K	1	// Sample rate 44.1 kHz or 88.2 kHz
#define TDIF_RATE_32K	2	// Sample rate 32 kHz or 64 kHz

// Values for word length (wdLen)
#define TDIF_WDLEN_16B	0	// 16 bits
#define TDIF_WDLEN_20B	1	// 20 bits
#define TDIF_WDLEN_24B	2	// 24 bits



HRESULT tdifInitialize(void);


#define   EVT_TDIF_PARITY1			0x00000001
#define   EVT_TDIF_PARITY2			0x00000002
#define   EVT_TDIF_PARITY3			0x00000004
#define   EVT_TDIF_PARITY4			0x00000008
#define   EVT_TDIF_PARITY5			0x00000010
#define   EVT_TDIF_PARITY6			0x00000020
#define   EVT_TDIF_PARITY7			0x00000040
#define   EVT_TDIF_PARITY8			0x00000080
#define   EVT_TDIF_SLIP				0x00000100
#define   EVT_TDIF_RPT				0x00000200
#define   EVT_TDIF_LOCK_ACQUIRED	0x00001000
#define   EVT_TDIF_LOCK_LOST		0x00002000

void tdifRxStateMachine (uint32 * pEvent);

HRESULT tdifRxEnable (BOOL bEnable);

HRESULT tdifRxGetGlobalStatus(uint8 *rate, BOOL *emphasis);

HRESULT tdifRxGetChStatus(uint8 channel, uint8 *userData, uint8 *wdLen, BOOL *emphasis);

HRESULT tdifRxGetError(uTDIFRX_STATUS *pRxError, BOOL clearCondition);

BOOL tdifIsLocked(void);

// Read phase difference between sys_1fs and tdif_1fs
HRESULT tdifRxGetPhaseDiff(uint16 *phaseDiff);

// Setup transmitter: rate, emphasis on/off, 
HRESULT tdifTxSetSetup(uint8 rate, BOOL emphasis);

// Setup transmit channel: user data, word length, emphasis on/off
HRESULT tdifTxSetChStatus(uint8 channel, uint8 userData, uint8 wdLen, BOOL emphasis);

// Set TDIF transmitter mute for channel 0-7, if ALL_CH then mute/unmute all
HRESULT tdifTxSetMute(uint8 tdifTxCh, BOOL mute);


/************************* LOW LEVEL FUNCTIONS *****************************/

// Setup receiver lock range
// lockRange is a setup value for lock detector, 
// providing a maximum allowable signal skew to signal lock within. 
// Is interpreted as number of router clock cycles (~20ns)
HRESULT tdifRxSetSetup(uint8 lockRange);



#endif
