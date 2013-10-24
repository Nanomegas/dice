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

	Implementation of avc unit input and output plug signal format notify command

  Revisions:
		created 08/04/2003 LM

****************************************************************************/
#ifndef _AVCUNITSIGNALFORMAT_H
#define _AVCUNITSIGNALFORMAT_H

#include "TCTypes.h"
#include "avcUtils.h"
#include "packetblock.h"
#include "listmanager.h"

typedef	struct
{
	BOOL	bIn;
	BYTE	plugID;
	uint32	format;
} SIGNAL_FORMAT_DATA;					// INPUT and OUTPUT PLUG SIGNAL FORMAT

LM_CONTEXT*		avcUnitSignalFormatGetNotifyList(BOOL bIn);

HRESULT			avcUnitInputPlugSignalFormatStateChanged(void);
HRESULT			avcUnitOutputPlugSignalFormatStateChanged(void);

HRESULT			avcUnitSignalFormatNotifyAdd(PB *packetBlock, BOOL bIn, BYTE plugID, uint32 format);

HRESULT			avcUnitSignalFormatNotifyCheck(BOOL bIn);
HRESULT			avcUnitInputPlugSignalFormatNotifyCheck(void);
HRESULT			avcUnitOutputPlugSignalFormatNotifyCheck(void);

HRESULT			avcUnitSignalFormatInitialize(void);									// Initialization of this module

#endif //_AVCUNITSIGNALFORMAT_H

