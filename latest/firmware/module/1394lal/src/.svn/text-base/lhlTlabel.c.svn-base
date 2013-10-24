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
 * DESCRIPTION: Link Hardware Abstraction Layer Transaction Label 

	Purpose:	Link Hardware Abstraction Layer 
						handling of transaction labels
	
	Revisions:
		created 28/08/2002 lm

****************************************************************************/

#include "TCTasking.h"
#include "lhl.h"
#include "lhlCli.h"
#include "lhlStatistics.h"
#include "lhlTLabel.h"

static char 		moduleName[] = __FILE__ ;

static uint32 lhlTLabelMutexSemID; // mutual exclusion semaphore for lhlTLabel

static TLABEL_POOL lhlTLabels;
#ifdef _LOOSE_ISO
static TLABEL_POOL lhlTLabelsAsyncStream;
#endif //_LOOSE_ISO

/*********************************************************
	Initializes this module
*/
HRESULT lhlTLabelInitialize(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = lhlTLabelCreateSemaphores();
	if (hResult != NO_ERROR) return hResult;

	lhlTLabelInitializeTLabelPool();

	return hResult;
}

/*********************************************************
	Create semaphores for this module
*/
HRESULT lhlTLabelCreateSemaphores(void)
{
	HRESULT hResult = NO_ERROR;

	// exclusive access for the lhlTLabel (mutex)
	hResult = TCMutexOpen(&lhlTLabelMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT lhlTLabelInitializeTLabelPool(void)
{
	HRESULT hResult = NO_ERROR;
	uint32 index;

	for (index = 0; index < TLABEL_VALUE_ITEMS; index++)
	{
		if (index >= TLABEL_VALUE_MIN &&
			index <= TLABEL_VALUE_MAX)
		{
			lhlTLabels.allocated[index] = FALSE;
		}
		else
		{
			lhlTLabels.allocated[index] = TRUE;
		}
	}
	lhlTLabels.ptr = TLABEL_VALUE_MIN;

#ifdef _LOOSE_ISO
	for (index = 0; index < TLABEL_VALUE_ITEMS; index++)
	{
		lhlTLabelsAsyncStream.allocated[index] = FALSE;
	}
#endif //_LOOSE_ISO

#ifdef _STATISTICS
	lhlStatistics.TLabelsReqInuse = 0;
	lhlStatistics.TLabelsReqInuseMax = 0;
	lhlStatistics.TLabelsStmInuse = 0;
	lhlStatistics.TLabelsStmInuseMax = 0;
#endif //_STATISTICS

	return hResult;
}

HRESULT lhlTLabelGetShifted(uint32 *tLabel)
{
	HRESULT	hResult = NO_ERROR;

	hResult = lhlTLabelGet(tLabel);
	*tLabel = *tLabel << SHIFT_TLABEL;

	return hResult;
}

HRESULT lhlTLabelGet(uint32 *tLabel)
{
	HRESULT	hResult = NO_ERROR;
	uint32	index;
	uint32	ptr;
	BOOL	bFound = FALSE;
	
	*tLabel = 0;

	// exclusive access for the lhlTLabel (mutex)
	hResult = TCMutexLock(lhlTLabelMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	ptr = lhlTLabels.ptr;
	for (index = 0; index < TLABEL_VALUE_ITEMS; index++)
	{
		if (lhlTLabels.allocated[ptr] == FALSE)
		{
			lhlTLabels.allocated[ptr] = TRUE;
			*tLabel = ptr;
			bFound = TRUE;

#ifdef _STATISTICS
			lhlStatistics.TLabelsReqInuse++;
			if (lhlStatistics.TLabelsReqInuseMax < lhlStatistics.TLabelsReqInuse)
			{
				lhlStatistics.TLabelsReqInuseMax = lhlStatistics.TLabelsReqInuse;
			}
#endif //_STATISTICS
		}
		ptr++;
		if (ptr > TLABEL_VALUE_MAX)
		{
			ptr = TLABEL_VALUE_MIN;
		}
		if (bFound == TRUE)
		{
			lhlTLabels.ptr = ptr;
			break;
		}
	}

	if (bFound == FALSE)
	{
		SYS_DEBUG(SYSDEBUG_TRACE_WARNINGS, "lhl tlabel pool has no more free items\n\r");
		hResult = E_FAIL;
		sysLogError(hResult, __LINE__, moduleName);
	}

	// exclusive access for the lhlTLabel (mutex)
	TCMutexUnlock(lhlTLabelMutexSemID);

	return hResult;
}

HRESULT lhlTLabelPutShifted(uint32 tLabel)
{
	HRESULT	hResult = NO_ERROR;

	tLabel = tLabel >> SHIFT_TLABEL;
	hResult = lhlTLabelPut(tLabel);

	return hResult;
}

HRESULT lhlTLabelPut(uint32 tLabel)
{
	HRESULT	hResult = NO_ERROR;

	// exclusive access for the lhlTLabel (mutex)
	hResult = TCMutexLock(lhlTLabelMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	if (tLabel >= TLABEL_VALUE_MIN &&
		tLabel <= TLABEL_VALUE_MAX)
	{
		if (lhlTLabels.allocated[tLabel] == TRUE)
		{
			lhlTLabels.allocated[tLabel] = FALSE;
#ifdef _STATISTICS
			lhlStatistics.TLabelsReqInuse--;
#endif //_STATISTICS
		}
		else
		{
			SYS_DEBUG(SYSDEBUG_TRACE_WARNINGS, "lhl tlabel pool freeing item already free\n\r");
		}
	}
	else
	{
		hResult = E_LHL_TLABEL_MISMATCH;
		sysLogError(hResult, __LINE__, moduleName);
	}

	// exclusive access for the lhlTLabel (mutex)
	TCMutexUnlock(lhlTLabelMutexSemID);

	return hResult;
}

#ifdef _LOOSE_ISO
HRESULT lhlTLabelGetShiftedAsyncStream(uint8 channel, uint32 *tLabel)
{
	HRESULT	hResult = NO_ERROR;

	hResult = lhlTLabelGetAsyncStream(channel, tLabel);
	*tLabel = *tLabel << SHIFT_TLABEL;

	return hResult;
}

HRESULT lhlTLabelGetAsyncStream(uint8 channel, uint32 *tLabel)
{
	HRESULT	hResult = NO_ERROR;
	BOOL	bFound = FALSE;
	
	*tLabel = 0;

	// exclusive access for the lhlTLabel (mutex)
	hResult = TCMutexLock(lhlTLabelMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	if (/*channel >= TLABEL_VALUE_ASYNC_STREAM_MIN &&*/
		channel <= TLABEL_VALUE_ASYNC_STREAM_MAX)
	{
		if (lhlTLabelsAsyncStream.allocated[channel] == FALSE)
		{
			lhlTLabelsAsyncStream.allocated[channel] = TRUE;
			*tLabel = channel;
			bFound = TRUE;

#ifdef _STATISTICS
			lhlStatistics.TLabelsStmInuse++;
			if (lhlStatistics.TLabelsStmInuseMax < lhlStatistics.TLabelsStmInuse)
			{
				lhlStatistics.TLabelsStmInuseMax = lhlStatistics.TLabelsStmInuse;
			}
#endif //_STATISTICS
		}
	}

	if (bFound == FALSE)
	{
		SYS_DEBUG(SYSDEBUG_TRACE_WARNINGS, "lhl tlabel stream channel already in use\n\r");
		hResult = E_FAIL;
		sysLogError(hResult, __LINE__, moduleName);
	}

	// exclusive access for the lhlTLabel (mutex)
	TCMutexUnlock(lhlTLabelMutexSemID);

	return hResult;
}

HRESULT lhlTLabelPutShiftedAsyncStream(uint32 tLabel)
{
	HRESULT	hResult = NO_ERROR;

	tLabel = tLabel >> SHIFT_TLABEL;
	hResult = lhlTLabelPutAsyncStream(tLabel);

	return hResult;
}

HRESULT lhlTLabelPutAsyncStream(uint32 tLabel)
{
	HRESULT	hResult = NO_ERROR;

	// exclusive access for the lhlTLabel (mutex)
	hResult = TCMutexLock(lhlTLabelMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	if (/*tLabel >= TLABEL_VALUE_ASYNC_STREAM_MIN &&*/
		tLabel <= TLABEL_VALUE_ASYNC_STREAM_MAX)
	{
		if (lhlTLabelsAsyncStream.allocated[tLabel] == TRUE)
		{
			lhlTLabelsAsyncStream.allocated[tLabel] = FALSE;
#ifdef _STATISTICS
			lhlStatistics.TLabelsStmInuse--;
#endif //_STATISTICS
		}
		else
		{
			SYS_DEBUG(SYSDEBUG_TRACE_WARNINGS, "lhl tlabel async stream freeing item already free\n\r");
		}
	}
	else
	{
		hResult = E_LHL_TLABEL_MISMATCH;
		sysLogError(hResult, __LINE__, moduleName);
	}

	// exclusive access for the lhlTLabel (mutex)
	TCMutexUnlock(lhlTLabelMutexSemID);

	return hResult;
}

#endif //_LOOSE_ISO
