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
	
  DESCRIPTION:

	Purpose:	API to dynamic memory allocation CLI
	
				Command Line Interface

	Revisions:
		created 28/05/2002 lm

****************************************************************************/

#ifdef _STATISTICS

#include "TCTasking.h"
#include "packetBlock.h"
#include "mpm.h"
#include "mpmStatistics.h"

MPM_STATISTICS		mpmStatistics;

/*****************************************************************************

	Statistics

*****************************************************************************/

HRESULT mpmStatisticsInitialize (void) 
{
	HRESULT				hResult = NO_ERROR;
	uint32				indexPool = 0;

	for (indexPool = 0; indexPool < MPM_POOL_ITEMS; indexPool++)
	{
		mpmStatistics.blockSize[indexPool] = mpmPoolDesc[indexPool].blockSize;
		mpmStatistics.blockItems[indexPool] = mpmPoolDesc[indexPool].blockItems;
		mpmStatistics.inuse[indexPool] = 0;
		mpmStatistics.used[indexPool] = 0;
		mpmStatistics.inuseMax[indexPool] = 0;
	}

	return hResult;
}

#endif //_STATISTICS
