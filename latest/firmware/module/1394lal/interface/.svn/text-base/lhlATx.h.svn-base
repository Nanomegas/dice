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
 * DESCRIPTION: Link Hardware Abstraction Layer transmit packet i/o

	Purpose:	Link Hardware Abstraction Layer transmit packet i/o
						handling for asynchronous transmit packets
	
	Revisions:
		created 04/03/2002 lm
****************************************************************************/

#ifndef _LHLATX_H
#define _LHLATX_H

#include "tctypes.h"

void		lhlTxSndPendSignalSema(void);	// Signal to transmit packets thread to send packet

HRESULT		lhlTxCreateSemaphores(void);
HRESULT		lhlTxResumeIntHandlers(void);	// resume interrupt handlers for this module
HRESULT		lhlTxSuspendIntHandlers(void);	// suspend interrupt handlers for this module

HRESULT		lhlTxResumeTasks(void);			// resume tasks for this module
HRESULT		lhlTxSuspendTasks(void);		// suspend tasks for this module

HRESULT		lhlTxInitialize(void);			// create tasks (interrupt handlers, threads and semaphores) for this module

HRESULT lhlTxSndPhyPacket(QUADLET phyPacket);

//some of the new context stuff
#define MAX_TX_CONTEXTS	2

typedef struct
{
	uint16		bPosted;
	uint16		headerLen;
	uint32		payloadLen;
	PB_HEADER	header;
	uint32 *	pPayload;
	uint32		txStat;
	uint32		semID;
	uint32		tsnd;
	uint32		tack;
} POSTED_TX_CONTEXT;

typedef struct
{
	uint16						currTxContext;
	uint16						inBr;
	POSTED_TX_CONTEXT	contexts[MAX_TX_CONTEXTS];
} POSTED_TX_STATE;

extern volatile POSTED_TX_STATE postedTxState __attribute__((section (".sram.llctx")));


#endif
