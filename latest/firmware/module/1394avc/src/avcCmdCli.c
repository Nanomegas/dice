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

	Implementation of CLI for the avc unit commands
	
  Revisions:
		created 08/04/2003 LM

****************************************************************************/

static char 		moduleName[] = __FILE__ ;

#ifdef _CLI
#include "cliToolDefs.h"
#include "TCTasking.h"
#include "avc.h"
#include "avcUnitCli.h"
#include "avcCmdCli.h"
#include "avcUnit.h"
#include "avcDefs.h"
#include "lal.h"
#include "datastream.h"
#include "cli.h"
#include "fcp.h"
#include "format.h"
#include "targetPlugs.h"
#include "targetVendorDefs.h"

//LM??? needs more allocation if handling construction of extended subunit type/IDs (change to BYTES)

#define AVC_HEADER_BYTES				3						//  3 bytes: [fcp,ctype],[subunit type,ID],[opcode],
#define AVC_CMD_POWER_QUADLETS			(AVC_HEADER_BYTES+ 1)/4 //  4 bytes: ,[operand0]
#define AVC_CMD_UNIT_INFO_QUADLETS		(AVC_HEADER_BYTES+ 5)/4 //  8 bytes: ,[operand0],..,[operand4]
#define AVC_CMD_SUBUNIT_INFO_QUADLETS	(AVC_HEADER_BYTES+ 5)/4 //  8 bytes: ,[operand0],..,[operand4]
#define AVC_CMD_RESERVE_QUADLETS		(AVC_HEADER_BYTES+13)/4 // 16 bytes: ,[operand0],..,[operand12]
#define AVC_CMD_VERSION_QUADLETS		(AVC_HEADER_BYTES+33)/4 // 36 bytes: ,[operand0],..,[operand32]
#define AVC_CMD_VENDOR_QUADLETS			(AVC_HEADER_BYTES+3+TC_VENDOR_HEADER_SIZE+3/*align*/+sizeof(TC_PROGRAM_ATTRIBUTES))/4 // xx bytes: ,[operand0],..,[operandN]
#define AVC_CMD_PLUG_INFO_QUADLETS		(AVC_HEADER_BYTES+ 5)/4 //  8 bytes: ,[operand0],..,[operand4]
#define AVC_CMD_CHANNEL_USAGE_QUADLETS	(AVC_HEADER_BYTES+ 4+1/*align*/)/4 //  7 bytes: ,[operand0],..,[operand3]
#define AVC_CMD_CONNECT_QUADLETS		(AVC_HEADER_BYTES+ 5)/4 //  8 bytes: ,[operand0],..,[operand4]
#define AVC_CMD_CONNECT_AV_QUADLETS		(AVC_HEADER_BYTES+ 5)/4 //  8 bytes: ,[operand0],..,[operand4]
#define AVC_CMD_CONNECTIONS_QUADLETS	(AVC_HEADER_BYTES+ 1)/4 //  4 bytes: ,[operand0]
#define AVC_CMD_DIGITAL_INPUT_QUADLETS	(AVC_HEADER_BYTES+ 1)/4 //  4 bytes: ,[operand0]
#define AVC_CMD_DIGITAL_OUTPUT_QUADLETS	(AVC_HEADER_BYTES+ 1)/4 //  4 bytes: ,[operand0]
#define AVC_CMD_DISCONNECT_QUADLETS		(AVC_HEADER_BYTES+ 5)/4 //  8 bytes: ,[operand0],..,[operand4]
#define AVC_CMD_DISCONNECT_AV_QUADLETS	(AVC_HEADER_BYTES+ 5)/4 //  8 bytes: ,[operand0],..,[operand4]
#define AVC_CMD_SIGNAL_FORMAT_QUADLETS	(AVC_HEADER_BYTES+ 5)/4 //  8 bytes: ,[operand0],..,[operand4]
#define AVC_CMD_GENERAL_BUS_QUADLETS	(AVC_HEADER_BYTES+ 1)/4 //  4 bytes: ,[operand0]
#define AVC_CMD_SIGNAL_SOURCE_QUADLETS	(AVC_HEADER_BYTES+ 5)/4 //  8 bytes: ,[operand0],..,[operand4]
#define AVC_CMD_INPUT_SELECT_QUADLETS	(AVC_HEADER_BYTES+ 9)/4 // 12 bytes: ,[operand0],..,[operand8]
#define AVC_CMD_OUTPUT_PRESET_QUADLETS	(AVC_HEADER_BYTES+ 5)/4 //  8 bytes: ,[operand0],..,[operand4]
#define AVC_CMD_CCM_PROFILE_QUADLETS	(AVC_HEADER_BYTES+ 5)/4 //  8 bytes: ,[operand0],..,[operand4]
#define AVC_CMD_SECURITY_QUADLETS		(AVC_HEADER_BYTES+ 9)/4 //  8 bytes: ,[operand0],..,[operand8]

typedef struct
{
	uint32	wwuid_hi;
	uint32	wwuid_lo;
	char	name[7+1];
} AVC_CMP_DEVICE_INFO;

enum
{
	AVC_CMD_DEVICE_LOCAL	= 0,
	AVC_CMD_DEVICE_OTHER,
	AVC_CMD_DEVICE_BCAST,
	AVC_CMD_DEVICE_DICEA,
	AVC_CMD_DEVICE_DICEB,
	AVC_CMD_DEVICE_DICEC,
	AVC_CMD_DEVICE_DICE2,
	AVC_CMD_DEVICE_POCO,
	AVC_CMD_DEVICE_DHIVA,
	AVC_CMD_DEVICE_PC,
	AVC_CMD_DEVICE_PC2,
	AVC_CMD_DEVICE_PIO_DVD,
	AVC_CMD_DEVICE_PIO_RCV,
	AVC_CMD_DEVICE_DENON,
	AVC_CMD_DEVICE_ITEMS,
	AVC_CMD_DEVICE_UNKNOWN	= AVC_CMD_DEVICE_ITEMS
};

static const AVC_CMP_DEVICE_INFO avcCmdCliDevices[AVC_CMD_DEVICE_ITEMS+1] =
{
	{0x00000000, 0x00000000, "local  "},
	{0x00000000, 0x00000000, "other  "},
	{0x00000000, 0x0000ffff, "bcast  "},
	{0x00016604, 0xAAAAAAAA, "diceA  "},
	{0x00016604, 0xBBBBBBBB, "diceB  "},
	{0x00016604, 0xCCCCCCCC, "diceC  "},
	{0x00016604, 0x00000000, "dice2  "}, // generic type must come after specific types
	{0x00016602, 0x00000000, "poco   "},
	{0x00109f01, 0x00000000, "dhiva  "},
	{0x230050c5, 0x00000000, "pc     "},
	{0x000020ed, 0x00000000, "pc2    "},
	{0x00e03600, 0x00506b4f, "pioneer"},
	{0x00e03600, 0x00901abb, "vsa-pio"},
	{0x0005cd00, 0x001004d7, "denon  "},
	{0x00000000, 0x00000000, "?      "}	//always last entry
};

enum
{
	AVC_CLI_TARGET_NODE_REMOTE	= 0,
	AVC_CLI_TARGET_NODE_LOCAL	= 1,	

	AVC_CLI_TARGET_UNIT_UNIT	= 0,
#if 1
	AVC_CLI_TARGET_UNIT_SUBUNIT_AUDIO	= 1,
	AVC_CLI_TARGET_UNIT_SUBUNIT_PANEL	= 2,
	AVC_CLI_TARGET_UNIT_SUBUNIT_DISC	= 3,
#else
	AVC_CLI_TARGET_UNIT_SUBUNIT	= 1,
#endif

#ifdef _OHCIDRIVER
	AVC_CLI_TARGET_NODE_DEFAULT	= AVC_CMD_DEVICE_OTHER, //AVC_CMD_DEVICE_LOCAL,
	AVC_CLI_TARGET_UNIT_DEFAULT	= AVC_CLI_TARGET_UNIT_UNIT
#else //_OHCIDRIVER
	AVC_CLI_TARGET_NODE_DEFAULT	= AVC_CMD_DEVICE_PC,
	AVC_CLI_TARGET_UNIT_DEFAULT	= AVC_CLI_TARGET_UNIT_UNIT
#endif //_OHCIDRIVER
};

static uint32	avcCmdCliTargetNode = AVC_CLI_TARGET_NODE_DEFAULT;
static uint32	avcCmdCliTargetUnit = AVC_CLI_TARGET_UNIT_DEFAULT;

HRESULT	avcCmdCliGetTargetModes(char *sTargetNode, uint32 *targetNode, char cTargetUnit, uint32 *targetUnit)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i;
	
	for (i = 0; i < AVC_CMD_DEVICE_ITEMS; i++)
	{
		if (strncmp(sTargetNode, avcCmdCliDevices[i].name, strlen(sTargetNode)) == 0)
		{
			break;
		}
	}
	if (i == AVC_CMD_DEVICE_ITEMS)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		return hResult;
	}

	*targetNode = i;
	
	switch (cTargetUnit)
	{
		case 'u':	*targetUnit = AVC_CLI_TARGET_UNIT_UNIT; break;
#if 1
		case 'a':	*targetUnit = AVC_CLI_TARGET_UNIT_SUBUNIT_AUDIO; break;
		case 'p':	*targetUnit = AVC_CLI_TARGET_UNIT_SUBUNIT_PANEL; break;
		case 'd':	*targetUnit = AVC_CLI_TARGET_UNIT_SUBUNIT_DISC; break;
#else
		case 's':	*targetUnit = AVC_CLI_TARGET_UNIT_SUBUNIT; break;
#endif
		default:	hResult = E_BAD_INPUT_PARAMETERS; break;
	}

	return hResult;
}

HRESULT avcCmdCliSetTarget(char *sTargetNode, char cTargetUnit)
{
	HRESULT		hResult = NO_ERROR;
	uint32		targetNode = 0;
	uint32		targetUnit = 0;

	hResult = avcCmdCliGetTargetModes(sTargetNode, &targetNode, cTargetUnit, &targetUnit);
	if (hResult != NO_ERROR) return hResult;

	avcCmdCliTargetNode = targetNode;
	avcCmdCliTargetUnit = targetUnit;

#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
	{
		avcCmdCliGetTarget();
	}
#endif //_SYSDEBUG

	return hResult;
}

HRESULT avcCmdCliGetTarget(void)
{
	HRESULT					hResult = NO_ERROR;
	LAL_NODE_HANDLE			handle;
	WWUID					wwuid;
	AVC_ADDRESS_SUBUNIT		addrSubunit;
	uint32					i;

	hResult = avcCmdCliGetTargetHandle(&handle);
	if (hResult != NO_ERROR) return hResult;

	hResult = lalGetWWUIDFromHandle(handle, &wwuid);
	if (hResult != NO_ERROR) return hResult;

	for (i = 0; i < AVC_CMD_DEVICE_ITEMS; i++)
	{
		if (avcCmdCliDevices[i].wwuid_hi == wwuid.hi &&
			avcCmdCliDevices[i].wwuid_hi != 0x00000000 &&
			(avcCmdCliDevices[i].wwuid_lo == wwuid.low ||
			 avcCmdCliDevices[i].wwuid_lo == 0x00000000))
		{
			break;
		}
	}

	if (avcCmdCliTargetNode >= AVC_CMD_DEVICE_ITEMS)
	{
		cliPrintf("targetNode:\tunknown (%i)\n\r", avcCmdCliTargetNode);
		hResult = E_FAIL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	cliPrintf("targetNode:\t%s: %s\n\r", avcCmdCliDevices[avcCmdCliTargetNode].name, avcCmdCliDevices[i].name);
	cliPrintf("handle:\t\t0x%06x\n\r", handle);
	cliPrintf("WWUID:\t\t0x%08x 0x%08x\n\r", wwuid.hi, wwuid.low);

	switch (avcCmdCliTargetUnit)
	{
		case AVC_CLI_TARGET_UNIT_UNIT:
			cliPrintf("targetUnit:\tAVC_CLI_TARGET_UNIT_UNIT\n\r");
			break;
		
#if 1
		case AVC_CLI_TARGET_UNIT_SUBUNIT_AUDIO:
			cliPrintf("targetUnit:\tAVC_CLI_TARGET_UNIT_SUBUNIT_AUDIO\n\r");
			break;

		case AVC_CLI_TARGET_UNIT_SUBUNIT_PANEL:
			cliPrintf("targetUnit:\tAVC_CLI_TARGET_UNIT_SUBUNIT_PANEL\n\r");
			break;

		case AVC_CLI_TARGET_UNIT_SUBUNIT_DISC:
			cliPrintf("targetUnit:\tAVC_CLI_TARGET_UNIT_SUBUNIT_DISC\n\r");
			break;
#else
		case AVC_CLI_TARGET_UNIT_SUBUNIT:
			cliPrintf("targetUnit:\tAVC_CLI_TARGET_UNIT_SUBUNIT\n\r");
			break;
#endif

		default:
			cliPrintf("targetUnit:\tunknown (%i)\n\r", avcCmdCliTargetUnit);
			hResult = E_FAIL;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
	}

	hResult = avcCmdCliGetTargetUnit(&addrSubunit);
	if (hResult != NO_ERROR) return hResult;

	avcUnitCliPrintSubunit(&addrSubunit);

	return hResult;
}

HRESULT	avcCmdCliShowTargets(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		i;
	
	cliPrintf("targetNodes:\n\r");

	for (i = 0; i < AVC_CMD_DEVICE_ITEMS; i++)
	{
		cliPrintf("\t%s (%i)\n\r", avcCmdCliDevices[i].name, i);
	}

	return hResult;
}

HRESULT avcCmdCliGetTargetNodeAddr(uint32 *nodeAddr)
{
	HRESULT				hResult = NO_ERROR;
	LAL_NODE_HANDLE		handle;

	hResult = avcCmdCliGetTargetHandle(&handle);
	if (hResult != NO_ERROR) return hResult;

	hResult = lalGetNodeAddrFromHandle(handle, nodeAddr);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcCmdCliGetTargetHandle(LAL_NODE_HANDLE *phandle)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bMatchWWUIDLo = FALSE;
	BOOL		bMatchWWUIDHi = TRUE;
	BOOL		bAllowLocalNode = FALSE;
	WWUID		wwuid;

	if (avcCmdCliTargetNode == AVC_CMD_DEVICE_LOCAL)
	{
		hResult = lalGetThisHandle(phandle);
		if (hResult != NO_ERROR) return hResult;
	}
	else if (avcCmdCliTargetNode < AVC_CMD_DEVICE_ITEMS)
	{
		if (avcCmdCliTargetNode == AVC_CMD_DEVICE_OTHER)
		{
			bMatchWWUIDHi = FALSE;
		}
		else if (avcCmdCliTargetNode == AVC_CMD_DEVICE_BCAST)
		{
			wwuid.low = avcCmdCliDevices[AVC_CMD_DEVICE_BCAST].wwuid_lo; //broadcast
		}
		else if (avcCmdCliDevices[avcCmdCliTargetNode].wwuid_lo != 0x00000000)
		{
			wwuid.low = avcCmdCliDevices[avcCmdCliTargetNode].wwuid_lo;
			bMatchWWUIDLo = TRUE;
		}

		wwuid.hi = avcCmdCliDevices[avcCmdCliTargetNode].wwuid_hi;
		
		hResult = lalGetHandleFromWWUIDSpecial(wwuid, phandle, bMatchWWUIDHi, bMatchWWUIDLo, bAllowLocalNode);
		if (hResult != NO_ERROR) return hResult;
	}
	else
	{
		hResult = E_FAIL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

HRESULT avcCmdCliGetTargetUnit(AVC_ADDRESS_SUBUNIT *addrSubunit)
{
	HRESULT		hResult = NO_ERROR;

	switch (avcCmdCliTargetUnit)
	{
		case AVC_CLI_TARGET_UNIT_UNIT:
			avcSubunitGetUnitAddress(addrSubunit);
			break;

#if 1
		case AVC_CLI_TARGET_UNIT_SUBUNIT_AUDIO:
			addrSubunit->subunit_type = AVC_SU_TYPE_AUDIO;
			addrSubunit->subunit_ID = 0;
			break;

		case AVC_CLI_TARGET_UNIT_SUBUNIT_PANEL:
			addrSubunit->subunit_type = AVC_SU_TYPE_PANEL;
			addrSubunit->subunit_ID = 0;
			break;

		case AVC_CLI_TARGET_UNIT_SUBUNIT_DISC:
			addrSubunit->subunit_type = AVC_SU_TYPE_DISC;
			addrSubunit->subunit_ID = 0;
			break;
#else
		case AVC_CLI_TARGET_UNIT_SUBUNIT:
			hResult = targetGetSubunitAddress(addrSubunit);
			if (hResult != NO_ERROR) return hResult;
			break;
#endif

		default:
			hResult = E_FAIL;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
	}

	return hResult;
}

HRESULT avcCmdCliGetCType(char cctype, uint8 *ctype)
{
	HRESULT				hResult = NO_ERROR;

	switch (cctype)
	{
		case 'c':	*ctype = AVC_CTYPE_CONTROL; break;
		case 's':	*ctype = AVC_CTYPE_STATUS; break;
		case 'i':	*ctype = AVC_CTYPE_SPECIFIC_INQUIRY; break;
		case 'n':	*ctype = AVC_CTYPE_NOTIFY; break;
		case 'g':	*ctype = AVC_CTYPE_GENERAL_INQUIRY; break;
		default:	hResult = E_BAD_INPUT_PARAMETERS; break;
	}

	return hResult;
}

HRESULT avcCmdCliConstructRequestHeader(pDataStream pStream, uint8 ctype, uint8 opcode, uint16 numBytes, uint32 *data)
{
	HRESULT				hResult = NO_ERROR;
	AVC_HEADER			avcheader;

	hResult = dsOpenStream(pStream, data, numBytes, dsMODE_WRITE);
	if (hResult != NO_ERROR) return hResult;

	memset(&avcheader, 0, sizeof(avcheader));

	avcheader.fcptype = FCP_CTS_CODE_AVC;
	avcheader.ctype = ctype;

	hResult = avcCmdCliGetTargetUnit(&avcheader.addrSubunit);
	if (hResult != NO_ERROR) return hResult;

	avcheader.opcode = opcode;

	hResult = avcEncodeHeader(pStream, &avcheader);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcCmdCliSendRequest(uint8 ctype, uint16 numBytes, uint32 *data, pDataStream pStream)
{
	HRESULT				hResult = NO_ERROR;
	LAL_NODE_HANDLE		handle;

	hResult = avcCmdCliGetTargetHandle(&handle);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcPadToNextQuadletInStream(pStream);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcBlockingSendRequest(handle, ctype, numBytes, data);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

#ifdef _CLI_TOOL_AVC_CMD
// AV/C General Commands functions

HRESULT avcCmdCliPower(char cctype, uint8 power_state)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	uint32				data[AVC_CMD_POWER_QUADLETS]; 
	uint8				opcode = AVC_CMD_POWER; 
	uint8				ctype = 0;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite8Bits(&ds, power_state);				// operand[0]
	if (hResult != NO_ERROR) return hResult;
	
	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcCmdCliUnitInfo(char cctype)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	BYTE				ctype = 0;
	uint32				data[AVC_CMD_UNIT_INFO_QUADLETS]; 
	uint8				opcode = AVC_CMD_UNIT_INFO; 
	uint32				i;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	for (i = 0; i < 5; i++)
	{
		hResult = dsWrite8Bits(&ds, 0xFF);
		if (hResult != NO_ERROR) return hResult;
	}
	
	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcCmdCliSubunitInfo(char cctype, uint32 page)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	BYTE				ctype = 0;
	uint32				data[AVC_CMD_SUBUNIT_INFO_QUADLETS]; 
	uint8				opcode = AVC_CMD_SUBUNIT_INFO; 
	uint32				zeroBit = 0;
	uint32				extension_code = 0x07;
	uint32				page_data = 0xFFFFFFFF;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWriteBits(&ds, 1, zeroBit);
	if (hResult != NO_ERROR) return hResult;
	
	hResult = dsWriteBits(&ds, 3, page);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWriteBits(&ds, 1, zeroBit);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWriteBits(&ds, 3, extension_code);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite32Bits(&ds, page_data);
	if (hResult != NO_ERROR) return hResult;
	
	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcCmdCliReserve(char cctype, uint8 priority, char *text)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	uint32				data[AVC_CMD_RESERVE_QUADLETS]; 
	uint8				opcode = AVC_CMD_RESERVE; 
	uint8				ctype = 0;
	BYTE				text_data[AVC_RESERVE_TEXT_SIZE];
	uint32				i;
	uint32				len = strlen(text);

	if (len > AVC_RESERVE_TEXT_SIZE)
	{
		cliPrintf("AVC Reserve text exceeds maximum lenght (%i): %i\n\r", AVC_RESERVE_TEXT_SIZE, len);
		hResult = E_FAIL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite8Bits(&ds, priority);				// operand[0]
	if (hResult != NO_ERROR) return hResult;
	
	memset(text_data, 0, AVC_RESERVE_TEXT_SIZE);
	
	for (i = 0; i < len; i++)
	{
		text_data[i] = (BYTE) text[i];
	}

	formatSwapStrBytes((char *) text_data, AVC_RESERVE_TEXT_SIZE);
					
														// operand[1],..,operand[12]
	if (len == 0) // no text string is present
	{
		hResult = dsWriteChunk(&ds, AVC_RESERVE_TEXT_SIZE, AVC_RESERVE_NO_TEXT_STRING);
	}
	else
	{
		hResult = dsWriteChunk(&ds, AVC_RESERVE_TEXT_SIZE, (char *) text_data);
	}

	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcCmdCliVersion(char cctype, uint32 version)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	BYTE				ctype = 0;
	uint32				data[AVC_CMD_VERSION_QUADLETS]; 
	uint8				opcode = AVC_CMD_VERSION; 
	uint32				i;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite8Bits(&ds, 0xFF);						// operand[0]: subfunction
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite8Bits(&ds, (BYTE) version);			// operand[1]: version_information
	if (hResult != NO_ERROR) return hResult;

	for (i = 0; i < AVC_VERSION_SU_DEP_FIELD_SIZE; i++)
	{
		hResult = dsWrite8Bits(&ds, 0xFF);					// operand[2],..,operand[32]: su_dependent_field
		if (hResult != NO_ERROR) return hResult;
	}
	
	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcCmdCliVendor(char cctype)
{
	HRESULT					hResult = NO_ERROR;
	DataStream				ds;
	BYTE					ctype = 0;
	uint32					data[AVC_CMD_VENDOR_QUADLETS]; 
	uint8					opcode = AVC_CMD_VENDOR_DEPENDENT; 
	TC_PROGRAM_ATTRIBUTES	programAttributes;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite24Bits(&ds, THIS_VENDOR_ID);			// operand[0,..,2]: company_ID
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite8Bits(&ds, TC_VSAVC_CLASS_GENERAL);		// operand[3]: TC - class id
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite8Bits(&ds, 0);								// operand[4]: TC - sequence number
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite16Bits(&ds, TC_VSAVC_CMD_PGM_IDENTIFY);	// operand[5,..,6]: TC - command id
	if (hResult != NO_ERROR) return hResult;

																// operand[7]: Firmware info
	memset(&programAttributes, 0xff, sizeof(TC_PROGRAM_ATTRIBUTES));

	hResult = dsWriteChunk(&ds, sizeof(TC_PROGRAM_ATTRIBUTES), (char *) &programAttributes);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

// AV/C Connection Commands functions

HRESULT avcCmdCliPlugInfo(char cctype, uint32 subfunction)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	BYTE				ctype = 0;
	uint32				data[AVC_CMD_PLUG_INFO_QUADLETS]; 
	uint8				opcode = AVC_CMD_PLUG_INFO; 
	uint32				i;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite8Bits(&ds, (BYTE) subfunction);
	if (hResult != NO_ERROR) return hResult;
	
	for (i = 0; i < AVC_PLUG_INFO_PLUGS_SIZE; i++)
	{
		hResult = dsWrite8Bits(&ds, 0xFF);
		if (hResult != NO_ERROR) return hResult;
	}

	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcCmdCliChannelUsage(char cctype, uint32 channel)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	uint32				data[AVC_CMD_CHANNEL_USAGE_QUADLETS]; 
	uint8				opcode = AVC_CMD_CHANNEL_USAGE; 
	uint8				ctype = 0;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite8Bits(&ds, (BYTE) channel);			// operand[0]
	if (hResult != NO_ERROR) return hResult;
	
	hResult = dsWrite16Bits(&ds, 0xFFFF);					// operand[1,..,2]: nodeID
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite8Bits(&ds, 0xFF);						// operand[3]: oPCR number
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

// testing different combinations

#define CONNECT_CONN_CONTROL_ITEMS		8

static AVC_CONNECTION connectConnControl[CONNECT_CONN_CONTROL_ITEMS] =
{
//	lock	perm	src						dst
	{0,		0,		{AVC_PLUG_ISOCH_0,		AVC_PLUG_AUDIO_0_0}},
	{0,		0,		{AVC_PLUG_ISOCH_0,		AVC_PLUG_AUDIO_0_ANY_AVAILABLE}},
	{0,		1,		{AVC_PLUG_EXT_0,		AVC_PLUG_AUDIO_0_0}},
	{0,		0,		{AVC_PLUG_EXT_0,		AVC_PLUG_AUDIO_0_ANY_AVAILABLE}},
	{1,		1,		{AVC_PLUG_AUDIO_0_0,	AVC_PLUG_ISOCH_0}},
	{0,		0,		{AVC_PLUG_AUDIO_0_0,	AVC_PLUG_ISOCH_ANY_AVAILABLE}},
	{0,		1,		{AVC_PLUG_AUDIO_0_0,	AVC_PLUG_EXT_0}},
	{1,		0,		{AVC_PLUG_AUDIO_0_0,	AVC_PLUG_EXT_ANY_AVAILABLE}}
};

#define CONNECT_CONN_STATUS_ITEMS		6

static AVC_CONNECTION connectConnStatus[CONNECT_CONN_STATUS_ITEMS] =
{
//	lock	perm	src						dst
	{1,		1,		{AVC_PLUG_ISOCH_0,		AVC_PLUG_INVALID}},
	{1,		1,		{AVC_PLUG_EXT_0,		AVC_PLUG_INVALID}},
	{1,		1,		{AVC_PLUG_AUDIO_0_0,	AVC_PLUG_INVALID}},
	{1,		1,		{AVC_PLUG_INVALID,		AVC_PLUG_ISOCH_0}},
	{1,		1,		{AVC_PLUG_INVALID,		AVC_PLUG_EXT_0}},
	{1,		1,		{AVC_PLUG_INVALID,		AVC_PLUG_AUDIO_0_0}}
};

HRESULT avcCmdCliConnect(char cctype, uint32 index)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	uint32				data[AVC_CMD_CONNECT_QUADLETS]; 
	uint8				opcode = AVC_CMD_CONNECT; 
	uint8				ctype = 0;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	switch (ctype)
	{
		case AVC_CTYPE_CONTROL:
			if (index >= CONNECT_CONN_CONTROL_ITEMS) return E_BAD_INPUT_PARAMETERS;
			hResult = avcSubunitEncodeConnection(&ds, &connectConnControl[index]);
			if (hResult != NO_ERROR) return hResult;
			break;
		case AVC_CTYPE_STATUS:
		case AVC_CTYPE_NOTIFY:	
			if (index >= CONNECT_CONN_STATUS_ITEMS) return E_BAD_INPUT_PARAMETERS;
			hResult = avcSubunitEncodeConnection(&ds, &connectConnStatus[index]);
			if (hResult != NO_ERROR) return hResult;
			break;
		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			break;
	}

	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

#define CONNECT_AV_CONN_CONTROL_ITEMS		2

static uint32 connectAVConnControl[CONNECT_AV_CONN_CONTROL_ITEMS][8] =
{
	{	
		AVC_SRC_DST_TYPE_IGNORE,			//	video_src_type	
		AVC_SRC_DST_TYPE_SUBUNIT,			//	audio_src_type
		AVC_SRC_DST_TYPE_IGNORE,			//	video_dst_type
		AVC_SRC_DST_TYPE_SUBUNIT,			//	audio_dst_type
		AVC_SU_ANY_AVAILABLE,				//	video_src
		AVC_SU_ANY_AVAILABLE,				//	audio_src
		AVC_SU_ANY_AVAILABLE,				//	video_dst
		AVC_SU_TYPE_AUDIO					//	audio_dst
	},
	{	
		AVC_SRC_DST_TYPE_IGNORE,			//	video_src_type	
		AVC_SRC_DST_TYPE_SUBUNIT,			//	audio_src_type
		AVC_SRC_DST_TYPE_IGNORE,			//	video_dst_type
		AVC_SRC_DST_TYPE_ISOCH_EXT_PLUG,	//	audio_dst_type
		AVC_SU_ANY_AVAILABLE,				//	video_src
		AVC_SU_TYPE_AUDIO,					//	audio_src
		AVC_SU_ANY_AVAILABLE,				//	video_dst
		AVC_PLUG_ID_ANY_AVAILABLE_ISOCH		//	audio_dst
	} 
};

#define CONNECT_AV_CONN_STATUS_ITEMS		2

static uint32 connectAVConnStatus[CONNECT_AV_CONN_CONTROL_ITEMS][8] =
{
	{	
		AVC_SRC_DST_TYPE_RESERVED,			//	video_src_type	
		AVC_SRC_DST_TYPE_RESERVED,			//	audio_src_type
		AVC_SRC_DST_TYPE_IGNORE,			//	video_dst_type
		AVC_SRC_DST_TYPE_SUBUNIT,			//	audio_dst_type
		AVC_SU_ANY_AVAILABLE,				//	video_src
		AVC_SU_ANY_AVAILABLE,				//	audio_src
		AVC_SU_ANY_AVAILABLE,				//	video_dst
		AVC_SU_TYPE_AUDIO					//	audio_dst
	},
	{	
		AVC_SRC_DST_TYPE_RESERVED,			//	video_src_type	
		AVC_SRC_DST_TYPE_RESERVED,			//	audio_src_type
		AVC_SRC_DST_TYPE_IGNORE,			//	video_dst_type
		AVC_SRC_DST_TYPE_ISOCH_EXT_PLUG,	//	audio_dst_type
		AVC_SU_ANY_AVAILABLE,				//	video_src
		AVC_SU_ANY_AVAILABLE,				//	audio_src
		AVC_SU_ANY_AVAILABLE,				//	video_dst
		AVC_PLUG_ID_ANY_AVAILABLE_ISOCH,	//	audio_dst
	} 
};

HRESULT avcCmdCliConnectAV(char cctype, uint32 index)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	uint32				data[AVC_CMD_CONNECT_AV_QUADLETS]; 
	uint8				opcode = AVC_CMD_CONNECT_AV; 
	uint8				ctype = 0;
	uint32				i;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	switch (ctype)
	{
		case AVC_CTYPE_CONTROL:
			if (index >= CONNECT_AV_CONN_CONTROL_ITEMS) return E_BAD_INPUT_PARAMETERS;

			for (i = 0; i < 4; i++)
			{
				hResult = dsWriteBits(&ds, 2, connectAVConnControl[index][i]);
				if (hResult != NO_ERROR) return hResult;
			}
			for (i = 0; i < 4; i++)
			{
				hResult = dsWrite8Bits(&ds, (BYTE) connectAVConnControl[index][i+4]);
				if (hResult != NO_ERROR) return hResult;
			}
			break;
		case AVC_CTYPE_STATUS:
		case AVC_CTYPE_NOTIFY:	
			if (index >= CONNECT_AV_CONN_STATUS_ITEMS) return E_BAD_INPUT_PARAMETERS;

			for (i = 0; i < 4; i++)
			{
				hResult = dsWriteBits(&ds, 2, connectAVConnStatus[index][i]);
				if (hResult != NO_ERROR) return hResult;
			}
			for (i = 0; i < 4; i++)
			{
				hResult = dsWrite8Bits(&ds, (BYTE) connectAVConnStatus[index][i+4]);
				if (hResult != NO_ERROR) return hResult;
			}
			break;
		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			break;
	}

	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcCmdCliConnections(char cctype)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	BYTE				ctype = 0;
	uint32				data[AVC_CMD_CONNECTIONS_QUADLETS]; 
	uint8				opcode = AVC_CMD_CONNECTIONS; 

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite8Bits(&ds, 0xFF);						// operand[0]
	if (hResult != NO_ERROR) return hResult;
	
	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcCmdCliDigitalInput(char cctype, uint8 connection_state)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	uint32				data[AVC_CMD_DIGITAL_INPUT_QUADLETS]; 
	uint8				opcode = AVC_CMD_DIGITAL_INPUT; 
	uint8				ctype = 0;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite8Bits(&ds, connection_state);			// operand[0]
	if (hResult != NO_ERROR) return hResult;
	
	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcCmdCliDigitalOutput(char cctype, uint8 connection_state)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	uint32				data[AVC_CMD_DIGITAL_OUTPUT_QUADLETS]; 
	uint8				opcode = AVC_CMD_DIGITAL_OUTPUT; 
	uint8				ctype = 0;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite8Bits(&ds, connection_state);			// operand[0]
	if (hResult != NO_ERROR) return hResult;
	
	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcCmdCliDisconnect(char cctype, uint32 index)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	uint32				data[AVC_CMD_DISCONNECT_QUADLETS]; 
	uint8				opcode = AVC_CMD_DISCONNECT; 
	uint8				ctype = 0;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	switch (ctype)
	{
		case AVC_CTYPE_CONTROL:
			if (index >= CONNECT_CONN_CONTROL_ITEMS) return E_BAD_INPUT_PARAMETERS;
			hResult = avcSubunitEncodeConnection(&ds, &connectConnControl[index]);
			if (hResult != NO_ERROR) return hResult;
			break;
		case AVC_CTYPE_STATUS:
		case AVC_CTYPE_NOTIFY:	
		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			break;
	}

	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcCmdCliDisconnectAV(char cctype, uint32 index)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	uint32				data[AVC_CMD_DISCONNECT_AV_QUADLETS]; 
	uint8				opcode = AVC_CMD_DISCONNECT_AV; 
	uint8				ctype = 0;
	uint32				i;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	switch (ctype)
	{
		case AVC_CTYPE_CONTROL:
			if (index >= CONNECT_AV_CONN_CONTROL_ITEMS) return E_BAD_INPUT_PARAMETERS;

			for (i = 0; i < 4; i++)
			{
				hResult = dsWriteBits(&ds, 2, connectAVConnControl[index][i]);
				if (hResult != NO_ERROR) return hResult;
			}
			for (i = 0; i < 4; i++)
			{
				hResult = dsWrite8Bits(&ds, (BYTE) connectAVConnControl[index][i+4]);
				if (hResult != NO_ERROR) return hResult;
			}
			break;
		case AVC_CTYPE_STATUS:
		case AVC_CTYPE_NOTIFY:	
		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			break;
	}

	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

static HRESULT avcCmdCliSignalFormat(BOOL bIn, char cctype, BYTE plugID, uint32 format)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	uint32				data[AVC_CMD_SIGNAL_FORMAT_QUADLETS]; 
	uint8				opcode = AVC_CMD_INPUT_PLUG_SIGNAL_FORMAT; 
	uint8				ctype = 0;

	if (bIn)
	{
		opcode = AVC_CMD_INPUT_PLUG_SIGNAL_FORMAT; 
	}
	else
	{
		opcode = AVC_CMD_OUTPUT_PLUG_SIGNAL_FORMAT; 
	}

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	switch (ctype)
	{
		case AVC_CTYPE_CONTROL:
			hResult = dsWrite8Bits(&ds, plugID);
			if (hResult != NO_ERROR) return hResult;

			hResult = dsWrite32Bits(&ds, (format & 0x3FFFFFFF) | 0x80000000);	// eoh:1=1, form:1=0, (keeping fmt and fdf)
			if (hResult != NO_ERROR) return hResult;
			break;
		case AVC_CTYPE_STATUS:
		case AVC_CTYPE_NOTIFY:	

		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			hResult = dsWrite8Bits(&ds, plugID);
			if (hResult != NO_ERROR) return hResult;

			hResult = dsWrite32Bits(&ds, 0xFFFFFFFF);							// eoh:1=1, form:1=1, fmt:6:3F, fdf:24:FFFFFF
			if (hResult != NO_ERROR) return hResult;
			break;
	}

	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcCmdCliInputPlugSignalFormat(char cctype, BYTE plugID, uint32 format)
{
	return avcCmdCliSignalFormat(TRUE, cctype, plugID, format);
}

HRESULT avcCmdCliOutputPlugSignalFormat(char cctype, BYTE plugID, uint32 format)
{
	return avcCmdCliSignalFormat(FALSE, cctype, plugID, format);
}

HRESULT avcCmdCliGeneralBusSetup(char cctype, BYTE bus_type)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	uint32				data[AVC_CMD_GENERAL_BUS_QUADLETS]; 
	uint8				opcode = AVC_CMD_GENERAL_BUS_SETUP; 
	uint8				ctype = 0;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite8Bits(&ds, bus_type);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

// AV/C CCM Commands functions
HRESULT avcCmdCliSignalSource(char cctype, uint32 index)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	uint32				data[AVC_CMD_SIGNAL_SOURCE_QUADLETS];
	uint8				opcode = AVC_CMD_SIGNAL_SOURCE; 
	uint8				ctype = 0;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	UNUSED_ARG(index);

	switch (ctype)
	{
		case AVC_CTYPE_CONTROL:
			//LM???
			break;
		case AVC_CTYPE_STATUS:
		case AVC_CTYPE_NOTIFY:	
			//LM???
			break;
		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			break;
	}

	hResult = dsWrite8Bits(&ds, 0xFF);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite32Bits(&ds, 0xFFFEFF00);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcCmdCliInputSelect(char cctype, uint32 nodeAddr, BYTE subfunction)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	uint32				data[AVC_CMD_INPUT_SELECT_QUADLETS]; 
	uint8				opcode = AVC_CMD_INPUT_SELECT; 
	uint8				ctype = 0;
	QUADLET				quad1 = 0;
	QUADLET				quad2 = 0;
				
	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	switch (ctype)
	{
		case AVC_CTYPE_CONTROL:
			quad1 = 0x0F000000;	
			quad1 += nodeAddr << 8;
			quad2 = 0x00080000;

			//LM???
			break;
		case AVC_CTYPE_STATUS:
		case AVC_CTYPE_NOTIFY:	
			//LM???
			//break;
		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:

			subfunction = 0xFF;
			quad1 = 0xFFFFFFFF;
			//LM??? reserved must be 0x00 for pioneer vsa
			quad2 = 0x00FFFE00;
			break;
	}

	hResult = dsWrite8Bits(&ds, subfunction);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite32Bits(&ds, quad1);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite32Bits(&ds, quad2);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcCmdCliOutputPreset(char cctype, uint32 index)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	uint32				data[AVC_CMD_OUTPUT_PRESET_QUADLETS]; 
	uint8				opcode = AVC_CMD_OUTPUT_PRESET; 
	uint8				ctype = 0;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	UNUSED_ARG(index);

	switch (ctype)
	{
		case AVC_CTYPE_CONTROL:
			//LM???
			break;
		case AVC_CTYPE_STATUS:
		case AVC_CTYPE_NOTIFY:	
			//LM???
			break;
		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			break;
	}

	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcCmdCliCCMProfile(char cctype, uint32 index)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	uint32				data[AVC_CMD_CCM_PROFILE_QUADLETS]; 
	uint8				opcode = AVC_CMD_CCM_PROFILE; 
	uint8				ctype = 0;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	UNUSED_ARG(index);

	switch (ctype)
	{
		case AVC_CTYPE_CONTROL:
			//LM???
			break;
		case AVC_CTYPE_STATUS:
		case AVC_CTYPE_NOTIFY:	
			//LM???
			break;
		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			break;
	}

	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

#ifdef _AVC_SECURITY
// AV/C Security commands
HRESULT avcCmdCliSecurity(char cctype)
{
	HRESULT				hResult = NO_ERROR;
	DataStream			ds;
	uint32				data[AVC_CMD_SECURITY_QUADLETS]; 
	uint8				opcode = AVC_CMD_SECURITY; 
	uint8				ctype = 0;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), data);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWriteBits(&ds, 4, AVC_SECURITY_CATEGORY_DTCP_AKE);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWriteBits(&ds, 4, 0);							// akeID
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite32Bits(&ds, 0xFFFFFFFF);
	hResult = dsWrite32Bits(&ds, 0xFFFFFFFF);

//	hResult = dsPadToEndOfQuadlet(&ds, 0xFF);
//	if (hResult != NO_ERROR) return hResult;

	switch (ctype)
	{
		case AVC_CTYPE_CONTROL:
			//LM???
			break;
		case AVC_CTYPE_STATUS:
		case AVC_CTYPE_NOTIFY:	
			//LM???
			break;
		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			break;
	}

	hResult = avcCmdCliSendRequest(ctype, sizeof(data), data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}
#endif //_AVC_SECURITY

static HRESULT avcCmdCliWaitResponse(uint32 *receivedResponses)
{
	HRESULT				hResult = NO_ERROR;
	uint32				received = 0;
	uint32				starttime = 0;

	starttime = TCTimeGet();

	DO_FOREVER
	{
		received = avcUnitGetReceivedResponses();

		if (*receivedResponses + 1 == received)
		{
			break;
		}

		if (TCTimeElapsed(starttime) > 2000)
		{
			hResult = E_FAIL;
			break;
		}

		TCTaskWait(50);
	}

	*receivedResponses = received;

	return hResult;
}

// AV/C Check All commands
HRESULT avcCmdCliCheckAll(char cctype)
{
	HRESULT				hResult = NO_ERROR;
	uint32				receivedResponses = 0;
/*
	switch (ctype)
	{
		case AVC_CTYPE_CONTROL:
		case AVC_CTYPE_STATUS:
		case AVC_CTYPE_NOTIFY:	
			break;
		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			break;
	}
*/
	receivedResponses = avcUnitGetReceivedResponses();

	// AV/C General Commands functions
	hResult = avcCmdCliPower(cctype, 0x7f/*power_state*/);
	avcCmdCliWaitResponse(&receivedResponses);
	hResult = avcCmdCliUnitInfo(cctype);
	avcCmdCliWaitResponse(&receivedResponses);
	hResult = avcCmdCliSubunitInfo(cctype, 0 /*page*/);
	avcCmdCliWaitResponse(&receivedResponses);
	hResult = avcCmdCliReserve(cctype, 0x04 /*priority*/, "test" /*text*/);
	avcCmdCliWaitResponse(&receivedResponses);
	hResult = avcCmdCliVersion(cctype, 0xff /*version*/);
	avcCmdCliWaitResponse(&receivedResponses);
	hResult = avcCmdCliVendor(cctype);
	avcCmdCliWaitResponse(&receivedResponses);

	// AV/C Connection Commands functions
	hResult = avcCmdCliPlugInfo(cctype, 0 /*subfunction*/);
	avcCmdCliWaitResponse(&receivedResponses);
	hResult = avcCmdCliChannelUsage(cctype, 0 /*channel*/);
	avcCmdCliWaitResponse(&receivedResponses);
	hResult = avcCmdCliConnect(cctype, 0 /*index*/);
	avcCmdCliWaitResponse(&receivedResponses);
	hResult = avcCmdCliConnectAV(cctype, 0 /*index*/);
	avcCmdCliWaitResponse(&receivedResponses);
	hResult = avcCmdCliConnections(cctype);
	avcCmdCliWaitResponse(&receivedResponses);
	hResult = avcCmdCliDigitalInput(cctype, 0xff /*connection_state*/);
	avcCmdCliWaitResponse(&receivedResponses);
	hResult = avcCmdCliDigitalOutput(cctype, 0xff /*connection_state*/);
	avcCmdCliWaitResponse(&receivedResponses);
	hResult = avcCmdCliDisconnect(cctype, 0 /*index*/);
	avcCmdCliWaitResponse(&receivedResponses);
	hResult = avcCmdCliDisconnectAV(cctype, 0 /*index*/);
	avcCmdCliWaitResponse(&receivedResponses);
	hResult = avcCmdCliInputPlugSignalFormat(cctype, 0 /*plugID*/, 0 /*format*/);
	avcCmdCliWaitResponse(&receivedResponses);
	hResult = avcCmdCliOutputPlugSignalFormat(cctype, 0 /*plugID*/, 0 /*format*/);
	avcCmdCliWaitResponse(&receivedResponses);
	hResult = avcCmdCliGeneralBusSetup(cctype, 0 /*bus_type*/);
	avcCmdCliWaitResponse(&receivedResponses);

	// AV/C CCM Commands functions
	hResult = avcCmdCliSignalSource(cctype, 0 /*index*/);
	avcCmdCliWaitResponse(&receivedResponses);
	hResult = avcCmdCliInputSelect(cctype, 0 /*index*/, 0);
	avcCmdCliWaitResponse(&receivedResponses);
	hResult = avcCmdCliOutputPreset(cctype, 0 /*index*/);
	avcCmdCliWaitResponse(&receivedResponses);
	hResult = avcCmdCliCCMProfile(cctype, 0 /*index*/);
	avcCmdCliWaitResponse(&receivedResponses);

#if 0 //def _AVC_SECURITY
	// AV/C Security commands
	hResult = avcCmdCliSecurity(cctype);
	avcCmdCliWaitResponse(&receivedResponses);
#endif //_AVC_SECURITY

	return hResult;
}


// AV/C Set Target Tool
static  char	avcCmdSetTarget_UsageString[]=\
				"avc:settarget\n\r"\
				"use: avc:settarget <targetNode> <targetUnit>\n\r"\
				"targetNode: 'local', 'other' or use showtargets for supported devices\n\r"\
				"targetUnit: unit:u, audio subunit:a, panel subunit:p\n\r"\
				"------------------------------------------------------------\n\r"\
				"set target mode (node and unit)\n\r";

static	CLIDescriptor	avcCmdSetTarget_Descriptor[]=
{
	{kCLI_Name,				(uint32)"settarget"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliSetTarget},
	{kCLI_UsageString, 		(uint32)avcCmdSetTarget_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"sc"},
	{kCLI_ModuleName,		(uint32)"set target mode"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Get Target Tool
static  char	avcCmdGetTarget_UsageString[]=\
				"avc:gettarget\n\r"\
				"use: avc:gettarget\n\r"\
				"------------------------------------------------------------\n\r"\
				"get target mode\n\r";

static	CLIDescriptor	avcCmdGetTarget_Descriptor[]=
{
	{kCLI_Name,				(uint32)"gettarget"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliGetTarget},
	{kCLI_UsageString, 		(uint32)avcCmdGetTarget_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"get target mode"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Show Targets Tool
static  char	avcCmdShowTargets_UsageString[]=\
				"avc:showtargets\n\r"\
				"use: avc:showtargets\n\r"\
				"------------------------------------------------------------\n\r"\
				"show target devices\n\r";

static	CLIDescriptor	avcCmdShowTargets_Descriptor[]=
{
	{kCLI_Name,				(uint32)"showtargets"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliShowTargets},
	{kCLI_UsageString, 		(uint32)avcCmdShowTargets_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"show target devices"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Power Command Tool
static  char	avcCmdPower_UsageString[]=\
				"avc:power: \n\r"\
				"use: avc:power <ctype> <power_state>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"power_state: off:0x60 on:0x70 status:0x7f\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc power command\n\r";

static	CLIDescriptor	avcCmdPower_Descriptor[]=
{
	{kCLI_Name,				(uint32)"power"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliPower},
	{kCLI_UsageString, 		(uint32)avcCmdPower_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ci"},
	{kCLI_ModuleName,		(uint32)"sends avc power command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Unit Info Command Tool
static  char	avcCmdUnitInfo_UsageString[]=\
				"avc:unitinfo: \n\r"\
				"use: avc:unitinfo <ctype> \n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc unit info status command\n\r";

static	CLIDescriptor	avcCmdUnitInfo_Descriptor[]=
{
	{kCLI_Name,				(uint32)"unitinfo"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliUnitInfo},
	{kCLI_UsageString, 		(uint32)avcCmdUnitInfo_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"c"},
	{kCLI_ModuleName,		(uint32)"sends avc unit info status command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Subunit Info Command Tool
static  char	avcCmdSubunitInfo_UsageString[]=\
				"avc:subunitinfo: \n\r"\
				"use: avc:subunitinfo <ctype> <page>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"page: 0,..,7\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc subunit info status command\n\r";

static	CLIDescriptor	avcCmdSubunitInfo_Descriptor[]=
{
	{kCLI_Name,				(uint32)"subunitinfo"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliSubunitInfo},
	{kCLI_UsageString, 		(uint32)avcCmdSubunitInfo_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ci"},
	{kCLI_ModuleName,		(uint32)"sends avc subunit info status command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Reserve Command Tool
static  char	avcCmdReserve_UsageString[]=\
				"avc:reserve: \n\r"\
				"use: avc:reserve <ctype> <priority> <text>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"priority:0x00,..,0x0f\n\r"\
				"text:12 bytes of ASCII or ""\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc reserve command\n\r";

static	CLIDescriptor	avcCmdReserve_Descriptor[]=
{
	{kCLI_Name,				(uint32)"reserve"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliReserve},
	{kCLI_UsageString, 		(uint32)avcCmdReserve_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"cis"},
	{kCLI_ModuleName,		(uint32)"sends avc reserve command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Version Command Tool
static  char	avcCmdVersion_UsageString[]=\
				"avc:version: \n\r"\
				"use: avc:version <ctype> <version_information>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"version_information: 0xff (latest version) 0x00,..,0xfe (specific version)\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc version status command\n\r";

static	CLIDescriptor	avcCmdVersion_Descriptor[]=
{
	{kCLI_Name,				(uint32)"version"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliVersion},
	{kCLI_UsageString, 		(uint32)avcCmdVersion_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ci"},
	{kCLI_ModuleName,		(uint32)"sends avc version status command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Vendor Command Tool
static  char	avcCmdVendor_UsageString[]=\
				"avc:vendor: \n\r"\
				"use: avc:vendor <ctype> \n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc vendor status command\n\r";

static	CLIDescriptor	avcCmdVendor_Descriptor[]=
{
	{kCLI_Name,				(uint32)"vendor"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliVendor},
	{kCLI_UsageString, 		(uint32)avcCmdVendor_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"c"},
	{kCLI_ModuleName,		(uint32)"sends avc vendor status command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Plug Info Command Tool
static  char	avcCmdPlugInfo_UsageString[]=\
				"avc:pluginfo: \n\r"\
				"use: avc:pluginfo <ctype> <subfunction>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"subfunction: 0x00: Isoch+Ext (subunit), 0x01: Async, 0x40-0x7F: General Bus\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc plug info status command\n\r";

static	CLIDescriptor	avcCmdPlugInfo_Descriptor[]=
{
	{kCLI_Name,				(uint32)"pluginfo"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliPlugInfo},
	{kCLI_UsageString, 		(uint32)avcCmdPlugInfo_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ci"},
	{kCLI_ModuleName,		(uint32)"sends avc plug info status command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Channel Usage Command Tool
static  char	avcCmdChannelUsage_UsageString[]=\
				"avc:channelusage: \n\r"\
				"use: avc:channelusage <ctype> <channel>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"channel: 0,..,63\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc channel usage status/notify command\n\r";

static	CLIDescriptor	avcCmdChannelUsage_Descriptor[]=
{
	{kCLI_Name,				(uint32)"channelusage"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliChannelUsage},
	{kCLI_UsageString, 		(uint32)avcCmdChannelUsage_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ci"},
	{kCLI_ModuleName,		(uint32)"sends avc channel usage status/notify command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Connect Command Tool
static  char	avcCmdConnect_UsageString[]=\
				"avc:connect: \n\r"\
				"use: avc:connect <ctype> <index>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"index in tables of connection combinations\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc connect command\n\r";

static	CLIDescriptor	avcCmdConnect_Descriptor[]=
{
	{kCLI_Name,				(uint32)"connect"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliConnect},
	{kCLI_UsageString, 		(uint32)avcCmdConnect_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ci"},
	{kCLI_ModuleName,		(uint32)"sends avc connect command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C ConnectAV Command Tool
static  char	avcCmdConnectAV_UsageString[]=\
				"avc:connectAV: \n\r"\
				"use: avc:connectAV <ctype> <index>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"index in tables of connection combinations\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc connectAV command\n\r";

static	CLIDescriptor	avcCmdConnectAV_Descriptor[]=
{
	{kCLI_Name,				(uint32)"connectAV"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliConnectAV},
	{kCLI_UsageString, 		(uint32)avcCmdConnectAV_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ci"},
	{kCLI_ModuleName,		(uint32)"sends avc connectAV command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Connections Command Tool
static  char	avcCmdConnections_UsageString[]=\
				"avc:connections: \n\r"\
				"use: avc:connections <ctype> \n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc connections status command\n\r";

static	CLIDescriptor	avcCmdConnections_Descriptor[]=
{
	{kCLI_Name,				(uint32)"connections"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliConnections},
	{kCLI_UsageString, 		(uint32)avcCmdConnections_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"c"},
	{kCLI_ModuleName,		(uint32)"sends avc connections status command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Digital Input Command Tool
static  char	avcCmdDigitalInput_UsageString[]=\
				"avc:digitalinput: \n\r"\
				"use: avc:digitalinput <ctype> <connection_state>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"connection_state: break:0x60 establish:0x70 status:0xff\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc digital input command\n\r";

static	CLIDescriptor	avcCmdDigitalInput_Descriptor[]=
{
	{kCLI_Name,				(uint32)"digitalinput"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliDigitalInput},
	{kCLI_UsageString, 		(uint32)avcCmdDigitalInput_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ci"},
	{kCLI_ModuleName,		(uint32)"sends avc digital input command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Digital Output Command Tool
static  char	avcCmdDigitalOutput_UsageString[]=\
				"avc:digitaloutput: \n\r"\
				"use: avc:digitaloutput <ctype> <connection_state>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"connection_state: break:0x60 establish:0x70 status:0xff\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc digital output command\n\r";

static	CLIDescriptor	avcCmdDigitalOutput_Descriptor[]=
{
	{kCLI_Name,				(uint32)"digitaloutput"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliDigitalOutput},
	{kCLI_UsageString, 		(uint32)avcCmdDigitalOutput_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ci"},
	{kCLI_ModuleName,		(uint32)"sends avc digital output command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Disconnect Command Tool
static  char	avcCmdDisconnect_UsageString[]=\
				"avc:disconnect: \n\r"\
				"use: avc:disconnect <ctype> <index>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"index in tables of connection combinations\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc disconnect command\n\r";

static	CLIDescriptor	avcCmdDisconnect_Descriptor[]=
{
	{kCLI_Name,				(uint32)"disconnect"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliDisconnect},
	{kCLI_UsageString, 		(uint32)avcCmdDisconnect_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ci"},
	{kCLI_ModuleName,		(uint32)"sends avc disconnect command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C DisconnectAV Command Tool
static  char	avcCmdDisconnectAV_UsageString[]=\
				"avc:disconnectAV: \n\r"\
				"use: avc:disconnectAV <ctype> <index>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"index in tables of connection combinations\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc disconnectAV command\n\r";

static	CLIDescriptor	avcCmdDisconnectAV_Descriptor[]=
{
	{kCLI_Name,				(uint32)"disconnectAV"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliDisconnectAV},
	{kCLI_UsageString, 		(uint32)avcCmdDisconnectAV_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ci"},
	{kCLI_ModuleName,		(uint32)"sends avc disconnectAV command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C input plug signal format Command Tool
static  char	avcCmdInputPlugSignalFormat_UsageString[]=\
				"avc:ipsf: \n\r"\
				"use: avc:ipsf <ctype> <plugID> <format>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"plugID: 0,.,,30\n\r"\
				"format: plug signal format [0|0|fmt:6|fdf:24]\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc input plug signal format command\n\r";

static	CLIDescriptor	avcCmdInputPlugSignalFormat_Descriptor[]=
{
	{kCLI_Name,				(uint32)"ipsf"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliInputPlugSignalFormat},
	{kCLI_UsageString, 		(uint32)avcCmdInputPlugSignalFormat_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"cii"},
	{kCLI_ModuleName,		(uint32)"sends avc input plug signal format command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C output plug signal format Command Tool
static  char	avcCmdOutputPlugSignalFormat_UsageString[]=\
				"avc:opsf: \n\r"\
				"use: avc:opsf <ctype> <plugID> <format>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"plugID: 0,.,,30\n\r"\
				"format: plug signal format [0|0|fmt:6|fdf:24]\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc output plug signal format command\n\r";

static	CLIDescriptor	avcCmdOutputPlugSignalFormat_Descriptor[]=
{
	{kCLI_Name,				(uint32)"opsf"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliOutputPlugSignalFormat},
	{kCLI_UsageString, 		(uint32)avcCmdOutputPlugSignalFormat_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"cii"},
	{kCLI_ModuleName,		(uint32)"sends avc output plug signal format command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C general bus setup Command Tool
static  char	avcCmdGeneralBusSetup_UsageString[]=\
				"avc:generalbus: \n\r"\
				"use: avc:generalbus <ctype> <index>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"index in tables of connection combinations\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc general bus setup command\n\r";

static	CLIDescriptor	avcCmdGeneralBusSetup_Descriptor[]=
{
	{kCLI_Name,				(uint32)"generalbus"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliGeneralBusSetup},
	{kCLI_UsageString, 		(uint32)avcCmdGeneralBusSetup_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ci"},
	{kCLI_ModuleName,		(uint32)"sends avc general bus setup command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C signal source Command Tool
static  char	avcCmdSignalSource_UsageString[]=\
				"avc:signalsource: \n\r"\
				"use: avc:signalsource <ctype> <index>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"index in tables of connection combinations\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc signalsource command\n\r";

static	CLIDescriptor	avcCmdSignalSource_Descriptor[]=
{
	{kCLI_Name,				(uint32)"signalsource"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliSignalSource},
	{kCLI_UsageString, 		(uint32)avcCmdSignalSource_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ci"},
	{kCLI_ModuleName,		(uint32)"sends avc signalsource command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C input select Command Tool
static  char	avcCmdInputSelect_UsageString[]=\
				"avc:inputselect: \n\r"\
				"use: avc:inputselect <ctype> <nodeAddr> <subfuntion>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"subfuntion: 0:connect, 1:path chg, 2:select, 3:disconnect\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc inputselect command\n\r";

static	CLIDescriptor	avcCmdInputSelect_Descriptor[]=
{
	{kCLI_Name,				(uint32)"inputselect"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliInputSelect},
	{kCLI_UsageString, 		(uint32)avcCmdInputSelect_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"cii"},
	{kCLI_ModuleName,		(uint32)"sends avc inputselect command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C output preset Command Tool
static  char	avcCmdOutputPreset_UsageString[]=\
				"avc:outputpreset: \n\r"\
				"use: avc:outputpreset <ctype> <index>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"index in tables of connection combinations\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc outputpreset command\n\r";

static	CLIDescriptor	avcCmdOutputPreset_Descriptor[]=
{
	{kCLI_Name,				(uint32)"outputpreset"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliOutputPreset},
	{kCLI_UsageString, 		(uint32)avcCmdOutputPreset_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ci"},
	{kCLI_ModuleName,		(uint32)"sends avc outputpreset command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C CCM profile Command Tool
static  char	avcCmdCCMProfile_UsageString[]=\
				"avc:CCMprofile: \n\r"\
				"use: avc:CCMprofile <ctype> <index>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"index in tables of connection combinations\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc CCMprofile command\n\r";

static	CLIDescriptor	avcCmdCCMProfile_Descriptor[]=
{
	{kCLI_Name,				(uint32)"CCMprofile"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliCCMProfile},
	{kCLI_UsageString, 		(uint32)avcCmdCCMProfile_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ci"},
	{kCLI_ModuleName,		(uint32)"sends avc CCMprofile command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

#ifdef _AVC_SECURITY
// AV/C Security Command Tool
static  char	avcCmdSecurity_UsageString[]=\
				"avc:security: \n\r"\
				"use: avc:security <ctype>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc security command\n\r";

static	CLIDescriptor	avcCmdSecurity_Descriptor[]=
{
	{kCLI_Name,				(uint32)"security"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliSecurity},
	{kCLI_UsageString, 		(uint32)avcCmdSecurity_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"c"},
	{kCLI_ModuleName,		(uint32)"sends avc security command"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};
#endif //_AVC_SECURITY

// AV/C check all Command Tool
static  char	avcCmdCheckAll_UsageString[]=\
				"avc:checkall: \n\r"\
				"use: avc:checkall <ctype>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"------------------------------------------------------------\n\r"\
				"performs avc check all (e.g. use general or specific inquiry)\n\r";

static	CLIDescriptor	avcCmdCheckAll_Descriptor[]=
{
	{kCLI_Name,				(uint32)"checkall"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcCmdCliCheckAll},
	{kCLI_UsageString, 		(uint32)avcCmdCheckAll_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"c"},
	{kCLI_ModuleName,		(uint32)"performs avc check all"},
	{kCLI_GroupName,		(uint32)"avc"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void avcCmdCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(avcCmdSetTarget_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdGetTarget_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdShowTargets_Descriptor);

	hResult += cliInstallCLIDescriptor(avcCmdPower_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdUnitInfo_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdSubunitInfo_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdReserve_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdVersion_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdVendor_Descriptor);

	hResult += cliInstallCLIDescriptor(avcCmdPlugInfo_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdChannelUsage_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdConnect_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdConnectAV_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdConnections_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdDigitalInput_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdDigitalOutput_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdDisconnect_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdDisconnectAV_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdInputPlugSignalFormat_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdOutputPlugSignalFormat_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdGeneralBusSetup_Descriptor);

	hResult += cliInstallCLIDescriptor(avcCmdSignalSource_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdInputSelect_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdOutputPreset_Descriptor);
	hResult += cliInstallCLIDescriptor(avcCmdCCMProfile_Descriptor);

#ifdef _AVC_SECURITY
	hResult += cliInstallCLIDescriptor(avcCmdSecurity_Descriptor);
#endif //_AVC_SECURITY

	hResult += cliInstallCLIDescriptor(avcCmdCheckAll_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing avc unit cmd cli tools");
	}
}

#endif //_CLI_TOOL_AVC_CMD
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
