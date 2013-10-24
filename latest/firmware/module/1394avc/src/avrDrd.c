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
  		the host drd communication interface.  There are 3 states:
			- waiting for drd initialization complete
				In this state, the drd messaging subsystem is trying to initialize
				and synchronize communications with the host.  While in this state,
				we simply wait.  Application attempts to 'set' or 'get' host parameters
				are rejected.
			- collecting i/o lists
				In this state, drd messaging initialization has completed and this 
				module is retrieving the input list and output list information from the
				host.    Application attempts to 'set' or 'get' host parameters
				are rejected.
			- available for applications
				The host interface is under the control of the avr application.
				This module sends commands as directed by the application.
				All data received from the host is parsed and stored in the host
  				state table, and any associated callbacks are invoked.
	
  Revisions:
		created 01/05/2003 LM

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "packetblock.h"
#include "avrDrd.h"
#include "drdCli.h"
#include "avc.h"
#include "avcDefs.h"
#include "cr.h"
#include "cli.h"
#include "datastream.h"
#include "systemcallbacks.h"
#ifdef _SYSDEBUG
#include "sysDebug.h"
#endif //_SYSDEBUG

AVR_DRD_HOST_STATE		avrDrdHostState = { 0 };		// global for ease of access

// task (thread) IDs for this module
uint32					avrDrdHandlerThreadID;		// avrDrd handler thread
const char*				avrDrdHandlerThreadName = "avrDrdHandlerThread";

void avrDrdHandlerThread(void *dummy)
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
HRESULT avrDrdCreateSemaphores(void)
{
	HRESULT		hResult = NO_ERROR;

#if 0 //LM??? not used
	hResult = TCMutexOpen(&avrDrdHandlerSemID);
	if (hResult != NO_ERROR) return hResult;
#endif

	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT avrDrdResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avrDrd handler thread
	hResult = TCTaskResume(avrDrdHandlerThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT avrDrdSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avrDrd handler thread
	hResult = TCTaskSuspend(avrDrdHandlerThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Create tasks for this module
*/
HRESULT avrDrdInitializeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avrDrd handler thread
	hResult = TCTaskCreate(&avrDrdHandlerThreadID, avrDrdHandlerThreadName, avrDrdHandlerThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Initialization of this module
*/
HRESULT avrDrdInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = avrDrdCreateSemaphores();
	if (hResult != NO_ERROR) return hResult;

	hResult = avrDrdResumeTasks();
	if (hResult != NO_ERROR) return hResult;

#if 0 //def _CLI
	avrDrdCliInstallTools();
#endif //_CLI

	return hResult;
}

HRESULT	avrDrdSetMasterVolume (uint32 newVolume)
{
	avrDrdHostState.masterVolume = newVolume;

	return E_PKT_AVC_STABLE;
}

HRESULT	avrDrdSetTrimVolume (uint32 newVolume, uint32 audioChannel)
{
    avrDrdHostState.trimVolume[audioChannel] = newVolume;

	return E_PKT_AVC_STABLE;
}

HRESULT	avrDrdSetAudioDelay (uint32 newDelay, uint32 audioChannel)
{
	UNUSED_ARG(newDelay);
	UNUSED_ARG(audioChannel);
	return E_NOT_IMPLEMENTED;
}

HRESULT	avrDrdSetMute (BOOL newValue)
{
	avrDrdHostState.mute = newValue;

	return E_PKT_AVC_STABLE;
}

HRESULT	avrDrdSetAudioMode (uint32 newMode)
{
	UNUSED_ARG(newMode);
	return E_NOT_IMPLEMENTED;
}
