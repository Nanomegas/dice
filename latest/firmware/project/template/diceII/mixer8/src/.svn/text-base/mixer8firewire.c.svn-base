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
 * DESCRIPTION: mixer8

	Purpose:	mixer8
				

	Revisions:
		created 09/03/2005 AliA

****************************************************************************/

#include "TCTasking.h"
#include "TCInterrupt.h"
#include "ErrorCodes.h"
#include "mixer8.h"
#include "lal.h"
#include "mixer8firewire.h"

#ifdef _MIXER8

static HRESULT 	mixer8firewireCoreCallback(PB *packetBlock);
static BOOL		mixer8firewireCheckForWrite(uint32 RegionOffSet);
static BOOL 	mixer8firewireCheckForBulkWrite(uint32 RegionOffSet, uint32 Size);

static HRESULT mixer8firewireCoreCallback(PB *packetBlock)
{
	HRESULT hResult = NO_ERROR;
	PB_HEADER		*pHeader = NULL;
	PB_PACKETTYPE	packetType = PB_TYPE_UNDEF;
	PB_LOCKTYPE		lockType;
	OFFSET_1394		OffSetDest;
	uint32			payloadSize = 0;
	uint32			RegionOffSet = 0;	
	QUADLET			*pPayload = NULL;

	hResult = pbGetPacketHeader (packetBlock, &pHeader);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPacketType(packetBlock,&packetType);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetDestinationOffset(packetBlock, &OffSetDest);
	if (hResult != NO_ERROR) return hResult;

	RegionOffSet = OffSetDest.Low - MIXER8_BASE_START;	

	hResult = pbGetDataLen(packetBlock,&payloadSize);
	if(hResult != NO_ERROR) 
		return hResult;	

	if (packetType == PB_TYPE_WRITE_REQUEST_QUADLET)
	{
		hResult = pbGetPayload(packetBlock, (void **)&pPayload);
		if (hResult != NO_ERROR) return hResult;
		hResult = pbGetDataLen(packetBlock,&payloadSize);
		if (hResult != NO_ERROR) return hResult;
		payloadSize = 4;

		if(mixer8firewireCheckForWrite(RegionOffSet))
		{
			if (RegionOffSet == (MIXER8_OFFSET(CurrentConfiguration)))
			{
				if(mixer8CheckValidConfiguration((uint32)*pPayload))
				{
					memcpy((uint32*) ((int)&mixer8 + (int)RegionOffSet), pPayload, payloadSize);
					hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
					TCSemaphoreSignal(mixer8SemID);
				}
				else
				{
					hResult = lalReplyWriteResponse(packetBlock, RSP_DATA_ERROR, TRUE);
				}
			}
			else
			{
				memcpy((uint32*) ((int)&mixer8 + (int)RegionOffSet), pPayload, payloadSize);
				hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
				TCSemaphoreSignal(mixer8SemID);
			}
		}
		else
		{
			hResult = lalReplyWriteResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);			
		}		
	}
	if (packetType == PB_TYPE_WRITE_REQUEST)
	{
		hResult = pbGetPayload(packetBlock, (void **) &pPayload);
		if (hResult != NO_ERROR) return hResult;
		hResult = pbGetDataLen(packetBlock,&payloadSize);
		if (hResult != NO_ERROR) return hResult;

		if(mixer8firewireCheckForBulkWrite(RegionOffSet,payloadSize))
		{			
			memcpy((uint32*) ((int)&mixer8 + (int)RegionOffSet), pPayload, payloadSize);
			hResult = lalReplyWriteResponse(packetBlock, RSP_COMPLETE, TRUE);
			TCSemaphoreSignal(mixer8SemID);
		}
		else
		{
			hResult = lalReplyWriteResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);
		}
	}

	if (packetType == PB_TYPE_READ_REQUEST)
	{
		hResult = pbGetDataLen(packetBlock,&payloadSize);
		if (hResult != NO_ERROR) return hResult;

		hResult = lalReplyReadResponse(packetBlock, RSP_COMPLETE, (uint16) payloadSize,
			(uint32*) ((int)&mixer8 + (int)RegionOffSet), TRUE);
	}

	if (packetType == PB_TYPE_READ_REQUEST_QUADLET)
	{
		payloadSize = 4;
		hResult = lalReplyReadResponse(packetBlock, RSP_COMPLETE, (uint16) payloadSize,
			(uint32*) ((int)&mixer8 + (int)RegionOffSet), TRUE);
	}

	if (packetType == PB_TYPE_LOCK_REQUEST)
	{
		hResult = pbGetLockType(packetBlock, &lockType);
		if (hResult != NO_ERROR) return hResult;
		hResult = pbGetDataLen(packetBlock,&payloadSize);
		if (hResult != NO_ERROR) return hResult;
		hResult = pbGetPayload(packetBlock, (void **) &pPayload);
		if (hResult != NO_ERROR) return hResult;

		hResult = lalReplyLockResponse(packetBlock, RSP_COMPLETE, (uint16)payloadSize, pPayload, TRUE);
	}

	return hResult;
}

static BOOL mixer8firewireCheckForWrite(uint32 RegionOffSet)
{
	if (RegionOffSet == (MIXER8_OFFSET(Version)))
		return FALSE;
	else if (RegionOffSet == (MIXER8_OFFSET(SupportedConfiguration)))
		return FALSE;
	else if (RegionOffSet == (MIXER8_OFFSET(MaxAuxAvailable)))
		return FALSE;
	else if (RegionOffSet == (MIXER8_OFFSET(MaxChannelAvailable)))
		return FALSE;
	else
		return TRUE;
}

static BOOL mixer8firewireCheckForBulkWrite(uint32 RegionOffSet, uint32 Size)
{
	if ((RegionOffSet <= (MIXER8_OFFSET(Version))) && 
		((RegionOffSet + Size) >= (MIXER8_OFFSET(Version))))
		return FALSE;
	else if ((RegionOffSet <= (MIXER8_OFFSET(SupportedConfiguration))) && 
		((RegionOffSet + Size) >= (MIXER8_OFFSET(SupportedConfiguration))))
		return FALSE;
	else if ((RegionOffSet <= (MIXER8_OFFSET(MaxAuxAvailable))) && 
		((RegionOffSet + Size) >= (MIXER8_OFFSET(MaxChannelAvailable))))
		return FALSE;
	else
		return TRUE;
}

HRESULT mixer8firewireInitialize(void)
{
	HRESULT			hResult = NO_ERROR;
	OFFSET_1394  	mixer8Address, mixer8AddressHi;
	
	mixer8Address.High = MIXER8_BASE_START_HI;  
	mixer8Address.Low  = MIXER8_BASE_START;  

	mixer8AddressHi.High = MIXER8_BASE_END_HI;
	mixer8AddressHi.Low  = MIXER8_BASE_END;

	hResult = lalAddressRangeRegisterCB(&mixer8firewireCoreCallback,mixer8Address,mixer8AddressHi);

	return hResult;
}

#endif//_MIXER8
