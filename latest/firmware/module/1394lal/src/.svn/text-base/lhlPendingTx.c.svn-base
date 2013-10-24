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
		created 04/03/2002 lm

****************************************************************************/

#include "TCTasking.h"
#include "lhl.h"
#include "lhlCli.h"
#include "lhlPendingTx.h"
#include "lhlStatistics.h"

static char 		moduleName[] = __FILE__ ;

/*********************************************************
	Pending transaction function for lhl modules
*/

static uint32 lhlPendingTxMutexSemID; // mutual exclusion semaphore for lhlPendingTx

#define _LHLPENDING_CHECK
#undef _LHLPENDING_CHECK

// pending transactions table
typedef struct
{
#ifdef _LHLPENDING_CHECK
	uint32		checkHeader;
#endif //_LHLPENDING_CHECK
	PENDING_TX	lhlPendingTx;
#ifdef _LHLPENDING_CHECK
	uint32		checkTrailer;
#endif //_LHLPENDING_CHECK
} PENDING_TX_STRUCT;

PENDING_TX_STRUCT		lhlPendingTxStruct;

static HRESULT lhlPendingTxGet(PENDING_TX **lhlPendingTx)
{
	HRESULT		hResult = NO_ERROR;

	*lhlPendingTx = NULL;

#ifdef _LHLPENDING_CHECK
	if (lhlPendingTxStruct.checkHeader != 0x12345678 ||
		lhlPendingTxStruct.checkTrailer != 0x87654321)
	{
		hResult = E_FAIL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
#endif //_LHLPENDING_CHECK

	*lhlPendingTx = &lhlPendingTxStruct.lhlPendingTx;

	return hResult;
}

#ifdef _SYSDEBUGERROR
static HRESULT __lhlPendingTxDisplay(PENDING_TX *lhlPendingTx)
{
	HRESULT			hResult = NO_ERROR;
	uint16			i;
	char			timestr[16];

	if (lhlPendingTx == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	sysGetCurTimeStr(timestr);
	sysDebugPrintf("lhlPendingTxDisplay: time: %s\n\r", timestr);

	for (i = 0; i < MAX_PENDING_TX; i++)
	{
		if (lhlPendingTx->rec[i].inUse == 1)
		{
			sysGetTimeStr(lhlPendingTx->rec[i].timeStamp, timestr);
			sysDebugPrintf("%i: nodeAddr 0x%04x, tLabel 0x%04x, type:0x%02x, time: %s\n\r", i, lhlPendingTx->rec[i].nodeAddr, lhlPendingTx->rec[i].tLabel, lhlPendingTx->rec[i].pendingTxType, timestr);
			//lhlPendingTx->rec[i].packetBlock = packetBlock;
		}
	}

	return hResult;
}

#if 0
static HRESULT lhlPendingTxDisplay(void)
{
	HRESULT		hResult = NO_ERROR;
	PENDING_TX	*lhlPendingTx = NULL;

	// exclusive access for the pending transactions table (mutex)
	hResult = TCMutexLock(lhlPendingTxMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlPendingTxGet(&lhlPendingTx);
	if (hResult != NO_ERROR) goto ErrorExit;

	hResult = __lhlPendingTxDisplay(lhlPendingTx);

ErrorExit:

	// exclusive access for the pending transactions table (mutex)
	TCMutexUnlock(lhlPendingTxMutexSemID);

	return hResult;
}
#endif
#endif //_SYSDEBUGERROR

static HRESULT __lhlPendingTxClearEntry(PENDING_TX *lhlPendingTx, uint32 index)
{
	HRESULT		hResult = NO_ERROR;

	if (lhlPendingTx == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (index >= MAX_PENDING_TX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	memset(&lhlPendingTx->rec[index], 0, sizeof(PENDING_REC));	// done with this entry
	lhlPendingTx->inuse--;
#ifdef _STATISTICS
	lhlStatistics.PendTxInuse = lhlPendingTx->inuse;
#endif //_STATISTICS

	return hResult;
}

/*********************************************************
	Create semaphores for this module
*/
HRESULT lhlPendingTxCreateSemaphores(void)
{
	HRESULT		hResult = NO_ERROR;

	// exclusive access for the pending transactions table (mutex)
	hResult = TCMutexOpen(&lhlPendingTxMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT lhlPendingTxInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

	// clear pending request table
	memset(&lhlPendingTxStruct, 0, sizeof(lhlPendingTxStruct));

#ifdef _LHLPENDING_CHECK
	lhlPendingTxStruct.checkHeader = 0x12345678;
	lhlPendingTxStruct.checkTrailer = 0x87654321;
#endif //_LHLPENDING_CHECK

	hResult = lhlPendingTxCreateSemaphores();
	if (hResult != NO_ERROR) return hResult;

#ifdef _STATISTICS
	lhlStatistics.PendTxInuse = 0;
	lhlStatistics.PendTxInuseMax = 0;
#endif //_STATISTICS

#if 0 //def _SYSDEBUGERROR
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_ERRORS))
	{
		sysDebugPrintf("lhlPendingTxInitialize\n\r");
		lhlPendingTxDisplay();
	}
#endif //_SYSDEBUGERROR

	return hResult;
}

HRESULT lhlPendingTxClear(void)
{
	HRESULT		hResult = NO_ERROR;
	uint16		i;
	PENDING_TX	*lhlPendingTx = NULL;

	// exclusive access for the pending transactions table (mutex)
	hResult = TCMutexLock(lhlPendingTxMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlPendingTxGet(&lhlPendingTx);
	if (hResult != NO_ERROR) goto ErrorExit;

	for (i = 0; i < MAX_PENDING_TX; i++)
	{
		if (lhlPendingTx->rec[i].inUse)
		{
			if (pbIsValid(lhlPendingTx->rec[i].packetBlock))
			{
				pbSetFinalStatus(lhlPendingTx->rec[i].packetBlock, E_LHL_PENDING_TX_INTERRUPTED);
				pbPacketDone(lhlPendingTx->rec[i].packetBlock, PB_DONE_LHL_PENDING_CLEAR);
			}
			else
			{
				hResult = E_LHL_INVALID_PB;
				sysLogError(hResult, __LINE__, moduleName);
			}

			__lhlPendingTxClearEntry(lhlPendingTx, i);
		}
	}

#if 0 //def _SYSDEBUGERROR
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_ERRORS))
	{
		sysDebugPrintf("lhlPendingTxClear\n\r");
		__lhlPendingTxDisplay(lhlPendingTx);
	}
#endif //_SYSDEBUGERROR

ErrorExit:

	// exclusive access for the pending transactions table (mutex)
	TCMutexUnlock(lhlPendingTxMutexSemID);

	return hResult;
}

HRESULT lhlPendingTxGetType(PB_PACKETTYPE packetType, uint8 *pendingTxType)
{
	HRESULT			hResult = NO_ERROR;

	switch (packetType) 
	{
		case PB_TYPE_WRITE_RESPONSE:
		case PB_TYPE_READ_RESPONSE:
		case PB_TYPE_READ_RESPONSE_QUADLET:
		case PB_TYPE_LOCK_RESPONSE:
			*pendingTxType = LHL_PENDING_RSP;
			break;
#ifdef _LOOSE_ISO
		case PB_TYPE_ASYNC_STREAM:
			*pendingTxType = LHL_PENDING_STM;
			break;
#endif //_LOOSE_ISO
		default:
			*pendingTxType = LHL_PENDING_REQ;
			break;
	}

	return hResult;
}

HRESULT lhlPendingTxAdd(uint32 tLabel, uint32 nodeAddr, PB *packetBlock)
{
	HRESULT			hResult = NO_ERROR;
	uint16			i;
	BOOL			bFound = FALSE;
	uint8			pendingTxType = LHL_PENDING_REQ;
	uint32			curTime = 0;
	PB_PACKETTYPE	packetType = PB_TYPE_UNDEF;
	PENDING_TX		*lhlPendingTx = NULL;

	hResult = pbGetPacketType(packetBlock, &packetType);
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlPendingTxGetType(packetType, &pendingTxType);
	if (hResult != NO_ERROR) return hResult;

	// exclusive access for the pending transactions table (mutex)
	hResult = TCMutexLock(lhlPendingTxMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlPendingTxGet(&lhlPendingTx);
	if (hResult != NO_ERROR) goto ErrorExit;

	curTime = TCTimeGet();

	for (i = 0; i < MAX_PENDING_TX; i++)
	{
		if (lhlPendingTx->rec[i].inUse == 0)
		{
			lhlPendingTx->rec[i].tLabel = tLabel;
			lhlPendingTx->rec[i].nodeAddr = nodeAddr;
			lhlPendingTx->rec[i].pendingTxType = pendingTxType;
			lhlPendingTx->rec[i].packetBlock = packetBlock;
			lhlPendingTx->rec[i].timeStamp = curTime;
			lhlPendingTx->rec[i].inUse = 1;

			SYS_DEBUG(SYSDEBUG_TRACE_PACKETS, "lhlPendingTxAdd: nodeAddr 0x%04x, tLabel 0x%04x, type:0x%02x, index:%i\n\r", nodeAddr, tLabel, pendingTxType, i);

#if 0 //def _SYSDEBUGERROR
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_ERRORS))
			{
				sysDebugPrintf("lhlPendingTxAdd\n\r");
				__lhlPendingTxDisplay(lhlPendingTx);
			}
#endif //_SYSDEBUGERROR

			lhlPendingTx->inuse++;
			if (lhlPendingTx->inuseMax < lhlPendingTx->inuse)
			{
				lhlPendingTx->inuseMax = lhlPendingTx->inuse;
			}
#ifdef _STATISTICS
			lhlStatistics.PendTxInuse = lhlPendingTx->inuse;
			lhlStatistics.PendTxInuseMax = lhlPendingTx->inuseMax;
#endif //_STATISTICS

			bFound = TRUE;
			break;
		}
	}

	if (bFound == FALSE)
	{
		hResult = E_LHL_OUT_OF_PENDING_REQS;
		sysLogError(hResult, __LINE__, moduleName);
	}

ErrorExit:

	// exclusive access for the pending transactions table (mutex)
	TCMutexUnlock(lhlPendingTxMutexSemID);

	return hResult;
}

/*********************************************************************
	operation			PendingTx	req/rsp/stm
	----------------------------------------
REQ
	send request		(add)		req
	error sending		(remove)	req
	ack pending			(find)		req
	ack complete/error	(remove)	req
	resc response		(remove)	req (rsp->req)
	timeout/error		(remove)	req

RSP
	send response		(add)		rsp
	error sending		(remove)	rsp
	ack complete/error	(remove)	rsp
	timeout/error		(remove)	rsp

REQ
	send stream			(add)		stm
	error sending		(remove)	stm
	ack complete/error	(remove)	stm
	timeout/error		(remove)	stm

***********************************************************************/

HRESULT lhlPendingTxMatch(uint32 tLabel, uint32 nodeAddr, uint8 pendingTxType, PB **packetBlock, BOOL bRemoveEntry)
{
	HRESULT		hResult = NO_ERROR;
	uint16		i;
	BOOL		bFound = FALSE;
	PENDING_TX	*lhlPendingTx = NULL;

	// exclusive access for the pending transactions table (mutex)
	hResult = TCMutexLock(lhlPendingTxMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlPendingTxGet(&lhlPendingTx);
	if (hResult != NO_ERROR) goto ErrorExit;

	for (i = 0; i < MAX_PENDING_TX; i++)
	{
		if (lhlPendingTx->rec[i].inUse)
		{
			if ((lhlPendingTx->rec[i].tLabel == tLabel) && 
				(lhlPendingTx->rec[i].nodeAddr == nodeAddr) &&
				(lhlPendingTx->rec[i].pendingTxType == pendingTxType))
			{
				if (packetBlock)
				{
					*packetBlock = lhlPendingTx->rec[i].packetBlock;
				}

				if (bRemoveEntry)
				{
					__lhlPendingTxClearEntry(lhlPendingTx, i);
				}
				bFound = TRUE;
				break;
			}
		}
	}

	SYS_DEBUG(SYSDEBUG_TRACE_PACKETS, "lhlPendingTxMatch: nodeAddr 0x%04x, tLabel 0x%04x, type:0x%02x, found:%i, remove:%i\n\r", nodeAddr, tLabel, pendingTxType, bFound, bRemoveEntry);

	if (bFound == FALSE)
	{
		hResult = E_LHL_PENDING_REQ_NOT_FOUND;
#ifdef _STATISTICS
		switch (pendingTxType)
		{
			case LHL_PENDING_REQ:
				lhlStatistics.PendTxReqMatchErr++;
				break;
			case LHL_PENDING_RSP:
				lhlStatistics.PendTxRspMatchErr++;
				break;
#ifdef _LOOSE_ISO
			case LHL_PENDING_STM:
				lhlStatistics.PendTxStmMatchErr++;
				break;
#endif //_LOOSE_ISO
		}
#endif //_STATISTICS

#if 0 //LM???
		sysLogError(hResult, __LINE__, moduleName);
#endif //LM???

#ifdef _SYSDEBUGERROR
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_ERRORS))
		{
			sysDebugPrintf("lhlPendingTxMatch\n\r");
			__lhlPendingTxDisplay(lhlPendingTx);
		}
#endif //_SYSDEBUGERROR
	}

ErrorExit:

	// exclusive access for the pending transactions table (mutex)
	TCMutexUnlock(lhlPendingTxMutexSemID);

	return hResult;
}

HRESULT lhlPendingTxAgeout(void) // age out transmit request packets
{
	HRESULT		hResult = NO_ERROR;
	uint16		i;
	PENDING_TX	*lhlPendingTx = NULL;

	// exclusive access for the pending transactions table (mutex)
	hResult = TCMutexLock(lhlPendingTxMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlPendingTxGet(&lhlPendingTx);
	if (hResult != NO_ERROR) goto ErrorExit;

	for (i = 0; i < MAX_PENDING_TX; i++)
	{
		if (lhlPendingTx->rec[i].inUse)
		{
			if (TCTimeElapsed(lhlPendingTx->rec[i].timeStamp) > LHL_PACKET_AGEOUT_MSECS)
			{
				hResult = E_LHL_TX_PACKET_AGED_OUT;
#ifdef _STATISTICS
				switch (lhlPendingTx->rec[i].pendingTxType)
				{
					case LHL_PENDING_REQ:
						lhlStatistics.PendTxReqAgeout++;
						break;
					case LHL_PENDING_RSP:
						lhlStatistics.PendTxRspAgeout++;
						break;
#ifdef _LOOSE_ISO
					case LHL_PENDING_STM:
						lhlStatistics.PendTxStmAgeout++;
						break;
#endif //_LOOSE_ISO
				}
#endif //_STATISTICS

				sysLogError(hResult, __LINE__, moduleName);

				SYS_DEBUG(SYSDEBUG_TRACE_PACKETS, "lhlPendingTxAgeout: nodeAddr 0x%08x, tLabel 0x%08x, type:%i\n\r", lhlPendingTx->rec[i].nodeAddr, lhlPendingTx->rec[i].tLabel, lhlPendingTx->rec[i].pendingTxType);
				if (pbIsValid(lhlPendingTx->rec[i].packetBlock))
				{
					lhlPendingTx->rec[i].packetBlock->creatorCBProc = NULL;
					pbPacketDone(lhlPendingTx->rec[i].packetBlock, PB_DONE_LHL_PENDING_AGEOUT);
				}
				else
				{
					hResult = E_LHL_INVALID_PB;
					sysLogError(hResult, __LINE__, moduleName);
				}

				__lhlPendingTxClearEntry(lhlPendingTx, i);
			}
		}
	}

ErrorExit:

	// exclusive access for the pending transactions table (mutex)
	TCMutexUnlock(lhlPendingTxMutexSemID);

	return hResult;
}

