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

	All the things you can do to an AVR DRD can be done via this module.
	All the information specific to an AVR DRD can be known via this module.

  Internal logic:
  	
  		This module implements a simple state machine which follows the state of
  		the host drd communication interface. There are 3 states:
			- waiting for drd initialization complete
				In this state, the drd messaging subsystem is trying to initialize
				and synchronize communications with the host.
				While in this state, we simply wait.
				Application attempts to 'set' or 'get' host parameters are rejected.
			- collecting i/o lists
				In this state, drd messaging initialization has completed and this 
				module is retrieving the input list and output list information from the host.
				Application attempts to 'set' or 'get' host parameters are rejected.
			- available for applications
				The host interface is under the control of the application.
				This module sends commands as directed by the application.
				All data received from the host is parsed and stored in the host state table,
				and any associated callbacks are invoked.
	
  Revisions:
		created 01/05/2003 LM

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "packetblock.h"
#include "drd.h"
#include "drdCli.h"
#include "avc.h"
#include "avcDefs.h"
#include "avcUnitPower.h"
#include "cr.h"
#include "cli.h"
#include "datastream.h"
#include "systemcallbacks.h"
#ifdef _SYSDEBUG
#include "sysDebug.h"
#endif //_SYSDEBUG

DRD_HOST_STATE		drdHostState;					// global for ease of access

// task (thread) IDs for this module
uint32					drdHandlerThreadID;		// drd handler thread
const char*				drdHandlerThreadName = "drdHandlerThread";

static CALLBACK_DESCRIPTOR	DRD_POWER_CALLBACK[] =
{
	{CALLBACK_DESCRIPTOR_TYPE, AVC_POWER_TYPE},
	{CALLBACK_DESCRIPTOR_NEXT, 0},
	{CALLBACK_DESCRIPTOR_NAME, (uint32)"DRD_POWER_CALLBACK"},
	{AVC_POWER_ON_CALLBACK_KEY, (uint32) &drdPowerOn},
	{AVC_POWER_OFF_CALLBACK_KEY, (uint32) &drdPowerOff},
	{AVC_POWER_STATUS_CALLBACK_KEY, (uint32) &drdPowerStatus},
	{KV_TERMINATEKEY, KV_TERMINATEVALUE}
};

HRESULT drdPowerInit(void) 
{
	return crAddCallback(DRD_POWER_CALLBACK);
}	

HRESULT drdPowerStatus(void)
{
	DRD_HOST_POWER_STATE	state = HOST_POWER_STANDBY;
	TC_POWERSTATE			unitpowerstate = TC_POWERSTATE_UNKNOWN;

	drdGetPowerState(&state);

	unitpowerstate = (state == HOST_POWER_ON) ? TC_POWERSTATE_ON : TC_POWERSTATE_OFF;

	return ((HRESULT) unitpowerstate);
}

HRESULT drdPowerOn(void)
{
	return drdSetPowerState(HOST_POWER_ON);
}

HRESULT drdPowerOff(void)
{
	return drdSetPowerState(HOST_POWER_STANDBY);
}

HRESULT drdGetPowerState(DRD_HOST_POWER_STATE *state)
{
#if 1 //LM???
	HRESULT		hResult = NO_ERROR;	

	*state = drdHostState.hostPowerState;

#if 0 //def _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_RDR))
	{
		drdCliDisplayPowerState(drdHostState.hostPowerState);
	}
#endif //_SYSDEBUG

	return hResult;
#else
	return sendHostDataRequest(TC_DRD_POWER_STATE_MSG);
#endif
}

HRESULT drdSetPowerState(DRD_HOST_POWER_STATE state)
{
	HRESULT					hResult = NO_ERROR;	
	DRD_HOST_POWER_STATE	oldstate = drdHostState.hostPowerState;

	if (state == HOST_POWER_ON)
	{
#if 1 //LM???
		drdHostState.hostPowerState = state;
#else
		hResult = drdSendHostMessage(TC_DRD_POWER_STATE_MSG, "ON", TC_NO_WAIT) ;
#endif
	}
	else if (state == HOST_POWER_STANDBY)
	{
#if 1 //LM???
		drdHostState.hostPowerState = state;
#else
		hResult = drdSendHostMessage(TC_DRD_POWER_STATE_MSG, "SB", TC_NO_WAIT) ;
#endif
	}
	else
	{
		hResult = E_DRD_BAD_SET_PARAM;
	}

#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_DRD))
	{
		drdCliDisplayPowerState(state);
	}
#endif //_SYSDEBUG

	if (oldstate != state)
	{
		avcUnitPowerStateChanged();
	}

	return hResult;
}

void drdHandlerThread(void *dummy)
{
//	HRESULT				hResult = NO_ERROR;	

	UNUSED_ARG(dummy);

	DO_FOREVER
	{
		TCTaskWait(10000);
	}
}

/*********************************************************
	Create semaphores for this module
*/
HRESULT drdCreateSemaphores(void)
{
	HRESULT		hResult = NO_ERROR;

#if 0 //LM??? not used
	hResult = TCMutexOpen(&drdHandlerSemID);
	if (hResult != NO_ERROR) return hResult;
#endif

	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT drdResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// drd handler thread
	hResult = TCTaskResume(drdHandlerThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT drdSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// drd handler thread
	hResult = TCTaskSuspend(drdHandlerThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Create tasks for this module
*/
HRESULT drdInitializeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// drd handler thread
	hResult = TCTaskCreate(&drdHandlerThreadID, drdHandlerThreadName, drdHandlerThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Initialization of this module
*/
HRESULT drdInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = drdCreateSemaphores();
	if (hResult != NO_ERROR) return hResult;

	hResult = drdResumeTasks();
	if (hResult != NO_ERROR) return hResult;

	hResult = drdPowerInit();
	if (hResult != NO_ERROR) return hResult;
	
#ifdef _CLI_TOOL_DRD
	drdCliInstallTools();
#endif //_CLI_TOOL_DRD

	return hResult;
}
