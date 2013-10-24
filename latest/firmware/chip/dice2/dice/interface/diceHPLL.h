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
	
	Purpose:	DICE - Hybrid PLL API interface
	
	Revisions:
		created 28/07/2003 kbj

****************************************************************************/

#ifndef _DICEHPLL_H
#define _DICEHPLL_H

#include "diceHPLLReg.h"

// HPLL identification number
#define HPLL_ID1			0
#define HPLL_ID2			1
#define NUM_HPLL			2

HRESULT hpllInitialize(void);

#define HPLL_LOW_RATE	0	// Set low rate mode (32-48 kHz)
#define HPLL_HIGH_RATE	1	// Set high rate mode (64-96 kHz)

// Set HPLL in high(96kHz) or low(48kHz) sample rate mode
// HRESULT hpllSetRate(uint8 hpllId, uint8 rate);

// Set HPLL in high(96kHz) or low(48kHz) sample rate mode for SYT clock source (AVS)
// HRESULT hpllSetSytRate(uint8 hpllId, uint8 rate);

// Set Phase offset in steps of 1/(256xbase rate)
// Legal values 0..one less than co_ndiv_f, default 0
HRESULT hpllSetPhaseOffset(uint8 hpllId, uint16 phase);

// Set Jitter BW, floor and ceiling
HRESULT hpllSetJitterBW(uint8 hpllId, uint8 floor, uint8 ceiling);

// Get Period count for current reference also difference between greatest and smallest period since last 'read'
HRESULT hpllGetPeriod(uint8 hpllId, uint16* pCount, uint16* pDiff);

// Set max. slew for locked and unlocked situations
HRESULT hpllSetMaxGlideRate(uint8 hpllId, uint8 locked, uint8 unlocked);

// Returns TRUE if locked
BOOL hpllIsLocked(uint8 hpllId);

// Setting clock value for HPLL
HRESULT hpllGetClock(uint32 *hpllClk);

HRESULT hpllSetClock(uint32 hpllClk);

//Set the dividers used by the Clock Controller. 
//preDiv: pre-divider used to bring high rates down to base rate before PLL
//addDiv: feedback divider to multiply ref_event.

#define HPLL_PREDIV_HALF 2
#define HPLL_PREDIV_NONE 1

HRESULT hpllSetDividers (uint8 hpllId, uint32 preDiv, uint32 addDiv);
HRESULT hpllGetDividers (uint8 hpllId, uint32 * pPreDiv, uint32 * pAddDiv);

#define HPLL_FORCE_RATE_32K		0
#define HPLL_FORCE_RATE_44_1K	1
#define HPLL_FORCE_RATE_48K		2

// Force the PLL to run on a fixed rate while setting up source etc.

//HRESULT hpllForceRate (uint8 hpllID, uint8 rate);

// Release PLL after call to force.
//HRESULT hpllRelease (uint8 hpllID);

HRESULT hpllCoast (uint8 hpllId, uint8 bCoast);


HRESULT hpllSetReference (uint8 hpllId, uint8 ref);

HRESULT hpllEnableClocks (uint8 hpllId, uint8 bEnable);

#endif
