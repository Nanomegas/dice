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

	Implementation of avc unit signal source notify command

  Revisions:
		created 08/04/2003 LM

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "avc.h"
#include "avcUnitCli.h"
#include "avcUnitSignalSource.h"
#include "avcUnitNotify.h"
#include "cr.h"
#include "listmanager.h"
#include "systemcallbacks.h"

static	BOOL			avcUnitSignalSourceInitialized = FALSE;

#define SIGNAL_SOURCE_MAX_NOTIFY_ITEMS		12

static BYTE				signalSourceNotifyList_BUFFER[LM_LISTSIZE(SIGNAL_SOURCE_MAX_NOTIFY_ITEMS, sizeof(SIGNAL_SOURCE_NOTIFY))];		// Used to keep track of thread control blocks for those
static LM_CONTEXT*		signalSourceNotifyList = (LM_CONTEXT *) signalSourceNotifyList_BUFFER;

LM_CONTEXT*	avcUnitSignalSourceGetNotifyList(void)
{
	return signalSourceNotifyList;
}

HRESULT avcUnitSignalSourceStateChanged(void)
{
	return avcUnitNotifyChange(avcUnitSignalSourceNotifyCheck);
}

HRESULT avcUnitSignalSourceNotifyAdd(PB *packetBlock, BYTE channel, uint16 nodeAddr, BYTE oPCR)
{
	HRESULT					hResult = NO_ERROR;
	SIGNAL_SOURCE_NOTIFY	notify;
				
	memset(&notify, 0, sizeof(notify));
	notify.data.channel = channel;
	notify.data.nodeAddr = nodeAddr;
	notify.data.oPCR = oPCR;

	hResult = avcUnitNotifyAdd(signalSourceNotifyList, (UNION_NOTIFY *) &notify, packetBlock, TRUE);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}
				
static HRESULT avcUnitSignalSourceNotifyCheckCB(UNION_NOTIFY *notify, BOOL *bChanged)
{
	HRESULT					hResult = NO_ERROR;
	SIGNAL_SOURCE_NOTIFY*	signalSourceNotify = (SIGNAL_SOURCE_NOTIFY *) notify;

	*bChanged = (avcSubunitChannelIsConnected(signalSourceNotify->data.channel, &signalSourceNotify->data.nodeAddr, &signalSourceNotify->data.oPCR) == FALSE);

	return hResult;
}

static HRESULT avcUnitSignalSourceNotifyUpdateCB(UNION_NOTIFY *notify, pDataStream pStream)
{
	HRESULT					hResult = NO_ERROR;
	SIGNAL_SOURCE_NOTIFY*	signalSourceNotify = (SIGNAL_SOURCE_NOTIFY *) notify;

	hResult = dsWrite8Bits(pStream, signalSourceNotify->data.channel);	// operand[0]: channel
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite16Bits(pStream, 0xFFFF);							// operand[1,..,2]: nodeAddr
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite8Bits(pStream, 0xFF);								// operand[3]: oPCR 
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcUnitSignalSourceNotifyCheck(void)
{
	HRESULT		hResult = NO_ERROR;

	if (avcUnitSignalSourceInitialized == TRUE)
	{
		// wait around until an element has actually been added to the list.
		if (signalSourceNotifyList->allocated > 0) 
		{
			hResult = avcUnitNotifyCheck(signalSourceNotifyList, avcUnitSignalSourceNotifyCheckCB, avcUnitSignalSourceNotifyUpdateCB);
		}
	}		

	return hResult;
}


/*********************************************************
	Initialization of this module
*/
HRESULT avcUnitSignalSourceInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = lmCreateList(signalSourceNotifyList, LM_LISTSIZE(SIGNAL_SOURCE_MAX_NOTIFY_ITEMS, sizeof(SIGNAL_SOURCE_NOTIFY)), sizeof(SIGNAL_SOURCE_NOTIFY),NULL);
	if (hResult != NO_ERROR) return hResult;
	
	avcUnitSignalSourceInitialized = TRUE;
	
	return hResult;
}
