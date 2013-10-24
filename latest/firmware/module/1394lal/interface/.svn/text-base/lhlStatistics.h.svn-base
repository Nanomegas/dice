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
* DESCRIPTION: Link Hardware Abstraction Layer Statistics

	Purpose:	Link Hardware Abstraction Layer
						handling statistics
	
	Revisions:
		created 04/03/2002 lm
****************************************************************************/

#ifndef _LHL_STATISTICS_H
#define _LHL_STATISTICS_H

#ifdef _STATISTICS

#include "TCTypes.h"

// runtime lhl statistics

#define LHL_STATISTICS_RESET_ALL			0x00000001
#define	LHL_STATISTICS_RESET_BRI			0x00000004
#define	LHL_STATISTICS_RESET_COUNT			0x00000002
#define	LHL_STATISTICS_RESET_ERRORS			0x00000008
#define	LHL_STATISTICS_RESET_SELECTED		0x0000000A
#define	LHL_STATISTICS_RESET_SAVE_REQ		0x0000001A

#define STATISTICS_ITEMS					64 // quadlets
#define STATISTICS_SIZE						(4*STATISTICS_ITEMS) // bytes

#define STATISTICS_VALID_ENTRY_ITEMS		1
#define STATISTICS_BRI_ITEMS				2
#define STATISTICS_COUNT_ITEMS				15
#define STATISTICS_ASYNC_STREAM_ITEMS		2
#define STATISTICS_TIMEOUT_ITEMS			2
#define STATISTICS_PENDINGTX_ITEMS			8
#define STATISTICS_PB_ITEMS					2
#define STATISTICS_TLABELS_ITEMS			4
#define STATISTICS_STATUSINFO_ITEMS			2
#define STATISTICS_SYSLOG_ITEMS				2
#define STATISTICS_MSGQUEUE_ITEMS			2
#define STATISTICS_HANDLE_ITEMS				1

#define STATISTICS_BRI_ACC_ITEMS			(STATISTICS_BRI_ITEMS+STATISTICS_VALID_ENTRY_ITEMS)
#define STATISTICS_COUNT_ACC_ITEMS			(STATISTICS_COUNT_ITEMS+STATISTICS_BRI_ACC_ITEMS)
#define STATISTICS_ASYNC_STREAM_ACC_ITEMS	(STATISTICS_ASYNC_STREAM_ITEMS+STATISTICS_COUNT_ACC_ITEMS)
#define STATISTICS_TIMEOUT_ACC_ITEMS		(STATISTICS_TIMEOUT_ITEMS+STATISTICS_ASYNC_STREAM_ACC_ITEMS)
#define STATISTICS_PENDINGTX_ACC_ITEMS		(STATISTICS_PENDINGTX_ITEMS+STATISTICS_TIMEOUT_ACC_ITEMS)
#define STATISTICS_PB_ACC_ITEMS				(STATISTICS_PB_ITEMS+STATISTICS_PENDINGTX_ACC_ITEMS)
#define STATISTICS_TLABELS_ACC_ITEMS		(STATISTICS_TLABELS_ITEMS+STATISTICS_PB_ACC_ITEMS)
#define STATISTICS_STATUSINFO_ACC_ITEMS		(STATISTICS_STATUSINFO_ITEMS+STATISTICS_TLABELS_ACC_ITEMS)
#define STATISTICS_SYSLOG_ACC_ITEMS			(STATISTICS_SYSLOG_ITEMS+STATISTICS_STATUSINFO_ACC_ITEMS)
#define STATISTICS_MSGQUEUE_ACC_ITEMS		(STATISTICS_MSGQUEUE_ITEMS+STATISTICS_SYSLOG_ACC_ITEMS)
#define STATISTICS_HANDLE_ACC_ITEMS			(STATISTICS_HANDLE_ITEMS+STATISTICS_MSGQUEUE_ACC_ITEMS)

#define STATISTICS_VALID_ITEMS				STATISTICS_HANDLE_ACC_ITEMS // in quadlets (including validItems)

typedef struct
{
	uint32	validItems;

	uint32	BrBriBRInt;					// bri
	uint32	BrBriSIInt;

	uint32	RxReqInt;					// req/rsp counters
	uint32	RxRspInt;
	uint32	TxReqInt;
	uint32	TxRspInt;
	uint32	TxReqIntStart;
	uint32	TxReqIntWake;
	uint32	TxRspIntStart;
	uint32	TxRspIntWake;
	uint32	RxReqCount;
	uint32	RxRspCount;
	uint32	TxReqCount;
	uint32	TxRspCount;
	uint32  RxNoHeader;
	uint32  RxMorePackets;
	uint32  SelfIDErrs;

	uint32	TxStmCount;					// async stream
	uint32	RxStmCount;

	uint32	TxReqTimeout;				// timeouts
	uint32	TxRspTimeout;

	uint32	PendTxInuse;				// PendingTx
	uint32	PendTxInuseMax;
	uint32	PendTxReqMatchErr;
	uint32	PendTxRspMatchErr;
	uint32	PendTxReqAgeout;
	uint32	PendTxRspAgeout;
	uint32	PendTxStmMatchErr;
	uint32	PendTxStmAgeout;

	uint32	pbAllocBlock;				// packetblock
	uint32	pbAllocPayload;

	uint32	TLabelsReqInuse;			// tLabels
	uint32	TLabelsReqInuseMax;
	uint32	TLabelsStmInuse;
	uint32	TLabelsStmInuseMax;

	uint32	statusInfoInuse;			// statusInfo
	uint32	statusInfoInuseMax;

	uint32	sysLogErr;					// syslogErr
	uint32	sysLogISR;

	uint32	msgQueueInuse;				// statusInfo
	uint32	msgQueueInuseMax;

	uint32	handleInvalid;				// handles

	// only add when updating defines
	uint32	reserved[STATISTICS_ITEMS - STATISTICS_VALID_ITEMS];
} LHL_STATISTICS;

extern LHL_STATISTICS	lhlStatistics;

HRESULT			lhlStatisticsInitialize(void);
HRESULT			lhlStatisticsReset(void);
HRESULT			lhlStatisticsResetSpecial(uint32 resetMode);

#endif //_STATISTICS

#endif
