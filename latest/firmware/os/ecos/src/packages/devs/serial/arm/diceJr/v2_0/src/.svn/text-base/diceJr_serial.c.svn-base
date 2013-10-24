//==========================================================================
//
//      io/serial/arm/diceJr_serial.c
//
//      TC diceJr Serial I/O Interface Module (interrupt driven)
//
//==========================================================================
//==========================================================================
//#####DESCRIPTIONBEGIN####
// 
// Author(s):   Peter De Schrijver (p2@mind.be)
// Contributors:  Peter De Schrijver (p2@mind.be)
// Date:        2002-10-25
// Purpose:     NET+ARM Serial I/O module (interrupt driven version)  
// Description:
// 
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/hal/hal_io.h>             // IO macros
#include <pkgconf/system.h>
#include <pkgconf/io_serial.h>
#include <pkgconf/io.h>
#include <pkgconf/hal_arm_diceJr.h>
#include <cyg/io/io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/io/devtab.h>
#include <cyg/io/serial.h>
#include <cyg/infra/diag.h>

#ifdef CYGPKG_IO_SERIAL_ARM_DICEJR

#include "diceJr_serial.h"

typedef struct diceJr_serial_info {
    CYG_ADDRWORD   base;
    CYG_WORD       int_num;
    cyg_interrupt  serial_interrupt;
    cyg_handle_t   serial_interrupt_handle;
    int            tx_enabled;
} diceJr_serial_info;

static bool diceJr_serial_init(struct cyg_devtab_entry *tab);
static bool diceJr_serial_putc(serial_channel *chan, unsigned char c);
static Cyg_ErrNo diceJr_serial_lookup(struct cyg_devtab_entry **tab,
                                     struct cyg_devtab_entry *sub_tab,
                                     const char *name);
static unsigned char diceJr_serial_getc(serial_channel *chan);
static Cyg_ErrNo diceJr_serial_set_config(serial_channel *chan, cyg_uint32 key,
                                          const void *xbuf, cyg_uint32 *len);
static void diceJr_serial_start_xmit(serial_channel *chan);
static void diceJr_serial_stop_xmit(serial_channel *chan);

static cyg_uint32 diceJr_serial_ISR(cyg_vector_t vector, cyg_addrword_t data);
static void diceJr_serial_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data);

static SERIAL_FUNS(diceJr_serial_funs,
                   diceJr_serial_putc,
                   diceJr_serial_getc,
                   diceJr_serial_set_config,
                   diceJr_serial_start_xmit,
                   diceJr_serial_stop_xmit
		   );

#ifdef CYGPKG_IO_SERIAL_ARM_DICEJR_SERIAL0

static diceJr_serial_info diceJr_serial_info1 = {
    (CYG_ADDRWORD)(UART0BASE),
    CYGNUM_HAL_INTERRUPT_UART0,
 };

#if CYGNUM_IO_SERIAL_ARM_DICEJR_SERIAL0_BUFSIZE > 0
static unsigned char diceJr_serial_out_buf1[CYGNUM_IO_SERIAL_ARM_DICEJR_SERIAL0_BUFSIZE];
static unsigned char diceJr_serial_in_buf1[CYGNUM_IO_SERIAL_ARM_DICEJR_SERIAL0_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(diceJr_serial_channel1,
                                       diceJr_serial_funs,
                                       diceJr_serial_info1,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_DICEJR_SERIAL0_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &diceJr_serial_out_buf1[0], sizeof(diceJr_serial_out_buf1),
                                       &diceJr_serial_in_buf1[0], sizeof(diceJr_serial_in_buf1)
    );    
#else
static SERIAL_CHANNEL(diceJr_serial_channel1,
                      diceJr_serial_funs,
                      diceJr_serial_info1,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_DICEJR_SERIAL0_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(diceJr_serial_io1,
             CYGDAT_IO_SERIAL_ARM_DICEJR_SERIAL0_NAME,
             0,
             &cyg_io_serial_devio,
             diceJr_serial_init,
             diceJr_serial_lookup,
             &diceJr_serial_channel1
    );
#endif
                           

#ifdef CYGPKG_IO_SERIAL_ARM_DICEJR_SERIAL1
static diceJr_serial_info diceJr_serial_info2 = {
    (CYG_ADDRWORD)(UART1BASE),
    CYGNUM_HAL_INTERRUPT_UART1,
};

#if CYGNUM_IO_SERIAL_ARM_DICEJR_SERIAL1_BUFSIZE > 0
static unsigned char diceJr_serial_out_buf2[CYGNUM_IO_SERIAL_ARM_DICEJR_SERIAL1_BUFSIZE];
static unsigned char diceJr_serial_in_buf2[CYGNUM_IO_SERIAL_ARM_DICEJR_SERIAL1_BUFSIZE];

static SERIAL_CHANNEL_USING_INTERRUPTS(diceJr_serial_channel2,
                                       diceJr_serial_funs,
                                       diceJr_serial_info2,
                                       CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_DICEJR_SERIAL1_BAUD),
                                       CYG_SERIAL_STOP_DEFAULT,
                                       CYG_SERIAL_PARITY_DEFAULT,
                                       CYG_SERIAL_WORD_LENGTH_DEFAULT,
                                       CYG_SERIAL_FLAGS_DEFAULT,
                                       &diceJr_serial_out_buf2[0], sizeof(diceJr_serial_out_buf2),
                                       &diceJr_serial_in_buf2[0], sizeof(diceJr_serial_in_buf2)
    );                    
#else
static SERIAL_CHANNEL(diceJr_serial_channel2,
                      diceJr_serial_funs,
                      diceJr_serial_info2,
                      CYG_SERIAL_BAUD_RATE(CYGNUM_IO_SERIAL_ARM_DICEJR_SERIAL1_BAUD),
                      CYG_SERIAL_STOP_DEFAULT,
                      CYG_SERIAL_PARITY_DEFAULT,
                      CYG_SERIAL_WORD_LENGTH_DEFAULT,
                      CYG_SERIAL_FLAGS_DEFAULT
    );
#endif

DEVTAB_ENTRY(diceJr_serial_io2,
             CYGDAT_IO_SERIAL_ARM_DICEJR_SERIAL1_NAME,
             0,
             &cyg_io_serial_devio,
             diceJr_serial_init,
             diceJr_serial_lookup,
             &diceJr_serial_channel2
    );
#endif
                           
static bool diceJr_serial_config_port(serial_channel *chan, cyg_serial_info_t *new_config,bool init) {

    diceJr_serial_info *diceJr=(diceJr_serial_info *)chan->dev_priv;
    unsigned int baud_divisor = 49152000/16/select_baud[new_config->baud];
    cyg_uint8* base = (cyg_uint8*)diceJr->base;

    if (baud_divisor == 0) return false;

    *((volatile int *)(base + UART_LCR)) |= 0x80;  
    //*((volatile int *)(base + UART_DLL)) = 0x35; //divisor = 53, baudrate = 57962
    *((volatile int *)(base + UART_DLL)) = baud_divisor & 0xFF;
    *((volatile int *)(base + UART_DLH)) = (baud_divisor & 0xFF00) >> 8;
    *((volatile int *)(base + UART_LCR)) &= 0x7F;
	
    //*UART0_IER = 0x01;//0x0F, zluo, not sure should I enable it.
  
    //Enable FIFO's and set RCVR Trigger = 1 char (FCR)
    *((volatile int *)(base + UART_FCR)) = 0x01;
	
    //Select 8bits/char, 1stop bit and enable even parity (LCR) 
    *((volatile int *)(base + UART_LCR)) = 0x3;
  
    //Clear "loop back mode" and set RTS and DTR (MCR)
    *((volatile int *)(base + UART_MCR)) = 0x03;    //Get the initialization sequence from netos_serl.c, need to refine.
    {
		int dummy;
		//clear potentiali THR empty interrupt
		dummy = *((volatile int *)(base + UART_IIR));
		//XXX, should all clear potential RxRdy Interruption?
    }
    *((volatile unsigned long*)UART0_IER) = 0x01;//enable Rx interrupt.
	
    if (new_config != &chan->config) {
        chan->config = *new_config;
    }

    return true;

}

static bool diceJr_serial_init(struct cyg_devtab_entry *tab) {
	
    serial_channel *chan = (serial_channel *)tab->priv;
    diceJr_serial_info *diceJr = (diceJr_serial_info *)chan->dev_priv;
#ifdef CYGDBG_IO_INIT
    diag_printf("DICEJR SERIAL init - dev: %x.%d\n",diceJr->base, diceJr->tx_int_num);
#endif    
    (chan->callbacks->serial_init)(chan); 
    if (chan->out_cbuf.len != 0) {
        cyg_drv_interrupt_create(diceJr->int_num,
                                 99,
                                 (cyg_addrword_t)chan,
                                 diceJr_serial_ISR,
                                 diceJr_serial_DSR,
                                 &diceJr->serial_interrupt_handle,
                                 &diceJr->serial_interrupt);
        cyg_drv_interrupt_attach(diceJr->serial_interrupt_handle);
		cyg_drv_interrupt_unmask(diceJr->int_num); //it is unmasked before we have to enable the rcv anyway.
    }
    diceJr_serial_config_port(chan, &chan->config, true);
    return true;

}

static Cyg_ErrNo diceJr_serial_lookup(struct cyg_devtab_entry **tab,
                                    struct cyg_devtab_entry *sub_tab,
                                    const char *name) {

    serial_channel *chan = (serial_channel *)(*tab)->priv;
    (chan->callbacks->serial_init)(chan);

    return ENOERR;

}

static bool diceJr_serial_putc(serial_channel *chan, unsigned char c) {

    diceJr_serial_info *diceJr = (diceJr_serial_info *)chan->dev_priv;
    volatile cyg_uint8 *base = (cyg_uint8 *)diceJr->base;    
        
    if(*((volatile int *)(base+UART_LSR)) & BIT5) {
        HAL_WRITE_UINT8(base+UART_THR, c);
        return true;
    }
    else
        return false;
}

static unsigned char diceJr_serial_getc(serial_channel *chan) {

    //unsigned char c;
    cyg_uint32 c;
    diceJr_serial_info *diceJr = (diceJr_serial_info *)chan->dev_priv;
    CYG_ADDRWORD base = diceJr->base;

    //The read will also clear timer close interrupt.
    HAL_READ_UINT32(base+UART_RBR, c);
    
    return c;
}    

static Cyg_ErrNo diceJr_serial_set_config(serial_channel *chan, cyg_uint32 key,
                                          const void *xbuf, cyg_uint32 *len) {
    switch (key) {
        case CYG_IO_SET_CONFIG_SERIAL_INFO:
        {
            cyg_serial_info_t *config = (cyg_serial_info_t *)xbuf;
            if ( *len < sizeof(cyg_serial_info_t) ) {
                return -EINVAL;
            }
            *len = sizeof(cyg_serial_info_t);
            if ( true != diceJr_serial_config_port(chan, config, false))
                return -EINVAL;
        }
        break;

        default:
            return -EINVAL;
    }
    return ENOERR;
}

static void diceJr_serial_start_xmit(serial_channel *chan) {

    diceJr_serial_info *diceJr = (diceJr_serial_info *)chan->dev_priv;
    cyg_uint8* base = (cyg_uint8*)diceJr->base;

    (chan->callbacks->xmt_char)(chan);  // Kick transmitter (if necessary), zluo.

    *((volatile int *)(base+UART_IER)) |= 0x2;
    diceJr->tx_enabled = true;
}

static void diceJr_serial_stop_xmit(serial_channel *chan) {

    diceJr_serial_info *diceJr = (diceJr_serial_info *)chan->dev_priv;
    cyg_uint8* base = (cyg_uint8*)diceJr->base;

    *((volatile int *)(base+UART_IER)) &= ~0x2;
    diceJr->tx_enabled=false;
}

static cyg_uint32 diceJr_serial_ISR(cyg_vector_t vector, cyg_addrword_t data) {

    serial_channel *chan = (serial_channel *)data;
    diceJr_serial_info *diceJr = (diceJr_serial_info *)chan->dev_priv;

    cyg_drv_interrupt_mask(diceJr->int_num);
    cyg_drv_interrupt_acknowledge(diceJr->int_num);

    return CYG_ISR_CALL_DSR;
}

static void diceJr_serial_DSR(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data) {

    serial_channel *chan = (serial_channel *)data;
    diceJr_serial_info *diceJr = (diceJr_serial_info *)chan->dev_priv;
    cyg_uint8* base = (cyg_uint8*)diceJr->base;
    cyg_uint32 iir_status = *((volatile int *)(base+UART_IIR)) & 0xF;
    cyg_uint32 dummy;
    char nxt_char;

    if (iir_status == 0x2) {//THR empty
		//read the status ready
		(chan->callbacks->xmt_char)(chan);
		if(diceJr->tx_enabled) {
			*((volatile int *)(base+UART_IER)) |= 0x2;// for tx to generate interrupt
		} else {
			*((volatile int *)(base+UART_IER)) &= ~0x2;// for tx to generate interrupt
		}
    }
 
    if ((iir_status == 0x4) ||(iir_status == 0xC)) { //char in RBR
		while (*((volatile int *)(base+UART_LSR)) & BIT0) {//While characters in RX buffer
			nxt_char =  *((volatile int *)(base+UART_RBR));//Fetch character
			(chan->callbacks->rcv_char)(chan, nxt_char);
		}
    }
  
    if (iir_status == 0x6) { //Receiver Status 
		dummy = *((volatile int *)(base+UART_LSR));
    }
    
    if (iir_status == 0x1) {// No interrupt pending
    }
	
    cyg_drv_interrupt_unmask(diceJr->int_num);
}

#endif
