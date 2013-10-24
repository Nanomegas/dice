//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd..
//
// (C) COPYRIGHT 2005 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
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

		Implement AV/C Music sub-unit

		This module is the 'main' for the music subunit.  Currently, this subunit
		executes av/c commands while running on the av/c command handler's thread.

		Basically, it registers a callback with the avc command handler.  When the
		callback is invoked, it executes the command (send command to host), sends
		response and returns to caller.  It does not need to block anywhere, and
		therefore does not need to be its own thread.
 
		PROBLEMS, ASSUMPTIONS and LIMITATIONS
			- this module handles a single instance, subunit id #0

  Revisions:
		created 11/21/2006 BK

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

#define NUM_MUSIC_SUBUNIT_INSTANCES		1

#include "musicDescriptor.c"	// contrary to convention, we are including a 'c' file

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

static HRESULT avcMusicRxPacketCallback(AVC_HEADER		*avcHeaderInfo,
										 PB 			*pPacketBlock);

//static HRESULT musicDSCustomResolver(DataStream			*ds,
//									 EDS_DESCRIPTOR_REF	*result);

/*****************************************************************************
 
	Descriptors

	Descriptor information for the music subunit is all static, never changing data.
	The data is generated automatically from the Rubric descriptor builder application.
	It contains an array of uint8 named "MusicSubunitStatusData"	
*/

// music subunit id data
static EDS_CLIENT_SESSION musicSession = 
{
	AVC_SU_TYPE_MUSIC, 		// subunit_type
	0,						// subunit_ID
	0,						// subunit_desc_ref
	NULL					// custom_resolver
};

static EDS_SUBUNITIDENTIFIER siMusicData = 
{
	2,							// uint8		size_of_list_id;
	0,							// uint8		size_of_object_id;
	2,							// uint8		size_of_entry_position;
	0,							// uint16		number_of_root_lists;
	0,							// uint16		si_length;
	0,							// uint16		si_infoblock_count;
	0,							// RANGEMAP*	si_writable_mask;
	(void *)MusicSubunitIDData,	// void*		subunit_information;
	0,							// uint16		mi_length;
	0,							// uint16		mi_infoblock_count;
	0,							// RANGEMAP*	mi_writable_mask;
	NULL,						// void*		manufacturer_information;
	0,							// uint16		extended_infoblock_count; 
};

// music subunit status desctiptor
static EDS_OPAQUEDESCRIPTOR musicStatus = 
{
	AVC_DS_SUBUNIT_STATUS_DESCRIPTOR,		// descriptor_type
	0,										// si_size
	&MusicSubunitStatusData,				// specific_info
	0										// extended_infoblock_count
};

static EDS_DESCRIPTOR_REF		opaque_ref = 0;

/*************************************************************************
	MUSIC CALLBACK DESCRIPTOR
	
	This defines the callback for the av/c handler.
*************************************************************************/

static CALLBACK_DESCRIPTOR	AVC_MUSIC_CALLBACK[]=
{
	{CALLBACK_DESCRIPTOR_TYPE,AVC_SUBUNIT_TYPE},
	{CALLBACK_DESCRIPTOR_NEXT,0},
	{CALLBACK_DESCRIPTOR_NAME,(uint32)"AVC_MUSIC_CALLBACK"},
	{AVC_SUBUNIT_CALLBACK_KEY,(uint32)&avcMusicRxPacketCallback}, // CALLBACK PTR of TYPE AVC_SUBUNIT_CALLBACK},
	{AVC_SUBUNIT_SUBUNITTYPE_KEY,AVC_SU_TYPE_MUSIC},
	{AVC_SUBUNIT_ID_KEY, 0},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


/*************************************************************************
	FUNCTIONS
*************************************************************************/
/*
	The Music subunit adds a new descriptor_type 0x80 Subunit Status Descriptor

	Using a custom resolver to handle host descriptor reads.
*/
HRESULT musicDSCustomResolver(DataStream*	ds, EDS_DESCRIPTOR_REF* result)
{
	*result = opaque_ref;

	return NO_ERROR;
}

static HRESULT createMusicDescriptors(void)
{
	HRESULT 				hResult = NO_ERROR;

	hResult = edsRegisterSubunitClientSession(&musicSession);

	if (NO_ERROR == hResult)
	{
		hResult = edsRegisterCustomResolver(&musicSession, 
								  &musicDSCustomResolver);
	}

	// music subunit id data
	if (NO_ERROR == hResult)
	{
		siMusicData.si_length = sizeof(MusicSubunitIDData);
		hResult = edsCreateSubunitIdentifier(&musicSession, &siMusicData);
	}
	
	// music subunit status data
	if (NO_ERROR == hResult)
	{
		musicStatus.si_size = sizeof(MusicSubunitStatusData);
		hResult = edsCreateOpaqueDescriptor(&musicSession, musicStatus.type, 
			musicStatus.si_size, musicStatus.specific_info,
			&opaque_ref);
	}
	if (NO_ERROR == hResult)
	{
		hResult = edsAssociateChildWithParent(musicSession.subunit_desc_ref,
											  opaque_ref,
											  EDS_BUFFERTYPE_EXTENDEDINFO);
	}

	return (hResult);
}

/*****************************************************************************
 
	Helper functions

******************************************************************************/


/*****************************************************************************
 
	Bus reset callback

	We get called from low-level when a bus reset has completed.  We use this
	to flush any and all pending 'notify' requests.
*/
static void busResetCompleteCallback(void)
{
	memset(&muteNotifyInfo, 0, sizeof(muteNotifyInfo));
	memset(&delayNotifyInfo, 0, sizeof(delayNotifyInfo));
	memset(&volumeNotifyInfo, 0, sizeof(volumeNotifyInfo));
}


/*****************************************************************************
 
   avcMusicRxPacketCallback
 
		This function gets called when a packet is received with has us as its
		subunit type and id.

******************************************************************************/
static HRESULT	avcMusicRxPacketCallback (AVC_HEADER	*avcHeaderInfo,
										  PB 			*pPacketBlock)
{
	HRESULT				hResult;
	DataStream			*pAvcStream;
		
	//KBJ??? ASSERT (AVC_SU_TYPE_MUSIC == avcHeaderInfo.subunittype);
	//KBJ??? ASSERT (NUM_MUSIC_SUBUNIT_INSTANCES > avcHeaderInfo.subunitid);

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
						hResult = avcCmdStreamFmtSupport(avcHeaderInfo, pPacketBlock, pAvcStream, AVC_SU_TYPE_MUSIC);
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
 
   avcMusicInitialize
 
		Since this module is not a thread, it needs an opportunity to initialize
		itself, namely to register a callback with the avcCommand handler.

******************************************************************************/
HRESULT avcMusicInitialize (void)
{
	int32	instance;
	HRESULT	hResult = NO_ERROR;


	for (instance = 0; (instance < NUM_MUSIC_SUBUNIT_INSTANCES) && (NO_ERROR == hResult); ++instance)
	{
		hResult = crAddCallback(AVC_MUSIC_CALLBACK);
		
		if (NO_ERROR != hResult)
		{
			sysLogError(hResult, __LINE__, moduleName);
		}
	}

	// set up descriptor manager with audio s/u descriptors
	if (NO_ERROR == hResult)
	{
		hResult = createMusicDescriptors();
	}

	// register with bus reset event - we need to flush notify's on bus reset
	if (NO_ERROR == hResult)
	{
		hResult = lalRegisterBRCompletionCallback(busResetCompleteCallback);
	}

	return(hResult);
}
#endif //_AVC && _EDS && _DRD
