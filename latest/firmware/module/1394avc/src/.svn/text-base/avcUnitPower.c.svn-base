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
	this module handles notification of the AV/C unit power status

	power notification is done by sending the unit a command,
	instead of sending the command to each subunit

  Revisions:
		created 08/04/2003 LM

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "avc.h"
#include "avcUnitCli.h"
#include "avcUnitPower.h"
#include "avcUnitNotify.h"
#include "cr.h"
#include "listmanager.h"
#include "systemcallbacks.h"

// semaphore IDs for this module
uint32					avcUnitPowerSemID;				// mutual exclusion semaphore for access to the power state

static	BOOL			avcUnitPowerInitialized = FALSE;

#define POWER_MAX_NOTIFY_ITEMS		12

static BYTE				powerNotifyList_BUFFER[LM_LISTSIZE(POWER_MAX_NOTIFY_ITEMS, sizeof(POWER_NOTIFY))];		// Used to keep track of thread control blocks for those
static LM_CONTEXT*		powerNotifyList = (LM_CONTEXT *) powerNotifyList_BUFFER;

LM_CONTEXT*	avcUnitPowerGetNotifyList(void)
{
	return powerNotifyList;
}

// turn the power on - calls all callbacks which are registered for receiving power on notifications
HRESULT  avcUnitPowerOn() 
{
	HRESULT			hResult = NO_ERROR;
	TC_POWERSTATE	avc_powerstate;

	avcUnitPowerHandle(AVC_POWER_ON, &avc_powerstate);

	if (avc_powerstate == TC_POWERSTATE_ON)
	{	
		hResult = E_PKT_AVC_ACCEPTED;
	}
	else
	{
		hResult = E_PKT_AVC_NOT_IMPLEMENTED;	
	}

	return hResult;
}

// turn the power off - calls all callbacks which are registered for receiving power on notifications
HRESULT  avcUnitPowerOff() 
{
	HRESULT			hResult = NO_ERROR;
	TC_POWERSTATE	avc_powerstate;
	
	avcUnitPowerHandle(AVC_POWER_OFF, &avc_powerstate);

	if (avc_powerstate == TC_POWERSTATE_OFF)
	{	
		hResult = E_PKT_AVC_ACCEPTED;
	}
	else
	{
		hResult = E_PKT_AVC_NOT_IMPLEMENTED;	
	}

	return hResult;
}

//
//	Determine the state of the Power by calling this function.
//
//	Looks for all powerstate functions, if any of the callbacks
//  return with a POWER_ON mode, then this responds with a power_on response.
//  if all callbacks report power_off then the power is response is power_off.
//
TC_POWERSTATE avcUnitPowerState()
{
	TC_POWERSTATE	avc_powerstate = TC_POWERSTATE_UNKNOWN;
	
	avcUnitPowerHandle(AVC_POWER_STATUS, &avc_powerstate);

	return avc_powerstate;
}

HRESULT avcUnitPowerHandle(uint32 power_state, TC_POWERSTATE *avc_powerstate) 
{
	HRESULT	 				hResult = NO_ERROR;
	HRESULT	 				hResultCB = NO_ERROR;
	uint32	 				index = 0;
	CALLBACK_DESCRIPTOR*	cb = NULL;
	AVC_POWER_CALLBACK		callback = NULL;
	BOOL					bFound = FALSE;
	uint32					key = 0;

	switch (power_state)
	{
		// power operand values
		case AVC_POWER_ON:
			*avc_powerstate = TC_POWERSTATE_ON;
			key = AVC_POWER_ON_CALLBACK_KEY; 
			break;
		case AVC_POWER_OFF:
			*avc_powerstate = TC_POWERSTATE_OFF;
			key = AVC_POWER_OFF_CALLBACK_KEY;
			break;
		case AVC_POWER_STATUS:
			*avc_powerstate = TC_POWERSTATE_UNKNOWN;
			key = AVC_POWER_STATUS_CALLBACK_KEY;
			break;
		default:
			*avc_powerstate = TC_POWERSTATE_UNKNOWN;
			hResult = E_PKT_AVC_NOT_IMPLEMENTED;
			return hResult;
	}

	// exclusive access for access to the power state (mutex)
	hResult = TCMutexLock(avcUnitPowerSemID);
	if (hResult != NO_ERROR) return hResult;
				
	DO_FOREVER
	{
		hResult = crGetCallback(AVC_POWER_TYPE, index++, &cb);
		if (hResult != NO_ERROR) break;
		
		hResult = kvGetValue(cb, key, (uint32 *) &callback);
		if (hResult != NO_ERROR) continue;

		hResultCB = (*callback)();

		if (power_state == AVC_POWER_STATUS)
		{			
			if (*avc_powerstate == TC_POWERSTATE_UNKNOWN ||
				hResultCB == TC_POWERSTATE_ON)
			{
				*avc_powerstate = (TC_POWERSTATE) hResultCB;
			}
		}	
		bFound = TRUE;	
	}	

	// exclusive access for access to the power state (mutex)
	TCMutexUnlock(avcUnitPowerSemID);

	if (bFound == FALSE)
	{
		*avc_powerstate = TC_POWERSTATE_UNKNOWN;
	}
	
	return hResult;
}

HRESULT avcUnitPowerStateChanged(void)
{
	return avcUnitNotifyChange(avcUnitPowerNotifyCheck);
}

HRESULT avcUnitPowerNotifyAdd(PB *packetBlock, TC_POWERSTATE unitpowerstate)
{
	HRESULT				hResult = NO_ERROR;
	POWER_NOTIFY		notify;
				
	memset(&notify, 0, sizeof(notify));
	notify.data.state = unitpowerstate;

	hResult = avcUnitNotifyAdd(powerNotifyList, (UNION_NOTIFY *) &notify, packetBlock, TRUE);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}
				
static HRESULT avcUnitPowerNotifyCheckCB(UNION_NOTIFY *notify, BOOL *bChanged)
{
	HRESULT				hResult = NO_ERROR;
	TC_POWERSTATE		unitpowerstate = TC_POWERSTATE_UNKNOWN;
	POWER_NOTIFY*		powerNotify = (POWER_NOTIFY *) notify;

	unitpowerstate = avcUnitPowerState();
	
	*bChanged = (powerNotify->data.state != unitpowerstate);

	return hResult;
}

static HRESULT avcUnitPowerNotifyUpdateCB(UNION_NOTIFY *notify, pDataStream pStream)
{
	HRESULT				hResult = NO_ERROR;
	TC_POWERSTATE		unitpowerstate = TC_POWERSTATE_UNKNOWN;
	BYTE				power_state = 0;

	UNUSED_ARG(notify);

	unitpowerstate = avcUnitPowerState();
	power_state = (BYTE) ((unitpowerstate == TC_POWERSTATE_ON) ? AVC_POWER_ON : AVC_POWER_OFF);

	hResult = dsWrite8Bits(pStream, power_state);				// operand[0]
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcUnitPowerNotifyCheck(void)
{
	HRESULT		hResult = NO_ERROR;

	if (avcUnitPowerInitialized == TRUE)
	{
		if (powerNotifyList->allocated > 0) 
		{
			hResult = avcUnitNotifyCheck(powerNotifyList, avcUnitPowerNotifyCheckCB, avcUnitPowerNotifyUpdateCB);
		}
	}

	return hResult;
}

/*********************************************************
	Create semaphores for this module
*/
HRESULT avcUnitPowerCreateSemaphores(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = TCMutexOpen(&avcUnitPowerSemID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Initialization of this module
*/
HRESULT avcUnitPowerInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = avcUnitPowerCreateSemaphores();
	if (hResult != NO_ERROR) return hResult;
		
	hResult = lmCreateList(powerNotifyList, LM_LISTSIZE(POWER_MAX_NOTIFY_ITEMS, sizeof(POWER_NOTIFY)), sizeof(POWER_NOTIFY),NULL);
	if (hResult != NO_ERROR) return hResult;
	
	avcUnitPowerInitialized = TRUE;
	
	return hResult;
}
