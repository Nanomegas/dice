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
   * DESCRIPTION: Command Line Interface - CB (for remote CLI Calls)

	Purpose: 

		This file describes the Command Line Interpreter (cli) CB Cli Module
	
  Revisions:
		created 22/10/2002 lm
****************************************************************************/

#ifdef _CLI
#ifdef _CLI_CB

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "ErrorCodes.h"
#include "lal.h"
#include "nciHandles.h"
#include "csr.h"
#include "cli.h"
#include "cliSIO.h"
#include "cliCB.h"
#include "cliCBCli.h"
#include "format.h"
#include "packetBlock.h"

HRESULT	cliCBCliNodeAddrCall(uint32 nodeAddr, char *cliStr)
{
	HRESULT		hResult = NO_ERROR;
	char		cliCommandStr[kTempBufferSize] = {0};

	if (strlen(cliStr) > kTempBufferSize - 1)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	strcpy(cliCommandStr, cliStr);
	formatSwapStrBytes(cliCommandStr, kTempBufferSize);

	hResult = lalWriteNode(nodeAddr, cliDestOffsetLo, kTempBufferSize, (uint32 *) &cliCommandStr);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT	cliCBCliHandleCall(LAL_NODE_HANDLE handle, char *cliStr)
// Used to send a CLI command to another machine, by handle
{
	HRESULT		hResult = NO_ERROR;
	uint32		nodeAddr = 0xffff;

	// verify handle
	if (nciIsValidHandle(handle))
	{
		// get nodeAddr for transaction
		hResult = lalGetNodeAddrFromHandle(handle, &nodeAddr);
		if (hResult != NO_ERROR) return hResult;
	}

	hResult = cliCBCliNodeAddrCall(nodeAddr, cliStr);

	return hResult;
}

HRESULT	cliCBCliNodeIdCall(uint32 nodeId, char *cliStr)
// Used to send a CLI command to another device, by node id
{
	HRESULT		hResult = NO_ERROR;
	uint16		busId = 0;

	hResult = lalGetBusID(&busId);
	if (hResult != NO_ERROR) return hResult;

	hResult = cliCBCliNodeAddrCall(nodeId | busId, cliStr);

	return hResult;
}

static  char	cliCBCliNodeIdCall_UsageString[]=\
				"rc:id: \n\r"\
				"use: rc:id <nodeId> <str>\n\r"\
				"------------------------------------------------------------\n\r"\
				"remote call to cli using nodeid";

static	CLIDescriptor	cliCBCliNodeIdCall_Descriptor[]=
{
	{kCLI_Name,				(uint32)"id"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cliCBCliNodeIdCall},
	{kCLI_UsageString, 		(uint32)cliCBCliNodeIdCall_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"is"},
	{kCLI_ModuleName,		(uint32)"remote call to cli using nodeid"},
	{kCLI_GroupName,		(uint32)"rc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	cliCBCliHandleCall_UsageString[]=\
				"rc:handle: \n\r"\
				"use: rc:handle <handle> <str>\n\r"\
				"------------------------------------------------------------\n\r"\
				"remote call to cli using handle";

static	CLIDescriptor	cliCBCliHandleCall_Descriptor[]=
{
	{kCLI_Name,				(uint32)"handle"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cliCBCliHandleCall},
	{kCLI_UsageString, 		(uint32)cliCBCliHandleCall_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"is"},
	{kCLI_ModuleName,		(uint32)"remote call to cli using nodeid handle"},
	{kCLI_GroupName,		(uint32)"rc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	cliCBCliNodeAddrCall_UsageString[]=\
				"rc:node: \n\r"\
				"use: rc:node <nodeAddr> <str>\n\r"\
				"------------------------------------------------------------\n\r"\
				"remote call to cli using nodeaddr";

static	CLIDescriptor	cliCBCliNodeAddrCall_Descriptor[]=
{
	{kCLI_Name,				(uint32)"node"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cliCBCliNodeAddrCall},
	{kCLI_UsageString, 		(uint32)cliCBCliNodeAddrCall_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"is"},
	{kCLI_ModuleName,		(uint32)"remote call to cli using nodeaddr"},
	{kCLI_GroupName,		(uint32)"rc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	cliCBCliNodeVerbose_UsageString[]=\
				"rc:verbose: \n\r"\
				"use: rc:verbose <on|off>\n\r"\
				"------------------------------------------------------------\n\r"\
				"remote call to cli verbose mode on/off when receiving commands";

static	CLIDescriptor	cliCBCliNodeVerbose_Descriptor[]=
{
	{kCLI_Name,				(uint32)"verbose"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cliCBNodeVerbose},
	{kCLI_UsageString, 		(uint32)cliCBCliNodeVerbose_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"remote call to cli verbose mode"},
	{kCLI_GroupName,		(uint32)"rc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	cliCBCliNodeDeferred_UsageString[]=\
				"rc:deferred: \n\r"\
				"use: rc:deferred <on|off>\n\r"\
				"------------------------------------------------------------\n\r"\
				"remote call to cli command is deferred";

static	CLIDescriptor	cliCBCliNodeDeferred_Descriptor[]=
{
	{kCLI_Name,				(uint32)"deferred"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cliCBNodeDeferred},
	{kCLI_UsageString, 		(uint32)cliCBCliNodeDeferred_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"remote call to cli deferred"},
	{kCLI_GroupName,		(uint32)"rc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void cliCBCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(cliCBCliNodeIdCall_Descriptor);
	hResult += cliInstallCLIDescriptor(cliCBCliHandleCall_Descriptor);
	hResult += cliInstallCLIDescriptor(cliCBCliNodeAddrCall_Descriptor);
	hResult += cliInstallCLIDescriptor(cliCBCliNodeVerbose_Descriptor);
	hResult += cliInstallCLIDescriptor(cliCBCliNodeDeferred_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing cli CB cli tools");
	}
}

#endif //_CLI_CB
#endif //_CLI
