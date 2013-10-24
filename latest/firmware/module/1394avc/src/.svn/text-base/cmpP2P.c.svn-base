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
* DESCRIPTION: Connection Management Procedures (CMP) P2P

	This module implements the connection management procedures
	Point-2-Point connections

	Revisions:
		created 09/17/2002 lm

****************************************************************************/

#ifdef _CMP

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "cmpP2P.h"
#include "cmp.h"
#include "cmpCli.h"
#include "cmpDefs.h"
#include "cmpUtils.h"
#include "targetPlugs.h"
#include "irm.h"
#include "avs.h"

CMP_P2P_CONNECTION_OWNED cmpP2PInConnectionsOwned[iPCR_MAX_ITEMS];

static HRESULT cmpP2PInConnectionGetOwned(uint32 iPCRNumber, uint32 *oPCRNumber, uint16 *oNodeAddr, CONNECTION_STATUS *status)
{
	HRESULT		hResult = NO_ERROR;

	// check range of iPCRNumber;
	if (iPCRNumber > plugsGetNumIsochInPlugs())
	{
		hResult = E_CMP_PCR_INVALID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

#ifdef _CMP_P2P_USE_NODE_HANDLE
	{
		uint32	nodeAddr = 0;

		hResult = lalGetNodeAddrFromHandle(cmpP2PInConnectionsOwned[iPCRNumber].oNodeHandle, &nodeAddr);
		if (hResult != NO_ERROR) return hResult;

		*oNodeAddr = (uint16) nodeAddr;
	}
#else //_CMP_P2P_USE_NODE_HANDLE
	*oNodeAddr = cmpP2PInConnectionsOwned[iPCRNumber].oNodeAddr;
#endif //_CMP_P2P_USE_NODE_HANDLE
	
	*oPCRNumber = cmpP2PInConnectionsOwned[iPCRNumber].oPCRNumber;
	*status = cmpP2PInConnectionsOwned[iPCRNumber].status;

	return hResult;
}

static HRESULT cmpP2PInConnectionSetOwned(uint32 iPCRNumber, uint32 oPCRNumber, uint16 oNodeAddr, CONNECTION_STATUS status)
{
	HRESULT		hResult = NO_ERROR;

	// check range of iPCRNumber;
	if (iPCRNumber > plugsGetNumIsochInPlugs())
	{
		hResult = E_CMP_PCR_INVALID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	cmpP2PInConnectionsOwned[iPCRNumber].oPCRNumber = oPCRNumber;
	cmpP2PInConnectionsOwned[iPCRNumber].status = status;

#ifdef _CMP_P2P_USE_NODE_HANDLE
	{
		LAL_NODE_HANDLE		oNodeHandle = 0;

		hResult = lalGetHandleFromNodeAddr(oNodeAddr, &oNodeHandle);
		if (hResult != NO_ERROR) return hResult;

		cmpP2PInConnectionsOwned[iPCRNumber].oNodeHandle = oNodeHandle;
	}
#else //_CMP_P2P_USE_NODE_HANDLE
	cmpP2PInConnectionsOwned[iPCRNumber].oNodeAddr = oNodeAddr;
#endif //_CMP_P2P_USE_NODE_HANDLE
	
	return hResult;
}

static HRESULT cmpP2PInConnectionGetOwnedStatus(uint32 iPCRNumber, CONNECTION_STATUS *status)
{
	HRESULT		hResult = NO_ERROR;

	// check range of iPCRNumber;
	if (iPCRNumber > plugsGetNumIsochInPlugs())
	{
		hResult = E_CMP_PCR_INVALID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*status = cmpP2PInConnectionsOwned[iPCRNumber].status;

	return hResult;
}

static HRESULT cmpP2PInConnectionSetOwnedStatus(uint32 iPCRNumber, CONNECTION_STATUS status)
{
	HRESULT		hResult = NO_ERROR;

	// check range of iPCRNumber;
	if (iPCRNumber > plugsGetNumIsochInPlugs())
	{
		hResult = E_CMP_PCR_INVALID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	cmpP2PInConnectionsOwned[iPCRNumber].status = status;

	return hResult;
}

// overlay a connection between the iPCR,iNodeAddr and the oPCR,oNodeAddr specified
static HRESULT cmpP2PConnectionOverlaying(uint32 iPCR, uint32 iPCRNumber, uint16 iNodeAddr, uint32 oPCR, uint32 oPCRNumber, uint16 oNodeAddr, uint32 channel, BOOL bHighPriority)
{
	HRESULT		hResult = NO_ERROR;
	uint32		iPCRNew = 0;
	uint32		oPCRNew = 0;
	uint32		iPCRP2PCount = 0;
	uint32		oPCRP2PCount = 0;
	
	oPCRNew = oPCR;
	cmpSetPCRChannel(&oPCRNew, channel);
	oPCRP2PCount = cmpGetPCRP2PCount(oPCR);
	oPCRP2PCount++;
	cmpSetPCRP2PCount(&oPCRNew, oPCRP2PCount);

	hResult = cmpLockoPCRWithPriority(oPCRNumber, oNodeAddr, oPCR, oPCRNew, bHighPriority);
	if (hResult != NO_ERROR) return hResult;

	iPCRNew = iPCR;
	cmpSetPCRChannel(&iPCRNew, channel);
	iPCRP2PCount = cmpGetPCRP2PCount(iPCR);
	iPCRP2PCount++;
	cmpSetPCRP2PCount(&iPCRNew, iPCRP2PCount);

	hResult = cmpLockiPCRWithPriority(iPCRNumber, iNodeAddr, iPCR, iPCRNew, bHighPriority);
	if (hResult != NO_ERROR)
	{
		cmpLockoPCRWithPriority(oPCRNumber, oNodeAddr, oPCRNew, oPCR, bHighPriority);		// break p2p connection for oPCR
	}

	return hResult;
}

// create or overlay a connection between the iPCR,iNodeAddr and the oPCR,oNodeAddr specified
static HRESULT cmpP2PConnectionEstablishing(uint32 iPCRNumber, uint16 iNodeAddr, uint32 oPCRNumber, uint16 oNodeAddr, BOOL bRestore)
{
	HRESULT		hResult = NO_ERROR;
	uint32		iPCR = 0;
	uint32		oPCR = 0;
	uint32		iPCRP2PCount = 0;
	uint32		oPCRConnCount = 0;
	uint32		channel = IRM_ANY_AVAIL_ISOCH_CHANNEL;
	uint32		bandwidth = 0;

	hResult = cmpReadiPCRWithPriority(iPCRNumber, iNodeAddr, &iPCR, TRUE);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpReadoPCRWithPriority(oPCRNumber, oNodeAddr, &oPCR, TRUE);
	if (hResult != NO_ERROR) return hResult;

	iPCRP2PCount = cmpGetPCRP2PCount(iPCR);
	oPCRConnCount = cmpGetPCRConnCount(oPCR);

	if (iPCRP2PCount != 0)		// iPCR already connected (can't establish p2p connection)
	{
		hResult = E_CMP_PCR_ALREADY_CONNECTED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (bRestore)				// channel in oPCR already specified (perform restoring of p2p connection)
	{
		channel = cmpGetPCRChannel(oPCR);
	}

	if (oPCRConnCount == 0)		// oPCR not connected (allocate IRM resources)
	{
		bandwidth = cmpGetPCRBandwidth(oPCR);

		hResult = cmpAllocateIRMResources(&channel, bandwidth);
		if (hResult != NO_ERROR) return hResult;
	}
	else if (bRestore == FALSE)	// oPCR already connected (perform overlaying of p2p connection)
	{
		channel = cmpGetPCRChannel(oPCR);
	}

	//LM??? retry
	hResult = cmpP2PConnectionOverlaying(iPCR, iPCRNumber, iNodeAddr, oPCR, oPCRNumber, oNodeAddr, channel, FALSE);
#ifdef _SYSDEBUG
	if (hResult != NO_ERROR)
	{
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
		{
			sysPrintCurTime();
			sysDebugPrintf("cmp: cmpP2PConnectionOverlaying failed - we should retry\n\r");
		}
	}
#endif //_SYSDEBUG

	if (hResult != NO_ERROR)
	{
		if (bandwidth) cmpDeallocateIRMResources(channel, bandwidth);
		return hResult;
	}

	return hResult;
}

// break a connection between the iPCR,iNodeAddr and the oPCR,oNodeAddr specified
static HRESULT cmpP2PConnectionBreaking(uint32 iPCRNumber, uint16 iNodeAddr, uint32 oPCRNumber, uint16 oNodeAddr, BOOL *bUnconnected)
{
	HRESULT		hResult = NO_ERROR;
	uint32		iPCR = 0;
	uint32		oPCR = 0;
	uint32		iPCRNew = 0;
	uint32		oPCRNew = 0;
	uint32		iPCRP2PCount = 0;
	uint32		oPCRP2PCount = 0;
	uint32		oPCRConnCount = 0;
	uint32		channel = 0;
	uint32		bandwidth = 0;
	BOOL		bBreakingOK = FALSE;

	hResult = cmpReadiPCR(iPCRNumber, iNodeAddr, &iPCR);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpReadoPCR(oPCRNumber, oNodeAddr, &oPCR);
	if (hResult != NO_ERROR) return hResult;

	iPCRNew = iPCR;
	oPCRNew = oPCR;

	iPCRP2PCount = cmpGetPCRP2PCount(iPCR);
	oPCRP2PCount = cmpGetPCRP2PCount(oPCR);

	if (iPCRP2PCount == 0)		// oPCR not connected (can't break p2p connection)
	{
		hResult = E_CMP_PCR_NOT_CONNECTED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (oPCRP2PCount == 0)		// oPCR not connected (just update local iPCR - connection was not established correctly)
	{
#ifdef _SYSDEBUG
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
		{
			sysPrintCurTime();
			sysDebugPrintf("cmp:couldn't break connection completely because oPCRP2PCount: 0\n\r");
		}
#endif //_SYSDEBUG
	}
	
	if (oPCRP2PCount > 0)
	{
		oPCRP2PCount--;
		cmpSetPCRP2PCount(&oPCRNew, oPCRP2PCount);

		hResult = cmpLockoPCR(oPCRNumber, oNodeAddr, oPCR, oPCRNew);
		bBreakingOK = (hResult == NO_ERROR);
	}

	if (hResult == NO_ERROR)
	{
		iPCRP2PCount--;
		cmpSetPCRP2PCount(&iPCRNew, iPCRP2PCount);
		hResult = cmpLockiPCR(iPCRNumber, iNodeAddr, iPCR, iPCRNew);
	}

	*bUnconnected = (cmpGetPCRP2PCount(iPCRNew) == 0);

	if (bBreakingOK == TRUE)
	{
		oPCRConnCount = cmpGetPCRConnCount(oPCRNew);

		if (oPCRConnCount == 0)		// deallocate 1394 resources if oPCR unconnected
		{
			bandwidth = cmpGetPCRBandwidth(oPCR);
			channel = cmpGetPCRChannel(oPCR);

			cmpDeallocateIRMResources(channel, bandwidth);
		}
	}

	return hResult;
}

HRESULT	cmpP2PInIsConnectionEstablished(uint32 iPCRNumber, uint32 oPCRNumber, uint16 oNodeAddr, BOOL *bEstablished)
{
	HRESULT				hResult = NO_ERROR;
	uint32				oPCRNumberTemp;
	uint16				oNodeAddrTemp;
	CONNECTION_STATUS	status;

	*bEstablished = FALSE;

	hResult = cmpP2PInConnectionGetOwned(iPCRNumber, &oPCRNumberTemp, &oNodeAddrTemp, &status);
	if (hResult != NO_ERROR) return hResult;
	
	if (status == CONNECTION_STATUS_CONNECTED)
	{
		if ((oPCRNumber == oPCRNumberTemp) &&
			(oNodeAddr == oNodeAddrTemp))
		{
			*bEstablished = TRUE;
		}
	}

	return hResult;
}

HRESULT cmpP2PInConnectionEstablish(uint32 iPCRNumber, uint32 oPCRNumber, uint16 oNodeAddr)
{
	HRESULT		hResult = NO_ERROR;
	uint32		iNodeAddr = 0;

	// check range of iPCRNumber;
	if (iPCRNumber > plugsGetNumIsochInPlugs())
	{
		hResult = E_CMP_PCR_INVALID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = lalGetThisNodeAddr(&iNodeAddr);
	if (hResult != NO_ERROR) return hResult;

	//LM???  retry
	hResult = cmpP2PConnectionEstablishing(iPCRNumber, (uint16) iNodeAddr, oPCRNumber, oNodeAddr, FALSE /*bRestore*/);
#ifdef _SYSDEBUG
	if (hResult != NO_ERROR)
	{
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
		{
			sysPrintCurTime();
			sysDebugPrintf("cmp: cmpP2PConnectionEstablishing failed - we should retry\n\r");
		}
	}
#endif //_SYSDEBUG

	if (hResult != NO_ERROR) return hResult;

	// update owned connections
	hResult = cmpP2PInConnectionSetOwned(iPCRNumber, oPCRNumber, oNodeAddr, CONNECTION_STATUS_CONNECTED);

	return hResult;
}

HRESULT cmpP2PInConnectionBreak(uint32 iPCRNumber, uint32 oPCRNumber, uint16 oNodeAddr)
{
	HRESULT		hResult = NO_ERROR;
	uint32		iNodeAddr = 0;
	BOOL		bUnconnected = TRUE;

	// check range of iPCRNumber;
	if (iPCRNumber > plugsGetNumIsochInPlugs())
	{
		hResult = E_CMP_PCR_INVALID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = lalGetThisNodeAddr(&iNodeAddr);
	if (hResult != NO_ERROR) return hResult;

	//LM??? retry?
	hResult = cmpP2PConnectionBreaking(iPCRNumber, (uint16) iNodeAddr, oPCRNumber, oNodeAddr, &bUnconnected);
#ifdef _SYSDEBUG
	if (hResult != NO_ERROR)
	{
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
		{
			sysPrintCurTime();
			sysDebugPrintf("cmp: cmpP2PConnectionBreaking failed - we should retry?\n\r");
		}
	}
#endif //_SYSDEBUG

	// update owned connections
	if (bUnconnected == TRUE)
	{
		cmpP2PInConnectionSetOwnedStatus(iPCRNumber, CONNECTION_STATUS_UNCONNECTED);
	}

	return hResult;
}
												
HRESULT cmpP2PInConnectionRestore(uint32 iPCRNumber, uint32 oPCRNumber, uint16 oNodeAddr)
{
	HRESULT		hResult = NO_ERROR;
	uint32		iNodeAddr = 0;

	// check range of iPCRNumber;
	if (iPCRNumber > plugsGetNumIsochInPlugs())
	{
		hResult = E_CMP_PCR_INVALID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = lalGetThisNodeAddr(&iNodeAddr);
	if (hResult != NO_ERROR) return hResult;

	//LM???  retry
	hResult = cmpP2PConnectionEstablishing(iPCRNumber, (uint16) iNodeAddr, oPCRNumber, oNodeAddr, TRUE /*bRestore*/);
#ifdef _SYSDEBUG
	if (hResult != NO_ERROR)
	{
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
		{
			sysPrintCurTime();
			sysDebugPrintf("cmp: cmpP2PConnectionRestore failed - we should retry\n\r");
		}
	}
#endif //_SYSDEBUG

	if (hResult != NO_ERROR) return hResult;

	// update owned connections
	hResult = cmpP2PInConnectionSetOwned(iPCRNumber, oPCRNumber, oNodeAddr, CONNECTION_STATUS_CONNECTED);

	return hResult;
}

HRESULT cmpP2PInConnectionResetOwned(void)
{
	HRESULT				hResult = NO_ERROR;
	uint32				iPCRNumber;
	CONNECTION_STATUS	status;

	for (iPCRNumber = 0; iPCRNumber < plugsGetNumIsochInPlugs(); iPCRNumber++)
	{
		hResult = cmpP2PInConnectionGetOwnedStatus(iPCRNumber, &status);
		
		if (hResult == NO_ERROR &&
			status == CONNECTION_STATUS_CONNECTED)
		{
			hResult = cmpP2PInConnectionSetOwnedStatus(iPCRNumber, CONNECTION_STATUS_PENDING);
		}
	}

	return hResult;
}

HRESULT cmpP2PInConnectionRestoreOwned(BOOL *bRestoreAllDone)
{
	HRESULT				hResult = NO_ERROR;
	uint32				iPCRNumber = 0;
	uint32				oPCRNumber = 0;
	uint16				oNodeAddr = 0;
	CONNECTION_STATUS	status = CONNECTION_STATUS_UNCONNECTED;

	*bRestoreAllDone = TRUE;

	for (iPCRNumber = 0; iPCRNumber < plugsGetNumIsochInPlugs(); iPCRNumber++)
	{
		hResult = cmpP2PInConnectionGetOwnedStatus(iPCRNumber, &status);
		
		if (hResult == NO_ERROR &&
			status == CONNECTION_STATUS_PENDING)
		{
#ifdef _CMP_P2P_USE_NODE_HANDLE
			// handles are already updated during bus reset
			if (hResult == NO_ERROR)
			{
				hResult = cmpP2PInConnectionGetOwned(iPCRNumber, &oPCRNumber, &oNodeAddr, &status);
			}
#else //_CMP_P2P_USE_NODE_HANDLE
			// get nodeID from CIP header of a isoch packet on specified channel in iPCR update owned connection with obtained nodeAddr
			uint32	sourceID = 0;
			uint16	busID = 0;
			BOOL	bValid = FALSE;
			bValid = avsRxGetSourceID(iPCRNumber, &sourceID);
			if (bValid == TRUE)
			{
				if (hResult == NO_ERROR)
				{
					hResult = lalGetBusID(&busID);
				}
#ifdef _SYSDEBUG
				if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
				{
					uint32	iNodeAddr = 0;
					hResult = lalGetThisNodeAddr(&iNodeAddr);
					sysPrintCurTime();
					sysDebugPrintf("cmp:avsRxGetSourceID returned sourceNodeAddr: 0x%04x, thisNodeAddr: 0x%04x\n\r", busID | sourceID, iNodeAddr);
				}
#endif //_SYSDEBUG
				if (hResult == NO_ERROR)
				{
					hResult = cmpP2PInConnectionGetOwned(iPCRNumber, &oPCRNumber, &oNodeAddr, &status);
				}
				if (hResult == NO_ERROR)
				{
					oNodeAddr = (uint16) (busID | sourceID);
					hResult = cmpP2PInConnectionSetOwned(iPCRNumber, oPCRNumber, oNodeAddr, status);
				}
			}
			else
			{
#ifdef _SYSDEBUG
				if (sysDebugIsEnabled(SYSDEBUG_TRACE_CMP))
				{
					sysPrintCurTime();
					sysDebugPrintf("cmp:avsRxGetSourceID returned FALSE\n\r");
				}
#endif //_SYSDEBUG
				hResult = E_CMP_PCR_NOT_CONNECTED;
			}
#endif //_CMP_P2P_USE_NODE_HANDLE

			if (hResult == NO_ERROR)
			{
				hResult = cmpP2PInConnectionRestore(iPCRNumber, oPCRNumber, oNodeAddr);
			}
			if (hResult != NO_ERROR)
			{
				*bRestoreAllDone = FALSE;
			}
		}
	}

	return hResult;
}
								
// clear ownership of all non-established p2p in connections previously established by us
HRESULT cmpP2PInConnectionClearOwnedNonEstablished(void)
{
	HRESULT				hResult = NO_ERROR;
	uint32				iPCRNumber;
	uint32				oPCRNumber;
	uint16				oNodeAddr;
	CONNECTION_STATUS	status;
	BOOL				bEstablished = FALSE;

	for (iPCRNumber = 0; iPCRNumber < plugsGetNumIsochInPlugs(); iPCRNumber++)
	{
		hResult = cmpP2PInConnectionGetOwnedStatus(iPCRNumber, &status);
		
		if (hResult == NO_ERROR &&
			status == CONNECTION_STATUS_PENDING)
		{
			bEstablished = FALSE;

			hResult = cmpP2PInConnectionGetOwned(iPCRNumber, &oPCRNumber, &oNodeAddr, &status);
			if (hResult == NO_ERROR)
			{
				cmpP2PInIsConnectionEstablished(iPCRNumber, oPCRNumber, oNodeAddr, &bEstablished);
			}

			// update owned connections
			if (bEstablished == FALSE)
			{
				cmpP2PInConnectionSetOwnedStatus(iPCRNumber, CONNECTION_STATUS_UNCONNECTED);
			}
		}
	}

	return hResult;
}

#endif //_CMP
