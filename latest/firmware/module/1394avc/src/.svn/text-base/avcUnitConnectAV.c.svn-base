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

	Implementation of avc unit ConnectAV notify command

  Revisions:
		created 08/04/2003 LM

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "avc.h"
#include "avcUnitCli.h"
#include "avcUnitConnectAV.h"
#include "avcUnitNotify.h"
#include "cr.h"
#include "listmanager.h"
#include "systemcallbacks.h"

static	BOOL			avcUnitConnectAVInitialized = FALSE;

#define CONNECT_AV_MAX_NOTIFY_ITEMS		12

static BYTE				connectAVNotifyList_BUFFER[LM_LISTSIZE(CONNECT_AV_MAX_NOTIFY_ITEMS, sizeof(CONNECT_AV_NOTIFY))];		// Used to keep track of thread control blocks for those
static LM_CONTEXT*		connectAVNotifyList = (LM_CONTEXT *) connectAVNotifyList_BUFFER;

LM_CONTEXT*	avcUnitConnectAVGetNotifyList(void)
{
	return connectAVNotifyList;
}

HRESULT avcUnitConnectAVStateChanged(void)
{
	return avcUnitNotifyChange(avcUnitConnectAVNotifyCheck);
}

HRESULT avcUnitConnectAVNotifyAdd(PB *packetBlock, uint32 dummy)
{
	HRESULT				hResult = NO_ERROR;
	CONNECT_AV_NOTIFY	notify;
				
	memset(&notify, 0, sizeof(notify));
	notify.data.dummy = dummy;

	hResult = avcUnitNotifyAdd(connectAVNotifyList, (UNION_NOTIFY *) &notify, packetBlock, TRUE);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}
				
static HRESULT avcUnitConnectAVNotifyCheckCB(UNION_NOTIFY *notify, BOOL *bChanged)
{
	HRESULT				hResult = NO_ERROR;
	CONNECT_AV_NOTIFY*	connectAVNotify = (CONNECT_AV_NOTIFY *) notify;

	*bChanged = (connectAVNotify->data.dummy);

	return hResult;
}

static HRESULT avcUnitConnectAVNotifyUpdateCB(UNION_NOTIFY *notify, pDataStream pStream)
{
	HRESULT				hResult = NO_ERROR;
	CONNECT_AV_NOTIFY*	connectAVNotify = (CONNECT_AV_NOTIFY *) notify;

	//LM???
	UNUSED_ARG(pStream);
	UNUSED_ARG(connectAVNotify);

	return hResult;
}

HRESULT avcUnitConnectAVNotifyCheck(void)
{
	HRESULT		hResult = NO_ERROR;

	if (avcUnitConnectAVInitialized == TRUE)
	{
		// wait around until an element has actually been added to the list.
		if (connectAVNotifyList->allocated > 0) 
		{
			hResult = avcUnitNotifyCheck(connectAVNotifyList, avcUnitConnectAVNotifyCheckCB, avcUnitConnectAVNotifyUpdateCB);
		}
	}		
	
	return hResult;
}

/*********************************************************
	Initialization of this module
*/
HRESULT avcUnitConnectAVInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = lmCreateList(connectAVNotifyList, LM_LISTSIZE(CONNECT_AV_MAX_NOTIFY_ITEMS, sizeof(CONNECT_AV_NOTIFY)), sizeof(CONNECT_AV_NOTIFY),NULL);
	if (hResult != NO_ERROR) return hResult;
	
	avcUnitConnectAVInitialized = TRUE;
	
	return hResult;
}
