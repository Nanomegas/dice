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
 * DESCRIPTION: Link Abstraction Layer Address Range Callback Mgmt 

	Purpose:	Link Abstraction Layer 
				Address Range Callback Mgmt
	
	Revisions:
		created 04/16/2002 lm
****************************************************************************/

#include "TCTasking.h"
#include "lalAddrRng.h"

static char 		moduleName[] = __FILE__ ;

// list for routing of incoming packets to interested modules
LAL_ADDR_DISPATCH_LIST		lalAddressRangeCBs = {0};

/*
 *	Clear callbacks for transactions handled in and below the Link Abstration 
 *	Layer (LAL) and in the Node Controller Interface (NCI)
*/
HRESULT	lalAddressRangeInitialize(void)
{
	HRESULT			hResult = NO_ERROR;

	memset(&lalAddressRangeCBs, 0, sizeof(lalAddressRangeCBs));

	return hResult;
}

HRESULT lalAddressRangeRegisterCB (LAL_CALLBACK callback, OFFSET_1394 addressLow, OFFSET_1394 addressHi)
{
	HRESULT			hResult = NO_ERROR;
	LAL_CALLBACK	tCB = NULL;
	uint16			unusedRange = 0;

	if (lalAddressRangeCBs.numActiveRanges + 1 >= NUM_ALLOCATABLE_ADDR_RANGES)
	{
		hResult = E_LAL_TOO_MANY_CALLBACKS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (((int32)(addressHi.High - addressLow.High) < 0) ||
		((int32)(addressHi.Low - addressLow.Low) < 0))
	{
		hResult = E_LAL_INVALID_ADDR_RNG;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (lalAddressRangeLookup(addressLow, addressHi, &tCB) == TRUE)
	{
		// already allocated
		hResult = E_LAL_ADDR_RNG_CONFLICT;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = lalAddressRangeFindFree(&unusedRange);
	if (hResult != NO_ERROR)
	{
		return hResult;
	}

	lalAddressRangeCBs.numActiveRanges++;
	lalAddressRangeCBs.range[unusedRange].inUse = TRUE;
	lalAddressRangeCBs.range[unusedRange].AddrStart.Low = addressLow.Low;
	lalAddressRangeCBs.range[unusedRange].AddrStart.High = addressLow.High;
	lalAddressRangeCBs.range[unusedRange].AddrEnd.Low = addressHi.Low;
	lalAddressRangeCBs.range[unusedRange].AddrEnd.High = addressHi.High;
	lalAddressRangeCBs.range[unusedRange].callback = callback;

	return hResult;
}

BOOL lalAddressRangeLookup(OFFSET_1394 AddrStart, OFFSET_1394 AddrEnd, LAL_CALLBACK *callback)
{
	uint16	i;
	BOOL	bFound = FALSE;

	for (i = 0; i < NUM_ALLOCATABLE_ADDR_RANGES; i++)
	{
		if (lalAddressRangeCBs.range[i].inUse)
		{
			if ( (AddrStart.High >= lalAddressRangeCBs.range[i].AddrStart.High)
				&& (AddrEnd.High <= lalAddressRangeCBs.range[i].AddrEnd.High) )
			{
				if ( (AddrStart.Low >= lalAddressRangeCBs.range[i].AddrStart.Low)
					&& (AddrEnd.Low <= lalAddressRangeCBs.range[i].AddrEnd.Low) )
				{
					// return the callback
					*callback = lalAddressRangeCBs.range[i].callback;
					bFound = TRUE;
					break;
				}
			}
		}
	}

	return bFound;
}

HRESULT lalAddressRangeFindFree(uint16 *unusedRange)
{
	HRESULT	hResult = NO_ERROR;
	uint16	i;
	BOOL	bInUse = TRUE;
	BOOL	bFound = FALSE;

	*unusedRange = 0xff;

	for (i = 0; i < NUM_ALLOCATABLE_ADDR_RANGES; i++)
	{
		bInUse = lalAddressRangeCBs.range[i].inUse;
		if (bInUse == FALSE)
		{
			*unusedRange = i;
			bFound = TRUE;
			break;
		}
	}

	if (bFound == FALSE)
	{
		hResult = E_LAL_TOO_MANY_CALLBACKS;
		sysLogError(hResult, __LINE__, moduleName);
	}

	return hResult;
}


HRESULT lalAddressRangeRemove(OFFSET_1394 AddrStart, OFFSET_1394 AddrEnd)
{
	HRESULT	hResult = NO_ERROR;
	uint16	i;
	BOOL	bFound = FALSE;

	for (i = 0; i < NUM_ALLOCATABLE_ADDR_RANGES; i++)
	{
		if (lalAddressRangeCBs.range[i].inUse)
		{
			if ( (AddrStart.High >= lalAddressRangeCBs.range[i].AddrStart.High)
				&& (AddrEnd.High <= lalAddressRangeCBs.range[i].AddrEnd.High) )
			{
				if ( (AddrStart.Low >= lalAddressRangeCBs.range[i].AddrStart.Low)
					&& (AddrEnd.Low <= lalAddressRangeCBs.range[i].AddrEnd.Low) )
				{
					lalAddressRangeCBs.range[i].inUse = FALSE;
					lalAddressRangeCBs.range[i].AddrStart.Low = 0;
					lalAddressRangeCBs.range[i].AddrEnd.Low = 0;
					lalAddressRangeCBs.range[i].callback = NULL;
					bFound = TRUE;
					break;
				}
			}
		}
	}

	if (bFound == FALSE)
	{
		hResult = E_LAL_ADDR_RNG_NOT_FOUND;
		sysLogError(hResult, __LINE__, moduleName);
	}
	
	return hResult;
}
