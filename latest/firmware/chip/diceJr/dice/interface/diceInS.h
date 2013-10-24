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

#ifndef _DICEINS_H
#define _DICEINS_H

#include "diceInSReg.h"



HRESULT insInitialize(void);

// Set clock port.
HRESULT insSetClockPort (uint8 insId, uint8 mck_rate, uint8 bck_rate, uint8 fsy_len,
                         BOOL mck_inv, BOOL bck_inv, BOOL fsy_inv, BOOL enable);

// Easy way to set for I2S
HRESULT insSetClockPortI2S (uint8 insId, uint8 mck_rate, BOOL enable);

// Set Rx line data format
HRESULT insRxSetDataFormat(uint8 insId, uint8 line, uint8 mode, uint8 delay, uint8 shuffle, BOOL enable);

// Easy way to set for I2S
HRESULT insRxSetDataFormatI2S(uint8 insId, uint8 line, BOOL enable);

// Set Tx line data format
HRESULT insTxSetDataFormat(uint8 insId, uint8 line, uint8 mode, uint8 delay, uint8 shuffle, BOOL enable);

// Easy way to set for I2S
HRESULT insTxSetDataFormatI2S(uint8 insId, uint8 line, BOOL enable);

// Set InS transmitter mute for channel 0-15, if ALL_CH then mute/unmute all
HRESULT insTxSetMute(uint8 insId, uint8 insTxCh, BOOL mute);

 
#endif
