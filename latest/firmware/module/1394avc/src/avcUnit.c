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

	Implementation of avc unit commands
	AV/C Digital Interface Command Set General (Ver. 4.1) 
	section 11 - General Commands
	section 12 - Connection Commands

	AV/C Connection and Compatibility Management (CCM) Specification (Ver. 1.0)
	section 7 - Unit Commands

  Revisions:
		created 08/04/2003 LM

****************************************************************************/

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "cr.h"
#include "systemCallbacks.h"
#include "avcDefs.h"
#include "avcCmdGeneral.h"
#include "avcCmdConnection.h"
#include "avcCmdStreamFormat.h"
#include "avcCmdCCM.h"
#include "avcUnit.h"
#include "avcUnitNotify.h"
#include "avcUnitPower.h"
#include "avcUnitReserve.h"
#include "avcUnitChannelUsage.h"
#include "avcUnitConnect.h"
#include "avcUnitConnectAV.h"
#include "avcUnitSignalFormat.h"				// INPUT and OUTPUT PLUG SIGNAL FORMAT
#include "avcUnitSignalSource.h"
#include "avcUnitVendor.h"
#include "avcUnitCli.h"
#include "avcRspCli.h"
#ifdef _AVC_SECURITY
#include "avcUnitSecurity.h"
#endif //_AVC_SECURITY

// Subunit Callback Descriptor for the UNIT subunit type.
static CALLBACK_DESCRIPTOR AVC_UNIT_CALLBACK[] =
{
	{CALLBACK_DESCRIPTOR_TYPE, AVC_SUBUNIT_TYPE},
	{CALLBACK_DESCRIPTOR_NEXT, 0},
	{CALLBACK_DESCRIPTOR_NAME, (uint32)"AVC_UNIT_CALLBACK"},
	{AVC_SUBUNIT_CALLBACK_KEY, (uint32)&avcUnitCallback}, // CALLBACK PTR of TYPE AVC_SUBUNIT_CALLBACK},
	{AVC_SUBUNIT_SUBUNITTYPE_KEY, AVC_SU_TYPE_UNIT},
	{AVC_SUBUNIT_ID_KEY, AVC_SU_ID_IGNORE},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

HRESULT avcUnitCallback(AVC_HEADER *pHeader, PB *packetBlock)
{
	HRESULT			hResult = NO_ERROR;
	pDataStream		pStream = NULL;
	
	hResult = pbGetApplicationDatastream(packetBlock, &pStream);
	if (hResult != NO_ERROR)
	{
		hResult = E_PKT_AVC_NOT_IMPLEMENTED; // try to recover
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	if (avcCtypeIsCommand(pHeader->ctype) == TRUE)
	{
		// commands
#ifdef _SYSDEBUG
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
		{
			if (avcUnitCliIsPrintMode(AVC_UNIT_PRINT_RCV_COMMAND_ALL))
			{
				avcUnitCliPrintFrame(pHeader, pStream);
			}
		}
#endif //_SYSDEBUG

		hResult = E_PKT_AVC_NOT_IMPLEMENTED;

		switch (pHeader->opcode)
		{
			// General Commands
			case AVC_CMD_POWER:						hResult = avcCmdPower(pHeader, packetBlock, pStream); break;
			case AVC_CMD_UNIT_INFO:					hResult = avcCmdUnitInfo(pHeader, packetBlock, pStream); break;
			case AVC_CMD_SUBUNIT_INFO:				hResult = avcCmdSubunitInfo(pHeader, packetBlock, pStream); break;
			case AVC_CMD_RESERVE:					hResult = avcCmdReserve(pHeader, packetBlock, pStream); break;
			case AVC_CMD_VERSION:					hResult = avcCmdVersion(pHeader, packetBlock, pStream); break;
			case AVC_CMD_VENDOR_DEPENDENT:			hResult = avcCmdVendorDependent(pHeader, packetBlock, pStream); break;
			
			// Connection Commands
			case AVC_CMD_PLUG_INFO:					hResult = avcCmdPlugInfo(pHeader, packetBlock, pStream); break;
			case AVC_CMD_CHANNEL_USAGE:				hResult = avcCmdChannelUsage(pHeader, packetBlock, pStream); break;
			case AVC_CMD_CONNECT:					hResult = avcCmdConnect(pHeader, packetBlock, pStream); break;
			case AVC_CMD_CONNECT_AV:				hResult = avcCmdConnectAV(pHeader, packetBlock, pStream); break;
			case AVC_CMD_CONNECTIONS:				hResult = avcCmdConnections(pHeader, packetBlock, pStream); break;
			case AVC_CMD_DIGITAL_INPUT:				hResult = avcCmdDigitalInput(pHeader, packetBlock, pStream); break;
			case AVC_CMD_DIGITAL_OUTPUT:			hResult = avcCmdDigitalOutput(pHeader, packetBlock, pStream); break;
			case AVC_CMD_DISCONNECT:				hResult = avcCmdDisconnect(pHeader, packetBlock, pStream); break;
			case AVC_CMD_DISCONNECT_AV:				hResult = avcCmdDisconnectAV(pHeader, packetBlock, pStream); break;
			case AVC_CMD_INPUT_PLUG_SIGNAL_FORMAT:	hResult = avcCmdInputPlugSignalFormat(pHeader, packetBlock, pStream); break;
			case AVC_CMD_OUTPUT_PLUG_SIGNAL_FORMAT:	hResult = avcCmdOutputPlugSignalFormat(pHeader, packetBlock, pStream); break;
			case AVC_CMD_GENERAL_BUS_SETUP:			hResult = avcCmdGeneralBusSetup(pHeader, packetBlock, pStream); break;
			case AVC_CMD_STREAM_FMT_SUPPORT:		hResult = avcCmdStreamFmtSupport(pHeader, packetBlock, pStream, AVC_SU_TYPE_UNIT); break;
			case AVC_CMD_STREAM_FMT_EXT_INFO:		hResult = avcCmdStreamFmtExtInfo(pHeader, packetBlock, pStream); break;

			// CCM Commands
			case AVC_CMD_SIGNAL_SOURCE:				hResult = avcCmdSignalSource(pHeader, packetBlock, pStream); break;
			case AVC_CMD_INPUT_SELECT:				hResult = avcCmdInputSelect(pHeader, packetBlock, pStream); break;
			case AVC_CMD_OUTPUT_PRESET:				hResult = avcCmdOutputPreset(pHeader, packetBlock, pStream); break;
			case AVC_CMD_CCM_PROFILE:				hResult = avcCmdCCMProfile(pHeader, packetBlock, pStream); break;

			// Other Commands

#ifdef _AVC_SECURITY
			case AVC_CMD_SECURITY:					hResult = avcSecurity(pHeader, packetBlock, pStream); break;
#endif //_AVC_SECURITY
			
			default:
				break;
		}

#ifdef _CLI
		avcUnitIncrementReceivedCommands();
#endif //_CLI

	}
	else if (avcCtypeIsResponse(pHeader->ctype) == TRUE)
	{
		// response

		if ((avcUnitCliIsPrintMode(AVC_UNIT_PRINT_RCV_RESPONSE_ALL)) ||
			(avcUnitCliIsPrintMode(AVC_UNIT_PRINT_RCV_RESPONSE_IMPLEMENTED) && pHeader->ctype != AVC_RESPONSE_NOT_IMPLEMENTED) ||
			(avcUnitCliIsPrintMode(AVC_UNIT_PRINT_RCV_RESPONSE_NOT_IMPLEMENTED) && pHeader->ctype == AVC_RESPONSE_NOT_IMPLEMENTED))
		{
#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				avcUnitCliPrintFrame(pHeader, pStream);
			}
#endif //_SYSDEBUG

			switch (pHeader->opcode)
			{
				// General Commands
				case AVC_CMD_POWER:						hResult = avcRspCliPower(pHeader, packetBlock, pStream); break;
				case AVC_CMD_UNIT_INFO:					hResult = avcRspCliUnitInfo(pHeader, packetBlock, pStream); break;
				case AVC_CMD_SUBUNIT_INFO:				hResult = avcRspCliSubunitInfo(pHeader, packetBlock, pStream); break;
				case AVC_CMD_RESERVE:					hResult = avcRspCliReserve(pHeader, packetBlock, pStream); break;
				case AVC_CMD_VERSION:					hResult = avcRspCliVersion(pHeader, packetBlock, pStream); break;
				case AVC_CMD_VENDOR_DEPENDENT:			hResult = avcRspCliVendor(pHeader, packetBlock, pStream); break;

				// Connection Commands
				case AVC_CMD_PLUG_INFO:					hResult = avcRspCliPlugInfo(pHeader, packetBlock, pStream); break;
				case AVC_CMD_CHANNEL_USAGE:				hResult = avcRspCliChannelUsage(pHeader, packetBlock, pStream); break;
				case AVC_CMD_CONNECT:					hResult = avcRspCliConnect(pHeader, packetBlock, pStream); break;
				case AVC_CMD_CONNECT_AV:				hResult = avcRspCliConnectAV(pHeader, packetBlock, pStream); break;
				case AVC_CMD_CONNECTIONS:				hResult = avcRspCliConnections(pHeader, packetBlock, pStream); break;
				case AVC_CMD_DIGITAL_INPUT:				hResult = avcRspCliDigitalInput(pHeader, packetBlock, pStream); break;
				case AVC_CMD_DIGITAL_OUTPUT:			hResult = avcRspCliDigitalOutput(pHeader, packetBlock, pStream); break;
				case AVC_CMD_DISCONNECT:				hResult = avcRspCliDisconnect(pHeader, packetBlock, pStream); break;
				case AVC_CMD_DISCONNECT_AV:				hResult = avcRspCliDisconnectAV(pHeader, packetBlock, pStream); break;
				case AVC_CMD_INPUT_PLUG_SIGNAL_FORMAT:	hResult = avcRspCliInputPlugSignalFormat(pHeader, packetBlock, pStream); break;
				case AVC_CMD_OUTPUT_PLUG_SIGNAL_FORMAT:	hResult = avcRspCliOutputPlugSignalFormat(pHeader, packetBlock, pStream); break;
				case AVC_CMD_GENERAL_BUS_SETUP:			hResult = avcRspCliGeneralBusSetup(pHeader, packetBlock, pStream); break;
					
				// CCM Commands
				case AVC_CMD_SIGNAL_SOURCE:				hResult = avcRspCliSignalSource(pHeader, packetBlock, pStream); break;
				case AVC_CMD_INPUT_SELECT:				hResult = avcRspCliInputSelect(pHeader, packetBlock, pStream); break;
				case AVC_CMD_OUTPUT_PRESET:				hResult = avcRspCliOutputPreset(pHeader, packetBlock, pStream); break;
				case AVC_CMD_CCM_PROFILE:				hResult = avcRspCliCCMProfile(pHeader, packetBlock, pStream); break;

				// Other Commands
				
#ifdef _AVC_SECURITY
				case AVC_CMD_SECURITY:					hResult = avcSecurity(pHeader, packetBlock, pStream); break;
#endif //_AVC_SECURITY

				default:
					break;
			}
		}

#ifdef _CLI
		avcUnitIncrementReceivedResponses();
#endif //_CLI

		hResult = NO_ERROR;

	}
	else
	{
		// unknown ctype
		hResult = E_PKT_AVC_NOT_IMPLEMENTED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT avcUnitResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avc unit reserve
	hResult = avcUnitReserveResumeTasks();
	if (hResult != NO_ERROR) return hResult;

	// avc unit notify
	hResult = avcUnitNotifyResumeTasks();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT avcUnitSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avc unit reserve
	hResult = avcUnitReserveSuspendTasks();
	if (hResult != NO_ERROR) return hResult;

	// avc unit notify
	hResult = avcUnitNotifySuspendTasks();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Create tasks for this module
*/
HRESULT avcUnitInitializeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avc unit reserve
	hResult = avcUnitReserveInitializeTasks();
	if (hResult != NO_ERROR) return hResult;

	// avc unit notify
	hResult = avcUnitNotifyInitializeTasks();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

// Unit initialization
HRESULT avcUnitInitialize(void)
{
	HRESULT		hResult = NO_ERROR;
	
	hResult = crAddCallback(AVC_UNIT_CALLBACK);
	if (hResult != NO_ERROR) return hResult;
	
	// avc unit reserve
	hResult = avcUnitReserveInitialize();
	if (hResult != NO_ERROR) return hResult;

	// avc unit notify
	hResult = avcUnitNotifyInitialize();
	if (hResult != NO_ERROR) return hResult;

	// avc unit power
	hResult = avcUnitPowerInitialize();
	if (hResult != NO_ERROR) return hResult;

	// avc unit vendor dependent
	hResult = avcUnitVendorInitialize();
	if (hResult != NO_ERROR) return hResult;

	// avc unit channel usage
	hResult = avcUnitChannelUsageInitialize();
	if (hResult != NO_ERROR) return hResult;

	// avc unit connect
	hResult = avcUnitConnectInitialize();
	if (hResult != NO_ERROR) return hResult;

	// avc unit input and output plug signal format
	hResult = avcUnitSignalFormatInitialize();
	if (hResult != NO_ERROR) return hResult;

	// avc unit signal source
	hResult = avcUnitSignalSourceInitialize();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}







