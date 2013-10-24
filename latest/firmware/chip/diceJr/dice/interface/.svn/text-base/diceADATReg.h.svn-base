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
// ADAT receive status flag register, All errors are sticky cleared when read
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned sync           : 1;    //receiver is in sync
		unsigned lock           : 1;    //receiver is locked
		unsigned no_sync        : 1;    //sticky bit registering a sync error
		unsigned no_lock        : 1;    //sticky bit registering a lock error
		unsigned o_run			: 1;
		unsigned u_run			: 1;		
		unsigned user_data		: 4;	// User data
		unsigned				: 22;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 22;	// Not used
		unsigned user_data		: 4;	// User data
		unsigned u_run			: 1;		
		unsigned o_run			: 1;
		unsigned no_lock        : 1;    //sticky bit registering a lock error
		unsigned no_sync        : 1;    //sticky bit registering a sync error
		unsigned lock           : 1;    //receiver is locked
		unsigned sync           : 1;    //receiver is in sync
	} bit;
#endif

} uADATRX_STATUS;


//*******************************************************************************
// ADAT receiver register structure
typedef struct
{
	uADATRX_STATUS	status;			// Read status flags
	uADATRX_STATUS	statusAux;			// Read status flags
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
		unsigned user_data_aux	: 4;	// User data
		unsigned				: 24;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 24;	// Not used
		unsigned user_data_aux	: 4;	// User data
		unsigned user_data		: 4;	// User data
	} bit;
#endif

} uADATTX_USER;


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
	uADATTX_MUTE		chMute;			// Channel mute register
	uADATTX_MUTE		chMuteAux;		// Channel mute register
} DICE_ADATTX;

extern DICE_ADATRX *pDiceADATRx;
extern DICE_ADATTX *pDiceADATTx;

#endif
