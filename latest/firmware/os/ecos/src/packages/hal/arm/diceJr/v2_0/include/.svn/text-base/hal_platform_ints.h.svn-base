#ifndef CYGONCE_HAL_PLATFORM_INTS_H
#define CYGONCE_HAL_PLATFORM_INTS_H
//==========================================================================
//
//      hal_platform_ints.h
//
//      HAL Interrupt and clock assignments for AT91/EB40
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
// Contributors: gthomas
// Date:         2001-07-12
// Purpose:      Define Interrupt support
// Description:  The interrupt specifics for the AT91/EB40 board/platform are
//               defined here.
//              
// Usage:        #include <cyg/hal/hal_platform_ints.h>
//               ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================


#define  CYGNUM_HAL_INTERRUPT_TIMER2              0x13
#define  CYGNUM_HAL_INTERRUPT_MIDI                0x12
#define  CYGNUM_HAL_INTERRUPT_MIXER_OVERFLOW      0x11
#define  CYGNUM_HAL_INTERRUPT_JETPLL0             0x10
#define  CYGNUM_HAL_INTERRUPT_ARM_AUDIO_OVERFLOW  0xF
#define  CYGNUM_HAL_INTERRUPT_ARM_AUDIO_IRQ       0xE
#define  CYGNUM_HAL_INTERRUPT_AVS1   0xD
#define  CYGNUM_HAL_INTERRUPT_AVS0   0xC

#define  CYGNUM_HAL_INTERRUPT_RESV   0xB
#define  CYGNUM_HAL_INTERRUPT_GRAY   0xA
#define  CYGNUM_HAL_INTERRUPT_PRMGR  0x9
#define  CYGNUM_HAL_INTERRUPT_SPI    0x8
#define  CYGNUM_HAL_INTERRUPT_WATCHDOG  0x7

#define  CYGNUM_HAL_INTERRUPT_GPIO   0x6
#define  CYGNUM_HAL_INTERRUPT_TIMER  0x5
#define  CYGNUM_HAL_INTERRUPT_UART1  0x4
#define  CYGNUM_HAL_INTERRUPT_UART0  0x3
#define  CYGNUM_HAL_INTERRUPT_I2C    0x2
#define  CYGNUM_HAL_INTERRUPT_LINKON 0x1
#define  CYGNUM_HAL_INTERRUPT_SLINK  0x0

#define CYGNUM_HAL_ISR_MIN                     0
#define CYGNUM_HAL_ISR_MAX                     0x13

#define CYGNUM_HAL_ISR_COUNT                   (CYGNUM_HAL_ISR_MAX + 1)
// The vector used by the Real time clock
#define CYGNUM_HAL_INTERRUPT_RTC               CYGNUM_HAL_INTERRUPT_TIMER

//----------------------------------------------------------------------------
// Reset.
#define HAL_PLATFORM_RESET()

//we should change it here with 0x0?
#define HAL_PLATFORM_RESET_ENTRY 0x04000040

#endif // CYGONCE_HAL_PLATFORM_INTS_H
