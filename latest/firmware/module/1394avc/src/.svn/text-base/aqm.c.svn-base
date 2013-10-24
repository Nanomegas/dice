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
	Declares the application queue manager function.
	These functions perform queue get and send functions and additionally
    are bus reset aware, suspending the calling thread until any occuring
    bus reset is complete.

	These functions are very similar to TCMsgQueueSend and TCMsgQueueReceive
	The calling thread will be suspended if a bus reset is occuring

  Revisions:
		created 21/03/2003 KBJ/LM

****************************************************************************/

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "ErrorCodes.h"
#include "lal.h"
#include "aqm.h"

HRESULT aqmQueueReceive(uint32 queueID, void *pDst, uint32 msec)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bResetDetected = FALSE;

	hResult = lalWaitOnResetCompletion (&bResetDetected);
	if (hResult != NO_ERROR) return hResult;

	hResult = TCMsgQueueReceive (queueID, pDst, msec);

	if (hResult != NO_ERROR)
	{
		if (hResult == E_MSGQUEUE_TIMEOUT)
		{
			SYS_DEBUG(SYSDEBUG_TRACE_AVC | SYSDEBUG_TRACE_AKE,
						"aqmQueueReceive: queueID:%i timed out\n\r", queueID);
		}
		else
		{
			sysLogError(hResult, __LINE__, moduleName);
		}
	}

	return hResult;
}

HRESULT aqmQueueSend(uint32 queueID, void *pSrc, uint32 msec)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bResetDetected = FALSE;

	hResult = lalWaitOnResetCompletion (&bResetDetected);
	if (hResult != NO_ERROR) return hResult;

	hResult = TCMsgQueueSend(queueID, pSrc, msec);

	if (hResult != NO_ERROR)
	{
		sysLogError(hResult, __LINE__, moduleName);
	}

	return hResult;
}
