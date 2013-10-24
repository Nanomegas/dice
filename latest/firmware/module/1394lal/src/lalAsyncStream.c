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
 * DESCRIPTION: Link Abstraction Layer Interfaces Async Streaming

	Purpose:	Link Abstraction Layer 
				Async Streaming (Loose Isoch)
	
	Revisions:
		created 04/22/2002 lm
****************************************************************************/

#include "TCTasking.h"
#include "lal.h"
#include "lhlLLC.h"
#include "lhlMsgQueue.h"
#include "lhlReq.h"

#ifdef _LOOSE_ISO

static char 		moduleName[] = __FILE__ ;

BOOL lalAsyncStreamEnabled = FALSE;

LAL_STREAM_DISPATCH_LIST	lalAsyncStreamChannels = {0};

/* Async stream control */

HRESULT	lalAsyncStreamInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

	memset(&lalAsyncStreamChannels, 0, sizeof(lalAsyncStreamChannels));

	return hResult;
}

HRESULT lalAsyncStreamEnable(void)
{
	HRESULT		hResult = NO_ERROR;
	
	lalAsyncStreamEnabled = TRUE;

	hResult = lhlLLCSetAsyncStreamEnabled(lalAsyncStreamEnabled);

	return hResult;
}

HRESULT lalAsyncStreamDisable(void)
{
	HRESULT		hResult = NO_ERROR;

	lalAsyncStreamEnabled = FALSE;

	hResult = lhlLLCSetAsyncStreamEnabled(lalAsyncStreamEnabled);

	return hResult;
}

HRESULT lalAsyncStreamEnableChannel(uint8 channel)
{
	HRESULT		hResult = NO_ERROR;

	hResult = lhlLLCSetAsyncStreamChannel(channel, TRUE);
	
	return hResult;
}

HRESULT lalAsyncStreamDisableChannel(uint8 channel)
{
	HRESULT		hResult = NO_ERROR;

	hResult = lhlLLCSetAsyncStreamChannel(channel, FALSE);

	return hResult;
}

BOOL lalAsyncStreamIsEnabled(void)
{
	return lalAsyncStreamEnabled;
}

HRESULT lalAsyncStreamRegisterChannelCB(uint8 channel, LAL_CALLBACK callback)
{
	HRESULT		hResult = NO_ERROR;

	if (channel > 63)
	{
		hResult = E_LAL_INVALID_ADDR_RNG;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (lalAsyncStreamChannels.callback[channel] != 0)
	{
		hResult = E_LAL_ADDR_RNG_CONFLICT;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	lalAsyncStreamChannels.callback[channel] = callback;

	return hResult;
}

BOOL lalAsyncStreamLookupChannelCB(uint8 channel, LAL_CALLBACK *callback)
{
	BOOL	bFound = FALSE;

	// return the callback
	*callback = lalAsyncStreamChannels.callback[channel];
	if (*callback != NULL)
	{
		bFound = TRUE;
	}

	return bFound;
}

HRESULT lalAsyncStreamSendData(uint8 channel, uint8 tag, uint8 sy, uint16 bytelength, uint32* bufPtr)
{
	HRESULT		hResult = NO_ERROR;

	if (channel > 63)
	{
		return E_LAL_CHANNEL_INVALID;
	}

    hResult = lhlAsyncStreamSendData(channel, tag, sy, bytelength, bufPtr, LHL_QUEUE_GENERAL);
    
    return hResult;	
}
#endif //LOOSE_ISO
