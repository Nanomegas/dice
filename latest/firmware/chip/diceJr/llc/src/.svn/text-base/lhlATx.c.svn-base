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
						for the Samsung LLC
	
	Revisions:
		created 18/12/2002 kbj

****************************************************************************/

#include "TCTasking.h"
#include "TCInterrupt.h"
#include "1394.h"
#include "packetBlock.h"
#include "lal.h"
#include "lhl.h"
#include "lhlATx.h"
#include "lhlCli.h"
#include "lhlMsgQueue.h"
#include "lhlPendingTx.h"
#include "bri.h"
#include "llcInt.h"
#include "llcReg.h"
#include "llcFIFO.h"
#include "llcUtils.h"
#include "sysDebug.h"

static char 		moduleName[] = __FILE__ ;

// semaphore IDs for this module
uint32					lhlTxDoneSemID;			// hardware Tx data done semaphore 
uint32					lhlTxSndPendSemID;		// pending transmit queue packets semaphore
uint32					lhlTxPhyPckSemID;
// task (thread) IDs for this module
uint32					lhlTxSndPendThreadID;	// pending transmit queue packets thread
const char*				lhlTxSndPendThreadName = "lhlTxSndPendThread";

//The interrupt system takes care of moving the packet to the LLC. This is done to assure
//that only one execution context is doing the move as we have one thread and one FIQ posting
//packets.
//
//The state machine is driven by a number of events:
//	LLCID_ASY_TX_CMPL interrupt, indicates completion of transmission
//	LLCID_APP_GEN_INTR interrupt, used to signal that a packet was posted
//	brStart indication that bus reset processing has started
//  brEnd indication that bus reset pre processing has completed.
//
//	This system handles two concurrent Tx contexts, typically one is used for Thread 
//  and one for FIQ context.
//
//  Each context holds the following state:
//		Posted					True if posted and not yet completed
//		HeaderLength		Number of header quadlets
//		PayloadLength		Number of payload quadlets if any
//		Header data			Up to 4 quadlets of header data
//		PayloadDataPtr	Pointer to payload
//		TxStat					Final status of transmission, LLC ASY_TX_STAT_REG_DP
//		Semaphore				Signal When completed
//
//	Furthermore the system holds a current context (currTxContext) variable which can be:
//		0: No Tx in progress
//		1: Context[0] in progress
//		2: Context[1] in progress
//
//	Posting rules:
//		It is only legal to post when Posted is false
//		Post all the data and then set Posted=true
//		Issue LLCID_APP_GEN_INTR to notify interrupt routine
//		Semaphore (if any will be signaled after clearing Posted)
//
//  State changes:
//		LLCID_APP_GEN_INTR	if currTxContext or not LLC_INT_STATE_PROCESS_NORMAL return
//												else check and send (see below)
//
//		LLCID_ASY_TX_CMPL		read status and update current context with completion
//												currTxContext=0, check and send (see below)
//
//		brStart							Busreset started, update both contexts with error completion
//												currTxContext=0, 
//
//		brEnd								check and send
//
//		Check and Send			Check contexts, if one is posted:
//													currTxContext=the one
//													Move header and payload to LLC
//

/*
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
} POSTED_TX_CONTEXT;

typedef struct
{
	uint16						currTxContext;
	uint16						inBr;
	POSTED_TX_CONTEXT	contexts[MAX_TX_CONTEXTS];
} POSTED_TX_STATE;
*/

POSTED_TX_STATE volatile postedTxState __attribute__((section (".sram.llctx")));

static void initializeContexts(void)
{
	postedTxState.inBr = TRUE;
}




////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Interrupt level implementation of the posted context system.
///
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

static void ChkAndSend(void) __attribute__((section (".sram.llccode")));

static void ChkAndSend(void)
{
	uint32 asyTxStat;
	llcLinkRegRead(ASY_TX_STAT_REG_DP, &asyTxStat);
	if ((asyTxStat && LLC_ASY_TX_CMPLT) || ((asyTxStat && LLC_ASY_TX_STATUS_MASK) == LLC_ASY_TX_ST_IDLE))
	{
		uint16 i=0;
		POSTED_TX_CONTEXT * pContext = postedTxState.contexts;
		
		while (i<MAX_TX_CONTEXTS)
		{
			if (pContext->bPosted)
			{
				pContext->tsnd = *((volatile uint32 *)TIMER2_CUR_VAL);
				pContext->tack = 0; 
				//move the packet to the LLC
				llcSendContext(pContext->pPayload, pContext->header.quadlets, pContext->payloadLen, pContext->headerLen);
				postedTxState.currTxContext=i+1;
				return;
			}
			i++;
			pContext++;
		}
	}
	else
	{
		//in progress, we will get back when completed
	}
}

static void lhlTxIntHandler(void)
{
	//if we are called it can only be to signal context[1] semaphore
	// ML: TODO add flags if the other context had a semaphore
  // ML 140610: Busreset storm fix. Use semID to indicate whether semaphore was signaled.
  //            The problem would occur if a bus reset comes in after the Isr handler had returned
  //            TRUE but before this DSR was called. The lhlBrInOutHandler would not clean up as
  //            the posted flag was clear so it would think cleaning was done.
  if (postedTxState.contexts[1].semID) 
  {
    fSYS_TRACE1(SYSDEBUG_TRACE_TESTS,0);
    TCSemaphoreSignal(postedTxState.contexts[1].semID);
    postedTxState.contexts[1].semID = 0; //done signaling.
  }  
}

BOOL lhlTxIsrHandler(void) __attribute__((section (".sram.llccode")));

BOOL lhlTxIsrHandler(void)
{
	BOOL bRet=FALSE;
	uint32 asyTxStat;
#ifdef _STATISTICS
	lhlStatistics.TxReqInt++;
#endif //_STATISTICS
	llcLinkRegRead(ASY_TX_STAT_REG_DP, &asyTxStat);
	if (postedTxState.currTxContext)
	{
		if (asyTxStat & LLC_ASY_TX_CMPLT)
		{
			volatile POSTED_TX_CONTEXT * pContext = &postedTxState.contexts[postedTxState.currTxContext-1];
			pContext->txStat = asyTxStat;
			pContext->bPosted = FALSE;
			fSYS_TRACE1(SYSDEBUG_TRACE_TESTS,postedTxState.currTxContext);
			pContext->tack = *((volatile uint32 *)TIMER2_CUR_VAL);
			if (pContext->semID) bRet |= TRUE; //_lTCSemaphoreSignal(pContext->semID);
			postedTxState.currTxContext=0;
			ChkAndSend();
		}
		else
		{
			//spurious or request for posting
		}
	}
	else
	{
		//We have no context, so let's see if we have something to send
		ChkAndSend();
	}
	fSYS_TRACE1(SYSDEBUG_TRACE_TESTS,bRet);
	return bRet;
}


#pragma long_calls

//called at DSR level
void lhlBrInOutHandler (BOOL bIn)
{
	uint8 i;

	if (bIn && !postedTxState.inBr)
	{
		postedTxState.inBr = TRUE;
		postedTxState.currTxContext=0;
		POSTED_TX_CONTEXT * pContext = postedTxState.contexts;
		for (i=0; i<MAX_TX_CONTEXTS; i++)
		{
			// ML 140610: Busreset storm fix. Use semID to indicate whether semaphore was signaled and
			//            not posted as it would be cleared by the Isr.
			if (pContext->semID)
			{
				pContext->txStat = LLC_ASY_TX_CMPLT | (LLC_EVT_FLUSHED<<LLC_ASY_TX_EVENT_SHIFT);
				pContext->bPosted = FALSE;
				fSYS_TRACE1(SYSDEBUG_TRACE_TESTS,i+1);
				_lTCSemaphoreSignal(pContext->semID);
				pContext->semID = 0; //done signaling.
			}
			pContext++;
		}
	}
	else if (!bIn && postedTxState.inBr)
	{
		//we have left the br state
		postedTxState.inBr = FALSE;
		ChkAndSend();
	}
}

#pragma long_calls_off




////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Thread level using Context[1] exclusively. Only this thread will touch this context and whenever posting
/// it will wait for completion before reusing the context.
///	This system does not use the other context (0) and that can be used by another syste like an FIQ high
/// priority handler.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


static HRESULT context1PostPacket(PB * pPacket,uint32 semID)
{
	HRESULT hResult = NO_ERROR;

	//check if context is available, should be as we only have one thread using the context
	volatile POSTED_TX_CONTEXT * pContext = &postedTxState.contexts[1];
	if (pContext->bPosted)
	{
			hResult = E_LAL_RESPONSE_UNEXPECTED;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
	}
	PB_HEADER*	pHeader = 0;
	uint32		speedCode = 0;
#ifdef _LOOSE_ISO
	uint8		asyncStream = FALSE;
#endif //_LOOSE_ISO


 	// ML 140610: Busreset storm fix. Let's skip if we ar ein bus reset
 	if (postedTxState.inBr)
	{
	  hResult = E_LAL_BUS_RESET_IN_PROGRESS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	pbGetPacketHeader(pPacket, &pHeader);
	pbGetPacketSpeed(pPacket, &speedCode);

	PB_PACKETTYPE packetType;
	pbGetPacketType(pPacket, &packetType);

	pContext->header.quadlets[0] = (pHeader->quadlets[0] & 0x0000ffff);		// 0:spd,tag,ch,tcode,sy
	pContext->header.quadlets[0]|= speedCode;

#ifdef _LOOSE_ISO
	pbIsAsyncStreamPacket (pPacket, &asyncStream);
	if (asyncStream == TRUE)
	{
		pContext->header.quadlets[1] = (pHeader->quadlets[1] & 0xffff0000);		// 1:data_length,reserved
	}
	else
#endif //_LOOSE_ISO
	if (packetType == TCODE_PHY_PACKET)
	{
		pContext->header.quadlets[0] = ((uint32)TCODE_LLC_SPECIFIC) << SHIFT_TCODE;
		pContext->header.quadlets[1] = pHeader->quadlets[0];
		pContext->header.quadlets[2] = ~pHeader->quadlets[0];
	}
	else
	{
		pContext->header.quadlets[0] |= LLC_SOURCEID_NODEID;
		pContext->header.quadlets[1] = (pHeader->quadlets[0] & 0xffff0000);		// 1:dstId,destination_offsetHigh
		pContext->header.quadlets[1] |= (pHeader->quadlets[1] & 0x0000ffff);
		pContext->header.quadlets[2] = pHeader->quadlets[2];						// 2:destination_offsetLow
		pContext->header.quadlets[3] = pHeader->quadlets[3];						// 3:data_length,extended_tcode
	}
	hResult = lhlGetHeaderQuadSizeFromTCode(LHL_TX, packetType, (uint16 *)&pContext->headerLen);
	if (hResult != NO_ERROR) return hResult;

	uint32 payloadNumBytes=0;
	
	pbGetPayload(pPacket, (void **)&pContext->pPayload);
	pbGetPayloadSize(pPacket, &payloadNumBytes);
	
	//Ugly, the payload is not always refering to the outgoing packet.
	if ((packetType==PB_TYPE_READ_REQUEST) || (packetType==PB_TYPE_READ_REQUEST_QUADLET) ||(packetType==PB_TYPE_WRITE_RESPONSE) || (packetType==TCODE_PHY_PACKET)) 
		payloadNumBytes=0;
	//Another hack as the read resp payload should always be one quadlet even in case of error
	if (packetType==PB_TYPE_READ_RESPONSE_QUADLET)
		payloadNumBytes=4;

	pContext->payloadLen = (payloadNumBytes+3)>>2;
	
	// ML 140610: Busreset storm fix. There is potentially a race condition here if
	//            Isr or Dsr comes here. Fixed with interrupt disable.
	TCInterruptGlobalDisable();
	if (postedTxState.inBr)
	{
	  TCInterruptGlobalEnable();
	  hResult = E_LAL_BUS_RESET_IN_PROGRESS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
  pContext->semID = semID;
	pContext->bPosted = TRUE;
  fSYS_TRACE1(SYSDEBUG_TRACE_TESTS,0);
  llcLinkRegWrite(INTERRUPT_REG_SET_DP, LLC_INT_BIT(LLCID_ASY_TX_CMPL));	
	TCInterruptGlobalEnable();
	return hResult;
}


/***************************************************************************
	Handling of the acknowledge transmitted packet request/response done
*/
static void lhlTxPacketDoneLLC(uint32 tLabel, uint16 tCode, uint32 nodeAddr, uint8 response)
{
	HRESULT		hResult = NO_ERROR;
	PB			*pMatchPB = NULL;
	uint16		conf;
	BOOL		bRemoveEntry = TRUE;
	uint8		pendingTxType; // ack for sending req/rsp/stm match corresponding type in PendingTx


	volatile POSTED_TX_CONTEXT * pContext = &postedTxState.contexts[1];
	
	if ((pContext->txStat & LLC_ASY_TX_CMPLT) == 0)
	{
		sysLogError(E_LLC_TX_NEVER_COMPLETED, __LINE__, moduleName);
	}

	conf = (pContext->txStat & LLC_ASY_TX_EVENT_MASK) >> LLC_ASY_TX_EVENT_SHIFT;

	// Handling of packet
	hResult = lhlPendingTxGetType((PB_PACKETTYPE) tCode, &pendingTxType);
	if (hResult != NO_ERROR) return;

	if (response)
	{
		// response acked by destination
		bRemoveEntry = TRUE;			
	}
	else
	{
		// OHCI_ACK_PENDING: succes, leave the packet in the pending list to be matched with the response packet
		// o.w. the transaction failed at the link layer, this status will be passed to the callback
		bRemoveEntry = (conf != ACK_1394_PENDING);
	}

	if (bRemoveEntry) 
	{
		hResult = lhlPendingTxMatch(tLabel, nodeAddr, pendingTxType, &pMatchPB, bRemoveEntry);
		if (hResult == NO_ERROR)
		{
			// either packet acked by destination, or error returned, or request expects no response  
			if (pbIsValid(pMatchPB))
			{
				HRESULT hResultFinal = NO_ERROR;

				if (conf != ACK_1394_COMPLETE && 
					conf != LLC_EVENT_INIT)			// Termination of async packet with no ack (e.g. bcast, phy, async stream)
				{
					hResultFinal = (HRESULT) (E_LHL_CONF_BASE + conf);
					SYS_DEBUG(SYSDEBUG_TRACE_PACKETS, "lhlATx SndPacketDone: ack error for %s call CB (conf: %i)\n\r", (response) ? "rsp" : "req", conf);
				}
				else
				{
					SYS_DEBUG(SYSDEBUG_TRACE_PACKETS, "lhlATx SndPacketDone: ack complete for %s call CB\n\r", (response) ? "rsp" : "req" );
				}

				pbSwapSrcDstNodeAddr(pMatchPB);
				pbSetFinalStatus(pMatchPB, hResultFinal);
				pbPacketDone(pMatchPB, PB_DONE_LHL_ATX_SND_DONE);
			}
			else
			{
				hResult = E_LHL_INVALID_PB;
				sysLogError(hResult, __LINE__, moduleName);
			}
		}
		else
		{
			SYS_DEBUG(SYSDEBUG_TRACE_PACKETS, "lhlATx SndPacketDone: match not found for %s\n\r", (response) ? "rsp" : "req" );
		}
	}
}



/***************************************************************************
 ***************************************************************************

	Packet Transmit

 ***************************************************************************/

 /*********************************************************
	The tx thread calls this when any transmit queue has one or more packets to transmit.
		
	the priority queue is always checked first and serviced until empty, the 
	general queue is ignored during bus reset.  this allows node handles to be 
	fixed up after a bus reset, making them transparent to the upper layers,
	unless a node has left the bus of course. this routine will call the appropriate
	send routine for request or response.
*/
static HRESULT lhlTxSndPacketPend(void)
{
	HRESULT		hResult = NO_ERROR;
	PB*			packetBlock = NULL;
	PB_HEADER 	*pHeader = NULL;
	uint32		tLabel = 0;
	uint32		tCode = 0;
	uint32		nodeAddr = 0;
	uint32 busGenCurrent = 0;
	uint32 busGenPacket = 0;

	DO_FOREVER 
	{
		// look for priority packets (non-blocking)
		hResult = lhlMsgQueueGetPacketBlock(LHL_QUEUE_PRIORITY, &packetBlock, TC_NO_WAIT);
		if (hResult != NO_ERROR) return hResult;

		if (pbIsValid(packetBlock))
		{	
			// if bus reset happened, queued requests must be thrown out
			briGetCurrentBusGeneration(&busGenCurrent);			
			pbGetBusGeneration (packetBlock, &busGenPacket);
			
			if (busGenCurrent != busGenPacket)
			{
				
				pbSwapSrcDstNodeAddr(packetBlock);

				pbSetFinalStatus(packetBlock, E_LHL_BUS_RESET_IN_PROGRESS);
				pbPacketDone(packetBlock, PB_DONE_LHL_ATX_SND_PEND_1);
				hResult = E_BRI_NEW_BUS_RESET;
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}
		}
		else
		{
			if (briInBusReset() == FALSE)
			{
				// if not in bus reset processing, look for general packets (non-blocking)
				hResult = lhlMsgQueueGetPacketBlock(LHL_QUEUE_GENERAL, &packetBlock, TC_NO_WAIT);
				if (hResult != NO_ERROR) return hResult;
			}
			else
			{
				DO_FOREVER
				{
					// if in bus reset processing, free for general packets (non-blocking)
					hResult = lhlMsgQueueGetPacketBlock(LHL_QUEUE_GENERAL, &packetBlock, TC_NO_WAIT);
					if (hResult != NO_ERROR) return hResult;

					if (pbIsValid(packetBlock))
					{
#if 1 
						pbSwapSrcDstNodeAddr(packetBlock);
#endif
						pbSetFinalStatus(packetBlock, E_LHL_BUS_RESET_IN_PROGRESS);
						pbPacketDone(packetBlock, PB_DONE_LHL_ATX_SND_PEND_2);
					}
					else
					{
						break; // no more packetBlocks to free
					}
				}
			}
		}

		if (pbIsValid(packetBlock))
		{
			briGetCurrentBusGeneration(&busGenCurrent);			
			pbGetBusGeneration (packetBlock, &busGenPacket);
			if (busGenCurrent != busGenPacket)
			{				
				pbSwapSrcDstNodeAddr(packetBlock);
				pbSetFinalStatus(packetBlock, E_LHL_BUS_RESET_IN_PROGRESS);
				pbPacketDone(packetBlock, PB_DONE_LHL_ATX_SND_PEND_1);
				hResult = E_BRI_NEW_BUS_RESET;
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}

			hResult = pbGetPacketHeader (packetBlock, &pHeader);
			if (hResult != NO_ERROR) return hResult;
			hResult = pbGetPacketTLabel (packetBlock, &tLabel);
			if (hResult != NO_ERROR) return hResult;
			hResult = pbGetPacketDstNodeAddr (packetBlock, &nodeAddr);
			if (hResult != NO_ERROR) return hResult;
			hResult = lhlPendingTxAdd(tLabel, nodeAddr, packetBlock);		// insert before send and remove if send error
			if (hResult != NO_ERROR) return hResult;    
			hResult = context1PostPacket(packetBlock,lhlTxDoneSemID);
			if (hResult != NO_ERROR)
			{
#if 1 
				pbSwapSrcDstNodeAddr(packetBlock);
#endif
				pbSetFinalStatus(packetBlock, hResult);
				pbPacketDone(packetBlock, PB_DONE_LHL_ATX_SND_PEND_3);
			}
			else
			{
				// We have now posted the write, the semaphore will be signaled
				uint8	matchResponse = FALSE; // error in sending req/rsp - match corresponding type in PendingTx
				pbIsResponsePacket(packetBlock, &matchResponse);

				hResult = TCSemaphoreWait(lhlTxDoneSemID);
				
				tCode = (pHeader->quadlets[0] & MASK_TCODE) >> SHIFT_TCODE;
				lhlTxPacketDoneLLC(tLabel, tCode, nodeAddr, matchResponse);
			}
		}
		else
		{
			break; // no more packets to send
		}
	}
	
	return hResult;
}

//instead of queue, only one outstanding phy packet possible.

static PB* pPendingPhyPacket;

static HRESULT lhlTxSndPhyPacketPend(void)
{
	HRESULT hResult;

	if (!pbIsValid(pPendingPhyPacket))
	{
		//There is no packet to send
		hResult = NO_ERROR;
		return hResult;
	}
	hResult = context1PostPacket(pPendingPhyPacket,lhlTxDoneSemID);
	if (hResult != NO_ERROR)
	{
		pbSetFinalStatus(pPendingPhyPacket, hResult);
		pbPacketDone(pPendingPhyPacket, PB_DONE_LHL_ATX_SND_PEND_3);
	}
	else
	{
		//wait for tx completion
		hResult = TCSemaphoreWait(lhlTxDoneSemID);
		volatile POSTED_TX_CONTEXT * pContext = &postedTxState.contexts[1];
		uint16		conf;

		if ((pContext->txStat & LLC_ASY_TX_CMPLT) == 0)
		{
			sysLogError(E_LLC_TX_NEVER_COMPLETED, __LINE__, moduleName);
		}
		conf = (pContext->txStat & LLC_ASY_TX_EVENT_MASK) >> LLC_ASY_TX_EVENT_SHIFT;
		if (conf==ACK_1394_RESERVED_0)
			pbSetFinalStatus(pPendingPhyPacket, (NO_ERROR));
		else
			pbSetFinalStatus(pPendingPhyPacket, (HRESULT) (E_LHL_CONF_BASE + conf));
		pbPacketDone(pPendingPhyPacket, PB_DONE_LHL_ATX_SND_DONE);
	}
	pPendingPhyPacket = NULL;
	return hResult;
}

//we need to wake up the PHY send routine when we are done
//
static HRESULT lhlPhyPacketCB(void * packetBlock)
{
	TCSemaphoreSignal(lhlTxPhyPckSemID);
	return NO_ERROR; //do not destroy packet
}


HRESULT lhlTxSndPhyPacket(QUADLET phyPacket)
{
	PB*			packetBlock = NULL;
	HRESULT hResult;	

	if (pPendingPhyPacket)
	{
		hResult = E_LHL_OUT_OF_PENDING_REQS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = pbCreatePacket (&packetBlock,0,lhlPhyPacketCB,0,0);
	if (hResult != NO_ERROR) return hResult;

	pbSetPacketType (packetBlock, PB_TYPE_PHY_PACKET);
	
	PB_HEADER*	pHeader;
	pbGetPacketHeader (packetBlock, &pHeader);
	pHeader->quadlets[0] = phyPacket;
	
	//now we have a phy packet, put it in the queue (simple queue)
	pPendingPhyPacket = packetBlock;
	lhlTxSndPendSignalSema();
	TCSemaphoreWait(lhlTxPhyPckSemID);
	//packet has been sent or failed.
	pbGetFinalStatus (packetBlock, &hResult);
	if (hResult!=NO_ERROR)
		sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

/*********************************************************
	wait for and send transmit packets
*/
void lhlTxSndPendSignalSema(void)
{
	// Signal to transmit packets thread to send packet
	TCSemaphoreSignal(lhlTxSndPendSemID);
}

void lhlTxSndPendThread(void *dummy)
{
	HRESULT hResult = NO_ERROR;

	UNUSED_ARG(dummy);

	DO_FOREVER
	{
		hResult = TCSemaphoreWait(lhlTxSndPendSemID);
		if (hResult != NO_ERROR) 
		{
			DO_FOREVER {}
		}
		lhlTxSndPhyPacketPend();
		lhlTxSndPacketPend();
	}
}

/*********************************************************
	Create semaphores for this module
*/
HRESULT lhlTxCreateSemaphores(void)
{
	HRESULT hResult = NO_ERROR;

	// transmit request packet done
	hResult = TCSemaphoreOpenIRQ(&lhlTxDoneSemID, 0);
	if (hResult != NO_ERROR) return hResult;

	// access to the transmit pending request table
	hResult = TCSemaphoreOpen(&lhlTxSndPendSemID, 0);
	if (hResult != NO_ERROR) return hResult;

	hResult = TCSemaphoreOpen(&lhlTxPhyPckSemID, 0);
	if (hResult != NO_ERROR) return hResult;


	return hResult;
}

/*********************************************************
	Resume interrupt handlers for this module
*/
HRESULT lhlTxResumeIntHandlers(void)
{
	HRESULT hResult = NO_ERROR;

	// transmit packet interrupt handler
	if ((hResult = llcIntEventAddCallback(LLCID_ASY_TX_CMPL, lhlTxIntHandler)) != NO_ERROR)
	{
		return hResult;
	}
	if ((hResult = llcIntEventAddIsr(LLC_INT_ISR_TX, lhlTxIsrHandler)) != NO_ERROR)
	{
		return hResult;
	}
	return hResult;
}

/*********************************************************
	Suspend interrupt handlers for this module
*/
HRESULT lhlTxSuspendIntHandlers(void)
{
	HRESULT hResult = NO_ERROR;

	// transmit packet interrupt handler
	if ((hResult = llcIntEventRemoveCallback(LLCID_ASY_TX_CMPL)) != NO_ERROR)
	{
		return hResult;
	}
	
	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT lhlTxResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// access to the transmit pending request table
	hResult = TCTaskResume(lhlTxSndPendThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT lhlTxSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// access to the transmit pending request table
	hResult = TCTaskSuspend(lhlTxSndPendThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Create tasks for this module
*/
HRESULT lhlTxInitialize(void)
{
	HRESULT hResult = NO_ERROR;

	initializeContexts();
	hResult = lhlTxCreateSemaphores();
	if (hResult != NO_ERROR) return hResult;

	// access to the transmit pending request table
	hResult = TCTaskCreate(&lhlTxSndPendThreadID, lhlTxSndPendThreadName, lhlTxSndPendThread, TASK_PRIORITY_TX, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

