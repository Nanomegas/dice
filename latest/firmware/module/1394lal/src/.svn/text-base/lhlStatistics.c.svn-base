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
 * DESCRIPTION: Link Hardware Abstraction Layer Statistics

	Purpose:	Link Hardware Abstraction Layer
						handling statistics
	
	Revisions:
		created 04/03/2002 lm

****************************************************************************/

#include "TCTasking.h"
#include "lhlStatistics.h"

#ifdef _STATISTICS
// runtime lhl statistics
LHL_STATISTICS			lhlStatistics;

/*********************************************************
	Initializes this module (messageQueues, AR/AT Tasks (and resources)
*/
HRESULT lhlStatisticsInitialize(void)
{
	HRESULT hResult = NO_ERROR;

	lhlStatisticsReset();

	return hResult;
}

/***************************************************************************
 ***************************************************************************

	Statistics functions

 ***************************************************************************
****************************************************************************/

HRESULT lhlStatisticsReset(void)
{
	HRESULT hResult = NO_ERROR;

	// initialize member variables
	memset(&lhlStatistics, 0, sizeof(lhlStatistics));
	lhlStatistics.validItems = STATISTICS_VALID_ITEMS;

	return hResult;
}

HRESULT lhlStatisticsResetSpecial(uint32 resetMode)
{
	HRESULT hResult = NO_ERROR;

	uint32	RxReqInt = 0;
	uint32	RxReqCount = 0;

	if (resetMode & LHL_STATISTICS_RESET_SAVE_REQ)
	{
		RxReqInt = 1;
		RxReqCount = 1;
	}

	if (resetMode & LHL_STATISTICS_RESET_ALL)
	{
		hResult = lhlStatisticsReset();
	}
	else
	{
		if (resetMode & LHL_STATISTICS_RESET_COUNT)
		{
			lhlStatistics.RxReqInt = 0;
			lhlStatistics.RxRspInt = 0;
			lhlStatistics.TxReqInt = 0;
			lhlStatistics.TxRspInt = 0;

			lhlStatistics.RxReqCount = 0;
			lhlStatistics.RxRspCount = 0;
			lhlStatistics.TxReqCount = 0;
			lhlStatistics.TxRspCount = 0;
			
			lhlStatistics.TxReqTimeout = 0;
			lhlStatistics.TxRspTimeout = 0;

#ifdef _LOOSE_ISO
			lhlStatistics.TxStmCount = 0;
			lhlStatistics.RxStmCount = 0;
#endif //_LOOSE_ISO

		}

		if (resetMode & LHL_STATISTICS_RESET_BRI)
		{
			lhlStatistics.BrBriBRInt = 0;
			lhlStatistics.BrBriSIInt = 0;
		}

		if (resetMode & LHL_STATISTICS_RESET_ERRORS)
		{
			lhlStatistics.PendTxReqMatchErr = 0;
			lhlStatistics.PendTxRspMatchErr = 0;
			lhlStatistics.PendTxReqAgeout = 0;
			lhlStatistics.PendTxRspAgeout = 0;
#ifdef _LOOSE_ISO
			lhlStatistics.PendTxStmMatchErr = 0;
			lhlStatistics.PendTxStmAgeout = 0;
#endif //_LOOSE_ISO
			lhlStatistics.sysLogErr = 0;
			lhlStatistics.sysLogISR = 0;
		}

		if (resetMode & LHL_STATISTICS_RESET_SAVE_REQ)
		{
			lhlStatistics.RxReqInt = RxReqInt;
			lhlStatistics.RxReqCount = RxReqCount;
		}
	}

	return hResult;
}

#endif //_STATISTICS
