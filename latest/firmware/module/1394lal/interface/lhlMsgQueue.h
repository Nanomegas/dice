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
 * DESCRIPTION: Link Hardware Abstraction Layer Message Queue Fcts

	Purpose:	Link Hardware Abstraction Layer Message Queue Fcts
	
	Revisions:
		created 04/08/2002 lm

****************************************************************************/
#ifndef _LHLMSGQUEUE_H
#define _LHLMSGQUEUE_H

#include "packetBlock.h"
#include "TCTypes.h"

enum // queueType
{
	LHL_QUEUE_PRIORITY = 0,
	LHL_QUEUE_GENERAL,
	LHL_QUEUE_ITEMS
};

// Message queues for lhl modules
HRESULT			lhlMsgQueueInitialize(void);

// put to queue and signal tx semaphore
HRESULT			lhlMsgQueueEnQPacketBlock(uint16 queue, PB *packetBlock);
// put to queue, signal tx semaphore, and wait for supplied semaphore to be signalled
HRESULT			lhlMsgQueueEnQPacketBlockWaitTimeout(uint16 queue, PB *packetBlock, uint32 semID, uint32 msec);

HRESULT			lhlMsgQueuePutPacketBlock(uint16 queue, PB *packetBlock);
HRESULT			lhlMsgQueueGetPacketBlock(uint16 queue, PB **packetBlock, uint32 msec);

#endif
