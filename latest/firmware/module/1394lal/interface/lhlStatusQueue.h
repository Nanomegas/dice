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
 * DESCRIPTION: Link Hardware Abstraction Layer (Status Queue)

	Purpose:	Link Hardware Abstraction Layer
				Status "Queues" for requests and responses
				
	Revisions:
		created 08/26/2002 lm

****************************************************************************/

#ifndef _LHLSTATUSQUEUE_H
#define _LHLSTATUSQUEUE_H

#include "TCTypes.h"
#include "1394.h"
#include "packetBlock.h"
#include "bmdefs.h"
#ifdef _WINMP
#pragma warning( disable : 4514 )
#endif //_WINMP

#define		STATUS_INFO_ITEMS		16						// maximum outstanding requests and responses to 1394 (meaning waiting threads)

//	structures used to transfer packet status from the request packet
//	acknowledge to the request function, and to transfer status/data from the
//	response packet to the original request function.
typedef struct
{
	uint32			semID;									// synchronization semaphore 
	HRESULT			hResultFinal;
	uint32			tLabel;
	uint32			nodeAddr;
	uint32			genType;								// covering both request and response types
	PB_PACKETTYPE	packetType;
	uint32			*data;
	uint32			numBytes;
} STATUS_INFO;

typedef struct 
{
	STATUS_INFO		statusInfo[STATUS_INFO_ITEMS];
	BOOL			allocated[STATUS_INFO_ITEMS];
	uint32			ptr;
} STATUS_INFO_QUEUE;

HRESULT		lhlStatusQueueInitialize(void);
HRESULT		lhlStatusQueueCreateSemaphores(void);
HRESULT		lhlStatusQueueResetInfo(STATUS_INFO *statusInfo);
HRESULT		lhlStatusQueueCreate(STATUS_INFO **statusInfo);
HRESULT		lhlStatusQueueMatch(uint32 nodeAddr, uint32 tLabel, uint32 genType, PB_PACKETTYPE packetType, STATUS_INFO **statusInfo, BOOL bRemoveEntry);

_TC_INLINE HRESULT lhlStatusQueueMatchFinish(uint32 nodeAddr, uint32 tLabel, uint32 genType, PB_PACKETTYPE packetType) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT lhlStatusQueueMatchFinish(uint32 nodeAddr, uint32 tLabel, uint32 genType, PB_PACKETTYPE packetType)
{
	return lhlStatusQueueMatch(nodeAddr, tLabel, genType, packetType, NULL, TRUE);
}

_TC_INLINE HRESULT lhlStatusQueueMatchUpdate(uint32 nodeAddr, uint32 tLabel, uint32 genType, PB_PACKETTYPE packetType, STATUS_INFO **statusInfo) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT lhlStatusQueueMatchUpdate(uint32 nodeAddr, uint32 tLabel, uint32 genType, PB_PACKETTYPE packetType, STATUS_INFO **statusInfo)
{
	return lhlStatusQueueMatch(nodeAddr, tLabel, genType, packetType, statusInfo, FALSE);
}

#endif
