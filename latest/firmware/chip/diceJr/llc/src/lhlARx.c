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
 * DESCRIPTION: Link Hardware Abstraction Layer receive packet i/o

	Purpose:	Link Hardware Abstraction Layer receive packet i/o
						handling for asynchronous receive packets
						for the Samsung LLC
	
	Revisions:
		created 12/18/2002 kbj

****************************************************************************/

#include "TCTasking.h"
#include "1394.h"
#include "packetBlock.h"
#include "lalDefs.h"
#include "lalAddrRng.h"
#include "lalAsyncStream.h"
#include "lhl.h"
#include "lhlARx.h"
#include "lhlCli.h"
#include "lhlLLC.h"
#include "lhlRsp.h"
#include "lhlStatistics.h"
#include "llcReg.h"
#include "llcInt.h"
#include "llcFIFO.h"
#include "llcUtils.h"
#include "sysDebug.h"

// semaphore IDs for this module
uint32					lhlRxDataSemID;			// hardware Rx request data available semaphore 

// task (thread) IDs for this module
uint32					lhlRxDataThreadID;		// hardware Tx request data available thread
const char*				lhlRxDataThreadName = "lhlRxDataThread";

/****************************************************************************
 ***************************************************************************

	Packet buffer queue used by receiver

 ***************************************************************************
****************************************************************************/
typedef struct 
{
	uint32	size;						// Size of packet data
	QUADLET data[LLC_ASY_RX_DEPTH];		// Raw packet data from LLC fifo
} PACKET;

#ifndef MAX_PACKET_BUFS
#define MAX_PACKET_BUFS	5
#endif

typedef struct
{										// Buffer is empty when in == out
	uint32		in;						// Next free packet buffer incremented by interrupt 
	uint32		out;					// Next packet to be processed by receive task
	uint32		full;					// 1 if all packet buffers filled with data
	PACKET		packet[MAX_PACKET_BUFS];
	uint32      errorFlag;				// A way for the Int routine to log errors
} PACKET_BUFS;

#define LHL_ARX_OVERFLOW	0x00000001


static PACKET_BUFS packetRxBufs __attribute__((section (".sram.llcdata")));

static QUADLET fiqWrResponse[3] __attribute__((section (".sram.llcdata")));

static void lhlInitRxBuf(void)
{
	packetRxBufs.in = 0;
	packetRxBufs.out = 0;
	packetRxBufs.full = 0;
	packetRxBufs.errorFlag = 0;
}

_TC_INLINE_STATIC void lhlSetError (uint32 evt) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void lhlSetError (uint32 evt)
{
	packetRxBufs.errorFlag |= evt;
}

_TC_INLINE_STATIC uint32 lhlGetError (void) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC uint32 lhlGetError (void)
{
	uint32 evt = packetRxBufs.errorFlag;
	packetRxBufs.errorFlag = 0;
	return evt;
}



_TC_INLINE_STATIC int lhlGetRxInBuf(PACKET **pPacket) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC int lhlGetRxInBuf(PACKET **pPacket)
{
	uint32 next;

	if (packetRxBufs.full) {
		return 0;
	}

	next = packetRxBufs.in + 1;
	if (next == MAX_PACKET_BUFS) next = 0;

	*pPacket = &packetRxBufs.packet[packetRxBufs.in];
	
	if (next == packetRxBufs.out) 
	{
		packetRxBufs.full = 1;
		return 0;
	}

	packetRxBufs.in = next;
	return 1;
}

_TC_INLINE_STATIC int lhlPacketInBuf(void) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC int lhlPacketInBuf(void)
{
	// Buffer is empty
	if ((packetRxBufs.out == packetRxBufs.in) && !packetRxBufs.full) return 0;
	return 1;
}

_TC_INLINE_STATIC void lhlGetOutBuf(PACKET **pPacket) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void lhlGetOutBuf(PACKET **pPacket)
{
	*pPacket = &packetRxBufs.packet[packetRxBufs.out];
}

_TC_INLINE_STATIC void lhlNextOutBuf(void) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void lhlNextOutBuf(void)
{
	uint32 next;

	next = packetRxBufs.out + 1;
	if (next == MAX_PACKET_BUFS) next = 0;	
	packetRxBufs.out = next;
	packetRxBufs.full = 0;
}


/*********************************************************
	called when the rx thread is notified of a packet in the rx fifo
		this function gets a packet, creates a packet block when necessary, and
		dispatches it to the appropriate module, based on previous address 
		allocations.  also, when the packet is a confirmation to a previous
		request, the appropriate callback is executed.
*/
HRESULT lhlRxGetPacket(void)
{
	HRESULT hResult = NO_ERROR;
	PACKET  *pPacket;

	
	while (lhlPacketInBuf())
	{
		lhlGetOutBuf(&pPacket);
		hResult = llcProcessRxPacket(pPacket->data, pPacket->size);
		lhlNextOutBuf();
	}
	return hResult;
}


/*********************************************************
	called when an AR request/response interrupt occures
	copies packet from llc FIFO buffer to next free packetRxBufs
*/

typedef struct
{
	uint8	hlen;	//length of header 0=ignore
	uint8	payload; //ptr to payload quad, 255 if no payload
} LLC_RX_PKT_DEF;

static uint32 discardTlabelAbove = 63;
	
HRESULT		lhlRxDiscardTlabelAbove (uint32 tlabel)
{
	discardTlabelAbove =tlabel;
	return NO_ERROR;
}

static const LLC_RX_PKT_DEF rxhdrlen[16] =
{
	{4,255},	//TCODE_WRQUAD			Includes one data quadlet
	{4,3},		//TCODE_WRBLOCK			Length in h[3]>>16
	{3,255},	//TCODE_WRRESPONSE	No further data
	{0,255},	//TCODE_RESERVED_1	
	{3,255},	//TCODE_RDQUAD			No further data
	{4,255},	//TCODE_RDBLOCK			No Further data
	{4,255},	//TCODE_RDQUADRSP		Includes one data quadlet
	{4,3},		//TCODE_RDBLOCKRSP	Length in h[3]>>16
	{0,255},	//TCODE_CYCLESTART	
	{4,3},		//TCODE_LOCKRQST		Length in h[3]>>16
	{1,0},		//TCODE_ISOBLOCK		Length in h[0]>>16
	{4,3},		//TCODE_LOCKRSP			Length in h[3]>>16
	{0,255},	//TCODE_RESERVED_2	
	{0,255},	//TCODE_RESERVED_3	
	{3,255},	//TCODE_LLC_SPECIFIC	Always 3 for this LLC
	{0,255}		//TCODE_RESERVED_4	
};



BOOL lhlRxIsrHandler(void) __attribute__((section (".sram.llccode")));

BOOL lhlRxIsrHandler(void)
{
	PACKET  *pPacket;
	uint32  fifoStat;
	uint32  n;
	uint16	tCode;
	uint32 * pDst;
	uint32   head;
	
#ifdef _STATISTICS
	lhlStatistics.RxReqInt++;
#endif //_STATISTICS
	llcLinkRegRead(ARFIFO_STAT_REG_DP, &fifoStat);
	if (fifoStat & LLC_ARFIFO_MT) return FALSE;
	//we need to read the first quad to find out if it is a header
	llcLinkRegRead(ASY_RX_FIFO_REG_DP, &head);
	llcLinkRegRead(ARFIFO_STAT_REG_DP, &fifoStat);	
	if (fifoStat & LLC_ARFIFO_HEAD)
	{
		//Check the packet
		tCode = (uint16)((head&MASK_TCODE)>>SHIFT_TCODE);
		n = rxhdrlen[tCode].hlen;
		if ((tCode == TCODE_WRRESPONSE) && (((head&MASK_TLABEL) >>SHIFT_TLABEL ) > discardTlabelAbove))
		{
			//write somewhere to tell we got this response
			//we will read the packet
			llcGetContext(fiqWrResponse,n);
			return FALSE;
		}
		if (n)
		{
			//okay, we want it
			n--;
			if (lhlGetRxInBuf(&pPacket))
			{
				//and we have space for it
				pDst = pPacket->data;
				pPacket->size = n+1;
				*pDst++ = head;
				//while (n--)
				//	llcLinkRegRead(ASY_RX_FIFO_REG_DP, pDst++);
				llcGetContext(pDst,n);
				pDst+=n;
				
				//so we got the header, how much is left				
				if (rxhdrlen[tCode].payload!=255)
				{			
					n = (pPacket->data[rxhdrlen[tCode].payload] >> 16) & 0xffff;
					n = (n+3)>>2; //quadlets
					pPacket->size += n;
					
					///TODO: make sure it is not too big
					//while (n--)
					//	llcLinkRegRead(ASY_RX_FIFO_REG_DP, pDst++);
					llcGetContext(pDst,n);
					pDst+=n;
				}
				//now we should have the trailer
				llcLinkRegRead(ASY_RX_FIFO_REG_DP, pDst++);
				pPacket->size++;
				//let's make sure it was the trailer
				llcLinkRegRead(ARFIFO_STAT_REG_DP, &fifoStat);
				if ((fifoStat & LLC_ARFIFO_TAIL))
				{					
					//we handled one packet, hopefully we will get here again if there is more
					llcLinkRegWrite(INTERRUPT_REG_CLEAR_DP, LLC_INT_BIT(LLCID_ASY_RX_PKT_AVL));
					fSYS_TRACE1(SYSDEBUG_TRACE_TESTS,head);
					return TRUE; //Tell DSR to signal semaphore
				}
			}
  		else
		  {
			  lhlSetError (LHL_ARX_OVERFLOW);
		  }
		}
	}
	//if we get here we either didn't want the packet
	//didn't have space for it or an error was detected
#ifdef _STATISTICS
	lhlStatistics.RxNoHeader++;
#endif //_STATISTICS

	//read the fifo until a footer is read
	while ((!(fifoStat & LLC_ARFIFO_MT)) && (!(fifoStat & LLC_ARFIFO_TAIL)))
	{	
		llcLinkRegRead(ASY_RX_FIFO_REG_DP, &head);
		llcLinkRegRead(ARFIFO_STAT_REG_DP, &fifoStat);
	}
	return TRUE; //so we can indicate the error
}

void lhlRxDsrHandler(void)
{
	TCSemaphoreSignal(lhlRxDataSemID);
}


/*********************************************************
	Thread waiting for and process incoming request/response packets
*/
void lhlRxDataThread(void *dummy)
{
	HRESULT hResult = NO_ERROR;
	
	UNUSED_ARG(dummy);

	DO_FOREVER
	{
		hResult = TCSemaphoreWait(lhlRxDataSemID);		
		if (hResult != NO_ERROR) 
		{
			DO_FOREVER {}
		}
		lhlRxGetPacket();
		uint32 evt = lhlGetError();
		if (evt & LHL_ARX_OVERFLOW)
		{
			sysLogError(E_LLC_RX_OVERFLOW, __LINE__, __FILE__ );
		}
	}
}

/*********************************************************
	Create semaphores for this module
*/
HRESULT lhlRxCreateSemaphores(void)
{
	HRESULT hResult = NO_ERROR;

	// received request/response packet data
	hResult = TCSemaphoreOpenIRQ(&lhlRxDataSemID, 0);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Resume interrupt handlers for this module
*/
HRESULT lhlRxResumeIntHandlers(void)
{
	HRESULT hResult = NO_ERROR;

	// receive request/response packet interrupt handler
	
	// The Isr handler will not enable the interrupt
	if ((hResult = llcIntEventAddIsr(LLC_INT_ISR_RX, lhlRxIsrHandler)) != NO_ERROR)
	{
		return hResult;
	}
	// The Dsr handler will enable the interrupt
	if ((hResult = llcIntEventAddCallback(LLCID_ASY_RX_PKT_AVL, lhlRxDsrHandler)) != NO_ERROR)
	{
		return hResult;
	}

	return hResult;
}

/*********************************************************
	Suspend interrupt handlers for this module
*/
HRESULT lhlRxSuspendIntHandlers(void)
{
	HRESULT hResult = NO_ERROR;

	// receive request/response packet interrupt handler
	if ((hResult = llcIntEventRemoveCallback(LLCID_ASY_RX_PKT_AVL)) != NO_ERROR)
	{
		return hResult;
	}

	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT lhlRxResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// receive request/response packet available
	hResult = TCTaskResume(lhlRxDataThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT lhlRxSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// receive request/response packet available
	hResult = TCTaskSuspend(lhlRxDataThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Create tasks for this module
*/
HRESULT lhlRxInitialize(void)
{
	HRESULT hResult = NO_ERROR;

	lhlInitRxBuf();

	hResult = lhlRxCreateSemaphores();
	if (hResult != NO_ERROR) return hResult;

	// receive request packet available
	hResult = TCTaskCreate(&lhlRxDataThreadID, lhlRxDataThreadName, lhlRxDataThread, TASK_PRIORITY_RX, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}
