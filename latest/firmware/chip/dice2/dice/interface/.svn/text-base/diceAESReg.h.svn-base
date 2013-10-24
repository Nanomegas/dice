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
	
	Purpose:	DICE - AES receiver and transmitter register definition
	
	Revisions:
		created 12/06/2003 kbj

****************************************************************************/

#ifndef _DICEAESREG_H
#define _DICEAESREG_H

// Number of AES receivers and transmitters
#define NUM_AES						4

// Number of AES channels 
#define NUM_AES_CHANNELS			(NUM_AES*2)

// Number of channel status bytes
#define NUM_AES_CS_BYTES			4

// Number of selected channel status bytes 
#define NUM_AES_SEL_CS_BYTES 		24

//*******************************************************************************
// AES receive setup register
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned cs_channel		: 3;	// Selects from which channel all cs bits should be sampled
		unsigned				: 3;	// Not used
		unsigned master_clock   : 2;	// Master receiver clock
		unsigned				: 24;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 24;	// Not used
		unsigned master_clock   : 2;	// Master receiver clock
		unsigned				: 3;	// Not used
		unsigned cs_channel		: 3;	// Selects from which channel all cs bits should be sampled
	} bit;
#endif

} uAESRX_SETUP;

//*******************************************************************************
// AES receive error flag register, All errors are sticky cleared when read
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned lock			: 1;	// Rx channel is in lock
		unsigned no_lock		: 1;	// Rx channel has been out of lock since last read
		unsigned crc_err		: 1;	// CRC error has been detected in rx channel
		unsigned pty_err		: 1;	// Parity error has been detected in rx channel
		unsigned v_high			: 1;	// The v bit has heen detected as 1 in rx channel
		unsigned o_run			: 1;	// Slip sample error (system 1fs slower than 1fs master AES rx)
		unsigned u_run			: 1;	// Resample error (system 1fs faster than 1fs master AES rx)
		unsigned uo_run			: 1;	// Or'ed function of resample and slip error bits
		unsigned				: 24;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 24;	// Not used
		unsigned uo_run			: 1;	// Or'ed function of resample and slip error bits
		unsigned u_run			: 1;	// Resample error (system 1fs faster than 1fs master AES rx)
		unsigned o_run			: 1;	// Slip sample error (system 1fs slower than 1fs master AES rx)
		unsigned v_high			: 1;	// The v bit has heen detected as 1 in rx channel
		unsigned pty_err		: 1;	// Parity error has been detected in rx channel
		unsigned crc_err		: 1;	// CRC error has been detected in rx channel
		unsigned no_lock		: 1;	// Rx channel has been out of lock since last read
		unsigned lock			: 1;	// Rx channel is in lock
	} bit;
#endif

} uAESRX_ERROR;

//*******************************************************************************
// AES receive PLL pulse width register
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned down_pulse_w   : 4;	// PLL down pulse width 1-16 cycles
		unsigned up_pulse_w		: 4;	// PLL up pulse width 1-16 cycles
		unsigned				: 24;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 24;	// Not used
		unsigned up_pulse_w		: 4;	// PLL up pulse width 1-16 cycles
		unsigned down_pulse_w   : 4;	// PLL down pulse width 1-16 cycles
	} bit;
#endif

} uAESRX_PLL_PULSE;

//*******************************************************************************
// AES receive force VCO register
typedef	union 
{
	uint32 reg;
#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned force_down		: 1;	// PLL force down pulse
		unsigned force_up		: 1;	// PLL force up pulse 
		unsigned				: 30;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 30;	// Not used
		unsigned force_up		: 1;	// PLL force up pulse 
		unsigned force_down		: 1;	// PLL force down pulse
	} bit;
#endif

} uAESRX_FORCE_VCO;

//*******************************************************************************
// AES receiver register structure
typedef struct
{
	uAESRX_SETUP		setup;				// Setup register
	uAESRX_ERROR		error;				// Read error flags, bits 0-4 is unused (all zero)
	uAESRX_ERROR		rxError[NUM_AES];	// Error flags for receiver 0-3
	uint32				notUsed1[10];		// Not used space
	uAESRX_PLL_PULSE	pllPulse;			// PLL pulse width
	uAESRX_FORCE_VCO	forceVCO;			// Force VCO
	uint32				minVCOFqLSB;		// VCO min. frequence LSB
	uint32				minVCOFqMSB;		// VCO max. frequence MSB
	uint32				notUsed2[12];		// Not used space

	// AESRX channel status bytes 0-3
	uint32			csChannel[NUM_AES_CHANNELS][NUM_AES_CS_BYTES];	

	// AESRX channel status bytes 0-23 for channel selected in setup
	uint32			csChannelSel[NUM_AES_SEL_CS_BYTES];
} DICE_AESRX;

//*******************************************************************************
// AES transmitter mode setup register
typedef	union 
{
	uint32 reg;
#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned mode			: 1;	// Master mode(0) or slave mode(1)
		unsigned				: 3;	// Not used
		unsigned crc_disable	: 4;	// Auto CRC disable
		unsigned				: 24;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 24;	// Not used
		unsigned crc_disable	: 4;	// Auto CRC disable
		unsigned				: 3;	// Not used
		unsigned mode			: 1;	// Master mode(0) or slave mode(1)
	} bit;
#endif

} uAESTX_MODE_SETUP;

// Disable all tx channels
#define	AESTX_CRC_ENABLE	0x0
#define	AESTX_CRC_DISABLE	0xf

// Values for mode
#define	AESTX_MASTER_MODE	0
#define	AESTX_SLAVE_MODE	1

//*******************************************************************************
// AES transmitter CBL setup register
typedef	union 
{
	uint32 reg;
#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned slave_mode		: 3;	// Channel block sync generated from channel 0-7
		unsigned				: 1;	// Not used
		unsigned master_mode	: 1;	// Channel block sync generated intern or extern
		unsigned				: 27;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 27;	// Not used
		unsigned master_mode	: 1;	// Channel block sync generated intern or extern
		unsigned				: 1;	// Not used
		unsigned slave_mode		: 3;	// Channel block sync generated from channel 0-7
	} bit;
#endif

} uAESTX_CBL_SETUP;

//*******************************************************************************
// AES transmitter CS select register
typedef	union 
{
	uint32 reg;
#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned ch0_cs			: 2;	// CS bit select in slave mode for channel 0/4
		unsigned ch1_cs			: 2;	// CS bit select in slave mode for channel 1/5
		unsigned ch2_cs			: 2;	// CS bit select in slave mode for channel 2/6
		unsigned ch3_cs			: 2;	// CS bit select in slave mode for channel 3/7
		unsigned				: 24;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 24;	// Not used
		unsigned ch3_cs			: 2;	// CS bit select in slave mode for channel 3/7
		unsigned ch2_cs			: 2;	// CS bit select in slave mode for channel 2/6
		unsigned ch1_cs			: 2;	// CS bit select in slave mode for channel 1/5
		unsigned ch0_cs			: 2;	// CS bit select in slave mode for channel 0/4
	} bit;
#endif

} uAESTX_CS_SEL;

//*******************************************************************************
// AES transmitter APB CS select register
typedef	union 
{
	uint32 reg;
#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned ch0_cs_apb		: 1;	// CS bit select from APB for channel 0
		unsigned ch1_cs_apb		: 1;	// CS bit select from APB for channel 1
		unsigned ch2_cs_apb		: 1;	// CS bit select from APB for channel 2
		unsigned ch3_cs_apb		: 1;	// CS bit select from APB for channel 3
		unsigned ch4_cs_apb		: 1;	// CS bit select from APB for channel 4
		unsigned ch5_cs_apb		: 1;	// CS bit select from APB for channel 5
		unsigned ch6_cs_apb		: 1;	// CS bit select from APB for channel 6
		unsigned ch7_cs_apb		: 1;	// CS bit select from APB for channel 7
		unsigned				: 24;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 24;	// Not used
		unsigned ch7_cs_apb		: 1;	// CS bit select from APB for channel 7
		unsigned ch6_cs_apb		: 1;	// CS bit select from APB for channel 6
		unsigned ch5_cs_apb		: 1;	// CS bit select from APB for channel 5
		unsigned ch4_cs_apb		: 1;	// CS bit select from APB for channel 4
		unsigned ch3_cs_apb		: 1;	// CS bit select from APB for channel 3
		unsigned ch2_cs_apb		: 1;	// CS bit select from APB for channel 2
		unsigned ch1_cs_apb		: 1;	// CS bit select from APB for channel 1
		unsigned ch0_cs_apb		: 1;	// CS bit select from APB for channel 0
	} bit;
#endif

} uAESTX_APB_CS_SEL;

//*******************************************************************************
// AES transmitter Channel mute register
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
		unsigned ch7_mute		: 1;	// Mute for channel 7
		unsigned				: 24;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 24;	// Not used
		unsigned ch7_mute		: 1;	// Mute for channel 7
		unsigned ch6_mute		: 1;	// Mute for channel 6
		unsigned ch5_mute		: 1;	// Mute for channel 5
		unsigned ch4_mute		: 1;	// Mute for channel 4
		unsigned ch3_mute		: 1;	// Mute for channel 3
		unsigned ch2_mute		: 1;	// Mute for channel 2
		unsigned ch1_mute		: 1;	// Mute for channel 1
		unsigned ch0_mute		: 1;	// Mute for channel 0
	} bit;
#endif

} uAESTX_CH_MUTE;

//*******************************************************************************
// AES transmitter validity bit in master mode
typedef	union 
{
	uint32 reg;
#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned ch0_validity	: 1;	// Validity bit for channel 0
		unsigned ch1_validity	: 1;	// Validity bit for channel 1
		unsigned ch2_validity	: 1;	// Validity bit for channel 2
		unsigned ch3_validity	: 1;	// Validity bit for channel 3
		unsigned ch4_validity	: 1;	// Validity bit for channel 4
		unsigned ch5_validity	: 1;	// Validity bit for channel 5
		unsigned ch6_validity	: 1;	// Validity bit for channel 6
		unsigned ch7_validity	: 1;	// Validity bit for channel 7
		unsigned				: 24;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 24;	// Not used
		unsigned ch7_validity	: 1;	// Validity bit for channel 7
		unsigned ch6_validity	: 1;	// Validity bit for channel 6
		unsigned ch5_validity	: 1;	// Validity bit for channel 5
		unsigned ch4_validity	: 1;	// Validity bit for channel 4
		unsigned ch3_validity	: 1;	// Validity bit for channel 3
		unsigned ch2_validity	: 1;	// Validity bit for channel 2
		unsigned ch1_validity	: 1;	// Validity bit for channel 1
		unsigned ch0_validity	: 1;	// Validity bit for channel 0
	} bit;
#endif

} uAESTX_CH_VALIDITY;

//*******************************************************************************
// AES transmitter USER select register
typedef	union 
{
	uint32 reg;
#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned ch0_cs_user	: 4;	// USER bit select in master mode for channel 0/2/4/6
		unsigned ch1_cs_user	: 4;	// USER bit select in master mode for channel 1/3/5/7
		unsigned				: 24;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 24;	// Not used
		unsigned ch1_cs_user	: 4;	// USER bit select in master mode for channel 1/3/5/7
		unsigned ch0_cs_user	: 4;	// USER bit select in master mode for channel 0/2/4/6
	} bit;
#endif

} uAESTX_USER_SEL;

//*******************************************************************************
// AES transmitter register structure
typedef struct
{
	uAESTX_MODE_SETUP	modeSetup;				// Mode setup register
	uAESTX_CBL_SETUP	cblSetup;				// CBL setup register
	uAESTX_CS_SEL		csSelCh0_3;				// CS select channel 0-3 (slave mode)
	uAESTX_CS_SEL		csSelCh4_7;				// CS select channel 4-7 (slave mode)
	uAESTX_APB_CS_SEL	csSelApb;				// APB CS select channel 0-7 (slave mode)
	uAESTX_CH_MUTE		chMute;					// Channel mute
	uAESTX_CH_VALIDITY  chValidity;				// Channel validity bit in (master mode)
	uint32				notUsed1[9];			// Not used space
	uAESTX_USER_SEL		userSelCh[NUM_AES];		// USER select channel [0-1],[2-3],[4-5],[6-7] (master mode)
	uint32				notUsed2[12];			// Not used space

	// AESTX channel status bytes 0-3 for each transmitted channel
	uint32				csChannel[NUM_AES_CHANNELS][NUM_AES_CS_BYTES];	

	// AESTX channel status bytes 0-23 for all channels
	uint32				csChannelSel[NUM_AES_SEL_CS_BYTES];
} DICE_AESTX;

extern DICE_AESRX *pDiceAESRx;
extern DICE_AESTX *pDiceAESTx;

#endif
