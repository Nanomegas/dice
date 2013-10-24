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

	Implementation of avc unit channel usage notify command

  Revisions:
		created 08/04/2003 LM

****************************************************************************/
#ifndef _AVCUNITCHANNELUSAGE_H
#define _AVCUNITCHANNELUSAGE_H

#include "TCTypes.h"
#include "avcUtils.h"
#include "packetblock.h"
#include "listmanager.h"

typedef	struct
{
	BYTE		channel;
	uint16		nodeAddr;
	BYTE		oPCRNumber;
} CHANNEL_USAGE_DATA;

LM_CONTEXT*		avcUnitChannelUsageGetNotifyList(void);

HRESULT			avcUnitChannelUsageStateChanged(uint32 channel);

HRESULT			avcUnitChannelUsageNotifyAdd(PB *packetBlock, BYTE channel, uint16 nodeAddr, BYTE oPCRNumber);
HRESULT			avcUnitChannelUsageNotifyCheck(void);

HRESULT			avcUnitChannelUsageInitialize(void);									// Initialization of this module

#endif //_AVCUNITCHANNELUSAGE_H

