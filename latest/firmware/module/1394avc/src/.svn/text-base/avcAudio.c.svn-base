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

		Implement AV/C Audio sub-unit

		This module is the 'main' for the audio subunit.  Currently, this subunit
		executes av/c commands while running on the av/c command handler's thread.

		Basically, it registers a callback with the avc command handler.  When the
		callback is invoked, it executes the command (send command to host), sends
		response and returns to caller.  It does not need to block anywhere, and
		therefore does not need to be its own thread.
 
		PROBLEMS, ASSUMPTIONS and LIMITATIONS
			- this module handles a single instance, subunit id #0
			- no validation of function block type/id
			- fake validation of audio channel number
			- mute is implemented for master channel only
			- changing configuration causes all pending notify requests to be flushed

  Revisions:
		created 05/12/2003 kbj

***************************************************************************/
#if defined (_AVC) && defined (_EDS) && defined (_DRD)

#include "TCTypes.h"
#include "TCDebug.h"
#include "ErrorCodes.h"
#include "avrDrd.h"
#include "avcDefs.h"
#include "avcUtils.h"
#include "avc.h"
#include "avcCmdConnection.h"
#include "avcCmdStreamFormat.h"
#include "systemcallbacks.h"
#include "datastream.h"
#include "packetBlock.h"
#include "cr.h"
#include "targetPlugs.h"
#include "lal.h"
#include "eds.h"
#include "avs.h"

#define NUM_AUDIO_SUBUNIT_INSTANCES		1

#include "audioDescriptor.c"	// contrary to convention, we are including a 'c' file

// callback descriptor - drd interface

/*
static void avrDrdEventCallback(AVR_EVENT_TYPE event, uint32 audioChannel);	// forward ref

static CALLBACK_DESCRIPTOR	avrDrdHostEventCBDescriptor[]=
{
	{CALLBACK_DESCRIPTOR_TYPE, 	DRD_TYPE },
	{CALLBACK_DESCRIPTOR_NAME,	(uint32)"AVR DRD EVENT CB" },
	{AVR_DRD_CALLBACK_KEY,		(uint32)&avrDrdEventCallback },
	{CALLBACK_DESCRIPTOR_NEXT,	0 },
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};
*/

/************  local definitions  *************/

/* Command execution often depends on both CTYPE and control_attribute.  To maximize
	maintainability, we will treat commands as CTYPE/ATTRIBUTE combinations.
	This is done with a macro which combines the 2 8-bit values into a single
	16-bit value. */

typedef uint16	CTYPE_ATTRIBUTE;
#define	CTYPE_ATTRIBUTE(ctype, attribute) ((CTYPE_ATTRIBUTE)((ctype<<8)+attribute))

// function block control command parameters

typedef struct 
{
	uint32			fbType;				// function block type
	uint32			fbId;				// function block ID
	uint8			fbAttribute;		// control attribute
	CTYPE_ATTRIBUTE	ctype_attribute;	// combined ctype and attribute
		
} FUNCTION_BLOCK_PARMS;


// structure for 'remembering' notify requests

typedef struct
{
	LAL_NODE_HANDLE	nodeHandle;		// who posted the notify?
	uint8			fbId;			// function block id
} NOTIFY_INFO;

static NOTIFY_INFO	muteNotifyInfo;

	/* volume and delay arrays are sized one more than 'max' because there's
		an additional channel, the 'master' channel */

static NOTIFY_INFO	delayNotifyInfo[DRD_MAX_AUDIO_CHANNEL_NUMBER+1];
static NOTIFY_INFO	volumeNotifyInfo[DRD_MAX_AUDIO_CHANNEL_NUMBER+1];

// common text for error messages
static char	moduleName[] = __FILE__;


/*****************************************************************************
			FORWARDS
*****************************************************************************/

static HRESULT	avcAudioRxPacketCallback(AVC_HEADER		*avcHeaderInfo,
										 PB 			*pPacketBlock);


/*****************************************************************************
 
	Descriptors

	Descriptor information for the audio subunit is all static, never changing data.
	The data is generated automatically from an Excel spreadsheet and dumped into
	a 'c' file which we include here.  It contains an array of uint32 named
		"AudioSubunitData"	
*/

static EDS_CLIENT_SESSION audioSession = {
		AVC_SU_TYPE_AUDIO,		// subunit type
		0,						// subunit id
		0,						// subuint identifier
		0 };					// custom_resolver


static EDS_SUBUNITIDENTIFIER siAudioData = 
{
	2,							// uint8		size_of_list_id;
	0,							// uint8		size_of_object_id;
	2,							// uint8		size_of_entry_position;
	0,							// uint16		number_of_root_lists;
	0,							// uint16		si_length;
	0,							// uint16		si_infoblock_count;
	0,							// RANGEMAP*	si_writable_mask;
	(void *)AudioSubunitData,	// void*		subunit_information;
	0,							// uint16		mi_length;
	0,							// uint16		mi_infoblock_count;
	0,							// RANGEMAP*	mi_writable_mask;
	NULL,						// void*		manufacturer_information;
	0,							// uint16		extended_infoblock_count; 
};

/*************************************************************************
	AUDIO CALLBACK DESCRIPTOR
	
	This defines the callback for the av/c handler.
*************************************************************************/

static CALLBACK_DESCRIPTOR	AVC_AUDIO_CALLBACK[]=
{
	{CALLBACK_DESCRIPTOR_TYPE,AVC_SUBUNIT_TYPE},
	{CALLBACK_DESCRIPTOR_NEXT,0},
	{CALLBACK_DESCRIPTOR_NAME,(uint32)"AVC_AUDIO_CALLBACK"},
	{AVC_SUBUNIT_CALLBACK_KEY,(uint32)&avcAudioRxPacketCallback}, // CALLBACK PTR of TYPE AVC_SUBUNIT_CALLBACK},
	{AVC_SUBUNIT_SUBUNITTYPE_KEY,AVC_SU_TYPE_AUDIO},
	{AVC_SUBUNIT_ID_KEY, 0},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


/*************************************************************************
	FUNCTIONS
*************************************************************************/

static HRESULT createAudioDescriptors(void)
{
	HRESULT 				hResult = NO_ERROR;
			
	// Create client descriptor session 
	hResult = edsRegisterSubunitClientSession(&audioSession);
	// Create subunit identifier descriptor
	if (NO_ERROR == hResult)
	{
		siAudioData.si_length = sizeof(AudioSubunitData);
		siAudioData.subunit_information = &AudioSubunitData;
		hResult = edsCreateSubunitIdentifier(&audioSession, &siAudioData);
	}

	return (hResult);
}

/*****************************************************************************
 
	Helper functions

******************************************************************************/

static BOOL	functionBlockExists(uint32 functionBlockType, uint32 functionBlockId)
{
	UNUSED_ARG(functionBlockType);
	UNUSED_ARG(functionBlockId);
	return(TRUE);
}

/*************************/
static uint32 curDrdModeChannelCount(void)
{
	switch (avrDrdHostState.audioMode)
	{
		case DRD_MODE_MONO:		
			return(DRD_NUM_SPEAKERS_MONO);
				
		case DRD_MODE_STEREO:	
			return(DRD_NUM_SPEAKERS_STEREO);
				
		case DRD_MODE_DOLBY_PROLOGIC:
			return(DRD_NUM_SPEAKERS_PROLOGIC);
				
		case DRD_MODE_AC3:
			return(DRD_NUM_SPEAKERS_AC3);
	}
	return(0);
}	
/*************************/

static BOOL	AudioChannelIsValid(uint32 audioChannel)
{
	if (audioChannel <= curDrdModeChannelCount())
	{
		return(TRUE);
	}
	else
	{
		return(FALSE);
	}
}

/*****************************************************************************
 
	NOTIFY handlers

	Notify is implemented by registering a callback with the avrDrd for the associated
	data element.  Volume and delay notifies are a bit more involved because the notify
	is posted on a per-channel basis.

******************************************************************************/
/*
static void drdMuteCallback(void)
{
	uint32			muteResponse[3];		// 10-byte response
	DataStream		ds;
	HRESULT			hResult = NO_ERROR;

	if (0 == muteNotifyInfo.nodeHandle)
	{
		return;		// no notify pending
	}
	else
	{
		muteNotifyInfo.nodeHandle = 0;			// notify happens once only
		hResult = dsOpenStream(&ds, muteResponse, sizeof(muteResponse), dsMODE_WRITE | dsMODE_PLATFORMENDIAN);
		if (NO_ERROR == hResult)
		{
			dsWrite8Bits(&ds, AVC_SU_TYPE_AUDIO<<3); 			// subunit type and id
			dsWrite8Bits(&ds, AVC_CMD_FUNCTION_BLOCK);			// opcode
			dsWrite8Bits(&ds, AVC_FB_TYPE_FEATURE);				// op[0], function block type
			dsWrite8Bits(&ds, muteNotifyInfo.fbId);				// op[1], function block id
			dsWrite8Bits(&ds, AVC_FB_CTRL_ATTRIBUTE_CURRENT);	// op[2], control attribute
			dsWrite8Bits(&ds, 2);								// op[3], selector length
			dsWrite8Bits(&ds, 0);								// op[4], audio channel number
			dsWrite8Bits(&ds, AVC_AUDIO_MUTE_CONTROL);			// op[5], control selector
			dsWrite8Bits(&ds, 1);								// op[6], length of control data
			if (avrDrdHostState.mute)
			{
				hResult = dsWrite8Bits(&ds, AVC_AUDIO_MUTE_ON);	// op[7], mute_on
			}
			{
				hResult = dsWrite8Bits(&ds, AVC_AUDIO_MUTE_OFF);	// op[7], mute_on
			}
		}

		if (NO_ERROR == hResult)
		{
			hResult = avcBlockingSendRequest(
							muteNotifyInfo.nodeHandle,		// handle
							AVC_RESPONSE_CHANGED, 			// ctype
							sizeof(muteResponse),			// byteLength,
							muteResponse);					// *bufPtr);
		}
		dsCloseStream(&ds); 
	}
	if (hResult != NO_ERROR)
	{
		sysLogError(hResult, __LINE__, moduleName);
	}
}
*/
/*************************/
static HRESULT postNotifyMute (uint32 audioChannel, uint32 functionBlockId, PB *packetBlock)
{
	HRESULT	hResult = NO_ERROR;

	UNUSED_ARG(audioChannel);
	UNUSED_ARG(packetBlock);

	if (0 != muteNotifyInfo.nodeHandle)
	{
		return(E_PKT_AVC_REJECTED);			// notify already posted
	}
	else
	{
		muteNotifyInfo.fbId = (uint8)functionBlockId;
		//KBJ??? hResult = lalGetHandleFromReqPB(packetBlock, &muteNotifyInfo.nodeHandle);
		if (NO_ERROR != hResult)
		{
			muteNotifyInfo.nodeHandle = 0;		// make sure we're not left with a bogus handle
		}
	}
	return(hResult);
}	

/*****************************************************************************
	NotifyVolume

	Volume is bit trickier than the other notifies because the AV/C api uses channel
	numbers, where channel 0 is the 'master' channel and the AVR DRD has separate
	api's for master and 'trim' volume.
*/
/*
static HRESULT sendVolumeNotifyResponse(LAL_NODE_HANDLE handle, uint8 functionBlockId, uint32 channel, uint32 volumeValue)
{
	DataStream		ds;
	uint32			volumeResponse[3];		// 11-byte response
	HRESULT			hResult;

	hResult = dsOpenStream(&ds, volumeResponse, sizeof(volumeResponse), dsMODE_WRITE | dsMODE_PLATFORMENDIAN);
	if (NO_ERROR == hResult)
	{
		dsWrite8Bits(&ds, AVC_SU_TYPE_AUDIO<<3);		// subunit type and id
		dsWrite8Bits(&ds, AVC_CMD_FUNCTION_BLOCK);		// opcode
		dsWrite8Bits(&ds, AVC_FB_TYPE_FEATURE);		// op[0], function block type
		dsWrite8Bits(&ds, functionBlockId);			// op[1], function block id
		dsWrite8Bits(&ds, AVC_FB_CTRL_ATTRIBUTE_CURRENT);	// op[2], control attribute
		dsWrite8Bits(&ds, 2);							// op[3], selector length
		dsWrite8Bits(&ds, (uint8)channel);				// op[4], audio channel number
		dsWrite8Bits(&ds, AVC_AUDIO_VOLUME_CONTROL);	// op[5], control selector
		dsWrite8Bits(&ds, 2);							// op[6], length of control data

		hResult = dsWrite8Bits(&ds, (uint8)((volumeValue >> 8) & 0xFF));	// op[7], volume steps high
		if (NO_ERROR == hResult)
		{
			hResult = dsWrite8Bits(&ds, (uint8)(volumeValue & 0xFF));	// op[8], volume steps low
		}
	}

	if (NO_ERROR == hResult)
	{
		hResult = avcBlockingSendRequest(
						handle,							// handle
						AVC_RESPONSE_CHANGED, 			// ctype
						sizeof(volumeResponse),			// byteLength,
						volumeResponse);				// *bufPtr);
	}
	dsCloseStream(&ds);
	return(hResult); 
}
*/
/*************************/
/*
static void drdTrimVolumeCallback (uint32 channel)
{
	LAL_NODE_HANDLE	handle;
	HRESULT			hResult;

	//KBJ??? ASSERT(DRD_MAX_AUDIO_CHANNEL_NUMBER >= channel);
	//KBJ??? ASSERT(AVC_AUDIO_CHANNEL_MASTER != channel);

	if (0 == volumeNotifyInfo[channel].nodeHandle)
	{
		return;		// no notify pending
	}
	else
	{
		handle = volumeNotifyInfo[channel].nodeHandle;
		volumeNotifyInfo[channel].nodeHandle = 0;			// notify happens once only
		hResult = sendVolumeNotifyResponse(
							handle, 
							volumeNotifyInfo[channel].fbId,
							channel, 
							avrDrdHostState.trimVolume[channel]);
	}
	if (hResult != NO_ERROR)
	{
		sysLogError(hResult, __LINE__, moduleName);
	}
}
*/
/*************************/
/*
static void drdMasterVolumeCallback (void)
{
	LAL_NODE_HANDLE	handle;
	HRESULT			hResult;

	if (0 == volumeNotifyInfo[AVC_AUDIO_CHANNEL_MASTER].nodeHandle)
	{
		return;		// no notify pending
	}
	else
	{
		handle = volumeNotifyInfo[AVC_AUDIO_CHANNEL_MASTER].nodeHandle;
		volumeNotifyInfo[AVC_AUDIO_CHANNEL_MASTER].nodeHandle = 0;			// notify happens once only
		hResult = sendVolumeNotifyResponse(
							handle, 
							volumeNotifyInfo[AVC_AUDIO_CHANNEL_MASTER].fbId,
							AVC_AUDIO_CHANNEL_MASTER, 
							avrDrdHostState.masterVolume);
	}
	if (hResult != NO_ERROR)
	{
		sysLogError(hResult, __LINE__, moduleName);
	}
}
*/
/*************************/

static HRESULT postNotifyVolume (uint32	audioChannel, uint32 functionBlockId, PB *packetBlock)
{
	HRESULT	hResult = NO_ERROR;

	//KBJ??? ASSERT(DRD_MAX_AUDIO_CHANNEL_NUMBER >= audioChannel);
	UNUSED_ARG(packetBlock);

	if (0 != volumeNotifyInfo[audioChannel].nodeHandle)
	{
		return(E_PKT_AVC_REJECTED);			// notify already posted
	}
	else
	{
		volumeNotifyInfo[audioChannel].fbId = (uint8)functionBlockId;
		//KBJ??? hResult = lalGetHandleFromReqPB(packetBlock, &volumeNotifyInfo[audioChannel].nodeHandle);
		if (NO_ERROR != hResult)
		{
			volumeNotifyInfo[audioChannel].nodeHandle = 0;	// make sure we're not left with a bogus handle
		}
	}
	return(hResult);
}	

/*****************************************************************************
	NotifyDelay

*/
/*
static HRESULT sendDelayNotifyResponse(LAL_NODE_HANDLE 	handle,
									   uint8			functionBlockId,
									   uint32 			channel, 
									   uint32 			delayValue)
{
	DataStream		ds;
	uint32			delayResponse[3];		// 11-byte response
	HRESULT			hResult;

	hResult = dsOpenStream(&ds, delayResponse, sizeof(delayResponse), dsMODE_WRITE | dsMODE_PLATFORMENDIAN);
	if (NO_ERROR == hResult)
	{
		dsWrite8Bits(&ds, AVC_SU_TYPE_AUDIO<<3);			// subunit type and id
		dsWrite8Bits(&ds, AVC_CMD_FUNCTION_BLOCK);			// opcode
		dsWrite8Bits(&ds, AVC_FB_TYPE_FEATURE);				// op[0], function block type
		dsWrite8Bits(&ds, functionBlockId);					// op[1], function block id
		dsWrite8Bits(&ds, AVC_FB_CTRL_ATTRIBUTE_CURRENT);	// op[2], control attribute
		dsWrite8Bits(&ds, 2);								// op[3], selector length
		dsWrite8Bits(&ds, (uint8)channel);					// op[4], audio channel number
		dsWrite8Bits(&ds, AVC_AUDIO_DELAY_CONTROL);			// op[5], control selector
		dsWrite8Bits(&ds, 2);								// op[6], length of control data

		hResult = dsWrite8Bits(&ds, (uint8)((delayValue >> 8) & 0xFF));	// op[7], delay high
		if (NO_ERROR == hResult)
		{
			hResult = dsWrite8Bits(&ds, (uint8)(delayValue & 0xFF));		// op[8], delay steps low
		}
	}

	if (NO_ERROR == hResult)
	{
		hResult = avcBlockingSendRequest(
						handle,							// handle
						AVC_RESPONSE_CHANGED, 			// ctype
						sizeof(delayResponse),			// byteLength,
						delayResponse);					// *bufPtr);
	}
	dsCloseStream(&ds);
	return(hResult); 
}
*/
/*************************/
/*
static void drdDelayCallback (uint32 channel)
{
	LAL_NODE_HANDLE	handle;
	HRESULT			hResult;

	//KBJ??? ASSERT(DRD_MAX_AUDIO_CHANNEL_NUMBER >= channel);
	//KBJ??? ASSERT(AVC_AUDIO_CHANNEL_MASTER != channel);	// delay has no 'master' channel

	if (0 == delayNotifyInfo[channel].nodeHandle)
	{
		return;		// no notify pending
	}
	else
	{
		handle = delayNotifyInfo[channel].nodeHandle;
		delayNotifyInfo[channel].nodeHandle = 0;			// notify happens once only
		hResult = sendDelayNotifyResponse(
							handle, 
							delayNotifyInfo[channel].fbId,
							channel, 
							avrDrdHostState.audioDelays[channel]);
	}
	if (hResult != NO_ERROR)
	{
		sysLogError(hResult, __LINE__, moduleName);
	}
}
*/
/*************************/

static HRESULT postNotifyDelay (uint32	audioChannel, uint32 functionBlockId, PB *packetBlock)
{
	HRESULT	hResult = NO_ERROR;

	//KBJ??? ASSERT(DRD_MAX_AUDIO_CHANNEL_NUMBER >= audioChannel);
	//KBJ??? ASSERT(AVC_AUDIO_CHANNEL_MASTER != audioChannel);	// delay has no 'master' channel

	UNUSED_ARG(packetBlock);

	if (0 != delayNotifyInfo[audioChannel].nodeHandle)
	{
		return(E_PKT_AVC_REJECTED);			// notify already posted
	}
	else
	{
		delayNotifyInfo[audioChannel].fbId = (uint8)functionBlockId;
		//KBJ??? hResult = lalGetHandleFromReqPB(packetBlock, &delayNotifyInfo[audioChannel].nodeHandle);
		if (NO_ERROR != hResult)
		{
			delayNotifyInfo[audioChannel].nodeHandle = 0;	// make sure we're not left with a bogus handle
		}
	}
	return(hResult);
}	

/*****************************************************************************
 
	volumeControlHandler
	muteControlHandler
	delayControlHandler

		These functions each handle a specific feature block control
		block.  The same rules apply as for their 'parent' functions:
			- if function returns NO_ERROR, the function is responsible for sending
				a response packet (via avcSendRequest) and for disposing of the 
				packetBlock.
			- if the function returns anything other than NO_ERROR, the avc response
				and packetBlock disposal will be performed for it.

******************************************************************************/

static HRESULT volumeControlHandler(DataStream *pAvcStream, FUNCTION_BLOCK_PARMS *pFbParms, uint32 audioChannel, PB *packetBlock)
{
	uint8	valueMsb=0;
	uint8	valueLsb=0;
	uint32	streamPosVolume;	// save stream position in case its a status command
	HRESULT	hResult = NO_ERROR;

	streamPosVolume = dsGetPosition(pAvcStream);

	// trap the volume parameters
	hResult = dsRead8Bits (pAvcStream, &valueMsb);
	if (NO_ERROR == hResult)
	{
		hResult = dsRead8Bits (pAvcStream, &valueLsb);
	}
			
	if (NO_ERROR == hResult)
	{
		/* diverge again, depending on command type and control attribute.
			General inquiry CTYPE has been handled at the highest level, so
			we can ignore that one. */

		switch (pFbParms->ctype_attribute)
		{
			// ctype GENERAL INQUIRY has been handled already at some higher level

			// ctype NOTIFY is supported for Current only
			case CTYPE_ATTRIBUTE (AVC_CTYPE_NOTIFY, AVC_FB_CTRL_ATTRIBUTE_CURRENT):
					// wants to be 'notified' when current volume changes
					hResult = postNotifyVolume(audioChannel, pFbParms->fbId, packetBlock);
					if (NO_ERROR == hResult)
					{
						hResult = E_PKT_AVC_INTERIM;
					}
					break;

			// ctype CONTROL
			case CTYPE_ATTRIBUTE (AVC_CTYPE_CONTROL, AVC_FB_CTRL_ATTRIBUTE_DELTA):
					// relative change in volume
					{
						int16	relativeVolume = (int16)((valueMsb << 8) + valueLsb);	// signed value

						/* Request is a relative change, based on a 16-bit signed value.
							We will convert this to an absolute volume value and set it 
							as such for the appropriate channel. */
						if (AVC_AUDIO_CHANNEL_MASTER == audioChannel)
						{
							hResult = avrDrdSetMasterVolume (
											(uint32)(avrDrdHostState.masterVolume + relativeVolume));
						}
						else
						{
							hResult = avrDrdSetTrimVolume (
											(uint32)(avrDrdHostState.masterVolume + relativeVolume),
											audioChannel);
						}
						if (NO_ERROR == hResult)
						{
							hResult = E_PKT_AVC_ACCEPTED;
						}
					}
					break;

			case CTYPE_ATTRIBUTE (AVC_CTYPE_CONTROL, AVC_FB_CTRL_ATTRIBUTE_CURRENT):
					// absolute change in volume
					{
						uint32	absoluteVolume  = (valueMsb << 8) + valueLsb;

						if (AVC_AUDIO_CHANNEL_MASTER == audioChannel)
						{
							hResult = avrDrdSetMasterVolume (absoluteVolume);
						}
						else
						{
							hResult = avrDrdSetTrimVolume (absoluteVolume, audioChannel);
						}
						if (NO_ERROR == hResult)
						{
							hResult = E_PKT_AVC_ACCEPTED;
						}
					}
					break;


			// ctype STATUS
			case CTYPE_ATTRIBUTE (AVC_CTYPE_STATUS, AVC_FB_CTRL_ATTRIBUTE_CURRENT):
					/* get the requested volume and write it into the packet block at the
						previously saved location, overwriting the volume field */
					{
						uint32	volume;

						if (AVC_AUDIO_CHANNEL_MASTER == audioChannel)
						{
							volume = avrDrdHostState.masterVolume;
						}
						else
						{
							volume = avrDrdHostState.trimVolume[audioChannel];
						}

						valueMsb = (uint8)((volume >> 8) & 0xFF);
						valueLsb = (uint8)(volume  & 0xFF);

						dsSetPosition(pAvcStream, streamPosVolume);
						hResult = dsSwitchMode(pAvcStream, dsMODE_WRITE);
						if (NO_ERROR == hResult)
						{
							hResult = dsWrite8Bits (pAvcStream, valueMsb);
							if (NO_ERROR == hResult)
							{
								hResult = dsWrite8Bits (pAvcStream, valueLsb);
							}
						}

						if (NO_ERROR == hResult)
						{
							// all ok, return 'stable' to avc_main and avc_main will send the response
							hResult = E_PKT_AVC_STABLE;
						}
					}
					break;

			// ctype STATUS
			case CTYPE_ATTRIBUTE (AVC_CTYPE_STATUS, AVC_FB_CTRL_ATTRIBUTE_RESOLUTION):
					/* get the requested value and write it into the packet block at the
						previously saved location, overwriting the return parameter field */
					{
						dsSetPosition(pAvcStream, streamPosVolume);
						hResult = dsSwitchMode(pAvcStream, dsMODE_WRITE);
						if (NO_ERROR == hResult)
						{
							hResult = dsWrite8Bits (pAvcStream, 0x01);
							if (NO_ERROR == hResult)
							{
								hResult = dsWrite8Bits (pAvcStream, 0x00);
							}
						}

						if (NO_ERROR == hResult)
						{
							// all ok, return 'stable' to avc_main and avc_main will send the response
							hResult = E_PKT_AVC_STABLE;
						}
					}
					break;

			// ctype STATUS
			case CTYPE_ATTRIBUTE (AVC_CTYPE_STATUS, AVC_FB_CTRL_ATTRIBUTE_MINIMUM):
					/* get the requested value and write it into the packet block at the
						previously saved location, overwriting the return parameter field */
					{
						dsSetPosition(pAvcStream, streamPosVolume);
						hResult = dsSwitchMode(pAvcStream, dsMODE_WRITE);
						if (NO_ERROR == hResult)
						{
							hResult = dsWrite8Bits (pAvcStream, 0x89);
							if (NO_ERROR == hResult)
							{
								hResult = dsWrite8Bits (pAvcStream, 0x00);
							}
						}

						if (NO_ERROR == hResult)
						{
							// all ok, return 'stable' to avc_main and avc_main will send the response
							hResult = E_PKT_AVC_STABLE;
						}
					}
					break;

			// ctype STATUS
			case CTYPE_ATTRIBUTE (AVC_CTYPE_STATUS, AVC_FB_CTRL_ATTRIBUTE_MAXIMUM):
					/* get the requested value and write it into the packet block at the
						previously saved location, overwriting the return parameter field */
					{
						dsSetPosition(pAvcStream, streamPosVolume);
						hResult = dsSwitchMode(pAvcStream, dsMODE_WRITE);
						if (NO_ERROR == hResult)
						{
							hResult = dsWrite8Bits (pAvcStream, 0x00);
							if (NO_ERROR == hResult)
							{
								hResult = dsWrite8Bits (pAvcStream, 0x00);
							}
						}

						if (NO_ERROR == hResult)
						{
							// all ok, return 'stable' to avc_main and avc_main will send the response
							hResult = E_PKT_AVC_STABLE;
						}
					}
					break;


			// ctype SPECIFIC INQUIRY
			case CTYPE_ATTRIBUTE (AVC_CTYPE_SPECIFIC_INQUIRY, AVC_FB_CTRL_ATTRIBUTE_DELTA):
			case CTYPE_ATTRIBUTE (AVC_CTYPE_SPECIFIC_INQUIRY, AVC_FB_CTRL_ATTRIBUTE_CURRENT):
					hResult = E_PKT_AVC_IMPLEMENTED;
					break;

			default:
					hResult = E_PKT_AVC_NOT_IMPLEMENTED;
					break;
		}
	}

	return(hResult);
}
/*************************/

static HRESULT delayControlHandler(DataStream *pAvcStream, FUNCTION_BLOCK_PARMS *pFbParms, uint32 audioChannel, PB *packetBlock)
{
	uint8	valueMsb=0;
	uint8	valueLsb=0;
	uint32	streamPosDelay;	// save stream position in case its a status command
	HRESULT	hResult=NO_ERROR;

	streamPosDelay = dsGetPosition(pAvcStream);

	if (AVC_AUDIO_CHANNEL_MASTER == audioChannel)	
	{
		hResult = E_PKT_AVC_NOT_IMPLEMENTED;		// delay is not available on master channel
	}

	// trap the delay parameters
	if (NO_ERROR == hResult)
	{
		hResult = dsRead8Bits (pAvcStream, &valueMsb);
		if (NO_ERROR == hResult)
		{
			hResult = dsRead8Bits (pAvcStream, &valueLsb);
		}
	}
				
	if (NO_ERROR == hResult)
	{
		/* diverge again, depending on command type and control attribute.
			General inquiry CTYPE has been handled at the highest level, so
			we can ignore that one. */

		switch (pFbParms->ctype_attribute)
		{
			// ctype GENERAL INQUIRY has been handled already at some higher level

			// ctype NOTIFY is supported for Current only
			case CTYPE_ATTRIBUTE (AVC_CTYPE_NOTIFY, AVC_FB_CTRL_ATTRIBUTE_CURRENT):
					// wants to be 'notified' when current delay changes
					hResult = postNotifyDelay(audioChannel, pFbParms->fbId, packetBlock);
					if (NO_ERROR == hResult)
					{
						hResult = E_PKT_AVC_INTERIM;
					}
					break;

			// ctype CONTROL
			case CTYPE_ATTRIBUTE (AVC_CTYPE_CONTROL, AVC_FB_CTRL_ATTRIBUTE_CURRENT):
					// change current delay for a channel
					{
						uint32	delay  = (valueMsb << 8) + valueLsb;

						hResult = avrDrdSetAudioDelay (delay, audioChannel);
						if (NO_ERROR == hResult)
						{
							hResult = E_PKT_AVC_ACCEPTED;
						}
						break;
					}


			// ctype STATUS
			case CTYPE_ATTRIBUTE (AVC_CTYPE_STATUS, AVC_FB_CTRL_ATTRIBUTE_CURRENT):
					/* get the requested delay and write it into the packet block at the
						previously saved location, overwriting the delay field */
					{
						uint32	delay;

					   	delay = avrDrdHostState.audioDelays[audioChannel];

						valueMsb = (uint8)((delay >> 8) & 0xFF);
						valueLsb = (uint8)(delay  & 0xFF);

						dsSetPosition(pAvcStream, streamPosDelay);
						hResult = dsSwitchMode(pAvcStream, dsMODE_WRITE);
						if (NO_ERROR == hResult)
						{
							hResult = dsWrite8Bits (pAvcStream, valueMsb);
							if (NO_ERROR == hResult)
							{
								hResult = dsWrite8Bits (pAvcStream, valueLsb);
							}
						}

						if (NO_ERROR == hResult)
						{
							// all ok, return 'stable' to avc_main and avc_main will send the response
							hResult = E_PKT_AVC_STABLE;
						}
						break;
					}


			// ctype SPECIFIC INQUIRY
			case CTYPE_ATTRIBUTE (AVC_CTYPE_SPECIFIC_INQUIRY, AVC_FB_CTRL_ATTRIBUTE_DELTA):
			case CTYPE_ATTRIBUTE (AVC_CTYPE_SPECIFIC_INQUIRY, AVC_FB_CTRL_ATTRIBUTE_CURRENT):
					hResult = E_PKT_AVC_IMPLEMENTED;
					break;

			default:
					hResult = E_PKT_AVC_NOT_IMPLEMENTED;
					break;
		}
	}

	return(hResult);
}
/*************************/

static HRESULT muteControlHandler(DataStream *pAvcStream, FUNCTION_BLOCK_PARMS *pFbParms, uint32 audioChannel, PB *packetBlock)
{
	uint8	muteValue=0;
	uint32	fbIDOperand = 4;
	uint8	fbID = 0;
	uint32	streamPosMute;	// save stream position in case its a status command
	HRESULT	hResult = NO_ERROR;

	streamPosMute = dsGetPosition(pAvcStream);

	if (AVC_AUDIO_CHANNEL_MASTER != audioChannel)
	{
		hResult = E_PKT_AVC_NOT_IMPLEMENTED;		// only one mute is supported, mutes all channels
	}
	if (NO_ERROR == hResult)
	{
		dsSetPosition(pAvcStream, fbIDOperand);
		hResult = dsRead8Bits (pAvcStream, &fbID);
		if (fbID != 1)
		{
			hResult = E_PKT_AVC_NOT_IMPLEMENTED;
		}
	}
			
	if (NO_ERROR == hResult)
	{
		/* Switch depending on command type and control attribute.
			General inquiry CTYPE has been handled at the highest level, so
			we can ignore that one. */

		switch (pFbParms->ctype_attribute)
		{
			// ctype GENERAL INQUIRY has been handled already at some higher level

			// ctype NOTIFY is supported for Current only
			case CTYPE_ATTRIBUTE (AVC_CTYPE_NOTIFY, AVC_FB_CTRL_ATTRIBUTE_CURRENT):
					// wants to be 'notified' when current delay changes
					hResult = postNotifyMute(audioChannel, pFbParms->fbId, packetBlock);
					if (NO_ERROR == hResult)
					{
						hResult = E_PKT_AVC_INTERIM;
					}
					break;

			// ctype CONTROL
			case CTYPE_ATTRIBUTE (AVC_CTYPE_CONTROL, AVC_FB_CTRL_ATTRIBUTE_CURRENT):
					// getp the mute parameter
					dsSetPosition(pAvcStream, fbIDOperand+6);
					hResult = dsRead8Bits (pAvcStream, &muteValue);
					SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "mute value: %x\n\r", muteValue);
					if (AVC_AUDIO_MUTE_ON == muteValue)
					{
						avsRxMute(0, TRUE);
						hResult = avrDrdSetMute (TRUE);
					}
					else if (AVC_AUDIO_MUTE_OFF == muteValue)
					{
						avsRxMute(0, FALSE);
						hResult = avrDrdSetMute (FALSE); 
					}
					if (NO_ERROR == hResult)
					{
						// all ok, return 'accepted' to avc_main and avc_main will send the response
						hResult = E_PKT_AVC_ACCEPTED;
					}
					break;

			// ctype STATUS
			case CTYPE_ATTRIBUTE (AVC_CTYPE_STATUS, AVC_FB_CTRL_ATTRIBUTE_CURRENT):
					dsSetPosition(pAvcStream, streamPosMute);
					hResult = dsSwitchMode(pAvcStream, dsMODE_WRITE);
					if (NO_ERROR == hResult)
					{
						if (avrDrdHostState.mute)
						{
						 	hResult = dsWrite8Bits (pAvcStream, AVC_AUDIO_MUTE_ON);
						}
						else
						{
						 	hResult = dsWrite8Bits (pAvcStream, AVC_AUDIO_MUTE_OFF);
						}
					}
					if (NO_ERROR == hResult)
					{
						// all ok, return 'stable' to avc_main and avc_main will send the response
						hResult = E_PKT_AVC_STABLE;
					}
					break;

			// ctype SPECIFIC INQUIRY
			case CTYPE_ATTRIBUTE (AVC_CTYPE_SPECIFIC_INQUIRY, AVC_FB_CTRL_ATTRIBUTE_CURRENT):
					hResult = E_PKT_AVC_IMPLEMENTED;
					break;

			default:
					hResult = E_PKT_AVC_NOT_IMPLEMENTED;
					break;
		}
	}

	return(hResult);
}

/*****************************************************************************
 
	selectorFbHandler
	featureFbHandler
	processingFbHandler
	codecFbHandler

		These functions each handle commands for their specific type of function 
		block.  Some general rules apply:
			- if function returns NO_ERROR, the function is responsible for sending
				a response packet (via avcSendRequest) and for disposing of the 
				packetBlock.
			- if the function returns anything other than NO_ERROR, the avc response
				and packetBlock disposal will be performed for it.

		Parameters:
			pFbParms - 		pointer to function block parameters
			pAvcStream -	pointer to the stream, current context is at Operand[3], 
								selector length. 
			pPacketBlock -	pointer to the command packetBlock


******************************************************************************/

/*****************************************************************************
 
	selectorFbHandler
	
******************************************************************************/

static HRESULT selectorFbHandler(DataStream 			*pAvcStream, 
								 FUNCTION_BLOCK_PARMS	*pFbParms, 
								 AVC_HEADER				*avcHeaderInfo,
								 PB						*pPacketBlock)
{
	pAvcStream = 	pAvcStream;			// placate compiler
	pFbParms = 		pFbParms;				// placate compiler
	avcHeaderInfo = avcHeaderInfo;			// placate compiler
	pPacketBlock = 	pPacketBlock;			// placate compiler

	return(E_PKT_AVC_NOT_IMPLEMENTED);
}
/*************************/

static HRESULT featureFbHandler(DataStream 				*pAvcStream, 
								FUNCTION_BLOCK_PARMS	*pFbParms,
								AVC_HEADER				*avcHeaderInfo,
								PB						*pPacketBlock)
{
	HRESULT	hResult = NO_ERROR;
	uint8	featureBlockLength;	// Operand[3]
	uint8	audioChannel=0; 			// Operand[4]
	uint8	controlSelector=0;		// Operand[5]
	uint8	controlLength=0;			// Operand[6]

	UNUSED_ARG(avcHeaderInfo);
	UNUSED_ARG(pPacketBlock);

	/* trap common operands 3, 4, 5, and 6 */

	hResult = dsRead8Bits (pAvcStream, &featureBlockLength);
	if (NO_ERROR == hResult)
	{
		hResult = dsRead8Bits (pAvcStream, &audioChannel);
		if (NO_ERROR == hResult)
		{
			hResult = dsRead8Bits (pAvcStream, &controlSelector);
			if (NO_ERROR == hResult)
			{
				hResult = dsRead8Bits (pAvcStream, &controlLength);
			}
		}
	}

	if (NO_ERROR == hResult)
	{
		if (2 != featureBlockLength)
		{
			// operand 3 is always 2 for a feature function block, section 11.3
			sysDebugPrintf("avcAudio, feature function block - bad length %d\n\r", featureBlockLength);
			hResult = E_PKT_AVC_NOT_IMPLEMENTED;
		}
		else if(!AudioChannelIsValid(audioChannel))
		{
			sysDebugPrintf("avcAudio, invalid audio channel %d\n\r", audioChannel);
			hResult = E_PKT_AVC_NOT_IMPLEMENTED;
		}
	}

	if (hResult == NO_ERROR)
	{

		/* Now we diverge again, depending on which control is being commanded - 
			datastream is now at start of operand[7] */

		switch (controlSelector)
		{
			case AVC_AUDIO_VOLUME_CONTROL:
					if (2 != controlLength)
					{
						sysDebugPrintf("avcAudio, volume control - bad control length %d\n\r", controlLength);
						hResult = E_PKT_AVC_NOT_IMPLEMENTED;
					}
					else
					{
						hResult = volumeControlHandler(pAvcStream, pFbParms, audioChannel, pPacketBlock);
					}
					break;

			case AVC_AUDIO_MUTE_CONTROL:
					if (1 != controlLength)
					{
						sysDebugPrintf("avcAudio, mute control - bad control length %d\n\r", controlLength);
						hResult = E_PKT_AVC_NOT_IMPLEMENTED;
					}
					else
					{
						hResult = muteControlHandler(pAvcStream, pFbParms, audioChannel, pPacketBlock);
					}
					break;

			case AVC_AUDIO_DELAY_CONTROL:
					if (2 != controlLength)
					{
						sysDebugPrintf("avcAudio, delay control - bad control length %d\n\r", controlLength);
						hResult = E_PKT_AVC_NOT_IMPLEMENTED;
					}
					else
					{
						hResult = delayControlHandler(pAvcStream, pFbParms, audioChannel, pPacketBlock);
					}
					break;


			default:
					hResult = E_PKT_AVC_NOT_IMPLEMENTED;
					break;
					;
		}  // switch (controlSelector)
	}

	return hResult;
}
/*************************/

static HRESULT processingFbHandler(DataStream 				*pAvcStream, 
								   FUNCTION_BLOCK_PARMS		*pFbParms, 
								   AVC_HEADER				*avcHeaderInfo,
								   PB						*pPacketBlock)
{
	UNUSED_ARG(pAvcStream);
	UNUSED_ARG(pFbParms);
	UNUSED_ARG(avcHeaderInfo);
	UNUSED_ARG(pPacketBlock);

	return E_PKT_AVC_NOT_IMPLEMENTED;
}
/*************************/

static HRESULT codecFbHandler(DataStream 			*pAvcStream, 
							  FUNCTION_BLOCK_PARMS	*pFbParms, 
							  AVC_HEADER		*avcHeaderInfo,
							  PB					*pPacketBlock)
{
	UNUSED_ARG(pAvcStream);
	UNUSED_ARG(pFbParms);
	UNUSED_ARG(avcHeaderInfo);
	UNUSED_ARG(pPacketBlock);

	return E_PKT_AVC_NOT_IMPLEMENTED;
}

/*************************/
/* Handle the AV/C function block command
*/
static HRESULT functionBlockHandler(DataStream			*pAvcStream,
									AVC_HEADER			*pAvcHeaderInfo,
									PB					*pPacketBlock)
{
	HRESULT					hResult = NO_ERROR;
	FUNCTION_BLOCK_PARMS	fbParms;

	/* Before peeling the onion some more, check the command type.
			general inquiry can be executed right here. */
	if (AVC_CTYPE_GENERAL_INQUIRY == pAvcHeaderInfo->ctype)
	{
		hResult = (AVC_RESPONSE_IMPLEMENTED);
		goto functionBlockExit;
	}

	/* parse common function block parameters.
		Unfortunately, fb type and id are defined by the spec as possibly being extended,
		so we need help with these. */

	hResult = avcDecodeFunctionBlockType (pAvcStream, &fbParms.fbType);		// operand[0]
	if (NO_ERROR == hResult)
	{
		hResult = avcDecodeFunctionBlockID (pAvcStream, &fbParms.fbId);		// operand[1]
	}
	if (NO_ERROR == hResult)
	{
		hResult = dsRead8Bits (pAvcStream, &fbParms.fbAttribute);			// operand[2]
	}
	if (NO_ERROR == hResult)
	{
		// verify that function block in fact does exist
		if (!functionBlockExists(fbParms.fbType, fbParms.fbId))
		{
			hResult = E_PKT_AVC_REJECTED;
		}
	}

	if (NO_ERROR != hResult)
	{
		goto functionBlockExit;
	}

	/* combine ctype and attribute into a single 16-bit value */
	//KBJ??? ASSERT(256 > pAvcHeaderInfo->ctype);		// ctype must be 8-bits max
	fbParms.ctype_attribute = (CTYPE_ATTRIBUTE)(CTYPE_ATTRIBUTE(pAvcHeaderInfo->ctype, fbParms.fbAttribute));

	/* Now we've parsed up to Operand[3].  At this point, handling diverges depending
		on the function block type. */

	switch (fbParms.fbType)
	{
		case AVC_FB_TYPE_SELECTOR:
				hResult = selectorFbHandler(pAvcStream, &fbParms, pAvcHeaderInfo, pPacketBlock);
				break;
				
		case AVC_FB_TYPE_FEATURE:
				hResult = featureFbHandler(pAvcStream, &fbParms, pAvcHeaderInfo, pPacketBlock);
				break;
				
		case AVC_FB_TYPE_PROCESSING:
				hResult = processingFbHandler(pAvcStream, &fbParms, pAvcHeaderInfo, pPacketBlock);
				break;
				
		case AVC_FB_TYPE_CODEC:
				hResult = codecFbHandler(pAvcStream, &fbParms, pAvcHeaderInfo, pPacketBlock);
				break;
				
		case AVC_FB_TYPE_SUBUNIT_DEST_PLUG:
		case AVC_FB_TYPE_SUBUNIT_SOURCE_PLUG:
				hResult = E_PKT_AVC_NOT_IMPLEMENTED;
				break;
		default:
				hResult = E_PKT_AVC_NOT_IMPLEMENTED;
				break;
	}

functionBlockExit:

	return(hResult);
}		

/*************************/
/* Handle the AV/C function block command
*/
static HRESULT changeConfigurationHandler(DataStream	*pAvcStream,
										  AVC_HEADER	*pAvcHeaderInfo,
										  PB			*pPacketBlock)
{
	uint8	configIdLsb=0;
	uint8	configIdMsb;
	uint32	configId;
	uint32	streamConfigIdPosition;
	HRESULT	hResult = NO_ERROR;

	UNUSED_ARG(pPacketBlock);

	// stream is positioned at operand[0], the config id msb		
	streamConfigIdPosition = dsGetPosition(pAvcStream);

	// extract the config id
	hResult = dsRead8Bits(pAvcStream, &configIdMsb);
	if (NO_ERROR == hResult)
	{
		hResult = dsRead8Bits(pAvcStream, &configIdLsb);
	}
	if (NO_ERROR != hResult)
	{
		return(hResult);
	}
	configId = (configIdMsb << 8) + configIdLsb;
	
	switch (pAvcHeaderInfo->ctype)
	{
		case AVC_CTYPE_CONTROL:
				// tell host to change its mode
				hResult = avrDrdSetAudioMode (configId);
				if (NO_ERROR == hResult)
				{
					// all ok, return 'accepted' to avc_main and avc_main will send the response
					hResult = E_PKT_AVC_ACCEPTED;
				}
				break;
		case AVC_CTYPE_STATUS:
				// send a response with the current configuration
				configId = avrDrdHostState.audioMode;
				configIdMsb = (uint8)((configId >> 8) & 0xFF);
				configIdLsb = (uint8)(configId & 0xFF);

				// repostion stream at the config id location
				dsSetPosition(pAvcStream, streamConfigIdPosition);
				hResult = dsSwitchMode(pAvcStream, dsMODE_WRITE);
				if (NO_ERROR == hResult)
				{
					hResult = dsWrite8Bits(pAvcStream, configIdMsb);
					if (NO_ERROR == hResult)
					{
						hResult = dsWrite8Bits(pAvcStream, configIdLsb);
					}
				}
				if (NO_ERROR == hResult)
				{
					// all ok, return 'stable' to avc_main and avc_main will send the response
					hResult = E_PKT_AVC_STABLE;
				}
				break;

		case AVC_CTYPE_NOTIFY:
				hResult = AVC_RESPONSE_NOT_IMPLEMENTED;
				break;

		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
				hResult = AVC_RESPONSE_IMPLEMENTED;
				break;
		default:
				hResult = AVC_RESPONSE_NOT_IMPLEMENTED;
				break;
	}

	return(hResult);
}
	
/*****************************************************************************
 
	Bus reset callback

	We get called from low-level when a bus reset has completed.  We use this
	to flush any and all pending 'notify' requests.
*/
/*
static void busResetCompleteCallback(void)
{
	memset(&muteNotifyInfo, 0, sizeof(muteNotifyInfo));
	memset(&delayNotifyInfo, 0, sizeof(delayNotifyInfo));
	memset(&volumeNotifyInfo, 0, sizeof(volumeNotifyInfo));
}
*/
/*****************************************************************************
 
	AVR Host event callback

	We get called from avr drd on host events.  We're only interested in some events and
	then only if a corresponding 'notify' request is pending
*/
/*
static void avrDrdEventCallback(AVR_EVENT_TYPE event, uint32 audioChannel)
{
	switch (event)
	{
		case AVR_EV_MASTER_VOLUME:
				drdMasterVolumeCallback();
				break;
		case AVR_EV_TRIM_VOLUME:
				drdTrimVolumeCallback(audioChannel);
				break;
		case AVR_EV_MUTE:
				drdMuteCallback();
				break;
		case AVR_EV_AUDIO_DELAY:
				drdDelayCallback(audioChannel);
				break;
	}
}
*/
/*****************************************************************************
 
   avcAudioRxPacketCallback
 
		This function gets called when a packet is received with has us as its
		subunit type and id.

******************************************************************************/
static HRESULT	avcAudioRxPacketCallback (AVC_HEADER	*avcHeaderInfo,
										  PB 			*pPacketBlock)
{
	HRESULT				hResult;
	DataStream			*pAvcStream;
		
	//KBJ??? ASSERT (AVC_SU_TYPE_AUDIO == avcHeaderInfo.subunittype);
	//KBJ??? ASSERT (NUM_AUDIO_SUBUNIT_INSTANCES > avcHeaderInfo.subunitid);

	/* get our packet stream - stream has already been advanced to operand[0] */

	hResult = pbGetApplicationDatastream (pPacketBlock, &pAvcStream);	
	if (NO_ERROR == hResult)
	{
		/* We only receive commands, we never send them so we should never receive response codes */
		if (!avcCtypeIsCommand(avcHeaderInfo->ctype))
		{
			/* We don't want to send response to a response, so we will return NO_ERROR.  On the other
				hand, it is an error so do report the error */
			hResult = E_AVC_UNEXPECTED_RESPONSE;
			sysLogError(hResult, __LINE__, moduleName);
			pbPacketDone(pPacketBlock, 8);
			hResult = NO_ERROR;
		}
		else
		{
			/* see what is being commanded of us */
			switch (avcHeaderInfo->opcode)
			{
				case AVC_CMD_PLUG_INFO:
						hResult = avcCmdPlugInfo(avcHeaderInfo, pPacketBlock, pAvcStream);
						break;
				case AVC_CMD_STREAM_FMT_EXT_INFO:
						hResult = avcCmdStreamFmtExtInfo(avcHeaderInfo, pPacketBlock, pAvcStream);
						break;
				case AVC_CMD_STREAM_FMT_SUPPORT:
						hResult = avcCmdStreamFmtSupport(avcHeaderInfo, pPacketBlock, pAvcStream, AVC_SU_TYPE_AUDIO);
						break;
				case AVC_CMD_CHANGE_CONFIGURATION:
						hResult = changeConfigurationHandler(pAvcStream, avcHeaderInfo, pPacketBlock);
						break;
				case AVC_CMD_FUNCTION_BLOCK:
						hResult = functionBlockHandler(pAvcStream, avcHeaderInfo, pPacketBlock);
						break;
				default:
						hResult = AVC_RESPONSE_NOT_IMPLEMENTED;
						break;
			}
		}
	}

	switch (hResult)
	{
		case E_PKT_AVC_ACCEPTED:
		case E_PKT_AVC_IMPLEMENTED:
		case E_PKT_AVC_NOT_IMPLEMENTED:
		case E_PKT_AVC_REJECTED:
		case E_PKT_AVC_INTERIM:
		case E_PKT_AVC_STABLE:
				/* These return values cause avc_main (who called us) to send the
					respective AV/C response. */
				break;
		case NO_ERROR:
				/* This return value tells avc_main that we have handled the command and
					sent the response.  */
				break;
		default:
				/* Anything else is some sort of error - log the error and reject the command. */
				sysLogError(hResult, __LINE__, moduleName);
				hResult = E_PKT_AVC_REJECTED;
				break;
	}
	return(hResult);
}

/*****************************************************************************
 
   avcAudioInitialize
 
		Since this module is not a thread, it needs an opportunity to initialize
		itself, namely to register a callback with the avcCommand handler.

******************************************************************************/
HRESULT avcAudioInitialize (void)
{
	int32	instance;
	HRESULT	hResult = NO_ERROR;

//	sysDebugSetEnabled(0x8200);

	for (instance = 0; (instance < NUM_AUDIO_SUBUNIT_INSTANCES) && (NO_ERROR == hResult); ++instance)
	{
		hResult = crAddCallback(AVC_AUDIO_CALLBACK);
		
		if (NO_ERROR != hResult)
		{
			sysLogError(hResult, __LINE__, moduleName);
		}
	}

	// set up descriptor manager with audio s/u descriptors
	if (NO_ERROR == hResult)
	{
		hResult = createAudioDescriptors();
	}

#if 0
	// register with bus reset event - we need to flush notify's on bus reset
	if (NO_ERROR == hResult)
	{
		hResult = lalRegisterBRCompletionCallback(busResetCompleteCallback);
	}

	// register with drd host events - event may satisfy a pending notify
	if (NO_ERROR == hResult)
	{
		hResult = crAddCallback(avrDrdHostEventCBDescriptor);
	}
#endif // 0 testtest

	return(hResult);
}
#endif //_AVC && _EDS && _DRD
