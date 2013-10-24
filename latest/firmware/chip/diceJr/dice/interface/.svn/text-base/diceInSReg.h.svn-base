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
	
	Purpose:	DICE - InS receiver and transmitter register definition
	
	Revisions:
		created 01/02/2007 ml

****************************************************************************/

#ifndef _DICEINSREG_H
#define _DICEINSREG_H

// InS identification number
#define INS_ID0			0
#define INS_ID1			1
#define NUM_ID_INS		2


// InS receiver and transmitter lines 
#define INS_LN0			0
#define INS_LN1			1
#define INS_LN2			2
#define INS_LN3			3
#define NUM_INS_LINES	4

// Number of InS channels 
#define NUM_MAX_INS_CHANNELS		16

// Defines for MCK rate settings
#define INS_MCK_256BR	0x0	//MCK is always 256 times base rate (12MHz range)
#define INS_MCK_512BR	0x1	//MCK is always 512 times base rate (24MHz range)
#define INS_MCK_128FS	0x2	//MCK is 128 times sample rate (6/12/24MHz range)
#define INS_MCK_256FS	0x3	//MCK is 256 times sample rate (12/24MHz range, not valid for high rates)

// Defines for BCK rate settings
#define INS_BCK_64FS	0x0	//BCK is 64 times sample rate, 2 channel mode (3/6/12MHz range)
#define INS_BCK_128FS	0x1	//BCK is 128 times sample rate, 4 channel mode (6/12/24MHz range)
#define INS_BCK_256FS	0x2	//BCK is 256 times sample rate, 8 channel mode (12/24 MHz range, not valid for high rates)


// Define data shuffle settings (for a complete list, please refer to the users guide)
#define INS_SHFL_24MSB_FRST_LJST	0xc //typical I2S, 24 bits left justified, MSB first, last 8 bits are meta data
#define INS_SHFL_24MSB_FRST_RJST	0x8 //24 bits right justified MSB first, first 8 bits are meta data
#define INS_SHFL_32MSB_FRST			0x8 //32 bits MSB first, can be used for communicating RAW meta data with AES or 1394

#define INS_SHFL_24LSB_FRST_LJST	0xF //24 bits left justified, LSB first, last 8 bits are meta data
#define INS_SHFL_24LSB_FRST_RJST	0xb //24 bits right justified LSB first, first 8 bits are meta data
#define INS_SHFL_32LSB_FRST			0xF //32 bits LSB first, can be used for communicating RAW meta data with AES or 1394

// Define mode settings for line configuration
#define INS_MODE_I2S				0
#define INS_MODE_I4S				1
#define INS_MODE_I8S				2

// Define sync length configurations
#define INS_FSYN_LEN_1BIT			0
#define INS_FSYN_LEN_32BIT			1

//*******************************************************************************
// InS rx/tx clock setup register
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned	mck_rate	: 2;	//rate of the MCK pin
		unsigned	bck_rate	: 2;	//rate of the BCK pin
		unsigned	fsy_len		: 1;	//32 or 1 bit FSYNCK
		unsigned	mck_inv		: 1;	//Invert MCK 
		unsigned	bck_inv		: 1;	//Invert BCK
		unsigned	fsy_inv		: 1;	//Invert FSYNC
		unsigned	enable		: 1;	//Enable clock port
		unsigned				: 23;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 23;	// Not used
		unsigned	enable		: 1;	//Enable clock port
		unsigned	fsy_inv		: 1;	//Invert FSYNC
		unsigned	bck_inv		: 1;	//Invert BCK
		unsigned	mck_inv		: 1;	//Invert MCK 
		unsigned	fsy_len		: 1;	//32 or 1 bit FSYNCK
		unsigned	bck_rate	: 2;	//rate of the BCK pin
		unsigned	mck_rate	: 2;	//rate of the MCK pin
	} bit;
#endif

} uInS_CLK_SETUP;


//*******************************************************************************
// InS rx/tx line setup register
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned	mode		: 2;	//I2S, I4S or I8S mode
		unsigned	delay		: 2;	//Delay data relative to FSYNC
		unsigned	shuffle		: 4;	//See data shuffle table
		unsigned	enable		: 1;	//Enable this line 
		unsigned				: 23;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 23;	//Not used
		unsigned	enable		: 1;	//Enable this line 
		unsigned	shuffle		: 4;	//See data shuffle table
		unsigned	delay		: 2;	//Delay data relative to FSYNC
		unsigned	mode		: 2;	//I2S, I4S or I8S mode
	} bit;
#endif

} uInS_LINE_SETUP;


//*******************************************************************************
// InS tx Channel mute register
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
		unsigned ch8_mute		: 1;	// Mute for channel 8
		unsigned ch9_mute		: 1;	// Mute for channel 9
		unsigned ch10_mute		: 1;	// Mute for channel 10
		unsigned ch11_mute		: 1;	// Mute for channel 11
		unsigned ch12_mute		: 1;	// Mute for channel 12
		unsigned ch13_mute		: 1;	// Mute for channel 13
		unsigned ch14_mute		: 1;	// Mute for channel 14
		unsigned ch15_mute		: 1;	// Mute for channel 15
		unsigned				: 16;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 16;	// Not used
		unsigned ch15_Mute		: 1;	// Mute for channel 15
		unsigned ch14_Mute		: 1;	// Mute for channel 14
		unsigned ch13_Mute		: 1;	// Mute for channel 13
		unsigned ch12_Mute		: 1;	// Mute for channel 12
		unsigned ch11_Mute		: 1;	// Mute for channel 11
		unsigned ch10_Mute		: 1;	// Mute for channel 10
		unsigned ch9_Mute		: 1;	// Mute for channel 9
		unsigned ch8_Mute		: 1;	// Mute for channel 8
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

} uInSTX_CH_MUTE;



#endif
