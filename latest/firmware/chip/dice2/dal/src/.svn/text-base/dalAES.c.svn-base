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
	
	Purpose:	Dice Audio Application Layer (DAL)- AES wrapper
				This module interfaces the DAL to the DICE AES
	
	Revisions:
		created 09/03/2005 ML

****************************************************************************/

#include <string.h>
#include "TCTypes.h"
#include "TCDebug.h"
#include "TCTasking.h"
#include "ErrorCodes.h"
#include "dice.h"
#include "diceAES.h"
#include "dalAES.h"
#include "dalRates.h"

static char 	moduleName[] = __FILE__ ;

static uint8 defaultSync = AES_ANY;


// Obtain interface specific PLL settings and rate mode estimate
void dalAESPllSettings (uint32 aesID, DAL_RX_PLL_DATA * pPllData)
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




// Is the AES interface locked
uint8 dalAESIsLocked (uint32 aesID)
{
	return (uint8)aesIsMasterLocked ();
}



// When the AES is not clock master call this function to set the default local master
void dalAESSetMasterClkDefault(void)
{
	aesSetMasterClk(defaultSync);
}


// When the AES is clock master select which sub-interface is selected
void dalAESSetMasterClk (uint32 aesID)
{
	aesSetMasterClk(aesID);
}


/*********** Functions which can be used by the application ****************/

// When AES is not selected as clock master it will need to know which of the 4 interfaces to receive sync from
// default is AES_ANY.
HRESULT dalAESDefaultSyncMode (uint8 syncFrom)
{
	HRESULT hResult = NO_ERROR;

	if (syncFrom <= NUM_AES)
	{
		defaultSync = syncFrom;
		return hResult;
	}
	hResult = E_DAL_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

