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
 * DESCRIPTION: API for the AVR DRD module

	This file describes the API to the AVR (Audio-Video-Receiver) DRD.

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
				 |      AVR DRD       |	** this module
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
		

	This module provides AVR services to the av/c subunits:
		- a function for each command that can be sent to host
			(eg. setTunerFrequency, getMasterVolume)
		- a structure which reflects the current state of the host
		- optional callback registration for each host state element
			(to support av/c 'notify' functionality)

	This module also collects the AV/R's input and output lists at the appropriate time.

	Application Guide:

		To send a command to the Host, call the appropriate drdSet.. function.
		The drdSet.. function queues a message for transmission to host and returns to
		called immediately.  Since we're running open-loop to the host, applications
		never receive direct confirmation that the command succeeded.  Normally, the host
		sends a reply with the current state of whatever we were trying to set and this
		information is logged into the host state table (global, read-only access).

	The AV/C notify function is supported by registering a callback for the
	appropriate parameter.

	Revisions:
		created 09/25/2002 lm

*************************************************************************/

#ifndef _AVRDRD_H
#define _AVRDRD_H

#include "1394.h"
#include "TCTypes.h"
#include "drdDefs.h"
#include "drd.h"

/*************************************************************************
	
	Atomic definitions needed for the big structure with all the host state information

*************************************************************************/

typedef enum	// tuner output
{
	TUNER_MONO_OUT,
	TUNER_STEREO_OUT
} DRD_TUNER_OUTPUT;

typedef enum	// tuner scan mode
{
	TUNER_AUTO_MODE,
	TUNER_MANUAL_MODE
} DRD_TUNER_MODE;

// Audio formats
typedef enum
{
	DRD_AF_UNKNOWN 				= 0,	// Two channel, left justified
	DRD_AF_2CHAN_LEFT 			= 1,	// Two channel, left justified
	DRD_AF_2CHAN_EARLY_LRCK		= 2,	// Two channel. early LRCK
	DRD_AF_6CHAN_LEFT_JUST		= 3,	// Six channel, pairs, left justified
	DRD_AF_6CHAN_EARLY_LRCK		= 4,	// six channels, pairs, early LRCK
	DRD_AF_6CHAN_ONE_WIRE		= 5,	// six channels one wire
	DRD_AF_8CHAN_LEFT_JUST 		= 6,	// Eight channel, pairs, left justified
	DRD_AF_8CHAN_EARLY_LRCK		= 7,	// Eight channels, pairs, early LRCK
	DRD_AF_8CHAN_ONE_WIRE		= 8,	// Eight channels one wire
	DRD_AF_DVD_2CHAN_LEFT 		= 9, 	// DVD audio, two channel, left justified
	DRD_AF_DVD_2CHAN_EARLY_LRCK	= 10,	// DVD audio, two channel. early LRCK
	DRD_AF_DVD_6CHAN_LEFT_JUST	= 11,	// DVD audio, six channel, pairs, left justified
	DRD_AF_DVD_6CHAN_EARLY_LRCK	= 12,	// DVD audio, six channels, pairs, early LRCK
	DRD_AF_DVD_6CHAN_ONE_WIRE	= 13,	// DVD audio, six channels one wire
	DRD_AF_DVD_6CHAN_TWO_WIRE	= 14,	// DVD audio, six channels two wire
	DRD_AF_DVD_8CHAN_TWO_WIRE	= 15	// DVD audio, eight channels two wire
} AVR_AUDIO_MODE;

typedef enum
{
	DRD_AF_FREQ_UNKNOWN			= 0,
	DRD_AF_FREQ_44_1			= 1,	// 44.1
	DRD_AF_FREQ_48				= 2,	// 48
	DRD_AF_FREQ_96				= 3,	// 96
	DRD_AF_FREQ_192				= 4		// 192
} AVR_AUDIO_FREQ;	

typedef enum
{
	DRD_AF_TYPE_UNKNOWN			= 0,
	DRD_AF_TYPE_RAW_AUDIO		= 1,
	DRD_AF_TYPE_SPDIF_DATA		= 2,
	DRD_AF_TYPE_SPDIF_NODATA	= 3
} AVR_AUDIO_TYPE;

typedef struct 
{										// audio format - consists of format, frequency and type
	AVR_AUDIO_MODE		mode;
	AVR_AUDIO_FREQ		freq;
	AVR_AUDIO_TYPE		type;
} AVR_AUDIO_FORMAT;

// callback descriptor must identify your callback function with this key 
#define AVR_DRD_CALLBACK_KEY	0x41655221		// "AVR!"

typedef enum 
{
	AVR_EV_NONE,				// no event
	AVR_EV_POWER_STATE,			// powerState
	AVR_EV_MASTER_VOLUME,		// masterVolume
	AVR_EV_TRIM_VOLUME,			// trimVolume[]
	AVR_EV_MUTE,				// mute
	AVR_EV_INPUT_SELECT,		// curInputDeviceId
	AVR_EV_OUTPUT_STATUS,		// curOutputDeviceIds
	AVR_EV_AUDIO_MODE,			// audioMode
	AVR_EV_AUDIO_DELAY,			// audioDelays[]
	AVR_EV_AUDIO_FORMAT_IN,		// audioFormatIn
	AVR_EV_AUDIO_FORMAT_OUT,	// audioFormatOut
	AVR_EV_TUNER_FREQUENCY,		// tunerFrequency
	AVR_EV_TUNER_PRESET,		// tunerPreset
	AVR_EV_TUNER_SCAN_MODE,		// tunerScanMode
	AVR_EV_TUNER_OUTPUT			// tunerOutput
} AVR_EVENT_TYPE;

typedef struct							
{										// A big structure with all the host state information
	struct
	{
		uint32				videoCount;
		uint32				digitalAudioCount;
		uint32				cdPlayerCount;
		uint32				dvdPlayerCount;
		uint32				tapePlayerCount;
		uint32				phonographCount;
		uint32				amRadioCount;
		uint32				fmRadioCount;
	} inputList;			
							
	struct					
	{						
		uint32				speakerAnalogGroups;
		uint32				videoCount;
		uint32				digitalAudioCount;
		uint32				analogLineCount;
	} outputList;			
							
#if 0 //LM???
	AVR_DEVICE_ID			curInputDeviceId;
																			// 0-n output devices, unused array elements have device = DEV_NULL_DEVICE
	AVR_DEVICE_ID			curOutputDeviceIds[AVR_DRD_MAX_ACTIVE_OUTPUTS];
#endif //LM???
							
	AVR_AUDIO_FORMAT		audioFormatIn;									// format INto the avr from device
	AVR_AUDIO_FORMAT		audioFormatOut;									// format OUT of the avr into the device
							
	uint32					tunerFrequency;									// 2 implied decimal places
							
	DRD_TUNER_MODE			tunerScanMode;			
							
	DRD_TUNER_OUTPUT		tunerOutput;
							
	uint32					tunerPreset;
							
	BOOL					mute;
							
	uint32					audioMode;										// stereo, thx, etc
							
																			// Audio delay and trim volume is on a per-channel basis
																			// DRD channel number is spec'd as 1-8 - we would have preferred a 0-based number
																			// For simplicity's sake, we size the array of channels such that the channel number can be used directly as the array index
																			// This means that array[0] is never used, but so what
	uint32					audioDelays[DRD_MAX_AUDIO_CHANNEL_NUMBER+1];	// 16-bit number for each audio channel
	uint32					masterVolume;
	uint32					trimVolume[DRD_MAX_AUDIO_CHANNEL_NUMBER+1];		// 16-bit number for each audio channel

} AVR_DRD_HOST_STATE;

extern AVR_DRD_HOST_STATE avrDrdHostState;		// global for ease of access

/* DRD event notification.  When information is received from the Host, the DRD invokes
	a callback (if registered) with an event type indicating what new information is 
	available in the global status structure.
	
	Callbacks are implemented via the callback registry mechanism, see files:
		systemcallbacks.h
		callbackRegistry.h

	Sample descriptor to register a callback with this module:

static		CALLBACK_DESCRIPTOR	avrDrdHostEventCBDescriptor[]=
{
	{CALLBACK_DESCRIPTOR_TYPE, 	AVR_DRD_TYPE },
	{CALLBACK_DESCRIPTOR_NAME,	(uint32)"AVR DRD EVENT CB" },
	{AVR_DRD_CALLBACK_KEY,		(uint32)&avrDrdEventCallback },
	{CALLBACK_DESCRIPTOR_NEXT,	0 },
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};
*/



/* drd event callback - called when a drd event occurs (called via the callback registry)
	event - indicates the particular drd event that occurred
	audioChannel - TRIM_VOLUME and AUDIO_DELAY events only, channel affected by the event
	returns - nothing, any errors detected in the callback must be handled by the callback function
*/

typedef	void (*AVR_DRD_EVENT_CALLBACK)(AVR_EVENT_TYPE event, uint32 audioChannel);

void		avrDrdHandlerThread(void *dummy);

HRESULT		avrDrdCreateSemaphores(void);
HRESULT		avrDrdResumeTasks(void);									// Resume tasks for this module
HRESULT		avrDrdSuspendTasks(void);									// Suspend tasks for this module
HRESULT		avrDrdInitializeTasks(void);								// Create tasks for this module
HRESULT		avrDrdInitialize(void);										// Initialization of this module


















#if 1 //LM???

/*
	Interface to this module available to AV/C subuinits
*/

// Is the host on-line and available for interaction on the bus?
BOOL	avrDrdHostIsReady(void);

/* Set/get host parameter.
	The 'set' and 'get' functions queue a message for transmission to the host.  There is
	no direct confirmation that the host received and executed the requested action.
	Indirect confirmation can be obtained by registering a callback which will be called 
	when the host sends the current setting for the associated parameter.

	To remove a callback, register a callback with address 0 (IE Null callback).
	(or remove all callbacks in one fell swoop via 'removeAllCallbacks')

	The set/get functions will fail if the host is not ready - see host state table above
	All set/get/register functions are non-blocking.
*/

// master volume
HRESULT		avrDrdSetMasterVolume (uint32 newVolume);
HRESULT		avrDrdGetMasterVolume (void);

// trim Volume
HRESULT		avrDrdSetTrimVolume (uint32 newVolume, uint32 audioChannel);
HRESULT		avrDrdGetTrimVolume (uint32 audioChannel);

// mute
HRESULT		avrDrdSetMute (BOOL newValue);
HRESULT		avrDrdGetMute (void);

// select input
HRESULT		avrDrdSetInputSelect (DRD_IO_DEVICE deviceType, uint32 deviceNumber);
HRESULT		avrDrdGetInputSelect (void);

// connect/disconnect outputs
HRESULT		avrDrdConnectOutput (DRD_IO_DEVICE deviceType, uint32 deviceNumber);
HRESULT		avrDrdDisconnectOutput (DRD_IO_DEVICE deviceType, uint32 deviceNumber);
HRESULT		avrDrdDisconnectAllOutputs (void);

// output status
HRESULT		avrDrdGetOutputStatus (void);

// audio format
HRESULT		avrDrdSetAudioFormatIn (AVR_AUDIO_FORMAT format);	// format from Dhiva Into AVR
HRESULT		avrDrdGetAudioFormatIn (void);	
HRESULT		avrDrdGetAudioFormatOut (void);	// format Out of avr into Dhiva

// audio mode
HRESULT		avrDrdSetAudioMode (uint32 newMode);
HRESULT		avrDrdGetAudioMode (void);

// audio Delay
HRESULT		avrDrdSetAudioDelay (uint32 newDelay, uint32 audioChannel);
HRESULT		avrDrdGetAudioDelay (uint32 audioChannel);

// tuner frequency
HRESULT		avrDrdSetTunerFrequency (uint32 newFreq);
HRESULT		avrDrdSetTunerFrequencyUp (void);
HRESULT		avrDrdSetTunerFrequencyDown (void);
HRESULT		avrDrdGetTunerFrequency (void);

// tuner scan mode
HRESULT		avrDrdSetTunerScanMode (DRD_TUNER_MODE newScanMode);
HRESULT		avrDrdGetTunerScanMode (void);

// tuner output type
HRESULT		avrDrdSetTunerOutput (DRD_TUNER_OUTPUT newOutputType);
HRESULT		avrDrdGetTunerOutput (void);

// tuner Preset
HRESULT		avrDrdSetTunerPreset (uint32	newPreset);
HRESULT		avrDrdSetTunerPresetNext (void);
HRESULT		avrDrdSetTunerPresetPrev (void);
HRESULT		avrDrdGetTunerPreset (void);

// Deprecated, non-supported functions
HRESULT		avrDrdSetOutputSelect (DRD_IO_DEVICE deviceType, uint32 deviceNumber);

#endif //LM???

#endif //_AVRDRD_H
