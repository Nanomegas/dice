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

	Implementation of avc unit input and output plug signal format notify command

  Revisions:
		created 08/04/2003 LM

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "avc.h"
#include "avcUnitCli.h"
#include "avcUnitSignalFormat.h"
#include "avcUnitNotify.h"
#include "cmp.h"
#include "cr.h"
#include "listmanager.h"
#include "systemcallbacks.h"

static	BOOL			avcUnitSignalFormatInitialized = FALSE;

#define SIGNAL_FORMAT_MAX_NOTIFY_ITEMS		12

static BYTE				signalFormatNotifyListIn_BUFFER[LM_LISTSIZE(SIGNAL_FORMAT_MAX_NOTIFY_ITEMS, sizeof(SIGNAL_FORMAT_NOTIFY))];		// Used to keep track of thread control blocks for those
static LM_CONTEXT*		signalFormatNotifyListIn = (LM_CONTEXT *) signalFormatNotifyListIn_BUFFER;

static BYTE				signalFormatNotifyListOut_BUFFER[LM_LISTSIZE(SIGNAL_FORMAT_MAX_NOTIFY_ITEMS, sizeof(SIGNAL_FORMAT_NOTIFY))];		// Used to keep track of thread control blocks for those
static LM_CONTEXT*		signalFormatNotifyListOut = (LM_CONTEXT *) signalFormatNotifyListOut_BUFFER;

LM_CONTEXT*	avcUnitSignalFormatGetNotifyList(BOOL bIn)
{
	if (bIn)	return signalFormatNotifyListIn;
	else		return signalFormatNotifyListOut;
}

HRESULT avcUnitInputPlugSignalFormatStateChanged(void)
{
	return avcUnitNotifyChange(avcUnitInputPlugSignalFormatNotifyCheck);
}

HRESULT avcUnitOutputPlugSignalFormatStateChanged(void)
{
	return avcUnitNotifyChange(avcUnitOutputPlugSignalFormatNotifyCheck);
}

HRESULT	avcUnitSignalFormatNotifyAdd(PB *packetBlock, BOOL bIn, BYTE plugID, uint32 format)
{
	HRESULT					hResult = NO_ERROR;
	SIGNAL_FORMAT_NOTIFY	notify;
	LM_CONTEXT*				signalFormatNotifyList = NULL;
	CMP_SIGNAL_FORMAT_CB	callback = NULL;
	
	signalFormatNotifyList = avcUnitSignalFormatGetNotifyList(bIn);
				
	memset(&notify, 0, sizeof(notify));

	notify.data.bIn = bIn;
	notify.data.plugID = plugID;
	notify.data.format = format;

	hResult = avcUnitNotifyAdd(signalFormatNotifyList, (UNION_NOTIFY *) &notify, packetBlock, TRUE);
	if (hResult != NO_ERROR) return hResult;

	callback = (bIn) ? avcUnitInputPlugSignalFormatStateChanged : avcUnitOutputPlugSignalFormatStateChanged;

	hResult = cmpRegisterSignalFormatCB (bIn, plugID, format, callback);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}
				
static HRESULT avcUnitSignalFormatNotifyCheckCB(UNION_NOTIFY *notify, BOOL *bChanged)
{
	HRESULT					hResult = NO_ERROR;
	SIGNAL_FORMAT_NOTIFY*	signalFormat = (SIGNAL_FORMAT_NOTIFY *) notify;
	uint32					format = 0;

	if (signalFormat->data.bIn)
	{
		hResult = cmpGetiPCRFormat(signalFormat->data.plugID, &format);
		if (hResult != NO_ERROR) return hResult;
	}
	else
	{
		hResult = cmpGetoPCRFormat(signalFormat->data.plugID, &format);
		if (hResult != NO_ERROR) return hResult;
	}

	*bChanged = (format != signalFormat->data.format);

	if (*bChanged)
	{
		cmpUnregisterSignalFormatCB (signalFormat->data.bIn, signalFormat->data.plugID);
	}

	return hResult;
}

static HRESULT avcUnitSignalFormatNotifyUpdateCB(UNION_NOTIFY *notify, pDataStream pStream)
{
	HRESULT					hResult = NO_ERROR;
	SIGNAL_FORMAT_NOTIFY*	signalFormat = (SIGNAL_FORMAT_NOTIFY *) notify;
	uint32					format = 0;

	if (signalFormat->data.bIn)
	{
		hResult = cmpGetiPCRFormat(signalFormat->data.plugID, &format);
		if (hResult != NO_ERROR) return hResult;
	}
	else
	{
		hResult = cmpGetoPCRFormat(signalFormat->data.plugID, &format);
		if (hResult != NO_ERROR) return hResult;
	}

	hResult = dsWrite8Bits(pStream, signalFormat->data.plugID);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWrite32Bits(pStream, (format & 0x3FFFFFFF) | 0x80000000);	// eoh:1=1, form:1=0, (keeping fmt and fdf)
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcUnitSignalFormatNotifyCheck(BOOL bIn)
{
	HRESULT			hResult = NO_ERROR;
	LM_CONTEXT*		signalFormatNotifyList = NULL;
	
	signalFormatNotifyList = avcUnitSignalFormatGetNotifyList(bIn);

	if (avcUnitSignalFormatInitialized == TRUE)
	{
		// wait around until an element has actually been added to the list.
		if (signalFormatNotifyList->allocated > 0) 
		{
			hResult = avcUnitNotifyCheck(signalFormatNotifyList, avcUnitSignalFormatNotifyCheckCB, avcUnitSignalFormatNotifyUpdateCB);
		}
	}		
	
	return hResult;
}

HRESULT avcUnitInputPlugSignalFormatNotifyCheck(void)
{
	return avcUnitSignalFormatNotifyCheck(TRUE);
}

HRESULT avcUnitOutputPlugSignalFormatNotifyCheck(void)
{
	return avcUnitSignalFormatNotifyCheck(FALSE);
}

/*********************************************************
	Initialization of this module
*/
HRESULT avcUnitSignalFormatInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = lmCreateList(signalFormatNotifyListIn, LM_LISTSIZE(SIGNAL_FORMAT_MAX_NOTIFY_ITEMS, sizeof(SIGNAL_FORMAT_NOTIFY)), sizeof(SIGNAL_FORMAT_NOTIFY),NULL);
	if (hResult != NO_ERROR) return hResult;
	
	hResult = lmCreateList(signalFormatNotifyListOut, LM_LISTSIZE(SIGNAL_FORMAT_MAX_NOTIFY_ITEMS, sizeof(SIGNAL_FORMAT_NOTIFY)), sizeof(SIGNAL_FORMAT_NOTIFY),NULL);
	if (hResult != NO_ERROR) return hResult;

	avcUnitSignalFormatInitialized = TRUE;
	
	return hResult;
}
