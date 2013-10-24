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

****************************************************************************/

#ifndef _MPMCLI_H
#define _MPMCLI_H
#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_MPM

#include "TCTypes.h"
#include "mpm.h"
#include "mpmStatistics.h"

typedef enum
{
	DUMP_ALL_SUMMARY = 1,
	DUMP_AVAIL_SUMMARY = 2,
	DUMP_INUSE_SUMMARY = 3,
	DUMP_ALL_PB = 4,
	DUMP_AVAIL_PB = 5,
	DUMP_INUSE_PB = 6
} DUMP_TYPE ;

HRESULT			mpmCliStatisticsShow(void);
#ifdef _STATISTICS
HRESULT			mpmCliStatisticsDisplay(MPM_STATISTICS *statistics);
#endif //_STATISTICS

HRESULT			mpmCliDumpByIndex(uint32 blockSize, uint32 blockIndex);
HRESULT			mpmCliDump(DUMP_TYPE dumpType) ;
uint32			mpmCliDumpPool(MPM_POOL_DESC *poolDescPtr, DUMP_TYPE dumpType);

void			mpmCliInstallTools(void);

#endif //_CLI_TOOL_MPM
#endif //_CLI
#endif //_MPMCLI_H
