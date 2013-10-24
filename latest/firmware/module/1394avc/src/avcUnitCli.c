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

	Implementation of CLI for the avc unit
	
  Revisions:
		created 08/04/2003 LM

****************************************************************************/

#ifdef _CLI
#include "cliToolDefs.h"
#include "TCTasking.h"
#include "avc.h"
#include "avcUnitCli.h"
#include "avcUnitNotify.h"
#include "avcUnitPower.h"
#include "avcUnitReserve.h"
#include "avcUnitChannelUsage.h"
#include "avcUnitConnect.h"
#include "avcUnitConnectAV.h"
#include "avcUnitSignalFormat.h"				// inputPlugSignalFormat, outputPlugSignalFormat
#include "avcUnitSignalSource.h"
#include "avcCmdCli.h"
#include "avcUnit.h"
#include "avcDefs.h"
#include "lal.h"
#include "datastream.h"
#include "cli.h"
#include "targetPlugs.h"
#include "targetVendorDefs.h"
#include "format.h"

static char 		moduleName[] = __FILE__ ;

//static uint32 avcUnitPrintMode = AVC_UNIT_PRINT_RCV_COMMAND_ALL | AVC_UNIT_PRINT_RCV_RESPONSE_ALL;
//static uint32 avcUnitPrintMode = AVC_UNIT_PRINT_SND_COMMAND_ALL | AVC_UNIT_PRINT_SND_RESPONSE_ALL | AVC_UNIT_PRINT_RCV_COMMAND_ALL | AVC_UNIT_PRINT_RCV_RESPONSE_ALL;
static uint32 avcUnitPrintMode = AVC_UNIT_PRINT_RCV_RAW | AVC_UNIT_PRINT_SND_RAW | AVC_UNIT_PRINT_SND_COMMAND_ALL | AVC_UNIT_PRINT_SND_RESPONSE_ALL | AVC_UNIT_PRINT_RCV_COMMAND_ALL | AVC_UNIT_PRINT_RCV_RESPONSE_ALL;

static uint32 avcUnitReceivedCommands = 0;
static uint32 avcUnitReceivedResponses = 0;

static const char avcUnitCliPrintModeNames[AVC_UNIT_PRINT_ITEMS][64] =
{
	"AVC_UNIT_PRINT_RCV_RAW                      = 0x00000100",
	"AVC_UNIT_PRINT_RCV_COMMAND_ALL              = 0x0000000F",
	"AVC_UNIT_PRINT_RCV_RESPONSE_IMPLEMENTED     = 0x00000010",
	"AVC_UNIT_PRINT_RCV_RESPONSE_NOT_IMPLEMENTED = 0x00000020",
	"AVC_UNIT_PRINT_RCV_RESPONSE_ALL             = 0x000000F0",
	
	"AVC_UNIT_PRINT_SND_RAW                      = 0x01000000",
	"AVC_UNIT_PRINT_SND_COMMAND_ALL              = 0x000F0000",
	"AVC_UNIT_PRINT_SND_RESPONSE_IMPLEMENTED     = 0x00100000",
	"AVC_UNIT_PRINT_SND_RESPONSE_NOT_IMPLEMENTED = 0x00200000",
	"AVC_UNIT_PRINT_SND_RESPONSE_ALL             = 0x00F00000"
};

BOOL avcUnitCliIsPrintMode(uint32 mode)
{
	return  ((avcUnitPrintMode & mode) == mode);
}

uint32 avcUnitCliGetPrintMode(void)
{
	return avcUnitPrintMode;
}

HRESULT avcUnitCliSetPrintMode(uint32 mode)
{
	HRESULT		hResult = NO_ERROR;

	avcUnitPrintMode = mode;

	return hResult;
}

HRESULT avcUnitCliDisplayPrintMode(void)
{
	HRESULT		hResult = NO_ERROR;

	cliPrintf("avcUnitPrintMode: 0x%08x\n\r", avcUnitCliGetPrintMode());

	return hResult;
}

HRESULT avcUnitCliShowPrintModes(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i;

	cliPrintf("avcUnitCliPrintModeNames:\n\r");
	for (i = 0; i < AVC_UNIT_PRINT_ITEMS; i++)
	{
		cliPrintf("%s\n\r", avcUnitCliPrintModeNames[i]);
	}

	return hResult;
}

void avcUnitIncrementReceivedCommands(void)
{
	avcUnitReceivedCommands++;
}

uint32 avcUnitGetReceivedCommands(void)
{
	return avcUnitReceivedCommands;
}

void avcUnitIncrementReceivedResponses(void)
{
	avcUnitReceivedResponses++;
}

uint32 avcUnitGetReceivedResponses(void)
{
	return avcUnitReceivedResponses;
}

// AV/C Frame functions
void avcUnitCliPrintCtype(uint32 ctype)
{
	cliPrintf("ctype:\t\t(0x%02x) ", ctype);

	switch (ctype)
	{
		// command type			
		case AVC_CTYPE_CONTROL:					cliOutputLine("AVC_CTYPE_CONTROL"); break;
		case AVC_CTYPE_STATUS:					cliOutputLine("AVC_CTYPE_STATUS"); break;
		case AVC_CTYPE_SPECIFIC_INQUIRY:		cliOutputLine("AVC_CTYPE_SPECIFIC_INQUIRY"); break;
		case AVC_CTYPE_NOTIFY:					cliOutputLine("AVC_CTYPE_NOTIFY"); break;
		case AVC_CTYPE_GENERAL_INQUIRY:			cliOutputLine("AVC_CTYPE_GENERAL_INQUIRY"); break;

		// response type			
		case AVC_RESPONSE_NOT_IMPLEMENTED:		cliOutputLine("AVC_RESPONSE_NOT_IMPLEMENTED"); break;
		case AVC_RESPONSE_ACCEPTED:				cliOutputLine("AVC_RESPONSE_ACCEPTED"); break;
		case AVC_RESPONSE_REJECTED:				cliOutputLine("AVC_RESPONSE_REJECTED"); break;
		case AVC_RESPONSE_IN_TRANSITION:		cliOutputLine("AVC_RESPONSE_IN_TRANSITION"); break;
		case AVC_RESPONSE_IMPLEMENTED:			cliOutputLine("AVC_RESPONSE_STABLE (IMPLEMENTED)"); break;
		case AVC_RESPONSE_CHANGED:				cliOutputLine("AVC_RESPONSE_CHANGED"); break;
		case AVC_RESPONSE_INTERIM:				cliOutputLine("AVC_RESPONSE_INTERIM"); break;
		default:								cliPrintf("AVC ctype: 0x%02x\n\r", ctype); break;
	}
}

void avcUnitCliPrintSubunitType(SUBUNIT_TYPE subunit_type)
{
	cliPrintf("subunit_type:\t(0x%02x) ", subunit_type);
	switch (subunit_type)
	{
		case AVC_SU_TYPE_MONITOR:				cliOutputLine("AVC_SU_TYPE_MONITOR"); break;
		case AVC_SU_TYPE_AUDIO:					cliOutputLine("AVC_SU_TYPE_AUDIO"); break;
		case AVC_SU_TYPE_MUSIC:					cliOutputLine("AVC_SU_TYPE_MUSIC"); break;
		case AVC_SU_TYPE_PRINTER:				cliOutputLine("AVC_SU_TYPE_PRINTER"); break;
		case AVC_SU_TYPE_DISC:					cliOutputLine("AVC_SU_TYPE_DISC"); break;
		case AVC_SU_TYPE_TAPE:					cliOutputLine("AVC_SU_TYPE_TAPE"); break;
		case AVC_SU_TYPE_TUNER:					cliOutputLine("AVC_SU_TYPE_TUNER"); break;
		case AVC_SU_TYPE_CA:					cliOutputLine("AVC_SU_TYPE_CA"); break;
		case AVC_SU_TYPE_CAMERA:				cliOutputLine("AVC_SU_TYPE_CAMERA"); break;
		case AVC_SU_TYPE_PANEL:					cliOutputLine("AVC_SU_TYPE_PANEL"); break;
		case AVC_SU_TYPE_BULLETIN_BOARD:		cliOutputLine("AVC_SU_TYPE_BULLETIN_BOARD"); break;
		case AVC_SU_TYPE_CAMERA_STORAGE:		cliOutputLine("AVC_SU_TYPE_CAMERA_STORAGE"); break;
		case AVC_SU_TYPE_VENDOR_UNIQUE:			cliOutputLine("AVC_SU_TYPE_VENDOR_UNIQUE"); break;
		case AVC_SU_TYPE_EXTEND_FIRST:			cliOutputLine("AVC_SU_TYPE_EXTEND_FIRST"); break;
		case AVC_SU_TYPE_UNIT:					cliOutputLine("AVC_SU_TYPE_UNIT"); break;
		default:								cliPrintf("AVC subunit Type: 0x%x\n\r", subunit_type); break;
	}
}

void avcUnitCliPrintSubunitID(SUBUNIT_ID subunit_ID)
{
	cliPrintf("subunit_ID:\t%i\n\r", subunit_ID);
}

void avcUnitCliPrintSubunit(AVC_ADDRESS_SUBUNIT *subunit)
{
	avcUnitCliPrintSubunitType(subunit->subunit_type);
	avcUnitCliPrintSubunitID(subunit->subunit_ID);
}

void avcUnitCliPrintUnitPlugID(BYTE plugID, BOOL bInput)
{
	cliPrintf("plugID:\t\t[0x%02X] ", plugID);

	// unit plug addresses

	if		(plugID <= 0x1E)	cliPrintf("%s[%i]\n\r", (bInput) ? "Isoch Input Plug iPCR" : "Isoch Output Plug oPCR", plugID-0x00);	// 00-1E: Isoch I/O Plug iPCR/oPCR [0;30]
	else if (plugID <= 0x3F)	cliPrintf("Reserved\n\r");																				// 1F-3F: Reserved
	else if (plugID <= 0x5E)	cliPrintf("%s %i\n\r", (bInput) ? "General Bus Input Plug" : "General Bus Output Plug", plugID-0x40);	// 40-5E: General Bus I/O Plug [0;30]
	else if (plugID <= 0x7E)	cliPrintf("Reserved\n\r");																				// 5F-7E: Reserved
	else if (plugID == 0x7F)	cliPrintf("%s\n\r", (bInput) ? "Any available Isoch Input Plug" : "Any available Isoch Output Plug");	//    7F: Any available Isoch I/O Plug
	else if (plugID <= 0x9E)	cliPrintf("%s %i\n\r", (bInput) ? "External Input Plug" : "External Output Plug", plugID-0x80);			// 80-9E: External I/O Plug [0;30]
	else if (plugID == 0x9F)	cliPrintf("Reserved\n\r");																				//    9F: Reserved
	else if (plugID <= 0xBE)	cliPrintf("%s %i\n\r", (bInput) ? "Async Input Plug" : "Async Output Plug", plugID-0xA0);				// A0-BE: Async I/O Plug [0;30]
	else if (plugID == 0xBF)	cliPrintf("%s\n\r", (bInput) ? "Any available Async Input Plug" : "Any available Async Output Plug");	//    BF: Any available Async I/O Plug
	else if (plugID <= 0xFC)	cliPrintf("Reserved\n\r");																				// C0-FC: Reserved
	else if (plugID == 0xFD)	cliPrintf("%s\n\r", (bInput) ? "Reserved" : "Multiple Plugs");											//    FD: I:Reserved, O:Multiple Plugs
	else if (plugID == 0xFE)	cliPrintf("Invalid\n\r");																				//    FE: Invalid
	else if (plugID == 0xFF)	cliPrintf("%s\n\r", (bInput) ? "Any available Extern Input Plug" : "Any available Extern Output Plug");	//    FF: Any available External I/O Plug
	else						cliPrintf("%s\n\r", (bInput) ? "Unknown Input PlugID" : "Unknown Output PlugID");				//    --: ??
}

void avcUnitCliPrintSubunitPlugID(BYTE plugID, BOOL bSource)
{
	cliPrintf("plugID:\t\t[0x%02X] ", plugID);

	// subunit plug addresses
	if		(plugID <= 0x1E)	cliPrintf("%s %i\n\r", (bSource) ? "Source Plug" : "Destination Plug", plugID-0x00);					// 00-1E: Src/Dst Plug [0;30]
	else if (plugID <= 0xFC)	cliPrintf("Reserved\n\r");																				// 1F-FC: Reserved
	else if (plugID == 0xFD)	cliPrintf("%s\n\r", (bSource) ? "Reserved" : "Multiple Plugs");											//    FD: I:Reserved, O:Multiple Plugs
	else if (plugID == 0xFE)	cliPrintf("Invalid\n\r");																				//    FE: Invalid
	else if (plugID == 0xFF)	cliPrintf("%s\n\r", (bSource) ? "Any available Source Plug" : "Any available Destination Plug");		//    FF: Any available Src/Dst I/O Plug
	else						cliPrintf("%s\n\r", (bSource) ? "Unknown Source PlugID" : "Unknown Destination PlugID");				//    --: ??
}

void avcUnitCliPrintPlug(AVC_ADDRESS_PLUG *plug, BOOL bInputOrSourcePlug)
{
	avcUnitCliPrintSubunit(&plug->addrSubunit);
	if (avcSubunitIsUnit(&plug->addrSubunit))
	{
		avcUnitCliPrintUnitPlugID(plug->plugID, bInputOrSourcePlug);
	}
	else
	{
		avcUnitCliPrintSubunitPlugID(plug->plugID, bInputOrSourcePlug);
	}
}

void avcUnitCliPrintOpcode(uint32 opcode)
{
	cliPrintf("opcode:\t\t");

	switch (opcode)
	{
		// unit commands												
		case AVC_CMD_DIGITAL_OUTPUT:			cliOutputLine("AVC_CMD_DIGITAL_OUTPUT"); break;
		case AVC_CMD_DIGITAL_INPUT:				cliOutputLine("AVC_CMD_DIGITAL_INPUT"); break;
		case AVC_CMD_CHANNEL_USAGE:				cliOutputLine("AVC_CMD_CHANNEL_USAGE"); break;
		case AVC_CMD_OUTPUT_PLUG_SIGNAL_FORMAT:	cliOutputLine("AVC_CMD_OUTPUT_PLUG_SIGNAL_FORMAT"); break;
		case AVC_CMD_INPUT_PLUG_SIGNAL_FORMAT:	cliOutputLine("AVC_CMD_INPUT_PLUG_SIGNAL_FORMAT"); break;
		case AVC_CMD_CONNECT_AV:				cliOutputLine("AVC_CMD_CONNECT_AV"); break;
		case AVC_CMD_DISCONNECT_AV:				cliOutputLine("AVC_CMD_DISCONNECT_AV"); break;
		case AVC_CMD_CONNECTIONS:				cliOutputLine("AVC_CMD_CONNECTIONS"); break;
		case AVC_CMD_CONNECT:					cliOutputLine("AVC_CMD_CONNECT"); break;
		case AVC_CMD_DISCONNECT:				cliOutputLine("AVC_CMD_DISCONNECT"); break;
		case AVC_CMD_SUBUNIT_INFO:				cliOutputLine("AVC_CMD_SUBUNIT_INFO"); break;
		case AVC_CMD_UNIT_INFO:					cliOutputLine("AVC_CMD_UNIT_INFO"); break;
		case AVC_CMD_GENERAL_BUS_SETUP:			cliOutputLine("AVC_CMD_GENERAL_BUS_SETUP"); break;
		case AVC_CMD_STREAM_FMT_SUPPORT:		cliOutputLine("AVC_CMD_STREAM_FMT_SUPPORT"); break;

		// Unit commands (CCM)
		case AVC_CMD_SIGNAL_SOURCE:				cliOutputLine("AVC_CMD_SIGNAL_SOURCE"); break;
		case AVC_CMD_INPUT_SELECT:				cliOutputLine("AVC_CMD_INPUT_SELECT"); break;
		case AVC_CMD_OUTPUT_PRESET:				cliOutputLine("AVC_CMD_OUTPUT_PRESET"); break;
		case AVC_CMD_CCM_PROFILE:				cliOutputLine("AVC_CMD_CCM_PROFILE"); break;
												
		// unit and subunit commands													
		case AVC_CMD_VENDOR_DEPENDENT:			cliOutputLine("AVC_CMD_VENDOR_DEPENDENT"); break;
		case AVC_CMD_RESERVE:					cliOutputLine("AVC_CMD_RESERVE"); break;
		case AVC_CMD_PLUG_INFO:					cliOutputLine("AVC_CMD_PLUG_INFO"); break;
		case AVC_CMD_OPEN_INFOBLOCK:			cliOutputLine("AVC_CMD_OPEN_INFOBLOCK"); break;
		case AVC_CMD_READ_INFOBLOCK:			cliOutputLine("AVC_CMD_READ_INFOBLOCK"); break;
		case AVC_CMD_WRITE_INFOBLOCK:			cliOutputLine("AVC_CMD_WRITE_INFOBLOCK"); break;
		case AVC_CMD_OPEN_DESCRIPTOR:			cliOutputLine("AVC_CMD_OPEN_DESCRIPTOR"); break;
		case AVC_CMD_READ_DESCRIPTOR:			cliOutputLine("AVC_CMD_READ_DESCRIPTOR"); break;
		case AVC_CMD_WRITE_DESCRIPTOR:			cliOutputLine("AVC_CMD_WRITE_DESCRIPTOR"); break;
		case AVC_CMD_SEARCH_DESCRIPTOR:			cliOutputLine("AVC_CMD_SEARCH_DESCRIPTOR"); break;
		case AVC_CMD_CREATE_DESCRIPTOR:			cliOutputLine("AVC_CMD_CREATE_DESCRIPTOR"); break;
		case AVC_CMD_OBJECT_NUMBER_SELECT:		cliOutputLine("AVC_CMD_OBJECT_NUMBER_SELECT"); break;
		case AVC_CMD_SECURITY:					cliOutputLine("AVC_CMD_SECURITY"); break;
		case AVC_CMD_VERSION:					cliOutputLine("AVC_CMD_VERSION"); break;
		case AVC_CMD_POWER:						cliOutputLine("AVC_CMD_POWER"); break;
		case AVC_CMD_RATE:						cliOutputLine("AVC_CMD_RATE"); break;
		case AVC_CMD_FUNCTION_BLOCK:			cliOutputLine("AVC_CMD_FUNCTION_BLOCK"); break;
		case AVC_CMD_CHANGE_CONFIGURATION:		cliOutputLine("AVC_CMD_CHANGE_CONFIGURATION"); break;
		case AVC_CMD_STREAM_FMT_EXT_INFO:		cliOutputLine("AVC_CMD_STREAM_FMT_EXT_INFO"); break;

		// Panel Subunit opcodes
		case AVC_CMD_PANEL_PASS_THROUGH:		cliOutputLine("AVC_CMD_PANEL_PASS_THROUGH"); break;
		case AVC_CMD_PANEL_GUI_UPDATE:			cliOutputLine("AVC_CMD_PANEL_GUI_UPDATE"); break;
		case AVC_CMD_PANEL_PUSH_GUI_DATA:		cliOutputLine("AVC_CMD_PANEL_PUSH_GUI_DATA"); break;
		case AVC_CMD_PANEL_USER_ACTION:			cliOutputLine("AVC_CMD_PANEL_USER_ACTION"); break;

		default:								cliPrintf("AVC opcode: 0x%02x\n\r", opcode); break;
	}
}

void avcUnitCliPrintHeader(AVC_HEADER *pHeader)
{
	cliOutputLine("");
	cliOutputLine("-----------------------------------------------------------------------");
	if (avcCtypeIsCommand(pHeader->ctype) == TRUE)
	{
		cliOutputLine("AV/C Header [Command Frame]");
	}
	else if (avcCtypeIsResponse(pHeader->ctype) == TRUE)
	{
		cliOutputLine("AV/C Header [Response Frame]");
	}
	else
	{
		cliOutputLine("AV/C Header [Unknown frame type]");
	}
	cliOutputLine("-----------------------------------------------------------------------");
	cliPrintf("fcptype:\t0x%02x\n\r", pHeader->fcptype);
	avcUnitCliPrintCtype(pHeader->ctype);
	avcUnitCliPrintSubunit(&pHeader->addrSubunit);
	avcUnitCliPrintOpcode(pHeader->opcode);
}

HRESULT avcUnitCliPrintOperands(AVC_HEADER *pHeader, pDataStream pStream)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i = 0;
	BYTE		operand = 0;
	uint32		items = pStream->total_size - pStream->cur_offset;

	cliPrintf("operands:\t%i\t", items);

	UNUSED_ARG(pHeader);

	for (i = 0; i < items; i++)
	{
		if (i && (i % 10 == 0)) cliPrintf("\n\r\t\t\t");
		hResult = dsRead8Bits(pStream, &operand);
		if (hResult != NO_ERROR) return hResult;

		cliPrintf("[%02X] ", operand);
	}
	cliPrintf("\n\r");
	cliOutputLine("-----------------------------------------------------------------------");
	
	hResult = dsGotoMarker(pStream, DSMARKER_OPERAND_0);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT	avcUnitCliPrintFrame(AVC_HEADER *pHeader, pDataStream pStream)
{
	HRESULT		hResult = NO_ERROR;
	
	if (pStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (pHeader == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	avcUnitCliPrintHeader(pHeader);
	
	hResult = avcUnitCliPrintOperands(pHeader, pStream);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcUnitCliPrintDataStream(pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;
	AVC_HEADER		avcHeader;
	uint32			savepos = 0;
	
	if (pStream == NULL)
	{
		hResult = E_AVC_DS_INVALID_STREAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	savepos = dsGetPosition(pStream);
	dsSetPosition(pStream, 0);

	hResult = avcDecodeHeader(pStream, &avcHeader);
	if (hResult != NO_ERROR) return hResult;

	avcUnitCliPrintFrame(&avcHeader, pStream);

	dsSetPosition(pStream, savepos);

	return hResult;
}

HRESULT avcUnitCliPrintPacketBlock(PB *packetBlock)
{
	HRESULT			hResult = NO_ERROR;
	pDataStream		pStream = NULL;
	
	hResult = pbGetApplicationDatastream(packetBlock, &pStream);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcUnitCliPrintDataStream(pStream);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcUnitCliPrintRawData(uint32 ctype, uint32 numBytes, uint32 *data)
{
	HRESULT			hResult = NO_ERROR;
	uint32			i;

	cliOutputLine("");
	cliOutputLine("-----------------------------------------------------------------------");
	if (avcCtypeIsCommand(ctype) == TRUE)
	{
		cliPrintf("AV/C Packet [Command Frame] Raw Data");
	}
	else if (avcCtypeIsResponse(ctype) == TRUE)
	{
		cliPrintf("AV/C Packet [Response Frame] Raw Data");
	}
	else
	{
		cliPrintf("AV/C Packet [Unknown frame type] Raw Data");
	}
	cliPrintf(" (size: %i bytes)\n\r", numBytes);
	cliOutputLine("-----------------------------------------------------------------------");

	cliPrintf("\t");
	for (i = 0; i < numBytes/4; i++)
	{
		if (i && (i % 6 == 0)) cliPrintf("\n\r\t");
		cliPrintf("[%08X] ", data[i]);
	}
	cliPrintf("\n\r");

	if (numBytes % 4 != 0)
	{
		cliPrintf("numBytes % 4 != 0\n\r");
	}
	
	cliOutputLine("-----------------------------------------------------------------------");

	return hResult;
}


HRESULT avcUnitCliPrintPacketBlockRawData(uint32 ctype, PB *packetBlock)
{
	HRESULT			hResult = NO_ERROR;
	uint32			payloadSize = 0;
	uint32			*pPayload = NULL;

	hResult = pbGetPayloadSize (packetBlock, &payloadSize);
	if (hResult != NO_ERROR) return hResult;

	hResult = pbGetPayload(packetBlock, (void **) &pPayload);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcUnitCliPrintRawData(ctype, payloadSize, pPayload);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

// AV/C General Commands functions
void avcUnitCliPrintPowerState(uint32 power_state)
{
	cliOutputLine("-----------------------------------------------------------------------");
	cliPrintf("power_state:\t");

	switch (power_state)
	{
		// power operand values
		case AVC_POWER_ON:						cliOutputLine("AVC_POWER_ON"); break;
		case AVC_POWER_OFF:						cliOutputLine("AVC_POWER_OFF"); break;
		case AVC_POWER_STATUS:					cliOutputLine("AVC_POWER_STATUS"); break;
		default:								cliPrintf("AVC unit power_state: 0x%02x\n\r", power_state); break;
	}
	cliOutputLine("-----------------------------------------------------------------------");
}

void avcUnitCliPrintCompanyID(uint32 company_ID)
{
	cliPrintf("company_ID:\t");

	targetVendorDefsPrintVendorID(company_ID);
	cliPrintf("\n\r");
	cliOutputLine("-----------------------------------------------------------------------");
}

void avcUnitCliPrintReservePriority(uint32 priority)
{
	uint32		stored;

	cliPrintf("priority:\t");

	avcUnitReserveGetStoredPriority(priority, &stored);

	if		(priority == 0x04)	cliPrintf("default: 0x%02x (stored: 0x%02x)\n\r", priority, stored);
	else if	(priority <= 0x0f)	cliPrintf("command: 0x%02x (stored: 0x%02x)\n\r", priority, stored);
	else if (priority == 0xff)	cliPrintf("status: 0x%02x\n\r", priority);
	else						cliPrintf("illegal: 0x%02x (max: 0x0f)\n\r", priority);
}

void avcUnitCliPrintReserveText(char *text_data)
{
	char	text[AVC_RESERVE_TEXT_SIZE + 1];
	uint32	i;

	cliPrintf("Text:\t\t");

	if (strncmp(AVC_RESERVE_NO_TEXT_STRING, text_data, AVC_RESERVE_TEXT_SIZE) == 0)
	{
		cliPrintf("no string is present\n\r");
	}
	else
	{
		for (i = 0; i < AVC_RESERVE_TEXT_SIZE; i++) text[i] = text_data[i];
		text[AVC_RESERVE_TEXT_SIZE] = 0;
		formatSwapStrBytes((char *) text, AVC_RESERVE_TEXT_SIZE);
		cliPrintf("%s\n\r", text);
	}
}

void avcUnitCliPrintReserve(uint32 priority, char *text)
{
	cliOutputLine("-----------------------------------------------------------------------");
	cliPrintf("reserve:\t\n\r");
	avcUnitCliPrintReservePriority(priority);
	avcUnitCliPrintReserveText(text);
	cliOutputLine("-----------------------------------------------------------------------");
}

void avcUnitCliPrintVersionInfo(BYTE subfunction, BYTE version_information, BYTE *su_dependent_field)
{
	uint32	i;

	cliOutputLine("-----------------------------------------------------------------------");
	cliPrintf("version:\t\n\r");
	cliPrintf("subfunction:\t0x%02x %s\n\r", subfunction, (subfunction == 0xFF) ? "Version status" : "Unknown");
	cliPrintf("version_info:\t0x%02x %s\n\r", version_information, (version_information == 0xFF) ? "Latest version" : "Specific version");
	cliPrintf("su_dep_field:\t%i\t", AVC_VERSION_SU_DEP_FIELD_SIZE);
	for (i = 0; i < AVC_VERSION_SU_DEP_FIELD_SIZE; i++)
	{
		if (i && (i % 10 == 0)) cliPrintf("\n\r\t\t\t");
		cliPrintf("[%02X] ", su_dependent_field[i]);
	}
	cliPrintf("\n\r");
	cliOutputLine("-----------------------------------------------------------------------");
}

void avcUnitCliPrintProgramType(uint32 programType)
{
	cliPrintf("programType:\t");

	switch (programType)
	{
		// company_ID values
		case PROG_AVR_DRD:						cliOutputLine("PROG_AVR_DRD"); break;
		default:								cliPrintf("AVC unit programType: 0x%08x\n\r", programType); break;
	}
}

void avcUnitCliPrintVendorFirmwareInfo(TC_PROGRAM_ATTRIBUTES *programAttributes)
{
	uint32	i;

	cliOutputLine("-----------------------------------------------------------------------");
	cliPrintf("vendor firmware info:\n\r");

	cliPrintf("att.ver.:\t0x%08x\n\r", programAttributes->attributeVersion);
	avcUnitCliPrintProgramType(programAttributes->programType);
	cliPrintf("prog.ver.:\t0x%04x (major) 0x%04x (build)\n\r", (uint16) (programAttributes->programVersion >> 16) , (uint16) (programAttributes->programVersion));
	if (programAttributes->attributeVersion < 0x000000002)
	{
		for (i = 0; i < 5; i++)
		{
			cliPrintf("reserved[i]:\t0x%08x\n\r", programAttributes->reserved[i]);
		}
	}
	cliOutputLine("-----------------------------------------------------------------------");
}

// AV/C Connection Commands functions
HRESULT avcUnitCliPrintPlugInfo(AVC_ADDRESS_SUBUNIT *addrSubunit, BYTE subfunction, BYTE *plugs)
{
	HRESULT		hResult = NO_ERROR;

	cliOutputLine("-----------------------------------------------------------------------");
	cliPrintf("plug info:\t");

	if (avcSubunitIsUnit(addrSubunit) == FALSE)	// Plug Info on Subunit
	{
		cliPrintf("for subunit:\n\r");
		cliPrintf("subfunction:\t0x%02x\n\r", subfunction);
		if (subfunction == 0x00)		// Subfunction: must be 0x00
		{
			cliPrintf("DstPlugs:\t%i\n\r", plugs[0]);
			cliPrintf("SrcPlugs:\t%i\n\r", plugs[1]);
		}
		else
		{
			cliPrintf("subfunction: illegial\n\r");
			return E_BAD_INPUT_PARAMETERS;
		}
	}
	else													// Plug Info on Unit
	{
		cliPrintf("for unit:\n\r");
		cliPrintf("subfunction:\t[0x%02x] ", subfunction);

		if (subfunction == 0x00)		// Subfunction: Serial Bus Isochronous and External Plug
		{
			cliPrintf("Serial Bus Isoch and Ext Plug\n\r");
		}
		else if (subfunction == 0x01)	// Subfunction: Serial Bus Asynchronous Plug
		{
			cliPrintf("Serial Bus Async Plug\n\r");
		}
		else if (subfunction <= 0x3F)	// Subfunction: (0x02-0x3F) Reserved
		{
			cliPrintf("Reserved\n\r");
			return hResult;
		}
		else if (subfunction <= 0x7F)	// Subfunction: (0x40-0x7F) General Bus Plug
		{
			cliPrintf("General Bus Plug (%s)\n\r", (subfunction == 0x40) ? "Bluetooth" : "reserved");
			return hResult;
		}
		else							// Subfunction: (0x80-0xFF) Reserved
		{
			cliPrintf("Reserved\n\r");
			return hResult;
		}
		
		if (subfunction == 0x00)		// Subfunction: Serial Bus Isochronous and External Plug
		{
			cliPrintf("IsochInPlugs:\t%i\n\r", plugs[0]);
			cliPrintf("IsochOutPlugs:\t%i\n\r", plugs[1]);
			cliPrintf("ExtInPlugs:\t%i\n\r", plugs[2]);
			cliPrintf("ExtOutPlugs:\t%i\n\r", plugs[3]);
		}
		else if (subfunction == 0x01)	// Subfunction: Serial Bus Asynchronous Plug
		{
			cliPrintf("AsyncInPlugs:\t%i\n\r", plugs[0]);
			cliPrintf("AsyncOutPlugs:\t%i\n\r", plugs[1]);
			cliPrintf("0xFF:\t\t0x%02x\n\r", plugs[2]);
			cliPrintf("0xFF:\t\t0x%02x\n\r", plugs[3]);
		}
	}			

	cliOutputLine("-----------------------------------------------------------------------");

	return hResult;
}

HRESULT avcUnitCliPrintChannelUsage(BYTE channel, uint16 nodeAddr, BYTE oPCRNumber)
{
	HRESULT		hResult = NO_ERROR;
	uint16		busId = 0;

	hResult = lalGetBusID(&busId);
	if (hResult != NO_ERROR) return hResult;

	cliOutputLine("-----------------------------------------------------------------------");
	cliPrintf("channel usage:\n\r");
	cliPrintf("channel:\t%i\n\r", channel);
	cliPrintf("nodeID:\t\t0x%04x (id: %i)\n\r", nodeAddr, (nodeAddr != 0xFFFF) ? nodeAddr-busId : -1);
	cliPrintf("oPCRNumber:\t0x%02x (%i)\n\r", oPCRNumber, (oPCRNumber != 0xFF) ? oPCRNumber : -1);

	cliOutputLine("-----------------------------------------------------------------------");

	return hResult;
}

void avcUnitCliPrintConnect(AVC_CONNECTION *connection)
{
	cliOutputLine("-----------------------------------------------------------------------");
	cliPrintf("connect:\n\r");
	cliPrintf("lock:\t\t%s\n\r", (connection->bLock) ? "TRUE" : "FALSE");
	cliPrintf("perm:\t\t%s\n\r", (connection->bPerm) ? "TRUE" : "FALSE");

	cliPrintf("src_plug:\n\r");
	avcUnitCliPrintPlug(&connection->plugs.src, TRUE);
	cliPrintf("dst_plug:\n\r");
	avcUnitCliPrintPlug(&connection->plugs.dst, FALSE);
	cliOutputLine("-----------------------------------------------------------------------");
}

void avcUnitCliPrintConnectAV(uint32 dummy)
{
	cliOutputLine("-----------------------------------------------------------------------");
	cliPrintf("connectAV:\n\r");
	cliPrintf("dummy:\t\t%i\n\r", dummy);
	cliOutputLine("-----------------------------------------------------------------------");
}

void avcUnitCliPrintConnections(uint32 total_connections, AVC_CONNECTION *connectionArray)
{
	uint32			i;

	cliOutputLine("-----------------------------------------------------------------------");
	cliPrintf("connections:\n\r");
	cliPrintf("total_conn:\t%i\n\r", total_connections);
	cliOutputLine("-----------------------------------------------------------------------");
	
	for (i = 0; i < total_connections; i++)
	{
		avcUnitCliPrintConnect(&connectionArray[i]);
	}
}

void avcUnitCliPrintConnectionState(uint32 connection_state)
{
	cliOutputLine("-----------------------------------------------------------------------");
	cliPrintf("connection_state:\t");

	switch (connection_state)
	{
		// connection state operand values
		case AVC_CONNECTION_ESTABLISH:			cliOutputLine("AVC_CONNECTION_ESTABLISH"); break;
		case AVC_CONNECTION_BREAK:				cliOutputLine("AVC_CONNECTION_BREAK"); break;
		case AVC_CONNECTION_STATUS:				cliOutputLine("AVC_CONNECTION_STATUS"); break;
		default:								cliPrintf("AVC unit connection_state: 0x%02x\n\r", connection_state); break;
	}
	cliOutputLine("-----------------------------------------------------------------------");
}

void avcUnitCliPrintSignalFormat(BOOL bIn, BYTE plugID, uint32 format)
{
	cliOutputLine("-----------------------------------------------------------------------");
	cliPrintf("%s plug signal format\n\r", (bIn) ? "input" : "output");
	avcUnitCliPrintUnitPlugID(plugID, bIn);
	cliPrintf("format:\t\t0x%08x (fmt:0x%02x fdf:0x%06x)\n\r", format, format >> 24, format & 0x00ffffff);

	cliOutputLine("-----------------------------------------------------------------------");
}

void avcUnitCliPrintOutputStatus(BYTE status)
{
	cliPrintf("output-status:\t");
	switch (status)
	{
		case 0:		cliPrintf("effective\n\r"); break;
		case 1:		cliPrintf("not effective\n\r"); break;
		case 2:		cliPrintf("insufficient resource\n\r"); break;
		case 3:		cliPrintf("ready\n\r"); break;
		case 4:		cliPrintf("virtual output\n\r"); break;
		default:	cliPrintf("reserved\n\r"); break;
	}
}

void avcUnitCliPrintSignalStatus(BYTE status)
{
	if (status == 0)
	{
		cliPrintf("signal-status:\tidentical\n\r");
	}
	else
	{
		cliPrintf("signal-status:\tprocessed:%i,filtered:%i,converted:%i,OSD-overload:%i\n\r", ((status&0x08)==0x08), ((status&0x04)==0x04), ((status&0x02)==0x02), ((status&0x01)==0x01));
	}
}

// AV/C CCM Commands functions
void avcUnitCliPrintSignalSource(BYTE status, AVC_PLUG_CONNECTION *plugs)
{
	cliOutputLine("-----------------------------------------------------------------------");
	cliPrintf("signal source\n\r");

	avcUnitCliPrintOutputStatus((BYTE) (status >> 5));
	cliPrintf("conv:\t\tdst plug can change format:%s\n\r", (status & 0x10) ? "YES" : "NO");
	avcUnitCliPrintSignalStatus((BYTE) (status & 0x0F));

	cliPrintf("src_plug:\n\r");
	avcUnitCliPrintPlug(&plugs->src, TRUE);
	cliPrintf("dst_plug:\n\r");
	avcUnitCliPrintPlug(&plugs->dst, FALSE);
	cliOutputLine("-----------------------------------------------------------------------");
}

void avcUnitCliPrintInputStatus(BYTE status)
{
	cliPrintf("status:\t\t");
	switch (status)
	{
		case 0:		cliPrintf("active\n\r"); break;
		case 1:		cliPrintf("ready\n\r"); break;
		case 2:		cliPrintf("no selection\n\r"); break;
		case 3:		cliPrintf("cannot input\n\r"); break;
		case 4:		cliPrintf("insufficient resource\n\r"); break;
		default:	cliPrintf("reserved\n\r"); break;
	}
}

void avcUnitCliPrintInputSelect(BYTE status, uint16 nodeID, BYTE oPlugID, BYTE iPlugID, AVC_ADDRESS_PLUG *signalDst)
{
	cliOutputLine("-----------------------------------------------------------------------");
	cliPrintf("input select\n\r");

	avcUnitCliPrintInputStatus(status);
	cliPrintf("nodeID:\t\t%04X\n\r", nodeID);
	cliPrintf("output_plug:\n\r");
	avcUnitCliPrintUnitPlugID(oPlugID, FALSE);
	cliPrintf("input_plug:\n\r");
	avcUnitCliPrintUnitPlugID(iPlugID, TRUE);
	cliPrintf("signal_dst:\n\r");
	avcUnitCliPrintPlug(signalDst, FALSE);
	cliOutputLine("-----------------------------------------------------------------------");
}

void avcUnitCliPrintOutputPreset(void)
{
	cliOutputLine("-----------------------------------------------------------------------");
	cliPrintf("output preset\n\r");

	cliOutputLine("-----------------------------------------------------------------------");
}

void avcUnitCliPrintCCMProfile(void)
{
	cliOutputLine("-----------------------------------------------------------------------");
	cliPrintf("CCM profile\n\r");

	cliOutputLine("-----------------------------------------------------------------------");
}

#ifdef _AVC_SECURITY
// AV/C Security Commands functions
void avcUnitCliPrintSecurity(uint32 category)
{
	cliOutputLine("-----------------------------------------------------------------------");
	cliPrintf("security:\n\r");

	cliPrintf("category:\t");
	switch (category)
	{
		// AVC Security Categories							
		case AVC_SECURITY_CATEGORY_DTCP_AKE:	cliPrintf("AVC_SECURITY_CATEGORY_DTCP_AKE\n\r"); break;
		case AVC_SECURITY_CATEGORY_VENDOR:		cliPrintf("AVC_SECURITY_CATEGORY_VENDOR\n\r"); break;
		case AVC_SECURITY_CATEGORY_EXTENDED:	cliPrintf("AVC_SECURITY_CATEGORY_EXTENDED\n\r"); break;
		default:								cliPrintf("Reserved: %i\n\r"); break;
	}

	cliOutputLine("-----------------------------------------------------------------------");
}
#endif //_AVC_SECURITY

// List manager debug functions
void avcUnitCliPrintPowerNotify(void *data)
{
	POWER_NOTIFY*	notify = NULL;
	POWER_DATA*		power = NULL;		

	notify = (POWER_NOTIFY *) data;
	power = &notify->data;

	avcUnitCliPrintPowerState(power->state);
}

void avcUnitCliPrintReserveNotify(void *data)
{
	RESERVE_NOTIFY*		notify;
	RESERVE_DATA*		reservation = NULL;		
	char				text[AVC_RESERVE_TEXT_SIZE + 1];
	uint32				i;

	notify = (RESERVE_NOTIFY *) data;
	reservation = &notify->data;

	for (i = 0; i < AVC_RESERVE_TEXT_SIZE; i++) text[i] = reservation->text[i];
	text[AVC_RESERVE_TEXT_SIZE] = 0;

	cliPrintf("avcUnitReserveNotify\n\r");
	avcUnitCliPrintSubunit(&reservation->addrSubunit);
	avcUnitCliPrintReserve(reservation->priority, text);

	cliPrintf("mode:\t\t");
	switch (reservation->mode)
	{
		// reservation modes
		case RESERVE_MODE_NOTIFY:				cliOutputLine("RESERVE_MODE_NOTIFY"); break;
		case RESERVE_MODE_RESERVE:				cliOutputLine("RESERVE_MODE_RESERVE"); break;
		case RESERVE_MODE_BUSRESET:				cliOutputLine("RESERVE_MODE_BUSRESET"); break;
		default:								cliPrintf("reservation mode unknown: 0x%08x\n\r", reservation->mode); break;
	}
}

void avcUnitCliPrintChannelUsageNotify(void *data)
{
	CHANNEL_USAGE_NOTIFY*	notify = NULL;
	CHANNEL_USAGE_DATA*		channelUsage = NULL;		

	notify = (CHANNEL_USAGE_NOTIFY *) data;
	channelUsage = &notify->data;

	avcUnitCliPrintChannelUsage(channelUsage->channel, channelUsage->nodeAddr, channelUsage->oPCRNumber);
}

void avcUnitCliPrintConnectNotify(void *data)
{
	CONNECT_NOTIFY*			notify = NULL;
	CONNECT_DATA*			connect = NULL;		

	notify = (CONNECT_NOTIFY *) data;
	connect = &notify->data;

	avcUnitCliPrintConnect(&connect->connection);
}

void avcUnitCliPrintConnectAVNotify(void *data)
{
	CONNECT_AV_NOTIFY*		notify = NULL;
	CONNECT_AV_DATA*		connectAV = NULL;		

	notify = (CONNECT_AV_NOTIFY *) data;
	connectAV = &notify->data;

	avcUnitCliPrintConnectAV(connectAV->dummy);
}

void avcUnitCliPrintSignalFormatNotify(void *data)
{
	SIGNAL_FORMAT_NOTIFY*	notify = NULL;
	SIGNAL_FORMAT_DATA*		signalFormat = NULL;		

	notify = (SIGNAL_FORMAT_NOTIFY *) data;
	signalFormat = &notify->data;

	avcUnitCliPrintSignalFormat(signalFormat->bIn, signalFormat->plugID, signalFormat->format);
}

void avcUnitCliPrintSignalSourceNotify(void *data)
{
	SIGNAL_SOURCE_NOTIFY*	notify = NULL;
	SIGNAL_SOURCE_DATA*		signalSource = NULL;		

	notify = (SIGNAL_SOURCE_NOTIFY *) data;
	signalSource = &notify->data;

	UNUSED_ARG(signalSource);

	cliOutputLine("-----------------------------------------------------------------------");
	cliPrintf("signal source notify\n\r");
	cliOutputLine("-----------------------------------------------------------------------");
//	avcUnitCliPrintSignalSource();
}

#ifdef _CLI_TOOL_AVC_UNIT

HRESULT avcUnitCliGetList(char clist, LM_CONTEXT **list, LM_PRINT_LIST_ELEM_CB *callback)
{
	HRESULT				hResult = NO_ERROR;

	switch (clist)
	{
		case 'r':	
			*list = avcUnitReserveGetReservationList();
			*callback = avcUnitCliPrintReserveNotify;
			break;
		case 'n':	
			*list = avcUnitReserveGetNotifyList(); 
			*callback = avcUnitCliPrintReserveNotify;
			break;
		case 'p':
			*list = avcUnitPowerGetNotifyList(); 
			*callback = avcUnitCliPrintPowerNotify;
			break;
		case 'u':
			*list = avcUnitChannelUsageGetNotifyList(); 
			*callback = avcUnitCliPrintChannelUsageNotify;
			break;
		case 'c':
			*list = avcUnitConnectGetNotifyList(); 
			*callback = avcUnitCliPrintConnectNotify;
			break;
		case 'a':
			*list = avcUnitConnectAVGetNotifyList(); 
			*callback = avcUnitCliPrintConnectAVNotify;
			break;
		case 'i':
			*list = avcUnitSignalFormatGetNotifyList(TRUE);  //bIn == TRUE
			*callback = avcUnitCliPrintSignalFormatNotify;
			break;
		case 'o':
			*list = avcUnitSignalFormatGetNotifyList(FALSE); //bIn == FALSE
			*callback = avcUnitCliPrintSignalFormatNotify;
			break;
		case 's':
			*list = avcUnitSignalSourceGetNotifyList(); 
			*callback = avcUnitCliPrintSignalSourceNotify;
			break;
		default:	
			hResult = E_BAD_INPUT_PARAMETERS;
			break;
	}

	return hResult;
}

HRESULT avcUnitCliList(char clist)
{
	HRESULT					hResult = NO_ERROR;
	LM_CONTEXT				*list;
	LM_PRINT_LIST_ELEM_CB	callback;

	hResult = avcUnitCliGetList(clist, &list, &callback);
	if (hResult != NO_ERROR) return hResult;

	lmPrintList(list, callback);

	return hResult;
}

// AV/C Unit List Tool
static  char	avcUnitCliList_UsageString[]=\
				"avc:list: \n\r"\
				"use: avc:list <list>\n\r"\
				"list:"\
				"\treserve reservation:              r\n\r"\
				"\treserve notify:                   n\n\r"\
				"\tpower notify:                     p\n\r"\
				"\tchannel usage notify:             u\n\r"\
				"\tconnect notify:                   c\n\r"\
				"\tconnectAV notify:                 a\n\r"\
				"\tinput plug signal format notify:  i\n\r"\
				"\toutput plug signal format notify: o\n\r"\
				"\tsignal source notify:             s\n\r"\
				"------------------------------------------------------------\n\r"\
				"display list\n\r";

static	CLIDescriptor	avcUnitCliList_Descriptor[]=
{
	{kCLI_Name,				(uint32)"list"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcUnitCliList},
	{kCLI_UsageString, 		(uint32)avcUnitCliList_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"c"},
	{kCLI_ModuleName,		(uint32)"display list"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Unit Get PrintMode Tool
static  char	avcUnitCliGetPrintMode_UsageString[]=\
				"avc:setprintmode: \n\r"\
				"use: avc:getprintmode\n\r"\
				"------------------------------------------------------------\n\r"\
				"get avc unit print mode\n\r";

static	CLIDescriptor	avcUnitCliGetPrintMode_Descriptor[]=
{
	{kCLI_Name,				(uint32)"getprintmode"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcUnitCliDisplayPrintMode},
	{kCLI_UsageString, 		(uint32)avcUnitCliGetPrintMode_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"get avc unit print mode"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Unit Set PrintMode Tool
static  char	avcUnitCliSetPrintMode_UsageString[]=\
				"avc:setprintmode: \n\r"\
				"use: avc:setprintmode <mode>\n\r"\
				"------------------------------------------------------------\n\r"\
				"set avc unit print mode\n\r";

static	CLIDescriptor	avcUnitCliSetPrintMode_Descriptor[]=
{
	{kCLI_Name,				(uint32)"setprintmode"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcUnitCliSetPrintMode},
	{kCLI_UsageString, 		(uint32)avcUnitCliSetPrintMode_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"set avc unit print mode"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Unit Show PrintModes Tool
static  char	avcUnitCliShowPrintModes_UsageString[]=\
				"avc:showprintmodes: \n\r"\
				"use: avc:showprintmodes\n\r"\
				"------------------------------------------------------------\n\r"\
				"show avc unit print modes\n\r";

static	CLIDescriptor	avcUnitCliShowPrintModes_Descriptor[]=
{
	{kCLI_Name,				(uint32)"showprintmodes"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcUnitCliShowPrintModes},
	{kCLI_UsageString, 		(uint32)avcUnitCliShowPrintModes_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"show avc unit print modes"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void avcUnitCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(avcUnitCliList_Descriptor);
	hResult += cliInstallCLIDescriptor(avcUnitCliGetPrintMode_Descriptor);
	hResult += cliInstallCLIDescriptor(avcUnitCliSetPrintMode_Descriptor);
	hResult += cliInstallCLIDescriptor(avcUnitCliShowPrintModes_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing avc unit cli tools");
	}
}
#endif //_CLI_TOOL_AVC_UNIT
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
