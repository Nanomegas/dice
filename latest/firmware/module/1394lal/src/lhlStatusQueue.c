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
 * DESCRIPTION: Link Hardware Abstraction Layer (Status Queue)

	Purpose:	Link Hardware Abstraction Layer
				Status "Queues" for requests and responses
	
	Revisions:
		created 08/26/2002 lm

****************************************************************************/

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "lhl.h"
#include "lhlCli.h"
#include "lhlStatistics.h"
#include "lhlStatusQueue.h"

static STATUS_INFO_QUEUE statusInfoQueue;

static uint32 lhlStatusQueueMutexSemID; // mutual exclusion semaphore for statusInfoQueue

// semaphore IDs for this module is also included in the STATUS_INFO structures

#ifdef _SYSDEBUGERROR
static HRESULT __lhlStatusQueueDisplay(void)
{
	HRESULT			hResult = NO_ERROR;
	STATUS_INFO		*si = NULL;
	uint16			i;
	char			timestr[16];

	sysGetCurTimeStr(timestr);
	sysDebugPrintf("lhlStatusQueueDisplay: time: %s\n\r", timestr);

	for (i = 0; i < STATUS_INFO_ITEMS; i++)
	{
		if (statusInfoQueue.allocated[i] == TRUE)
		{
			si = &statusInfoQueue.statusInfo[i];
			sysDebugPrintf("%i: nodeAddr 0x%04x, tLabel 0x%04x, pckType:0x%04x, genType:0x%04x\n\r", i, si->nodeAddr, si->tLabel, si->packetType, si->genType);
		}
	}

	return hResult;
}

#if 0
static HRESULT lhlStatusQueueDisplay(void)
{
	HRESULT		hResult = NO_ERROR;

	// exclusive access for the pending transactions table (mutex)
	hResult = TCMutexLock(lhlStatusQueueMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	hResult = __lhlStatusQueueDisplay();

	// exclusive access for the pending transactions table (mutex)
	TCMutexUnlock(lhlStatusQueueMutexSemID);

	return hResult;
}
#endif
#endif //_SYSDEBUGERROR

/*********************************************************
	Create semaphores for this module
*/
HRESULT lhlStatusQueueCreateSemaphores(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		index;

	for (index = 0; index < STATUS_INFO_ITEMS; index++)
	{
		// status info synchronization semaphore
		// handling multiple... requests/responses read, write, lock, error
		hResult = TCSemaphoreOpen(&statusInfoQueue.statusInfo[index].semID, 0);
		if (hResult != NO_ERROR) return hResult;
	}

	// exclusive access for the statusInfoQueue (mutex)
	hResult = TCMutexOpen(&lhlStatusQueueMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT lhlStatusQueueInitialize(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		index;
	STATUS_INFO *statusInfo;

	// statusInfoQueue structure cleared to zero
	memset(&statusInfoQueue, 0, sizeof(statusInfoQueue));

	hResult = lhlStatusQueueCreateSemaphores();
	if (hResult != NO_ERROR) return hResult;

	for (index = 0; index < STATUS_INFO_ITEMS; index++)
	{
		statusInfo = &statusInfoQueue.statusInfo[index];
		lhlStatusQueueResetInfo(statusInfo);
		statusInfoQueue.allocated[index] = FALSE;
	}

#ifdef _STATISTICS
	lhlStatistics.statusInfoInuse = 0;
	lhlStatistics.statusInfoInuseMax = 0;
#endif //_STATISTICS

	return hResult;
}

HRESULT lhlStatusQueueResetInfo(STATUS_INFO *statusInfo)
{
	HRESULT		hResult = NO_ERROR;

	if (statusInfo == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	statusInfo->nodeAddr = 0;
	statusInfo->tLabel = 0;
	statusInfo->genType = LHL_GEN_UNDEF;
	statusInfo->packetType = PB_TYPE_UNDEF;
	statusInfo->numBytes = 0;
	statusInfo->data = 0;
	statusInfo->hResultFinal = E_LHL_STATUSINFO_UNDEF;

	TCSemaphoreReset (statusInfo->semID);

	return hResult;
}

HRESULT lhlStatusQueueCreate(STATUS_INFO **statusInfo)
{
	HRESULT		hResult = NO_ERROR;
	uint32		index;
	uint32		ptr;
	BOOL		bFound = FALSE;
	
	// exclusive access for the statusInfoQueue (mutex)
	hResult = TCMutexLock(lhlStatusQueueMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	*statusInfo = NULL;

	// find a free statusInfo from the pool
	ptr = statusInfoQueue.ptr;
	
	for (index = 0; index < STATUS_INFO_ITEMS; index++)
	{
		if (statusInfoQueue.allocated[ptr] == FALSE)
		{
			*statusInfo = &statusInfoQueue.statusInfo[ptr];
			lhlStatusQueueResetInfo(*statusInfo);
			statusInfoQueue.allocated[ptr] = TRUE;
			bFound = TRUE;

#ifdef _STATISTICS
			lhlStatistics.statusInfoInuse++;
			if (lhlStatistics.statusInfoInuseMax < lhlStatistics.statusInfoInuse)
			{
				lhlStatistics.statusInfoInuseMax = lhlStatistics.statusInfoInuse;
			}
#endif //_STATISTICS
		}

		ptr = (ptr + 1) % STATUS_INFO_ITEMS;
		
		if (bFound == TRUE)
		{
			statusInfoQueue.ptr = ptr;
			break;
		}
	}

	if (bFound == FALSE)
	{
		SYS_DEBUG(SYSDEBUG_TRACE_WARNINGS, "lhlStatusQueue: No more free status info items\n\r");
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
	}

	// exclusive access for the statusInfoQueue (mutex)
	TCMutexUnlock(lhlStatusQueueMutexSemID);

	return hResult;
}

HRESULT lhlStatusQueueMatch(uint32 nodeAddr, uint32 tLabel, uint32 genType, PB_PACKETTYPE packetType, STATUS_INFO **statusInfo, BOOL bRemoveEntry)
{
	HRESULT		hResult = NO_ERROR;
	uint32		index;
	STATUS_INFO	*si = NULL;
	BOOL		bFound = FALSE;

	// exclusive access for the statusInfoQueue (mutex)
	hResult = TCMutexLock(lhlStatusQueueMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	if (statusInfo)
	{
		*statusInfo = NULL;
	}

	// find a matching statusInfo from the pool
	for (index = 0; index < STATUS_INFO_ITEMS; index++)
	{
		si = &statusInfoQueue.statusInfo[index];
		if (statusInfoQueue.allocated[index] == TRUE)
		{
			if (si->nodeAddr == nodeAddr &&
				si->tLabel == tLabel &&
				si->packetType == packetType &&
				si->genType == genType)
			{
				bFound = TRUE;
				if (statusInfo)
				{
					*statusInfo = si;
				}
				if (bRemoveEntry)
				{
					hResult = si->hResultFinal;
					lhlStatusQueueResetInfo(si);
					statusInfoQueue.allocated[index] = FALSE;
					statusInfoQueue.ptr = index;

#ifdef _STATISTICS
					lhlStatistics.statusInfoInuse--;
#endif //_STATISTICS
				}
				break;
			}
		}
	}

	if (bFound == FALSE ||
		(statusInfo && *statusInfo == NULL))
	{
		SYS_DEBUG(SYSDEBUG_TRACE_WARNINGS, "lhlStatusQueue: No match nodeAddr 0x%04x, tLabel 0x%04x, pckType:0x%04x, genType:0x%04x\n\r", nodeAddr, tLabel, packetType, genType);
#ifdef _SYSDEBUGERROR
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_ERRORS))
		{
			sysDebugPrintf("lhlStatusQueueMatch\n\r");
			__lhlStatusQueueDisplay();
		}
#endif //_SYSDEBUGERROR
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
	}

	// exclusive access for the statusInfoQueue (mutex)
	TCMutexUnlock(lhlStatusQueueMutexSemID);

	return hResult;
}
