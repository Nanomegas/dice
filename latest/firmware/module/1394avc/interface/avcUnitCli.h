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

#ifndef _AVCUNITCLI_H
#define _AVCUNITCLI_H

#ifdef _CLI

#include "cliToolDefs.h"

#include "TCTypes.h"
#include "packetBlock.h"
#include "datastream.h"
#include "avcUtils.h"
#include "listmanager.h"
#include "avcUnitVendorDefs.h"

enum
{
	AVC_UNIT_PRINT_RCV_RAW							= 0x00000100,
	AVC_UNIT_PRINT_RCV_COMMAND_ALL					= 0x0000000F,
	AVC_UNIT_PRINT_RCV_RESPONSE_IMPLEMENTED			= 0x00000010,
	AVC_UNIT_PRINT_RCV_RESPONSE_NOT_IMPLEMENTED		= 0x00000020,
	AVC_UNIT_PRINT_RCV_RESPONSE_ALL					= 0x000000F0,
	
	AVC_UNIT_PRINT_SND_RAW							= 0x01000000,
	AVC_UNIT_PRINT_SND_COMMAND_ALL					= 0x000F0000,
	AVC_UNIT_PRINT_SND_RESPONSE_IMPLEMENTED			= 0x00100000,
	AVC_UNIT_PRINT_SND_RESPONSE_NOT_IMPLEMENTED		= 0x00200000,
	AVC_UNIT_PRINT_SND_RESPONSE_ALL					= 0x00F00000,
	AVC_UNIT_PRINT_ITEMS							= 10,
};

BOOL		avcUnitCliIsPrintMode(uint32 mode);
uint32		avcUnitCliGetPrintMode(void);
HRESULT		avcUnitCliSetPrintMode(uint32 mode);
HRESULT		avcUnitCliDisplayPrintMode(void);
HRESULT		avcUnitCliShowPrintModes(void);

void		avcUnitIncrementReceivedCommands(void);
uint32		avcUnitGetReceivedCommands(void);
void		avcUnitIncrementReceivedResponses(void);
uint32		avcUnitGetReceivedResponses(void);

// AV/C Frame functions
void		avcUnitCliPrintCtype(uint32 ctype);
void		avcUnitCliPrintSubunitType(SUBUNIT_TYPE subunit_type);
void		avcUnitCliPrintSubunitID(SUBUNIT_ID subunit_ID);
void		avcUnitCliPrintSubunit(AVC_ADDRESS_SUBUNIT *subunit);
void		avcUnitCliPrintUnitPlugID(BYTE plugID, BOOL bInput);
void		avcUnitCliPrintSubunitPlugID(BYTE plugID, BOOL bSource);
void		avcUnitCliPrintPlug(AVC_ADDRESS_PLUG *plug, BOOL bInputOrSourcePlug);
void		avcUnitCliPrintOpcode(uint32 opcode);
void		avcUnitCliPrintHeader(AVC_HEADER *pHeader);
HRESULT		avcUnitCliPrintOperands(AVC_HEADER *pHeader, pDataStream pStream);
HRESULT		avcUnitCliPrintFrame(AVC_HEADER *pHeader, pDataStream pStream);
HRESULT		avcUnitCliPrintDataStream(pDataStream pStream);
HRESULT		avcUnitCliPrintPacketBlock(PB *packetBlock);
HRESULT		avcUnitCliPrintPacketBlockRawData(uint32 ctype, PB *packetBlock);
HRESULT		avcUnitCliPrintRawData(uint32 ctype, uint32 numBytes, uint32 *data);

// AV/C General Commands functions
void		avcUnitCliPrintPowerState(uint32 power_state);
void		avcUnitCliPrintCompanyID(uint32 company_ID);

void		avcUnitCliPrintReservePriority(uint32 priority);
void		avcUnitCliPrintReserveText(char *text_data); // not null-terminated
void		avcUnitCliPrintReserve(uint32 priority, char *text);

void		avcUnitCliPrintVersionInfo(BYTE subfunction, BYTE version_information, BYTE *su_dependent_field);
void		avcUnitCliPrintProgramType(uint32 programType);
void		avcUnitCliPrintVendorFirmwareInfo(TC_PROGRAM_ATTRIBUTES *programAttributes);

// AV/C Connection Commands functions
HRESULT		avcUnitCliPrintPlugInfo(AVC_ADDRESS_SUBUNIT *addrSubunit, BYTE subfunction, BYTE *plugs);
HRESULT		avcUnitCliPrintChannelUsage(BYTE channel, uint16 nodeAddr, BYTE oPCRNumber);
void		avcUnitCliPrintConnect(AVC_CONNECTION *connection);
void		avcUnitCliPrintConnectAV(uint32 dummy);
void		avcUnitCliPrintConnections(uint32 total_connections, AVC_CONNECTION *connectionArray);
void		avcUnitCliPrintConnectionState(uint32 connection_state);
void		avcUnitCliPrintSignalFormat(BOOL bIn, BYTE plugID, uint32 format);

// AV/C CCM Commands functions
void		avcUnitCliPrintSignalSource(BYTE status, AVC_PLUG_CONNECTION *plugs);
void		avcUnitCliPrintInputSelect(BYTE status, uint16 nodeID, BYTE oPlugID, BYTE iPlugID, AVC_ADDRESS_PLUG *signalDst);
void		avcUnitCliPrintOutputPreset(void);
void		avcUnitCliPrintCCMProfile(void);

#ifdef _AVC_SECURITY
// AV/C Security Commands functions
void		avcUnitCliPrintSecurity(uint32 category);
#endif //_AVC_SECURITY

#ifdef _CLI_TOOL_AVC_UNIT
// List manager debug functions
void		avcUnitCliPrintPowerNotify(void *data);
void		avcUnitCliPrintReserveNotify(void *data);
void		avcUnitCliPrintChannelUsageNotify(void *data);
void		avcUnitCliPrintConnectNotify(void *data);
void		avcUnitCliPrintConnectAVNotify(void *data);
void		avcUnitCliPrintSignalFormatNotify(void *data);
void		avcUnitCliPrintSignalSourceNotify(void *data);

HRESULT		avcUnitCliGetList(char clist, LM_CONTEXT **list, LM_PRINT_LIST_ELEM_CB *callback);
HRESULT		avcUnitCliList(char clist);

void		avcUnitCliInstallTools(void);

#endif //_CLI_TOOL_AVC_UNIT
#endif	//_CLI
#endif //_AVCUNITCLI_H

