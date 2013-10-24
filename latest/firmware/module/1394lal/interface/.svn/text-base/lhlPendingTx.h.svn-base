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
 * DESCRIPTION: Link Hardware Abstraction Layer Pending Transactions Fcts

	Purpose:	Link Hardware Abstraction Layer Pending Transactions Fcts
	
	Revisions:
		created 04/08/2002 lm
****************************************************************************/

#ifndef _LHLPENDINGTX_H
#define _LHLPENDINGTX_H

#include "TCTypes.h"
#include "packetBlock.h"

typedef enum
{
	LHL_PENDING_REQ = 0,
	LHL_PENDING_RSP
#ifdef _LOOSE_ISO
	,
	LHL_PENDING_STM
#endif //_LOOSE_ISO
} PENDING_TX_TYPE;

#define	REMOVE_PENDING_ENTRY	TRUE
#define	LEAVE_PENDING_ENTRY		FALSE

// pending transactions table length
#define	MAX_PENDING_TX			63

// a pending transaction
typedef struct
{
	uint16	inUse;
	uint32	nodeAddr;
	uint32	tLabel;
	uint8	pendingTxType;
	uint32	timeStamp;
	PB		*packetBlock;
} PENDING_REC;

// pending transactions table
typedef struct
{
	uint32		inuse;
	uint32		inuseMax;
	PENDING_REC	rec[MAX_PENDING_TX];
} PENDING_TX;

// Pending transaction function for lhl modules
HRESULT			lhlPendingTxCreateSemaphores(void);
HRESULT			lhlPendingTxInitialize(void);
HRESULT			lhlPendingTxClear(void);
HRESULT			lhlPendingTxGetType(PB_PACKETTYPE packetType, uint8 *pendingTxType);
HRESULT			lhlPendingTxAdd(uint32 tLabel, uint32 nodeAddr, PB *packetBlock);
HRESULT			lhlPendingTxMatch(uint32 tLabel, uint32 nodeAddr, uint8 pendingTxType, PB **packetBlock, BOOL bRemoveEntry);
HRESULT			lhlPendingTxAgeout(void);

#endif
