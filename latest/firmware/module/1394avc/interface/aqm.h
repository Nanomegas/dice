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
#ifndef _AQM_H
#define _AQM_H

#include "TCTypes.h"

#ifdef _WINMP
#pragma warning( disable : 4514 )
#endif //_WINMP

// aqmQueueSend/aqmQueueReceive
// If a bus reset is in progress, will wait until the bus reset has completed.
// Then attempts to send/receive a message from the specified queue.
HRESULT				aqmQueueSend(uint32 queueID, void *pSrc, uint32 msec);
HRESULT				aqmQueueReceive(uint32 queueID, void *pDst, uint32 msec);

_TC_INLINE HRESULT	aqmQueueOpen(uint32 *queueID, uint32 elemSizeQuads, void *queueStart, uint32 queueSize) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	aqmQueueOpen(uint32 *queueID, uint32 elemSizeQuads, void *queueStart, uint32 queueSize)
							{
								return TCMsgQueueOpen (queueID, elemSizeQuads, queueStart, queueSize, TRUE);
							}
 
_TC_INLINE HRESULT	aqmQueueClose(uint32 queueID) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	aqmQueueClose(uint32 queueID)
							{
								return TCMsgQueueClose (queueID);
							}
 
_TC_INLINE HRESULT	aqmQueueGetNoOfItems(uint32 queueID, uint32 *items) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	aqmQueueGetNoOfItems(uint32 queueID, uint32 *items)
							{
								return TCMsgQueueGetNoOfItems (queueID, items);
							}

#endif //_AQM_H

