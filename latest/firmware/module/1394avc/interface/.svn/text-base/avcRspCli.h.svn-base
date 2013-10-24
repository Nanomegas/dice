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

	Implementation of CLI for the avc unit response 

  Revisions:
		created 08/04/2003 LM

****************************************************************************/

#ifndef _AVCRSPCLI_H
#define _AVCRSPCLI_H

#include "TCTypes.h"
#include "packetBlock.h"
#include "datastream.h"
#include "avcUtils.h"
#include "systemCallbacks.h"

#ifdef _CLI

// AV/C General Commands functions
HRESULT			avcRspCliPower(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT			avcRspCliUnitInfo(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT			avcRspCliSubunitInfo(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT			avcRspCliReserve(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT			avcRspCliVersion(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT			avcRspCliVendor(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);

// AV/C Connection Commands functions
HRESULT			avcRspCliPlugInfo(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT			avcRspCliChannelUsage(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT			avcRspCliConnect(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT			avcRspCliConnectAV(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT			avcRspCliConnections(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT			avcRspCliDigitalInput(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT			avcRspCliDigitalOutput(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT			avcRspCliDisconnect(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT			avcRspCliDisconnectAV(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT			avcRspCliInputPlugSignalFormat(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT			avcRspCliOutputPlugSignalFormat(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT			avcRspCliGeneralBusSetup(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);

// AV/C CCM Commands functions
HRESULT			avcRspCliSignalSource(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT			avcRspCliInputSelect(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT			avcRspCliOutputPreset(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT			avcRspCliCCMProfile(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);

#endif	//_CLI

#endif //_AVCRSPCLI_H

