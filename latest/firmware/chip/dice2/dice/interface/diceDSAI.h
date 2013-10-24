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
	
	Purpose:	DICE - DSAI receiver and transmitter API
	
	Revisions:
		created 26/06/2003 kbj

****************************************************************************/

#ifndef _DICEDSAI_H
#define _DICEDSAI_H

#include "diceDSAIReg.h"

// DSAI identification number
#define DSAI_ID0			0
#define DSAI_ID1			1
#define DSAI_ID2			2
#define DSAI_ID3			3
#define NUM_DSAI			4

// Bit mask for DSAI receiver or transmitter
// if the parameter rxTxMask has BIT0 or BIT1 set then 
// transmitter and/or receiver will be updated
#define DSAI_RX				0x1
#define DSAI_TX				0x2

HRESULT dsaiInitialize(void);

// Set DSAI Rx and Tx channel mode for 8 or 16 slots
HRESULT dsaiSetChMode(uint8 dsaiId, uint8 rxTxMask, uint8 chMode);

#define MODE_8_CH			0
#define MODE_16_CH			1

// Set DSAI Rx and Tx sync length for 1 clock or 32 clocks
HRESULT dsaiSetSyncLen( uint8 dsaiId, uint8 rxTxMask, uint8 syncLen);

#define SYNC_LEN_1_CLK		1
#define SYNC_LEN_32_CLK		0

// Set DSAI Rx and Tx delay of data after sync
HRESULT dsaiSetDataDelay(uint8 dsaiId, uint8 rxTxMask, uint8 delay);

#define DELAY_0_CLK			0
#define DELAY_1_CLK			1
#define DELAY_2_CLK			2
#define DELAY_3_CLK			3

// Set DSAI Rx and Tx data shuffle mode
HRESULT dsaiSetDataShuffle(uint8 dsaiId, uint8 rxTxMask, uint8 shuffle);

// Data shuffle modes, swapping received 32 bits as specified below
#define SHUFFLE_M0			0	// b31,...,b0 -> data[b31:b8, b7:b0]
#define SHUFFLE_M1			1	// b31,...,b0 -> data[b31:b8, b0:b7]
#define SHUFFLE_M2			2	// b31,...,b0 -> data[b8:b31, b7:b0]
#define SHUFFLE_M3			3	// b31,...,b0 -> data[b8:b31, b0:b7]
#define SHUFFLE_M4			4	// b31,...,b0 -> data[b23:b0, b31:b24]
#define SHUFFLE_M5			5	// b31,...,b0 -> data[b23:b0, b24:b31]
#define SHUFFLE_M6			6	// b31,...,b0 -> data[b0:b23, b31:b24]
#define SHUFFLE_M7			7	// b31,...,b0 -> data[b0:b23, b24:b31]
#define SHUFFLE_M8			8	// b31,...,b0 -> data[b31:b24, b23:b0]
#define SHUFFLE_M9			9	// b31,...,b0 -> data[b31:b24, b0:b23]
#define SHUFFLE_M10			10	// b31,...,b0 -> data[b24:b31, b23:b0]
#define SHUFFLE_M11			11	// b31,...,b0 -> data[b24:b31, b0:b23]
#define SHUFFLE_M12			12	// b31,...,b0 -> data[b7:b0, b31:b8]
#define SHUFFLE_M13			13	// b31,...,b0 -> data[b7:b0, b8:b31]
#define SHUFFLE_M14			14	// b31,...,b0 -> data[b0:b7, b31:b8]
#define SHUFFLE_M15			15	// b31,...,b0 -> data[b0:b7, b8:b31]

// Set DSAI Rx and Tx to be master - clock and sync provider
HRESULT dsaiSetMaster(uint8 dsaiId, uint8 rxTxMask, BOOL master);

// Get DSAI sample error status, specifie either Rx or Tx bit
HRESULT dsaiGetStatus(uint8 dsaiId, uint8 rxTxMask, uint8 *status);

// Set DSAI transmitter mute for channel 0-7, if ALL_CH then mute/unmute all
HRESULT dsaiTxSetMute(uint8 dsaiId, uint8 dsaiTxCh, BOOL mute);

#endif
