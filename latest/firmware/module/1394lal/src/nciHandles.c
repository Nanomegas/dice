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
 * DESCRIPTION: Node Controller Interface 

	Purpose: Node Controller Interface functions for handling Handles

 	Note:  The validate function allows up to 4 self-id packets per node,
 			as described in the 1395a draft 3.0, but it assumes that the 4th
			packet (aka packet #3) will use bit0 for a 'more' bit as per
			1394-1995.

	Revisions:
		created 04/30/2002 lm
****************************************************************************/

#include "TCTasking.h"
#include "lalDefs.h"
#include "lhlCli.h"
#include "nci.h"
#include "nciBM.h"
#include "nciHandles.h"
#include "bri.h"

static char 		moduleName[] = __FILE__;

static LAL_NODE_HANDLE	gNextHandleId;

NCI_HANDLE_TABLE		nciHandles;

/*****************************************************************************

	nciHandlesInitialize

******************************************************************************/
HRESULT	nciHandlesInitialize(void)
{
	HRESULT	hResult = NO_ERROR;

	// Initialize data structures
	memset (&nciHandles, 0, sizeof(nciHandles));

	// Initialize globals
	gNextHandleId = 0x10000;

	return hResult;
}

/*****************************************************************************

	nciFixupNodeHandles

******************************************************************************/
HRESULT nciFixupNodeHandles(void)
{
	HRESULT			hResult = NO_ERROR;
	uint16			handleIndex = 0;
	uint16			nodeIndex = 0;
	uint16			numChecked = 0;
	uint16			numValidated = 0;
	BOOL			handleFixed;
	uint16			busId = 0;
	uint32			busGen = 0;
	LAL_SPEED		speed = lalSpeedDefault;

	nciHandles.bValid = FALSE;

	if (briIsSelfIDsValid() != TRUE)
	{
		hResult = E_NCI_SID_INFO_NOT_AVAILABLE;		// requested size is larger than the largest defined 
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = briGetCurrentBusGeneration(&busGen);
	if (hResult != NO_ERROR)
	{
		return hResult;
	}
	nciHandles.generation = busGen;

	hResult = briGetBusID(&busId);
	if (hResult != NO_ERROR)
	{
		return hResult;
	}

	// set all node handles to inactive and invalidate all node-based handles
	for (handleIndex=0;handleIndex<MAX_NODES_PER_BUS;handleIndex++)
	{
		if (nciHandles.handleRec[handleIndex].status != NCI_HANDLE_STATUS_UNUSED)
		{
			// nodeAddr based handles do not persist
			if (nciHandles.handleRec[handleIndex].status == NCI_HANDLE_STATUS_NODE)
			{
				// invalidate handle and set as unused
				memset(&nciHandles.handleRec[handleIndex], 0, sizeof(NCI_HANDLE_STRUCT));
			}
			else
			{
				nciHandles.handleRec[handleIndex].bActive = FALSE;
			}
		}
	}

	/*
		look for wwuid associated with previously created handles,
		if the handle was for a non-compliant device invalidate it.
	*/
	for (handleIndex=0;handleIndex<MAX_NODES_PER_BUS;handleIndex++)
	{
		if (nciHandles.handleRec[handleIndex].status != NCI_HANDLE_STATUS_UNUSED)
		{
			handleFixed = FALSE;

			if (nciHandles.handleRec[handleIndex].status == NCI_HANDLE_STATUS_WWUID)
			{
				// look for the wwuid in new bus info block cache and store nodeAddr, if no match make it inactive
				for (nodeIndex = 0; nodeIndex < briGetNumNodes(); nodeIndex++)
				{
					if ((nciHandles.handleRec[handleIndex].wwuid.hi  == briGetBusInfoBlockCacheData(nodeIndex, BIB_WWUIDHI_QUAD)) && 
						(nciHandles.handleRec[handleIndex].wwuid.low == briGetBusInfoBlockCacheData(nodeIndex, BIB_WWUIDLO_QUAD)))
					{
						nciHandles.handleRec[handleIndex].nodeAddr = (uint16) (busId | nodeIndex);
#ifdef _BM
						nciBMGetNodeSpeed(nodeIndex, &speed);
#endif //_BM
						nciHandles.handleRec[handleIndex].speed = speed;
						nciHandles.handleRec[handleIndex].status = NCI_HANDLE_STATUS_WWUID;
						nciHandles.handleRec[handleIndex].bActive = TRUE;

						numValidated++;
						handleFixed = TRUE;
						break;
					}
				}

				if (handleFixed == FALSE)
				{
					// set as inactive in case it was temporarily off-bus
					nciHandles.handleRec[handleIndex].bActive = FALSE;
				}
			}
			else
			{
				// set as unused
				memset(&nciHandles.handleRec[handleIndex], 0, sizeof(NCI_HANDLE_STRUCT));
				hResult = E_NCI_HANDLE_INVALID_ENTRY;
				sysLogError(hResult, __LINE__, moduleName);
			}

			// if all handles checked, no need to look further
			if (++numChecked > nciHandles.count)
			{
				break;
			}
		}
	}

//	nciHandles.count = numValidated;

	SYS_DEBUG(SYSDEBUG_TRACE_BUSRESET, "fixup handles done\n\r");

	return hResult;
}


/*****************************************************************************

	nciCreateHandle

*****************************************************************************/

HRESULT nciCreateHandle(uint32 wwuid_hi, uint32  wwuid_low, LAL_NODE_HANDLE* pHandle)
{
	HRESULT			hResult = NO_ERROR;
	uint16			nodeIndex, handleIndex;
	uint16			busId;
	BOOL			bHandleCreated = FALSE;
	LAL_NODE_HANDLE	hHandle = 0;
	LAL_SPEED		speed = lalSpeedDefault;
	
	*pHandle = 0;

	// find wwuid in bibCache
	for (nodeIndex = 0; nodeIndex < briGetNumNodes(); nodeIndex++)
	{
		if (((wwuid_hi == briGetBusInfoBlockCacheData(nodeIndex, BIB_WWUIDHI_QUAD)) &&
			(wwuid_low == briGetBusInfoBlockCacheData(nodeIndex, BIB_WWUIDLO_QUAD))) ||
			((wwuid_hi == 0) && (wwuid_low == BROADCAST_NODE_ADDR)))		// broadcast
		{
			// found in bibCache or broadcast, look for it already in the handle table
			for (handleIndex=0;handleIndex<MAX_NODES_PER_BUS;handleIndex++)
			{
				if ((nciHandles.handleRec[handleIndex].wwuid.hi  == wwuid_hi) &&
					(nciHandles.handleRec[handleIndex].wwuid.low == wwuid_low) )
				{
					*pHandle = nciHandles.handleRec[handleIndex].handle;
					nciHandles.handleRec[handleIndex].numInst++;
					nciHandles.handleRec[handleIndex].bActive = TRUE;
					bHandleCreated = TRUE;
					return hResult;
				}
			}

			// if not found already, create a new one
			if (bHandleCreated == FALSE)
			{
				for (handleIndex=0;handleIndex<MAX_NODES_PER_BUS;handleIndex++)
				{
					if (nciHandles.handleRec[handleIndex].status == NCI_HANDLE_STATUS_UNUSED)
					{
						if (wwuid_hi == 0)	// nodeAddr based handle
						{
							hHandle = wwuid_low;
						}
						else
						{
							hResult = nciGetUniqueHandleID(&hHandle);

							if (hResult != NO_ERROR)
							{
								hResult = E_NCI_CREATE_HANDLE_FAILED;
								sysLogError(hResult, __LINE__, moduleName);
								return hResult;
							}
						}

						if (wwuid_low == BROADCAST_NODE_ADDR)
						{
							nciHandles.handleRec[handleIndex].nodeAddr = (uint16) wwuid_low;
						}
						else
						{
							hResult = briGetBusID(&busId);
							if (hResult != NO_ERROR) return hResult;
							nciHandles.handleRec[handleIndex].nodeAddr = (uint16) (nodeIndex | busId);
						}

						nciHandles.handleRec[handleIndex].wwuid.hi = wwuid_hi;
						nciHandles.handleRec[handleIndex].wwuid.low = wwuid_low;
#ifdef _BM
						nciBMGetNodeSpeed(nodeIndex, &speed);
#endif //_BM
						nciHandles.handleRec[handleIndex].speed = speed;
						nciHandles.handleRec[handleIndex].handle = hHandle;
						nciHandles.handleRec[handleIndex].numInst++;
						nciHandles.handleRec[handleIndex].bActive = TRUE;

						if (wwuid_hi != 0)
						{
							nciHandles.handleRec[handleIndex].status = NCI_HANDLE_STATUS_WWUID;
						}
						else
						{
							nciHandles.handleRec[handleIndex].status = NCI_HANDLE_STATUS_NODE;
						}

						*pHandle = hHandle;
						bHandleCreated = TRUE;
						nciHandles.count++;
						return hResult;
					}
				}

				if (bHandleCreated == FALSE)
				{
					hResult = E_NCI_OUT_OF_HANDLES;
					sysLogError(hResult, __LINE__, moduleName);
					return hResult;
				}
			}
		}
	}

	hResult = E_NCI_CREATE_HANDLE_FAILED;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}


/*****************************************************************************

	nciIsValidHandle

*****************************************************************************/

BOOL nciIsValidHandle(LAL_NODE_HANDLE handle)
{
	uint16		handleIndex;
	BOOL		bValid = FALSE;

	for (handleIndex=0;handleIndex<MAX_NODES_PER_BUS;handleIndex++)
	{
		if (nciHandles.handleRec[handleIndex].status != NCI_HANDLE_STATUS_UNUSED)
		{
			if (handle == nciHandles.handleRec[handleIndex].handle)
			{
				if ((nciHandles.handleRec[handleIndex].numInst > 0) && 
					(nciHandles.handleRec[handleIndex].bActive == TRUE))
				{
					bValid = TRUE;
				}
				break;
			}
		}
	}

	return bValid;
}


/*****************************************************************************

	nciGetHandleFromNodeAddr

*****************************************************************************/

HRESULT nciGetHandleFromNodeAddr(uint32 nodeAddr, LAL_NODE_HANDLE* pHandle)
{
	HRESULT		hResult = NO_ERROR;
	uint16		handleIndex;
	BOOL		bFound = FALSE;

	*pHandle = 0;

	for (handleIndex=0;handleIndex<MAX_NODES_PER_BUS;handleIndex++)
	{
		if (nciHandles.handleRec[handleIndex].status != NCI_HANDLE_STATUS_UNUSED)
		{
			if (nodeAddr == nciHandles.handleRec[handleIndex].nodeAddr)
			{
				if (nciHandles.handleRec[handleIndex].bActive == TRUE) //LM???
				{
					if (nciHandles.handleRec[handleIndex].handle != 0)
					{
						*pHandle = nciHandles.handleRec[handleIndex].handle;
						bFound = TRUE;
						break;
					}
				}
			}
		}
	}

	if (bFound == FALSE)
	{
		hResult = E_NCI_HANDLE_NOT_FOUND;
	}

	return hResult;
}


/*****************************************************************************

	nciGetAddrFromHandle

*****************************************************************************/

HRESULT nciGetAddrFromHandle(LAL_NODE_HANDLE hHandle, uint32* pNodeAddr)
{
	HRESULT		hResult = NO_ERROR;
	uint16		handleIndex;
	BOOL		bFound = FALSE;

	*pNodeAddr = 0;
	
	if (hHandle != 0)
	{
		for (handleIndex=0;handleIndex<MAX_NODES_PER_BUS;handleIndex++)
		{
			if (nciHandles.handleRec[handleIndex].status != NCI_HANDLE_STATUS_UNUSED)
			{
				if (hHandle == nciHandles.handleRec[handleIndex].handle)
				{
					if (nciHandles.handleRec[handleIndex].bActive == TRUE) //LM???
					{
						*pNodeAddr = nciHandles.handleRec[handleIndex].nodeAddr;
						bFound = TRUE;
					}
					break;
				}
			}
		}
	}

	if (bFound == FALSE)
	{
		hResult = E_NCI_HANDLE_NOT_FOUND;
	}

	return hResult;
}

/*****************************************************************************

	nciCloseHandle

*****************************************************************************/

HRESULT nciCloseHandle(LAL_NODE_HANDLE hHandle)
{
	HRESULT		hResult = NO_ERROR;
	uint16		handleIndex;
	BOOL		bFound = FALSE;

	if (hHandle != 0)
	{
		for (handleIndex=0;handleIndex<MAX_NODES_PER_BUS;handleIndex++)
		{
			if (hHandle == nciHandles.handleRec[handleIndex].handle)
			{
				if (nciHandles.handleRec[handleIndex].numInst > 0)
				{
					if (--nciHandles.handleRec[handleIndex].numInst == 0)
					{
						// noone is using the handle, so invalidate it and set as unused
						memset(&nciHandles.handleRec[handleIndex], 0, sizeof(NCI_HANDLE_STRUCT));
						if (nciHandles.count > 0)
						{
							nciHandles.count--;
						}
					}
				}
				bFound = TRUE;
				break;
			}
		}
	}

	if (bFound == FALSE)
	{
		hResult = E_NCI_HANDLE_NOT_FOUND;
	}

	return hResult;
}


/*****************************************************************************

	nciGetUniqueHandleID

*****************************************************************************/

HRESULT nciGetUniqueHandleID(LAL_NODE_HANDLE* hHandle)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bUniqueHandleFound = FALSE;
	BOOL		bDuplicateHandleFound = FALSE;
	uint16		handleIndex = (uint16) gNextHandleId;
	uint32		numTries = 0;

	*hHandle = 0;

	while (bUniqueHandleFound == FALSE)
	{
		for (handleIndex = 0; handleIndex < MAX_NODES_PER_BUS; handleIndex++)
		{
			if (gNextHandleId == nciHandles.handleRec[handleIndex].handle)
			{
				bDuplicateHandleFound = TRUE;
				if (++gNextHandleId > 0xffff0000)
				{
					gNextHandleId = 0x10000;
					break;
				}
			}
		}

		if (bDuplicateHandleFound == TRUE)
		{
			if (++numTries > 0xffff0000)
			{
				hResult = E_NCI_CREATE_HANDLE_FAILED;
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}

			// reset for next try
			bDuplicateHandleFound = FALSE;
		}
		else
		{
			bUniqueHandleFound = TRUE;
		}
	}

	*hHandle = gNextHandleId;

	// increment for next request
	if (++gNextHandleId > 0xffff0000)
	{
		gNextHandleId = 0x10000;
	}

	return hResult;
}

/*****************************************************************************

	nciGetSpeed

*****************************************************************************/

HRESULT nciGetSpeed(LAL_NODE_HANDLE hHandle, LAL_SPEED *speed)
{
	HRESULT		hResult = NO_ERROR;
	uint16		handleIndex;
	BOOL		bFound = FALSE;
	
	*speed = lalSpeedDefault;

	for (handleIndex=0;handleIndex<MAX_NODES_PER_BUS;handleIndex++)
	{
		if (hHandle == nciHandles.handleRec[handleIndex].handle)
		{
			*speed = nciHandles.handleRec[handleIndex].speed;
			bFound = TRUE;
			break;
		}
	}

	if (bFound == FALSE)
	{
		hResult = E_NCI_HANDLE_NOT_FOUND;
	}

	return hResult;
}

/*****************************************************************************

	nciSetSpeed

*****************************************************************************/

HRESULT nciSetSpeed(LAL_NODE_HANDLE hHandle, LAL_SPEED speed)
{
	HRESULT		hResult = NO_ERROR;
	uint16		handleIndex;
	BOOL		bFound = FALSE;
	
	for (handleIndex=0;handleIndex<MAX_NODES_PER_BUS;handleIndex++)
	{
		if (hHandle == nciHandles.handleRec[handleIndex].handle)
		{
			nciHandles.handleRec[handleIndex].speed = speed;
			bFound = TRUE;
			break;
		}
	}

	if (bFound == FALSE)
	{
		hResult = E_NCI_HANDLE_NOT_FOUND;
	}

	return hResult;
}


/*****************************************************************************

	nciGetQueuePriority

*****************************************************************************/

HRESULT nciGetQueuePriority(LAL_NODE_HANDLE hHandle, PB_QTYPE* qType)
{
	HRESULT		hResult = NO_ERROR;
	uint16		handleIndex;
	BOOL		bFound = FALSE;
	
	for (handleIndex=0;handleIndex<MAX_NODES_PER_BUS;handleIndex++)
	{
		if (hHandle == nciHandles.handleRec[handleIndex].handle)
		{
			*qType = nciHandles.handleRec[handleIndex].qType;
			bFound = TRUE;
			break;
		}
	}

	if (bFound == FALSE)
	{
		hResult = E_NCI_HANDLE_NOT_FOUND;
	}

	return hResult;
}

/*****************************************************************************

	nciSetQueuePriority

*****************************************************************************/

HRESULT nciSetQueuePriority(LAL_NODE_HANDLE hHandle, PB_QTYPE qType)
{
	HRESULT		hResult = NO_ERROR;
	uint16		handleIndex;
	BOOL		bFound = FALSE;
	
	for (handleIndex=0;handleIndex<MAX_NODES_PER_BUS;handleIndex++)
	{
		if (hHandle == nciHandles.handleRec[handleIndex].handle)
		{
			nciHandles.handleRec[handleIndex].qType = qType;
			bFound = TRUE;
			break;
		}
	}

	if (bFound == FALSE)
	{
		hResult = E_NCI_HANDLE_NOT_FOUND;
	}

	return hResult;
}
