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

	Implementation of avc unit connect notify command

  Revisions:
		created 08/04/2003 LM

****************************************************************************/
#ifndef _AVCUNITCONNECT_H
#define _AVCUNITCONNECT_H

#include "TCTypes.h"
#include "avcUtils.h"
#include "packetblock.h"
#include "listmanager.h"

typedef	struct
{
	AVC_CONNECTION command;
	AVC_CONNECTION connection;
} CONNECT_DATA;

LM_CONTEXT*		avcUnitConnectGetNotifyList(void);

HRESULT			avcUnitConnectStateChanged(void);

HRESULT			avcUnitConnectNotifyAdd(PB *packetBlock, AVC_CONNECTION *command, AVC_CONNECTION *connection);
HRESULT			avcUnitConnectNotifyCheck(void);

HRESULT			avcUnitConnectInitialize(void);									// Initialization of this module

#endif //_AVCUNITCONNECT_H

