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

	Implementation of avc unit connect notify command

  Revisions:
		created 08/04/2003 LM

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "avc.h"
#include "avcUnitCli.h"
#include "avcUnitConnect.h"
#include "avcUnitNotify.h"
#include "cr.h"
#include "listmanager.h"
#include "systemcallbacks.h"

static	BOOL			avcUnitConnectInitialized = FALSE;

#define CONNECT_MAX_NOTIFY_ITEMS		12

static BYTE				connectNotifyList_BUFFER[LM_LISTSIZE(CONNECT_MAX_NOTIFY_ITEMS, sizeof(CONNECT_NOTIFY))];		// Used to keep track of thread control blocks for those
static LM_CONTEXT*		connectNotifyList = (LM_CONTEXT *) connectNotifyList_BUFFER;

LM_CONTEXT*	avcUnitConnectGetNotifyList(void)
{
	return connectNotifyList;
}

HRESULT avcUnitConnectStateChanged(void)
{
	return avcUnitNotifyChange(avcUnitConnectNotifyCheck);
}

HRESULT avcUnitConnectNotifyAdd(PB *packetBlock, AVC_CONNECTION *command, AVC_CONNECTION *connection)
{
	HRESULT			hResult = NO_ERROR;
	CONNECT_NOTIFY	notify;
				
	memset(&notify, 0, sizeof(notify));
	avcSubunitCopyConnection(&notify.data.command, command);
	avcSubunitCopyConnection(&notify.data.connection, connection);

	hResult = avcUnitNotifyAdd(connectNotifyList, (UNION_NOTIFY *) &notify, packetBlock, TRUE);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}
				
static HRESULT avcUnitConnectNotifyCheckCB(UNION_NOTIFY *notify, BOOL *bChanged)
{
	HRESULT			hResult = NO_ERROR;
	CONNECT_NOTIFY*	connectNotify = (CONNECT_NOTIFY *) notify;
	AVC_CONNECTION	connection;

	avcSubunitCopyConnection(&connection, &connectNotify->data.command);	// make a copy of connect command
	avcSubunitFindPlug(&connection);										// Overwrites connection with the missing fields

	*bChanged = (avcSubunitConnectionsEqual(&connection, &connectNotify->data.connection) == FALSE);

	return hResult;
}

static HRESULT avcUnitConnectNotifyUpdateCB(UNION_NOTIFY *notify, pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;
	CONNECT_NOTIFY*	connectNotify = (CONNECT_NOTIFY *) notify;
	AVC_CONNECTION	connection;

	avcSubunitCopyConnection(&connection, &connectNotify->data.command);	// make a copy of connect command
	avcSubunitFindPlug(&connection);										// Overwrites connection with the missing fields

	hResult = avcSubunitEncodeConnection(pStream, &connection);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcUnitConnectNotifyCheck(void)
{
	HRESULT		hResult = NO_ERROR;

	if (avcUnitConnectInitialized == TRUE)
	{
		// wait around until an element has actually been added to the list.
		if (connectNotifyList->allocated > 0) 
		{
			hResult = avcUnitNotifyCheck(connectNotifyList, avcUnitConnectNotifyCheckCB, avcUnitConnectNotifyUpdateCB);
		}
	}		
	
	return hResult;
}

/*********************************************************
	Initialization of this module
*/
HRESULT avcUnitConnectInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = lmCreateList(connectNotifyList, LM_LISTSIZE(CONNECT_MAX_NOTIFY_ITEMS, sizeof(CONNECT_NOTIFY)), sizeof(CONNECT_NOTIFY),NULL);
	if (hResult != NO_ERROR) return hResult;
	
	avcUnitConnectInitialized = TRUE;
	
	return hResult;
}
