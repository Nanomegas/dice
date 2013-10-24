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
 * DESCRIPTION: Link Abstraction Layer Interfaces for blocking 1394 i/o

	Purpose:	Link Abstraction Layer 
				Interfaces for blocking 1394 i/o
	
	Revisions:
		created 04/22/2002 lm
****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "lal.h"
#include "lhlReq.h"
#include "lhlRsp.h"
#include "lhlMsgQueue.h"
#include "lhlStatistics.h"
#include "nciHandles.h"

HRESULT lalSendWriteRequest (PB* packetBlock, OFFSET_1394 destOffset)
{
	uint32		payloadSize = 0;
	uint32*		pPayload = 0;
	uint32		nodeAddr = 0;

	pbGetPayloadSize (packetBlock, &payloadSize);
	pbGetPayload (packetBlock, (void **) &pPayload);
	pbGetPacketDstNodeAddr (packetBlock, &nodeAddr);

	return lhlReq1394(packetBlock, nodeAddr, destOffset, (uint16) payloadSize, pPayload, LHL_QUEUE_PRIORITY, LHL_REQ_WRITE, PB_LOCKTYPE_NONE, LHL_TX_REQ_TIMEOUT_MSECS);
}

