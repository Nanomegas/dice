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
	
	Purpose:	LLC interrupt handler
				Handling of Samsung LLC interrupts
	
	Revisions:
		created 12/17/2002 kbj


****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "TCInterrupt.h"
#include "1394.h"
#include "llcReg.h"
#include "llcInt.h"
#include "llcDefs.h"
#include "targetBoard.h"
#include "sysDebug.h"

#include <cyg/hal/hal_platform_ints.h> //for interrupt number
#include <cyg/kernel/kapi.h>
#include <cyg/io/devtab.h>
#include <cyg/hal/plf_io.h>
#include <cyg/hal/hal_intr.h>


// Interrupt events from Samsung LLC controller
// These are DSR level handlers
static INTEVENT_CALLBACK			intEventCallback[LLC_INT_ITEMS]	__attribute__((section (".sram.llcint")));	// installed DSR handlers
static uint32									llcIntInstalledMask							__attribute__((section (".sram.llcint")));	// mask of installed DSR handlers
static uint32									llcIntEvents										__attribute__((section (".sram.llcint")));	// total number of ISR calls
static uint32									llcIntEvent[LLC_INT_ITEMS]			__attribute__((section (".sram.llcint")));	// counted DSR events
static uint32									llcIntState											__attribute__((section (".sram.llcint")));	// FSR state to handle BR processing

static INTEVENT_ISR_CALLBACK	intEventIsrCallback[LLC_INT_ISR_COUNT] __attribute__((section (".sram.llcint")));	// ISR event callbacks
static uint32									llcIsrInstalledMask 						__attribute__((section (".sram.llcint")));	// Mask of installed ISR
static volatile uint32							llcIsrDsrRxEvent							__attribute__((section (".sram.llcint")));	// Mask of ISR requesting DSR
static volatile uint32							llcIsrDsrTxEvent							__attribute__((section (".sram.llcint")));	// Mask of ISR requesting DSR

/*********************************************************************

	Callbacks attached to the INTERRUPT_REG_DP register bits

    (refer to  INTERRUPT_REG_DP register Samsung LLC User's Guide chap. 3.15)

#define LLCID_INTR_OUT					31		// ORing of all interrupts - if set then intr. pending
#define LLCID_PHY_INTR					30		// Status change in PHY - read register in PHY to read status
#define LLCID_PHY_REG_RXD				29		// PHY received a register data. Read data from PHY control reg.
#define LLCID_BUS_RST_RXD				28		// PHY received a reset status from the bus.
#define LLCID_ARB_RST_GAB				27		// Arb reset gan observed
#define LLCID_BUS_INIT_OVR			26		// Bus initialization process is over
#define LLCID_PING_INTR					25		// Ping response receive or ping error
#define LLCID_ASY_TX_CMPL				24		// Tx has completed the current asyn packet. Ready for next.
#define LLCID_ISO_TX_CMPL				23		// Tx has completed the one iso stream packet. Ready for next.
#define LLCID_ASY_RX_PKT_AVL		22		// Proper asyn packet without error is received (In AR fifo)
#define LLCID_RST_CMND_RX				21		// Reset command is received - appl reset 
#define LLCID_ATFIFO_STAT				20		// Asyn TX FIFO status changed
#define LLCID_ITFIFO_STAT				19		// Iso TX FIFO status changed
#define LLCID_ARFIFO_STAT				18		// Asyn RX FIFO status changed
#define LLCID_IRFIFO_STAT				17		// Iso RX FIFO status chagned
#define LLCID_DUB_EXPC_OCC_INTR	16		// Status changed due to bits in the DUB_EXPC reg.
#define LLCID_ISO_RX_PKT_AVL		15		// Isochronous receive packet arrived
#define LLCID_TX_BUSY_ACK				14		// Link sending a busy acknowledge
#define LLCID_HEADER_ERR				13		// Header error observed in the Asyn rx packet
#define LLCID_TCODE_ERR					12		// tcode error observed in the Asyn rx packet
#define LLCID_ASY_DATA_ERR			11		// Data CRC/length error in the Asyn rx packet
#define LLCID_INC_SEC_COUNT			10		// Cycle count reaches 7999 (1 sec.)
#define LLCID_CY_START					9			// Cycle start send or received
#define LLCID_CY_DONE						8			// Isochronous cycle is completed
#define LLCID_CY_TOO_LONG				7			// Iso cycle has exceeded the limit
#define LLCID_CY_LOST						6			// Count rolls over twice without a cycle start
#define LLCID_ISO_TX_ERR				5			// Error in iso transmission
#define LLCID_ISO_RX_ERR				4			// Mismatch in the data length of the iso rx
#define LLCID_HOST_ACSS_ERR			3			// Error access to non avaiable FIFO or during retry
#define LLCID_ISO_HDR_RCVD			2			// Isoch stream packet header received
#define LLCID_NO_BUS_GRANT			1			// Could not request bus - lost sync. with PHY or speed mismatch
#define LLCID_APP_GEN_INTR			0			// Interrupt generated by the application layer

************************************************************************/

#define	LLCINT_EVENT_GENERAL_ITEMS		15	// list of prioritised active interrupts not including bus reset ones


uint8 llcIntEventGeneral[LLCINT_EVENT_GENERAL_ITEMS] = 
{
	LLCID_ASY_TX_CMPL,
	LLCID_ASY_RX_PKT_AVL,
	LLCID_RST_CMND_RX,
	LLCID_HEADER_ERR,
	LLCID_TCODE_ERR,
	LLCID_ASY_DATA_ERR,
	LLCID_INC_SEC_COUNT,
	LLCID_CY_TOO_LONG,
	LLCID_CY_LOST,
	LLCID_ISO_TX_ERR,
	LLCID_ISO_RX_ERR,
	LLCID_DUB_EXPC_OCC_INTR,
	LLCID_HOST_ACSS_ERR,
	LLCID_NO_BUS_GRANT,
	LLCID_APP_GEN_INTR
};


static const char llcIntEventIntNames[LLC_INT_ITEMS][32] =
{
	"LLC_APP_GEN_INTR      ",
	"LLC_NO_BUS_GRANT      ",		
	"LLC_ISO_HDR_RCVD      ",		
	"LLC_HOST_ACSS_ERR     ",	
	"LLC_ISO_RX_ERR        ",	
	"LLC_ISO_TX_ERR        ",	
	"LLC_CY_LOST           ",		
	"LLC_CY_TOO_LONG       ",			
	"LLC_CY_DONE           ",			
	"LLC_CY_START          ",		
	"LLC_INC_SEC_COUNT     ",		
	"LLC_ASY_DATA_ERR      ",	
	"LLC_TCODE_ERR         ",	
	"LLC_HEADER_ERR        ",	
	"LLC_TX_BUSY_ACK       ",		
	"LLC_ISO_RX_PKT_AVL    ",
	"LLC_DUB_EXPC_OCC_INTR ",
	"LLC_IRFIFO_STAT       ",
	"LLC_ARFIFO_STAT       ",
	"LLC_ITFIFO_STAT       ",			
	"LLC_ATFIFO_STAT       ",			
	"LLC_RST_CMND_RX       ",			
	"LLC_ASY_RX_PKT_AVL    ",
	"LLC_ISO_TX_CMPL       ",	
	"LLC_ASY_TX_CMPL       ",	
	"LLC_PING_INTR         ",
	"LLC_BUS_INIT_OVR      ",
	"LLC_ARB_RST_GAB       ",
	"LLC_BUS_RST_RXD       ",
	"LLC_PHY_REG_RXD       ",
	"LLC_PHY_INTR          ",
	"LLC_INTR_OUT          "
};

BOOL llcIntEventIsIntEnabled(uint32 index)
{
	uint32 intEventReg;
	uint32 bit;

	if (index >= LLC_INT_ITEMS)
	{
		return FALSE;
	}

	bit = LLC_INT_BIT(index);

	llcLinkRegRead(INTR_MASK_REG_SET_DP, &intEventReg);

	return ((bit & intEventReg) == bit);
}

const char *llcIntEventGetIntName(uint32 index)
{
	if (index >= LLC_INT_ITEMS)
	{
		index = 0;
	}

	return llcIntEventIntNames[index];
}

uint32 llcIntEventGetIntCount(uint32 index)
{
	if (index >= LLC_INT_ITEMS)
	{
		return 0;
	}

	return llcIntEvent[index];
}

uint32 llcIntEventGetIntCounts(void)
{
	return llcIntEvents;
}

void llcIntEventClearIntCounts(void)
{
	uint32 i;

	for (i = 0; i < LLC_INT_ITEMS; i++)
	{
		llcIntEvent[i] = 0;
	}
	llcIntEvents = 0;
}


_TC_INLINE_STATIC uint32 _llcIntEventGetState(void) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC uint32 _llcIntEventGetState(void)
{
	return llcIntState;
}


_TC_INLINE_STATIC void _llcIntEventSetState(uint32 state) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void _llcIntEventSetState(uint32 state)
{	
	llcIntState = state;
	if (state != LLC_INT_STATE_PROCESS_NORMAL)
		llcLinkRegWrite(INTR_MASK_REG_CLEAR_DP, HANDLE_BY_ISR);
	else
		llcLinkRegWrite(INTR_MASK_REG_SET_DP, llcIsrInstalledMask);
	
	lhlBrInOutHandler (state!=LLC_INT_STATE_PROCESS_NORMAL);
}

uint32 llcIntEventGetState(void)
{
	return _llcIntEventGetState();
}

HRESULT llcIntEventSetState(uint32 state)
{
	_llcIntEventSetState(state);
	return NO_ERROR;
}


_TC_INLINE_STATIC void _llcIntEventCallCallback(uint32 idx) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC void _llcIntEventCallCallback(uint32 idx)
{
	if (intEventCallback[idx]!= NULL)
	{
		(*intEventCallback[idx])();
	}
}




/*
	INTERRUPT_REG_DP Register Handler 
	must be called when any INTERRUPT_REG_DP register interrupt is generated
*/


/*
llcInstalledMask

Normal operation:
	ISR: read interrupts
			 mask non ISR interrupts
			 clear ISR interrupts
			 Check ISR events
			 Call handlers
			 if DSR needed set some flag


	 DSR: read interrupts
				clear non ISR interrupts
				Procede as usual
				unmask non ISR interrupts

Special operation
	In case we are entering bus reset processing
	we need to disable the RX_PKT interrupt because the
	bus reset handlers will read the reset bundle etc.
	This is the hack:
		In ISR check for BUS_RST_RXD and if it is detected
		mask the RX_PKT interrupt
		
		In llcIntEventSetState unmask RX_PKT interrupt if mode is NORMAL


*/



_TC_INLINE_STATIC BOOL _llcIntEventIsrCallCallback(uint32 idx) _TC_INLINE_ALWAYS;
_TC_INLINE_STATIC BOOL _llcIntEventIsrCallCallback(uint32 idx)
{
	if (intEventIsrCallback[idx]!= NULL)
	{
		return (*intEventIsrCallback[idx])();
	}
	return FALSE;
}



uint32 llc_interrupt_isr(uint32 vector, void *data) __attribute__((section (".sram.llccode")));

uint32 llc_interrupt_isr(uint32 vector, void *data)
{
	uint32 intEventReg;
	BOOL bDsr = FALSE;
	
	llcLinkRegRead(INTERRUPT_REG_CLEAR_DP, &intEventReg); //events which are not masked
	if ((intEventReg & LLC_INT_BIT(LLCID_INTR_OUT)) == 0) return CYG_ISR_HANDLED;

	//mask all those handled by DSR, DSR will unmask when done
	bDsr = intEventReg & llcIntInstalledMask;
	if (bDsr) llcLinkRegWrite(INTR_MASK_REG_CLEAR_DP, ~(HANDLE_BY_ISR | LLC_INT_BIT(LLCID_INTR_OUT)));
	
	// Clear the ones we are handeling here
	llcLinkRegWrite(INTERRUPT_REG_CLEAR_DP, intEventReg&(HANDLE_BY_ISR | LLC_INT_BIT(LLCID_INTR_OUT)));

	llcIntEvents++;

	//we need to look for bus reset and if we see it we must mask out LLCID_ASY_RX_PKT_AVL and leave it
	//to the DSR BR handler to read whatever is in the FIFO
	//The LLCID_BUS_RST_RXD might be masked waiting for DSR so we need to check the raw status
	uint32 rawEvent;
	llcLinkRegRead(INTERRUPT_REG_DP, &rawEvent);
	if (rawEvent & LLC_INT_BIT(LLCID_BUS_RST_RXD))
	{
		//at some point the DSR will handle this, we really don't need and ISR processing for now
		llcLinkRegWrite(INTR_MASK_REG_CLEAR_DP, HANDLE_BY_ISR);
		//we have to rely on the DSR putting us back into normal mode
	}
	else
	{
		if (intEventReg & LLC_INT_BIT(LLCID_ASY_RX_PKT_AVL))
			if (_llcIntEventIsrCallCallback(LLC_INT_ISR_RX)) llcIsrDsrRxEvent = 1;
		if (intEventReg & LLC_INT_BIT(LLCID_ASY_TX_CMPL))
			if (_llcIntEventIsrCallCallback(LLC_INT_ISR_TX)) llcIsrDsrTxEvent = 1;
			
	}
	//as long as we have unhandled Isr->dsr's keep calling the DSR.
	if (llcIsrDsrRxEvent || llcIsrDsrTxEvent) bDsr = 1;
	
	return bDsr ? CYG_ISR_CALL_DSR : CYG_ISR_HANDLED;
}

void llc_interrupt_dsr(uint32 vector, uint32 count, void * data) __attribute__((section (".sram.llccode")));

void llc_interrupt_dsr(uint32 vector, uint32 count, void * data)
{
	uint32 intEventReg;

	llcLinkRegRead(INTERRUPT_REG_DP, &intEventReg); //read raw interrupt stat
	intEventReg &=llcIntInstalledMask; //and with handled stat

	 //is there any ISR requests
	if (llcIsrDsrTxEvent)
	{
		intEventReg |= LLC_INT_BIT(LLCID_ASY_TX_CMPL);
		llcIsrDsrTxEvent = 0;
	}
	if (llcIsrDsrRxEvent)
	{
		intEventReg |= LLC_INT_BIT(LLCID_ASY_RX_PKT_AVL);
		llcIsrDsrRxEvent = 0;
	}
	fSYS_TRACE1(SYSDEBUG_TRACE_TESTS,intEventReg);	
	if (!intEventReg) 
	{
		//should never happen
		llcLinkRegWrite(INTR_MASK_REG_SET_DP, llcIntInstalledMask & ~HANDLE_BY_ISR);
		return;
	}

	// Clear all pending interrupts serviced
	llcLinkRegWrite(INTERRUPT_REG_CLEAR_DP, intEventReg & ~(HANDLE_BY_ISR | LLC_INT_BIT(LLCID_INTR_OUT)));
	

	if (llcIntState == LLC_INT_STATE_WAIT_BUSRESET ||
		llcIntState == LLC_INT_STATE_PROCESS_NORMAL ||
		llcIntState == LLC_INT_STATE_PROCESS_SI)
	{
		if (intEventReg & LLC_INT_BIT(LLCID_BUS_RST_RXD)) //new bus reset
		{
			_llcIntEventSetState(LLC_INT_STATE_WAIT_SI_COMPLETE);
			llcIntEvent[LLCID_BUS_RST_RXD]++;
			_llcIntEventCallCallback(LLCID_BUS_RST_RXD);	 
		}
		
	}
	if (llcIntState == LLC_INT_STATE_WAIT_SI_COMPLETE)
	{
		if (intEventReg & LLC_INT_BIT(LLCID_BUS_INIT_OVR)) //new selfid complete 
		{
			llcIntEvent[LLCID_BUS_INIT_OVR]++;
			_llcIntEventSetState(LLC_INT_STATE_PROCESS_SI);
			_llcIntEventCallCallback(LLCID_BUS_INIT_OVR);
		}
	}
	if (llcIntState == LLC_INT_STATE_PROCESS_NORMAL)
	{
		uint8 i;
		uint8 id;
		//only handle general items for optimization
		for (i = 0; i < LLCINT_EVENT_GENERAL_ITEMS; i++)
		{
			id = llcIntEventGeneral[i];
			if (intEventReg & LLC_INT_BIT(id))
			{
				llcIntEvent[id]++;
				_llcIntEventCallCallback(id);
				intEventReg &= ~LLC_INT_BIT(id);
				if (intEventReg == 0) break;
			}
		}
	}
	llcLinkRegWrite(INTR_MASK_REG_SET_DP, llcIntInstalledMask & ~(HANDLE_BY_ISR | LLC_INT_BIT(LLCID_INTR_OUT)));
}

HRESULT llcIntEventInitCallbacks(void)
{
	HRESULT hResult = NO_ERROR;
	uint32 i;

	for (i = 0; i < LLC_INT_ITEMS; i++)
	{
		intEventCallback[i] = NULL;
	}
	llcLinkRegWrite(INTR_MASK_REG_CLEAR_DP, LLC_ALL_INTR_MASK); // Clear all LLC IRQ enables
	llcIntInstalledMask = 0;

	return hResult;
}

HRESULT llcIntEventAddCallback(uint32 idx, INTEVENT_CALLBACK callback)
{
	HRESULT hResult = NO_ERROR;

	if (idx < LLC_INT_ITEMS)
	{
		if (intEventCallback[idx] == NULL)
		{
			intEventCallback[idx] = callback;
			llcIntInstalledMask |=LLC_INT_BIT(idx);
			llcLinkRegWrite(INTR_MASK_REG_SET_DP, LLC_INT_BIT(idx));
		}
		else
		{
			hResult = E_LLC_INTEVENT_CB_ERR;
		}
	}
	else
	{
		hResult = E_LLC_INTEVENT_CB_RANGE_ERR;
	}

	return hResult;
}

HRESULT		llcIntEventAddIsr(uint32 idx, INTEVENT_ISR_CALLBACK callback)
{
	HRESULT hResult = NO_ERROR;

	if (idx <= LLC_INT_ISR_TX)
	{
		if (intEventIsrCallback[idx] == NULL)
		{
			intEventIsrCallback[idx] = callback;
			llcIsrInstalledMask |=LLC_INT_BIT((idx==LLC_INT_ISR_RX)?LLCID_ASY_RX_PKT_AVL:LLCID_ASY_TX_CMPL);
		}
		else
		{
			hResult = E_LLC_INTEVENT_CB_ERR;
		}
	}
	else
	{
		hResult = E_LLC_INTEVENT_CB_RANGE_ERR;
	}

	return hResult;

}

HRESULT llcIntEventGetCallback(uint32 idx, INTEVENT_CALLBACK *callback)
{
	HRESULT hResult = NO_ERROR;

	if (idx < LLC_INT_ITEMS)
	{
		*callback = intEventCallback[idx];
	}
	else
	{
		hResult = E_LLC_INTEVENT_CB_RANGE_ERR;
	}

	return hResult;
}


HRESULT llcIntEventRemoveCallback(uint32 idx)
{
	HRESULT hResult = NO_ERROR;

	if (idx < LLC_INT_ITEMS)
	{
		if (intEventCallback[idx] != NULL)
		{
			intEventCallback[idx] = NULL;
			llcIntInstalledMask &= ~LLC_INT_BIT(idx);
			llcLinkRegWrite(INTR_MASK_REG_CLEAR_DP, LLC_INT_BIT(idx));
		}
		else
		{
			hResult = E_LLC_INTEVENT_CB_ERR;
		}
	}
	else
	{
		hResult = E_LLC_INTEVENT_CB_RANGE_ERR;
	}

	return hResult;
}

HRESULT	llcIntEventDisableMask(void)
{
  // clear all bits in INTR_MASK_REG_CLEAR_DP register
	llcLinkRegWrite(INTR_MASK_REG_CLEAR_DP, LLC_ALL_INTR_MASK);
	return NO_ERROR;
}

HRESULT llcIntEventClearBusReset(uint32 state)
{
	_llcIntEventSetState(state);

	return NO_ERROR;
}

HRESULT llcIntEventNewBusReset(BOOL bDuringCriticalBR)
{
	HRESULT hResult = NO_ERROR;
	
	if (llcIntState == LLC_INT_STATE_PROCESS_NORMAL)
	{
		_llcIntEventSetState(LLC_INT_STATE_WAIT_BUSRESET);
	}

	if (bDuringCriticalBR == TRUE)
	{
		// No action needed
	}

	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT llcIntResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT llcIntSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;
	return hResult;
}

/*********************************************************
	Create tasks for this module
*/
HRESULT llcIntInitializeTasks(void)
{
	HRESULT hResult = NO_ERROR;
	return hResult;
}

/**********************************************************************

 	LLC

***********************************************************************/

/*
	Initialize this module
	Add interrupt procedure to vector addresses
*/
HRESULT llcIntInitialize(void)
{
	HRESULT hResult = NO_ERROR;

	llcIntState = LLC_INT_STATE_WAIT_BUSRESET;

	hResult = llcIntEventInitCallbacks();
	if (hResult != NO_ERROR) return hResult;

	// Clear all pending interrupts 
	llcLinkRegWrite(INTERRUPT_REG_CLEAR_DP, LLC_ALL_INTR_MASK);

	//TCInterruptInstallHandler(CYGNUM_HAL_INTERRUPT_SLINK, NULL, &llcIntEventHandler);
	hResult = TCInterruptInstallEcosHandler(CYGNUM_HAL_INTERRUPT_SLINK, &llc_interrupt_isr, &llc_interrupt_dsr);
	//hResult = TCInterruptCreateLLC(&llcIntEventHandler);
	if (hResult != NO_ERROR) return hResult;

	// Enable interrupts from Samsung LLC

	llcLinkRegWrite(INTR_MASK_REG_SET_DP, LLC_INT_BIT(LLCID_INTR_OUT));

	return hResult;
}
