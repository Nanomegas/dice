//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004 - 2011 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//

//This is code for an UART midi device
#ifdef _AML

#include "TCTypes.h"
#include "aml.h"
#include "TCSemaphore.h"
#include "TCDebug.h"

#include <cyg/hal/plf_io.h>
#include <cyg/hal/hal_platform_ints.h>
#include <cyg/hal/drv_api.h>
#include "TCTasking.h"
#include "TCInterrupt.h"
#include "uartAml.h"

#define MPTR(x) (*((volatile uint32 *) (x)))
static uint32 uart1_midi_dev_id; 
static uint32 uart_port;
static uint8 uart1_rx_activity;
static uint8 uart1_tx_activity;

#define UART1_MIDI_TX_FIFO_MAX_ENTRIES	16		// must be a power of 2

static uint8 uart1_midi_tx_fifo[UART1_MIDI_TX_FIFO_MAX_ENTRIES];
static uint8 uart1_midi_tx_fifo_head;
static uint8 uart1_midi_tx_fifo_tail;
static uint8 uart1_midi_tx_fifo_numEntries;

#define UART1_MIDI_TX_FIFO_MASK		(UART1_MIDI_TX_FIFO_MAX_ENTRIES - 1)


void uart1_dsr(void)
{
	uint32	dummy;
	uint32	statusReg;

	// clear interrupt and read status
	dummy = MPTR(UART1_IIR);
	statusReg = MPTR(UART1_LSR);

	// check if character received
	if (statusReg & 0x01)
	{
		if (statusReg & 0x02)
		{
			// receiver overrun error
			SYS_TRACE2(SYSDEBUG_TRACE_AML, uart1_midi_dev_id, statusReg);
		}

		// read data
		dummy = MPTR(UART1_RBR);	
		//SYS_TRACE2(SYSDEBUG_TRACE_AML, uart1_midi_dev_id, dummy);
		uart1_rx_activity = 1;
		amlDrvIndRxByte(uart1_midi_dev_id, dummy);
	}

	// while the transmitter holding register is empty and there are bytes in the transmit FIFO, write one
	while ((statusReg & 0x20) && uart1_midi_tx_fifo_numEntries)
	{
		MPTR(UART1_THR) = uart1_midi_tx_fifo[uart1_midi_tx_fifo_tail] & 0xff;
		uart1_midi_tx_fifo_tail = (uart1_midi_tx_fifo_tail + 1) & UART1_MIDI_TX_FIFO_MASK;
		--uart1_midi_tx_fifo_numEntries;
		uart1_tx_activity = 1;
		statusReg = MPTR(UART1_LSR);
	}
}


uint8 uart1_activity (void)
{
	uint8 r=0;
	if (uart1_rx_activity)
	{
		r=UART_RX_ACTIVITY;
		uart1_rx_activity = 0;
	}
	if (uart1_tx_activity)
	{
		r|=UART_TX_ACTIVITY;
		uart1_tx_activity = 0;
	}
	return r;
}


void uart1_enable(uint32 dummy)
{
	MPTR(ICTL_ENABLE) |= 1 << CYGNUM_HAL_INTERRUPT_UART1;
}


void uart1_disable(uint32 dummy)
{
	MPTR(ICTL_ENABLE) &= ~(1 << CYGNUM_HAL_INTERRUPT_UART1);
}


void uart1MidiDev_init(uint32 dev_id, uint32 arg, uint32 *handle)
{
	uint32 baud_divisor = 99;
	uint32 dummy;

	uart1_disable(0); //disable it first
	uart1_midi_dev_id = dev_id;
	uart_port = arg; //only needed when it support uart0.

	memset( uart1_midi_tx_fifo, 0, sizeof(uart1_midi_tx_fifo) );
	uart1_midi_tx_fifo_head = 0;
	uart1_midi_tx_fifo_tail = 0;
	uart1_midi_tx_fifo_numEntries = 0;

	// write to baud rate register
	MPTR(UART1_LCR) = 0x80;
	MPTR(UART1_DLL) = baud_divisor & 0xFF;
	MPTR(UART1_DLH) = (baud_divisor & 0xFF00) >> 8;
	MPTR(UART1_LCR) = 0x00;

	// select 8 bits/char, 1 stop bit and no parity bit (LCR)
	MPTR(UART1_LCR) = 0x03;
	// clear "loop back mode"
	MPTR(UART1_MCR) = 0x00;

	// enable FIFO's and set RCVR Trigger = 1 char (FCR)
	MPTR(UART1_FCR) = 0x01;

	// clear potential THR empty interrupt
	dummy = MPTR(UART1_IIR);

	// enable Rx and Tx interrupts
	MPTR(UART1_IER) = 0x03;

	// create interrupt
	*handle = 1;
	TCInterruptInstallHandler(CYGNUM_HAL_INTERRUPT_UART1, NULL, &uart1_dsr);
}


bool uartMidiDev_putChar(uint32 dummy, uint8 data)
{
	uint32 txstatus;

	//SYS_TRACE2(SYSDEBUG_TRACE_AML, dummy, data);

	cyg_drv_dsr_lock();
	txstatus = MPTR(UART1_LSR);

	// if the uart's transmitter holding register is full, push the caller's byte into the FIFO if it is not full
	if (!(txstatus & 0x20))
	{
		bool status = false;

		if (uart1_midi_tx_fifo_numEntries <= UART1_MIDI_TX_FIFO_MAX_ENTRIES)
		{
			uart1_midi_tx_fifo[uart1_midi_tx_fifo_head] = data;
			uart1_midi_tx_fifo_head = (uart1_midi_tx_fifo_head + 1) & UART1_MIDI_TX_FIFO_MASK;
			++uart1_midi_tx_fifo_numEntries;
			status = true;
		}

		cyg_drv_dsr_unlock();
		return status;
	}

	if (uart1_midi_tx_fifo_numEntries)
	{
		// pull the next byte to write from the FIFO
		MPTR(UART1_THR) = uart1_midi_tx_fifo[uart1_midi_tx_fifo_tail] & 0xff;
		uart1_midi_tx_fifo_tail = (uart1_midi_tx_fifo_tail + 1) & UART1_MIDI_TX_FIFO_MASK;
		uart1_tx_activity = 1;

		// push the caller's byte into the FIFO
		uart1_midi_tx_fifo[uart1_midi_tx_fifo_head] = data;
		uart1_midi_tx_fifo_head = (uart1_midi_tx_fifo_head + 1) & UART1_MIDI_TX_FIFO_MASK;

		cyg_drv_dsr_unlock();
		return true;
	}
	cyg_drv_dsr_unlock();

	MPTR(UART1_THR) = data & 0xff;
	uart1_tx_activity = 1;

	return true;
}


midi_dev_cb_t uart1_midi_dev_cbs = {
	&uart1MidiDev_init,
	&uartMidiDev_putChar, 
	&uart1_enable, 
	&uart1_disable
};


#endif //_AML
