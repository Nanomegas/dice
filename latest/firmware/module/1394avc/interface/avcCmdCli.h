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

#ifndef _AVCCMDCLI_H
#define _AVCCMDCLI_H
#ifdef _CLI

#include "cliToolDefs.h"
#include "TCTypes.h"
#include "packetBlock.h"
#include "datastream.h"
#include "avcUtils.h"
#include "avcUnitReserve.h"
#include "lal.h"

HRESULT		avcCmdCliGetTargetModes(char *sTargetNode, uint32 *targetNode, char cTargetUnit, uint32 *targetUnit);
HRESULT		avcCmdCliSetTarget(char *sTargetNode, char cTargetUnit);
HRESULT		avcCmdCliGetTarget(void);
HRESULT		avcCmdCliShowTargets(void);
HRESULT		avcCmdCliGetTargetNodeAddr(uint32 *nodeAddr);
HRESULT		avcCmdCliGetTargetHandle(LAL_NODE_HANDLE *phandle);
HRESULT		avcCmdCliGetTargetUnit(AVC_ADDRESS_SUBUNIT *addrSubunit);

HRESULT		avcCmdCliConstructRequestHeader(pDataStream pStream, uint8 ctype, uint8 opcode, uint16 numBytes, uint32 *data);
HRESULT		avcCmdCliSendRequest(uint8 ctype, uint16 numBytes, uint32 *data, pDataStream pStream);
HRESULT		avcCmdCliGetCType(char cctype, uint8 *ctype);

#ifdef _CLI_TOOL_AVC_CMD
// AV/C General Commands functions
HRESULT		avcCmdCliPower(char cctype, uint8 power_state);
HRESULT		avcCmdCliUnitInfo(char cctype);
HRESULT		avcCmdCliSubunitInfo(char cctype, uint32 page);
HRESULT		avcCmdCliReserve(char cctype, uint8 priority, char *text);
HRESULT		avcCmdCliVersion(char cctype, uint32 version);
HRESULT		avcCmdCliVendor(char cctype);

// AV/C Connection Commands functions
HRESULT		avcCmdCliPlugInfo(char cctype, uint32 subfunction);
HRESULT		avcCmdCliChannelUsage(char cctype, uint32 channel);
HRESULT		avcCmdCliConnect(char cctype, uint32 index);
HRESULT		avcCmdCliConnectAV(char cctype, uint32 index);
HRESULT		avcCmdCliConnections(char cctype);
HRESULT		avcCmdCliDigitalInput(char cctype, uint8 connection_state);
HRESULT		avcCmdCliDigitalOutput(char cctype, uint8 connection_state);
HRESULT		avcCmdCliDisconnect(char cctype, uint32 index);
HRESULT		avcCmdCliDisconnectAV(char cctype, uint32 index);
HRESULT		avcCmdCliInputPlugSignalFormat(char cctype, BYTE plugID, uint32 format);
HRESULT		avcCmdCliOutputPlugSignalFormat(char cctype, BYTE plugID, uint32 format);
HRESULT		avcCmdCliGeneralBusSetup(char cctype, BYTE bus_type);

// AV/C CCM Commands functions
HRESULT		avcCmdCliSignalSource(char cctype, uint32 index);
HRESULT		avcCmdCliInputSelect(char cctype, uint32 nodeID, BYTE subfunction);
HRESULT		avcCmdCliOutputPreset(char cctype, uint32 index);
HRESULT		avcCmdCliCCMProfile(char cctype, uint32 index);

#ifdef _AVC_SECURITY
// AV/C Security commands
HRESULT		avcCmdCliSecurity(char cctype);
#endif //_AVC_SECURITY

// AV/C Check All commands
HRESULT		avcCmdCliCheckAll(char cctype);

void		avcCmdCliInstallTools(void);
#endif //_CLI_TOOL_AVC_CMD
#endif //_CLI
#endif //_AVCCMDCLI_H

