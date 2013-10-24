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
	
	Purpose:	DICE - TDIF receiver and transmitter register definition
	
	Revisions:
		created 26/06/2003 kbj

***********************************************************************/

#ifndef _DICETDIFREG_H
#define _DICETDIFREG_H

// Number of TDIF receivers and transmitters
#define NUM_TDIF				1

// Number of TDIF channels 
#define NUM_TDIF_CHANNELS		8

// Number of TDIF channels 
#define NUM_TDIF_LINES		4


//*******************************************************************************
// TDIF receive setup user and emphasis register
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned user_data2		: 4;	// 
		unsigned word_len2		: 2;	// 
		unsigned emphasis2		: 1;	// Channels 2,4,6,8
		unsigned				: 1;	// Not used
		unsigned user_data1		: 4;	// 
		unsigned word_len1		: 2;	// Lenght of information field
		unsigned emphasis1		: 1;	// Channels 1,3,5,7
		unsigned				: 17;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 17;	// Not used
		unsigned emphasis1		: 1;	// Channels 1,3,5,7
		unsigned word_len1		: 2;	// Lenght of information field
		unsigned user_data1		: 4;	// 
		unsigned				: 1;	// Not used
		unsigned emphasis2		: 1;	// Channels 2,4,6,8
		unsigned word_len2		: 2;	// 
		unsigned user_data2		: 4;	// 
	} bit;
#endif

} uTDIFRX_CH_STATUS;

//*******************************************************************************
// TDIF receive communication status register
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned parity_error	: 8;	// Parity error on channel 1-8, cleared when read (Sticky)
		unsigned slip_sample	: 1;	// Slip error, cleared when read (Sticky)
		unsigned double_sample	: 1;	// Resample error, cleared when read (Sticky)
		unsigned no_lock		: 1;	// 0 if TDIF has been out of lock since last read (Sticky, read presets to 1)
		unsigned lock			: 1;	// Current status for lock(1)/no lock(0)
		unsigned				: 20;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 20;	// Not used
		unsigned lock			: 1;	// Current status for lock(1)/no lock(0)
		unsigned no_lock		: 1;	// TDIF has been out of lock since last read (Sticky)
		unsigned double_sample	: 1;	// Resample error, cleared when read (Sticky)
		unsigned slip_sample	: 1;	// Slip error, cleared when read (Sticky)
		unsigned parity_error	: 8;	// Parity error on channel 1-8, cleared when read (Sticky)
	} bit;
#endif

} uTDIFRX_STATUS;

//*******************************************************************************
// TDIF receive communication setup register
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned lock_range		: 3;	// 
		unsigned vco_clk_sel	: 2;	// Reserved
		unsigned emphasis		: 1;	//
		unsigned fs0			: 1;	// 
		unsigned fs1			: 1;	// 
		unsigned				: 24;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 24;	// Not used
		unsigned fs1			: 1;	// 
		unsigned fs0			: 1;	// 
		unsigned emphasis		: 1;	//
		unsigned vco_clk_sel	: 2;	// Reserved
		unsigned lock_range		: 3;	// 
	} bit;
#endif

} uTDIFRX_SETUP;

typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned invRx0			: 1;	// 
		unsigned invRx1			: 1;	// 
		unsigned invRx2			: 1;	// 
		unsigned invRx3			: 1;	// 
		unsigned invLR			: 1;	// 
		unsigned invFs0			: 1;	// 
		unsigned invFs1			: 1;	// 
		unsigned invEmp			: 1;	// 
		unsigned				: 24;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 24;	// Not used
		unsigned invEmp			: 1;	// 
		unsigned invFs1			: 1;	// 
		unsigned invFs0			: 1;	// 
		unsigned invLR			: 1;	// 
		unsigned invRx3			: 1;	// 
		unsigned invRx2			: 1;	// 
		unsigned invRx1			: 1;	// 
		unsigned invRx0			: 1;	// 
	} bit;
#endif

} uTDIFRX_INVERSION;

//*******************************************************************************
// TDIF receiver register structure
typedef struct
{
	uTDIFRX_CH_STATUS	chSetup[NUM_TDIF_LINES];// Setup channel register
	uTDIFRX_STATUS		status;				// Read status flags
	uTDIFRX_SETUP		setup;				// Setup lock range ..
	uint32				phaseDiff;	
	uTDIFRX_INVERSION   inversion;
} DICE_TDIFRX;

//*******************************************************************************
// TDIF transmitter channel setup register
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned user_data		: 4;	//
		unsigned word_len		: 2;	// 
		unsigned emphasis		: 1;	// 
		unsigned efs0_1			: 1;	// emphasis if ch1, fs0 if ch2, fs1 if ch3
		unsigned				: 24;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 24;	// Not used
		unsigned efs0_1			: 1;	// emphasis if ch1, fs0 if ch2, fs1 if ch3
		unsigned emphasis		: 1;	// 
		unsigned word_len		: 2;	// 
		unsigned user_data		: 4;	//
	} bit;
#endif

} uTDIFTX_CH_SETUP;

//*******************************************************************************
// TDIF transmitter Channel mute register
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

} uTDIFTX_CH_MUTE;

typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned invRx0			: 1;	// 
		unsigned invRx1			: 1;	// 
		unsigned invRx2			: 1;	// 
		unsigned invRx3			: 1;	// 
		unsigned invLR			: 1;	// 
		unsigned invFs0			: 1;	// 
		unsigned invFs1			: 1;	// 
		unsigned invEmp			: 1;	// 
		unsigned				: 24;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 24;	// Not used
		unsigned invEmp			: 1;	// 
		unsigned invFs1			: 1;	// 
		unsigned invFs0			: 1;	// 
		unsigned invLR			: 1;	// 
		unsigned invRx3			: 1;	// 
		unsigned invRx2			: 1;	// 
		unsigned invRx1			: 1;	// 
		unsigned invRx0			: 1;	// 
	} bit;
#endif

} uTDIFTX_INVERSION;


//*******************************************************************************
// TDIF transmitter register structure
typedef struct
{
	uTDIFTX_CH_SETUP	chSetup[NUM_TDIF_CHANNELS];		// Setup Tx channels
	uTDIFTX_CH_MUTE		chMute;							// Mute Tx channels
	uTDIFTX_INVERSION	inversion;
} DICE_TDIFTX;


extern DICE_TDIFRX *pDiceTDIFRx;
extern DICE_TDIFTX *pDiceTDIFTx;

#endif
