#ifndef CYGONCE_HAL_PLF_IO_H
#define CYGONCE_HAL_PLF_IO_H
//=============================================================================
//
//      plf_io.h
//
//      Platform specific registers
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
// Author(s):   jskov
// Contributors:jskov, gthomas
// Date:        2001-07-12
// Purpose:     AT91/EB40 platform specific registers
// Description: 
// Usage:       #include <cyg/hal/plf_io.h>
//
//####DESCRIPTIONEND####
//
//g/hal/plf_io.h>
//
//####DESCRIPTIONEND####
//
// Memory map is 1-1, zluo: add for redboot

#define CYGARC_PHYSICAL_ADDRESS(_x_) (_x_)

// zluo, definations
// Miscellaneous: quadlet definition
#define QUADLET unsigned long


// Miscellaneous: bit positions
#define BIT0                    0x00000001
#define BIT1                    0x00000002
#define BIT2                    0x00000004
#define BIT3                    0x00000008
#define BIT4                    0x00000010
#define BIT5                    0x00000020
#define BIT6                    0x00000040
#define BIT7                    0x00000080
#define BIT8                    0x00000100
#define BIT9                    0x00000200
#define BIT10                   0x00000400
#define BIT11                   0x00000800
#define BIT12                   0x00001000
#define BIT13                   0x00002000
#define BIT14                   0x00004000
#define BIT15                   0x00008000
#define BIT16                   0x00010000
#define BIT17                   0x00020000
#define BIT18                   0x00040000
#define BIT19                   0x00080000
#define BIT20                   0x00100000
#define BIT21                   0x00200000
#define BIT22                   0x00400000
#define BIT23                   0x00800000
#define BIT24                   0x01000000
#define BIT25                   0x02000000
#define BIT26                   0x04000000
#define BIT27                   0x08000000
#define BIT28                   0x10000000
#define BIT29                   0x20000000
#define BIT30                   0x40000000
#define BIT31                   0x80000000

//DICE definations
#define MEMCTLBASE              0x81000000 // Memory controller
#define LLCBASE                 0x82000000

// APB Base addresses
#define ICTLBASE                0xC1000000 // Interrupt controller
#define WATCHDOGBASE            0xBF000000 // Watchdog
#define UART0BASE               0xBE000000 // UART #0
#define UART1BASE               0xBD000000 // UART #1
#define TIMERSBASE              0xC2000000 // Timers
#define GPIOBASE                0xC3000000 // General Purpose I/O
#define ICBASE                  0xC4000000 // I2C Master/Slave controller
#define ECPMBASE                0xC5000000 // Elliptic curve point math
#define GRAYBASE                0xC6000000 // Gray code decoders
#define GPCSRBASE               0xC7000000 // General Purpose Control & Status registers
#define PDBINTBASE              0xC8000000 // Isoc/PDB interface
#define PWRMGRBASE              0xD0000000 // Power Manager
#define DICEBASE				0xCE000000
#define AVSBASE                 0xCF000000
#define DICE_HYBRID_PLL1_BASE	0xCC000000
#define DICE_HYBRID_PLL2_BASE	0xCD000000


//Memory controller address
#define MEM_SCONR_OFF           0x00
#define MEM_STMG0R_OFF          0x04
#define MEM_STMG1R_OFF          0x08
#define MEM_SCTLR_OFF           0x0C
#define MEM_SREFR_OFF           0x10
#define MEM_SCSLR0_OFF          0x14
#define MEM_SCSLR1_OFF          0x18
#define MEM_SCSLR2_OFF          0x1C
#define MEM_SCSLR3_OFF          0x20
#define MEM_SCSLR4_OFF          0x24
#define MEM_SCSLR5_OFF          0x28
#define MEM_SCSLR6_OFF          0x2C
#define MEM_SCSLR7_OFF          0x30
#define MEM_SMSKR0_OFF          0x54
#define MEM_SMSKR1_OFF          0x58
#define MEM_SMSKR2_OFF          0x5C
#define MEM_SMSKR3_OFF          0x60
#define MEM_SMSKR4_OFF          0x64
#define MEM_SMSKR5_OFF          0x68
#define MEM_SMSKR6_OFF          0x6C
#define MEM_SMSKR7_OFF          0x70
#define MEM_CSALIAS0_OFF        0x74
#define MEM_CSALIAS1_OFF        0x78
#define MEM_CSREMAP0_OFF        0x84
#define MEM_CSREMAP1_OFF        0x88
#define MEM_SMTMGR_SET0_OFF     0x94
#define MEM_SMTMGR_SET1_OFF     0x98
#define MEM_SMTMGR_SET2_OFF     0x9C
#define MEM_FLASH_TRPDR_OFF     0xA0
#define MEM_SMCTLR_OFF          0xA4

#define MEM_SCONR               (MEMCTLBASE + MEM_SCONR_OFF)
#define MEM_STMG0R              (MEMCTLBASE + MEM_STMG0R_OFF)
#define MEM_STMG1R              (MEMCTLBASE + MEM_STMG1R_OFF)
#define MEM_SCTLR               (MEMCTLBASE + MEM_SCTLR_OFF)
#define MEM_SREFR               (MEMCTLBASE + MEM_SREFR_OFF)
#define MEM_SCSLR0              (MEMCTLBASE + MEM_SCSLR0_OFF)
#define MEM_SCSLR1              (MEMCTLBASE + MEM_SCSLR1_OFF)
#define MEM_SCSLR2              (MEMCTLBASE + MEM_SCSLR2_OFF)
#define MEM_SCSLR3              (MEMCTLBASE + MEM_SCSLR3_OFF) 
#define MEM_SCSLR4              (MEMCTLBASE + MEM_SCSLR4_OFF) 
#define MEM_SCSLR5              (MEMCTLBASE + MEM_SCSLR5_OFF)
#define MEM_SCSLR6              (MEMCTLBASE + MEM_SCSLR6_OFF)
#define MEM_SCSLR7              (MEMCTLBASE + MEM_SCSLR7_OFF)
#define MEM_SMSKR0              (MEMCTLBASE + MEM_SMSKR0_OFF)
#define MEM_SMSKR1              (MEMCTLBASE + MEM_SMSKR1_OFF)
#define MEM_SMSKR2              (MEMCTLBASE + MEM_SMSKR2_OFF)
#define MEM_SMSKR3              (MEMCTLBASE + MEM_SMSKR3_OFF)
#define MEM_SMSKR4              (MEMCTLBASE + MEM_SMSKR4_OFF)
#define MEM_SMSKR5              (MEMCTLBASE + MEM_SMSKR5_OFF)
#define MEM_SMSKR6              (MEMCTLBASE + MEM_SMSKR6_OFF)
#define MEM_SMSKR7              (MEMCTLBASE + MEM_SMSKR7_OFF)
#define MEM_CSALIAS0            (MEMCTLBASE + MEM_CSALIAS0_OFF)
#define MEM_CSALIAS1            (MEMCTLBASE + MEM_CSALIAS1_OFF)
#define MEM_CSREMAP0            (MEMCTLBASE + MEM_CSREMAP0_OFF)
#define MEM_CSREMAP1            (MEMCTLBASE + MEM_CSREMAP1_OFF)
#define MEM_SMTMGR_SET0         (MEMCTLBASE + MEM_SMTMGR_SET0_OFF)
#define MEM_SMTMGR_SET1         (MEMCTLBASE + MEM_SMTMGR_SET1_OFF)
#define MEM_SMTMGR_SET2         (MEMCTLBASE + MEM_SMTMGR_SET2_OFF)
#define MEM_FLASH_TRPDR         (MEMCTLBASE + MEM_FLASH_TRPDR_OFF)
#define MEM_SMCTLR              (MEMCTLBASE + MEM_SMCTLR_OFF)

//HPLL_1 adresses
#define HPLL_1_GPO_GRANT        (HPLL_1 + (4 * 0x36))
#define HPLL_1_X1_GPO           (HPLL_1 + (4 * 0x37))
#define HPLL_1_X2_GPO           (HPLL_1 + (4 * 0x38))
#define HPLL_1_X3_GPO           (HPLL_1 + (4 * 0x39))
#define HPLL_1_GPI              (HPLL_1 + (4 * 0xF6))
#define HPLL_1_CONFIG_AC        (HPLL_1 + (4 * 0xF8))
#define HPLL_1_CF_CAF_SELECT    (HPLL_1 + (4 * 0x1))

//HPLL_2 adresses
#define HPLL_2_CONFIG_AC        (HPLL_2 + (4 * 0xF8))

// Interrupt controller addresses
#define ICTL_ENABLE             (ICTLBASE+0x00)
#define ICTL_MASK               (ICTLBASE+0x08)
#define ICTL_RAWSTAT            (ICTLBASE+0x18)
#define ICTL_STAT               (ICTLBASE+0x20)
#define ICTL_FINALSTAT          (ICTLBASE+0x30)

#define ICTL_FIQ_ENABLE         (ICTLBASE+0xc0)
#define ICTL_FIQ_MASK           (ICTLBASE+0xc4)
#define ICTL_FIQ_RAW            (ICTLBASE+0xcc)
#define ICTL_FIQ_FINALSTAT      (ICTLBASE+0xd4)

// Watchdog addresses
#define WATCHDOG_RST_EN         (WATCHDOGBASE)
#define WATCHDOG_INT_CLR        (WATCHDOGBASE + 0x04)
#define WATCHDOG_PREL_VAL       (WATCHDOGBASE + 0x08)
#define WATCHDOG_PREC_VAL       (WATCHDOGBASE + 0x0C)
#define WATCHDOG_PRE12B_VAL     (WATCHDOGBASE + 0x08)


// UART  offsets
#define UART_RBR                0x00
#define UART_THR                0x00
#define UART_DLL                0x00
#define UART_IER                0x04
#define UART_DLH                0x04
#define UART_IIR                0x08
#define UART_FCR                0x08
#define UART_LCR                0x0C
#define UART_MCR                0x10
#define UART_LSR                0x14
#define UART_MSR                0x18
#define UART_SCR                0x1C

// UART #0 addresses
#define UART0_RBR               (UART0BASE + 0x00)
#define UART0_THR               (UART0BASE + 0x00)
#define UART0_DLL               (UART0BASE + 0x00)
#define UART0_IER               (UART0BASE + 0x04)
#define UART0_DLH               (UART0BASE + 0x04)
#define UART0_IIR               (UART0BASE + 0x08)
#define UART0_FCR               (UART0BASE + 0x08)
#define UART0_LCR               (UART0BASE + 0x0C)
#define UART0_MCR               (UART0BASE + 0x10)
#define UART0_LSR               (UART0BASE + 0x14)
#define UART0_MSR               (UART0BASE + 0x18)
#define UART0_SCR               (UART0BASE + 0x1C)

// UART #1 addresses
#define UART1_RBR               (UART1BASE)
#define UART1_THR               (UART1BASE)
#define UART1_DLL               (UART1BASE)
#define UART1_IER               (UART1BASE + 0x04)
#define UART1_DLH               (UART1BASE + 0x04)
#define UART1_IIR               (UART1BASE + 0x08)
#define UART1_FCR               (UART1BASE + 0x08)
#define UART1_LCR               (UART1BASE + 0x0C)
#define UART1_MCR               (UART1BASE + 0x10)
#define UART1_LSR               (UART1BASE + 0x14)
#define UART1_MSR               (UART1BASE + 0x18)
#define UART1_SCR               (UART1BASE + 0x1C)

// Timers register layout
#define TIMER1BASE              (TIMERSBASE + 0x00)
#define TIMER2BASE              (TIMERSBASE + 0x14)

#define TIMER1_LOAD_CNT         (TIMER1BASE + 0x00)
#define TIMER1_CUR_VAL          (TIMER1BASE + 0x04)
#define TIMER1_CTRL_REG         (TIMER1BASE + 0x08)
#define TIMER1_EOI              (TIMER1BASE + 0x0C)
#define TIMER1_INT_STATUS       (TIMER1BASE + 0x10)

#define TIMER2_LOAD_CNT         (TIMER2BASE + 0x00)
#define TIMER2_CUR_VAL          (TIMER2BASE + 0x04)
#define TIMER2_CTRL_REG         (TIMER2BASE + 0x08)
#define TIMER2_EOI              (TIMER2BASE + 0x0C)
#define TIMER2_INT_STATUS       (TIMER2BASE + 0x10)

#define TIMERS_INT_STATUS       (TIMERSBASE + 0xA0)
#define TIMERS_EOI              (TIMERSBASE + 0xA4)
#define TIMERS_RAW_STATUS       (TIMERSBASE + 0xA8)
#define TIMERS_COMP_VER         (TIMERSBASE + 0xAC)

// General Purpose register layout
#define GPIO_A_DR               (GPIOBASE + 0x000)
#define GPIO_A_DDR              (GPIOBASE + 0x004)
#define GPIO_A_CTL              (GPIOBASE + 0x008)
#define GPIO_INT_EN             (GPIOBASE + 0x030)
#define GPIO_INT_MASK           (GPIOBASE + 0x034)
#define GPIO_INT_LEVEL          (GPIOBASE + 0x038)
#define GPIO_INT_POL            (GPIOBASE + 0x03c)
#define GPIO_INT_STATUS         (GPIOBASE + 0x040)
#define GPIO_INT_RAW            (GPIOBASE + 0x044)
#define GPIO_DEBOUNCE           (GPIOBASE + 0x048)
#define GPIO_A_EOI              (GPIOBASE + 0x04c)
#define GPIO_EXT_A              (GPIOBASE + 0x050)
#define GPIO_LS_SYNC            (GPIOBASE + 0x060)

// I2C Master/Slave controller addresses
#define IC_CON                  (ICBASE + 0x000)
#define IC_TAR                  (ICBASE + 0x004)
#define IC_SAR                  (ICBASE + 0x008)
#define IC_HS_MAR               (ICBASE + 0x00C)
#define IC_DATA_CMD             (ICBASE + 0x010)
#define IC_SS_HCNT              (ICBASE + 0x014)
#define IC_SS_LCNT              (ICBASE + 0x018)
#define IC_FS_HCNT              (ICBASE + 0x01c)
#define IC_FS_LCNT              (ICBASE + 0x020)
#define IC_HS_HCNT              (ICBASE + 0x024)
#define IC_HS_LCNT              (ICBASE + 0x028)
#define IC_INTR_STAT            (ICBASE + 0x02c)
#define IC_INTR_MASK            (ICBASE + 0x030)
#define IC_RAW_INTR_STAT        (ICBASE + 0x034)
#define IC_RX_TL                (ICBASE + 0x038)
#define IC_TX_TL                (ICBASE + 0x03C)
#define IC_CLR_INTR             (ICBASE + 0x040)
#define IC_CLR_RX_UNDR          (ICBASE + 0x044)
#define IC_CLR_RX_OVER          (ICBASE + 0x048)
#define IC_CLR_TX_OVER          (ICBASE + 0x04C)
#define IC_CLR_RD_REQ           (ICBASE + 0x050)
#define IC_CLR_TX_ABRT          (ICBASE + 0x054)
#define IC_CLR_RX_DONE          (ICBASE + 0x058)
#define IC_CLR_ACTIVITY         (ICBASE + 0x05C)
#define IC_CLR_STOP_DET         (ICBASE + 0x060)
#define IC_CLR_START_DET        (ICBASE + 0x064)
#define IC_CLR_GEN_CALL         (ICBASE + 0x068)
#define IC_ENABLE               (ICBASE + 0x06C)
#define IC_STATUS               (ICBASE + 0x070)
#define IC_TXFLR                (ICBASE + 0x074)
#define IC_RXFLR                (ICBASE + 0x078)
#define IC_SRESET               (ICBASE + 0x07C)
#define IC_TX_ABRT_SOURCE       (ICBASE + 0x080)

// ECPM addresses
#if 0 //zluo, it is define in DTCP/dtcpECCDice2Defs.h
#define ECPM_STATUS             (ECPMBASE + 0x000) // read-only
#define ECPM_CONTROL            (ECPMBASE + 0x004)
#endif

#define ECPM_U_PART_0           (ECPMBASE + 0x014)
#define ECPM_U_PART_1           (ECPMBASE + 0x018)
#define ECPM_U_PART_2           (ECPMBASE + 0x01C)
#define ECPM_U_PART_3           (ECPMBASE + 0x020)
#define ECPM_U_PART_4           (ECPMBASE + 0x024)

#define ECPM_GX_PART_0          (ECPMBASE + 0x028)
#define ECPM_GX_PART_1          (ECPMBASE + 0x02C)
#define ECPM_GX_PART_2          (ECPMBASE + 0x030)
#define ECPM_GX_PART_3          (ECPMBASE + 0x034)
#define ECPM_GX_PART_4          (ECPMBASE + 0x038)

#define ECPM_GY_PART_0          (ECPMBASE + 0x03C)
#define ECPM_GY_PART_1          (ECPMBASE + 0x040)
#define ECPM_GY_PART_2          (ECPMBASE + 0x044)
#define ECPM_GY_PART_3          (ECPMBASE + 0x048)
#define ECPM_GY_PART_4          (ECPMBASE + 0x04C)

#define ECPM_A_PART_0           (ECPMBASE + 0x050)
#define ECPM_A_PART_1           (ECPMBASE + 0x054)
#define ECPM_A_PART_2           (ECPMBASE + 0x058)
#define ECPM_A_PART_3           (ECPMBASE + 0x05C)
#define ECPM_A_PART_4           (ECPMBASE + 0x060)

#define ECPM_P_PART_0           (ECPMBASE + 0x064)
#define ECPM_P_PART_1           (ECPMBASE + 0x068)
#define ECPM_P_PART_2           (ECPMBASE + 0x06C)
#define ECPM_P_PART_3           (ECPMBASE + 0x070)
#define ECPM_P_PART_4           (ECPMBASE + 0x074)

#define ECPM_Y_XTRA_PART_0      (ECPMBASE + 0x078)
#define ECPM_Y_XTRA_PART_1      (ECPMBASE + 0x07c)
#define ECPM_Y_XTRA_PART_2      (ECPMBASE + 0x080)
#define ECPM_Y_XTRA_PART_3      (ECPMBASE + 0x084)
#define ECPM_Y_XTRA_PART_4      (ECPMBASE + 0x088)

#define ECPM_X_RESULT_PART_0    (ECPMBASE + 0x08C) // read-only
#define ECPM_X_RESULT_PART_1    (ECPMBASE + 0x090) // read-only
#define ECPM_X_RESULT_PART_2    (ECPMBASE + 0x094) // read-only
#define ECPM_X_RESULT_PART_3    (ECPMBASE + 0x098) // read-only
#define ECPM_X_RESULT_PART_4    (ECPMBASE + 0x09C) // read-only

#define ECPM_Y_RESULT_PART_0    (ECPMBASE + 0x0A0) // read-only
#define ECPM_Y_RESULT_PART_1    (ECPMBASE + 0x0A4) // read-only
#define ECPM_Y_RESULT_PART_2    (ECPMBASE + 0x0A8) // read-only
#define ECPM_Y_RESULT_PART_3    (ECPMBASE + 0x0AC) // read-only
#define ECPM_Y_RESULT_PART_4    (ECPMBASE + 0x0B0) // read-only


// Gray code decoders addresses
#define GRAY_INT_STATUS         (GRAYBASE + 0x00) // read-only
#define GRAY_INT_SETUP          (GRAYBASE + 0x04)
#define GRAY_COUNTERS           (GRAYBASE + 0x08) // read-only


// General purpose control & status register addresses
#define GPCSR_MISC              (GPCSRBASE + 0x000)
#define GPCSR_IO_SEL            (GPCSRBASE + 0x004)
#define GPCSR_DSAI_SEL          (GPCSRBASE + 0x008)
#define GPCSR_DSAI_CLOCK_INV    (GPCSRBASE + 0x00C)
#define GPCSR_VIDEO_SEL         (GPCSRBASE + 0x010)

#define GPCSR_MUX_SEL0          ((GPCSRBASE + 0x004)
#define GPCSR_MUX_SEL1          ((GPCSRBASE + 0x008)
#define GPCSR_MUX_SEL2          ((GPCSRBASE + 0x00C)
#define GPCSR_MUX_SEL3          ((GPCSRBASE + 0x010)
#define GPCSR_MUX_SEL4          ((GPCSRBASE + 0x014)
#define GPCSR_MUX_SEL5          ((GPCSRBASE + 0x018)
#define GPCSR_MUX_SEL6          ((GPCSRBASE + 0x01C)
#define GPCSR_MUX_SEL7          ((GPCSRBASE + 0x020)

#define GPCSR_IRQ_MUX_5_TO_0_OFF (0x024)
#define GPCSR_IRQ_MUX_5_TO_0    (GPCSRBASE + GPCSR_IRQ_MUX_5_TO_0_OFF)
#define GPCSR_IRQ_MUX_11_TO_6_OFF (0x028)
#define GPCSR_IRQ_MUX_11_TO_6   (GPCSRBASE + GPCSR_IRQ_MUX_11_TO_6_OFF)
#define GPCSR_IRQ_MUX_17_TO_12_OFF (0x02C)
#define GPCSR_IRQ_MUX_17_TO_12  (GPCSRBASE + GPCSR_IRQ_MUX_17_TO_12_OFF)
#define GPCSR_IRQ_MUX_18_OFF    (0x030)
#define GPCSR_IRQ_MUX_18        (GPCSRBASE + GPCSR_IRQ_MUX_18_OFF)

#define GPCSR_FIQ_MUX_5_TO_0_OFF (0x034)
#define GPCSR_FIQ_MUX_5_TO_0    (GPCSRBASE + GPCSR_FIQ_MUX_5_TO_0_OFF)
#define GPCSR_FIQ_MUX_7_TO_6_OFF (0x038)
#define GPCSR_FIQ_MUX_7_TO_6    (GPCSRBASE + GPCSR_FIQ_MUX_7_TO_6_OFF)

// Isoc/PDB interface address
#define PDB_INT_CFG             (PDBINTBASE + 0x00)


// Power manager addresses
#define PWRMGR_SWRST            (PWRMGRBASE + 0x00)
#define PWRMGR_MUX              (PWRMGRBASE + 0x04)
#define PWRMGR_INT              (PWRMGRBASE + 0x08)
#define PWRMGR_STAT             (PWRMGRBASE + 0x0C)
#define PWRMGR_RST_FLAGS        (PWRMGRBASE + 0x10)y

//---------- (060301) Following defines are for rebuilt ARM system ----------

//Physical interrupts sorted by priority in descending order

#define IRQ_SLINKOFFSET         15
#define IRQ_SLINKBIT            BIT0
#define IRQ_LLCOFFSET           15
#define IRQ_LLCBIT              BIT0
#define IRQ_LINKONOFFSET        14
#define IRQ_LINKONBIT           BIT1
#define IRQ_I2COFFSET           13
#define IRQ_I2CBIT              BIT2
#define IRQ_UART0OFFSET         12
#define IRQ_UART0BIT            BIT3
#define IRQ_UART1OFFSET         11
#define IRQ_UART1BIT            BIT4
#define IRQ_TIMEROFFSET         10
#define IRQ_TIMERBIT            BIT5
#define IRQ_GPIOOFFSET          9
#define IRQ_GPIOBIT             BIT6
#define IRQ_WATCHDOGOFFSET      8
#define IRQ_WATCHDOGBIT         BIT7
#define IRQ_ECPMOFFSET          7
#define IRQ_ECPMBIT             BIT8
#define IRQ_PWRMGROFFSET        6
#define IRQ_PWRMGRBIT           BIT9
#define IRQ_GRAYOFFSET          5
#define IRQ_GRAYBIT             BIT10
#define IRQ_AVS0OFFSET          4
#define IRQ_AVS0BIT             BIT11
#define IRQ_AVS1OFFSET          3
#define IRQ_AVS1BIT             BIT12
#define IRQ_AVS2OFFSET          2
#define IRQ_AVS2BIT             BIT13
#define IRQ_APBAUDIOIRQOFFSET   1
#define IRQ_APBAUDIOIRQBIT      BIT14
#define IRQ_APBAUDIOOVERFLOWBIT BIT15
#define IRQ_HPLL0BIT            BIT16
#define IRQ_HPLL1BIT            BIT17
#define IRQ_MIDIBIT             BIT18

//-----------------------------------------------------------------------------
// end of plf_io.h
#endif // CYGONCE_HAL_PLF_IO_H
