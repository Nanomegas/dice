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
 * DESCRIPTION: Utility for doing lock transactions
	
	Revisions:
		created 20/09/2002 lm

****************************************************************************/

#ifdef _1394CORE

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "lock.h"

HRESULT doLockTransaction(PB *packetBlock, uint32 *reg, uint32 *oldValue, uint32 lockTypesAllowed, uint32 *modifyMask)
{
	HRESULT			hResult = NO_ERROR;
	PB_LOCKTYPE		lockType = PB_LOCKTYPE_NONE;
	uint32			*data = NULL;
	uint32			*arg = NULL;
	uint32			*payload = NULL;
	uint32			payloadSize = 0;

	hResult = pbGetLockType(packetBlock, &lockType);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPayloadSize(packetBlock, &payloadSize);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPayload(packetBlock, (void **) &payload);
	if (hResult != NO_ERROR) return hResult;

	if (((1 << (lockType-1)) & lockTypesAllowed) == 0)
	{
		hResult = E_LOCK_TYPE_NOT_ALLOWED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	switch (payloadSize)
	{
		case 4:
			data = payload;
			break;
		case 8:
			arg = payload;
			data = payload + 1;  // bump up one quadlet
			break;
		case 16:
			arg = payload;
			data = payload + 2;
			break;
		default:
			hResult = E_AVC_PP_BADDATA;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
	}

	switch (lockType)
	{
		case PB_LOCKTYPE_COMPARE_SWAP:
			if (payloadSize == 8)
			{
				if (oldValue)
				{
					*oldValue = *reg;
				}
				if (*arg == *reg)
				{
					*reg = ((*data & *modifyMask) | (*reg & (~*modifyMask))); 
				}
			}
			else if ((payloadSize == 16) && (lockTypesAllowed & LT_ALLOW_64))
			{
				if (oldValue)
				{
					*oldValue = *reg;
					*(oldValue+4) = *(reg+4);
				}
				if ((*arg == *reg) && (*(uint32 *)(*arg+4) == *(uint32 *)(*reg +4) ))
				{
					*reg = ((*data & *modifyMask) | (*reg & (~*modifyMask)));
					// bump pointers up to next quadlet
					reg  += 4;
					data += 4;
					*reg = ((*data & *(modifyMask+4)) | (*reg & (~*(modifyMask+4))));
				}
			}
			else
            {
				hResult = E_LOCK_TYPE_NOT_ALLOWED;
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
            }
	        break;
	
		// add other lock types here as needed
		default: 
			hResult = E_LOCK_TYPE_NOT_IMPLEMENTED;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
	}

	return hResult;
}

#endif //_1394CORE
