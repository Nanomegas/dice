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
	
	Purpose:	Dice Audio Application Layer (DAL)- ADAT wrapper
				This module interfaces the DAL to the DICE ADAT
	
	Revisions:
		created 09/03/2005 ML

****************************************************************************/

#include <string.h>
#include "TCTypes.h"
#include "TCDebug.h"
#include "TCTasking.h"
#include "ErrorCodes.h"
#include "dice.h"
#include "diceADAT.h"
#include "dalADAT.h"
#include "dalRates.h"

static char 	moduleName[] = __FILE__ ;

static DAL_ADAT_SMUX_ENUM adatMode = eDAL_ADAT_SMUX_NORMAL;
static uint8 adatUserSel = 0;

// Obtain interface specific PLL settings and rate mode estimate
// The ADAT carrier will always run at base rate.
void dalADATPllSettings (uint32 data, DAL_RX_PLL_DATA * pPllData)
{
	uint32 refHz = pPllData->ref8Hz;
	RATE_DESC * pRate;
	uint8	userBits, shift=0;
	
	if (adatMode == eDAL_ADAT_SMUX_FORCE)
	{
		if (adatUserSel) shift = 1; //SMUX enabled by application
	}
	else
	{
		if (pPllData->rateMode == eDAL_RATE_MODE_MID)
			shift = 1; //assume SMUX
		else if (pPllData->rateMode == eDAL_RATE_MODE_HIGH)
			shift = 2; //assume SMUX 192
		else if (pPllData->rateMode == eDAL_RATE_MODE_LOW_MID)
		{
			if (adatUserSel) 
				shift = 1; //SMUX enabled by application
			else if (adatMode == eDAL_ADAT_SMUX_NORMAL)
			{
				adatRxGetUserData(&userBits);
				if (userBits & ADAT_SMUX_USER_MASK) shift = 1; //SMUX enabled by user bit
			}				
		}
	}
	refHz <<= shift;
	// convert to Hz
	refHz >>= 3; // divide by 8
	//now find the nearest ID in the table
	pRate = dalRateFindNearest (&pPllData->rateID, refHz);
	
	pPllData->rateHZ = refHz;
	//if we didn't find any match, return (rateID is set to none)
	if (!pRate) return;
	
	//ADAT reference is always base rate
	pPllData->preDiv = 1;
	//no additional feedback divider needed
	pPllData->addDiv = 1;	
}




// Is the ADAT interface locked
uint8 dalADATIsLocked (uint32 data)
{
	return (uint8)adatIsLocked ();
}

HRESULT dalSetAdatRxSmuxMode  (DAL_ADAT_SMUX_ENUM mode, uint8 usersel)
{
	HRESULT hResult = NO_ERROR;

	if (mode < eDAL_ADAT_SMUX_COUNT)
	{
		adatMode = mode;
		adatUserSel = usersel;
		return hResult;
	}
	hResult = E_DAL_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}




