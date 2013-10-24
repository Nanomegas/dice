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
	
	Purpose:	DICE - APB receiver and transmitter register definition
	
	Revisions:
		created 26/06/2003 kbj

****************************************************************************/

#ifndef _DICEAPBREG_H
#define _DICEAPBREG_H

// Number of samples received and tramsitted in one block
#define APB_NUM_SAMPLES		32

//*******************************************************************************
// APB status register
typedef	union 
{
	uint32 reg;

#ifdef _LITTLE_ENDIAN
	struct
	{
		unsigned irq_data		: 1;	// IRQ new sample data
		unsigned overflow		: 1;	// Receiver overflow
		unsigned				: 30;	// Not used
	} bit;
#else
	struct
	{
		unsigned				: 30;	// Not used
		unsigned overflow		: 1;	// Receiver overflow
		unsigned irq_data		: 1;	// IRQ new sample data
	} bit;
#endif

} uAPB_STATUS;

//*******************************************************************************
// APB receiver register structure
typedef struct
{
	int32			data[APB_NUM_SAMPLES];		// Rx and Tx data buffer
	uint32			unused[APB_NUM_SAMPLES]; 
	uAPB_STATUS		status;						// Read status flags
} DICE_APBAUDIO;


extern DICE_APBAUDIO *pDiceAPB;

#endif
