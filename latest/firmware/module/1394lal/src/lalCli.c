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
 * DESCRIPTION: Link Abstraction Layer Command Line Interface

		lal CLI Tools and Variables
	
		These are used by the CLI to view lal info.

	Revisions:
		created 09/09/2002 lm

****************************************************************************/

#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_LAL
#include "TCTasking.h"
#include "lal.h"
#include "lalCli.h"
#include "cli.h"
#include "lhlReq.h"
#include "lhlMsgQueue.h"
#include "lhlService.h"

static char 		moduleName[] = __FILE__ ;

extern		LAL_ADDR_DISPATCH_LIST		lalAddressRangeCBs;

HRESULT lalCliRead(LAL_NODE_HANDLE hHandle, uint16 nodeAddr, uint16 destOffsetHi, uint32 destOffsetLow, uint16 numBytes)
{
	HRESULT		hResult = NO_ERROR;
	OFFSET_1394	destOffset;
	uint16		i;
	uint32		quad[256];

	destOffset.High = destOffsetHi;
	destOffset.Low = destOffsetLow;

	if (nodeAddr == 0 && hHandle != 0)
	{
		hResult = lalRead1394(hHandle, destOffset, numBytes, quad);
	}
	else
	{
		hResult = lalReadNode(nodeAddr, destOffset, numBytes, quad);
	}

	if (hResult != NO_ERROR)
	{
		cliOutputLine("read failed");
		return hResult;
	}

	cliOutputLine("read returned:");
	for (i = 0; i < (numBytes + 3) / 4; i++)
	{
		cliPrintf("0x%08x\n\r", quad[i]);
	}

	return hResult;
}

HRESULT lalCliRead1394(LAL_NODE_HANDLE hHandle, uint16 destOffsetHi, uint32 destOffsetLow, uint16 numBytes)
{
	return lalCliRead(hHandle, 0, destOffsetHi, destOffsetLow, numBytes);
}

HRESULT lalCliReadNode(uint16 nodeAddr, uint16 destOffsetHi, uint32 destOffsetLow, uint16 numBytes)
{
	return lalCliRead(0, nodeAddr, destOffsetHi, destOffsetLow, numBytes);
}

HRESULT lalCliWrite(LAL_NODE_HANDLE hHandle, uint16 nodeAddr, uint16 destOffsetHi, uint32 destOffsetLow, uint32 numBytes, uint32 data)
{
	HRESULT		hResult = NO_ERROR;
	OFFSET_1394	destOffset;

	destOffset.High = destOffsetHi;
	destOffset.Low = destOffsetLow;

	if (numBytes < 1 || numBytes > 4)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (nodeAddr == 0 && hHandle != 0)
	{
		hResult = lalWrite1394(hHandle, destOffset, (uint16) numBytes, &data);
	}
	else
	{
		hResult = lalWriteNode(nodeAddr, destOffset, (uint16) numBytes, &data);
	}

	if (hResult != NO_ERROR)
	{
		cliOutputLine("write failed");
		return hResult;
	}

	cliOutputLine("write done:");
	cliPrintf("0x%08x\n\r", data);

	return hResult;
}

HRESULT lalCliWrite1394(LAL_NODE_HANDLE hHandle, uint16 destOffsetHi, uint32 destOffsetLow, uint32 numBytes, uint32 data)
{
	return lalCliWrite(hHandle, 0, destOffsetHi, destOffsetLow, numBytes, data);
}

HRESULT lalCliWriteNode(uint16 nodeAddr, uint16 destOffsetHi, uint32 destOffsetLow, uint32 numBytes, uint32 data)
{
	return lalCliWrite(0, nodeAddr, destOffsetHi, destOffsetLow, numBytes, data);
}

HRESULT lalCliWriteTest(LAL_NODE_HANDLE hHandle, uint16 nodeAddr, uint16 destOffsetHi, uint32 destOffsetLow, uint16 numBytes)
{
	HRESULT		hResult = NO_ERROR;
	OFFSET_1394	destOffset;
	uint32		i;
	uint32		j;
	uint32		quad[256] = {0};


	if (numBytes > 256 * 4)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	cliOutputLine(" writing:");

	j = 0;
	for (i = 0; i < numBytes; i += 4)
	{
		if (j == 0)	// sends a test fcp commmand
		{
			quad[j] = 0x0108c000;
		}
		else if (j == 1)
		{
			quad[j] = 0;
		}
		else
		{
			quad[j] = i | ((i+1)<<8)| ((i+2)<<16)| ((i+3)<<24);
		}

		cliPrintf("0x%08x\n\r", quad[j]);
		j++;
	}

	destOffset.High = destOffsetHi;
	destOffset.Low = destOffsetLow;

	if (nodeAddr == 0 && hHandle != 0)
	{
		hResult = lalWrite1394(hHandle, destOffset, numBytes, quad);
	}
	else
	{
		hResult = lalWriteNode(nodeAddr, destOffset, numBytes, quad);
	}

	if (hResult != NO_ERROR)
	{
		cliOutputLine("write failed.");
		return hResult;
	}

	cliOutputLine("write succeeded.");

	return hResult;
}

HRESULT lalCliWriteTest1394(LAL_NODE_HANDLE hHandle, uint16 destOffsetHi, uint32 destOffsetLow, uint16 numBytes)
{
	return lalCliWriteTest(hHandle, 0, destOffsetHi, destOffsetLow, numBytes);
}

HRESULT lalCliWriteTestNode(uint16 nodeAddr, uint16 destOffsetHi, uint32 destOffsetLow, uint16 numBytes)
{
	return lalCliWriteTest(0, nodeAddr, destOffsetHi, destOffsetLow, numBytes);
}

HRESULT lalCliLock(LAL_NODE_HANDLE hHandle, uint16 nodeAddr, uint16 destOffsetHi, uint32 destOffsetLow, uint32 arg, uint32 data)
{
	HRESULT		hResult = NO_ERROR;
	OFFSET_1394	destOffset;
	uint32		lockReqPayload[2];

	lockReqPayload[0] = arg;
	lockReqPayload[1] = data;

	cliPrintf("32-bit lock - arg: 0x%08x, data: 0x%08x\n\r", lockReqPayload[0], lockReqPayload[1]);

	destOffset.High = destOffsetHi;
	destOffset.Low = destOffsetLow;

	if (nodeAddr != 0 && hHandle == 0)
	{
		hResult = lalGetHandleFromNodeAddr(nodeAddr, &hHandle);
	}

	hResult = lalLock1394(hHandle, destOffset, 8, lockReqPayload, PB_LOCKTYPE_COMPARE_SWAP);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("lock failed.");
		return hResult;
	}

	cliOutputLine("lock returned:");
	cliPrintf("0x%08x\n\r", lockReqPayload[0]);

	return hResult;
}

HRESULT lalCliLock1394(LAL_NODE_HANDLE hHandle, uint16 destOffsetHi, uint32 destOffsetLow, uint32 arg, uint32 data)
{
	return lalCliLock(hHandle, 0, destOffsetHi, destOffsetLow, data, arg);
}

HRESULT lalCliLockNode(uint16 nodeAddr, uint16 destOffsetHi, uint32 destOffsetLow, uint32 arg, uint32 data)
{
	return lalCliLock(0, nodeAddr, destOffsetHi, destOffsetLow, data, arg);
}

HRESULT	lalCliSMR(uint32 nodeAddr)
{
	HRESULT		hResult = NO_ERROR;
	uint16		maxRec = 0;

	hResult = lalGetMaxRecordSizeBytes(nodeAddr, &maxRec);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("maxRec not available for that node.");
		return hResult;
	}

	cliPrintf("node max_rec is %d bytes\n\r", maxRec);

	return hResult;
}

#ifdef _LOOSE_ISO

HRESULT lalCliAsyncStream1394(uint32 channel, uint32 tag, uint32 sy, uint32 numBytes)
{
	HRESULT		hResult = NO_ERROR;
	uint32		quad[256] = {0};
	uint32		i;
	uint32		j;
	uint32		id = 0;
	uint32		rest = 0;

	if (numBytes > 256 * 4)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

//	quad[0] = 0x00000800;

	for (i = 0; i < numBytes / 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			quad[i] |= ((id++ % 16) * 0x11) << (4 - j - 1) * 8;
		}
	}
	rest = numBytes % 4;
	if (rest)
	{
		for (j = 0; j < rest; j++)
		{
			quad[i] |= ((id++ % 16) * 0x11) << (4 - j - 1) * 8;
		}
	}

	lalAsyncStreamEnable();
	lalAsyncStreamSendData((uint8) channel, (uint8) tag, (uint8) sy, (uint16) numBytes, &(quad[0]));

	return hResult;
}

HRESULT lalCliAsyncStreamMonitor1394(uint32 channel)
{
	HRESULT		hResult = NO_ERROR;

	hResult = lalAsyncStreamDisable();
	if (hResult != NO_ERROR) return hResult;

	hResult = lalAsyncStreamEnable();
	if (hResult != NO_ERROR) return hResult;

	hResult = lalAsyncStreamEnableChannel((uint8) channel);
	if (hResult != NO_ERROR) return hResult;

	hResult = lalAsyncStreamRegisterChannelCB((uint8) channel, &lalCliAsyncStreamMonitorCB);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT lalCliAsyncStreamMonitorCB(PB *packetBlock)
{
	HRESULT		hResult = NO_ERROR;
	uint32		payloadSize, i;
	uint32		*payload;
	PB_HEADER	*header;

	hResult = pbGetPacketHeader(packetBlock, &header);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPayloadSize(packetBlock, &payloadSize);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPayload(packetBlock, (void **) &payload);
	if (hResult != NO_ERROR) return hResult;

	cliOutputLine("Async stream packet:");
	cliOutputLine("  Header:");
	
	cliPrintf("    0x%08x\n\r", header[0]);
	cliPrintf("  Payload: (%i)\n\r", payloadSize);

	payloadSize = (payloadSize + 3) >> 2;

	for (i = 0; i < payloadSize; i++)
	{
		cliPrintf("    0x%08x\n\r", payload[i]);
	}

	return hResult;
}

#endif //_LOOSE_ISO

HRESULT lalCliDisplayAllocatedRanges(void)
{
	HRESULT		hResult = NO_ERROR;
	uint16		i = 0;

	for (i = 0; i < NUM_ALLOCATABLE_ADDR_RANGES; i++)
	{
		if (lalAddressRangeCBs.range[i].inUse)
		{
			cliPrintf("[%02i] 0x%04x:0x%08x\n\r", i,lalAddressRangeCBs.range[i].AddrStart.High,lalAddressRangeCBs.range[i].AddrStart.Low);
			cliPrintf("     0x%04x:0x%08x\n\r", lalAddressRangeCBs.range[i].AddrEnd.High,lalAddressRangeCBs.range[i].AddrEnd.Low);	
		}
	}
	
	return hResult;	
}

HRESULT lalCliSetWWUID(uint32 WWUIDHigh, uint32 WWUIDLow, BOOL bReset)
{
	HRESULT		hResult = NO_ERROR;

	lalSetWWUID(WWUIDHigh, WWUIDLow);
	
	if (bReset)
	{
		lhlServiceResetLinkController();
	}

	return hResult;	
}

HRESULT lalCliGetWWUID(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		WWUIDHigh = 0;
	uint32		WWUIDLow = 0;
	
	lalGetWWUID(&WWUIDHigh, &WWUIDLow);
	
	cliPrintf("WWUID: 0x%08x 0x%08x\n\r", WWUIDHigh, WWUIDLow);

	return hResult;	
}

static  char	read1394_UsageString[]=\
				"read: \n\r"\
				"use: read1394 <handle> <offset_hi> <offset_lo> <numBytes>\n\r"\
				"<numBytes>: max 1024\n\r"\
				"------------------------------------------------------------\n\r"\
				"1394 read using node handle";
static	CLIDescriptor	read1394_Descriptor[]=
{
	{kCLI_Name,				(uint32)"read1394"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lalCliRead1394},
	{kCLI_UsageString, 		(uint32)read1394_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iiii"},
	{kCLI_ModuleName,		(uint32)"1394 read using node handle"},
	{kCLI_GroupName,		(uint32)"lal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	readNode_UsageString[]=\
				"read: \n\r"\
				"use: readnode <nodeAddr> <offset_hi> <offset_lo> <numBytes>\n\r"\
				"<numBytes>: max 1024\n\r"\
				"------------------------------------------------------------\n\r"\
				"1394 read using node address";
static	CLIDescriptor	readNode_Descriptor[]=
{
	{kCLI_Name,				(uint32)"readnode"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lalCliReadNode},
	{kCLI_UsageString, 		(uint32)readNode_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iiii"},
	{kCLI_ModuleName,		(uint32)"1394 read using node address"},
	{kCLI_GroupName,		(uint32)"lal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static char	write1394_UsageString[]=\
				"write1394: \n\r"\
				"use: write1394 <handle> <offset_hi> <offset_lo> <numBytes> <quadlet>\n\r"\
				"  quadlet contains data, numBytes: 1,..,4 \n\r"\
				"------------------------------------------------------------\n\r"\
				"1394 write using node handle";
static	CLIDescriptor	write1394_Descriptor[]=
{
	{kCLI_Name,				(uint32)"write1394"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lalCliWrite1394},
	{kCLI_UsageString, 		(uint32)write1394_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iiiii"},
	{kCLI_ModuleName,		(uint32)"1394 write using node handle"},
	{kCLI_GroupName,		(uint32)"lal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static char	writeNode_UsageString[]=\
				"writenode: \n\r"\
				"use: writenode <nodeAddr> <offset_hi> <offset_lo> <numBytes> <quadlet>\n\r"\
				"  quadlet contains data, numBytes: 1,..,4 \n\r"\
				"------------------------------------------------------------\n\r"\
				"1394 write using node address";
static	CLIDescriptor	writeNode_Descriptor[]=
{
	{kCLI_Name,				(uint32)"writenode"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lalCliWriteNode},
	{kCLI_UsageString, 		(uint32)writeNode_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iiiii"},
	{kCLI_ModuleName,		(uint32)"1394 write using node address"},
	{kCLI_GroupName,		(uint32)"lal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static char	writeTest1394_UsageString[]=\
				"writetest1394: \n\r"\
				"use: writetest1394 <handle> <offset_hi> <offset_lo> <numBytes>\n\r"\
				"<numBytes>: max 1024\n\r"\
				"  data to be sent is defined in the tool code\n\r"\
				"------------------------------------------------------------\n\r"\
				"1394 write test using node handle";
static	CLIDescriptor	writeTest1394_Descriptor[]=
{
	{kCLI_Name,				(uint32)"writetest1394"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lalCliWriteTest1394},
	{kCLI_UsageString, 		(uint32)writeTest1394_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iiii"},
	{kCLI_ModuleName,		(uint32)"1394 write test using node handle"},
	{kCLI_GroupName,		(uint32)"lal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static char	writeTestNode_UsageString[]=\
				"writetestnode: \n\r"\
				"use: writetestnode <nodeAddr> <offset_hi> <offset_lo> <numBytes>\n\r"\
				"<numBytes>: max 1024\n\r"\
				"  data to be sent is defined in the tool code\n\r"\
				"------------------------------------------------------------\n\r"\
				"1394 write test using node address";
static	CLIDescriptor	writeTestNode_Descriptor[]=
{
	{kCLI_Name,				(uint32)"writetestnode"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lalCliWriteNode},
	{kCLI_UsageString, 		(uint32)writeTestNode_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iiii"},
	{kCLI_ModuleName,		(uint32)"1394 write test using node address"},
	{kCLI_GroupName,		(uint32)"lal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static char	lock1394_UsageString[]=\
				"lock1394: \n\r"\
				"use: write <handle> <offset_hi> <offset_lo> <arg> <data>\n\r"\
				"------------------------------------------------------------\n\r"\
				"1394 32-bit lock using node handle";
static	CLIDescriptor	lock1394_Descriptor[]=
{
	{kCLI_Name,				(uint32)"lock1394"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lalCliLock1394},
	{kCLI_UsageString, 		(uint32)lock1394_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iiiii"},
	{kCLI_ModuleName,		(uint32)"1394 lock using node handle"},
	{kCLI_GroupName,		(uint32)"lal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static char	lockNode_UsageString[]=\
				"locknode: \n\r"\
				"use: write <nodeAddr> <offset_hi> <offset_lo> <arg> <data>\n\r"\
				"------------------------------------------------------------\n\r"\
				"1394 32-bit lock using node address";
static	CLIDescriptor	lockNode_Descriptor[]=
{
	{kCLI_Name,				(uint32)"locknode"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lalCliLockNode},
	{kCLI_UsageString, 		(uint32)lockNode_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iiiii"},
	{kCLI_ModuleName,		(uint32)"1394 lock using node address"},
	{kCLI_GroupName,		(uint32)"lal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

#ifdef _LOOSE_ISO
/*
	Write Async Stream Tool

*/
static char	stream1394_UsageString[]=\
				"async1394: \n\r"\
				"use: async1394 <channel> <tag> <sy> <numBytes>\n\r"\
				"<numBytes>: max 1024\n\r"\
				"  data to be sent is defined in the tool code\n\r"\
				"------------------------------------------------------------\n\r"\
				"1394 async stream write";
static	CLIDescriptor	stream1394_Descriptor[]=
{
	{kCLI_Name,				(uint32)"async1394"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lalCliAsyncStream1394},
	{kCLI_UsageString, 		(uint32)stream1394_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iiii"},
	{kCLI_ModuleName,		(uint32)"1394 async stream write"},
	{kCLI_GroupName,		(uint32)"lal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/*
	Async Stream Register Monitor Tool
*/
static char	monitor1394_UsageString[]=\
				"masync1394: \n\r"\
				"use: masync1394 <channel>\n\r"\
				"  Prints anything that arrives on the given channel\n\r"\
				"------------------------------------------------------------\n\r"\
				"1394 async stream monitor";
static	CLIDescriptor	monitor1394_Descriptor[]=
{
	{kCLI_Name,				(uint32)"masync1394"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lalCliAsyncStreamMonitor1394},
	{kCLI_UsageString, 		(uint32)monitor1394_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"1394 async stream read"},
	{kCLI_GroupName,		(uint32)"lal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

#endif //_LOOSE_ISO

/*
	Get Max Rec Tool

	gets the maximum number of packet bytes advertised in a node's Config ROM
*/
static char	SMR_UsageString[]=\
				"smr: \n\r"\
				"use: smr <nodeAddr>\n\r"\
				"------------------------------------------------------------\n\r"\
				"show max # of packet bytes from a node's Config ROM nodeCaps";
static	CLIDescriptor	smr_Descriptor[]=
{
	{kCLI_Name,				(uint32)"smr"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lalCliSMR},
	{kCLI_UsageString, 		(uint32)SMR_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"show device max_rec"},
	{kCLI_GroupName,		(uint32)"lal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/*
	Tool for displaying allocated addresses ranges
*/

static char	lalDisplayRanges_UsageString[]=\
				"rng\n\r"\
				"------------------------------------------------------------\n\r"\
				"display the 1394 addresses which are allocated on this unit";
static	CLIDescriptor	lalDisplayRanges_Descriptor[]=
{
	{kCLI_Name,				(uint32)"rng"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lalCliDisplayAllocatedRanges},
	{kCLI_UsageString, 		(uint32)lalDisplayRanges_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"display allocated 1394 ranges"},
	{kCLI_GroupName,		(uint32)"lal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static char	lalSetWWUID_UsageString[]=\
				"setwwuid\n\r"\
				"use: setwwuid <wwuid.high> <wwuid.low> <sw-reset>\n\r"\
				"------------------------------------------------------------\n\r"\
				"set wwuid of this unit";
static	CLIDescriptor	lalSetWWUID_Descriptor[]=
{
	{kCLI_Name,				(uint32)"setwwuid"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lalCliSetWWUID},
	{kCLI_UsageString, 		(uint32)lalSetWWUID_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iii"},
	{kCLI_ModuleName,		(uint32)"set wwuid of this unit"},
	{kCLI_GroupName,		(uint32)"lal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static char	lalGetWWUID_UsageString[]=\
				"getwwuid\n\r"\
				"------------------------------------------------------------\n\r"\
				"get wwuid of this unit";
static	CLIDescriptor	lalGetWWUID_Descriptor[]=
{
	{kCLI_Name,				(uint32)"getwwuid"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&lalCliGetWWUID},
	{kCLI_UsageString, 		(uint32)lalGetWWUID_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"get wwuid of this unit"},
	{kCLI_GroupName,		(uint32)"lal"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void lalCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(read1394_Descriptor);
	hResult += cliInstallCLIDescriptor(write1394_Descriptor);
	hResult += cliInstallCLIDescriptor(writeTest1394_Descriptor);
	hResult += cliInstallCLIDescriptor(lock1394_Descriptor);
	hResult += cliInstallCLIDescriptor(readNode_Descriptor);
	hResult += cliInstallCLIDescriptor(writeNode_Descriptor);
	hResult += cliInstallCLIDescriptor(writeTestNode_Descriptor);
	hResult += cliInstallCLIDescriptor(lockNode_Descriptor);
#ifdef _LOOSE_ISO
	hResult += cliInstallCLIDescriptor(stream1394_Descriptor);
	hResult += cliInstallCLIDescriptor(monitor1394_Descriptor);
#endif //_LOOSE_ISO
	hResult += cliInstallCLIDescriptor(smr_Descriptor);
	hResult += cliInstallCLIDescriptor(lalDisplayRanges_Descriptor);
	hResult += cliInstallCLIDescriptor(lalSetWWUID_Descriptor);
	hResult += cliInstallCLIDescriptor(lalGetWWUID_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing lal cli tools");
	}
}

#endif //_CLI_TOOL_LAL
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
