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
	
	Purpose:	DICE - I2S receiver and transmitter register definition
	
	Revisions:
		created 26/06/2003 kbj

****************************************************************************/

#ifndef _DICEI2SREG_H
#define _DICEI2SREG_H

// Number of max I2S receivers and transmitters data lines (only 2 for I2S1 and 2)
#define NUM_I2S_LINES			4

// Number of I2S channels 
#define NUM_I2S_CHANNELS		8

//*******************************************************************************
// I2S receive clock setup register
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned mck_in_freq	: 1;	//
		unsigned m192			: 1;
		unsigned				: 30;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 30;	// Not used
		unsigned m192			: 1;
		unsigned mck_in_freq	: 1;	//
	} bit;
#endif

} uI2SRX_CLK_SETUP;

//*******************************************************************************
// I2S receive status flag register, All errors are sticky cleared when read
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned mclk_out_div	: 2;	//only valid for data line 0
		unsigned bick_inv		: 1;	//only valid for data line 0
		unsigned lrck_out_inv	: 1;	//only valid for data line 0
		unsigned data_delay		: 1;	// 
		unsigned data_in_size	: 1;	// 
		unsigned l_justify		: 1;	//
		unsigned msb_first		: 1;	//
		unsigned mclk_out_en	: 1;	//only valid for data line 0
		unsigned				: 23;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 23;	// Not used
		unsigned mclk_out_en	: 1;	//
		unsigned msb_first		: 1;	//
		unsigned l_justify		: 1;	//
		unsigned data_in_size	: 1;	// 
		unsigned data_delay		: 1;	// 
		unsigned lrck_out_inv	: 1;	// 
		unsigned bick_inv		: 1;	//
		unsigned mclk_out_div	: 2;	//
	} bit;
#endif

} uI2SRX_CH_SETUP;

//*******************************************************************************
// I2S receiver register structure
typedef struct
{
	uI2SRX_CLK_SETUP	clkSetup;			// Setup clock
	uI2SRX_CH_SETUP		chSetup[NUM_I2S_LINES];	// Setup channels
} DICE_I2SRX;

//*******************************************************************************
// I2S transmitter setup clock register
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned sys_mck_freq	: 1;	//
		unsigned m192			: 1;
		unsigned				: 30;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 30;	// Not used
		unsigned m192			: 1;
		unsigned sys_mck_freq	: 1;	//
	} bit;
#endif

} uI2STX_CLK_SETUP;

//*******************************************************************************
// I2S transmitter setup channel register
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned mclk_out_div   : 2;	//only valid for data line 0
		unsigned bick_inv		: 1;	//only valid for data line 0
		unsigned lrck_out_inv	: 1;	//only valid for data line 0
		unsigned data_delay		: 1;	// 
		unsigned data_in_size	: 1;	// 
		unsigned l_justify		: 1;	// 
		unsigned msb_first		: 1;	// 
		unsigned mclk_out_en	: 1;	//only valid for data line 0
		unsigned				: 23;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 23;	// Not used
		unsigned mclk_out_en	: 1;	//
		unsigned msb_first		: 1;	// 
		unsigned l_justify		: 1;	// 
		unsigned data_in_size	: 1;	// 
		unsigned data_delay		: 1;	// 
		unsigned lrck_out_inv	: 1;	// 
		unsigned bick_inv		: 1;	// 
		unsigned mclk_out_div   : 2;	//
	} bit;
#endif

} uI2STX_CH_SETUP;

//*******************************************************************************
// I2S transmitter Channel mute register
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

} uI2STX_CH_MUTE;

//*******************************************************************************
// I2S transmitter register structure
typedef struct
{
	uI2STX_CLK_SETUP	clkSetup;			// Clock setup
	uI2STX_CH_SETUP		chSetup[NUM_I2S_LINES];	// Channel setup
	uI2STX_CH_MUTE		chMute;				// Channel mute
} DICE_I2STX;

extern DICE_I2SRX *pDiceI2SRx0;
extern DICE_I2STX *pDiceI2STx0;
extern DICE_I2SRX *pDiceI2SRx1;
extern DICE_I2STX *pDiceI2STx1;
extern DICE_I2SRX *pDiceI2SRx2;
extern DICE_I2STX *pDiceI2STx2;

#endif
