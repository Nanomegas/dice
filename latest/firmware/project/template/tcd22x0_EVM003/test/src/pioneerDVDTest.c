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
 * DESCRIPTION: 

	Pioneer DVD Test Module CLI for testing CMP AVC and AKE
		
	Revisions:
		created 07/08/2003 lm

******************************************************************************/

#ifdef _CLI
#ifdef _AVC
#include "cliToolDefs.h"
#include "TCTasking.h"
#include "cli.h"
#include "pioneerDVDTest.h"
#include "cmpUtils.h"
#include "cmpP2P.h"
#include "lal.h"
#include "avcCmdCli.h"
#ifdef _DTCP
#include "akeSink.h"
#include "akeCommand.h"
#endif //_DTCP

//static char 		moduleName[] = __FILE__ ;

HRESULT	pioneerDVDInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

#ifdef _CLI_TOOL_PIONEER_DVD
	pioneerDVDCliInstallTools();
#endif //_CLI_TOOL_PIONEER_DVD

	return hResult;
}

#ifdef _CLI_TOOL_PIONEER_DVD

static uint32	pioneerDVDiPCRNumber = 0;
static uint32	pioneerDVDoPCRNumber = 0; 

HRESULT pioneerDVDGetHandle(LAL_NODE_HANDLE *pHandle)
{
	return avcCmdCliGetTargetHandle(pHandle);
}

HRESULT pioneerDVDGetNodeAddr(uint32 *nodeAddr)
{
	return avcCmdCliGetTargetNodeAddr(nodeAddr);
}

HRESULT pioneerDVDSetIOPCR(uint32 iPCRNumber, uint32 oPCRNumber)
{
	HRESULT		hResult = NO_ERROR;

	pioneerDVDiPCRNumber = iPCRNumber;
	pioneerDVDoPCRNumber = oPCRNumber; 

	return hResult;
}

HRESULT pioneerDVDIsEstablished(BOOL *bEstablished, BYTE *channel, BYTE *plugID)
{
	HRESULT		hResult = NO_ERROR;
	uint32		oNodeAddr = 0;
	uint32		iPCR = 0;

	hResult = pioneerDVDGetNodeAddr(&oNodeAddr);
	if (hResult != NO_ERROR) return hResult;

	*bEstablished = FALSE;

	cmpP2PInIsConnectionEstablished(pioneerDVDiPCRNumber, pioneerDVDoPCRNumber, (uint16) oNodeAddr, bEstablished);

	if (*bEstablished)
	{
		hResult = cmpGetiPCR(pioneerDVDiPCRNumber, &iPCR);
		if (hResult != NO_ERROR) return hResult;

		*channel = (BYTE) cmpGetPCRChannel(iPCR);
		*plugID = (BYTE) pioneerDVDiPCRNumber;
	}

	return hResult;
}

HRESULT pioneerDVDEstablish(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		oNodeAddr = 0;

	hResult = pioneerDVDGetNodeAddr(&oNodeAddr);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpP2PInConnectionEstablish(pioneerDVDiPCRNumber, pioneerDVDoPCRNumber, (uint16) oNodeAddr);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT pioneerDVDBreak(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		oNodeAddr = 0;

	hResult = pioneerDVDGetNodeAddr(&oNodeAddr);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpP2PInConnectionBreak(pioneerDVDiPCRNumber, pioneerDVDoPCRNumber, (uint16) oNodeAddr);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT pioneerDVDStartAke(BYTE akeProcedure, BYTE exchangeKey, BOOL bForceStart)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bEstablished = FALSE;
	BYTE		channel = 0;
	BYTE		plugID = 0;

	hResult = pioneerDVDIsEstablished(&bEstablished, &channel, &plugID);
	if (hResult != NO_ERROR) return hResult;
	
	if (bEstablished == FALSE)
	{
		TCTaskWait(50);

		hResult = pioneerDVDEstablish();
		if (hResult != NO_ERROR) return hResult;

		TCTaskWait(50);

		hResult = pioneerDVDIsEstablished(&bEstablished, &channel, &plugID);
		if (hResult != NO_ERROR) return hResult;

		if (bEstablished == FALSE)
		{
			hResult = E_FAIL;
			return hResult;
		}
	}

#ifdef _DTCP
	{
		LAL_NODE_HANDLE		talkToNodehandle = 0;

		TCTaskWait(50);

		hResult = pioneerDVDGetHandle(&talkToNodehandle);
		if (hResult != NO_ERROR) return hResult;

		TCTaskWait(50);

		hResult = akeSinkStartAKE(talkToNodehandle, akeProcedure, exchangeKey, channel, plugID, bForceStart);
		if (hResult != NO_ERROR) return hResult;
	}
#endif //_DTCP

	return hResult;
}

HRESULT pioneerDVDStartFull(void)
{
#ifdef _DTCP
	return pioneerDVDStartAke(AKE_PROCEDURE_REST_FULL_AUTH, AKE_EXCHANGE_KEY_COPY_NEVER, FALSE);
//	return pioneerDVDStartAke(AKE_PROCEDURE_REST_FULL_EX_FULL_AUTH, AKE_EXCHANGE_KEY_COPY_NEVER, FALSE);
//	return pioneerDVDStartAke(AKE_PROCEDURE_REST_FULL_EX_FULL_AUTH, AKE_EXCHANGE_KEY_COPY_ONCE, FALSE);
#else
	return E_NOT_IMPLEMENTED;
#endif
}

HRESULT pioneerDVDStartRest(void)
{
#ifdef _DTCP
#ifdef _DTLA_SUPPORT_REST_AUTH_ONLY
	return pioneerDVDStartAke(AKE_PROCEDURE_REST_EN_REST_AUTH, AKE_EXCHANGE_KEY_NO_MORE_COPIES, FALSE);
#else //_DTLA_SUPPORT_REST_AUTH_ONLY
	return pioneerDVDStartAke(AKE_PROCEDURE_REST_AUTH, AKE_EXCHANGE_KEY_NO_MORE_COPIES, FALSE);
#endif //_DTLA_SUPPORT_REST_AUTH_ONLY
#else
	return E_NOT_IMPLEMENTED;
#endif
}

HRESULT pioneerDVDStart(void)
{
#ifdef _AKE_FULL_AUTH
	return pioneerDVDStartFull();
#else //_AKE_FULL_AUTH
	return pioneerDVDStartRest();
#endif //_AKE_FULL_AUTH
}

HRESULT pioneerDVDContentKeyRequest(void)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bEstablished = FALSE;
	BYTE		channel = 0;
	BYTE		plugID = 0;

	hResult = pioneerDVDIsEstablished(&bEstablished, &channel, &plugID);
	if (hResult != NO_ERROR) return hResult;
	
#ifdef _DTCP
	if (bEstablished == TRUE)
	{
		TCTaskWait(50);

		hResult = akeCommandSendContentKeyReq(channel);
		if (hResult != NO_ERROR) return hResult;
	}
#endif //_DTCP

	return hResult;
}

HRESULT pioneerDVDStop(BOOL bStopMessage)
{
	HRESULT				hResult = NO_ERROR;
	BOOL				bEstablished = FALSE;
	BYTE				channel = 0;
	BYTE				plugID = 0;

#ifdef _DTCP
	if (bStopMessage == TRUE)
	{
		LAL_NODE_HANDLE		talkToNodehandle = 0;

		TCTaskWait(50);

		hResult = pioneerDVDGetHandle(&talkToNodehandle);
		if (hResult != NO_ERROR) return hResult;

		TCTaskWait(50);

		hResult = akeSinkStopAKE(talkToNodehandle, channel, plugID);
		if (hResult != NO_ERROR) return hResult;
	}
#endif //_DTCP

	hResult = pioneerDVDIsEstablished(&bEstablished, &channel, &plugID);
	if (hResult != NO_ERROR) return hResult;
	
	if (bEstablished == TRUE)
	{
		TCTaskWait(50);

		hResult = pioneerDVDBreak();
		if (hResult != NO_ERROR) return hResult;
	}

	return hResult;
}


// Pioneer DVD Set i/o PCR numbers
static  char	pioneerDVDSetIOPCR_UsageString[]=\
				"pioneer:iopcr: \n\r"\
				"use: pioneer:iopcr <iPCRNumber> <oPCRNumber>\n\r"\
				"------------------------------------------------------------\n\r"\
				"setup i/o PCR numbers for connections\n\r";

static	CLIDescriptor	pioneerDVDSetIOPCR_Descriptor[]=
{
	{kCLI_Name,				(uint32)"iopcr"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&pioneerDVDSetIOPCR},
	{kCLI_UsageString, 		(uint32)pioneerDVDSetIOPCR_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ii"},
	{kCLI_ModuleName,		(uint32)"setup i/o PCR numbers for connections"},
	{kCLI_GroupName,		(uint32)"pioneer"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// Pioneer DVD Establish Connection
static  char	pioneerDVDEstablish_UsageString[]=\
				"pioneer:establish: \n\r"\
				"use: pioneer:establish\n\r"\
				"------------------------------------------------------------\n\r"\
				"establish point-to-point connection to pioneer DVD\n\r"\
				"causing pioneer DVD to start isoch streaming\n\r";

static	CLIDescriptor	pioneerDVDEstablish_Descriptor[]=
{
	{kCLI_Name,				(uint32)"establish"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&pioneerDVDEstablish},
	{kCLI_UsageString, 		(uint32)pioneerDVDEstablish_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"establish pioneer DVD isoch stream connection"},
	{kCLI_GroupName,		(uint32)"pioneer"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// Pioneer DVD Break Connection
static  char	pioneerDVDBreak_UsageString[]=\
				"pioneer:break: \n\r"\
				"use: pioneer:break\n\r"\
				"------------------------------------------------------------\n\r"\
				"break point-to-point connection to pioneer DVD\n\r"\
				"causing pioneer DVD to stop isoch streaming\n\r";

static	CLIDescriptor	pioneerDVDBreak_Descriptor[]=
{
	{kCLI_Name,				(uint32)"break"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&pioneerDVDBreak},
	{kCLI_UsageString, 		(uint32)pioneerDVDBreak_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"break pioneer DVD isoch stream connection"},
	{kCLI_GroupName,		(uint32)"pioneer"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// Pioneer DVD Start AKE
static  char	pioneerDVDStart_UsageString[]=\
				"pioneer:start: \n\r"\
				"use: pioneer:start\n\r"\
				"------------------------------------------------------------\n\r"\
				"start pioneer DVD AKE and isoch streaming\n\r";

static	CLIDescriptor	pioneerDVDStart_Descriptor[]=
{
	{kCLI_Name,				(uint32)"start"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&pioneerDVDStart},
	{kCLI_UsageString, 		(uint32)pioneerDVDStart_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"start pioneer DVD AKE and isoch streaming"},
	{kCLI_GroupName,		(uint32)"pioneer"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// Pioneer DVD Start AKE
static  char	pioneerDVDStartAke_UsageString[]=\
				"pioneer:ake: \n\r"\
				"use: pioneer:ake <akeProcedure> <exchangeKey> <forceStart>\n\r"\
				"akeProcedure: set bits: 0:rest, 1:en rest, 2: full 3:ex full\n\r"\
				"exchangeKey: 0: copy free, set bits: 0:never, 1:once, 2:no more\n\r"\
				"forceStart: on|off whether to force a restart of AKE even if already started\n\r"\
				"------------------------------------------------------------\n\r"\
				"start pioneer DVD AKE with setup and isoch streaming\n\r";

static	CLIDescriptor	pioneerDVDStartAke_Descriptor[]=
{
	{kCLI_Name,				(uint32)"ake"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&pioneerDVDStartAke},
	{kCLI_UsageString, 		(uint32)pioneerDVDStartAke_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iii"},
	{kCLI_ModuleName,		(uint32)"start pioneer DVD AKE with setup and isoch streaming"},
	{kCLI_GroupName,		(uint32)"pioneer"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// Pioneer DVD Stop AKE
static  char	pioneerDVDStop_UsageString[]=\
				"pioneer:stop: \n\r"\
				"use: pioneer:stop <bStopMessage>\n\r"\
				"bStopMessage: <on|off> send explicit stop message\n\r"\
				"------------------------------------------------------------\n\r"\
				"stop pioneer DVD AKE and isoch streaming\n\r";

static	CLIDescriptor	pioneerDVDStop_Descriptor[]=
{
	{kCLI_Name,				(uint32)"stop"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&pioneerDVDStop},
	{kCLI_UsageString, 		(uint32)pioneerDVDStop_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"stop pioneer DVD AKE and isoch streaming"},
	{kCLI_GroupName,		(uint32)"pioneer"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// Pioneer DVD Start Full AKE
static  char	pioneerDVDFull_UsageString[]=\
				"pioneer:full: \n\r"\
				"use: pioneer:full\n\r"\
				"------------------------------------------------------------\n\r"\
				"start pioneer DVD AKE with Full Auth and isoch streaming\n\r";

static	CLIDescriptor	pioneerDVDFull_Descriptor[]=
{
	{kCLI_Name,				(uint32)"full"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&pioneerDVDStartFull},
	{kCLI_UsageString, 		(uint32)pioneerDVDFull_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"start pioneer DVD AKE with Full Auth and isoch streaming"},
	{kCLI_GroupName,		(uint32)"pioneer"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// Pioneer DVD Start Rest AKE
static  char	pioneerDVDRest_UsageString[]=\
				"pioneer:rest: \n\r"\
				"use: pioneer:rest\n\r"\
				"------------------------------------------------------------\n\r"\
				"start pioneer DVD AKE with rest Auth and isoch streaming\n\r";

static	CLIDescriptor	pioneerDVDRest_Descriptor[]=
{
	{kCLI_Name,				(uint32)"rest"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&pioneerDVDStartRest},
	{kCLI_UsageString, 		(uint32)pioneerDVDRest_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"start pioneer DVD AKE with rest Auth and isoch streaming"},
	{kCLI_GroupName,		(uint32)"pioneer"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// Pioneer DVD Send Content Key Request
static  char	pioneerDVDContentKeyRequest_UsageString[]=\
				"pioneer:stop: \n\r"\
				"use: pioneer:contentkey\n\r"\
				"------------------------------------------------------------\n\r"\
				"send pioneer DVD content key request\n\r";

static	CLIDescriptor	pioneerDVDContentKeyRequest_Descriptor[]=
{
	{kCLI_Name,				(uint32)"contentkey"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&pioneerDVDContentKeyRequest},
	{kCLI_UsageString, 		(uint32)pioneerDVDContentKeyRequest_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"send pioneer DVD content key request"},
	{kCLI_GroupName,		(uint32)"pioneer"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void pioneerDVDCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(pioneerDVDSetIOPCR_Descriptor);
	hResult += cliInstallCLIDescriptor(pioneerDVDEstablish_Descriptor);
	hResult += cliInstallCLIDescriptor(pioneerDVDBreak_Descriptor);
	hResult += cliInstallCLIDescriptor(pioneerDVDStart_Descriptor);
	hResult += cliInstallCLIDescriptor(pioneerDVDStartAke_Descriptor);
	hResult += cliInstallCLIDescriptor(pioneerDVDStop_Descriptor);
	hResult += cliInstallCLIDescriptor(pioneerDVDFull_Descriptor);
	hResult += cliInstallCLIDescriptor(pioneerDVDRest_Descriptor);
	hResult += cliInstallCLIDescriptor(pioneerDVDContentKeyRequest_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing pioneerDVD cli tools");
	}
}

#endif //_CLI_TOOL_PIONEER_DVD
#endif //_AVC
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
