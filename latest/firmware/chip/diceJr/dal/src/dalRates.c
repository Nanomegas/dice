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
	
	Purpose:	Dice Audio Application Layer (DAL)- Clock rate Interface
  
  Description:       
  
	Revisions:
		created 09/03/2005 ML

****************************************************************************/

#include <string.h>
#include <stdlib.h>
#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCDebug.h"
#include "dal.h"
#include "dalRates.h"

static char 	moduleName[] = __FILE__ ;

#define INITIALIZED_RATES (eDAL_NOMINAL_RATE_USER_4+1)
	
typedef struct
{
	RATE_DESC	rates[eDAL_NOMINAL_RATE_ANY];
	RATE_DESC *	sorted[eDAL_NOMINAL_RATE_ANY];
	uint8		nbElements;
} DAL_RATES;

static DAL_RATES	dalRates;

static int compareRates (const void * e1, const void * e2)
{
	uint32 v1 = (*((RATE_DESC **)e1))->rate;
	uint32 v2 = (*((RATE_DESC **)e2))->rate;
	
	if (v1 < v2)
		return -1;
	else if (v1 > v2)
		return 1;
	else
		return 0;
} 

static void sortRates (void)
{
	uint8 i;
	
	dalRates.nbElements = 0;
	//don't include user rates in detection, only for internal rate use.	
	for (i = 0; i < eDAL_NOMINAL_RATE_USER_1; i++) 
	{
		if (dalRates.rates[i].sysNominalID != eDAL_NOMINAL_RATE_NONE)
			dalRates.sorted[dalRates.nbElements++] = &dalRates.rates[i];
	}
	
	qsort (dalRates.sorted, dalRates.nbElements, sizeof(RATE_DESC *), compareRates);
	
	for (i = 0; i < dalRates.nbElements; i++) 
	{
		dalRates.sorted[i]->order = i;		
	}
}

RATE_DESC * dalRateFindNearest (DAL_NOMINAL_RATE_ENUM * pRateID, uint32 rate)
{
	uint8 idx,ilo,ihi,rateID;

	//do a binary search	
	ilo = 0; ihi = dalRates.nbElements-1; //both inclusive
	idx = (ilo+ihi)>>1; // start in the middle
	while ((ihi - ilo) > 1)
	{
		if (rate >= dalRates.sorted[idx]->rate) 
			ilo = idx; //no need to search for lower rates
		if (rate <= dalRates.sorted[idx]->rate) 
			ihi = idx; //no need to search for higher rates
		idx = (ilo + ihi) >> 1;	
	}
	// if exact match ihi==iho==idx
	// if not exact match the closest one is either ihi or ilo
	if (rate < dalRates.sorted[ilo]->rate)
		rateID = dalRates.sorted[ilo]->ID;
	else if (rate > dalRates.sorted[ihi]->rate)
		rateID = dalRates.sorted[ihi]->ID;
	else if ((rate - dalRates.sorted[ilo]->rate) < (dalRates.sorted[ihi]->rate - rate))
		rateID = dalRates.sorted[ilo]->ID;
	else
		rateID = dalRates.sorted[ihi]->ID;
	// unfortunately this might not be within the range of that rate
	if ((rate >= dalRates.rates[rateID].min) && (rate <= dalRates.rates[rateID].max))
	{
		*pRateID = rateID;
		return &dalRates.rates[rateID];
	}
	rateID = dalRates.rates[rateID].sysNominalID; // try the nominal ID
	if ((rate >= dalRates.rates[rateID].min) && (rate <= dalRates.rates[rateID].max))
	{
		*pRateID = rateID;
		return &dalRates.rates[rateID];
	}
	// we are out of options
	*pRateID = eDAL_NOMINAL_RATE_NONE;
	return NULL;
}
	
RATE_DESC * dalRateByID        (DAL_NOMINAL_RATE_ENUM rateID)
{
	if ((rateID >= eDAL_NOMINAL_RATE_ANY) || (dalRates.rates[rateID].sysNominalID == eDAL_NOMINAL_RATE_NONE))
	{
		sysLogError(E_DAL_ILLEGAL_RATE, __LINE__, moduleName);		
		return NULL;
	}
	return &dalRates.rates[rateID];
}

uint8 dalRateModeCompatible (DAL_RATE_MODE_ENUM rateMode1, DAL_RATE_MODE_ENUM rateMode2)
{		
	if (rateMode1 == rateMode2) return TRUE;
	if ((rateMode1 == eDAL_RATE_MODE_ALL) || (rateMode2 == eDAL_RATE_MODE_ALL)) return TRUE;
	if ((rateMode1 == eDAL_RATE_MODE_LOW_MID) && ((rateMode2 == eDAL_RATE_MODE_LOW) || (rateMode2 == eDAL_RATE_MODE_MID))) return TRUE;
	if ((rateMode2 == eDAL_RATE_MODE_LOW_MID) && ((rateMode1 == eDAL_RATE_MODE_LOW) || (rateMode1 == eDAL_RATE_MODE_MID))) return TRUE;
	return FALSE;
}

DAL_NOMINAL_RATE_ENUM dalRatesGetSystemNominal (DAL_NOMINAL_RATE_ENUM rateID)
{
	if (rateID >= eDAL_NOMINAL_RATE_ANY) return eDAL_NOMINAL_RATE_NONE;
	return dalRates.rates[rateID].sysNominalID;
}

	

RATE_DESC * dalRateGetDefaultRate (DAL_RATE_MODE_ENUM rateMode, DAL_NOMINAL_RATE_ENUM rateID)
{
	if ((rateID < eDAL_NOMINAL_RATE_ANY) && (dalRates.rates[rateID].sysNominalID != eDAL_NOMINAL_RATE_NONE))
	{
		if (dalRateModeCompatible (dalRates.rates[rateID].rateMode, rateMode))
		{
			return &dalRates.rates[rateID];
		}
	}
	switch (rateMode)
	{
		default:
		case eDAL_RATE_MODE_LOW:
		case eDAL_RATE_MODE_LOW_MID:
			return &dalRates.rates[eDAL_NOMINAL_RATE_48];		
		case eDAL_RATE_MODE_MID:
			return &dalRates.rates[eDAL_NOMINAL_RATE_96];
		case eDAL_RATE_MODE_HIGH:
			return &dalRates.rates[eDAL_NOMINAL_RATE_192];
		
	}
}
		
HRESULT dalRateInstallUserRate (uint8 UserID, const RATE_DESC * pRateDesc)
{
	HRESULT hResult = NO_ERROR;

	if ((UserID < eDAL_NOMINAL_RATE_USER_1) || (UserID > eDAL_NOMINAL_RATE_USER_8))
	{
		hResult = E_DAL_BAD_INPUT_PARAM;
		goto errorReturn;
	}
	if (!pRateDesc)
	{
		//uninstall
		dalRates.rates[UserID].sysNominalID = eDAL_NOMINAL_RATE_NONE;
	}
	else
	{
		// check the integrety of the rate
		hResult = E_DAL_BAD_INPUT_PARAM;
		if (pRateDesc->min > pRateDesc->rate) goto errorReturn;
		if (pRateDesc->max < pRateDesc->rate) goto errorReturn;
		if ((pRateDesc->rateMode != eDAL_RATE_MODE_LOW) &&
			(pRateDesc->rateMode != eDAL_RATE_MODE_MID) &&
			(pRateDesc->rateMode != eDAL_RATE_MODE_HIGH)) goto errorReturn;
		if (pRateDesc->sysNominalID > eDAL_NOMINAL_RATE_192) goto errorReturn;
		
		// install rate
		hResult = NO_ERROR;
		dalRates.rates[UserID] = *pRateDesc;
		dalRates.rates[UserID].ID = UserID;
		switch (dalRates.rates[UserID].sysNominalID)
		{
			case eDAL_NOMINAL_RATE_32:
				dalRates.rates[UserID].baseID = eDAL_NOMINAL_RATE_32;
				break;
			case eDAL_NOMINAL_RATE_44_1:		    
			case eDAL_NOMINAL_RATE_88_2:
			case eDAL_NOMINAL_RATE_176_4:
				dalRates.rates[UserID].baseID = eDAL_NOMINAL_RATE_44_1;
				break;
			default:
			case eDAL_NOMINAL_RATE_48:
			case eDAL_NOMINAL_RATE_96:
			case eDAL_NOMINAL_RATE_192:
				dalRates.rates[UserID].baseID = eDAL_NOMINAL_RATE_48;
				break;
		}
    }
    sortRates ();		
	return hResult;
	
errorReturn:	
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

static const RATE_DESC initialRates[INITIALIZED_RATES] =
{
   //min     max     rate    d   n     rateMode             sysNominalID             ID                        baseID                  
#ifndef _SYSRUNS50MHZ
/* ML: Test with PLLE high, these are the original values */
	{30000,  34000,  32000,  1536,  1, eDAL_RATE_MODE_LOW,  eDAL_NOMINAL_RATE_32,    eDAL_NOMINAL_RATE_32,     eDAL_NOMINAL_RATE_32,   0},
	{42000,  46000,  44100, 10031,  9, eDAL_RATE_MODE_LOW,  eDAL_NOMINAL_RATE_44_1,  eDAL_NOMINAL_RATE_44_1,   eDAL_NOMINAL_RATE_44_1, 0},
	{46000,  50000,  48000,  1024,  1, eDAL_RATE_MODE_LOW,  eDAL_NOMINAL_RATE_48,	 eDAL_NOMINAL_RATE_48,     eDAL_NOMINAL_RATE_48,   0},
	{86000,  90000,  88200, 10031,  9, eDAL_RATE_MODE_MID,  eDAL_NOMINAL_RATE_88_2,  eDAL_NOMINAL_RATE_88_2,   eDAL_NOMINAL_RATE_44_1, 0},
	{94000,  98000,  96000,  1024,  1, eDAL_RATE_MODE_MID,  eDAL_NOMINAL_RATE_96,	 eDAL_NOMINAL_RATE_96,     eDAL_NOMINAL_RATE_48,   0},
	{174000,178000, 176400, 10031,  9, eDAL_RATE_MODE_HIGH, eDAL_NOMINAL_RATE_176_4, eDAL_NOMINAL_RATE_176_4,  eDAL_NOMINAL_RATE_44_1, 0},
	{190000,194000, 192000,  1024,  1, eDAL_RATE_MODE_HIGH, eDAL_NOMINAL_RATE_192,   eDAL_NOMINAL_RATE_192,    eDAL_NOMINAL_RATE_48,   0},
#else
	{30000,  34000,  32000,  3125,  2, eDAL_RATE_MODE_LOW,  eDAL_NOMINAL_RATE_32,    eDAL_NOMINAL_RATE_32,     eDAL_NOMINAL_RATE_32,   0},
	{42000,  46000,  44100, 10204,  9, eDAL_RATE_MODE_LOW,  eDAL_NOMINAL_RATE_44_1,  eDAL_NOMINAL_RATE_44_1,   eDAL_NOMINAL_RATE_44_1, 0},
	{46000,  50000,  48000,  6250,  6, eDAL_RATE_MODE_LOW,  eDAL_NOMINAL_RATE_48,	 eDAL_NOMINAL_RATE_48,     eDAL_NOMINAL_RATE_48,   0},
	{86000,  90000,  88200, 10204,  9, eDAL_RATE_MODE_MID,  eDAL_NOMINAL_RATE_88_2,  eDAL_NOMINAL_RATE_88_2,   eDAL_NOMINAL_RATE_44_1, 0},
	{94000,  98000,  96000,  6250,  6, eDAL_RATE_MODE_MID,  eDAL_NOMINAL_RATE_96,	 eDAL_NOMINAL_RATE_96,     eDAL_NOMINAL_RATE_48,   0},
	{174000,178000, 176400, 10204,  9, eDAL_RATE_MODE_HIGH, eDAL_NOMINAL_RATE_176_4, eDAL_NOMINAL_RATE_176_4,  eDAL_NOMINAL_RATE_44_1, 0},
	{190000,194000, 192000,  6250,  6, eDAL_RATE_MODE_HIGH, eDAL_NOMINAL_RATE_192,   eDAL_NOMINAL_RATE_192,    eDAL_NOMINAL_RATE_48,   0},
#endif
	// user rates defaults
	// 44.1 pull-down, USER_1
	{42000,  46000,  44056, 23429, 21, eDAL_RATE_MODE_LOW,  eDAL_NOMINAL_RATE_44_1,  eDAL_NOMINAL_RATE_USER_1, eDAL_NOMINAL_RATE_44_1, 0},
	// 48  pull-down, USER_2
	{46000,  50000,  47952, 41001, 40, eDAL_RATE_MODE_LOW,  eDAL_NOMINAL_RATE_48,	 eDAL_NOMINAL_RATE_USER_2, eDAL_NOMINAL_RATE_48,   0},
	// 88.2 pull-down, USER_3
	{86000,  90000,  88112, 23429, 21, eDAL_RATE_MODE_MID,  eDAL_NOMINAL_RATE_88_2,  eDAL_NOMINAL_RATE_USER_3, eDAL_NOMINAL_RATE_44_1, 0},
	// 96 pull-down, USER_4
	{94000,  98000,  95904, 41001, 40, eDAL_RATE_MODE_MID,  eDAL_NOMINAL_RATE_96,	 eDAL_NOMINAL_RATE_USER_4, eDAL_NOMINAL_RATE_48,   0}	
};

HRESULT dalRateSkewRates (int32 tenthPercent)
{
	HRESULT hResult = NO_ERROR;
	int i;
	
	if (tenthPercent>100) tenthPercent=100;
	if (tenthPercent<-100) tenthPercent=-100;

	for (i=0; i<INITIALIZED_RATES; i++)
	{
		dalRates.rates[i].denom = (initialRates[i].denom* 1000)/(1000+tenthPercent);
	}
	return hResult;
}


HRESULT dalRateInitialize (void)
{
	HRESULT hResult = NO_ERROR;
	int i;
	
	for (i=0; i<INITIALIZED_RATES; i++)
	{
		dalRates.rates[i] = initialRates[i];
	}
	for (i=INITIALIZED_RATES; i<eDAL_NOMINAL_RATE_ANY; i++)
	{
		dalRates.rates[i].sysNominalID = eDAL_NOMINAL_RATE_NONE;
	}
	sortRates ();	
	return hResult;
}
