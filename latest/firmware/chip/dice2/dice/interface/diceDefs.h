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
//===========================================================================
/****************************************************************************
	
	Purpose:	DICE register definition for 
				router and clock controller
	
	Revisions:
		created 12/06/2003 kbj

****************************************************************************/

#ifndef _DICEDEFS_H
#define _DICEDEFS_H

#include "coreDefs.h"

// Macro to convert register to memory pointer
#define mDicePtr(x)					((volatile int *)(DICEBASE+x))

_TC_INLINE void diceRegRead(uint32 registerOffset, uint32 *value) _TC_INLINE_ALWAYS;
_TC_INLINE void diceRegRead(uint32 registerOffset, uint32 *value)
// Read a value from the DICE
{
	*value = *mDicePtr(registerOffset);
}

_TC_INLINE void diceRegWrite(uint32 registerOffset, uint32 value) _TC_INLINE_ALWAYS;
_TC_INLINE void diceRegWrite(uint32 registerOffset, uint32 value)
// Write a value to the DICE
{
	*mDicePtr(registerOffset) = value;
}

// Receiver and transmitter channels 0-7, for Dice Rx/Tx, 0-15 for AVS Rx/Tx
#define CH_ID0			0
#define CH_ID1			1
#define CH_ID2			2
#define CH_ID3			3
#define CH_ID4			4
#define CH_ID5			5
#define CH_ID6			6
#define CH_ID7			7
#define CH_ID8			8
#define CH_ID9 			9
#define CH_ID10			10
#define CH_ID11			11
#define CH_ID12			12
#define CH_ID13			13
#define CH_ID14			14
#define CH_ID15			15
#define ALL_CH			16

// Identification for router 1,2
#define ROUTER_ID1		0	
#define ROUTER_ID2		1

// Dice device receiver types 
typedef enum
{
	DEV_AES_RX,
	DEV_ADAT_RX,
	DEV_TDIF_RX,
	DEV_DSAI_RX0,
	DEV_DSAI_RX1,
	DEV_DSAI_RX2,
	DEV_DSAI_RX3,
	DEV_I2S_RX0,
	DEV_I2S_RX1,
	DEV_I2S_RX2,
	DEV_APB_RX,
	DEV_AVS_ADO1,
	DEV_AVS_ADO2,
	DEV_AVS_ADO3,
	DEV_AVS_ADO4,
	NUM_DEVICES_RX				// 15 Rx devices
} DEV_RX_TYPES;

#define DEV_MUTED NUM_DEVICES_RX  // special device, always muted

// Dice device transmitter types 
typedef enum
{
	DEV_AES_TX,
	DEV_ADAT_TX,
	DEV_TDIF_TX,
	DEV_DSAI_TX0,
	DEV_DSAI_TX1,
	DEV_DSAI_TX2,
	DEV_DSAI_TX3,
	DEV_I2S_TX0,
	DEV_I2S_TX1,
	DEV_I2S_TX2,
	DEV_APB_TX,
	DEV_AVS_ADI1,
	DEV_AVS_ADI2,
	DEV_PWM_TX,
	NUM_DEVICES_TX				// 14 Tx devices
} DEV_TX_TYPES;


//**********************************************************
// DICE register base definitions
#define DICE_ROUTER_BASE			0x000000
#define DICE_CLOCK_BASE				0x010000
#define DICE_AESRX_BASE				0x020000
#define DICE_AESTX_BASE				0x030000
#define DICE_ADATRX_BASE			0x040000
#define DICE_ADATTX_BASE			0x050000
#define DICE_TDIFRX_BASE			0x060000
#define DICE_TDIFTX_BASE			0x070000
#define DICE_DSAI0RX_BASE			0x080000
#define DICE_DSAI0TX_BASE			0x090000
#define DICE_DSAI1RX_BASE			0x0A0000
#define DICE_DSAI1TX_BASE			0x0B0000
#define DICE_DSAI2RX_BASE			0x0C0000
#define DICE_DSAI2TX_BASE			0x0D0000
#define DICE_DSAI3RX_BASE			0x0E0000
#define DICE_DSAI3TX_BASE			0x0F0000
#define DICE_I2S0RX_BASE			0x100000
#define DICE_I2S0TX_BASE			0x110000
#define DICE_I2S1RX_BASE			0x120000
#define DICE_I2S1TX_BASE			0x130000
#define DICE_I2S2RX_BASE			0x140000
#define DICE_I2S2TX_BASE			0x150000
#define DICE_APB_BASE				  0x160000
#define DICE_PWMTX_BASE				0x170000

//**********************************************************
// DICE ROUTER register definitions
#define DICE_ROUTER1_SETUP			(DICE_ROUTER_BASE+0x000)
#define DICE_ROUTER1_ENTRY			(DICE_ROUTER_BASE+0x400)
#define DICE_ROUTER2_SETUP			(DICE_ROUTER_BASE+0x800)
#define DICE_ROUTER2_ENTRY			(DICE_ROUTER_BASE+0xc00)


#if 1 //KBJ??? for Dice2 test only

#define DICE_TOP_LEVEL_REG			0x300000

//**********************************************************
// DICE_TOP_LEVEL_REG register
#define OPTO_OUT_AES_SEL			BIT0
#define DSAI_RX0_CLK_IN_INV			BIT1
#define DSAI_RX1_CLK_IN_INV			BIT2
#define DSAI_RX2_CLK_IN_INV			BIT3
#define DSAI_RX3_CLK_IN_INV			BIT4
#define RST_N						BIT5
#define UP_DOWN_ADAT_VCO_SEL		BIT6

#define EXT_AES_MCK_PRE_MASK		0x0180
#define EXT_AES_MCK_PRE_SHIFT		7
#define EXT_ADAT_MCK_PRE_MASK		0x0600
#define EXT_ADAT_MCK_PRE_SHIFT		9
#define HPLL_X3_PRE_MASK			0x1800
#define HPLL_X3_PRE_SHIFT			11

#define DIVIDE_NONE					0
#define DIVIDE_BY2					1
#define DIVIDE_BY4					2
#define DIVIDE_BY8					3

#define IRQ1_SEL					BIT13
#define IRQ2_SEL					BIT14
#define IRQ3_SEL					BIT15
#define IRQ4_SEL					BIT16
#define IRQ1_OE						BIT17
#define IRQ2_OE						BIT18
#define IRQ3_OE						BIT19
#define IRQ4_OE						BIT20
#define AES_VCO_UP_POLARITY_SEL		BIT21
#define AES_VCO_DN_POLARITY_SEL		BIT22
#define ADAT_VCO_UP_POLARITY_SEL	BIT23
#define ADAT_VCO_DN_POLARITY_SEL	BIT24
#define TDIF_VCO_UP_POLARITY_SEL	BIT25
#define TDIF_VCO_DN_POLARITY_SEL	BIT26

#endif

#endif
