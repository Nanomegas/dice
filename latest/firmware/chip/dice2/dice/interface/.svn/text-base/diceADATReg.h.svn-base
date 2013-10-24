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
	
	Purpose:	DICE - ADAT receiver and transmitter register definition
	
	Revisions:
		created 26/06/2003 kbj

****************************************************************************/

#ifndef _DICEADATREG_H
#define _DICEADATREG_H

// Number of ADAT receivers and transmitters
#define NUM_ADAT  					  1

// Number of ADAT channels 
#define NUM_ADAT_CHANNELS			8

//*******************************************************************************
// ADAT receive setup register
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned user_data		: 4;	// User data
		unsigned pll_sel		: 2;	// Pll selection
		unsigned s_mux_en		: 1;	// Mux enable
		unsigned				: 25;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 25;	// Not used
		unsigned s_mux_en		: 1;	// Mux enable
		unsigned pll_sel		: 2;	// Pll selection
		unsigned user_data		: 4;	// User data
	} bit;
#endif

} uADATRX_SETUP;

//*******************************************************************************
// ADAT receive status flag register, All errors are sticky cleared when read
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned no_lock		: 1;	// Rx channel has been out of lock since last read
		unsigned lock			: 1;	// Rx channel is in lock
		unsigned lock_1			: 1;	// 
		unsigned lock_2			: 1;	// 
		unsigned lock_sel		: 1;	// 
		unsigned o_run			: 1;	// Slip sample error (system 1fs slower than 1fs master rx)
		unsigned u_run			: 1;	// Resample error (system 1fs faster than 1fs master rx)
		unsigned uo_run			: 1;	// Or'ed function of resample and slip error bits
		unsigned				: 24;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 24;	// Not used
		unsigned uo_run			: 1;	// Or'ed function of resample and slip error bits
		unsigned u_run			: 1;	// Resample error (system 1fs faster than 1fs master rx)
		unsigned o_run			: 1;	// Slip sample error (system 1fs slower than 1fs master rx)
		unsigned lock_sel		: 1;	// 
		unsigned lock_2			: 1;	// 
		unsigned lock_1			: 1;	// 
		unsigned lock			: 1;	// Rx channel is in lock
		unsigned no_lock		: 1;	// Rx channel has been out of lock since last read
	} bit;
#endif

} uADATRX_STATUS;

//*******************************************************************************
// ADAT receiver register structure
typedef struct
{
	uADATRX_SETUP		setup;			// Setup register
	uADATRX_STATUS	status;			// Read status flags
} DICE_ADATRX;

//*******************************************************************************
// ADAT transmitter setup user register
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned user_data		: 4;	// User data
		unsigned user_ch		: 3;	// User channel
		unsigned loop_user_data	: 1;	// 
		unsigned				: 24;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 24;	// Not used
		unsigned loop_user_data	: 1;	// 
		unsigned user_ch		: 3;	// User channel
		unsigned user_data		: 4;	// User data
	} bit;
#endif

} uADATTX_USER;

//*******************************************************************************
// ADAT transmitter setup mux register
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned s_mux_en		: 1;	// Enable SMux
		unsigned				: 31;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 31;	// Not used
		unsigned s_mux_en		: 1;	// Enable SMux
	} bit;
#endif

} uADATTX_MUX;

//*******************************************************************************
// ADAT transmitter setup mute register
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned ch0_mute		: 1;	// Mute for channel 0
		unsigned ch1_mute		: 1;	// Mute for channel 1
		unsigned ch2_mute		: 1;	// Mute for channel 2
		unsigned ch3_mute		: 1;	// Mute for channel 3
		unsigned ch4_mute		: 1;	// Mute for channel 4
		unsigned ch5_mute		: 1;	// Mute for channel 5
		unsigned ch6_mute		: 1;	// Mute for channel 6
		unsigned ch7_Mute		: 1;	// Mute for channel 7
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

} uADATTX_MUTE;

//*******************************************************************************
// ADAT transmitter register structure
typedef struct
{
	uADATTX_USER		userSetup;		// User setup register
	uADATTX_MUX			muxSetup;		// Mux setup register
	uADATTX_MUTE		chMute;			// Channel mute register
} DICE_ADATTX;

extern DICE_ADATRX *pDiceADATRx;
extern DICE_ADATTX *pDiceADATTx;

#endif
