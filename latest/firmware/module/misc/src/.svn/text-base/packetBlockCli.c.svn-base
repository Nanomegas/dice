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

 DESCRIPTION:

	Implementation of CLI for the packetBlock Utilities

  Revisions:
		created 14/10/2003 LM

****************************************************************************/

#ifdef _1394CORE


#ifdef _CLI
#include "cliToolDefs.h"
#include "TCTasking.h"
#include "packetBlock.h"
#include "format.h"
#include "errorText.h"
#include "cli.h"

#ifdef _SYSDEBUG

static char	moduleName[] = __FILE__;

static char *packetTypeText(PB_PACKETTYPE type)
{
	switch (type)
	{
		case PB_TYPE_WRITE_REQUEST:			return("PB_TYPE_WRITE_REQUEST");
    	case PB_TYPE_WRITE_REQUEST_QUADLET:	return("PB_TYPE_WRITE_REQUEST_QUADLET");
		case PB_TYPE_WRITE_RESPONSE:		return("PB_TYPE_WRITE_RESPONSE");
		case PB_TYPE_READ_REQUEST:			return("PB_TYPE_READ_REQUEST");
		case PB_TYPE_READ_REQUEST_QUADLET:	return("PB_TYPE_READ_REQUEST_QUADLET");
		case PB_TYPE_READ_RESPONSE:			return("PB_TYPE_READ_RESPONSE");
		case PB_TYPE_READ_RESPONSE_QUADLET:	return("PB_TYPE_READ_RESPONSE_QUADLET");
		case PB_TYPE_LOCK_REQUEST:			return("PB_TYPE_LOCK_REQUEST");
		case PB_TYPE_LOCK_RESPONSE:			return("PB_TYPE_LOCK_RESPONSE");
		case PB_TYPE_ASYNC_STREAM:			return("PB_TYPE_ASYNC_STREAM");
	}
	
	return("unknown");
}

void pbCliPrintPB(PB *pPB, char *preface)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i;
	uint32		bytesThisRow;
	uint8		*payloadPtr;
	char		str[20];

	if (!pbIsValid(pPB))
	{
		hResult = E_PB_BAD_PACKET_BLOCK_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return;
	}

	sysDebugPrintf("%s packetSeqNum:    %d\n\r", preface, pPB->packetSeqNum);
	sysDebugPrintf("%s busGeneration:   %d\n\r", preface, pPB->busGeneration);
	sysDebugPrintf("%s nodeHandle:      0x%08x\n\r", preface, pPB->nodeHandle);
	sysDebugPrintf("%s finalStatus:     %d (%s)\n\r", preface, pPB->finalStatus, errorText(pPB->finalStatus));
	sysDebugPrintf("%s packetType:      %d (%s)\n\r", preface, pPB->packetType, packetTypeText(pPB->packetType));


	sysDebugPrintf("%s callerIdCreate:    0x%08x\n\r", preface, pPB->callerIdCreate);
	sysDebugPrintf("%s callerIdDoneIndex:   %d\n\r", preface, pPB->callerIdDoneIndex);
	for (i = 0; i < pPB->callerIdDoneIndex; i++)
	{
		sysDebugPrintf("%s callerIdDone[%d]:    %0x%08x\n\r", preface, i, pPB->callerIdDone[i]);
	}

	// packet header
	sysDebugPrintf("%s packetHeader:    ", preface);
	for (i = 0; i < ARRAYSIZE(pPB->packetHeader.quadlets); ++i)
	{
		if (i != 0)
		{
			sysDebugPrintf("%s                  ", preface);
		}
		sprintf(str, "0x%08x\n\r", pPB->packetHeader.quadlets[i]);
		sysDebugPrintf("%s", str);
	}
	sysDebugPrintf("%s payloadByteSize: %d\n\r", preface, pPB->payloadByteSize);
	if (pPB->payloadByteSize > 0)
	{
		// payload
		sysDebugPrintf("%s payload: ", preface);
		for (i = 0, bytesThisRow = 0, payloadPtr = pPB->pPayload; i < pPB->payloadByteSize; ++i)
		{
			if ((i != 0) && (bytesThisRow == 0))
			{
				sysDebugPrintf("%s          ", preface);
			}
			sprintf(str, " 0x%02x", payloadPtr[i]);
			sysDebugPrintf("%s", str);
			++bytesThisRow;
			if (bytesThisRow >= 8)
			{
				bytesThisRow = 0;
				sysDebugPrintf("\n\r");
			}
		}
		if ((bytesThisRow != 0) || (i == 0))
		{
			sysDebugPrintf("\n\r");
		}
	}
}
#endif //_SYSDEBUG

#ifdef _CLI_TOOL_PB

static char str[120];

static HRESULT	pbCreatorCallback(PB *pb)
{
	HRESULT	hResult = NO_ERROR;

	sprintf(str, "   creator called back with pb = %x\n\r", (uint32)pb);
	cliOutputLine(str);
	pbPacketDone(pb, PB_DONE_PB_CB_CREATOR);

	return hResult;
}	

static HRESULT	pbTestCreate(uint32 numBytes)
{
	HRESULT	hResult = NO_ERROR;
	PB		*pb = NULL;
	
	hResult = pbCreatePacket (&pb, 1234, (PB_CALLBACK)pbCreatorCallback, numBytes, PB_CREATE_PB_TEST_CREATE);
	if (hResult == NO_ERROR)
	{
		sprintf(str, "   packet created, *pb = %x\n\r", (uint32)pb);
		cliOutputLine(str);
	}

	return hResult;
}

static HRESULT	pbTestDuplicate(PB *pbOrig)
{
	HRESULT	hResult = NO_ERROR;
	PB		*pbNew = NULL;
	
	hResult = pbCreateDuplicatePacket (pbOrig, &pbNew, (PB_CALLBACK)pbCreatorCallback, PB_CREATE_PB_TEST_CREATE);
	if (hResult == NO_ERROR)
	{
		sprintf(str, "   duplicate packet created, *pb = %x\n\r", (uint32)pbNew);
		cliOutputLine(str);
	}

	return hResult;
}

static HRESULT	pbTestDone(PB *pb)
{
	HRESULT	hResult = NO_ERROR;

	pbPacketDone(pb, PB_DONE_PB_TEST_DONE);

	return hResult;	
}	

static  char	pbCreate_UsageString[]=\
				"pb:create \n\r"\
				"use: pb:create <numPayloadBytes>\n\r"\
				"------------------------------------------------------------\n\r"\
				"create a packet block";

static	CLIDescriptor	pbCreate_Descriptor[]=
{
	{kCLI_Name,				(uint32)"create"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&pbTestCreate},
	{kCLI_UsageString, 		(uint32)pbCreate_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"pb create"},
	{kCLI_GroupName,		(uint32)"pb"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	pbDuplicate_UsageString[]=								\
				"pb:duplicate \n\r"\
				"use: pb:duplicate <original packetBlock address>\n\r"\
				"------------------------------------------------------------\n\r"\
				"duplicates a packet block";
 
static	CLIDescriptor	pbDuplicate_Descriptor[]=
{
	{kCLI_Name,				(uint32)"pbDuplicate"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&pbTestDuplicate},   
	{kCLI_UsageString, 		(uint32)pbDuplicate_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"pb duplicate"},
	{kCLI_GroupName,		(uint32)"pb"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	pbDone_UsageString[]=						\
				"pb:done \n\r"\
				"use: pb:done <original packetBlock address>\n\r"\
				"------------------------------------------------------------\n\r"\
				"finishes a packet block life";
 
static	CLIDescriptor	pbDone_Descriptor[]=
{
	{kCLI_Name,				(uint32)"done"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&pbTestDone},
	{kCLI_UsageString, 		(uint32)pbDone_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"pb done"},
	{kCLI_GroupName,		(uint32)"pb"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void pbCliInstallTools(void)
{
	HRESULT	hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(pbCreate_Descriptor);
	hResult += cliInstallCLIDescriptor(pbDuplicate_Descriptor);
	hResult += cliInstallCLIDescriptor(pbDone_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing pb tools");
	}
}	
#endif //_CLI_TOOL_PB
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP

#endif //_1394CORE
