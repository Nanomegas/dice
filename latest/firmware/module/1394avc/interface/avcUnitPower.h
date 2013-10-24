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

	Implementation of avc unit POWER command
	this module handles control and status of the AV/C unit power status

  Revisions:
		created 08/04/2003 LM

****************************************************************************/
#ifndef _AVCUNITPOWER_H
#define _AVCUNITPOWER_H

#include "TCTypes.h"
#include "avcUtils.h"
#include "avcUnitPower.h"
#include "packetblock.h"
#include "listmanager.h"

typedef enum
{
	TC_POWERSTATE_ON = 0,
	TC_POWERSTATE_OFF,
	TC_POWERSTATE_UNKNOWN
} TC_POWERSTATE;

typedef TC_POWERSTATE	(* AVCUNIT_POWERQUERY) (void);
typedef void			(* AVCUNIT_POWERSET) (TC_POWERSTATE newstate);

typedef	struct
{
	TC_POWERSTATE	state;
} POWER_DATA;

//	Power commands

LM_CONTEXT*		avcUnitPowerGetNotifyList(void);

HRESULT			avcUnitPowerCommandRegister(AVCUNIT_POWERQUERY query, AVCUNIT_POWERSET set);

HRESULT			avcUnitPowerOn(void);
HRESULT			avcUnitPowerOff(void);
TC_POWERSTATE 	avcUnitPowerState(void);
HRESULT			avcUnitPowerHandle(uint32 power_state, TC_POWERSTATE *avc_powerstate);
HRESULT			avcUnitPowerStateChanged(void);

HRESULT			avcUnitPowerNotifyAdd(PB *packetBlock, TC_POWERSTATE unitpowerstate);
HRESULT			avcUnitPowerNotifyCheck(void);

HRESULT			avcUnitPowerCreateSemaphores(void);								
HRESULT			avcUnitPowerInitialize(void);									// Initialization of this module

#endif //_AVCUNITPOWER_H

