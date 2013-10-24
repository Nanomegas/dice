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

	Implementation of avc unit ConnectAV notify command

  Revisions:
		created 08/04/2003 LM

****************************************************************************/
#ifndef _AVCUNITCONNECTAV_H
#define _AVCUNITCONNECTAV_H

#include "TCTypes.h"
#include "avcUtils.h"
#include "packetblock.h"
#include "listmanager.h"

typedef	struct
{
	uint32 dummy;
} CONNECT_AV_DATA;

LM_CONTEXT*		avcUnitConnectAVGetNotifyList(void);

HRESULT			avcUnitConnectAVStateChanged(void);

HRESULT			avcUnitConnectAVNotifyAdd(PB *packetBlock, uint32 dummy);
HRESULT			avcUnitConnectAVNotifyCheck(void);

HRESULT			avcUnitConnectAVInitialize(void);									// Initialization of this module

#endif //_AVCUNITCONNECTAV_H

