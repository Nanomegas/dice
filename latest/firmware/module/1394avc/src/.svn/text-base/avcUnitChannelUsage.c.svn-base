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

	Implementation of avc unit channel usage notify command

  Revisions:
		created 08/04/2003 LM

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "avc.h"
#include "avcUnitCli.h"
#include "avcUnitChannelUsage.h"
#include "avcUnitNotify.h"
#include "cr.h"
#include "listmanager.h"
#include "systemcallbacks.h"
#include "cmp.h"

static	BOOL			avcUnitChannelUsageInitialized = FALSE;

#define CHANNEL_USAGE_MAX_NOTIFY_ITEMS		12

static BYTE				channelUsageNotifyList_BUFFER[LM_LISTSIZE(CHANNEL_USAGE_MAX_NOTIFY_ITEMS, sizeof(CHANNEL_USAGE_NOTIFY))];		// Used to keep track of thread control blocks for those
static LM_CONTEXT*		channelUsageNotifyList = (LM_CONTEXT *) channelUsageNotifyList_BUFFER;

LM_CONTEXT*	avcUnitChannelUsageGetNotifyList(void)
{
	return channelUsageNotifyList;
}

HRESULT avcUnitChannelUsageStateChanged(uint32 channel)
{
	UNUSED_ARG(channel);

	return avcUnitNotifyChange(avcUnitChannelUsageNotifyCheck);
}

HRESULT avcUnitChannelUsageNotifyAdd(PB *packetBlock, BYTE channel, uint16 nodeAddr, BYTE oPCRNumber)
{
	HRESULT					hResult = NO_ERROR;
	CHANNEL_USAGE_NOTIFY	notify;
				
	memset(&notify, 0, sizeof(notify));
	notify.data.channel = channel;
	notify.data.nodeAddr = nodeAddr;
	notify.data.oPCRNumber = oPCRNumber;

	hResult = avcUnitNotifyAdd(channelUsageNotifyList, (UNION_NOTIFY *) &notify, packetBlock, TRUE);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpRegisterChannelUsageCB (channel, avcUnitChannelUsageStateChanged);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}
				
static HRESULT avcUnitChannelUsageNotifyCheckCB(UNION_NOTIFY *notify, BOOL *bChanged)
{
	HRESULT					hResult = NO_ERROR;
	CHANNEL_USAGE_NOTIFY*	channelUsageNotify = (CHANNEL_USAGE_NOTIFY *) notify;

	*bChanged = (avcSubunitChannelIsConnected(channelUsageNotify->data.channel, &channelUsageNotify->data.nodeAddr, &channelUsageNotify->data.oPCRNumber) == FALSE);

	if (*bChanged)
	{
		cmpUnregisterChannelUsageCB (channelUsageNotify->data.channel);
	}

	return hResult;
}

static HRESULT avcUnitChannelUsageNotifyUpdateCB(UNION_NOTIFY *notify, pDataStream pStream)
{
	HRESULT					hResult = NO_ERROR;
	CHANNEL_USAGE_NOTIFY*	channelUsageNotify = (CHANNEL_USAGE_NOTIFY *) notify;

	hResult = dsWrite8Bits(pStream, channelUsageNotify->data.channel);	// operand[0]: channel
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite16Bits(pStream, 0xFFFF);							// operand[1,..,2]: nodeAddr
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite8Bits(pStream, 0xFF);								// operand[3]: oPCRNumber 
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcUnitChannelUsageNotifyCheck(void)
{
	HRESULT		hResult = NO_ERROR;

	if (avcUnitChannelUsageInitialized == TRUE)
	{
		// wait around until an element has actually been added to the list.
		if (channelUsageNotifyList->allocated > 0) 
		{
			hResult = avcUnitNotifyCheck(channelUsageNotifyList, avcUnitChannelUsageNotifyCheckCB, avcUnitChannelUsageNotifyUpdateCB);
		}
	}		

	return hResult;
}

/*********************************************************
	Initialization of this module
*/
HRESULT avcUnitChannelUsageInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = lmCreateList(channelUsageNotifyList, LM_LISTSIZE(CHANNEL_USAGE_MAX_NOTIFY_ITEMS, sizeof(CHANNEL_USAGE_NOTIFY)), sizeof(CHANNEL_USAGE_NOTIFY),NULL);
	if (hResult != NO_ERROR) return hResult;
	
	avcUnitChannelUsageInitialized = TRUE;
	
	return hResult;
}
