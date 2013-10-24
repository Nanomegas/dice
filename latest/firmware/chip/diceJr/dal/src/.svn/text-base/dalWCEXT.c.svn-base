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
	
	Purpose:	Dice Audio Application Layer (DAL)- WC and EXT wrapper
				This module interfaces the DAL to the WC and EXT 
	
	Revisions:
		created 09/03/2005 ML

****************************************************************************/

#include <string.h>
#include "TCTypes.h"
#include "TCDebug.h"
#include "TCTasking.h"
#include "ErrorCodes.h"
#include "dalWCEXT.h"
#include "dalRates.h"

static char 	moduleName[] = __FILE__ ;

static DAL_WCIN_ENUM wcInMode = eDAL_WCIN_ACTUAL_RATE;
static DAL_WCIN_ENUM extInMode = eDAL_WCIN_ACTUAL_RATE;

// Obtain interface specific PLL settings and rate mode estimate
// The WC carrier can either be base rate or actual rate
static void dalPllSettings (DAL_RX_PLL_DATA * pPllData, DAL_WCIN_ENUM mode)
{
	uint32 refHz = pPllData->ref8Hz;
	RATE_DESC * pRate;
	uint8	shift=0;
	
	if (mode == eDAL_WCIN_BASE_RATE)
	{
		if (pPllData->rateMode == eDAL_RATE_MODE_MID)
			shift = 1;
		else if (pPllData->rateMode == eDAL_RATE_MODE_HIGH)
			shift = 2;
	} 
	else if (mode == eDAL_WCIN_2XBASE_RATE)
	{
		if (pPllData->rateMode == eDAL_RATE_MODE_HIGH)
			shift = 1; //so a 96kHz would be seen as 192k
	}
	
	refHz <<= shift;
	// convert to Hz
	refHz >>= 3; // divide by 8
	//now find the nearest ID in the table
	pRate = dalRateFindNearest (&pPllData->rateID, refHz);
	
	pPllData->rateHZ = refHz;
	//if we didn't find any match, return (rateID is set to none)
	if (!pRate) return;
	
	if (mode == eDAL_WCIN_ACTUAL_RATE)
	{
		if (pRate->rateMode == eDAL_RATE_MODE_LOW)
			pPllData->preDiv = 1;
		else if (pRate->rateMode == eDAL_RATE_MODE_MID)
			pPllData->preDiv = 2;
		else
			pPllData->preDiv = 4;
	} 
	else if (mode == eDAL_WCIN_2XBASE_RATE)
	{
		if (pPllData->rateMode == eDAL_RATE_MODE_HIGH)
			pPllData->preDiv = 2;
		else
		  pPllData->preDiv = 1;
	}	
	else
	{
		pPllData->preDiv = 1;
	}
	//no additional feedback divider needed
	pPllData->addDiv = 1;	
}

void dalWCPllSettings (uint32 data, DAL_RX_PLL_DATA * pPllData)
{
	dalPllSettings (pPllData, wcInMode);
}

void dalExtPllSettings (uint32 data, DAL_RX_PLL_DATA * pPllData)
{
	dalPllSettings (pPllData, extInMode);
}


HRESULT dalSetWcInMode (DAL_WCIN_ENUM mode)
{
	HRESULT hResult = NO_ERROR;

	if (mode < eDAL_WCIN_COUNT)
	{
		wcInMode = mode;
		return hResult;
	}
	hResult = E_DAL_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

HRESULT dalSetExtInMode (DAL_WCIN_ENUM mode)
{
	HRESULT hResult = NO_ERROR;

	if (mode < eDAL_WCIN_COUNT)
	{
		extInMode = mode;
		return hResult;
	}
	hResult = E_DAL_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}





