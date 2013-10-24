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
	
	Purpose:	LLC initialization and abstraction layer for
				DMA handling for asynchronous transmit and receive packets
				for the Samsung 1394 LLC
	
	Revisions:
		created 12/18/2002 kbj

****************************************************************************/

#ifndef _LLCDMA_H
#define _LLCDMA_H

#include "TCTypes.h"
#include "packetBlock.h"

// Transmit header information
#define LLC_SOURCEID_3FF		0x00800000
#define LLC_SOURCEID_NODEID		0x00000000

// Short waiting time for the transmit buffer to be ready
#define	WRITE_READY_TIMEOUT		10000

// Resetting TX and RX FIFO buffers
void llcResetFIFOBuffers(void);

// Process received packet from FIFO
HRESULT llcProcessRxPacket(QUADLET *pPacket, uint32 size);


#endif
