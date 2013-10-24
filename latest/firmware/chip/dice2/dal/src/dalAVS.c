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
	
	Purpose:	Dice Audio Application Layer (DAL)- AVS wrapper
				This module interfaces the DAL to the DICE AVS
	
	Revisions:
		created 09/03/2005 ML

****************************************************************************/

#include <string.h>
#include "TCTypes.h"
#include "TCDebug.h"
#include "TCTasking.h"
#include "ErrorCodes.h"
#include "avs.h"
#include "dalAVS.h"
#include "dalRates.h"

//static char 	moduleName[] = __FILE__ ;



// Obtain interface specific PLL settings and rate mode estimate
// The AVS carrier will always run at base rate / 8.
void dalAVSPllSettings (uint32 plugID, DAL_RX_PLL_DATA * pPllData)
{
#ifdef _AVS
	uint32 refHz = pPllData->ref8Hz;
	RATE_DESC * pRate;
	uint8	shift=0;
	
	
	if (pPllData->rateMode == eDAL_RATE_MODE_MID)
		shift = 1;
	else if (pPllData->rateMode == eDAL_RATE_MODE_HIGH)
		shift = 2;
	else if (pPllData->rateMode == eDAL_RATE_MODE_LOW_MID)
	{
		uint32 sfc;
		
		sfc = avsAudioRxGetReceivedSFC (plugID);
		if (sfc > AVS_SFC_96_KHZ)
			shift = 2;
		else if (sfc > AVS_SFC_48_KHZ)
			shift = 1;
	}
	refHz <<= shift;
	
	// convert to Hz
	// no need to do anything, should divide by 8 to get Hz	and
	// multiply by 8 to get base rate.

	//now find the nearest ID in the table
	pRate = dalRateFindNearest (&pPllData->rateID, refHz);
	
	pPllData->rateHZ = refHz;
	//if we didn't find any match, return (rateID is set to none)
	if (!pRate) return;
	
	//AVS reference is always base rate / 8
	pPllData->preDiv = 1;
	pPllData->addDiv = 8;
#endif
}




// Is the AVS interface locked
uint8 dalAVSIsLocked (uint32 plugID)
{
	uint8 ret = 0;
#ifdef _AVS
	ret = avsRxIsLocked (plugID);
#endif
	return ret;
}


void dalAVSTxMute (uint32 plugID, BOOL bMute)
{
#ifdef _AVS
	avsTxMute(plugID, bMute);
#endif
}

// assume that AVS rate enumeration matches DAL_NOMINAL_RATE_ENUM
void dalAVSTxSetNominalRate (uint32 plugID, DAL_NOMINAL_RATE_ENUM rate)
{
#ifdef _AVS
	if(rate > eDAL_NOMINAL_RATE_96)//New 192K Implementation fix
		rate = rate - 2;
	avsSetTxSampleRate(plugID, rate);
#endif
}

void dalAVSRxSetNominalRate (uint32 plugID, DAL_NOMINAL_RATE_ENUM rate)
{
#ifdef _AVS
	if(rate > eDAL_NOMINAL_RATE_96)//New 192K Implementation fix
		rate = rate - 2;
	avsSetRxSampleRate(plugID, rate);
#endif
}

void dalAVSTxEnable (uint32 plugID, BOOL bEnable)
{
/*
	bDalTxStarted[plugID] = bEnable;
	if (bUserTxStarted[plugID] && bDalTxStarted[NUM_AVS_TX])
	{
		if (!avsIsTxStarted(plugID)) avsStartTx(plugID);
	}
	else
	{
		if (avsIsTxStarted(plugID)) avsStopTx(plugID);
	}
*/
}

void dalAVSRxEnable (uint32 plugID, BOOL bEnable)
{
/*
	bDalRxStarted[plugID] = bEnable;
	if (bUserRxStarted[plugID] && bDalRxStarted[NUM_AVS_TX])
	{
		if (!avsIsRxStarted(plugID)) avsStartRx(plugID);
	}
	else
	{
		if (avsIsRxStarted(plugID)) avsStopRx(plugID);
	}
*/
}
