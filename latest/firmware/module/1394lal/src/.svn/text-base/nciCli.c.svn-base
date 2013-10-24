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
 * DESCRIPTION: Node Controller Interface Command Line Interface

		nci CLI Tools and Variables
	
		These are used by the CLI to view nci info.

	Revisions:
		created 05/01/2002 lm
****************************************************************************/

#ifdef _CLI
#include "cliToolDefs.h"
#include "TCTasking.h"
#include "nci.h"
#include "nciBM.h"
#include "nciBMCli.h"
#include "nciCli.h"
#include "nciHandles.h"
#include "cli.h"
#include "csr.h"
#include "bri.h"
#include "lal.h"
#include "lhlBri.h"

//static char 			moduleName[] = __FILE__ ;

HRESULT nciCliGetBusInfo(void)
{
	HRESULT				hResult = NO_ERROR;
	LAL_BUS_RESET_INFO	busResetInfo;
	char				buf[32];

	hResult = nciGetBusInfo(&busResetInfo);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("get bus info failed.");

		return hResult;
	}

	cliOutputLine("Bus Info:");
	sprintf(buf, " current bus time      0x%08x", busResetInfo.currentTime);
	cliOutputLine(buf);
	sprintf(buf, " bus generation        0x%08x", busResetInfo.busGeneration);
	cliOutputLine(buf);
	sprintf(buf, " number of nodes found 0x%08x", busResetInfo.numNodes);
	cliOutputLine(buf);
	sprintf(buf, " self id quads found   0x%08x", busResetInfo.numSelfIDs);
	cliOutputLine(buf);
	sprintf(buf, " root node addr        0x%08x", busResetInfo.rootAddr);
	cliOutputLine(buf);
	sprintf(buf, " irm node addr         0x%08x", busResetInfo.irmAddr);
	cliOutputLine(buf);
	sprintf(buf, " thisNode addr         0x%08x", busResetInfo.nodeAddr);
	cliOutputLine(buf);
	sprintf(buf, " local bus Id          0x%08x", busResetInfo.busId);
	cliOutputLine(buf);

	return hResult;
}

HRESULT nciCliGetSpeedTxt(uint32 speed, char *buf)
{
	HRESULT			hResult = NO_ERROR;

	switch (speed)
	{
		case lal100:
			sprintf(buf, "100");
			break;
		case lal200:
			sprintf(buf, "200");
			break;
		case lal400:
			sprintf(buf, "400");
			break;
		case lalSpeedFastest:
			sprintf(buf, "MAX");
			break;
		default:
			sprintf(buf, "???");
			hResult = E_NCI_INVALID_SPEED;
			break;
	}

	return hResult;
}

HRESULT nciCliGetNodeInfo(uint16 nodeIndex)
{
	HRESULT				hResult = NO_ERROR;
	LAL_DEVICE_INFO		lalDevInfo;
	char				buf[32];
	uint16				busId = 0;

	hResult = briGetBusID(&busId);
	if (hResult != NO_ERROR) return hResult;
	
	nodeIndex |= busId;

	hResult = nciGetNodeInfo(nodeIndex, &lalDevInfo);
	if (hResult != NO_ERROR) return hResult;

	sprintf(buf,"Node 0x%04x Info:", nodeIndex);
	cliOutputLine(buf);

	sprintf(buf, " active    0x%08x", lalDevInfo.active);
	cliOutputLine(buf);
	sprintf(buf, " nodeAddr  0x%08x", lalDevInfo.nodeAddr);
	cliOutputLine(buf);
	sprintf(buf, " WWUID hi  0x%08x", lalDevInfo.WWUID.hi);
	cliOutputLine(buf);
	sprintf(buf, " WWUID low 0x%08x", lalDevInfo.WWUID.low);
	cliOutputLine(buf);

	return hResult;
}

#ifdef _CLI_TOOL_NCI
HRESULT nciCliDumpAllNodeInfo(void)
{
	HRESULT				hResult = NO_ERROR;
	uint16				nodeIndex;
	LAL_BUS_RESET_INFO	busResetInfo;

	hResult = nciCliDumpHandleTable();
	if (hResult != NO_ERROR) return hResult;
	
	hResult = nciCliGetBusInfo();
	if (hResult != NO_ERROR) return hResult;

	hResult = nciCliDumpLocalNodeInfo();
	if (hResult != NO_ERROR) return hResult;

	hResult = nciGetBusInfo(&busResetInfo);
	if (hResult != NO_ERROR) return hResult;

	for (nodeIndex = 0; nodeIndex < busResetInfo.numNodes; nodeIndex++)
	{
		hResult = nciCliGetNodeInfo(nodeIndex);
	}

	return hResult;
}

/*
	Update node info cache
*/

HRESULT nciCliUpdate(void)
{
  return briUpdateNodeInfos(TRUE);
}

static  char	nciUpdate_UsageString[]=\
				"nci:update : \n\r"\
				"use: nci:update\n\r"\
				"------------------------------------------------------------\n\r"\
				"Update cached node info";
static	CLIDescriptor	nciUpdate_Descriptor[]=
{
	{kCLI_Name,				(uint32)"update"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&nciCliUpdate},
	{kCLI_UsageString, 		(uint32)nciUpdate_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"update node info cache"},
	{kCLI_GroupName,		(uint32)"nci"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

HRESULT nciCliDumpHandleTable(void)
{
	HRESULT			hResult = NO_ERROR;
	uint16			handleIndex;
	char			buf[64];

	cliOutputLine("Handle Table (hex):");
	if (nciHandles.count == 0)
	{
		cliOutputLine(" no entries.");
		return hResult;
	}

	cliOutputLine("index handle   wwuid_hi wwuid_lo nodeAddr numInst  spd  Q   type  active");
	cliOutputLine("------------------------------------------------------------------------");

	for (handleIndex = 0; handleIndex < MAX_NODES_PER_BUS; handleIndex++)
	{
		if (nciHandles.handleRec[handleIndex].status != NCI_HANDLE_STATUS_UNUSED)
		{
			sprintf(buf, "%02x    %08x %08x %08x %04x     %08x ",
							handleIndex,
							nciHandles.handleRec[handleIndex].handle,
							nciHandles.handleRec[handleIndex].wwuid.hi,
							nciHandles.handleRec[handleIndex].wwuid.low,
							nciHandles.handleRec[handleIndex].nodeAddr,
							nciHandles.handleRec[handleIndex].numInst);
			cliOutputRaw(buf);

			nciCliGetSpeedTxt(nciHandles.handleRec[handleIndex].speed, buf);
			cliOutputRaw(buf);

			if (nciHandles.handleRec[handleIndex].qType == PB_Q_TYPE_GENERAL)
			{
				cliOutputRaw("  Gen ");
			}
			else if (nciHandles.handleRec[handleIndex].qType == PB_Q_TYPE_IMMEDIATE)
			{
				cliOutputRaw("  Imm ");
			}
			else
			{
				cliOutputRaw("  ??? ");
			}

			if (nciHandles.handleRec[handleIndex].status == NCI_HANDLE_STATUS_NODE)
			{
				cliOutputRaw("node ");
			}
			else if (nciHandles.handleRec[handleIndex].status == NCI_HANDLE_STATUS_WWUID)
			{
				cliOutputRaw("wwuid");
			}
			else
			{
				cliOutputRaw("?????");
			}

			if (nciHandles.handleRec[handleIndex].bActive)
			{
				cliOutputLine(" yes");
			}
			else
			{
				cliOutputLine(" no ");
			}

		}
	}

	return hResult;
}

HRESULT nciCliCreateHandle(uint32 wwuid_hi, uint32 wwuid_lo)
{
	HRESULT				hResult = NO_ERROR;
	LAL_NODE_HANDLE		handle = 0;
	char				buf[64];

	hResult = nciCreateHandle(wwuid_hi, wwuid_lo, &handle);

	if (hResult == NO_ERROR)
	{
		sprintf(buf, "handle creation success.  Handle is: %08x", handle);
		cliOutputLine(buf);
	}
	else
	{
		sprintf(buf, "handle creation failed: %08x", hResult);
		cliOutputLine(buf);
	}

	return hResult;
}


HRESULT nciCliCloseHandle(LAL_NODE_HANDLE handle)
{
	HRESULT				hResult = NO_ERROR;
	char				buf[32];

	hResult = nciCloseHandle(handle);

	if (hResult == NO_ERROR)
	{
		sprintf(buf, "handle close success.");
		cliOutputLine(buf);
	}
	else
	{
		sprintf(buf, "handle close failed: %08x", hResult);
		cliOutputLine(buf);
	}

	return hResult;
}

/*
	Handle Table Dump Tool
*/
static  char	nciDump_UsageString[]=\
				"nci:dump : \n\r"\
				"use: nci:dump\n\r"\
				"------------------------------------------------------------\n\r"\
				"display handle table entries";
static	CLIDescriptor	nciDump_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dump"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&nciCliDumpHandleTable},
	{kCLI_UsageString, 		(uint32)nciDump_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"dump handle table"},
	{kCLI_GroupName,		(uint32)"nci"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/*
	WWUID-based Handle Create Tool
*/
static  char	nciCreate_UsageString[]=\
				"nci:create: \n\r"\
				"use: nci:create <wwuid_hi> <wwuid_lo>, for WWUID handle\n\r"\
				"       example: nci:create 0x109f01 0x1433\n\r"\
				"     nci:create 0 <nodeAddr>, for node-address based handle\n\r"\
				"       example: nci:create 0 0xffc1, for handle to node 1\n\r"\
				"------------------------------------------------------------\n\r"\
				"create a device handle";
static	CLIDescriptor	nciCreate_Descriptor[]=
{
	{kCLI_Name,				(uint32)"create"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&nciCliCreateHandle},
	{kCLI_UsageString, 		(uint32)nciCreate_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ii"},
	{kCLI_ModuleName,		(uint32)"create a device handle"},
	{kCLI_GroupName,		(uint32)"nci"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/*
	Handle Close Tool
*/
static  char	nciClose_UsageString[]=\
				"nci:close: \n\r"\
				"use: nci:close <handle>\n\r"\
				"------------------------------------------------------------\n\r"\
				"close a device handle";
static	CLIDescriptor	nciClose_Descriptor[]=
{
	{kCLI_Name,				(uint32)"close"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&nciCliCloseHandle},
	{kCLI_UsageString, 		(uint32)nciClose_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"close a device handle"},
	{kCLI_GroupName,		(uint32)"nci"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/*
	Get Bus Info Tool
*/
static  char	nciGBI_UsageString[]=\
				"nci:gbi: \n\r"\
				"use: nci:gbi\n\r"\
				"------------------------------------------------------------\n\r"\
				"get bus info";
static	CLIDescriptor	nciGBI_Descriptor[]=
{
	{kCLI_Name,				(uint32)"gbi"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&nciCliGetBusInfo},
	{kCLI_UsageString, 		(uint32)nciGBI_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"get bus info"},
	{kCLI_GroupName,		(uint32)"nci"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/*
	Get Node Info Tool
*/
static  char	nciGNI_UsageString[]=\
				"nci:gni: \n\r"\
				"use: nci:gni <nodeIndex>\n\r"\
				"------------------------------------------------------------\n\r"\
				"get node info\n\r";

static	CLIDescriptor	nciGNI_Descriptor[]=
{
	{kCLI_Name,				(uint32)"gni"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&nciCliGetNodeInfo},
	{kCLI_UsageString, 		(uint32)nciGNI_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"get node info"},
	{kCLI_GroupName,		(uint32)"nci"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/*
	Get Local Node Info Tool
*/
static  char	nciGLI_UsageString[]=\
				"nci:gli \n\r"\
				"use: nci:gli\n\r"\
				"------------------------------------------------------------\n\r"\
				"get local info";
static	CLIDescriptor	nciGLI_Descriptor[]=
{
	{kCLI_Name,				(uint32)"gli"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&nciCliDumpLocalNodeInfo},
	{kCLI_UsageString, 		(uint32)nciGLI_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"get local info"},
	{kCLI_GroupName,		(uint32)"nci"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/*
	Get All Info Tool
*/
static  char	nciGAI_UsageString[]=\
				"nci:gai \n\r"\
				"use: nci:gai\n\r"\
				"------------------------------------------------------------\n\r"\
				"get all info";
static	CLIDescriptor	nciGAI_Descriptor[]=
{
	{kCLI_Name,				(uint32)"gai"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&nciCliDumpAllNodeInfo},
	{kCLI_UsageString, 		(uint32)nciGAI_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"get all info"},
	{kCLI_GroupName,		(uint32)"nci"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

HRESULT nciCliDumpSelfIDQuads(void)
{
	HRESULT			hResult = NO_ERROR;
	uint32			i = 0;
	uint32			items = 0;
	QUADLET			*selfIDs = 0;
	char			buf[16];

	items = briGetNumSIDQuads();
	selfIDs = briGetSIDs();

	cliPrintf("    self id's:\n\r");

	if (briGetNumSIDQuads() > 0)
	{
		DO_FOREVER
		{
			if (i >= items) break;

			cliPrintf("   0x%08x [", selfIDs[i]);
			cliPrintf("phy_ID:%02x,", nciExtractSelfidNodeId(selfIDs[i]));
			cliPrintf("L:%s,", (nciExtractSelfidLinkActive(selfIDs[i])) ? "ON " : "OFF");
			cliPrintf("gap_cnt:%i,", nciExtractSelfidGapCnt(selfIDs[i]));
			nciCliGetSpeedTxt(nciExtractSelfidSpeed(selfIDs[i]), buf);
			cliPrintf("sp:%s,", buf);
			cliPrintf("c:%s,", (nciExtractSelfidContender(selfIDs[i])) ? "ON " : "OFF");
			cliPrintf("i:%s,", (nciExtractSelfidInitiator(selfIDs[i])) ? "ON " : "OFF");
			cliPrintf("m:%s", (nciExtractSelfidMorePackets(selfIDs[i])) ? "ON " : "OFF");
			cliPrintf("]\n\r");
			i++;
			while (nciExtractSelfidMorePackets(selfIDs[i]))
			{
				cliPrintf("   0x%08x\n\r", selfIDs[i]);
				i++;
			}
		}
	}
	else
	{
		cliPrintf("none\n\r");
	}

	return hResult;
}

/*
	print local node info
*/
HRESULT nciCliDumpLocalNodeInfo(void)
{
	HRESULT			hResult = NO_ERROR;
	uint32			WWUIDHigh = 0;
	uint32			WWUIDLow = 0;
	uint32			busGen = 0;
	LOCAL_NODEINFO	nodeInfo;

	cliPrintf("Local Node Info:\n\r");

	hResult = lalGetWWUID(&WWUIDHigh, &WWUIDLow);
	if (hResult == NO_ERROR)
	{
		cliPrintf(" Node WWUID: 0x%08x 0x%08x\n\r", WWUIDHigh, WWUIDLow);
	}
	else
	{
		cliPrintf("** Node WWUID not set **");
	}

	briGetCurrentBusGeneration(&busGen);
	cliPrintf(" bus generation: 0x%08x - this node is ", busGen);

	if (lhlBriIsRoot())
	{
		cliPrintf("root");
	}
	else
	{
		cliPrintf("not root");
	}

	if (lhlBriIsCycleMaster())
	{
		cliPrintf(" and cycle master\n\r");
	}
	else
	{
		cliPrintf(" and not cycle master\n\r");
	}

	if (briIsSelfIDsValid() == TRUE)
	{
		briGetLocalNodeInfo(&nodeInfo);
		cliPrintf(" local bus ID:   0x%04X, node ID: 0x%02X\n\r", nodeInfo.busId, nodeInfo.nodeId);
		cliPrintf("  nodes found:   %i, sid's found: %i\n\r", briGetNumNodes(), briGetNumSIDQuads());

		nciCliDumpSelfIDQuads();
	}
	else
	{
		cliPrintf("Local node self-id info not current\n\r");
	}
	return hResult;
}

void nciCliInstallTools(void)
{
	HRESULT			hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(nciDump_Descriptor);
	hResult += cliInstallCLIDescriptor(nciCreate_Descriptor);
	hResult += cliInstallCLIDescriptor(nciClose_Descriptor);
	hResult += cliInstallCLIDescriptor(nciGBI_Descriptor);
	hResult += cliInstallCLIDescriptor(nciGNI_Descriptor);
	hResult += cliInstallCLIDescriptor(nciGLI_Descriptor);
	hResult += cliInstallCLIDescriptor(nciGAI_Descriptor);
	hResult += cliInstallCLIDescriptor(nciUpdate_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing nci cli tools");
	}

#ifdef _CLI_TOOL_NCI_BM
	nciBMCliInstallTools();
#endif //_CLI_TOOL_NCI_BM
}

#endif //_CLI_TOOL_NCI
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
