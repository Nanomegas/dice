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
//===========================================================================
/****************************************************************************
	
	Purpose:	Dice Audio Application Layer (DAL)- Clock rate Interface
  
  Description:       
  
	Revisions:
		created 09/03/2005 ML

****************************************************************************/
/** @addtogroup DAL

	@{
*/

#ifndef _DALRATE_H
#define _DALRATE_H

#include "dal.h"
#include "dalClkCtrl.h"

typedef struct
{
	uint32				min;			//!<  minimum rate accepted
	uint32				max;			//!<  maximum rate accepted
	uint32				rate;			//!<  actual rate
	uint32				denom;			//!<  For internal generation, 
	uint32				nom;			//!<  baseRate = 49.152MHz * nom / denom
	DAL_RATE_MODE_ENUM	rateMode;		//!<  LOW, MID or HIGH
	uint8				sysNominalID;	//!<  the nominal ID used for interface meta data (NONE if rate not installed)
	uint8				ID;				//!<  the ID of this rate	(assigned by system)
	uint8				baseID;			//!<  the ID of the base rate	(assigned by system)
	uint8				order;			//!<  assigned by system
} RATE_DESC;

/** Initialize the Rate Module
*/
HRESULT dalRateInitialize (void);

/** User Rate
// if pRateDesc == NULL the rate will be removed
*/
HRESULT dalRateInstallUserRate (uint8 UserID, const RATE_DESC * pRateDesc);

/** This function returns a pointer to the nearest rate type matching
//  the specified rate in HZ. It will start the search at the initial
//	value of *pRateID. If no match is found a NULL pointer is returned.
*/
RATE_DESC * dalRateFindNearest (DAL_NOMINAL_RATE_ENUM * pRateID, uint32 rate);

/** This function returns the rate descriptor for the specified
//	rate ID. If the rateID is >= eDAL_NOMINAL_RATE_ANY or an 
//  undefined user rate it will return a NULL pointer.
*/
RATE_DESC * dalRateByID        (DAL_NOMINAL_RATE_ENUM rateID);


/** Return the rate descriptor for the specified rateID if it is legal
//	othervise it returns the default rate descriptor for that rate mode
*/
RATE_DESC * dalRateGetDefaultRate (DAL_RATE_MODE_ENUM rateMode, DAL_NOMINAL_RATE_ENUM rateID);

/** Get System Nominal Rate
*/
DAL_NOMINAL_RATE_ENUM dalRatesGetSystemNominal (DAL_NOMINAL_RATE_ENUM rateID);

/** Determine compatible rate mode
*/
uint8 dalRateModeCompatible (DAL_RATE_MODE_ENUM rateMode1, DAL_RATE_MODE_ENUM rateMode2);

/** Skew the rates in 10th of percent, max +/10% (+/-100)
*/
HRESULT dalRateSkewRates (int32 tenthPercent);


//! @}

#endif // _DALRATE_H

