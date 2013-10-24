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
		created 12/09/2002 lm

*****************************************************************************/

#ifndef _MPMSTATISTICS_H
#define _MPMSTATISTICS_H

#ifdef _STATISTICS

#include "TCTypes.h"
#include "mpm.h"

typedef struct
{
	uint32			blockItems[MPM_POOL_ITEMS];
	uint32			blockSize[MPM_POOL_ITEMS];
	uint32			inuse[MPM_POOL_ITEMS];
	uint32			used[MPM_POOL_ITEMS];
	uint32			inuseMax[MPM_POOL_ITEMS];
} MPM_STATISTICS;

HRESULT				mpmStatisticsInitialize (void);

extern MPM_STATISTICS		mpmStatistics;
#endif //_STATISTICS
#endif
