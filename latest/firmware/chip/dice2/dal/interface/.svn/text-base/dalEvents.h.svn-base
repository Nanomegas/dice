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
//===========================================================================
/****************************************************************************
	
	Purpose:	Dice Audio Application Layer (DAL)- Event call back system
  
  Description:       
  
	Revisions:
		created 09/03/2005 ML

****************************************************************************/

#ifndef _DALEVENTS_H
#define _DALEVENTS_H

#include "dal.h"



typedef struct tDAL_EVENT_CALLBACK
{
	DAL_CALLBACK_FUNC	func;
	uint16				holdoff_ticks;
	uint16				count_ticks;
	uint32				data;
	DAL_EVENTS			events;      //accumulated specific events since last CB
	uint32				event_mask;  //accumulated global events since last CB
	DAL_EVENTS			events_enable;
} DAL_EVENT_CALLBACK;

typedef struct tDAL_EVENT_INTERFACE
{
	DAL_CB_EVENT_MASK	masterMask;
	DAL_EVENT_CALLBACK	callbacks[DAL_CB_MAX_NUM];
} DAL_EVENT_INTERFACE;


uint32 dalEventsHandleCallback(DAL_EVENT_INTERFACE * pEventInterface, DAL_EVENTS * pEvents);

HRESULT dalEventsInstallCallback  (DAL_EVENT_INTERFACE * pEventInterface, const DAL_EVENTS * pEventMask, uint32 holdoff_ms, DAL_CALLBACK_FUNC func, uint32 data);
HRESULT dalEventsUninstallCallback(DAL_EVENT_INTERFACE * pEventInterface, DAL_CALLBACK_FUNC func);


#endif // _DALEVENTS_H

