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

	Implementation of avc connection and compatibility management (CCM) commands
	AV/C Connection and Compatibility Management Specification (Ver. 1.0) section 7

  Revisions:
		created 26/05/2003 LM

****************************************************************************/

#ifndef _AVCCMDCCM_H
#define _AVCCMDCCM_H

#include "TCTypes.h"
#include "packetBlock.h"
#include "datastream.h"
#include "avcUtils.h"

HRESULT		avcCmdSignalSource(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT		avcCmdInputSelect(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT		avcCmdOutputPreset(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT		avcCmdCCMProfile(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);

#endif //_AVCCMDCCM_H

