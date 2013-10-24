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
	
	Purpose:	DICE - APB receiver and transmitter API
	
	Revisions:
		created 26/06/2003 kbj

****************************************************************************/

#include "TCTypes.h"
#include "TCDebug.h"
#include "TCInterrupt.h"
#include "ErrorCodes.h"
#include "diceDefs.h"
#include "diceAPB.h"

#if 0
static int32 apbVolume = 256;
#endif

//static char 	moduleName[] = __FILE__ ;

DICE_APBAUDIO *pDiceAPB = (DICE_APBAUDIO *)(DICEBASE + DICE_APB_BASE);

// Clear everything 
HRESULT apbInitialize(void)
{
	HRESULT hResult = NO_ERROR;
	
	// Clear interrupts
	pDiceAPB->status.bit.irq_data = 1;

	hResult = TCInterruptCreateAPB (apbLoopTest);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}


HRESULT apbSetVolume(int32 volume)
{
	HRESULT hResult = NO_ERROR;

	if (volume >= 0 && volume <= 256)
	{
#if 0
		apbVolume = volume;
#endif
	}

	return hResult;
}

#if 0
// Waits for APB data -> copy received samples to transmitter
void apbLoopTest(void)
{
	uint8 i;
	//static int32 sample[APB_NUM_SAMPLES];
	static int32 data;
	static int32 overflow =  0;
	int32 label;
	
	// If new sample data then copy 
	if (pDiceAPB->status.bit.irq_data == 1)
	{
		for (i = 0; i < APB_NUM_SAMPLES; i++)
		{
#if 1		// Volume control
			label = pDiceAPB->data[i]&0xff000000;
			data = ((pDiceAPB->data[i]&0x00ffffff) << 8);
			data = ((data >> 8) * apbVolume) >> 8;
			pDiceAPB->data[i] = label | (data & 0x00ffffff);
#else
			// Read from receive buffer, only ch 0-3
#if 1		// Test 96kHz			
			if (i%8 > 3) continue;
#endif
			sample[i] = pDiceAPB->data[i];
			pDiceAPB->data[i] = sample[i];
#endif
		}
		if (pDiceAPB->status.bit.overflow == 1)
		{
			overflow++; // Check APB overflow
		}

		// Cear new data interrupt
		pDiceAPB->status.bit.irq_data = 1;
	}

	return 0;
}

#else //LM???

#if 0
#define M6TextSize				(188/4)
static uint32 M6plainText[M6TextSize] =		{0x47010203,0x04050607,0x08090a0b,0x0c0d0e0f,
											 0x10111213,0x14151617,0x18191a1b,0x1c1d1e1f,
											 0x20212223,0x24252627,0x28292a2b,0x2c2d2e2f,
											 0x30313233,0x34353637,0x38393a3b,0x3c3d3e3f,
											 0x40414243,0x44454647,0x48494a4b,0x4c4d4e4f,
											 0x50515253,0x54555657,0x58595a5b,0x5c5d5e5f,
											 0x60616263,0x64656667,0x68696a6b,0x6c6d6e6f,
											 0x70717273,0x74757677,0x78797a7b,0x7c7d7e7f,
											 0x80818283,0x84858687,0x88898a8b,0x8c8d8e8f,
											 0x90919293,0x94959697,0x98999a9b,0x9c9d9e9f,
											 0xa0a1a2a3,0xa4a5a6a7,0xa8a9aaab,0xacadaeaf,
											 0xb0b1b2b3,0xb4b5b6b7,0xb8b9babb};

static uint32 M6CurPosition = 0;
#endif

static uint32 M6Count = 0;
static uint32 M6offset = 0;

// copy M6 data to transmitter
void apbLoopTest(void)
{
#if 0
	uint8 i;
	static int32 data;
#endif
	static int32 overflow =  0;
	
	// If new sample data then copy 
	if (pDiceAPB->status.bit.irq_data == 1)
	{
#if 1
#if 0
		pDiceAPB->data[0x00] = 0 + M6offset;
		pDiceAPB->data[0x01] = 1 + M6offset;
		pDiceAPB->data[0x08] = 2 + M6offset;
		pDiceAPB->data[0x09] = 3 + M6offset;
		pDiceAPB->data[0x10] = 4 + M6offset;
		pDiceAPB->data[0x11] = 5 + M6offset;
		pDiceAPB->data[0x18] = 6 + M6offset;
		pDiceAPB->data[0x19] = 7 + M6offset;
#else
		pDiceAPB->data[0x10] = 0 + M6offset;
		pDiceAPB->data[0x11] = 1 + M6offset;
		pDiceAPB->data[0x18] = 2 + M6offset;
		pDiceAPB->data[0x19] = 3 + M6offset;
		pDiceAPB->data[0x00] = 4 + M6offset;
		pDiceAPB->data[0x01] = 5 + M6offset;
		pDiceAPB->data[0x08] = 6 + M6offset;
		pDiceAPB->data[0x09] = 7 + M6offset;
#endif
		if (M6offset == 0) M6offset = 8;
		else M6offset = 0;
#else
#if 1
		M6CurPosition = 0;
#endif
		pDiceAPB->data[0] = M6plainText[M6CurPosition++];
		if (M6CurPosition >= M6TextSize) M6CurPosition = 0;
		pDiceAPB->data[1] = M6plainText[M6CurPosition++];
		if (M6CurPosition >= M6TextSize) M6CurPosition = 0;
		pDiceAPB->data[8] = M6plainText[M6CurPosition++];
		if (M6CurPosition >= M6TextSize) M6CurPosition = 0;
		pDiceAPB->data[9] = M6plainText[M6CurPosition++];
		if (M6CurPosition >= M6TextSize) M6CurPosition = 0;
		pDiceAPB->data[16] = M6plainText[M6CurPosition++];
		if (M6CurPosition >= M6TextSize) M6CurPosition = 0;
		pDiceAPB->data[17] = M6plainText[M6CurPosition++];
		if (M6CurPosition >= M6TextSize) M6CurPosition = 0;
		pDiceAPB->data[24] = M6plainText[M6CurPosition++];
		if (M6CurPosition >= M6TextSize) M6CurPosition = 0;
		pDiceAPB->data[25] = M6plainText[M6CurPosition++];
		if (M6CurPosition >= M6TextSize) M6CurPosition = 0;
#endif
		M6Count++;
		if (pDiceAPB->status.bit.overflow == 1)
		{
			overflow++; // Check APB overflow
		}

		// Cear new data interrupt
		pDiceAPB->status.bit.irq_data = 1;
	}
}

#endif
