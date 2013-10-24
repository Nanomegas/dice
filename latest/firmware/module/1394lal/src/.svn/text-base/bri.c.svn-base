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
 * DESCRIPTION: Bus Reset Interface

	Purpose: Bus Reset Interface functions

	Revisions:
		created 04/24/2002 lm
****************************************************************************/

#include "TCTasking.h"
#include "lhl.h"
#include "lhlBri.h"
#include "lhlCli.h"
#include "bri.h"
#include "nci.h"
#include "nciBM.h"
#include "nciHandles.h"
#include "nciIRM.h"
#include "sysDebug.h"

static char 		moduleName[] = __FILE__ ;

BOOL			briInitialized;						// module initialization function has been called

uint32			briCurBusGen;						// current bus generation
uint32			briNumBRBeforeCompletion;			// number of bus resets that happened before a current one completes

uint32			briLastResetIndication;				// time when last bus reset indication
uint32			briLastResetCompletion;				// time when last bus reset completion

uint32			briIndicationNumCBs;				// number of indication callbacks
uint32			briCompletionNumCBs;				// number of completion callbacks
uint32			briIndicationIndex;					// current indication callback index in progress
uint32			briCompletionIndex;					// current completion callback index in progress

BRI_CALLBACK	briIndicationCBs[BRI_MAX_BR_CBS];	// callback list for bus reset indication
BRI_CALLBACK	briCompletionCBs[BRI_MAX_BR_CBS];	// callback list for bus reset complete

BOOL			briInBusResetProgress;				// in bus reset progress
#ifdef _BRI_PRE_COMPLETION
BOOL			briPreCompletionDone;				// in bus reset pre completion is done (before handling BM and handles stuff)
#endif //_BRI_PRE_COMPLETION

LOCAL_NODEINFO	briLocalNodeInfo;					// store info about local node bus properties

SELFID_BUNDLE	briSelfIDs;							// cache for the current self-id bundle
BIB_DATA		briBIBs[MAX_NODES_PER_BUS];			// busInfoBlock data

// semaphore IDs for this module
uint32			briIndicationSemID;					// bus reset indication check semaphore
uint32			briIndicationMutexSemID;			// exclusive access for briIndicationWaitingTasks (mutex)
uint32			briIndicationWaitingTasks;			// bus reset indication waiting tasks (only accessed thru briIndicationMutexSemID)

uint32			briCompletionSemID;					// bus reset completion check semaphore
uint32			briCompletionMutexSemID;			// exclusive access for briCompletionWaitingTasks (mutex)
uint32			briCompletionWaitingTasks;			// bus reset completion waiting tasks (only accessed thru briCompletionMutexSemID)

#ifdef _BRI_PRE_COMPLETION
uint32			briPreCompletionSemID;				// bus reset pre completion check semaphore
uint32			briPreCompletionMutexSemID;			// exclusive access for briPreCompletionWaitingTasks (mutex)
uint32			briPreCompletionWaitingTasks;		// bus reset pre completion waiting tasks (only accessed thru briPreCompletionMutexSemID)
#endif //_BRI_PRE_COMPLETION

/*********************************************************
	Initialize this module
*/
HRESULT	briInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

	briInBusResetProgress = TRUE;					// initialize to TRUE to ensure no one starts sending before we're initialized
#ifdef _BRI_PRE_COMPLETION
	briPreCompletionDone = FALSE;					// initialize to FALSE since pre completion is not done 
#endif //_BRI_PRE_COMPLETION

	if (briInitialized == TRUE)
	{
		hResult = E_ALREADY_INITIALIZED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// bus generation
	briCurBusGen = 0;

	// have seen no bus resets yet and aren't in the middle of processing one
	briNumBRBeforeCompletion = 0;

	// time when last bus reset indication
	briLastResetIndication = 0;
	// time when last bus reset completion
	briLastResetCompletion = 0;

	// reset the callback list counts
	briIndicationNumCBs = 0;
	briCompletionNumCBs = 0;

	// no callbacks in progress
	briIndicationIndex = 0;
	briCompletionIndex = 0;

	// clear the callback pointer lists
	memset(briIndicationCBs, 0, sizeof(briIndicationCBs));
	memset(briCompletionCBs, 0, sizeof(briCompletionCBs));

	briLocalNodeInfo.busId = 0;
	briLocalNodeInfo.nodeId = 0;

	// clear the self-id local cache
	memset(&briSelfIDs, 0, sizeof(briSelfIDs));

	// clear bus info block cache
	memset(&briBIBs, 0, sizeof(briBIBs));

	briIndicationWaitingTasks = 0;
	briCompletionWaitingTasks = 0;
#ifdef _BRI_PRE_COMPLETION
	briPreCompletionWaitingTasks = 0;
#endif //_BRI_PRE_COMPLETION
	
	briCreateSemaphores();

	// ready to go
	briInitialized = TRUE;

	return hResult;
}

/*********************************************************
	Create semaphores for this module
*/
HRESULT briCreateSemaphores(void)
{
	HRESULT		hResult = NO_ERROR;

	// bus reset indication check semaphore
	hResult = TCSemaphoreOpen(&briIndicationSemID, 0);
	if (hResult != NO_ERROR) return hResult;

	// exclusive exclusive access for briIndicationWaitingTasks (mutex)
	hResult = TCMutexOpen(&briIndicationMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	// bus reset completion check semaphore
	hResult = TCSemaphoreOpen(&briCompletionSemID, 0);
	if (hResult != NO_ERROR) return hResult;

	// exclusive exclusive access for briCompletionWaitingTasks (mutex)
	hResult = TCMutexOpen(&briCompletionMutexSemID);
	if (hResult != NO_ERROR) return hResult;

#ifdef _BRI_PRE_COMPLETION
	// bus reset pre completion check semaphore
	hResult = TCSemaphoreOpen(&briPreCompletionSemID, 0);
	if (hResult != NO_ERROR) return hResult;

	// exclusive exclusive access for briPreCompletionWaitingTasks (mutex)
	hResult = TCMutexOpen(&briPreCompletionMutexSemID);
	if (hResult != NO_ERROR) return hResult;
#endif //_BRI_PRE_COMPLETION

	return hResult;
}

/*********************************************************
	Return the number of bus resets this node has seen since hardware reset 
*/
HRESULT briGetCurrentBusGeneration(uint32 *pCurBusGen)
{
	HRESULT		hResult	= NO_ERROR;
	
	*pCurBusGen = 0;

	if (!briInitialized)
	{
		hResult = E_BRI_NOT_INITIALIZED_FATAL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*pCurBusGen = briCurBusGen;

	return hResult;
}

/*********************************************************
	Update the current bus generation number
*/
void briSetCurrentBusGeneration(uint32 CurBusGen)
{
	briCurBusGen = CurBusGen;
}

BOOL briInBusReset(void)
{
	BOOL	inReset = FALSE;

	if (lhlBriInBusReset())
	{
		inReset = TRUE;
	}
	else
	{
#ifdef _BRI_PRE_COMPLETION
		inReset = (briInBusResetProgress == TRUE && briPreCompletionDone == FALSE);
#else //_BRI_PRE_COMPLETION
		inReset = briInBusResetProgress;
#endif //_BRI_PRE_COMPLETION
	}

	return inReset;
}

/*********************************************************
	Build the speed map from the self-id bundle
*/
void briBuildSpeedMap(void)
{
#ifdef _BMC_CAPS
	HRESULT		hResult = NO_ERROR;
	uint32		generation;

	hResult = briGetCurrentBusGeneration(&generation);
	if (hResult != NO_ERROR) return;

	hResult = nciBMBuildSpeedMap(briGetNumNodes(), generation);
	if (hResult != NO_ERROR) return;
#endif //_BMC_CAPS
}

/*********************************************************
	Build the topology map from the self-id bundle
*/
void briBuildTopologyMap(void)
{
#ifdef _BMC_CAPS
	HRESULT		hResult = NO_ERROR;
	uint32		generation;

	hResult = briGetCurrentBusGeneration(&generation);
	if (hResult != NO_ERROR) return;

	hResult = nciBMBuildTopologyMap (briGetSIDs(), briGetNumSIDQuads(), briGetNumNodes(), generation);
	if (hResult != NO_ERROR) return;
#endif //_BMC_CAPS
}

/*********************************************************
	Scan all bus node Bus Info Blocks and cache locally
*/
HRESULT briBuildNodeInfos(void)
{
	return nciGetBusInfoBlocks(briGetNumNodes(), briGetBusInfoBlockCache());	// for handle maintenance
}

HRESULT briUpdateNodeInfos(BOOL bClear)
{
	return nciUpdateBusInfoBlocks(briGetNumNodes(), briGetBusInfoBlockCache(),bClear);	// for handle maintenance
}


/*********************************************************
	Based on WWUIDs found in the Node Info Blocks, fixup any open handles
*/
HRESULT briFixupHandles(void)
{
	return nciFixupNodeHandles();	// must have valid current bus info block cache
}

/*********************************************************
	Called by BR Thread.  Have the NCI validate the self-id bundle, find IRM, etc.
*/
HRESULT briProcessSelfId(void)
{
	HRESULT		hResult = NO_ERROR;

	if (briIsSelfIDsValid() == TRUE)
	{
		hResult = nciVerifySelfIDs(	briGetSIDs(), 
									briGetNumSIDQuads(),
									briGetNumNodes());
	}
	else
	{
		hResult = E_LHL_SELF_ID_DATA_ERROR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

/*********************************************************
	Add a callback for bus reset indication processing
*/
HRESULT briRegisterBRIndicationCallback (BRI_CALLBACK callback)
{
	HRESULT		hResult = NO_ERROR;

	if (!briInitialized)
	{
		hResult = E_BRI_NOT_INITIALIZED_FATAL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (briIndicationNumCBs >= BRI_MAX_BR_CBS)
	{
		hResult = E_BRI_TOO_MANY_CBS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	briIndicationCBs[briIndicationNumCBs] = callback;

	briIndicationNumCBs++;

	return hResult;
}

/*********************************************************
	Add a callback for bus reset complete processing
*/
HRESULT briRegisterBRCompletionCallback (BRI_CALLBACK callback)
{
	HRESULT		hResult = NO_ERROR;

	if (!briInitialized)
	{
		hResult = E_BRI_NOT_INITIALIZED_FATAL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (briCompletionNumCBs >= BRI_MAX_BR_CBS)
	{
		hResult = E_BRI_TOO_MANY_CBS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	briCompletionCBs[briCompletionNumCBs] = callback;

	briCompletionNumCBs++;

	return hResult;
}

/*********************************************************
	Let everyone who registered interest know that a bus reset is in progress
*/
void briCallBusResetIndicationCBs(void)
{
	while (briIndicationIndex < briIndicationNumCBs)
	{
		if (briIndicationCBs[briIndicationIndex])
		{
			(briIndicationCBs[briIndicationIndex])();
			briIndicationIndex++;
		}
		else
		{
			break;
		}
	}

	briIndicationIndex = 0;

#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_BUSRESET))
	{
		sysDebugPrintf("bri indication cbs done\n\r");
	}
#endif //_SYSDEBUG
}

/*********************************************************
	Let everyone who registered interest know that the bus reset is finished
*/
void briCallBusResetCompletionCBs(void)
{
	while (briCompletionIndex < briCompletionNumCBs)
	{
		if (briCompletionCBs[briCompletionIndex])
		{
			(briCompletionCBs[briCompletionIndex])();
			briCompletionIndex++;
		}
		else
		{
			break;
		}
	}
	briCompletionIndex = 0;

#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_BUSRESET))
	{
		sysDebugPrintf("bri completion cbs done\n\r");
	}
#endif //_SYSDEBUG
}

/*********************************************************
	First indication of a bus reset. called by the Link Rx Event Thread.
*/
void briBusResetIndication(void)
{
	// this will halt application messaging until br processing is done

	briLastResetIndication = TCTimeGet();

	briInBusResetProgress = TRUE;
#ifdef _BRI_PRE_COMPLETION
	briPreCompletionDone = FALSE;
#endif //_BRI_PRE_COMPLETION

	// clear the self-id local cache
	memset(&briSelfIDs, 0, sizeof(briSelfIDs));

	// clear bus info block cache
	memset(&briBIBs, 0, sizeof(briBIBs));

#ifdef _IRMC_CAPS
	nciIRMReset();					// reset IRM to default
#endif //_IRMC_CAPS

#ifdef _BM
	nciBMBusResetIndication();
#endif //_BM

	briSignalOnResetIndication ();
}

/*********************************************************
	reenable general async packet traffic, set event flag to wake up any suspended threads
*/
void briBusResetCompletion(void)
{
	briInBusResetProgress = FALSE;

#ifdef _BRI_PRE_COMPLETION
	briPreCompletionDone = TRUE;
#else //_BRI_PRE_COMPLETION
	// moved to briBusResetPreCompletion from briBusResetCompletion
	briLastResetCompletion = TCTimeGet();
#endif //_BRI_PRE_COMPLETION

	briSignalOnResetCompletion ();

	/*
		it is a common situation for nodes to set up manager preferences by 
		setting things and doing follow-on resets, however if there are many 
		more than this, there could be a cranky node on the bus
	*/
	if (briNumBRBeforeCompletion > BRI_BUSRESET_STORM_THRESHOLD)
	{
		sysDebugPrintf("%d interrupted bus resets\n\r", briNumBRBeforeCompletion);
	}
	briNumBRBeforeCompletion = 0;

#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_BUSRESET))
	{
		sysDebugPrintf("bus reset completion done\n\r");
	}
#endif //_SYSDEBUG
}

#ifdef _BRI_PRE_COMPLETION
void briBusResetPreCompletion(void)
{
	// moved to briBusResetPreCompletion from briBusResetCompletion
	briLastResetCompletion = TCTimeGet();

	briPreCompletionDone = TRUE;

	briSignalOnResetPreCompletion ();

#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_BUSRESET))
	{
		sysDebugPrintf("bus reset pre completion done\n\r");
	}
#endif //_SYSDEBUG
}
#else //_BRI_PRE_COMPLETION
void briBusResetPreCompletion(void)
{
}
#endif //_BRI_PRE_COMPLETION

/*********************************************************
	Wait for a bus reset indication
*/
HRESULT	briWaitOnResetIndication (BOOL *bResetDetected)
{
	HRESULT		hResult = NO_ERROR;

	*bResetDetected = TRUE;

	if (!briInitialized)
	{
		hResult = E_BRI_NOT_INITIALIZED_FATAL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// exclusive access for briIndicationWaitingTasks (mutex)
	hResult = TCMutexLock(briIndicationMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	if (briInBusResetProgress == FALSE)
	{
		*bResetDetected = FALSE;
		briIndicationWaitingTasks++;		
	}

	// Release exclusive access for briIndicationWaitingTasks (mutex)
	TCMutexUnlock(briIndicationMutexSemID);
	
	if (*bResetDetected == FALSE)
	{
		hResult = TCSemaphoreWait(briIndicationSemID);
		if (hResult != NO_ERROR) return hResult;
	}

	return hResult;
}

/*********************************************************
	Wait for a bus reset completion
*/
HRESULT	briWaitOnResetCompletion (BOOL *bResetDetected)
{
	HRESULT		hResult = NO_ERROR;

	*bResetDetected = FALSE;

	if (!briInitialized)
	{
		hResult = E_BRI_NOT_INITIALIZED_FATAL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// exclusive access for briCompletionWaitingTasks (mutex)
	hResult = TCMutexLock(briCompletionMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	if (briInBusResetProgress == TRUE)
	{
		*bResetDetected = TRUE;
		briCompletionWaitingTasks++;		
	}

	// Release exclusive access for briCompletionWaitingTasks (mutex)
	TCMutexUnlock(briCompletionMutexSemID);
	
	if (*bResetDetected == TRUE)
	{
		hResult = TCSemaphoreWait(briCompletionSemID);
		if (hResult != NO_ERROR) return hResult;
	}

	return hResult;
}

#ifdef _BRI_PRE_COMPLETION
/*********************************************************
	Wait for a bus reset pre completion
*/
HRESULT	briWaitOnResetPreCompletion (BOOL *bResetDetected)
{
	HRESULT		hResult = NO_ERROR;

	*bResetDetected = FALSE;

	if (!briInitialized)
	{
		hResult = E_BRI_NOT_INITIALIZED_FATAL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// exclusive access for briPreCompletionWaitingTasks (mutex)
	hResult = TCMutexLock(briPreCompletionMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	if (briInBusResetProgress == TRUE &&
		briPreCompletionDone == FALSE)
	{
		*bResetDetected = TRUE;
		briPreCompletionWaitingTasks++;		
	}

	// Release exclusive access for briPreCompletionWaitingTasks (mutex)
	TCMutexUnlock(briPreCompletionMutexSemID);
	
	if (*bResetDetected == TRUE)
	{
		hResult = TCSemaphoreWait(briPreCompletionSemID);
		if (hResult != NO_ERROR) return hResult;
	}

	return hResult;
}
#endif //_BRI_PRE_COMPLETION

/*********************************************************
	Signal to waiting threads bus reset indication
*/
HRESULT	briSignalOnResetIndication (void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		waitingTasks = 0;

	if (!briInitialized)
	{
		hResult = E_BRI_NOT_INITIALIZED_FATAL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// Exclusive exclusive access for briIndicationWaitingTasks (mutex)
	hResult = TCMutexLock(briIndicationMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	waitingTasks = briIndicationWaitingTasks;
	briIndicationWaitingTasks = 0;
	
	// Release exclusive access for briIndicationWaitingTasks (mutex)
	TCMutexUnlock(briIndicationMutexSemID);

	if (waitingTasks > 0)
	{
		while (waitingTasks--)
		{
			TCSemaphoreSignal(briIndicationSemID);
		}
	}

	return hResult;
}

/*********************************************************
	Signal to waiting threads bus reset completion
*/
HRESULT	briSignalOnResetCompletion (void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		waitingTasks = 0;

	if (!briInitialized)
	{
		hResult = E_BRI_NOT_INITIALIZED_FATAL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// Exclusive exclusive access for briCompletionWaitingTasks (mutex)
	hResult = TCMutexLock(briCompletionMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	waitingTasks = briCompletionWaitingTasks;
	briCompletionWaitingTasks = 0;
	
	// Release exclusive access for briCompletionWaitingTasks (mutex)
	TCMutexUnlock(briCompletionMutexSemID);

	if (waitingTasks > 0)
	{
		while (waitingTasks--)
		{
			TCSemaphoreSignal(briCompletionSemID);
		}
	}

	return hResult;
}

#ifdef _BRI_PRE_COMPLETION
/*********************************************************
	Signal to waiting threads bus reset completion
*/
HRESULT	briSignalOnResetPreCompletion (void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		waitingTasks = 0;

	if (!briInitialized)
	{
		hResult = E_BRI_NOT_INITIALIZED_FATAL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// Exclusive exclusive access for briPreCompletionWaitingTasks (mutex)
	hResult = TCMutexLock(briPreCompletionMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	waitingTasks = briPreCompletionWaitingTasks;
	briPreCompletionWaitingTasks = 0;
	
	// Release exclusive access for briPreCompletionWaitingTasks (mutex)
	TCMutexUnlock(briPreCompletionMutexSemID);

	if (waitingTasks > 0)
	{
		while (waitingTasks--)
		{
			TCSemaphoreSignal(briPreCompletionSemID);
		}
	}

	return hResult;
}
#endif //_BRI_PRE_COMPLETION

/*********************************************************
	time elapsed since last bus reset indication
*/

HRESULT briElapsedTimeSinceResetIndication(uint32 *elapsedTime)
{
	HRESULT		hResult = NO_ERROR;

	*elapsedTime = TCTimeElapsed(briLastResetIndication);

	return hResult;
}

/*********************************************************
	time elapsed since last bus reset completion
*/

HRESULT briElapsedTimeSinceResetCompletion(uint32 *elapsedTime)
{
	HRESULT		hResult = NO_ERROR;

	*elapsedTime = 0;

	if (briInBusResetProgress == TRUE)
	{
		hResult = E_BRI_NEW_BUS_RESET;
	}

	*elapsedTime = TCTimeElapsed(briLastResetCompletion);

	return hResult;
}

#ifdef _BRI_PRE_COMPLETION
HRESULT briElapsedTimeSinceResetPreCompletion(uint32 *elapsedTime)
{
	HRESULT		hResult = NO_ERROR;

	*elapsedTime = 0;

	if (briInBusResetProgress == TRUE &&
		briPreCompletionDone == FALSE)
	{
		hResult = E_BRI_NEW_BUS_RESET;
	}

	*elapsedTime = TCTimeElapsed(briLastResetCompletion);

	return hResult;
}
#endif //_BRI_PRE_COMPLETION

HRESULT briWaitTimeSinceResetIndication(uint32 msec)
{
	HRESULT		hResult = NO_ERROR;
	uint32		elapsedTime = 0;
	uint32		waitTime = 0;

	DO_FOREVER
	{
		// wait until msec has elapsed since the bus reset indication, or since another reset indication occurs
		hResult = briElapsedTimeSinceResetIndication(&elapsedTime);
		if (hResult != NO_ERROR) break;

		if (elapsedTime < msec)
		{
			waitTime = msec - elapsedTime;
			TCTaskWait(waitTime);
			continue; // check if another reset indication has occured in between
		}
		break;
	}

	return hResult;
}

HRESULT briWaitTimeSinceResetCompletion(uint32 msec)
{
	HRESULT		hResult = NO_ERROR;
	uint32		elapsedTime = 0;
	uint32		waitTime = 0;

	DO_FOREVER
	{
		// wait until msec has elapsed since the bus reset completion, or since another reset completion occurs
		hResult = briElapsedTimeSinceResetCompletion(&elapsedTime);
		if (hResult != NO_ERROR)
		{
			if (hResult == E_BRI_NEW_BUS_RESET)
			{
				BOOL	bResetDetected;
				briWaitOnResetCompletion (&bResetDetected);
				continue; // in a bus reset process
			}
			break;
		}

		if (elapsedTime < msec)
		{
			waitTime = msec - elapsedTime;
			TCTaskWait(waitTime);
			continue; // check if another reset completion has occured in between
		}
		break;
	}

	return hResult;
}

#ifdef _BRI_PRE_COMPLETION
HRESULT briWaitTimeSinceResetPreCompletion(uint32 msec)
{
	HRESULT		hResult = NO_ERROR;
	uint32		elapsedTime = 0;
	uint32		waitTime = 0;

	DO_FOREVER
	{
		// wait until msec has elapsed since the bus reset completion, or since another reset completion occurs
		hResult = briElapsedTimeSinceResetPreCompletion(&elapsedTime);
		if (hResult != NO_ERROR)
		{
			if (hResult == E_BRI_NEW_BUS_RESET)
			{
				BOOL	bResetDetected;
				briWaitOnResetPreCompletion (&bResetDetected);
				continue; // in a bus reset process
			}
			break;
		}

		if (elapsedTime < msec)
		{
			waitTime = msec - elapsedTime;
			TCTaskWait(waitTime);
			continue; // check if another reset completion has occured in between
		}
		break;
	}

	return hResult;
}
#endif //_BRI_PRE_COMPLETION

HRESULT briGetBusID(uint16 *pBusId)
{
	HRESULT		hResult = NO_ERROR;
	
	if (briIsSelfIDsValid() == TRUE)
	{
		*pBusId = briLocalNodeInfo.busId;
	}
	else
	{
		hResult = E_LHL_NODEID_NOT_AVAIL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

BOOL briIsThisNodeAddr(uint32 nodeAddr)
{
	HRESULT		hResult = NO_ERROR;
	uint32		localAddr = 0;

	hResult = briGetThisNodeAddr(&localAddr);
	if (hResult != NO_ERROR) return FALSE;

	return (nodeAddr == localAddr);
}

HRESULT briGetThisNodeAddr(uint32 *pNodeAddr)
{
	HRESULT		hResult = NO_ERROR;

	if (briIsSelfIDsValid() == FALSE)
	{
		hResult = E_LHL_NODEID_NOT_AVAIL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*pNodeAddr = (uint32) (briLocalNodeInfo.busId | briLocalNodeInfo.nodeId);

	return hResult;
}

BOOL briIsThisNodeRoot(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		localAddr = 0;
	uint32		rootAddr = 0;

	hResult = briGetThisNodeAddr(&localAddr);
	if (hResult != NO_ERROR) return FALSE;

	hResult = briGetNodeAddrRoot(&rootAddr);
	if (hResult != NO_ERROR) return FALSE;

	return (rootAddr == localAddr);
}

HRESULT briGetNodeAddrRoot(uint32 *pNodeAddr)
{
	HRESULT		hResult = NO_ERROR;
	uint16		busId = 0;
	uint32		rootId = ROOT_ID_UNDEF;
	uint32		numNodes = 0;

	// find out Root node address
	numNodes = briGetNumNodes();

	if (numNodes == 0)
	{
		hResult = E_NCI_NODE_ID_OUT_OF_RANGE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	rootId = numNodes - 1; 

	hResult = briGetBusID(&busId);
	if (hResult != NO_ERROR) return hResult;

	*pNodeAddr = rootId | busId;

	return hResult;
}

void briGetLocalNodeInfo(LOCAL_NODEINFO	*pNodeInfo)
{
	pNodeInfo->busId = briLocalNodeInfo.busId;
	pNodeInfo->nodeId = briLocalNodeInfo.nodeId;
}

void briSetLocalNodeInfo(LOCAL_NODEINFO	*pNodeInfo)
{
	briLocalNodeInfo.busId = pNodeInfo->busId;
	briLocalNodeInfo.nodeId = pNodeInfo->nodeId;
}

BIB_DATA* briGetBusInfoBlockCache(void)
{
	return briBIBs;
}

uint32 briGetBusInfoBlockCacheData(uint32 nodeIndex, uint32 dataIndex)
{
	HRESULT		hResult = NO_ERROR;
	uint32		data = 0;

	if (nodeIndex >= MAX_NODES_PER_BUS ||
		dataIndex >= BIB_QUAD_ITEMS)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return data;
	}

	data = briBIBs[nodeIndex].busInfoBlock[dataIndex];

	return data;
}

BOOL briIsSelfIDsValid(void)
{
	return briSelfIDs.bValid;
}

SELFID_BUNDLE*	briGetSelfIDBundle(void)
{
	return &briSelfIDs;
}

QUADLET* briGetSIDs(void)
{
	return briSelfIDs.sid;
}

QUADLET briGetIndexedSID(uint32 index)
{
	HRESULT		hResult = NO_ERROR;
	QUADLET	sid = 0;

	if (index >= SELFID_BUNDLE_SID_ITEMS)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return sid;
	}
	
	sid = briSelfIDs.sid[index];

	return sid;
}

uint32 briGetNumNodes(void)
{
	return ((uint32) briSelfIDs.numNodes);
}

uint32 briGetNumSIDQuads(void)
{
	return ((uint32) briSelfIDs.numSIDQuads);
}

