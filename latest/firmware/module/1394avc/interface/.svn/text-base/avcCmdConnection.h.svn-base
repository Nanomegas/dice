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

	Implementation of avc connection commands
	AV/C Digital Interface Command Set General (Ver. 4.1) section 12

  Revisions:
		created 08/04/2003 LM

****************************************************************************/

#ifndef _AVCCMDCONNECTION_H
#define _AVCCMDCONNECTION_H

#include "TCTypes.h"
#include "packetBlock.h"
#include "datastream.h"
#include "avcUtils.h"

#ifdef _WINMP
#pragma warning( disable : 4514 )
#endif //_WINMP

HRESULT				avcCmdPlugInfo(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT				avcCmdChannelUsage(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT				avcCmdConnect(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT				avcCmdConnectAV(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT				avcCmdConnections(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT				avcCmdDigitalInput(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT				avcCmdDigitalOutput(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT				avcCmdDisconnect(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT				avcCmdDisconnectAV(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT				avcCmdSignalFormat(BOOL bIn, AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT				avcCmdGeneralBusSetup(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);

_TC_INLINE HRESULT	avcCmdInputPlugSignalFormat(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	avcCmdInputPlugSignalFormat(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
								{
									return avcCmdSignalFormat(TRUE, pHeader, packetBlock, pStream);
								}

_TC_INLINE HRESULT	avcCmdOutputPlugSignalFormat(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	avcCmdOutputPlugSignalFormat(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
								{
									return avcCmdSignalFormat(FALSE, pHeader, packetBlock, pStream);
								}

#endif //_AVCCMDCONNECTION_H

