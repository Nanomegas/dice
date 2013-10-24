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
 * DESCRIPTION: Connection Management Procedures (CMP) CLI

	This module implements the 
		
		  Connection Management Procedures Command Line Interface

	Revisions:
		created 09/17/2002 lm

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_CMP
#include "TCTasking.h"
#include "cli.h"
#include "cmp.h"
#include "cmpCli.h"
#include "cmpDefs.h"
#include "cmpUtils.h"

HRESULT cmpCliiMPRVarCB(uint32 operation, char* varname, uint32 oldValue, uint32* newValue)
{
	HRESULT			hResult = NO_ERROR;
	uint32			respValue = 0;

	UNUSED_ARG(varname);

	switch (operation)
	{
		case kCLI_VariableCallbackOperation_GET:
			hResult = cmpGetiMPR(newValue);
			break;
		case kCLI_VariableCallbackOperation_SET:
			hResult = cmpSetiMPR(oldValue, *newValue, &respValue);
			if (oldValue != respValue)
			{
				hResult = E_LOCK_COMPARE_SWAP_FAIL;
			}
			break;
		default:
			hResult = E_NOT_IMPLEMENTED;
			break;
	}

	return hResult;
}

HRESULT cmpClioMPRVarCB(uint32 operation, char* varname, uint32 oldValue, uint32* newValue)
{
	HRESULT			hResult = NO_ERROR;
	uint32			respValue = 0;

	UNUSED_ARG(varname);

	switch (operation)
	{
		case kCLI_VariableCallbackOperation_GET:
			hResult = cmpGetoMPR(newValue);
			break;
		case kCLI_VariableCallbackOperation_SET:
			hResult = cmpSetoMPR(oldValue, *newValue, &respValue);
			if (oldValue != respValue)
			{
				hResult = E_LOCK_COMPARE_SWAP_FAIL;
			}
			break;
		default:
			hResult = E_NOT_IMPLEMENTED;
			break;
	}

	return hResult;
}

HRESULT cmpCliiPCR0VarCB(uint32 operation, char* varname, uint32 oldValue, uint32* newValue)
{
	HRESULT			hResult = NO_ERROR;
	uint32			respValue = 0;
	uint32			PCRNumber = 0;
	uint32			MPR = 0;

	UNUSED_ARG(varname);

	PCRNumber = 0;

	hResult = cmpGetiMPR(&MPR);
	if (hResult != NO_ERROR) return hResult;

	if (PCRNumber >= cmpGetMPRNumPlugs(MPR)) return E_FAIL;

	switch (operation)
	{
		case kCLI_VariableCallbackOperation_GET:
			hResult = cmpGetiPCR(PCRNumber, newValue);
			break;
		case kCLI_VariableCallbackOperation_SET:
			hResult = cmpSetiPCR(PCRNumber, oldValue, *newValue, &respValue);
			if (oldValue != respValue)
			{
				hResult = E_LOCK_COMPARE_SWAP_FAIL;
			}
			break;
		default:
			hResult = E_NOT_IMPLEMENTED;
			break;
	}

	return hResult;
}

HRESULT cmpClioPCR0VarCB(uint32 operation, char* varname, uint32 oldValue, uint32* newValue)
{
	HRESULT			hResult = NO_ERROR;
	uint32			respValue = 0;
	uint32			PCRNumber = 0;
	uint32			MPR = 0;

	UNUSED_ARG(varname);

	PCRNumber = 0;

	hResult = cmpGetoMPR(&MPR);
	if (hResult != NO_ERROR) return hResult;

	if (PCRNumber >= cmpGetMPRNumPlugs(MPR)) return E_FAIL;

	switch (operation)
	{
		case kCLI_VariableCallbackOperation_GET:
			hResult = cmpGetoPCR(PCRNumber, newValue);
			break;
		case kCLI_VariableCallbackOperation_SET:
			hResult = cmpSetoPCR(PCRNumber, oldValue, *newValue, &respValue);

			if (oldValue != respValue)
			{
				hResult = E_LOCK_COMPARE_SWAP_FAIL;
			}
			break;
		default:
			hResult = E_NOT_IMPLEMENTED;
			break;
	}

	return hResult;
}

void cmpCliDisplayDataRate(uint32 rate)
{
	switch (rate)
	{
		case 0:		cliPrintf("S100 (0x00)"); break;
		case 1:		cliPrintf("S200 (0x01)"); break;
		case 2:		cliPrintf("S400 (0x10)"); break;
		case 3:		cliPrintf("reserved (0x11)"); break;
		default:	cliPrintf("unknown rate:%i", rate); break;
	}

	cliPrintf("\n\r");
}

void cmpCliDisplayPlugState(uint32 state)
{
	cliPrintf("\tplugState:\t");
	
	switch (state)
	{
		case PLUG_ACTIVE:		cliPrintf("PLUG_ACTIVE (online and connected)"); break;
		case PLUG_READY:		cliPrintf("PLUG_READY (online and non-connected)"); break;
		case PLUG_SUSPENDED:	cliPrintf("PLUG_SUSPENDED (offline and connected)"); break;
		case PLUG_IDLE:			cliPrintf("PLUG_IDLE (offline and non-connected)"); break;
		default:				cliPrintf("unknown state: %i", state); break;
	}

	cliPrintf("\n\r");
}

HRESULT cmpCliiMPRRead(uint32 iNodeAddr)
{
	HRESULT			hResult = NO_ERROR;
	uint32			iMPR = 0;

	hResult = cmpReadiMPR((uint16) iNodeAddr, &iMPR);
	if (hResult != NO_ERROR) return hResult;

	cliPrintf("iMPR on nodeAddr[0x%04x]: 0x%08x\n\r", iNodeAddr, iMPR);
	cliPrintf("\tRateCap:\t");
	cmpCliDisplayDataRate(cmpGetMPRRateCap(iMPR));
	cliPrintf("\t#inplugs:\t%i\n\r", cmpGetMPRNumPlugs(iMPR));

	return hResult;
}

HRESULT cmpClioMPRRead(uint32 oNodeAddr)
{
	HRESULT			hResult = NO_ERROR;
	uint32			oMPR = 0;

	hResult = cmpReadoMPR((uint16) oNodeAddr, &oMPR);
	if (hResult != NO_ERROR) return hResult;

	cliPrintf("oMPR on nodeAddr[0x%04x]: 0x%08x\n\r", oNodeAddr, oMPR);
	cliPrintf("\tRateCap:\t");
	cmpCliDisplayDataRate(cmpGetMPRRateCap(oMPR));
	cliPrintf("\tBCBase:\t\t%i\n\r", cmpGetMPRBCBase(oMPR));
	cliPrintf("\t#outplugs:\t%i\n\r", cmpGetMPRNumPlugs(oMPR));

	return hResult;
}

HRESULT cmpCliiPCRRead(uint32 iPCRNumber, uint32 iNodeAddr)
{
	HRESULT			hResult = NO_ERROR;
	uint32			iPCR = 0;

	hResult = cmpReadiPCR(iPCRNumber, (uint16) iNodeAddr, &iPCR);
	if (hResult != NO_ERROR) return hResult;

	cliPrintf("iPCR[%i] on nodeAddr[0x%04x]: 0x%08x\n\r", iPCRNumber, iNodeAddr, iPCR);
	cliPrintf("\tOnline:\t%i\n\r", cmpGetPCROnline(iPCR));
	cliPrintf("\tBCCount:\t%i\n\r", cmpGetPCRBCCount(iPCR));
	cliPrintf("\tP2PCount:\t%i\n\r", cmpGetPCRP2PCount(iPCR));
	cliPrintf("\tConnCount:\t%i\n\r", cmpGetPCRConnCount(iPCR));
	cliPrintf("\tChannel:\t%i\n\r", cmpGetPCRChannel(iPCR));
	cmpCliDisplayPlugState(cmpGetPCRState(iPCR));

	return hResult;
}

HRESULT cmpClioPCRRead(uint32 oPCRNumber, uint32 oNodeAddr)
{
	HRESULT			hResult = NO_ERROR;
	uint32			oPCR = 0;

	hResult = cmpReadoPCR(oPCRNumber, (uint16) oNodeAddr, &oPCR);
	if (hResult != NO_ERROR) return hResult;

	cliPrintf("oPCR[%i] on nodeAddr[0x%04x]: 0x%08x\n\r", oPCRNumber, oNodeAddr, oPCR);
	cliPrintf("\tOnline:\t\t%i\n\r", cmpGetPCROnline(oPCR));
	cliPrintf("\tBCCount:\t%i\n\r", cmpGetPCRBCCount(oPCR));
	cliPrintf("\tP2PCount:\t%i\n\r", cmpGetPCRP2PCount(oPCR));
	cliPrintf("\tConnCount:\t%i\n\r", cmpGetPCRConnCount(oPCR));
	cliPrintf("\tChannel:\t%i\n\r", cmpGetPCRChannel(oPCR));
	cliPrintf("\tDataRate:\t");
	cmpCliDisplayDataRate(cmpGetPCRRate(oPCR));
	cliPrintf("\tOverheadID:\t%i\n\r", cmpGetPCROverheadID(oPCR));
	cliPrintf("\tPayload:\t%i\n\r", cmpGetPCRPayload(oPCR));
	cliPrintf("\tBandwidth:\t%i\n\r", cmpGetPCRBandwidth(oPCR));
	cmpCliDisplayPlugState(cmpGetPCRState(oPCR));

	return hResult;
}

HRESULT cmpCliiPCRReadAll(uint32 iNodeAddr)
{
	HRESULT			hResult = NO_ERROR;
	uint32			iMPR = 0;
	uint32			i;

	hResult = cmpReadiMPR((uint16) iNodeAddr, &iMPR);
	if (hResult != NO_ERROR) return hResult;

	for (i = 0; i < cmpGetMPRNumPlugs(iMPR); i++)
	{
		hResult = cmpCliiPCRRead(i, iNodeAddr);
		if (hResult != NO_ERROR) return hResult;
	}

	return hResult;
}

HRESULT cmpClioPCRReadAll(uint32 oNodeAddr)
{
	HRESULT			hResult = NO_ERROR;
	uint32			oMPR = 0;
	uint32			i;

	hResult = cmpReadoMPR((uint16) oNodeAddr, &oMPR);
	if (hResult != NO_ERROR) return hResult;

	for (i = 0; i < cmpGetMPRNumPlugs(oMPR); i++)
	{
		hResult = cmpClioPCRRead(i, oNodeAddr);
		if (hResult != NO_ERROR) return hResult;
	}

	return hResult;
}

HRESULT cmpCliDisplayInfo(uint32 nodeAddr)
{
	HRESULT			hResult = NO_ERROR;

	hResult = cmpCliiMPRRead(nodeAddr);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpClioMPRRead(nodeAddr);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpCliiPCRReadAll(nodeAddr);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpClioPCRReadAll(nodeAddr);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT cmpCliBCInConnectionEstablish(uint32 iPCRNumber)
{
	HRESULT			hResult = NO_ERROR;

	// create or overlay a broadcast in connection
	hResult = cmpBCInConnectionEstablish(iPCRNumber);

	return hResult;
}

HRESULT cmpCliBCInConnectionBreak(uint32 iPCRNumber)
{
	HRESULT			hResult = NO_ERROR;

	// break a broadcast in connection
	hResult = cmpBCInConnectionBreak(iPCRNumber);

	return hResult;
}

HRESULT cmpCliBCOutConnectionEstablish(uint32 oPCRNumber)
{
	HRESULT			hResult = NO_ERROR;

	// create or overlay a broadcast out connection
	hResult = cmpBCOutConnectionEstablish(oPCRNumber);

	return hResult;
}

HRESULT cmpCliBCOutConnectionBreak(uint32 iPCRNumber)
{
	HRESULT			hResult = NO_ERROR;

	// break a broadcast out connection
	hResult = cmpBCOutConnectionBreak(iPCRNumber);

	return hResult;
}

HRESULT cmpCliP2PInConnectionEstablish(uint32 iPCRNumber, uint32 oPCRNumber, uint16 oNodeAddr)
{
	HRESULT			hResult = NO_ERROR;

	// create or overlay a connection between the local iPCR and the remote oPCR,oNodeAddr specified
	hResult = cmpP2PInConnectionEstablish(iPCRNumber, oPCRNumber, oNodeAddr);

	return hResult;
}

HRESULT cmpCliP2PInConnectionBreak(uint32 iPCRNumber, uint32 oPCRNumber, uint16 oNodeAddr)
{
	HRESULT			hResult = NO_ERROR;

	// break a connection between the local iPCR and the remote oPCR,oNodeAddr specified
	hResult = cmpP2PInConnectionBreak(iPCRNumber, oPCRNumber, oNodeAddr);

	return hResult;
}

static CLIDescriptor   cmpCliiMPRVar_Descriptor[]=
{
	{kCLI_Name,				(uint32)"iMPR"},
	{kCLI_Type, 			kCLI_TYPE_VARIABLE},
	{kCLI_Function,			(uint32)&cmpCliiMPRVarCB},
	{kCLI_VariableSize,		32},
	{kCLI_VariableDisplayType, kCLI_DISPLAYTYPE_HEX},
	{kCLI_VariableBitFields, (uint32)"rcap=0xC0000000,b-ch base=0x1F000000,#iPCR=0x0000001F"},
	{kCLI_ModuleName,		(uint32)"var iMPR"},
	{kCLI_GroupName,		(uint32)"cmp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static CLIDescriptor   cmpClioMPRVar_Descriptor[]=
{
	{kCLI_Name,				(uint32)"oMPR"},
	{kCLI_Type, 			kCLI_TYPE_VARIABLE},
	{kCLI_Function,			(uint32)&cmpClioMPRVarCB},
	{kCLI_VariableSize,		32},
	{kCLI_VariableDisplayType, kCLI_DISPLAYTYPE_HEX},
	{kCLI_VariableBitFields, (uint32)"rcap=0xC0000000,b-ch base=0x1F000000,#oPCR=0x0000001F"},
	{kCLI_ModuleName,		(uint32)"var oMPR"},
	{kCLI_GroupName,		(uint32)"cmp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static CLIDescriptor   cmpCliiPCR0Var_Descriptor[]=
{
	{kCLI_Name,				(uint32)"iPCR[0]"},
	{kCLI_Type, 			kCLI_TYPE_VARIABLE},
	{kCLI_Function,			(uint32)&cmpCliiPCR0VarCB},
	{kCLI_VariableSize,		32},
	{kCLI_VariableDisplayType, kCLI_DISPLAYTYPE_HEX},
	{kCLI_VariableBitFields, (uint32)"on=0x80000000,bc=0x40000000,p2p=0x3F000000,ch=0x003F0000"},
	{kCLI_ModuleName,		(uint32)"var iPCR0"},
	{kCLI_GroupName,		(uint32)"cmp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static CLIDescriptor   cmpClioPCR0Var_Descriptor[]=
{
	{kCLI_Name,				(uint32)"oPCR[0]"},
	{kCLI_Type, 			kCLI_TYPE_VARIABLE},
	{kCLI_Function,			(uint32)&cmpClioPCR0VarCB},
	{kCLI_VariableSize,		32},
	{kCLI_VariableDisplayType, kCLI_DISPLAYTYPE_HEX},
	{kCLI_VariableBitFields, (uint32)"on=0x80000000,bc=0x40000000,p2p=0x3F000000,ch=0x003F0000,rate=0x0000C000,ohead=0x00003C00"},
	{kCLI_ModuleName,		(uint32)"var oPCR0"},
	{kCLI_GroupName,		(uint32)"cmp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static char		cmpCliDisplayInfo_UsageString[]=\
				"cmp:info\n\r"\
				"use: cmp:info <nodeAddr>\n\r"\
				"------------------------------------------------------------\n\r"\
				"display iMPR,oMPR and all iPCR,oPCR plug register info for node\n\r";

static CLIDescriptor		cmpCliDisplayInfo_Descriptor[]=
{
	{kCLI_Name,				(uint32)"info"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cmpCliDisplayInfo},
	{kCLI_UsageString, 		(uint32)&cmpCliDisplayInfo_UsageString},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"display plug register info"},
	{kCLI_GroupName,		(uint32)"cmp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static char		cmpCliBCInConnectionEstablish_UsageString[]=\
				"cmp:bcinestablish\n\r"\
				"use: cmp:bcinestablish <iPCRNumber>\n\r"\
				"------------------------------------------------------------\n\r"\
				"establish a broadcast in connection\n\r";

static CLIDescriptor		cmpCliBCInConnectionEstablish_Descriptor[]=
{
	{kCLI_Name,				(uint32)"bcinestablish"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cmpCliBCInConnectionEstablish},
	{kCLI_UsageString, 		(uint32)&cmpCliBCInConnectionEstablish_UsageString},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"establish a broadcast in connection"},
	{kCLI_GroupName,		(uint32)"cmp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static char		cmpCliBCInConnectionBreak_UsageString[]=\
				"cmp:bcinbreak\n\r"\
				"use: cmp:bcinbreak <iPCRNumber>\n\r"\
				"------------------------------------------------------------\n\r"\
				"break a broadcast in connection\n\r";

static CLIDescriptor		cmpCliBCInConnectionBreak_Descriptor[]=
{
	{kCLI_Name,				(uint32)"bcinbreak"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cmpCliBCInConnectionBreak},
	{kCLI_UsageString, 		(uint32)&cmpCliBCInConnectionBreak_UsageString},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"break a broadcast in connection"},
	{kCLI_GroupName,		(uint32)"cmp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static char		cmpCliBCOutConnectionEstablish_UsageString[]=\
				"cmp:bcoutestablish\n\r"\
				"use: cmp:bcoutestablish <iPCRNumber>\n\r"\
				"------------------------------------------------------------\n\r"\
				"establish a broadcast out connection\n\r";

static CLIDescriptor		cmpCliBCOutConnectionEstablish_Descriptor[]=
{
	{kCLI_Name,				(uint32)"bcoutestablish"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cmpCliBCOutConnectionEstablish},
	{kCLI_UsageString, 		(uint32)&cmpCliBCOutConnectionEstablish_UsageString},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"establish a broadcast out connection"},
	{kCLI_GroupName,		(uint32)"cmp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static char		cmpCliBCOutConnectionBreak_UsageString[]=\
				"cmp:bcoutbreak\n\r"\
				"use: cmp:bcoutbreak <iPCRNumber>\n\r"\
				"------------------------------------------------------------\n\r"\
				"break a broadcast out connection\n\r";

static CLIDescriptor		cmpCliBCOutConnectionBreak_Descriptor[]=
{
	{kCLI_Name,				(uint32)"bcoutbreak"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cmpCliBCOutConnectionBreak},
	{kCLI_UsageString, 		(uint32)&cmpCliBCOutConnectionBreak_UsageString},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"break a broadcast out connection"},
	{kCLI_GroupName,		(uint32)"cmp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static char		cmpCliP2PInConnectionEstablish_UsageString[]=\
				"cmp:p2pinestablish\n\r"\
				"use: cmp:p2pinestablish <iPCRNumber> <oPCRNumber> <oNodeAddr>\n\r"\
				"------------------------------------------------------------\n\r"\
				"establish a p-2-p in connection\n\r";

static CLIDescriptor		cmpCliP2PInConnectionEstablish_Descriptor[]=
{
	{kCLI_Name,				(uint32)"p2pinestablish"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cmpCliP2PInConnectionEstablish},
	{kCLI_UsageString, 		(uint32)&cmpCliP2PInConnectionEstablish_UsageString},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ArgumentPatternString, (uint32)"iii"},
	{kCLI_ModuleName,		(uint32)"establish a p-2-p in connection"},
	{kCLI_GroupName,		(uint32)"cmp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static char		cmpCliP2PInConnectionBreak_UsageString[]=\
				"cmp:p2pinbreak\n\r"\
				"use: cmp:p2pinbreak <iPCRNumber> <oPCRNumber> <oNodeAddr>\n\r"\
				"------------------------------------------------------------\n\r"\
				"break a p-2-p in connection\n\r";

static CLIDescriptor		cmpCliP2PInConnectionBreak_Descriptor[]=
{
	{kCLI_Name,				(uint32)"p2pinbreak"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cmpCliP2PInConnectionBreak},
	{kCLI_UsageString, 		(uint32)&cmpCliP2PInConnectionBreak_UsageString},
	{kCLI_ConstantsString,	(uint32)""},
	{kCLI_ArgumentPatternString, (uint32)"iii"},
	{kCLI_ModuleName,		(uint32)"break a p-2-p in connection"},
	{kCLI_GroupName,		(uint32)"cmp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void cmpCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(cmpCliiMPRVar_Descriptor);
	hResult += cliInstallCLIDescriptor(cmpClioMPRVar_Descriptor);
	hResult += cliInstallCLIDescriptor(cmpClioPCR0Var_Descriptor);
	hResult += cliInstallCLIDescriptor(cmpCliiPCR0Var_Descriptor);

#if 0	
	hResult += cliInstallCLIDescriptor(cmpCliiMPRRead_Descriptor);
	hResult += cliInstallCLIDescriptor(cmpClioMPRRead_Descriptor);
	hResult += cliInstallCLIDescriptor(cmpClioPCRRead_Descriptor);
	hResult += cliInstallCLIDescriptor(cmpCliiPCRRead_Descriptor);

	hResult += cliInstallCLIDescriptor(cmpCliiMPRLock_Descriptor);
	hResult += cliInstallCLIDescriptor(cmpClioMPRLock_Descriptor);
	hResult += cliInstallCLIDescriptor(cmpClioPCRLock_Descriptor);
	hResult += cliInstallCLIDescriptor(cmpCliiPCRLock_Descriptor);
#endif

	hResult += cliInstallCLIDescriptor(cmpCliDisplayInfo_Descriptor);

	hResult += cliInstallCLIDescriptor(cmpCliBCInConnectionEstablish_Descriptor);
	hResult += cliInstallCLIDescriptor(cmpCliBCInConnectionBreak_Descriptor);
	hResult += cliInstallCLIDescriptor(cmpCliBCOutConnectionEstablish_Descriptor);
	hResult += cliInstallCLIDescriptor(cmpCliBCOutConnectionBreak_Descriptor);
	hResult += cliInstallCLIDescriptor(cmpCliP2PInConnectionEstablish_Descriptor);
	hResult += cliInstallCLIDescriptor(cmpCliP2PInConnectionBreak_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing cmp cli tools");
	}
}
#endif //_CLI_TOOL_CMP
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
