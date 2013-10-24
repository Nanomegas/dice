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
 * DESCRIPTION: API for the DRD (Device Reference Design) module

	This file describes the API to the DRD (Device Reference Design)


	 +--------------------+
	 |                    |
	 |    DRD Messaging   |<---------> host computer
	 |                    |
	 +--------------------+
			  ^
			  |
			  |
			  v
	 +--------------------+
	 |                    |
	 |      --- DRD       |	** this module
	 |                    |
	 +--------------------+
			  ^
			  |
			  |
			  v
	 +--------------------+
	 |                    |
	 |	      AV/C        |
	 |    tuner & audio   |<---------> 1394 bus
	 |      subunits      |
	 |                    |
	 +--------------------+

	This module provides drd services to the av/c subunits:
		- a function for each command that can be sent to host
			(eg. for avr drd this could be setTunerFrequency, getMasterVolume)
		- a structure which reflects the current state of the host
		- optional callback registration for each host state element
			(to support av/c 'notify' functionality)

	Application Guide:
		To send a command to the Host, call the appropriate drdSet.. function.
		The drdSet.. function queues a message for transmission to host and returns to called immediately. 
		Since we're running open-loop to the host, applications never receive direct confirmation
		that the command succeeded.
		Normally, the host sends a reply with the current state of whatever we were trying to set
		and this information is logged into the host state table (global, read-only access).

	The AV/C notify function is supported by registering a callback for the appropriate parameter.

	Revisions:
		created 06/13/2003 lm

***********************************************************************/

#ifndef _DRD_H
#define _DRD_H

#include "1394.h"
#include "TCTypes.h"
#include "drdDefs.h"

/*************************************************************************
	
	Atomic definitions needed for the big structure with all the host state information

*************************************************************************/

// power state

typedef enum 														// host power state
{ 
	HOST_POWER_ON, 
	HOST_POWER_STANDBY
} DRD_HOST_POWER_STATE;

typedef enum														// all possible input and output devices
{
	DEV_NULL_DEVICE	= 0,
	DEV_TEST,				// I/O
	DEV_VIDEO,				// I/O
	DEV_DIGITAL_AUDIO,		// I/O
	DEV_CD_PLAYER,			// input
	DEV_DVD_PLAYER,			// input
	DEV_TAPE_PLAYER,		// input
	DEV_PHONOGRAPH,			// input
	DEV_AM_RADIO,			// input
	DEV_FM_RADIO,			// input
	DEV_ANALOG_SPEAKERS,	// output
	DEV_ANALOG_LINE			// output
} DRD_IO_DEVICE;

typedef struct 
{																	// device id - consists of device type and device number
	DRD_IO_DEVICE			deviceType;
	uint32					deviceNum;								// 0-based
} DRD_DEVICE_ID;

#define DRD_MAX_ACTIVE_INPUTS	1									// maximum number of concurrent input devices
#define DRD_MAX_ACTIVE_OUTPUTS	5									// maximum number of concurrent output devices

typedef struct							
{																	// structure with all the host state information
	DRD_HOST_POWER_STATE	hostPowerState;

	DRD_DEVICE_ID			curDevIDIn[DRD_MAX_ACTIVE_INPUTS];		// [0,..,n] input devices, unused array elements have device = DEV_NULL_DEVICE
	DRD_DEVICE_ID			curDevIDOut[DRD_MAX_ACTIVE_OUTPUTS];	// [0,..,n] output devices, unused array elements have device = DEV_NULL_DEVICE
} DRD_HOST_STATE;

HRESULT		drdPowerInit(void); 
HRESULT		drdPowerStatus(void); 
HRESULT		drdPowerOn(void);
HRESULT		drdPowerOff(void); 

HRESULT		drdGetPowerState(DRD_HOST_POWER_STATE *state);
HRESULT		drdSetPowerState(DRD_HOST_POWER_STATE state);

void		drdHandlerThread(void *dummy);

HRESULT		drdCreateSemaphores(void);
HRESULT		drdResumeTasks(void);									// Resume tasks for this module
HRESULT		drdSuspendTasks(void);									// Suspend tasks for this module
HRESULT		drdInitializeTasks(void);								// Create tasks for this module
HRESULT		drdInitialize(void);									// Initialization of this module

#endif //_DRD_H

