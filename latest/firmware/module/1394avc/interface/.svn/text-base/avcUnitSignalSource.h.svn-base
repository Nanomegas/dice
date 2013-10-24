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

	Implementation of avc connection and compatibility management (CCM) command: 
	signal source notify
	AV/C Connection and Compatibility Management (CCM) Specification (Ver. 1.0 Sec. 7)

  Revisions:
		created 26/05/2003 LM

****************************************************************************/
#ifndef _AVCUNITSIGNALSOURCE_H
#define _AVCUNITSIGNALSOURCE_H

#include "TCTypes.h"
#include "avcUtils.h"
#include "packetblock.h"
#include "listmanager.h"

typedef	struct
{
	BYTE		channel;
	uint16		nodeAddr;
	BYTE		oPCR;
} SIGNAL_SOURCE_DATA;

LM_CONTEXT*		avcUnitSignalSourceGetNotifyList(void);

HRESULT			avcUnitSignalSourceStateChanged(void);

HRESULT			avcUnitSignalSourceNotifyAdd(PB *packetBlock, BYTE channel, uint16 nodeAddr, BYTE oPCR);
HRESULT			avcUnitSignalSourceNotifyCheck(void);

HRESULT			avcUnitSignalSourceInitialize(void);									// Initialization of this module

#endif //_AVCUNITSIGNALSOURCE_H

