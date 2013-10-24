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

	Implementation of avc panel subunit commands (Responses)

	AV/C Panel Subunit Specification 1.21
	TA Document 2002009
	
	Revisions:
		created 17/09/2003 LM

****************************************************************************/

#ifndef _AVCPANELRSP_H
#define _AVCPANELRSP_H

#include "TCTypes.h"
#include "packetBlock.h"
#include "datastream.h"
#include "avcUtils.h"

HRESULT		avcPanelRspGuiUpdate(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT		avcPanelRspPushGuiData(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT		avcPanelRspUserAction(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT		avcPanelRspPassThrough(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);

#endif //_AVCPANELRSP_H

