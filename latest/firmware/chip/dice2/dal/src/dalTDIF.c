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
	
	Purpose:	Dice Audio Application Layer (DAL)- TDIF wrapper
				This module interfaces the DAL to the DICE TDIF
	
	Revisions:
		created 09/03/2005 ML

****************************************************************************/

#include <string.h>
#include "TCTypes.h"
#include "TCDebug.h"
#include "TCTasking.h"
#include "ErrorCodes.h"
#include "dice.h"
#include "diceTDIF.h"
#include "dalTDIF.h"
#include "dalRates.h"

//static char 	moduleName[] = __FILE__ ;


// Obtain interface specific PLL settings and rate mode estimate
// This implementation allows the interface to run with a clock
// up to 96Khz. At 192KHz it assumes a 96KHz clock and "SMUX"
void dalTDIFPllSettings (uint32 date, DAL_RX_PLL_DATA * pPllData)
{
	uint32 refHz = pPllData->ref8Hz;
	RATE_DESC * pRate;
	
	
	if (pPllData->rateMode == eDAL_RATE_MODE_HIGH)
		refHz <<= 1; //in high mode system runs half speed
	// convert to Hz
	refHz >>= 3; // divide by 8
	//now find the nearest ID in the table
	pRate = dalRateFindNearest (&pPllData->rateID, refHz);
	
	pPllData->rateHZ = refHz;
	//if we didn't find any match, return (rateID is set to none)
	if (!pRate) return;
	
	//calculate the predivider to make the reference base rate
	if (pRate->rateMode == eDAL_RATE_MODE_LOW)
		pPllData->preDiv = 1;
	else
		pPllData->preDiv = 2;
	
	//no additional feedback divider needed
	pPllData->addDiv = 1;	
}




// Is the TDIF interface locked
uint8 dalTDIFIsLocked (uint32 data)
{
	return (uint8)tdifIsLocked ();
}


