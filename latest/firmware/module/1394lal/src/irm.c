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
 * DESCRIPTION: Isochronous Resource Manager IRM

	This module implements the Isochronous Resource Manager functions

	Revisions:
		created 09/17/2002 lm

****************************************************************************/
#ifdef _IRM

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "irm.h"
#include "irmCli.h"
#include "lal.h"
#include "csr.h"
#include "bri.h"

HRESULT	irmAllocateSpecifiedChannel(uint32 *channel)
{
	HRESULT		hResult = NO_ERROR;

	if (*channel == IRM_ANY_AVAIL_ISOCH_CHANNEL)
	{
		hResult = irmAllocateNextAvailableChannel(channel);
	}
	else
	{
		hResult = irmAllocateChannel(*channel);
	}

	return hResult;
}

HRESULT irmAllocateNextAvailableChannel(uint32 *channel)
{
	HRESULT			hResult = NO_ERROR;
	uint32			newchannel = 0;
	uint32			maxretry = 3;
	
	while (maxretry--)	// retry a few times in case someone got in between our lock request
	{
		hResult = irmFindChannelAvailable(&newchannel);
		if (hResult != NO_ERROR) break;

		hResult = irmAllocateChannel(newchannel);
		if (hResult == NO_ERROR)
		{
			*channel = newchannel;
			break;
		}
	}
	
	return hResult;
}

HRESULT irmFindChannelAvailable(uint32 *channel)
{
	HRESULT			hResult = NO_ERROR;
	OFFSET_1394		offset = {CSR_SB_CH_AVAIL_LO, REGISTER_SPACE_ADDR_HI};
	uint32			buffer[2];
	uint32			newchannel = 0;
	uint32			channels = 0;
	uint32			bit = 0;
	uint32			nodeAddrIRM = 0;
	BOOL			bFound = FALSE;

	// find out IRM node address
	hResult = lalGetNodeAddrIRM(&nodeAddrIRM);
	if (hResult != NO_ERROR) return hResult;

	// read channel_avail_hi register (channels 32 to 63)
	hResult = lhlReadNodeTimeout(nodeAddrIRM, offset, 4, &buffer[0], LHL_QUEUE_PRIORITY, LHL_TX_REQ_TIMEOUT_MSECS);
	if (hResult != NO_ERROR) return hResult;

	if (buffer[0] == 0) // no available channels in the channel_avail_hi register (channels 0 to 31)
	{
		// read channel_avail_lo register (channels 0 to 31)
		offset.Low = CSR_SB_CH_AVAIL_HI;
		hResult = lhlReadNodeTimeout(nodeAddrIRM, offset, 4, &buffer[0], LHL_QUEUE_PRIORITY, LHL_TX_REQ_TIMEOUT_MSECS);
		if (hResult != NO_ERROR) return hResult;
	}

	newchannel = 0;
	if (buffer[0] != 0)
	{
		// shift right until we get an available channel
		channels = buffer[0];
		bit = 0x80000000;
		for (newchannel = 0; newchannel < 32; newchannel++)
		{
			if ((channels & bit) == bit)
			{
				bFound = TRUE;
				break;
			}
			bit >>= 1;
		}
	}
	
	if (bFound == FALSE)
	{
		hResult = E_IRM_NO_CHANNEL_AVAILABLE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// there is a free channel
	if (offset.Low == CSR_SB_CH_AVAIL_LO) // (channels 32 to 63)
	{
		newchannel += 32;
	}

	*channel = newchannel;

	return hResult;
}

HRESULT irmHandleChannel(uint32 channel, uint32 mode)
{
	HRESULT			hResult = NO_ERROR;
	OFFSET_1394		offset = {CSR_SB_CH_AVAIL_HI, REGISTER_SPACE_ADDR_HI};
	uint32			buffer[2];
	uint32			nodeAddrIRM = 0;
	uint32			bit = 0;
	uint32          busGen=0;
	uint32          newbusGen=1;

	// find out IRM node address
	hResult = lalGetNodeAddrIRM(&nodeAddrIRM);
	if (hResult != NO_ERROR) return hResult;

	if (channel < 32)							// read channel_avail_hi register (channels 0 to 31)
	{
		offset.Low  = CSR_SB_CH_AVAIL_HI;
	}
	else if (channel < 64)						// read channel_avail_lo register (channels 32 to 63)
	{
		offset.Low = CSR_SB_CH_AVAIL_LO;
		channel-=32;
	}
	else
	{
		hResult = E_IRM_CHANNEL_OUT_OF_RANGE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	//ML: Need to make sure that bus resets does not come in between
	briGetCurrentBusGeneration(&busGen);	

	hResult = lhlReadNodeTimeout(nodeAddrIRM, offset, 4, &buffer[0], LHL_QUEUE_PRIORITY, LHL_TX_REQ_TIMEOUT_MSECS);
	if (hResult != NO_ERROR) return hResult;

	briGetCurrentBusGeneration(&newbusGen);
	if (busGen != newbusGen)
	{
		//busreset during call. It can only have happened after the command succeded so the value we read
		//is stale, better return an error
		sysLogError(E_LAL_BUS_RESET_IN_PROGRESS, __LINE__, moduleName);
		return E_LAL_BUS_RESET_IN_PROGRESS;
	}


	// get the right bit cleared/set
	bit = 0x80000000 >> channel;
	switch (mode)
	{
		case IRM_ALLOCATE:
			if ((buffer[0] & bit) == 0)
			{
#if 0 //LM??? debug
				sysDebugPrintf("irmHandleChannel: channel: 0x%02x read offset%s: 0x%08x\n\r", channel, (offset.Low == CSR_SB_CH_AVAIL_HI) ? "Hi" : "Lo", buffer[0]);
#endif
				hResult = E_IRM_CHANNEL_NOT_AVAILABLE;
				//ML: not an error: sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}
			buffer[1] = buffer[0] & ~bit; 	// get the right bit cleared
			break;
		case IRM_FREE:
			if ((buffer[0] & bit) == bit)
			{
				hResult = E_IRM_CHANNEL_NOT_ALLOCATED;
				//ML: not an error: sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}
			buffer[1] = buffer[0] | bit;	// get the right bit set
			break;
	}
	//ML: need to be sure busreset does not happen over the call
	busGen = newbusGen;


	// request the new channel state change by building up the compare swap lock transaction buffer
	// buffer[0] = old channel state (arg_value)
	// buffer[1] = new channel state (data_value)
	hResult = lhlLockNodeTimeout(nodeAddrIRM, offset, 8, &buffer[0], LHL_QUEUE_PRIORITY, PB_LOCKTYPE_COMPARE_SWAP, LHL_TX_REQ_TIMEOUT_MSECS);
	if (hResult != NO_ERROR) return hResult;

	if (busGen != newbusGen)
	{
		//busreset during call. It can only have happened after the command succeded so action was not carried out
		//try again later.
		sysLogError(E_LAL_BUS_RESET_IN_PROGRESS, __LINE__, moduleName);
		return E_LAL_BUS_RESET_IN_PROGRESS;
	}

	return hResult;
}

HRESULT irmHandleBandwidth(uint32 bwu, uint32 mode)
{
	HRESULT			hResult = NO_ERROR;
	OFFSET_1394		offset = {CSR_SB_BW_AVAIL, REGISTER_SPACE_ADDR_HI};
	uint32			buffer[2];
	uint32			bw_remaining = 0;
	uint32			nodeAddrIRM = 0;
	uint32          busGen=0;
	uint32          newbusGen=1;

	// find out IRM node address
	hResult = lalGetNodeAddrIRM(&nodeAddrIRM);
	if (hResult != NO_ERROR) return hResult;

	// read bus_bw_register
	//ML: Need to make sure that bus resets does not come in between
	briGetCurrentBusGeneration(&busGen);	
	
	hResult = lhlReadNodeTimeout(nodeAddrIRM, offset, 4, &buffer[0], LHL_QUEUE_PRIORITY, LHL_TX_REQ_TIMEOUT_MSECS);
	if (hResult != NO_ERROR) return hResult;
	
	briGetCurrentBusGeneration(&newbusGen);
	if (busGen != newbusGen)
	{
		//busreset during call. It can only have happened after the command succeded so the value we read
		//is stale, better return an error
		sysLogError(E_LAL_BUS_RESET_IN_PROGRESS, __LINE__, moduleName);
		return E_LAL_BUS_RESET_IN_PROGRESS;
	}
	

	bw_remaining = (buffer[0] & BW_REMAINING_MASK);

	switch (mode)
	{
		case IRM_ALLOCATE:
			// check if bandwidth is available
			if (bw_remaining < bwu)
			{
				hResult = E_IRM_INSUFFICIENT_BANDWIDTH;
				//ML: not an error: sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}
			buffer[1] = bw_remaining - bwu;
			break;
		case IRM_FREE:
			if (bw_remaining + bwu > MAX_BWU_ISOCH)
			{
				hResult = E_FAIL;
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}
			buffer[1] = bw_remaining + bwu;
			break;
	}

	//ML: need to be sure busreset does not happen over the call
	busGen = newbusGen;
	// request the new bandwidth state change by building up the compare swap lock transaction buffer
	// buffer[0] = old bandwidth state (arg_value)
	// buffer[1] = new bandwidth state (data_value)
	hResult = lhlLockNodeTimeout(nodeAddrIRM, offset, 8, &buffer[0], LHL_QUEUE_PRIORITY, PB_LOCKTYPE_COMPARE_SWAP, LHL_TX_REQ_TIMEOUT_MSECS);
	if (hResult != NO_ERROR) return hResult;
	
	briGetCurrentBusGeneration(&newbusGen);
	if (busGen != newbusGen)
	{
		//busreset during call. It can only have happened after the command succeded so action was not carried out
		//try again later.
		sysLogError(E_LAL_BUS_RESET_IN_PROGRESS, __LINE__, moduleName);
		return E_LAL_BUS_RESET_IN_PROGRESS;
	}
	
	return hResult;
}

HRESULT irmGetBMNodeId(uint32 *nodeId)
{
	HRESULT			hResult = NO_ERROR;
	OFFSET_1394		offset = {CSR_SB_BM_ID, REGISTER_SPACE_ADDR_HI};
	uint32			buffer[2];
	uint32			nodeAddrIRM = 0;

	*nodeId = BUS_MANAGER_ID_UNDEF;

	// find out IRM node address
	hResult = lalGetNodeAddrIRM(&nodeAddrIRM);
	if (hResult != NO_ERROR) return hResult;

	// read bus manager id
	hResult = lhlReadNodeTimeout(nodeAddrIRM, offset, 4, &buffer[0], LHL_QUEUE_PRIORITY, LHL_TX_REQ_SHORT_TIMEOUT_MSECS);
	if (hResult != NO_ERROR) return hResult;

	*nodeId = buffer[0];
	
	return hResult;
}

HRESULT irmSetBMNodeId(uint32 nodeId)
{
	HRESULT			hResult = NO_ERROR;
	OFFSET_1394		offset = {CSR_SB_BM_ID, REGISTER_SPACE_ADDR_HI};
	uint32			buffer[2];
	uint32			nodeAddrIRM = 0;

	hResult = irmGetBMNodeId(&buffer[0]);
	if (hResult != NO_ERROR) return hResult;

	if (buffer[0] != BUS_MANAGER_ID_UNDEF)
	{
		if (buffer[0] != nodeId)
		{
			hResult = E_IRM_BUS_MANAGER_ALREADY_SET;
//			sysLogError(hResult, __LINE__, moduleName);
		}
		return hResult;
	}

	buffer[0] = BUS_MANAGER_ID_UNDEF;
	buffer[1] = nodeId;

	// compare/swap bus manager id

	// request the new bus manager id change by building up the compare swap lock transaction buffer
	// buffer[0] = old bus manager (arg_value)
	// buffer[1] = new bus manager (data_value)

	// find out IRM node address
	hResult = lalGetNodeAddrIRM(&nodeAddrIRM);
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlLockNodeTimeout(nodeAddrIRM, offset, 8, &buffer[0], LHL_QUEUE_PRIORITY, PB_LOCKTYPE_COMPARE_SWAP, LHL_TX_REQ_SHORT_TIMEOUT_MSECS);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT irmGetCMNodeId(uint32 *nodeId)
{
	HRESULT			hResult = NO_ERROR;
	OFFSET_1394		offset = {CSR_ARCH_STATE_SET, REGISTER_SPACE_ADDR_HI};
	uint32			buffer[2];
	uint32			nodeAddrRoot = 0;

	*nodeId = CYCLE_MASTER_ID_UNDEF;

	hResult = lalGetNodeAddrRoot(&nodeAddrRoot);
	if (hResult != NO_ERROR) return hResult;

	// read state_set id
	hResult = lhlReadNodeTimeout(nodeAddrRoot, offset, 4, &buffer[0], LHL_QUEUE_PRIORITY, LHL_TX_REQ_SHORT_TIMEOUT_MSECS);
	if (hResult != NO_ERROR) return hResult;
	
	if ((buffer[0] & CYCLE_MASTER_START))
	{
		*nodeId = (nodeAddrRoot & MASK_NODEID);
	}
	
	return hResult;
}

HRESULT irmGetIRMNodeId(uint32 *nodeId)
{
	HRESULT			hResult = NO_ERROR;
	uint32			nodeAddrIRM = 0;

	// find out IRM node address
	hResult = lalGetNodeAddrIRM(&nodeAddrIRM);
	if (hResult != NO_ERROR) return hResult;

	*nodeId = (nodeAddrIRM & MASK_NODEID);
	
	return hResult;
}

/*********************************************************
	Initialization of this module
*/
HRESULT irmInitialize(void)
{
	HRESULT			hResult = NO_ERROR;

#ifdef _CLI_TOOL_IRM
	irmCliInstallTools();
#endif //_CLI_TOOL_IRM

	return hResult;
}
#endif //_IRM

