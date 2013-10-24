#ifndef CYGONCE_HAL_PLATFORM_SETUP_H
#define CYGONCE_HAL_PLATFORM_SETUP_H

/*=============================================================================
//
//      hal_platform_setup.h
//
//      Platform specific support for HAL (assembly code)
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   gthomas
// Contributors:gthomas
// Date:        2001-07-12
// Purpose:     AT91/EB40 platform specific support routines
// Description: 
// Usage:       #include <cyg/hal/hal_platform_setup.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <cyg/hal/plf_io.h>
#include CYGHWR_MEMORY_LAYOUT_H

//#define REDBOOT_IN_EXT_SRAM
#define REDBOOT_IN_EXT_SDRAM

#ifdef REDBOOT_IN_EXT_SRAM
#define BASE_SDRAM_REG			0x01000000 //cs1, sdram
#define BASE_SRAM_8_REG			0x00080000 //cs2, sram8
#define BASE_SRAM_16_REG		0x00000000 //cs3, sram16
#define BASE_FLASH_REG			0x04000000
#define HAL_FLASH_PHYS_ADDR		 sram16
#define BASE_FLASH_REG			0x04000000
#define HAL_FLASH_PHYS_ADDR		0x04000000
#define REMAP_ALIAS_0_REG		0x04000000
#define REMAP_ALIAS_1_REG		0x01000000
#else //REDBOOT_IN_EXT_SDRAM
#define BASE_SDRAM_REG			0x00000000 //cs1, sdram
#define BASE_SRAM_8_REG			0x02080000 //cs2, sram8
#define BASE_SRAM_16_REG		0x02000000 //cs3, sram16
#define BASE_FLASH_REG			0x04000000
#define HAL_FLASH_PHYS_ADDR		0x04000000
#define REMAP_ALIAS_0_REG		0x04000000
#define REMAP_ALIAS_1_REG		0x01000000
#endif

//#define UNMAPPED(x) 			((x)-0x02000000)
#define UNMAPPED(x) 			(x)

.macro PLATFORM_SETUP1

#if defined(CYG_HAL_STARTUP_ROMRAM) || defined(CYG_HAL_STARTUP_ROM)

  LDR	R0,	=WATCHDOG_INT_CLR		// kick watchdog
  LDR	R1,	=0x00005678
  STR	R1,	[R0]
  LDR	R0,	=WATCHDOG_RST_EN		// disable watchdog
  LDR	R1,	=0x00001234
  STR	R1,	[R0]

  // We will program the memory controller for CS0 and CS1.
  ldr r2, =UNMAPPED(10f)
  add r2, r2, #HAL_FLASH_PHYS_ADDR

  /* mem_ctrl base */
  ldr r1,=MEMCTLBASE
  //configure flash
  ldr r0, =0x246
  str r0, [r1, #MEM_SMSKR0_OFF]
  ldr r0, =HAL_FLASH_PHYS_ADDR
  str r0, [r1, #MEM_SCSLR0_OFF]

  mov pc, r2

10:

  LDR   R0, =0x10000141
  STR   R0, [R1, #MEM_SMTMGR_SET0_OFF]  //SET 0 REGISTER
  LDR   R0, =0x10000041
  STR   R0, [R1, #MEM_SMTMGR_SET1_OFF]  //SET 1 REGISTER
//LDR   R0, =0x10744FFE	 // default 
  LDR   R0, =0x10200544
  STR   R0, [R1, #MEM_SMTMGR_SET2_OFF]  //SET 2 REGISTER

  //SM_CRTL REGISTER
  LDR   R0, =0x20F
  STR   R0, [R1, #MEM_SMCTLR_OFF]

  //CS1-3
  ldr r0, =0x08 //cs1, sdram, 16bit, 8MB
  str r0, [r1, #MEM_SMSKR1_OFF]
  ldr r0, =0x24 //cs2, sram, 8bit, 512k, SET1 
  str r0, [r1, #MEM_SMSKR2_OFF]
  ldr r0, =0x124 //cs3, sram, 16bit, 512k, SET0
  str r0, [r1, #MEM_SMSKR3_OFF]

  ldr r0, =0x20
  //diable CS4-7
  str r0, [r1, #MEM_SMSKR4_OFF]
  str r0, [r1, #MEM_SMSKR5_OFF]
  str r0, [r1, #MEM_SMSKR6_OFF]
  str r0, [r1, #MEM_SMSKR7_OFF]

  //setup base address
  ldr r0, =BASE_SDRAM_REG //cs1, sdram
  str r0, [r1, #MEM_SCSLR1_OFF]
  ldr r0, =BASE_SRAM_8_REG //cs2, sram8
  str r0, [r1, #MEM_SCSLR2_OFF]
  ldr r0, =BASE_SRAM_16_REG //cs3, sram16
  str r0, [r1, #MEM_SCSLR3_OFF]
  ldr r0, =0x05000000 
  str r0, [r1, #MEM_SCSLR4_OFF]
  ldr r0, =0x06000000 
  str r0, [r1, #MEM_SCSLR5_OFF]
  ldr r0, =0x07000000 
  str r0, [r1, #MEM_SCSLR6_OFF]
  ldr r0, =0x08000000 
  str r0, [r1, #MEM_SCSLR7_OFF]

  //SDRAM Configuration & Initialization
  ldr r0, =0x00140f68
  str r0, [r1, #MEM_SCONR_OFF]
  ldr r0, =0x00dcd449
  str r0, [r1, #MEM_STMG0R_OFF]
  ldr r0, =0x00012000
  str r0, [r1, #MEM_STMG1R_OFF]
  ldr r0, =0x00002071
  str r0, [r1, #MEM_SCTLR_OFF]
  ldr r0, =0x0000030c	//50MHz clock
//ldr r0, =0x0000001F	// 2MHz clock
  str r0, [r1, #MEM_SREFR_OFF]

  //alias and remap registers, same as base.
  ldr r0, =REMAP_ALIAS_0_REG
  str r0, [r1, #MEM_CSALIAS0_OFF]
  str r0, [r1, #MEM_CSREMAP0_OFF]
  ldr r0, =REMAP_ALIAS_1_REG
  str r0, [r1, #MEM_CSALIAS1_OFF]
  str r0, [r1, #MEM_CSREMAP1_OFF]

  //0]

  //alias and remap registers, same as base.
  ldr r0, =REMAP_ALIAS_0_REG
  str r0, [r1, #MEM_CSALIAS0_OFF]
  str r0, [r1, #MEM_CSREMAP0_OFF]
  ldr r0, =REMAP_ALIAS_1_REG
  str r0, [r1, #MEM_CSALIAS1_OFF]
  str r0, [r1, #MEM_CSREMAP1_OFF]

#if defined(CYG_HAL_STARTUP_ROMRAM)
		mov		r1,#0
		mov		r2,#HAL_FLASH_PHYS_ADDR
		ldr		r3,=(__heap1 + HAL_FLASH_PHYS_ADDR)

20:
		ldr		r0,[r2],#4
		str		r0,[r1]
		ldr		r5,[r1],#4
		cmp		r2,r3
		bne		20b

		ldr		r0,=30f				/* And we go back to RAM */

		mov		pc,r0
		nop
		nop
		nop
30:	
  //setup int mux
  ldr r1, =GPCSRBASE
  ldr r0, =0x0c83a443
  str r0, [r1, #GPCSR_IRQ_MUX_5_TO_0_OFF]
  ldr r0, =0x14488025
  str r0, [r1, #GPCSR_IRQ_MUX_11_TO_6_OFF]
  ldr r0, =0x20f6b98b
  str r0, [r1, #GPCSR_IRQ_MUX_17_TO_12_OFF]
  ldr r0, =0x12
  str r0, [r1, #GPCSR_IRQ_MUX_18_OFF]
  ldr r0, =0x0
  str r0, [r1, #GPCSR_FIQ_MUX_5_TO_0_OFF]
  ldr r0, =0x0
  str r0, [r1, #GPCSR_FIQ_MUX_7_TO_6_OFF]


#endif

#endif
		.endm


	// If there is any section copy, it should be done here.
	.macro PLATFORM_REGION_INIT

	ldr		r1, =CYGMEM_REGION_sram
	ldr		r2, =(__bss_start )
	ldr		r3, =(_end)

40:
	cmp		r1,r3
	beq		50f
	ldr		r0,[r2],#4
	str		r0,[r1],#4
	b		40b
50:

	.endm  // PLATFORM_REGION_INIT
			
	.macro PLATFORM_FIQ_HANDLER
	ldr     sp,.__fiq_stack

	mrs     r8,spsr                 // CPSR at time of interrupt
	and     r9,r8,#CPSR_MODE_BITS   // isolate pre-interrupt mode
	tst     r9,#CPSR_THUMB_ENABLE
	subeq   r9,lr,#4                // PC at time of interrupt (ARM)
	subne   r9,lr,#2                // PC at time of interrupt (thumb)

	stmfd   sp!, {r0-r7, r9}

#if 0
	ldr     r9, =0xc1000000
	mov     r8, #0
	str     r8, [r9, #0xc0]
#endif
	
	ldr     r8,.__fiq_handler
	ldr     r9, [r8]
	mov     lr, pc
	mov     pc, r9
	//subs    pc,lr,#4
	ldmfd   sp!, {r0-r7, pc}^

	.endm //PLATFORM_FIQ_HANDLER


//-----------------------------------------------------------------------------
// end of hal_platform_setup.h
#endif // CYGONCE_HAL_PLATFORM_SETUP_H
