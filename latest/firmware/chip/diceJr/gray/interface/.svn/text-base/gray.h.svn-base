/*===================================================================*
  Copyright (c) 2002 TC Applied Technologies Ltd.
  -------------------------------------------------------------------
  $Header: /usr/local/cvsroot/firmware/chip/diceJr/gray/interface/gray.h,v 1.2 2007/02/08 17:59:33 zluo Exp $
 *===================================================================*/ 

 /*********************************************************************
 * DESCRIPTION:

	This module implements the 
		
	Gray Module for Gray encoder interface
		
	Revisions:
		created 12/11/2004 lm

***********************************************************************/

#ifndef _GRAY_H
#define _GRAY_H

#include "TCTypes.h"

// move to ..ErrorCodes
/**************************** GRAY **************************************/

#define E_GRAY			0xC5000000		    // Gray Errors

/**************************** GRAY **************************************/

#define E_GRAY_EVENT_INITIALIZE			(E_GRAY+1)
#define E_GRAY_EVENT_CB_ERR				(E_GRAY+2)
#define E_GRAY_EVENT_CB_RANGE_ERR		(E_GRAY+3)
#define E_GRAY_EVENT_FULL				(E_GRAY+4)

/**************************** GRAY **************************************/

enum
{
	GRAY_ENC_0 = 0,
	GRAY_ENC_1,
	GRAY_ENC_2,
	GRAY_ENC_3,
	GRAY_ENC_ITEMS
};

enum
{
	GRAY_EVENT_DIRECT_POLLING = 0,				// _GRAYINT_POLLING && !_GRAYINT_DEFERRED
	GRAY_EVENT_DIRECT_INTERRUPT,				// !_GRAYINT_POLLING && !_GRAYINT_DEFERRED
	GRAY_EVENT_DEFERRED_POLLING,				// _GRAYINT_POLLING && _GRAYINT_DEFERRED
	GRAY_EVENT_DEFERRED_INTERRUPT				// !_GRAYINT_POLLING && _GRAYINT_DEFERRED
};

enum
{
	GRAY_ENC_MODE_DIRECT = 0,					// if !_GRAYINT_POLLING then callback is called directly from interrupt
	GRAY_ENC_MODE_DEFERRED,						// if _GRAYINT_POLLING then event is passed from polling thread to deferred thread and then calling callback
#ifdef _GRAYINT_DEFERRED
	GRAY_ENC_MODE_DEFAULT = GRAY_ENC_MODE_DEFERRED	
#else //_GRAYINT_DEFERRED
	GRAY_ENC_MODE_DEFAULT = GRAY_ENC_MODE_DIRECT
#endif //_GRAYINT_DEFERRED
} GRAY_ENC_MODE;

enum
{
	GRAY_ENC_ACC_MODE_NONE = 0,						// no gray encoder accelerators enabled
	GRAY_ENC_ACC_MODE_TYPE1,						// if polling gray encoder accelerators type1 count dependent, if !polling type1 time dependent
	GRAY_ENC_ACC_MODE_TYPE2,						// if polling gray encoder accelerators type2 count dependent, if !polling type2 time dependent
	GRAY_ENC_ACC_MODE_CUSTOM,						// using supplied accCustomCallback
	GRAY_ENC_ACC_MODE_DEFAULT = GRAY_ENC_ACC_MODE_TYPE1
} GRAY_ENC_ACC_MODE;

typedef void (* GRAYEVENT_CALLBACK)(uint8 enc, int8 change);
typedef void (* GRAYACC_CALLBACK)(int8 *change, uint32 timeDiff);

BOOL		grayEventIsIntEnabled(uint32 enc);
uint32		grayEventGetIntCount(uint32 enc);
uint32		grayEventGetIntCounts(void);
void		grayEventClearIntCounts(void);

void		grayEventHandler(void);

HRESULT		grayEventSetMode(uint32 enc, uint32 mode);
HRESULT		grayEventGetMode(uint32 enc, uint32 *mode);
HRESULT		grayEventSetAccMode(uint32 enc, uint32 accMode, GRAYACC_CALLBACK accCustomCallback);
HRESULT		grayEventGetAccMode(uint32 enc, uint32 *accMode);

HRESULT		grayEventInitCallbacks(void);
HRESULT		grayEventAddCallback(uint32 enc, GRAYEVENT_CALLBACK callback);
HRESULT		grayEventGetCallback(uint32 enc, GRAYEVENT_CALLBACK *callback);
HRESULT		grayEventCallCallback(uint32 enc, int8 change);
HRESULT		grayEventRemoveCallback(uint32 enc);

HRESULT		grayEventSetPollEnable(BOOL bEnable);	// specify whether to use polling (if _GRAYINT_POLLING) or interrupt
BOOL		grayEventGetPollEnable(void);

HRESULT		grayEventInitializeTasks(void);
HRESULT		grayEventInitialize(void);

#endif //_GRAY_H

