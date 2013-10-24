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
 * DESCRIPTION: Isochronous Resource Manager (IRM) CLI

	This module implements the 
		
		  Isochronous Resource Manager Command Line Interface

	Revisions:
		created 04/06/2003 lm

****************************************************************************/

#ifdef _IRM
#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_IRM
#include "TCTasking.h"
#include "cli.h"
#include "csr.h"
#include "bitstring32.h"
#include "lal.h"
#include "irm.h"
#include "irmCli.h"

//static char 		moduleName[] = __FILE__ ;

HRESULT irmCliAllocateChannelBandwidth(uint32 channel, uint32 bandwidth)
{
	HRESULT			hResult = NO_ERROR;

	hResult = irmCliAllocateChannel(channel);
	if (hResult != NO_ERROR) return hResult;

	hResult = irmCliAllocateBandwidth(bandwidth);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT irmCliAllocateChannel(uint32 channel)
{
	HRESULT			hResult = NO_ERROR;

	if (channel == 64)
	{
		hResult = irmAllocateNextAvailableChannel(&channel);
		if (hResult != NO_ERROR) return hResult;
	}
	else
	{
		hResult = irmAllocateChannel(channel);
		if (hResult != NO_ERROR) return hResult;
	}

	cliPrintf("allocate channel:%i\n\r", channel);
	
	return hResult;
}

HRESULT irmCliAllocateBandwidth(uint32 bandwidth)
{
	HRESULT			hResult = NO_ERROR;

	hResult = irmAllocateBandwidth(bandwidth);
	if (hResult != NO_ERROR) return hResult;

	cliPrintf("allocate bandwidth:%i\n\r", bandwidth);
	
	return hResult;
}

HRESULT irmCliFreeChannelBandwidth(uint32 channel, uint32 bandwidth)
{
	HRESULT			hResult = NO_ERROR;

	hResult = irmCliFreeChannel(channel);
	if (hResult != NO_ERROR) return hResult;

	hResult = irmCliFreeBandwidth(bandwidth);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT irmCliFreeChannel(uint32 channel)
{
	HRESULT			hResult = NO_ERROR;

	hResult = irmFreeChannel(channel);
	if (hResult != NO_ERROR) return hResult;

	cliPrintf("free channel:%i\n\r", channel);
	
	return hResult;
}

HRESULT irmCliFreeBandwidth(uint32 bandwidth)
{
	HRESULT			hResult = NO_ERROR;

	hResult = irmFreeBandwidth(bandwidth);
	if (hResult != NO_ERROR) return hResult;

	cliPrintf("free bandwidth:%i\n\r", bandwidth);
	
	return hResult;
}

static HRESULT irmCliDisplayIRMNodeAddr(void)
{
	HRESULT			hResult = NO_ERROR;
	uint32			nodeAddr = 0;

	hResult = lalGetNodeAddrIRM(&nodeAddr);
	if (hResult != NO_ERROR) return hResult;

	cliPrintf("irm node addr: 0x%04x\n\r", nodeAddr);
	
	return hResult;
}

HRESULT irmCliDisplayInfo(void)
{
	HRESULT			hResult = NO_ERROR;

	hResult = irmCliDisplayIRMNodeAddr();
	if (hResult != NO_ERROR) return hResult;

	hResult = irmCliDisplayChannel();
	if (hResult != NO_ERROR) return hResult;

	hResult = irmCliDisplayBandwidth();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT irmCliDisplayChannel(void)
{
	HRESULT			hResult = NO_ERROR;
	OFFSET_1394		offset = {CSR_SB_CH_AVAIL_HI, REGISTER_SPACE_ADDR_HI};
	uint32			buffer[2];
	uint32			channelhi = 0;	// (channels 0 to 31)
	uint32			channello = 0;	// (channels 32 to 63)
	uint32			nodeAddrIRM = 0;
	uint32			freechannels = 0;
	uint32			channel = 0;
	uint32			reg = 0;
	uint32			bit = 0;
	uint32			channelreg = 0;

	// find out IRM node address
	hResult = lalGetNodeAddrIRM(&nodeAddrIRM);
	if (hResult != NO_ERROR) return hResult;

	// read channel_avail_hi register (channels 0 to 31)
	offset.Low  = CSR_SB_CH_AVAIL_HI;

	// read bus manager id
	hResult = lhlReadNode(nodeAddrIRM, offset, 4, &buffer[0], LHL_QUEUE_GENERAL);
	if (hResult != NO_ERROR) return hResult;
	channelhi = buffer[0];

	// read channel_avail_lo register (channels 32 to 63)
	offset.Low = CSR_SB_CH_AVAIL_LO;
	hResult = lhlReadNode(nodeAddrIRM, offset, 4, &buffer[0], LHL_QUEUE_GENERAL);
	if (hResult != NO_ERROR) return hResult;
	channello = buffer[0];

	cliPrintf("channels allocated: ");

	freechannels = 0;
	channelreg = channelhi;
	for (reg = 0; reg < 2; reg++)
	{
#if 1
		bit = 0x80000000;
#else
		bit = 0x00000001;
#endif
		for (channel = 0; channel < 32; channel++)
		{
			if ((channelreg & bit) == bit)
			{
				freechannels++;
			}
			else
			{
				cliPrintf("%i,", channel + reg * 32);
			}
#if 1
			bit >>= 1;
#else
			bit <<= 1;
#endif
		}
		channelreg = channello;
	} 

	cliPrintf("\n\r");
	cliPrintf("number of channels available: %i\n\r", freechannels);
#if 1
	cliPrintf("ch[0],..........................................................,ch[63]\n\r");
	bitstring32Display(channelhi);
	bitstring32Display(channello);
#else
	cliPrintf("register[63:0]:\n\r");
	bitstring32Display(channello);
	bitstring32Display(channelhi);
#endif
	cliPrintf("\n\r");

	return hResult;
}

HRESULT irmCliDisplayBandwidth(void)
{
	HRESULT			hResult = NO_ERROR;
	OFFSET_1394		offset = {CSR_SB_BW_AVAIL, REGISTER_SPACE_ADDR_HI};
	uint32			buffer[2];
	uint32			nodeAddrIRM = 0;
	uint32			bw = 0;

	// find out IRM node address
	hResult = lalGetNodeAddrIRM(&nodeAddrIRM);
	if (hResult != NO_ERROR) return hResult;

	// read bus_bw_register
	hResult = lhlReadNode(nodeAddrIRM, offset, 4, &buffer[0], LHL_QUEUE_GENERAL);
	if (hResult != NO_ERROR) return hResult;

	bw = (buffer[0] & BW_REMAINING_MASK);
	cliPrintf("bandwidth available:%i (%#x)\n\r", bw, bw);
	cliPrintf("register[31:0]:\n\r");
	bitstring32Display(buffer[0]);
	cliPrintf("\n\r");

	return hResult;
}

static char		irmCliDisplayInfo_UsageString[]=\
				"irm:info\n\r"\
				"use: irm:info\n\r"\
				"------------------------------------------------------------\n\r"\
				"display irm info\n\r";

static CLIDescriptor		irmCliDisplayInfo_Descriptor[]=
{
	{kCLI_Name,				(uint32)"info"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&irmCliDisplayInfo},
	{kCLI_UsageString, 		(uint32)&irmCliDisplayInfo_UsageString},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"display irm info"},
	{kCLI_GroupName,		(uint32)"irm"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static char		irmCliDisplayChannel_UsageString[]=\
				"irm:channels\n\r"\
				"use: irm:channels\n\r"\
				"------------------------------------------------------------\n\r"\
				"display irm channel info\n\r";

static CLIDescriptor		irmCliDisplayChannel_Descriptor[]=
{
	{kCLI_Name,				(uint32)"channels"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&irmCliDisplayChannel},
	{kCLI_UsageString, 		(uint32)&irmCliDisplayChannel_UsageString},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"display irm channel info"},
	{kCLI_GroupName,		(uint32)"irm"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static char		irmCliDisplayBandwidth_UsageString[]=\
				"irm:bandwidth\n\r"\
				"use: irm:bandwidth\n\r"\
				"------------------------------------------------------------\n\r"\
				"display irm bandwidth info\n\r";

static CLIDescriptor		irmCliDisplayBandwidth_Descriptor[]=
{
	{kCLI_Name,				(uint32)"bandwidth"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&irmCliDisplayBandwidth},
	{kCLI_UsageString, 		(uint32)&irmCliDisplayBandwidth_UsageString},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ModuleName,		(uint32)"display irm bandwidth info"},
	{kCLI_GroupName,		(uint32)"irm"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static char		irmCliAllocate_UsageString[]=\
				"irm:allocate\n\r"\
				"use: irm:allocate <channel> <bandwidth>\n\r"\
				"------------------------------------------------------------\n\r"\
				"allocate irm resources channel (64: next available) and bandwidth\n\r";

static CLIDescriptor		irmCliAllocate_Descriptor[]=
{
	{kCLI_Name,				(uint32)"allocate"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&irmCliAllocateChannelBandwidth},
	{kCLI_UsageString, 		(uint32)&irmCliAllocate_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ii"},
	{kCLI_ModuleName,		(uint32)"allocate irm resources"},
	{kCLI_GroupName,		(uint32)"irm"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static char		irmCliFree_UsageString[]=\
				"irm:free\n\r"\
				"use: irm:free <channel> <bandwidth>\n\r"\
				"------------------------------------------------------------\n\r"\
				"free irm resources channel and bandwidth\n\r";

static CLIDescriptor		irmCliFree_Descriptor[]=
{
	{kCLI_Name,				(uint32)"free"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&irmCliFreeChannelBandwidth},
	{kCLI_UsageString, 		(uint32)&irmCliFree_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ii"},
	{kCLI_ModuleName,		(uint32)"free irm resources"},
	{kCLI_GroupName,		(uint32)"irm"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void irmCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(irmCliDisplayInfo_Descriptor);
	hResult += cliInstallCLIDescriptor(irmCliDisplayChannel_Descriptor);
	hResult += cliInstallCLIDescriptor(irmCliDisplayBandwidth_Descriptor);
	hResult += cliInstallCLIDescriptor(irmCliAllocate_Descriptor);
	hResult += cliInstallCLIDescriptor(irmCliFree_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing irm cli tools");
	}
}

#endif //_CLI_TOOL_IRM
#endif //_CLI
#endif //_IRM
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
