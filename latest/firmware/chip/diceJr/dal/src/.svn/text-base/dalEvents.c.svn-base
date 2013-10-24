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
	
	Purpose:	Dice Audio Application Layer (DAL)- Event call back system
  
  Description:       
  
	Revisions:
		created 09/03/2005 ML

****************************************************************************/

#include <string.h>
#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCDebug.h"
#include "dal.h"
#include "dalEvents.h"
#include "dalClkCtrl.h"
#include "dalRates.h"
#include "dalRouting.h"
#include "dalAES.h"
#include "dalADAT.h"
#include "dalAVS.h"


static char 	moduleName[] = __FILE__ ;


static uint32 _dalGetMasterEvents (const DAL_EVENTS * pEvents)
{
	uint32 events = 0;
	
	if (pEvents->fsm_events & DAL_CB_DOM1_MASK) events |= DAL_CB_DOMAIN1;
	if (pEvents->aes_events & DAL_CB_AES0_MASK) events |=DAL_CB_AES0;
	if (pEvents->aes_events & DAL_CB_AES1_MASK) events |=DAL_CB_AES1;
	if (pEvents->aes_events & DAL_CB_AES2_MASK) events |=DAL_CB_AES2;
	if (pEvents->aes_events & DAL_CB_AES3_MASK) events |=DAL_CB_AES3;
	if (pEvents->adat_events& DAL_CB_ADAT_MASK) events |= DAL_CB_ADAT;
	if (pEvents->adat_events& DAL_CB_ADAT_AUX_MASK) events |=DAL_CB_ADAT_AUX;
	if (pEvents->avs_events & DAL_CB_AVS1_MASK) events |=DAL_CB_AVS1;
	if (pEvents->avs_events & DAL_CB_AVS2_MASK) events |=DAL_CB_AVS2;
	
	return events;
}
	

// Here do the 'dal' side of callback mechanism
// This function is called on every tick from clkCtrlStateMachine()
// fsm_new_event holds the events stemming from the audio subsystem state machines (ie. AES, ADAT, etc.)
// new_events holds events coming from clkCtrlStateMachine(clkId) 
// for both - they are only from this tick!
uint32 dalEventsHandleCallback(DAL_EVENT_INTERFACE * pEventInterface, DAL_EVENTS * pEvents)
{
	uint32 cbResult = 0;
	uint8 icb;
	DAL_EVENT_CALLBACK * pcb;
	uint32 events;
	uint8  bCall;
	
	//create the event summary
	events = _dalGetMasterEvents (pEvents);
	
	for (icb = 0; icb < DAL_CB_MAX_NUM; icb++) 
	{
		bCall = 0;
		pcb = &pEventInterface->callbacks[icb];
		
		if (pcb->func) {
			// Note - *all* events will be stored and passed to the func, and not only the ones it 'wants'
			pcb->event_mask |= events;
			
			pcb->events.fsm_events |= pEvents->fsm_events;
			if (pcb->events.fsm_events & pcb->events_enable.fsm_events) bCall = 1;
			
			pcb->events.aes_events |= pEvents->aes_events;			
			if (pcb->events.aes_events & pcb->events_enable.aes_events) bCall = 1;
			
			pcb->events.adat_events |= pEvents->adat_events;
			if (pcb->events.adat_events & pcb->events_enable.adat_events) bCall = 1;
						
			pcb->events.avs_events |= pEvents->avs_events;
			if (pcb->events.avs_events & pcb->events_enable.avs_events) bCall = 1;
						
			
			if (pcb->holdoff_ticks && pcb->count_ticks) pcb->count_ticks--;
			
			if (bCall) 
			{
				if (!pcb->holdoff_ticks || !pcb->count_ticks) 
				{					
					pcb->func(pcb->event_mask, &pcb->events, pcb->data);
					pcb->event_mask = 0;
					memset (&pcb->events, 0, sizeof(DAL_EVENTS));
					pcb->count_ticks = pcb->holdoff_ticks;
				}
			}
		}
	}

	return cbResult;
}



#define MS_PER_TICK 20
HRESULT dalEventsInstallCallback  (DAL_EVENT_INTERFACE * pEventInterface, const DAL_EVENTS * pEventMask, uint32 holdoff_ms, DAL_CALLBACK_FUNC func, uint32 data)
{
	HRESULT hResult = NO_ERROR;
	uint8 icb;

	if (func) 
	{
		icb = 0;
		while (icb < DAL_CB_MAX_NUM) 
		{
			if (!pEventInterface->callbacks[icb].func) 
			{
				// this entry is not used so use it				
				pEventInterface->masterMask |= _dalGetMasterEvents (pEventMask);
				pEventInterface->callbacks[icb].func = func;
				pEventInterface->callbacks[icb].events_enable = *pEventMask;
				pEventInterface->callbacks[icb].data = data;
				pEventInterface->callbacks[icb].holdoff_ticks = holdoff_ms/MS_PER_TICK;
				pEventInterface->callbacks[icb].count_ticks = 0;
				pEventInterface->callbacks[icb].event_mask = 0;				
				break;
			}
			icb++;
		}
		if (icb >= DAL_CB_MAX_NUM) 
		{
			hResult = E_DAL_TOO_MANY_CALLBACKS;
			sysLogError(hResult, __LINE__, moduleName);
		}
	}
	else
	{
		hResult = E_DAL_INVALID_CALLBACK;
		sysLogError(hResult, __LINE__, moduleName);
	}

	return hResult;
}

HRESULT dalEventsUninstallCallback(DAL_EVENT_INTERFACE * pEventInterface, DAL_CALLBACK_FUNC func)
{
	HRESULT hResult = NO_ERROR;
	uint8 icb = 0, found = 0;

	if (func) 
	{
		// Reset masterMask - it will be re-built as we run thru callbacks[]
		pEventInterface->masterMask = 0;
		while (icb < DAL_CB_MAX_NUM) 
		{
			if (func == pEventInterface->callbacks[icb].func) 
			{
				memset (&pEventInterface->callbacks[icb], 0, sizeof(DAL_EVENT_CALLBACK));
				pEventInterface->callbacks[icb].func = NULL;
				found++;
			}
			// re-create masterMask
			pEventInterface->masterMask |= _dalGetMasterEvents (&pEventInterface->callbacks[icb].events);
			icb++;
		}
	}
	if (!found) 
	{
		hResult = E_DAL_INVALID_CALLBACK;
		sysLogError(hResult, __LINE__, moduleName);
	} 

	return hResult;
}
