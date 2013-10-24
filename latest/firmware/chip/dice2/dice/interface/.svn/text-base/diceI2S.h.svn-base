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

#ifndef _DICEI2S_H
#define _DICEI2S_H

#include "diceI2SReg.h"

// DSAI identification number
#define I2S_ID0			0
#define I2S_ID1			1
#define I2S_ID2			2
#define NUM_ID_I2S		3

// I2S receiver and transmitter channels 
#define I2S_CH0			0	// Audio ch 1+2
#define I2S_CH1			1	// Audio ch 3+4
#define I2S_CH2			2	// Audio ch 5+6
#define I2S_CH3			3	// Audio ch 7+8

HRESULT i2sInitialize(void);

// Set master freq.
HRESULT i2sRxSetMckFreq(uint8 i2sId, BOOL mckFreq);

// set 192KHz mode.
HRESULT i2sRxSetM192(uint8 i2sId, BOOL m192);

// Enable master clock output and set divider
HRESULT i2sRxSetClock(uint8 i2sId, BOOL mckOutEn, uint8 mckOutDiv);

// Invert clock signal
HRESULT i2sRxSetClockInv(uint8 i2sId, BOOL lrckOutInv, uint8 bickInv);

// Set data format, MSB/LSB first, Left/Right justity, Data size and data delay
HRESULT i2sRxSetDataFormat(uint8 i2sId, uint8 i2sRxCh, BOOL msbFirst, BOOL lJustify, BOOL dataSize, BOOL dataDelay);


// Set master freq.
HRESULT i2sTxSetMckFreq(uint8 i2sId, BOOL mckFreq);

// set 192KHz mode.
HRESULT i2sTxSetM192(uint8 i2sId, BOOL m192);

// Enable master clock output and set divider
HRESULT i2sTxSetClock(uint8 i2sId, BOOL mckOutEn, uint8 mckOutDiv);

// Invert clock signal
HRESULT i2sTxSetClockInv(uint8 i2sId, BOOL lrckOutInv, uint8 bickInv);

// Set data format, MSB/LSB first, Left/Right justity, Data size and data delay
HRESULT i2sTxSetDataFormat(uint8 i2sId, uint8 i2sRxCh, BOOL msbFirst, BOOL lJustify, BOOL dataSize, BOOL dataDelay);

// Set I2S transmitter mute for channel 0-7, if ALL_CH then mute/unmute all
HRESULT i2sTxSetMute(uint8 i2sId, uint8 i2sTxCh, BOOL mute);

 
#endif
