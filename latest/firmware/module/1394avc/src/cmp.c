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
* DESCRIPTION: Connection Management Procedures (CMP)

	Revisions:
		created 09/17/2002 lm

****************************************************************************/

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "lock.h"
#include "avs.h"
#include "targetPlugs.h"
#include "cmp.h"
#include "cmpCli.h"
#include "cmpDefs.h"
#include "cmpP2P.h"
#include "cmpUtils.h"
#include "lal.h"
#include "sysDebug.h"
#include "cli.h"

// task (thread) IDs for this module
uint32					cmpHandlerThreadID = 0;			// cmp handler thread
const char*				cmpHandlerThreadName = "cmpHandlerThread";

/*************************************************************************
  Callback for transactions in the PCR address range
**************************************************************************/

static uint32 cmpGetRegisterFromOffset(OFFSET_1394 offset)
{
	uint32 cmpRegister = CMP_REGISTER_UNKNOWN;

	if		(offset.Low == oMPR_OFFSET)		cmpRegister = CMP_REGISTER_OMPR;
	else if (offset.Low == iMPR_OFFSET)		cmpRegister = CMP_REGISTER_IMPR;
	else if (offset.Low >= oPCR_RANGE_LOW &&
			 offset.Low <= oPCR_RANGE_HI)	cmpRegister = CMP_REGISTER_OPCR;
	else if (offset.Low >= iPCR_RANGE_LOW &&
			 offset.Low <= iPCR_RANGE_HI)	cmpRegister = CMP_REGISTER_IPCR;

	return cmpRegister;
}

HRESULT cmpCB(PB *packetBlock)
{
	HRESULT			hResult = NO_ERROR;
	PB_PACKETTYPE	pPacketType = PB_TYPE_UNDEF;
	OFFSET_1394		offset;
	uint32			*payload = NULL;
	uint32			payloadSize = 0;
	uint32			PCRNumber = 0;
	RCODE_1394 		respCode = RSP_COMPLETE;
	uint32			respData = 0;
	PB_PACKETTYPE	respPacketType = PB_TYPE_UNDEF;
	PB_LOCKTYPE		lockType = PB_LOCKTYPE_NONE;

	hResult = pbGetPacketType(packetBlock, &pPacketType);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetLockType(packetBlock, &lockType);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetDestinationOffset(packetBlock,&offset);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPayloadSize(packetBlock, &payloadSize);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPayload(packetBlock, (void **) &payload);
	if (hResult != NO_ERROR) return hResult;

	if (pPacketType == PB_TYPE_READ_REQUEST_QUADLET)
	{
		respPacketType = PB_TYPE_READ_RESPONSE_QUADLET;

		switch (cmpGetRegisterFromOffset(offset))
		{
			case CMP_REGISTER_OMPR:		// oMPR register quadlet read request
				hResult = cmpGetoMPR(&respData);
				break;

			case CMP_REGISTER_IMPR:		// iMPR register quadlet read request
				hResult = cmpGetiMPR(&respData);
				break;
			
			case CMP_REGISTER_OPCR:		// oPCR register quadlet read request
				PCRNumber = (((offset.Low - oMPR_OFFSET) / 4 ) - 1);
				hResult = cmpGetoPCR(PCRNumber, &respData);
				break;

			case CMP_REGISTER_IPCR:		// iPCR register quadlet read request
				PCRNumber = (((offset.Low - iMPR_OFFSET) / 4 ) - 1);
				hResult = cmpGetiPCR(PCRNumber, &respData);
				break;
		}
	}
	else if (pPacketType == PB_TYPE_LOCK_REQUEST)
	{
		if (payloadSize == 8 &&							// only allow quadlet lock
			lockType == PB_LOCKTYPE_COMPARE_SWAP)		// only allow compare/swap
		{
			uint32	lockArg = payload[0];
			uint32	lockData = payload[1];

			respPacketType = PB_TYPE_LOCK_RESPONSE;

			switch (cmpGetRegisterFromOffset(offset))
			{
				case CMP_REGISTER_OMPR:		// oMPR register quadlet lock request
					hResult = cmpSetoMPR(lockArg, lockData, &respData);
					break;

				case CMP_REGISTER_IMPR:		// iMPR register quadlet lock request
					hResult = cmpSetiMPR(lockArg, lockData, &respData);
					break;
				
				case CMP_REGISTER_OPCR:		// oPCR register quadlet lock request
					PCRNumber = (((offset.Low - oMPR_OFFSET) / 4 ) - 1);
					hResult = cmpSetoPCR(PCRNumber, lockArg, lockData, &respData);
					break;

				case CMP_REGISTER_IPCR:		// iPCR register quadlet lock request
					PCRNumber = (((offset.Low - iMPR_OFFSET) / 4 ) - 1);
					hResult = cmpSetiPCR(PCRNumber, lockArg, lockData, &respData);
					break;
				}
		}
		else
		{
			respCode = RSP_TYPE_ERROR; // generate type error response
		}
  	}
	else
	{
		respCode = RSP_TYPE_ERROR;
	}

	if (hResult != NO_ERROR)
	{
		respCode = RSP_ADDRESS_ERROR;
	}

	if (respCode != RSP_COMPLETE)
	{
		// generate error response
		lalReplyErrorResponse(packetBlock, respCode, TRUE);
	}
	else if (respPacketType != PB_TYPE_UNDEF)
	{
		hResult = cmpGenerateQuadletResponse(packetBlock, respData, respPacketType);
	}

	return hResult;
}

HRESULT cmpGenerateQuadletResponse(PB *packetBlock, uint32 data, PB_PACKETTYPE packetType)
{
	HRESULT		hResult = NO_ERROR;

	switch (packetType)
	{
		case PB_TYPE_READ_RESPONSE_QUADLET:
			hResult = lalReplyReadResponse(packetBlock, RSP_COMPLETE, 4, &data, TRUE);
			break;

		case PB_TYPE_WRITE_RESPONSE:
			hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
			break;
		
		case PB_TYPE_LOCK_RESPONSE:
			hResult = lalReplyLockResponse(packetBlock, RSP_COMPLETE, 4, &data, TRUE);
			break;
		
		default:
			hResult = E_BAD_INPUT_PARAMETERS;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
	}

	return hResult;
}

static void cmpPCRInitialize(void)
{
	uint32		i;

	// initialize PCRs by updating avs from plug register and format values
	for (i = 0; i < plugsGetNumIsochOutPlugs(); i++)
	{
		cmpUpdateoPCR(i);
	}
	for (i = 0; i < plugsGetNumIsochInPlugs(); i++)
	{
		cmpUpdateiPCR(i);
	}

	cmpUpdateiMPR();
	cmpUpdateoMPR();
}

static void cmpPCRReset(void)
{
	uint32		i;

	//LM??? updating oPCR sourceID if changed af bus reset (is currently done in lhlBri)

	// set PCRs to the required post bus reset state
	// change any PLUG_ACTIVE plugs to PLUG_READY (and PLUG_SUSPENDED to PLUG_IDLE) without stopping data flow tx/rx
	for (i = 0; i < plugsGetNumIsochOutPlugs(); i++)
	{
		cmpResetoPCR(i, CMP_RESET_MARK);
	}
	for (i = 0; i < plugsGetNumIsochInPlugs(); i++)
	{
		cmpResetiPCR(i, CMP_RESET_MARK);
	}

	// set MPRs to post bus reset values
	cmpResetiMPR(iMPR_MASK_NP_EXT);
	cmpResetoMPR(oMPR_MASK_NP_EXT);
}

static HRESULT cmpPCRRestore(void)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bP2PInRestored = FALSE;
	BOOL		bBCInRestored = FALSE;
	BOOL		bBCOutRestored = FALSE;
	uint32		elapsedTime	= 0;
	uint32		elapsedTimeOld	= 0;

#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
	{
		sysPrintCurTime();
		sysDebugPrintf("cmp:restore owned connections\n\r");
	}
#endif //_SYSDEBUG

	// mark owned connections for restoring
	cmpP2PInConnectionResetOwned();
	cmpBCInConnectionResetOwned();
	cmpBCOutConnectionResetOwned();

#if 1 //LM??? testing
	// wait some time after bus reset to make transmitters update sourceIDs
	TCTaskWait(100);
#endif

	DO_FOREVER
	{
		if (bP2PInRestored == FALSE)
		{
			// re-establish all p2p in connections previously established by us
			cmpP2PInConnectionRestoreOwned(&bP2PInRestored);
		}
		if (bBCInRestored == FALSE)
		{
			// re-establish all broadcast in connections previously established by us
			cmpBCInConnectionRestoreOwned(&bBCInRestored);
		}
		if (bBCOutRestored == FALSE)
		{
			// re-establish all broadcast out connections previously established by us
			cmpBCOutConnectionRestoreOwned(&bBCOutRestored);
		}
		if (bP2PInRestored == TRUE &&
			bBCInRestored == TRUE &&
			bBCOutRestored == TRUE)
		{
			break;
		}

#ifdef _CMP_P2P_USE_NODE_HANDLE
		hResult = lalElapsedTimeSinceResetCompletion(&elapsedTime);		// time elapsed since last bus reset completion
#else //_CMP_P2P_USE_NODE_HANDLE
		hResult = lalElapsedTimeSinceResetPreCompletion(&elapsedTime);	// time elapsed since last bus reset completion
#endif //_CMP_P2P_USE_NODE_HANDLE
		if (hResult != NO_ERROR) return hResult;						// error if in new bus reset
		if (elapsedTime < elapsedTimeOld) return E_BRI_NEW_BUS_RESET;	// new bus reset detected
		elapsedTimeOld = elapsedTime;

		if (elapsedTime >= CMP_POST_BUS_RESET_WAITTIME) break;

		TCTaskWait(100);
	}

	if (bP2PInRestored == FALSE)
	{
		// clear ownership of all non-established p2p in connections previously established by us
		cmpP2PInConnectionClearOwnedNonEstablished();
	}
	if (bBCInRestored == FALSE)
	{
		// clear ownership of all non-established broadcast in connections previously established by us
		cmpBCInConnectionClearOwnedNonEstablished();
	}
	if (bBCOutRestored == FALSE)
	{
		// clear ownership of all non-established broadcast out connections previously established by us
		cmpBCOutConnectionClearOwnedNonEstablished();
	}

	return hResult;
}

static void cmpPCRShutdownNonEstablished(void)
{
	uint32		i;

	// stopping data flow tx/rx for PLUG_READY plugs (not re-established by their owners)
	for (i = 0; i < plugsGetNumIsochOutPlugs(); i++)
	{
		cmpResetoPCR(i, CMP_RESET_STOP);
	}
	for (i = 0; i < plugsGetNumIsochInPlugs(); i++)
	{
		cmpResetiPCR(i, CMP_RESET_STOP);
	}
}

/*********************************************************
	cmp handler thread
*/
void cmpHandlerThread(void *dummy)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bResetDetected = FALSE;

	UNUSED_ARG(dummy);

	cmpPCRInitialize();

	// wait for bus reset completion
	lalWaitOnResetCompletion(&bResetDetected);

	DO_FOREVER
	{
		// wait for reset indication
		lalWaitOnResetIndication(&bResetDetected);
		
#ifdef _SYSDEBUG
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
		{
			sysPrintCurTime();
			sysDebugPrintf("cmp:Bus Reset Indication\n\r");
		}
#endif //_SYSDEBUG

		cmpPCRReset();

#ifdef _SYSDEBUG
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
		{
			sysPrintCurTime();
			sysDebugPrintf("cmp:cmpPCRReset\n\r");
		}
#endif //_SYSDEBUG

#if 1 //LM??? cmp restores all owned connection automatically (not the responsibility of upper layer)
		// wait for bus reset completion
#ifdef _CMP_P2P_USE_NODE_HANDLE
		lalWaitOnResetCompletion(&bResetDetected);
#else //_CMP_P2P_USE_NODE_HANDLE
		lalWaitOnResetPreCompletion(&bResetDetected);
#endif //_CMP_P2P_USE_NODE_HANDLE

#ifdef _SYSDEBUG
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
		{
			sysPrintCurTime();
#ifdef _CMP_P2P_USE_NODE_HANDLE
			sysDebugPrintf("cmp:lalWaitOnResetCompletion\n\r");
#else //_CMP_P2P_USE_NODE_HANDLE
			sysDebugPrintf("cmp:lalWaitOnResetPreCompletion\n\r");
#endif //_CMP_P2P_USE_NODE_HANDLE
		}
#endif //_SYSDEBUG

		hResult = cmpPCRRestore();
		if (hResult != NO_ERROR) continue;
#endif
		// wait until one second has elapsed since the bus reset indication, or since another reset indication occurs
		hResult = lalWaitTimeSinceResetCompletion(CMP_POST_BUS_RESET_WAITTIME);
		if (hResult != NO_ERROR) continue;

#ifdef _SYSDEBUG
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
		{
			sysPrintCurTime();
			sysDebugPrintf("cmp:Bus Reset Indication - additional %i msecs\n\r", CMP_POST_BUS_RESET_WAITTIME);
		}
#endif //_SYSDEBUG

		cmpPCRShutdownNonEstablished();
	}
}

/*********************************************************
	Create semaphores for this module
*/
HRESULT cmpCreateSemaphores(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i;

	hResult = TCMutexOpen(&iMPRRegister.plugMutexSemID);
	if (hResult != NO_ERROR) return hResult;
	
	hResult = TCMutexOpen(&oMPRRegister.plugMutexSemID);
	if (hResult != NO_ERROR) return hResult;
	
	// setup semaphores for the input PCR's
	for (i = 0; i < plugsGetNumIsochInPlugs(); i++)
	{
		hResult = TCMutexOpen(&iPCRRegisters[i].plugMutexSemID);
		if (hResult != NO_ERROR) return hResult;
	}

	// setup semaphores for the output PCR's
	for (i = 0; i < plugsGetNumIsochOutPlugs(); i++)
	{
		hResult = TCMutexOpen(&oPCRRegisters[i].plugMutexSemID);
		if (hResult != NO_ERROR) return hResult;
	}

	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT cmpResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// cmp handler thread
	hResult = TCTaskResume(cmpHandlerThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT cmpSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// cmp handler thread
	hResult = TCTaskSuspend(cmpHandlerThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Create tasks for this module
*/
HRESULT cmpInitializeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// cmp handler thread
	hResult = TCTaskCreate(&cmpHandlerThreadID, cmpHandlerThreadName, cmpHandlerThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Initialization of this module
*/
HRESULT cmpInitialize(void)
{
	HRESULT			hResult = NO_ERROR;
	OFFSET_1394		pcrOffsetLo = {PCR_BASE_OFFSET, REGISTER_SPACE_ADDR_HI},
					pcrOffsetHi = {iPCR_RANGE_HI, REGISTER_SPACE_ADDR_HI};

	hResult = cmpCreateSemaphores();
	if (hResult != NO_ERROR) return hResult;

	hResult = lalAddressRangeRegisterCB(&cmpCB, pcrOffsetLo, pcrOffsetHi);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpResumeTasks();
	if (hResult != NO_ERROR) return hResult;

#ifdef _CLI_TOOL_CMP
	cmpCliInstallTools();
#endif //_CLI_TOOL_CMP

	return hResult;
}
