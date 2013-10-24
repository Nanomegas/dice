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
	
	Purpose:	Samsung LLC interrupt handler
	
	Revisions:
		created 12/17/2002 kbj


****************************************************************************/
#ifndef _LLCINT_H
#define _LLCINT_H

#include "TCTypes.h"

enum {
	LLC_INT_STATE_INITIALIZE		= 0,
	LLC_INT_STATE_WAIT_BUSRESET,
	LLC_INT_STATE_WAIT_SI_COMPLETE,
	LLC_INT_STATE_PROCESS_SI,
	LLC_INT_STATE_PROCESS_NORMAL
} LLC_INT_STATE;

typedef void (* INTEVENT_CALLBACK)(void);

BOOL		llcIntEventIsIntEnabled(uint32 index);
const char*	llcIntEventGetIntName(uint32 index);
uint32		llcIntEventGetIntCount(uint32 index);
uint32		llcIntEventGetIntCounts(void);
void		llcIntEventClearIntCounts(void);

HRESULT		llcIntEventInitCallbacks(void);
HRESULT		llcIntEventAddCallback(uint32 idx, INTEVENT_CALLBACK callback);
HRESULT		llcIntEventGetCallback(uint32 idx, INTEVENT_CALLBACK *callback);
HRESULT		llcIntEventRemoveCallback(uint32 idx);


#define HANDLE_BY_ISR (LLC_INT_BIT(LLCID_ASY_RX_PKT_AVL) | LLC_INT_BIT(LLCID_ASY_TX_CMPL))

#define LLC_INT_ISR_RX	  0
#define LLC_INT_ISR_TX	  1
#define LLC_INT_ISR_COUNT 2

typedef BOOL (* INTEVENT_ISR_CALLBACK)(void);
HRESULT		llcIntEventAddIsr(uint32 idx, INTEVENT_ISR_CALLBACK callback);

HRESULT 	llcIntEventClearBusReset(uint32 state);
HRESULT 	llcIntEventNewBusReset(BOOL bDuringCriticalBR);
HRESULT		llcIntEventDisableMask(void);



void			llcIntEventHandler(void);
uint32		llcIntEventGetState(void);
HRESULT		llcIntEventSetState(uint32 state);

//Hack to notify Tx code of changes of br status:
#pragma long_calls_off

#pragma long_calls
void lhlBrInOutHandler (BOOL bIn) __attribute__((section (".sram.llccode")));
#pragma long_calls_off


HRESULT		llcIntResumeTasks(void);
HRESULT		llcIntSuspendTasks(void);
HRESULT		llcIntInitializeTasks(void);
HRESULT		llcIntInitialize(void);

#endif
