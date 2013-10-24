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
 * DESCRIPTION: Link Hardware Abstraction Layer service handling
				of INTERRUPT_REG_DP interrupts

	Purpose:	Handling of errors signaled from the LLC controller

				// External software reset
				LLC_RST_CMND_RX

				// Asynchronous errors
				LLC_HEADER_ERR
				LLC_TCODE_ERR
				LLC_ASY_DATA_ERR
				LLC_INC_SEC_COUNT
				LLC_CY_TOO_LONG
				LLC_CY_LOST
				
				// Isochronous errors
				LLC_ISO_TX_ERR
				LLC_ISO_RX_ERR
				LLC_DUB_EXPC_OCC_INTR				

				// LLC acesses error
				LLC_HOST_ACSS_ERR
				
				// Lost sync. with PHY
				LLC_NO_BUS_GRANT
	
	Revisions:
		created 12/17/2002 kbj

****************************************************************************/

#include "TCTasking.h"
#include "1394.h"
#include "lal.h"
#include "lhl.h"
#include "lhlARx.h"
#include "lhlATx.h"
#include "lhlBri.h"
#include "lhlCli.h"
#include "lhlService.h"
#include "llcEnvDefs.h"
#include "llcReg.h"
#include "llcInt.h"
#include "targetBoard.h"

#ifdef _AVS
#include "avs.h"
#endif

static char 		moduleName[] = __FILE__ ;

// semaphore IDs for this module
uint32					lhlServiceSemID;		// interrupt service semaphore 

// task (thread) IDs for this module
uint32					lhlServiceThreadID;		// service thread
const char*				lhlServiceThreadName = "lhlServiceThread";

static uint32			intEventRegCopy;		// Copy of OHCI intEvent register 

static uint32			cycleLostCount;			// Incremented for each cycle lost




/*********************************************************
	called when an bad packet is received
*/


/* 
// ML: The service routine should not flush anything. Reading data from the LLC
//     should only be done by ARx handler and by llcSelfID handler.

static QUADLET			badPacket[LLC_MAX_DEPTH];
static uint32			badQltAvail;

       
static void lhlFlushHeader(void)
{
	uint32 i;
	uint32 lscrReg;

	llcLinkRegRead(LCSR_REG_DP, &lscrReg);

	// Check for last bad header printed 
	// and if store bad asyn header is enabled
	if ((badQltAvail == 0) && (lscrReg & LLC_STORE_BAD_ASY_HDR)) 
	{
		llcLinkRegRead(ASY_RX_FIFO_QLETS_REG_DP, &badQltAvail);
		badQltAvail &= LLC_RX_QLT_AVAIL_MASK;

		for (i = 0; i < badQltAvail; i++)
		{
			llcLinkRegRead(ASY_RX_FIFO_REG_DP, &badPacket[i]);
		}
	}
}

static void lhlDebugHeader(void)
{
	uint32 i;
	for (i = 0; i < badQltAvail; i++)
	{
		sysDebugPrintf("Hdr[%02i] 0x%08x\r\n", i, badPacket[i]); 
	}
	badQltAvail = 0; // Ready for next bad header
}

*/


/*********************************************************
	called when an LLC_RST_CMND_RX interrupt occures
*/
static void lhlRstCmndRxInt(void)
{
	intEventRegCopy |= LLC_INT_BIT(LLCID_RST_CMND_RX);
	TCSemaphoreSignal(lhlServiceSemID);
}	

/*********************************************************
	called when an LLC_HEADER_ERR interrupt occures
*/
static void lhlHeaderErrInt(void)
{
	//lhlFlushHeader();
	intEventRegCopy |= LLC_INT_BIT(LLCID_HEADER_ERR);
	TCSemaphoreSignal(lhlServiceSemID);
}	

/*********************************************************
	called when an LLC_TCODE_ERR interrupt occures
*/
static void lhlTCodeErrInt(void)
{
	//lhlFlushHeader();
	intEventRegCopy |= LLC_INT_BIT(LLCID_TCODE_ERR);
	TCSemaphoreSignal(lhlServiceSemID);
}	

/*********************************************************
	called when an LLC_ASY_DATA_ERR interrupt occures
*/
static void lhlAsyDataErrInt(void)
{
	//lhlFlushHeader();
	intEventRegCopy |= LLC_INT_BIT(LLCID_ASY_DATA_ERR);
	TCSemaphoreSignal(lhlServiceSemID);
}	

/*********************************************************
	called when an LLC_INC_SEC_COUNT interrupt occures
*/
static void lhlIncSecCountInt(void)
{
	// Signal to service thread about error
	// toggle LED bus active each 1 sec.
	lhlServiceToggleCycleLed();
}

/*********************************************************
	called when an LLC_CY_LOST interrupt occures
*/
/*
static void lhlCyLostInt(void)
{
	if (cycleLostCount++ > 500)
	{
		intEventRegCopy |= LLC_INT_BIT(LLCID_CY_LOST);
		TCSemaphoreSignal(lhlServiceSemID);
		cycleLostCount = 0; 
	}
}
*/
/*********************************************************
	called when an LLC_CY_TOO_LONG interrupt occures
*/
/*
static void lhlCyTooLongInt(void)
{
	// Signal to service thread about error
	intEventRegCopy |= LLC_INT_BIT(LLCID_CY_TOO_LONG);
	TCSemaphoreSignal(lhlServiceSemID);
}
*/
/*********************************************************
	called when an LLC_ISO_TX_ERR interrupt occures
*/
/*
static void lhlIsoTxErrInt(void)
{
	intEventRegCopy |= LLC_INT_BIT(LLCID_ISO_TX_ERR);
	TCSemaphoreSignal(lhlServiceSemID);
}	
*/
/*********************************************************
	called when an LLC_ISO_RX_ERR interrupt occures
*/
/*
static void lhlIsoRxErrInt(void)
{
	intEventRegCopy |= LLC_INT_BIT(LLCID_ISO_RX_ERR);
	TCSemaphoreSignal(lhlServiceSemID);
}	
*/
/*********************************************************
	called when an LLC_DUB_EXPC_OCC_INTR interrupt occures
*/
/*
static void lhlDubExpcOccInt(void)
{
	intEventRegCopy |= LLC_INT_BIT(LLCID_DUB_EXPC_OCC_INTR);
	TCSemaphoreSignal(lhlServiceSemID);
}
*/

/*********************************************************
	called when an LLC_HOST_ACSS_ERR interrupt occures
*/
static uint32 accErrReg;

static void lhlHostAcssErrInt(void)
{
	intEventRegCopy |= LLC_INT_BIT(LLCID_HOST_ACSS_ERR);
	llcLinkRegRead(HST_ACC_ERR_REG_DP, &accErrReg);
	TCSemaphoreSignal(lhlServiceSemID);
}

/*********************************************************
	called when an LLC_NO_BUS_GRANT interrupt occures
*/
static void lhlNoBusGrantInt(void)
{
	intEventRegCopy |= LLC_INT_BIT(LLCID_NO_BUS_GRANT);
	TCSemaphoreSignal(lhlServiceSemID);
}

/*********************************************************
	wait for service interrupts to be signaled
*/
void lhlServiceThread(void *dummy)
{
	HRESULT hResult = NO_ERROR;
	uint32 nodeIDReg;
	uint32 dupExpcStat;

	UNUSED_ARG(dummy);

	DO_FOREVER
	{
		hResult = TCSemaphoreWait(lhlServiceSemID);
		if (hResult != NO_ERROR)
		{
			DO_FOREVER {}
		}

		// LLC Errors that are only logged
		if (intEventRegCopy & LLC_INT_BIT(LLCID_HEADER_ERR)) 
		{
			intEventRegCopy &= ~LLC_INT_BIT(LLCID_HEADER_ERR);
			sysLogError(E_LLC_HEADER_ERR,__LINE__,moduleName);
			//lhlDebugHeader();
		}
		if (intEventRegCopy & LLC_INT_BIT(LLCID_TCODE_ERR)) 
		{
			intEventRegCopy &= ~LLC_INT_BIT(LLCID_TCODE_ERR);
			sysLogError(E_LLC_TCODE_ERR,__LINE__,moduleName);
			//lhlDebugHeader();
		}
		if (intEventRegCopy & LLC_INT_BIT(LLCID_ASY_DATA_ERR)) 
		{
			intEventRegCopy &= ~LLC_INT_BIT(LLCID_ASY_DATA_ERR);
			sysLogError(E_LLC_ASY_DATA_ERR,__LINE__,moduleName);
			//lhlDebugHeader();
		}
		if (intEventRegCopy & LLC_INT_BIT(LLCID_ISO_RX_ERR)) 
		{
			intEventRegCopy &= ~LLC_INT_BIT(LLCID_ISO_RX_ERR);
#ifdef _AVS //KBJ???, to handle problem with receiver stopping
			avsRxFlush();
#endif
			sysLogError(E_LLC_ISO_RX_ERR,__LINE__,moduleName);
		}
		if (intEventRegCopy & LLC_INT_BIT(LLCID_ISO_TX_ERR)) 
		{
			intEventRegCopy &= ~LLC_INT_BIT(LLCID_ISO_TX_ERR);
			sysLogError(E_LLC_ISO_TX_ERR,__LINE__,moduleName);
		}
		if (intEventRegCopy & LLC_INT_BIT(LLCID_DUB_EXPC_OCC_INTR)) 
		{
			intEventRegCopy &= ~LLC_INT_BIT(LLCID_DUB_EXPC_OCC_INTR);
			llcLinkRegRead(DUP_EXPC_STAT_REG, &dupExpcStat);
			sysDebugPrintf("Duplicate unexpected channel status 0x%08x\n\r", dupExpcStat);
		}
		if (intEventRegCopy & LLC_INT_BIT(LLCID_RST_CMND_RX)) 
		{
			intEventRegCopy &= ~LLC_INT_BIT(LLCID_RST_CMND_RX);
			lhlServiceResetLinkController();
		}
		if (intEventRegCopy & LLC_INT_BIT(LLCID_CY_LOST)) 
		{
			intEventRegCopy &= ~LLC_INT_BIT(LLCID_CY_LOST);
			sysLogError(E_LLC_CY_LOST, __LINE__, moduleName);
#if 0 //ML, this is not a good idea
			lhlBriForceBusReset();
#endif
		}
		if (intEventRegCopy & LLC_INT_BIT(LLCID_CY_TOO_LONG)) 
		{
			intEventRegCopy &= ~LLC_INT_BIT(LLCID_CY_TOO_LONG);
			/*
				if this node is root, must reset and then set LLC_CMSTR_EN bit 
				to reenable cycle master function
			*/
			llcLinkRegRead(ND_ID_REG_DP, &nodeIDReg);
			if (nodeIDReg & LLC_ROOT)
			{
				llcSetCycleMaster(TRUE);
				SYS_DEBUG(SYSDEBUG_TRACE_BUSRESET, "node is root, reenabled cyclemaster\n\r");
			}
			sysLogError(E_LLC_CY_TOO_LONG, __LINE__, moduleName);
		}
		if (intEventRegCopy & LLC_INT_BIT(LLCID_NO_BUS_GRANT)) 
		{
			intEventRegCopy &= ~LLC_INT_BIT(LLCID_NO_BUS_GRANT);
			sysLogError(E_LLC_NO_BUS_GRANT, __LINE__, moduleName);
			//KBJ??? action 
			// Reset PHY to Link interface
			// ML: i think this could happen as a rece condition between bus reset and packet tx.
			
			//sysDebugPrintf("Reset PHY to Link interface\r\n");
			//llcLinkSetBits(LPS_REG_DP, LLC_HOST_LN2P_RESET);
		}
		if (intEventRegCopy & LLC_INT_BIT(LLCID_HOST_ACSS_ERR)) 
		{
			intEventRegCopy &= ~LLC_INT_BIT(LLCID_HOST_ACSS_ERR);
			sysLogError(E_LLC_HOST_ACSS_ERR, __LINE__, moduleName);
			//llcLinkRegRead(HST_ACC_ERR_REG_DP, &accErrReg);
			//sysDebugPrintf("Host access error 0x%x\n\r", accErrReg);
		}
	}
}


/*********************************************************
	Create semaphores for this module
*/
HRESULT lhlServiceCreateSemaphores(void)
{
	HRESULT hResult = NO_ERROR;

	// transmit request packet done
	hResult = TCSemaphoreOpenIRQ(&lhlServiceSemID, 0);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Resume interrupt handlers for this module
*/
HRESULT lhlServiceResumeIntHandlers(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = llcIntEventAddCallback(LLCID_RST_CMND_RX, lhlRstCmndRxInt);
	if (hResult != NO_ERROR) return hResult;

	hResult = llcIntEventAddCallback(LLCID_HEADER_ERR, lhlHeaderErrInt);
	if (hResult != NO_ERROR) return hResult;

	hResult = llcIntEventAddCallback(LLCID_TCODE_ERR, lhlTCodeErrInt);
	if (hResult != NO_ERROR) return hResult;

	hResult = llcIntEventAddCallback(LLCID_ASY_DATA_ERR, lhlAsyDataErrInt);
	if (hResult != NO_ERROR) return hResult;
	
	hResult = llcIntEventAddCallback(LLCID_INC_SEC_COUNT, lhlIncSecCountInt);
	if (hResult != NO_ERROR) return hResult;

/* don't enable these interrups until the event holdoff is implemented
	hResult = llcIntEventAddCallback(LLCID_CY_TOO_LONG, lhlCyTooLongInt);
	if (hResult != NO_ERROR) return hResult;

	hResult = llcIntEventAddCallback(LLCID_CY_LOST, lhlCyLostInt);
	if (hResult != NO_ERROR) return hResult;

	hResult = llcIntEventAddCallback(LLCID_ISO_TX_ERR, lhlIsoTxErrInt);
	if (hResult != NO_ERROR) return hResult;
	
	hResult = llcIntEventAddCallback(LLCID_ISO_RX_ERR, lhlIsoRxErrInt);
	if (hResult != NO_ERROR) return hResult;

	hResult = llcIntEventAddCallback(LLCID_DUB_EXPC_OCC_INTR, lhlDubExpcOccInt);
	if (hResult != NO_ERROR) return hResult;
*/

	// Enable intr generation due to duplicate channel or unexpected channel
	llcLinkRegWrite(DUP_EXPC_STAT_REG, LLC_BIT16); //KBJ??? impl. if IRM | LLC_BIT17);
	
	hResult = llcIntEventAddCallback(LLCID_HOST_ACSS_ERR, lhlHostAcssErrInt);
	if (hResult != NO_ERROR) return hResult;

	hResult = llcIntEventAddCallback(LLCID_NO_BUS_GRANT, lhlNoBusGrantInt);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend interrupt handlers for this module
*/
HRESULT lhlServiceSuspendIntHandlers(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = llcIntEventRemoveCallback(LLCID_RST_CMND_RX);
	if (hResult != NO_ERROR) return hResult;

	hResult = llcIntEventRemoveCallback(LLCID_HEADER_ERR);
	if (hResult != NO_ERROR) return hResult;

	hResult = llcIntEventRemoveCallback(LLCID_TCODE_ERR);
	if (hResult != NO_ERROR) return hResult;

	hResult = llcIntEventRemoveCallback(LLCID_ASY_DATA_ERR);
	if (hResult != NO_ERROR) return hResult;

	hResult = llcIntEventRemoveCallback(LLCID_INC_SEC_COUNT);
	if (hResult != NO_ERROR) return hResult;

	hResult = llcIntEventRemoveCallback(LLCID_CY_TOO_LONG);
	if (hResult != NO_ERROR) return hResult;

	hResult = llcIntEventRemoveCallback(LLCID_CY_LOST);
	if (hResult != NO_ERROR) return hResult;

	hResult = llcIntEventRemoveCallback(LLCID_ISO_TX_ERR);
	if (hResult != NO_ERROR) return hResult;

	hResult = llcIntEventRemoveCallback(LLCID_ISO_RX_ERR);
	if (hResult != NO_ERROR) return hResult;
	
	hResult = llcIntEventRemoveCallback(LLCID_DUB_EXPC_OCC_INTR);
	if (hResult != NO_ERROR) return hResult;
	// Disable intr generation due to duplicate channel or unexpected channel
	llcLinkRegWrite(DUP_EXPC_STAT_REG, 0);

	hResult = llcIntEventRemoveCallback(LLCID_HOST_ACSS_ERR);
	if (hResult != NO_ERROR) return hResult;

	hResult = llcIntEventRemoveCallback(LLCID_NO_BUS_GRANT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT lhlServiceResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// transmit request packet done
	hResult = TCTaskResume(lhlServiceThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT lhlServiceSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// transmit request packet done
	hResult = TCTaskSuspend(lhlServiceThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Create tasks for this module
*/
void lhlServiceReset(void)
{
	intEventRegCopy = 0;
	cycleLostCount = 0;
	//badQltAvail = 0;
}

HRESULT lhlServiceInitialize(void)
{	
	HRESULT hResult = NO_ERROR;

	lhlServiceReset();
	
	hResult = lhlServiceCreateSemaphores();
	if (hResult != NO_ERROR) return hResult;

	// transmit request packet done
	hResult = TCTaskCreate(&lhlServiceThreadID, lhlServiceThreadName, lhlServiceThread, TASK_PRIORITY_SERVICE, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

// Resume all link layer interrupt handlers
HRESULT lhlServiceResumeAllIntHandlers(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = lhlBriResumeIntHandlers();
	if (hResult != NO_ERROR) return hResult;
	hResult = lhlRxResumeIntHandlers();
	if (hResult != NO_ERROR) return hResult;
	hResult = lhlTxResumeIntHandlers();
	if (hResult != NO_ERROR) return hResult;
	hResult = lhlServiceResumeIntHandlers();
	if (hResult != NO_ERROR) return hResult;
	
	return hResult;
}

// Suspend all link layer interrupt handlers
HRESULT lhlServiceSuspendAllIntHandlers(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = lhlBriSuspendIntHandlers();
	if (hResult != NO_ERROR) return hResult;
	hResult = lhlRxSuspendIntHandlers();
	if (hResult != NO_ERROR) return hResult;
	hResult = lhlTxSuspendIntHandlers();
	if (hResult != NO_ERROR) return hResult;
	hResult = lhlServiceSuspendIntHandlers();
	if (hResult != NO_ERROR) return hResult;
	
	return hResult;
}

// Suspend all interrupts and disable all masks in interrupt reigster
HRESULT lhlServiceDisableLinkController(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = llcIntEventDisableMask();
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlServiceSuspendAllIntHandlers();
	if (hResult != NO_ERROR) return hResult;

	hResult = llcBusResetInitialize();		// Stop DMA receiver and transmitter
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}
	
// Suspend all interrupts performs software reset of OHCI controller 
// Then resume all interrupts and force bus reset
HRESULT lhlServiceResetLinkController(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = lhlServiceSuspendAllIntHandlers();
	if (hResult != NO_ERROR) return hResult;
	
	llcSoftwareReset();

	hResult = lhlServiceResumeAllIntHandlers();
	if (hResult != NO_ERROR) return hResult;
	
	hResult = lhlBriForceBusReset(); 

	return hResult;
}

// LED Helper Functions

static BOOL	bInToggleBRLed = FALSE;

void lhlServiceToggleLedMode(void)
{
	//LED_TOGGLE(LLC_CYCLE_LED_ID);
}

void lhlServiceToggleCycleLed(void)
{
	if (lhlServiceInToggleBRLed() == FALSE)
	{
		lhlServiceToggleLedMode();
	}
}

void lhlServiceToggleBRLed(void)
{
	lhlServiceToggleLedMode();
}

BOOL lhlServiceInToggleBRLed(void)
{
	return bInToggleBRLed;
}

void lhlServiceEnterToggleBRLed(void)
{
	bInToggleBRLed = TRUE;
}

void lhlServiceLeaveToggleBRLed(void)
{
	bInToggleBRLed = FALSE;
}

