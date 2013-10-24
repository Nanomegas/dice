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
 * DESCRIPTION: Link Hardware Abstraction Layer (Bus Reset Interface)

	Purpose:	Link Hardware Abstraction Layer 
				Bus Reset Interface helper functions
	
	Revisions:
		created 12/16/2002 kbj

****************************************************************************/

#include "TCTasking.h"
#include "lhl.h"
#include "lhlARx.h"
#include "lhlBri.h"
#include "lhlCli.h"
#include "lhlPendingTx.h"
#include "lhlStatistics.h"
#include "lhlTLabel.h"
#include "bri.h"
#include "csr.h"
#include "llcPHY.h"
#include "llcReg.h"
#include "llcInt.h"
#include "llcSelfID.h"
#include "nciCli.h"
#include "nciBM.h"
#include "nciBMCli.h"

#ifdef _AVS
#include "avs.h"
#endif //_AVS

extern void lhlServiceReset(void);

static BOOL bHandlesValid = FALSE;					// state of handle fix-ups after bus scan

// static char 		moduleName[] = __FILE__ ;

// semaphore IDs for this module
uint32					lhlBriSemID;				// bus reset process semaphore

// task (thread) IDs for this module
uint32					lhlBriThreadID;				// bus reset process thread
const char*				lhlBriThreadName = "lhlBriThread";

#if 0 //LM???
uint32	lhlBriIntHandlerBusResetGen = 0;
uint32	lhlBriHandleBusResetPartGen = 0;
#endif

SELFID_BUNDLE			lhlBriSelfIDs;				// cache for the current self-id bundle


HRESULT	lhlBriForceBusReset(void)
{
	HRESULT	hResult = NO_ERROR;

	hResult = llcIntEventClearBusReset(LLC_INT_STATE_WAIT_BUSRESET);
	hResult = llcBusReset(FALSE);
	return hResult;
}

HRESULT	lhlBriForceRoot(void)
{
	HRESULT	hResult = NO_ERROR;

	hResult = llcIntEventClearBusReset(LLC_INT_STATE_WAIT_BUSRESET);
	hResult = llcBusReset(TRUE);
	return hResult;
}

BOOL lhlBriIsRoot(void)
{
	return llcIsRoot();
}

/*
	get the current bus cycle time
*/
HRESULT lhlBriGetCycleTime(uint32* time)
{
	HRESULT	hResult = NO_ERROR;

	hResult = llcGetCycleTime(time);
	return hResult;
}

/*
	set Cycle Master function of the Link chip
*/
HRESULT lhlBriSetCycleMaster(BOOL enabled)
{
	HRESULT	hResult = NO_ERROR;

	hResult = llcSetCycleMaster(enabled);
	return hResult;
}

BOOL lhlBriIsCycleMaster(void)
{
	return llcIsCycleMaster();
}

/*********************************************************
	when a bus reset occurs, a number of Link housekeeping actions must be
	performed.  the reset will cause receive FIFO read error bits to be set
	(normal).  also, this is a good time to check that there are no cable
	topology loops that this node can sense.  finally, we wait here until 
	the self-identify part of the reset cycle has occured, at which point we
	know that our node id has been determined and that we can start the
	bus reset processing.
*/
HRESULT lhlBriBusResetIndication(void)
{
	HRESULT	hResult = NO_ERROR;
	QUADLET	qPhyData = 0;

	// check for loop in bus cabling
	llcReadPhy(PHY_BUS_STATUS, &qPhyData);
	if (qPhyData & PHY_BUS_STATUS_CTOI)
	{
		hResult = E_LHL_CABLE_LOOP_DETECTED;
		sysLogErrorISR(hResult, __LINE__, "lhlBriBusResetIndication: ** check cable topology for loops **\n\r");
	}
	
	return hResult;
}

/*********************************************************
// Stop transmit and receive DMA controllers
*/
HRESULT lhlBriBusResetInitLink(void)
{
	HRESULT	hResult = NO_ERROR;
	hResult = llcBusResetInitialize();
	return hResult;
}


/*********************************************************
	Get the self-id bundles (check if self id data)
*/
HRESULT lhlBriGetSelfIDBundle(void)
{
	HRESULT			hResult = NO_ERROR;

	// clear the self-id local cache
	memset(&lhlBriSelfIDs, 0, sizeof(lhlBriSelfIDs));

	hResult = llcGetSelfIDs(&lhlBriSelfIDs);
	return hResult;
}

HRESULT lhlBriSetSelfIDBundle(void)
{
	HRESULT			hResult = NO_ERROR;
	uint8			root;
	LOCAL_NODEINFO	nodeInfo;
	uint16			generationID;

	// Check and verifies the collected selfIDs
	hResult = llcVerifySelfIDs(&lhlBriSelfIDs);
	if (hResult != NO_ERROR) return hResult;

	memcpy(briGetSelfIDBundle(), &lhlBriSelfIDs, sizeof(lhlBriSelfIDs));

	// copy the self-id local cache to the bri module
	hResult = llcGetBusNodeNumber(&nodeInfo.busId, &nodeInfo.nodeId, &root);
	if (hResult != NO_ERROR) return hResult;
	briSetLocalNodeInfo(&nodeInfo);

	llcGetSelfIDGeneration(&generationID);
	briSetCurrentBusGeneration(generationID);

	return hResult;
}

/*********************************************************
	checking low level bus reset state 
	- in bus reset
*/
BOOL lhlBriInBusReset(void)
{
	return llcIsBusReset();	// read llc IntEvent register bit LLC_BUSRESET
}

/*********************************************************
	called during bus reset process (checking for new bus resets and yielding)
*/
BOOL lhlBriNewBusResetDetected(BOOL bDuringCriticalBR)
{
	BOOL bNewBusResetDetected = FALSE;

	// check if need to re-initiate a bus reset
	
	if (bDuringCriticalBR == TRUE)
	{
		if (llcIsNewReset())	// new reset occured during bus reset process
		{
			llcIntEventNewBusReset(bDuringCriticalBR);
			bNewBusResetDetected = TRUE;
		}
	}
	else
	{
		if (llcIsBusReset())	// read llc IntEvent register bit LLC_BUSRESET
		{
			llcIntEventNewBusReset(bDuringCriticalBR);
			bNewBusResetDetected = TRUE;
		}
	}
	
#if 0 //LM???
	if (bNewBusResetDetected == FALSE)
	{
		if (lhlBriIntHandlerBusResetGen != lhlBriHandleBusResetPartGen)
		{
			BOOL reset = llcIsBusReset();
			if (reset)	// read llc IntEvent register bit LLC_BUSRESET
			{
				llcIntEventNewBusReset(bDuringCriticalBR);
			}
			else
			{
				//LM??? force bus reset
			}
			bNewBusResetDetected = TRUE;
		}
	}
#endif

	TCTaskYield();

	return bNewBusResetDetected;
}

/*********************************************************
	called when an bus reset process is started (selfId complete)
	Part1: BR critical part (ends by clearing BR and SelfIdComplete Bits)
*/
HRESULT lhlBriHandleBusResetPart1(void)
{
	HRESULT	hResult = NO_ERROR;
	BOOL	bDuringCriticalBR = TRUE;

	bHandlesValid = FALSE;

	lhlServiceReset();

	// get the BRI ready for this bus reset
	briBusResetIndication();

	// let everyone know who needs to know that a bus reset is in progress
	briCallBusResetIndicationCBs();

	if (lhlBriNewBusResetDetected(bDuringCriticalBR))
	{
		return E_BRI_NEW_BUS_RESET;
	}

	// copy the self ids collected in interrupt
	if ((hResult = lhlBriSetSelfIDBundle()) != NO_ERROR)
	{
		// Invalid SelfID buffer context
		// If two bus resets occur very close (two devices turned at approximately the same time)
		// the thread level llcVerifySelfIDs will fail because while the data are being analyzed
		// the interrupt routine is reading a new bundle. That will result in this function
		// returning an error and the system will not recover.
		//TODO: Rewrite busreset handling
		
#if 0 //ML: Removed
		lhlBriForceBusReset();
		return hResult;
#endif
	}

	if (lhlBriNewBusResetDetected(bDuringCriticalBR))
	{
		return E_BRI_NEW_BUS_RESET;
	}

	// restart LLC FIFO transmit and receive context
	if ((hResult = llcBusResetComplete()) != NO_ERROR)
	{
		// Invalid NodeID
#if 1 //LM??? not sure if we should repeately do this
		lhlBriForceBusReset();
#endif
		return hResult;
	}
	if (lhlBriNewBusResetDetected(bDuringCriticalBR))
	{
		return E_BRI_NEW_BUS_RESET;
	}

	// check that busGenration ID is the same as started  //LM???
	// llcGetSelfIDGeneration();

	// clear BR and SelfIdComplete Bits
	if ((hResult = llcIntEventClearBusReset(LLC_INT_STATE_PROCESS_NORMAL)) != NO_ERROR)
	{
		return hResult;
	}

#ifdef _AVS //KBJ??? to handle problem with receiver stopping
	avsRxFlush();
#endif //_AVS

    return hResult;
}

BOOL lhlBriHandlesAreValid(void)
{
	return bHandlesValid;
}
/*********************************************************
	Because nodeID's, can change across bus resets, a Handle is
	used to associate a WWUID with a nodeID.

	When transactions are initiated using Node Handles, this
	must be called during bus reset processing in order to scan 
	the bus for maintaining	the Handles across bus resets.
*/
HRESULT briScanBus(void)
{
	HRESULT	hResult = NO_ERROR;
	BOOL	bDuringCriticalBR = FALSE;

	// scan for node info blocks, and cache them locally
	if ((hResult = briBuildNodeInfos()) != NO_ERROR)
	{
		return hResult;
	}

	if (lhlBriNewBusResetDetected(bDuringCriticalBR))
	{
		return E_BRI_NEW_BUS_RESET;
	}

	// fix up the handles using bus info block info, reassociate handles with WWUIDs
	if ((hResult = briFixupHandles()) != NO_ERROR)
	{
		return hResult;
	}

	if (lhlBriNewBusResetDetected(bDuringCriticalBR))
	{
		return E_BRI_NEW_BUS_RESET;
	}

	bHandlesValid = TRUE;

	return NO_ERROR;
}

/*********************************************************
	called when an bus reset process is started (selfId complete)
	Part2: BR non-critical part (after clearing BR and SelfIdComplete Bits)
*/
HRESULT lhlBriHandleBusResetPart2(void)
{
	HRESULT			hResult = NO_ERROR;
	BOOL			bDuringCriticalBR = FALSE;

	// verify self ids and that own node id is valid
	if ((hResult = briProcessSelfId()) != NO_ERROR)
	{
		if (hResult == E_NCI_NO_CONTENDERS) 
		{
			if (csrIsCMCCapable())
			{
				lhlBriForceRoot();
				return hResult;
			} 
		} 
		else
		{
			lhlBriForceBusReset();
			return hResult;
		}
	}
	if (lhlBriNewBusResetDetected(bDuringCriticalBR))
	{
		return E_BRI_NEW_BUS_RESET;
	}

#ifdef _AVS //LM??? updating Tx plugs sourceID if changed af bus reset
	avsTxCheckSourceID();
#endif //_AVS

	// clear pending transactions (can't be called at interrupt level (because of mutex) and call CBs)
	lhlPendingTxClear();

	// bus reset pre completion
	briBusResetPreCompletion();
	if (lhlBriNewBusResetDetected(bDuringCriticalBR))
	{
		return E_BRI_NEW_BUS_RESET;
	}

#ifdef _OPTIMIZE_BUSRESET
#else //_OPTIMIZE_BUSRESET
	if ((hResult = briScanBus()) != NO_ERROR)
	{
		if (E_BRI_NEW_BUS_RESET == hResult)
		{
			return hResult;
		}
	}

	// build the maps
	briBuildTopologyMap();
	briBuildSpeedMap();

	if (lhlBriNewBusResetDetected(bDuringCriticalBR))
	{
		return E_BRI_NEW_BUS_RESET;
	}
#endif //_OPTIMIZE_BUSRESET

	// call all completion callbacks let everyone know that bus reset is done
	briCallBusResetCompletionCBs();
	if (lhlBriNewBusResetDetected(bDuringCriticalBR))
	{
		return E_BRI_NEW_BUS_RESET;
	}

	// bus reset completion - make sure no storm was happening and clear event to get ready for a new bus reset
	briBusResetCompletion();
	if (lhlBriNewBusResetDetected(bDuringCriticalBR))
	{
		return E_BRI_NEW_BUS_RESET;
	}

#ifdef _BMC_CAPS //LM???
//	nciBMSetBusMaster();
#endif //_BMC_CAPS

#if defined (_WINMP) && (_BM)
	if (nciBMIsThisNodeBM() == FALSE)
	{
		TCTaskWait(100); //LM???
		nciBMCliCheckBMInfo(NULL);
	}
#endif //_WINMP && _BM

	return hResult;
}

/*********************************************************
	called when an bus reset interrupt occures
*/
void lhlBriIntHandler(void)
{
	HRESULT hResult = NO_ERROR;

#if 0 //LM???
	lhlBriIntHandlerBusResetGen++;
#endif

#ifdef _STATISTICS
	lhlStatistics.BrBriBRInt++;
#endif //_STATISTICS

	if (hResult == NO_ERROR)
	{
		// initial tests (e.g. check for loops)
		hResult = lhlBriBusResetIndication();
	}

	/* KBJ??? to be removed doing nothing
	if (hResult == NO_ERROR)
	{
		// restore link to post-br runtime
		hResult = lhlBriBusResetInitLink();
	}
	*/
	// Clear interrupt
	//llcLinkRegWrite(INTERRUPT_REG_CLEAR_DP, LLC_INT_BIT(LLCID_BUS_RST_RXD));

}

/*********************************************************
	called when an selfId Complete interrupt occures
*/
void lhlBriSelfIdIntHandler(void)
{
#ifdef _STATISTICS
	lhlStatistics.BrBriSIInt++;
#endif //_STATISTICS

	// Gather self ids
	if (lhlBriGetSelfIDBundle() != NO_ERROR) return;

	// Clear interrupt
	//llcLinkRegWrite(INTERRUPT_REG_CLEAR_DP, LLC_INT_BIT(LLCID_BUS_INIT_OVR));
	TCSemaphoreSignal(lhlBriSemID);
}

/*********************************************************
	Thread waiting for bus reset process
*/
void lhlBriThread(void *dummy)
{
	HRESULT hResult = NO_ERROR;
	
	UNUSED_ARG(dummy);

	DO_FOREVER
	{

		hResult = TCSemaphoreWait(lhlBriSemID);
		if (hResult != NO_ERROR)
		{
			DO_FOREVER {}
		}

#if 0 //LM???
		if (lhlBriIntHandlerBusResetGen == lhlBriHandleBusResetPartGen)
		{
			hResult = E_FAIL; //LM???
		}
		lhlBriHandleBusResetPartGen = lhlBriIntHandlerBusResetGen;
#endif

		TCSemaphoreReset (lhlBriSemID); //LM??? avoid coming in here more than ones

		hResult = lhlBriHandleBusResetPart1();	// BR critical part
		if (hResult == NO_ERROR)
		{
			hResult = lhlBriHandleBusResetPart2();	// BR non-critical part
		}
	}
}

/*********************************************************
	Create semaphores for this module
*/
HRESULT lhlBriCreateSemaphores(void)
{
	HRESULT hResult = NO_ERROR;

	// bus reset process semaphore
	hResult = TCSemaphoreOpenIRQ(&lhlBriSemID, 0);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Resume interrupt handlers for this module
*/
HRESULT lhlBriResumeIntHandlers(void)
{
	HRESULT hResult = NO_ERROR;

	// bus reset interrupt handler
	if ((hResult = llcIntEventAddCallback(LLCID_BUS_RST_RXD, lhlBriIntHandler)) != NO_ERROR)
	{
		return hResult;
	}

	// selfId complete interrupt handler
	if ((hResult = llcIntEventAddCallback(LLCID_BUS_INIT_OVR, lhlBriSelfIdIntHandler)) != NO_ERROR)
	{
		return hResult;
	}

	return hResult;
}

/*********************************************************
	Suspend interrupt handlers for this module
*/
HRESULT lhlBriSuspendIntHandlers(void)
{
	HRESULT hResult = NO_ERROR;

	// bus reset interrupt handler
	if ((hResult = llcIntEventRemoveCallback(LLCID_BUS_RST_RXD)) != NO_ERROR)
	{
		return hResult;
	}

	// selfId complete interrupt handler
	if ((hResult = llcIntEventRemoveCallback(LLCID_BUS_INIT_OVR)) != NO_ERROR)
	{
		return hResult;
	}

	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT lhlBriResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// bus reset process started
	hResult = TCTaskResume(lhlBriThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT lhlBriSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// bus reset process started
	hResult = TCTaskSuspend(lhlBriThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Create tasks for this module
*/
HRESULT lhlBriInitialize(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = lhlBriCreateSemaphores();
	if (hResult != NO_ERROR) return hResult;

	// bus reset process started
	hResult = TCTaskCreate(&lhlBriThreadID, lhlBriThreadName, lhlBriThread, TASK_PRIORITY_BRI, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}
