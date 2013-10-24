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
	
	Purpose:	DICE - Hybrid PLL API implementation
	
	Revisions:
		created 28/07/2003 kbj

****************************************************************************/

#include "TCTypes.h"
#include "TCDebug.h"
#include "ErrorCodes.h"
#include "diceDefs.h"
#include "diceHPLL.h"
#include "targetBoard.h"

static char 	moduleName[] = __FILE__ ;

DICE_HPLL *pDiceHPLL1 = (DICE_HPLL *)(DICE_HYBRID_PLL1_BASE);
DICE_HPLL *pDiceHPLL2 = (DICE_HPLL *)(DICE_HYBRID_PLL2_BASE);

HRESULT hpllInitialize(void)
{
	HRESULT hResult = NO_ERROR;
	uint16 tmp;

	// Select internal VCO
	pDiceHPLL1->caf_select = 0;
	pDiceHPLL2->caf_select = 0;

	// Turn on internal VCO
	pDiceHPLL1->shutdown_i = 0;
	pDiceHPLL2->shutdown_i = 0;

	// (No) Compensation for Phase offset in HPLL
	pDiceHPLL1->phase_lag = 0;
	pDiceHPLL2->phase_lag = 0;

	// Cleanly turns on the clock and frame output, should be off on start
	pDiceHPLL1->caf_enable = 0;
	pDiceHPLL2->caf_enable = 0;

	// Select the timing reference to lock to
	pDiceHPLL1->ref_select = 1;
	pDiceHPLL2->ref_select = 1;
	
	pDiceHPLL1->mtr_select = 1;
    pDiceHPLL2->mtr_select = 1;
	
	// Select the edges to lock to (pos/neg/both)
	pDiceHPLL1->ref_edges = 0;
	pDiceHPLL2->ref_edges = 0;
	

	// Controls the reference divider (NL Prescaler)
	pDiceHPLL1->rdiv = 0; 
	pDiceHPLL2->rdiv = 0; 
	
	pDiceHPLL1->ndiv_f = 0xff; // numeric divider = 256
	pDiceHPLL2->ndiv_f = 0xff; // numeric divider = 256

	// Output goes to Min frequency when aquisition
	pDiceHPLL1->gravity = 1;
	pDiceHPLL2->gravity = 1;

	// Setting for Dice II
	pDiceHPLL1->edet_x1 = 0;
	pDiceHPLL1->edet_x1 = 0;
	pDiceHPLL2->edet_x2 = 0;
	pDiceHPLL2->edet_x2 = 0;

	// Make mtr_period count integer M-clock ticks 
	// and clear the stored counts
	pDiceHPLL1->res_ex = 9;
	pDiceHPLL2->res_ex = 9;
	tmp = pDiceHPLL1->greatest_mp_;
	tmp = pDiceHPLL1->smallest_mp_;
	tmp = pDiceHPLL2->greatest_mp_;
	tmp = pDiceHPLL2->smallest_mp_;

	// Ext PFD
	pDiceHPLL1->x1x2_mode = 1;
	pDiceHPLL1->tick_rate = 9;
	pDiceHPLL2->x1x2_mode = 1;
	pDiceHPLL2->tick_rate = 9;	

	return hResult;
}

static DICE_HPLL *hpllPtr(uint8 hpllID)
{
	switch (hpllID)
	{
		case HPLL_ID1:
			return pDiceHPLL1;
		case HPLL_ID2:
			return pDiceHPLL2;
	}
	return 0;
}


// Set HPLL in high(96kHz) or low(48kHz) sample rate mode
// ML, not used any more
/*
HRESULT hpllSetRate(uint8 hpllId, uint8 rate)
{
	HRESULT hResult = NO_ERROR;

	if (hpllId < NUM_HPLL)
	{
		DICE_HPLL* pHPLL = hpllPtr(hpllId);

		pHPLL->ndiv_e = 0x0; // set to default

		if (rate == HPLL_HIGH_RATE)
		{
			pHPLL->rdiv = 0x01;
		}
		else
		{
			pHPLL->rdiv = 0x00; 
		}

		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}
*/

// Set HPLL in high(96kHz) or low(48kHz) sample rate mode for SYT clock source (AVS)
// ML, not used any more
/*
HRESULT hpllSetSytRate(uint8 hpllId, uint8 rate)
{
	HRESULT hResult = NO_ERROR;

	if (hpllId < NUM_HPLL)
	{
		DICE_HPLL* pHPLL = hpllPtr(hpllId);

		pHPLL->ndiv_e = 0x07; // SYT rate is 1/8 of Fs
		pHPLL->ndiv_f = 0xff; // numeric divider = 256
	  pHPLL->rdiv   = 0x00;
		
    return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}
*/

//Set the dividers used by the Clock Controller. 
//pre_div: pre-divider used to bring high rates down to base rate before PLL
//add_div: feedback divider to multiply ref_event.

HRESULT hpllSetDividers (uint8 hpllId, uint32 preDiv, uint32 addDiv)
{
	HRESULT hResult = NO_ERROR;

	if (hpllId < NUM_HPLL && (preDiv > 0) && (addDiv > 0))
	{ 
		DICE_HPLL* pHPLL = hpllPtr(hpllId);

		pHPLL->ndiv_e = addDiv-1;
		pHPLL->rdiv = preDiv-1;		
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

HRESULT hpllGetDividers (uint8 hpllId, uint32 * pPreDiv, uint32 * pAddDiv)
{
	HRESULT hResult = NO_ERROR;

	if (hpllId < NUM_HPLL)
	{ 
		DICE_HPLL* pHPLL = hpllPtr(hpllId);

		*pAddDiv = pHPLL->ndiv_e + 1;
		*pPreDiv = pHPLL->rdiv + 1;		
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}



// Set Phase offset in steps of 1/(256xbase rate)
// Legal values 0..one less than co_ndiv_f, default 0
HRESULT hpllSetPhaseOffset(uint8 hpllId, uint16 phase)
{
	HRESULT hResult = NO_ERROR;

	if (hpllId < NUM_HPLL)
	{
		DICE_HPLL* pHPLL = hpllPtr(hpllId);

		if (phase < pHPLL->ndiv_f)
		{
			pHPLL->phase_lag = phase;
			return hResult;
		}
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set Jitter BW, floor and ceiling
HRESULT hpllSetJitterBW(uint8 hpllId, uint8 floor, uint8 ceiling)
{
	HRESULT hResult = NO_ERROR;

	if (hpllId < NUM_HPLL)
	{
		DICE_HPLL* pHPLL = hpllPtr(hpllId);

		pHPLL->bandwidth_f = floor;
		pHPLL->bandwidth_c = ceiling;
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set max. slew for locked and unlocked situations
HRESULT hpllSetMaxGlideRate(uint8 hpllId, uint8 locked, uint8 unlocked)
{
	HRESULT hResult = NO_ERROR;

	if (hpllId < NUM_HPLL)
	{
		DICE_HPLL* pHPLL = hpllPtr(hpllId);

		pHPLL->max_slew_f = locked;
		pHPLL->max_slew_v = unlocked;
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Returns TRUE if locked
BOOL hpllIsLocked(uint8 hpllId)
{
	if (hpllId < NUM_HPLL)
	{
		DICE_HPLL* pHPLL = hpllPtr(hpllId);

		if (pHPLL->main_status & HPLL_ST_UNLOCKED)
			return FALSE;
		else
			return TRUE;
	}

	return FALSE;
}

// Get Period count for current reference also difference between greatest and smallest period since last 'read'
HRESULT hpllGetPeriod(uint8 hpllId, uint16* pCount, uint16* pDiff)
{
	HRESULT hResult = NO_ERROR;

	if (hpllId < NUM_HPLL)
	{
		DICE_HPLL* pHPLL = hpllPtr(hpllId);

		*pCount = pHPLL->mtr_period;
    *pDiff  = pHPLL->greatest_mp_ - pHPLL->smallest_mp_;
    return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

static uint32 hpllClkHz = HPLL_CLK_HZ; // HPLL clock

// Setting clock value for HPLL
HRESULT hpllGetClock(uint32 *hpllClk)
{
    HRESULT hResult = NO_ERROR;
 
    *hpllClk = hpllClkHz;
 
    return hResult;
}
 
HRESULT hpllSetClock(uint32 hpllClk)
{
    HRESULT hResult = NO_ERROR;
 
    hpllClkHz = hpllClk;
 
    return hResult;
}

typedef struct
{
	uint32	rdiv;
	uint32	ndiv_e;
} PLL_FORCED_RATES;

static const PLL_FORCED_RATES hpllForcedRates[2][3] =
{
	{
		//49.152MHz definitions
		{1, 768}, //32KHz
		{11,6130},//44.1KHz
		{1, 512}  //48KHz
	},
	{
		//50.000MHz definitions
		{4, 3125}, //32KHz
		{9, 5102}, //44.1KHz
		{6, 3125}  //48KHz
	}
};	

/*
HRESULT hpllForceRate (uint8 hpllId, uint8 rate)
{
	PLL_FORCED_RATES rates;
	HRESULT hResult = NO_ERROR;


	if ((hpllId >= NUM_HPLL) || ((hpllClkHz != 49152000) && (hpllClkHz != 50000000)))
	{
   		hResult = E_DICE_BAD_INPUT_PARAM;
   		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}		
	DICE_HPLL* pHPLL = hpllPtr(hpllId);
	
	if (hpllClkHz == 49152000)
	{
		rates = hpllForcedRates[0][rate];		
	}
	if (hpllClkHz == 50000000)
	{
		rates = hpllForcedRates[1][rate];
	}
	pHPLL->ndiv_e = rates.ndiv_e-1;
	pHPLL->rdiv = rates.rdiv -1;		
    pHPLL->ref_select = 0;


    return hResult;
}
*/

HRESULT hpllSetReference (uint8 hpllId, uint8 ref)
{
	HRESULT hResult = NO_ERROR;
	
	if (hpllId >= NUM_HPLL)
	{
   		hResult = E_DICE_BAD_INPUT_PARAM;
   		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}		
	DICE_HPLL* pHPLL = hpllPtr(hpllId);
	pHPLL->ref_select = ref;
	return hResult;
}

/*
HRESULT hpllRelease (uint8 hpllId)
{
	HRESULT hResult = NO_ERROR;
	
	if (hpllId >= NUM_HPLL)
	{
   		hResult = E_DICE_BAD_INPUT_PARAM;
   		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}		
	DICE_HPLL* pHPLL = hpllPtr(hpllId);
	pHPLL->ref_select = 1;
	return hResult;
}
*/

HRESULT hpllCoast (uint8 hpllId, uint8 bCoast)
{
	HRESULT hResult = NO_ERROR;
	
	if (hpllId >= NUM_HPLL)
	{
   		hResult = E_DICE_BAD_INPUT_PARAM;
   		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}		
	DICE_HPLL* pHPLL = hpllPtr(hpllId);
	pHPLL->coast = bCoast ? 1 : 0;
	return hResult;
}	

HRESULT hpllEnableClocks (uint8 hpllId, uint8 bEnable)
{
	HRESULT hResult = NO_ERROR;
	
	if (hpllId >= NUM_HPLL)
	{
   		hResult = E_DICE_BAD_INPUT_PARAM;
   		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}		
	DICE_HPLL* pHPLL = hpllPtr(hpllId);
	pHPLL->caf_enable = bEnable ? 1 : 0;
	return hResult;
}	
