//==========================================================================
//
//      diceJr_misc.c
//
//      HAL misc board support code for TCAT TC22xx
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas, jskov
// Date:         2001-07-12
// Purpose:      HAL board support
// Description:  Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_intr.h>           // necessary?
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_if.h>             // calling interface
#include <cyg/hal/hal_misc.h>           // helper functions
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
#include <cyg/hal/drv_api.h>            // HAL ISR support
#endif
#include <cyg/hal/plf_io.h>             // platform registers

static cyg_uint32 _period;

void hal_clock_initialize(cyg_uint32 period)
{
     /* disable timer1 */
    HAL_WRITE_UINT32(TIMER1_CTRL_REG, 0x0);
    /* set the load count */
    HAL_WRITE_UINT32(TIMER1_LOAD_CNT, period);
    hal_interrupt_unmask(CYGNUM_HAL_INTERRUPT_TIMER);
    /* enable timer interrupt, XXX*/
    HAL_WRITE_UINT32(TIMER1_CTRL_REG, 0x3); //enabled, userdefined, unmasked
}

void hal_clock_reset(cyg_uint32 vector, cyg_uint32 period)
{
    cyg_uint32 val;

    *((unsigned long*)0xbf000004) = 0x5678; //hack, kick the wd
    //clear interrupt if there is any.
    HAL_READ_UINT32(TIMER1_INT_STATUS, val);
    if(val & 0x1) //if there is an interrupt, clear it
      HAL_READ_UINT32(TIMER1_EOI, val);

    // HAL_INTERRUPT_ACKNOWLEDGE doesn't do anything, we keep it here 
    // for code integraty for future porting.
    HAL_INTERRUPT_ACKNOWLEDGE(CYGNUM_HAL_INTERRUPT_RTC);
    if (period != _period) {
        hal_clock_initialize(period);
    }
    _period = period;
}

void hal_clock_read(cyg_uint32 *pvalue)
{
    cyg_uint32 val;
    HAL_READ_UINT32(TIMER1_CUR_VAL, val);
	//The counter is counting down.
    *pvalue = _period - val;
}

// -------------------------------------------------------------------------
//
// Delay for some number of micro-seconds
//   Use timer #2
// NOTE: This would actually conflict with using TIMER2 as an interrupt source.
//			 This function is only used by RedBoot at this point so it should not
//       be a problem.
void hal_delay_us(cyg_int32 usecs)
{
    cyg_uint32 stat;
    cyg_uint32 ticks;

    *((unsigned long*)0xbf000004) = 0x5678; //hack, kick the wd
    
    ticks = usecs * 50; 

    //disabled, user defined, int masked, 
    HAL_WRITE_UINT32(TIMER2_CTRL_REG, 0x6);
    /* just for safety, disable timer2 interrupt */
    HAL_WRITE_UINT32(TIMER2_LOAD_CNT, ticks);
    HAL_WRITE_UINT32(TIMER2_CTRL_REG, 0x7); //enable timer
    do {
        HAL_READ_UINT32(TIMER2_EOI, stat);
	*((unsigned long*)0xbf000004) = 0x5678; //hack, kick the wd
    } while ( (stat&0x1) != 0);

}

// -------------------------------------------------------------------------
// Interrupts on DICE
// All interrupts are connected through the Interrupt controller.
// There are 19 interrupts connected (Bit 0 to Bit 18)
// One of those interrupts CYGNUM_HAL_INTERRUPT_TIMER is shared by both timers.
// This module will demux those int two vectors:
//	CYGNUM_HAL_INTERRUPT_TIMER and CYGNUM_HAL_INTERRUPT_TIMER2
// The individual TIMER mask bits will be used instead of the global interrupt
// mask for those interrupts.

// -------------------------------------------------------------------------
// Hardware init
void hal_hardware_init(void)
{
    // Set up eCos/ROM interfaces
    hal_if_init();

    // Reset all interrupts
    // All interrupts are enabled and masked
		HAL_WRITE_UINT32(TIMER1_CTRL_REG, 0x4);
		HAL_WRITE_UINT32(TIMER2_CTRL_REG, 0x4);
    HAL_WRITE_UINT32(ICTL_MASK, ~(1<<CYGNUM_HAL_INTERRUPT_TIMER));  
    HAL_WRITE_UINT32(ICTL_ENABLE, 0xFFFFFFFF);  
}

//
// This routine is called to respond to a hardware interrupt (IRQ).  It
// should interrogate the hardware and return the IRQ vector number.

int hal_IRQ_handler(void)
{
    cyg_uint32 irq_num;

    cyg_uint32 isre;

		//Make it effective by checking groups of 8
    HAL_READ_UINT32(ICTL_FINALSTAT, isre);
    if ( isre & 0xff)
        irq_num = 0;
    else if ( isre & 0xff00 )
        irq_num = 8;
    else 
        irq_num = 16;
	//check that group of 8
	do {
		if (isre & (1 << irq_num)) 
		{
			if (irq_num == CYGNUM_HAL_INTERRUPT_TIMER)
			{
				//check which of the timers, demux
				HAL_READ_UINT32(TIMER2_INT_STATUS, isre);
				if(isre & 0x1) return CYGNUM_HAL_INTERRUPT_TIMER2;
			}
			return irq_num;
		}
		irq_num++;
	} while (irq_num & 7);

    return CYGNUM_HAL_INTERRUPT_NONE;
}

//
// Interrupt control
//

void hal_interrupt_mask(int vector)
{
    cyg_uint32 val;

		CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");
		if (vector == CYGNUM_HAL_INTERRUPT_TIMER)
		{
			//mask actual timer
	    HAL_READ_UINT32(TIMER1_CTRL_REG, val);
			val |= 0x4; //mask timer
			HAL_WRITE_UINT32(TIMER1_CTRL_REG, val);
			return;
		} else if (vector == CYGNUM_HAL_INTERRUPT_TIMER2)
		{
			//mask actual timer
	    HAL_READ_UINT32(TIMER2_CTRL_REG, val);
			val |= 0x4; //mask timer
			HAL_WRITE_UINT32(TIMER2_CTRL_REG, val);
			return;
		}
    *((volatile unsigned long*)ICTL_MASK) |= (1<<vector);
}

void hal_interrupt_unmask(int vector)
{
	  cyg_uint32 val;
  
		CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");
		if (vector == CYGNUM_HAL_INTERRUPT_TIMER)
		{
			//unmask actual timer
	    HAL_READ_UINT32(TIMER1_CTRL_REG, val);
			val &= ~0x4; //unmask mask timer
			HAL_WRITE_UINT32(TIMER1_CTRL_REG, val);
			//continue to unmask the int ctrl vector as well
		} else if (vector == CYGNUM_HAL_INTERRUPT_TIMER2)
		{
			//mask actual timer
	    HAL_READ_UINT32(TIMER2_CTRL_REG, val);
			val &= ~0x4; //unmask timer
			HAL_WRITE_UINT32(TIMER2_CTRL_REG, val);
			vector = CYGNUM_HAL_INTERRUPT_TIMER;
			//continue to unmask the int ctrl vector as well
		}
    *((volatile unsigned long*)ICTL_MASK) &= ~(1<<vector);
}

void hal_interrupt_acknowledge(int vector)
{
    CYG_ASSERT(vector <= CYGNUM_HAL_ISR_MAX &&
               vector >= CYGNUM_HAL_ISR_MIN , "Invalid vector");
    //diceJr only has ier which doesn't ack int. it is has to be done in the handler.
}

void hal_interrupt_configure(int vector, int level, int up)
{
    //diceJr doesn't support level interrupt.
}

void hal_interrupt_set_level(int vector, int level)
{
    //diceJr doesn't support level.zluo.
}

void hal_show_IRQ(int vector, int data, int handler)
{
//    UNDEFINED(__FUNCTION__);  // FIXME
}

//--------------------------------------------------------------------------
// EOF hal_misc.c
