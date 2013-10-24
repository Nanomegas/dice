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
* DESCRIPTION: Connection Management Procedures (CMP) BC

	This module implements the connection management procedures
	Broadcast connections

	Revisions:
		created 09/17/2002 lm

****************************************************************************/

#ifdef _CMP
static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "cmpBC.h"
#include "cmp.h"
#include "cmpCli.h"
#include "cmpDefs.h"
#include "cmpUtils.h"
#include "irm.h"
#include "targetPlugs.h"
#include "avs.h"

CMP_BC_CONNECTION_OWNED cmpBCInConnectionsOwned[iPCR_MAX_ITEMS];
CMP_BC_CONNECTION_OWNED cmpBCOutConnectionsOwned[oPCR_MAX_ITEMS];

static HRESULT cmpBCInConnectionGetOwnedStatus(uint32 iPCRNumber, CONNECTION_STATUS *status)
{
	HRESULT		hResult = NO_ERROR;

	// check range of iPCRNumber;
	if (iPCRNumber > plugsGetNumIsochInPlugs())
	{
		hResult = E_CMP_PCR_INVALID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*status = cmpBCInConnectionsOwned[iPCRNumber].status;

	return hResult;
}

static HRESULT cmpBCInConnectionSetOwnedStatus(uint32 iPCRNumber, CONNECTION_STATUS status)
{
	HRESULT		hResult = NO_ERROR;

	// check range of iPCRNumber;
	if (iPCRNumber > plugsGetNumIsochInPlugs())
	{
		hResult = E_CMP_PCR_INVALID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	cmpBCInConnectionsOwned[iPCRNumber].status = status;

	return hResult;
}

HRESULT	cmpBCInIsConnectionEstablished(uint32 iPCRNumber, BOOL *bEstablished)
{
	HRESULT		hResult = NO_ERROR;
	uint32		iPCR = 0;
	uint32		iPCRBCCount = 0;
	uint32		iNodeAddr = 0;

	hResult = lalGetThisNodeAddr(&iNodeAddr);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpReadiPCR(iPCRNumber, (uint16) iNodeAddr, &iPCR);
	if (hResult != NO_ERROR) return hResult;

	iPCRBCCount = cmpGetPCRBCCount(iPCR);

	*bEstablished = (iPCRBCCount > 0);

	return hResult;
}

static HRESULT cmpBCInConnectionOverlay(uint32 iPCR, uint32 iPCRNumber, uint32 iNodeAddr, uint32 channel, BOOL bHighPriority)
{
	HRESULT		hResult = NO_ERROR;
	uint32		iPCRNew = 0;

	iPCRNew = iPCR;

	cmpSetPCRChannel(&iPCRNew, channel);
	cmpSetPCRBCCount(&iPCRNew, 1);

	hResult = cmpLockiPCRWithPriority(iPCRNumber, (uint16) iNodeAddr, iPCR, iPCRNew, bHighPriority);
	if (hResult != NO_ERROR) return hResult;

	// update owned connections
	hResult = cmpBCInConnectionSetOwnedStatus(iPCRNumber, CONNECTION_STATUS_CONNECTED);

	return hResult;
}

HRESULT cmpBCInConnectionEstablish(uint32 iPCRNumber)
{
	HRESULT		hResult = NO_ERROR;
	uint32		iPCR = 0;
	uint32		iPCRBCCount = 0;
	uint32		iNodeAddr = 0;
	uint32		channel = 0;

	hResult = lalGetThisNodeAddr(&iNodeAddr);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpReadiPCR(iPCRNumber, (uint16) iNodeAddr, &iPCR);
	if (hResult != NO_ERROR) return hResult;

	iPCRBCCount = cmpGetPCRBCCount(iPCR);

	if (iPCRBCCount == 1)		// iPCR is connected (can't establish broadcast connection)
	{
		hResult = E_CMP_PCR_ALREADY_CONNECTED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	channel = cmpGetPCRChannel(iPCR);

	hResult = cmpBCInConnectionOverlay(iPCR, iPCRNumber, iNodeAddr, channel, FALSE);
	if (hResult != NO_ERROR) return hResult;

	// update owned connections
	hResult = cmpBCInConnectionSetOwnedStatus(iPCRNumber, CONNECTION_STATUS_CONNECTED);

	return hResult;
}

HRESULT cmpBCInConnectionBreak(uint32 iPCRNumber)
{
	HRESULT		hResult = NO_ERROR;
	uint32		iPCR = 0;
	uint32		iPCRNew = 0;
	uint32		iPCRBCCount = 0;
	uint32		iNodeAddr = 0;

	hResult = lalGetThisNodeAddr(&iNodeAddr);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpReadiPCR(iPCRNumber, (uint16) iNodeAddr, &iPCR);
	if (hResult != NO_ERROR) return hResult;

	iPCRBCCount = cmpGetPCRBCCount(iPCR);

	iPCRNew = iPCR;

	// update owned connections
	cmpBCInConnectionSetOwnedStatus(iPCRNumber, CONNECTION_STATUS_UNCONNECTED);

	if (iPCRBCCount == 0)		// iPCR not connected (can't break broadcast connection)
	{
		hResult = E_CMP_PCR_NOT_CONNECTED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	cmpSetPCRBCCount(&iPCRNew, 0);

	hResult = cmpLockiPCR(iPCRNumber, (uint16) iNodeAddr, iPCR, iPCRNew);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT cmpBCInConnectionRestore(uint32 iPCRNumber)
{
	HRESULT		hResult = NO_ERROR;
	uint32		iPCR = 0;
	uint32		iPCRBCCount = 0;
	uint32		iNodeAddr = 0;
	uint32		channel = 0;

	hResult = lalGetThisNodeAddr(&iNodeAddr);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpReadiPCRWithPriority(iPCRNumber, (uint16) iNodeAddr, &iPCR, TRUE);
	if (hResult != NO_ERROR) return hResult;

	iPCRBCCount = cmpGetPCRBCCount(iPCR);

	if (iPCRBCCount == 1)		// iPCR is connected (can't establish broadcast connection)
	{
		hResult = E_CMP_PCR_ALREADY_CONNECTED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	channel = cmpGetPCRChannel(iPCR);

	hResult = cmpBCInConnectionOverlay(iPCR, iPCRNumber, iNodeAddr, channel, TRUE);
	if (hResult != NO_ERROR) return hResult;

	// update owned connections
	hResult = cmpBCInConnectionSetOwnedStatus(iPCRNumber, CONNECTION_STATUS_CONNECTED);

	return hResult;
}

// mark owned connections for restoring
HRESULT cmpBCInConnectionResetOwned(void)
{
	HRESULT				hResult = NO_ERROR;
	uint32				iPCRNumber;
	CONNECTION_STATUS	status;

	for (iPCRNumber = 0; iPCRNumber < plugsGetNumIsochInPlugs(); iPCRNumber++)
	{
		hResult = cmpBCInConnectionGetOwnedStatus(iPCRNumber, &status);
		
		if (hResult == NO_ERROR &&
			status == CONNECTION_STATUS_CONNECTED)
		{
			hResult = cmpBCInConnectionSetOwnedStatus(iPCRNumber, CONNECTION_STATUS_PENDING);
		}
	}

	return hResult;
}

// restore owned connections following a bus reset (re-establish all broadcast in connections previously established by us)
HRESULT cmpBCInConnectionRestoreOwned(BOOL *bRestoreAllDone)
{
	HRESULT				hResult = NO_ERROR;
	uint32				iPCRNumber;
	CONNECTION_STATUS	status;

	*bRestoreAllDone = TRUE;

	for (iPCRNumber = 0; iPCRNumber < plugsGetNumIsochInPlugs(); iPCRNumber++)
	{
		hResult = cmpBCInConnectionGetOwnedStatus(iPCRNumber, &status);
		
		if (hResult == NO_ERROR &&
			status == CONNECTION_STATUS_PENDING)
		{
			hResult = cmpBCInConnectionRestore(iPCRNumber);
			if (hResult != NO_ERROR)
			{
				*bRestoreAllDone = FALSE;
			}
		}
	}

	return hResult;
}

// clear ownership of all non-established broadcast in connections previously established by us
HRESULT cmpBCInConnectionClearOwnedNonEstablished(void)
{
	HRESULT				hResult = NO_ERROR;
	uint32				iPCRNumber;
	CONNECTION_STATUS	status;
	BOOL				bEstablished = FALSE;

	for (iPCRNumber = 0; iPCRNumber < plugsGetNumIsochInPlugs(); iPCRNumber++)
	{
		hResult = cmpBCInConnectionGetOwnedStatus(iPCRNumber, &status);
		
		if (hResult == NO_ERROR &&
			status == CONNECTION_STATUS_PENDING)
		{
			bEstablished = FALSE;

			cmpBCInIsConnectionEstablished(iPCRNumber, &bEstablished);

			// update owned connections
			if (bEstablished == FALSE)
			{
				cmpBCInConnectionSetOwnedStatus(iPCRNumber, CONNECTION_STATUS_UNCONNECTED);
			}
		}
	}

	return hResult;
}

static HRESULT cmpBCOutConnectionGetOwned(uint32 oPCRNumber, CONNECTION_STATUS *status)
{
	HRESULT		hResult = NO_ERROR;

	// check range of oPCRNumber;
	if (oPCRNumber > plugsGetNumIsochOutPlugs())
	{
		hResult = E_CMP_PCR_INVALID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*status = cmpBCOutConnectionsOwned[oPCRNumber].status;

	return hResult;
}

static HRESULT cmpBCOutConnectionSetOwned(uint32 oPCRNumber, CONNECTION_STATUS status)
{
	HRESULT		hResult = NO_ERROR;

	// check range of oPCRNumber;
	if (oPCRNumber > plugsGetNumIsochOutPlugs())
	{
		hResult = E_CMP_PCR_INVALID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	cmpBCOutConnectionsOwned[oPCRNumber].status = status;

	return hResult;
}

HRESULT	cmpBCOutIsConnectionEstablished(uint32 oPCRNumber, BOOL *bEstablished)
{
	HRESULT		hResult = NO_ERROR;
	uint32		oPCR = 0;
	uint32		oPCRBCCount = 0;
	uint32		oNodeAddr = 0;

	hResult = lalGetThisNodeAddr(&oNodeAddr);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpReadoPCR(oPCRNumber, (uint16) oNodeAddr, &oPCR);
	if (hResult != NO_ERROR) return hResult;

	oPCRBCCount = cmpGetPCRBCCount(oPCR);

	*bEstablished = (oPCRBCCount > 0);

	return hResult;
}

static HRESULT cmpBCOutConnectionOverlay(uint32 oPCR, uint32 oPCRNumber, uint32 oNodeAddr, uint32 channel, BOOL bHighPriority)
{
	HRESULT		hResult = NO_ERROR;
	uint32		oPCRNew = 0;

	oPCRNew = oPCR;

	cmpSetPCRChannel(&oPCRNew, channel);
	cmpSetPCRBCCount(&oPCRNew, 1);

	hResult = cmpLockoPCRWithPriority(oPCRNumber, (uint16) oNodeAddr, oPCR, oPCRNew, bHighPriority);
	if (hResult != NO_ERROR) return hResult;

	// update owned connections
	hResult = cmpBCOutConnectionSetOwned(oPCRNumber, CONNECTION_STATUS_CONNECTED);

	return hResult;
}

HRESULT cmpBCOutConnectionEstablish(uint32 oPCRNumber)
{
	HRESULT		hResult = NO_ERROR;
	uint32		oPCR = 0;
	uint32		oMPR = 0;
	uint32		oPCRConnCount = 0;
	uint32		oPCRBCCount = 0;
	uint32		oNodeAddr = 0;
	uint32		channel = IRM_ANY_AVAIL_ISOCH_CHANNEL;
	uint32		bandwidth = 0;

	hResult = lalGetThisNodeAddr(&oNodeAddr);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpReadoPCR(oPCRNumber, (uint16) oNodeAddr, &oPCR);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpReadoMPR((uint16) oNodeAddr, &oMPR);
	if (hResult != NO_ERROR) return hResult;

	oPCRBCCount = cmpGetPCRBCCount(oPCR);
	oPCRConnCount = cmpGetPCRConnCount(oPCR);

	if (oPCRBCCount == 1)		// oPCR is connected (can't establish broadcast connection)
	{
		hResult = E_CMP_PCR_ALREADY_CONNECTED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (oPCRConnCount == 0)		// oPCR not connected (allocate IRM resources)
	{
		bandwidth = cmpGetPCRBandwidth(oPCR);

		// calculate channel number from oMPR broadcast channel base field and formula
		channel = cmpGetMPRBCBase(oMPR);
		if (channel < IRM_MAX_NUMBER_ISOCH_CHANNEL)
		{
			channel += oPCRNumber;
			if (channel >= IRM_MAX_NUMBER_ISOCH_CHANNEL)
			{
				channel -= IRM_MAX_NUMBER_ISOCH_CHANNEL;
			}
		}

		// if there's already a p2p connection on the oPCR either overlay bc connection on that channel or allocate new (device dependent - clarification doc)

		hResult = cmpAllocateIRMResources(&channel, bandwidth);
		if (hResult != NO_ERROR) return hResult;
	}
	else						// oPCR already connected (perform overlaying of broadcast connection)
	{
		channel = cmpGetPCRChannel(oPCR);
	}

	hResult = cmpBCOutConnectionOverlay(oPCR, oPCRNumber, oNodeAddr, channel, FALSE);

	if (hResult != NO_ERROR)
	{
		if (bandwidth) cmpDeallocateIRMResources(channel, bandwidth);
		return hResult;
	}

	// update owned connections
	hResult = cmpBCOutConnectionSetOwned(oPCRNumber, CONNECTION_STATUS_CONNECTED);

	return hResult;
}

HRESULT cmpBCOutConnectionBreak(uint32 oPCRNumber)
{
	HRESULT		hResult = NO_ERROR;
	uint32		oPCR = 0;
	uint32		oPCRNew = 0;
	uint32		oPCRBCCount = 0;
	uint32		oPCRConnCount = 0;
	uint32		oNodeAddr = 0;
	uint32		channel = 0;
	uint32		bandwidth = 0;

	hResult = lalGetThisNodeAddr(&oNodeAddr);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpReadoPCR(oPCRNumber, (uint16) oNodeAddr, &oPCR);
	if (hResult != NO_ERROR) return hResult;

	oPCRBCCount = cmpGetPCRBCCount(oPCR);

	oPCRNew = oPCR;

	if (oPCRBCCount == 0)		// oPCR not connected (can't break broadcast connection)
	{
		hResult = E_CMP_PCR_NOT_CONNECTED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	cmpSetPCRBCCount(&oPCRNew, 0);

	hResult = cmpLockoPCR(oPCRNumber, (uint16) oNodeAddr, oPCR, oPCRNew);
	if (hResult != NO_ERROR) return hResult;

	oPCRConnCount = cmpGetPCRConnCount(oPCRNew);

	if (oPCRConnCount == 0)		// deallocate 1394 resources if oPCR unconnected
	{
		bandwidth = cmpGetPCRBandwidth(oPCR);
		channel = cmpGetPCRChannel(oPCR);

		cmpDeallocateIRMResources(channel, bandwidth);
	}

	// update owned connections
	hResult = cmpBCOutConnectionSetOwned(oPCRNumber, CONNECTION_STATUS_UNCONNECTED);

	return hResult;
}

HRESULT cmpBCOutConnectionRestore(uint32 oPCRNumber)
{
	HRESULT		hResult = NO_ERROR;
	uint32		oPCR = 0;
	uint32		oPCRConnCount = 0;
	uint32		oPCRBCCount = 0;
	uint32		oNodeAddr = 0;
	uint32		channel = 0;
	uint32		bandwidth = 0;

	hResult = lalGetThisNodeAddr(&oNodeAddr);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpReadoPCRWithPriority(oPCRNumber, (uint16) oNodeAddr, &oPCR, TRUE);
	if (hResult != NO_ERROR) return hResult;

	oPCRBCCount = cmpGetPCRBCCount(oPCR);
	oPCRConnCount = cmpGetPCRConnCount(oPCR);

	if (oPCRBCCount == 1)		// oPCR is connected (can't establish broadcast connection)
	{
		hResult = E_CMP_PCR_ALREADY_CONNECTED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	channel = cmpGetPCRChannel(oPCR);

	if (oPCRConnCount == 0)		// oPCR not connected (allocate IRM resources)
	{
		bandwidth = cmpGetPCRBandwidth(oPCR);

		hResult = cmpAllocateIRMResources(&channel, bandwidth);
		if (hResult != NO_ERROR) return hResult;
	}
	else						// oPCR already connected (perform overlaying of broadcast connection)
	{
		channel = cmpGetPCRChannel(oPCR);
	}

	hResult = cmpBCOutConnectionOverlay(oPCR, oPCRNumber, oNodeAddr, channel, TRUE);

	if (hResult != NO_ERROR)
	{
		if (bandwidth) cmpDeallocateIRMResources(channel, bandwidth);
		return hResult;
	}

	// update owned connections
	hResult = cmpBCOutConnectionSetOwned(oPCRNumber, CONNECTION_STATUS_CONNECTED);

	return hResult;
}

// mark owned connections for restoring
HRESULT cmpBCOutConnectionResetOwned(void)
{
	HRESULT				hResult = NO_ERROR;
	uint32				oPCRNumber;
	CONNECTION_STATUS	status;

	for (oPCRNumber = 0; oPCRNumber < plugsGetNumIsochOutPlugs(); oPCRNumber++)
	{
		hResult = cmpBCOutConnectionGetOwned(oPCRNumber, &status);
		
		if (hResult == NO_ERROR &&
			status == CONNECTION_STATUS_CONNECTED)
		{
			hResult = cmpBCOutConnectionSetOwned(oPCRNumber, CONNECTION_STATUS_PENDING);
		}
	}

	return hResult;
}

// restore owned connections following a bus reset (re-establish all broadcast out connections previously established by us)
HRESULT cmpBCOutConnectionRestoreOwned(BOOL *bRestoreAllDone)
{
	HRESULT				hResult = NO_ERROR;
	uint32				oPCRNumber;
	CONNECTION_STATUS	status;

	*bRestoreAllDone = TRUE;

	for (oPCRNumber = 0; oPCRNumber < plugsGetNumIsochOutPlugs(); oPCRNumber++)
	{
		hResult = cmpBCOutConnectionGetOwned(oPCRNumber, &status);
		
		if (hResult == NO_ERROR &&
			status == CONNECTION_STATUS_PENDING)
		{
			hResult = cmpBCOutConnectionRestore(oPCRNumber);
			if (hResult != NO_ERROR)
			{
				*bRestoreAllDone = FALSE;
			}
		}
	}

	return hResult;
}

// clear ownership of all non-established broadcast out connections previously established by us
HRESULT cmpBCOutConnectionClearOwnedNonEstablished(void)
{
	HRESULT				hResult = NO_ERROR;
	uint32				oPCRNumber;
	CONNECTION_STATUS	status;
	BOOL				bEstablished = FALSE;

	for (oPCRNumber = 0; oPCRNumber < plugsGetNumIsochOutPlugs(); oPCRNumber++)
	{
		hResult = cmpBCOutConnectionGetOwned(oPCRNumber, &status);
		
		if (hResult == NO_ERROR &&
			status == CONNECTION_STATUS_PENDING)
		{
			bEstablished = FALSE;

			cmpBCOutIsConnectionEstablished(oPCRNumber, &bEstablished);

			// update owned connections
			if (bEstablished == FALSE)
			{
				cmpBCOutConnectionSetOwned(oPCRNumber, CONNECTION_STATUS_UNCONNECTED);
			}

		}
	}

	return hResult;
}

#endif //_CMP
