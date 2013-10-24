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
	
	Purpose:	DICE - DSAI receiver and transmitter register definition
	
	Revisions:
		created 26/06/2003 kbj

****************************************************************************/

#ifndef _DICEDSAIREG_H
#define _DICEDSAIREG_H

// Number of DSAI channels 
#define NUM_DSAI_CHANNELS		8

//*******************************************************************************
// DSAI setup setup register
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned data_delay		: 2;	// 0, 1, 2, 3 clocks
		unsigned sync_length	: 1;	// 32 clocks (0), 1 clock (1)
		unsigned ch_mode		: 1;	// Channel mode 8 channels (0), 16 channels (1) 
		unsigned shuffle		: 4;	// Data shuffle
		unsigned				: 24;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 24;	// Not used
		unsigned shuffle		: 4;	// Data shuffle
		unsigned ch_mode		: 1;	// Channel mode 8 channels (0), 16 channels (1) 
		unsigned sync_length	: 1;	// 32 clocks (0), 1 clock (1)
		unsigned data_delay		: 2;	// 0, 1, 2, 3 clocks
	} bit;
#endif

} uDSAI_SETUP;

//*******************************************************************************
// DSAI clock and sync configuration
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned sync_oe		: 1;	// Sync output enable
		unsigned clk_oe			: 1;	// Clock output enable
		unsigned				: 30;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 30;	// Not used
		unsigned clk_oe			: 1;	// Clock output enable
		unsigned sync_oe		: 1;	// Sync output enable
	} bit;
#endif

} uDSAI_MASTER_SETUP;

//*******************************************************************************
// DSAI transmitter Channel mute register
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned ch0_Mute		: 1;	// Mute for channel 0
		unsigned ch1_mute		: 1;	// Mute for channel 1
		unsigned ch2_mute		: 1;	// Mute for channel 2
		unsigned ch3_mute		: 1;	// Mute for channel 3
		unsigned ch4_mute		: 1;	// Mute for channel 4
		unsigned ch5_mute		: 1;	// Mute for channel 5
		unsigned ch6_mute		: 1;	// Mute for channel 6
		unsigned ch7_mute		: 1;	// Mute for channel 7
		unsigned				: 24;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 24;	// Not used
		unsigned ch7_Mute		: 1;	// Mute for channel 7
		unsigned ch6_mute		: 1;	// Mute for channel 6
		unsigned ch5_mute		: 1;	// Mute for channel 5
		unsigned ch4_mute		: 1;	// Mute for channel 4
		unsigned ch3_mute		: 1;	// Mute for channel 3
		unsigned ch2_mute		: 1;	// Mute for channel 2
		unsigned ch1_mute		: 1;	// Mute for channel 1
		unsigned ch0_mute		: 1;	// Mute for channel 0
	} bit;
#endif

} uDSAI_CH_MUTE;

//*******************************************************************************
// DSAI receiver and transmitter register structure
typedef struct
{
	uDSAI_SETUP			setup;				// Setup register
	uDSAI_MASTER_SETUP	masterSetup;		// Master setup
	uint32				sampleError;		// Sample error status (bits 1:0)
	uDSAI_CH_MUTE		txMuteCh;			// Mute of transmitter channels
} DICE_DSAI;

extern DICE_DSAI *pDiceDSAIRx0;
extern DICE_DSAI *pDiceDSAITx0;
extern DICE_DSAI *pDiceDSAIRx1;
extern DICE_DSAI *pDiceDSAITx1;
extern DICE_DSAI *pDiceDSAIRx2;
extern DICE_DSAI *pDiceDSAITx2;
extern DICE_DSAI *pDiceDSAIRx3;
extern DICE_DSAI *pDiceDSAITx3;

#endif
