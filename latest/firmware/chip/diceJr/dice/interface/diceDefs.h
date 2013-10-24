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
		updated 01/02/2007 ml

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

// Dice device receiver types 
typedef enum
{
	DEV_AES_RX,
	DEV_ADAT_RX,
	DEV_MIXER_RX,
	DEV_INS_RX0=4,
	DEV_INS_RX1,
	DEV_APB_RX=10,
	DEV_AVS_ADO1,
	DEV_AVS_ADO2,
	NUM_DEVICES_RX				// 13 Rx devices
} DEV_RX_TYPES;

#define DEV_MUTED 15  // special device, always muted

// Dice device transmitter types 
typedef enum
{
	DEV_AES_TX,
	DEV_ADAT_TX,
	DEV_MIXER_TX0,
	DEV_MIXER_TX1,
	DEV_INS_TX0,
	DEV_INS_TX1,
	DEV_APB_TX=10,
	DEV_AVS_ADI1,
	DEV_AVS_ADI2,
	NUM_DEVICES_TX				// 13 Tx devices
} DEV_TX_TYPES;


//**********************************************************
// DICE register base definitions
#define DICE_ROUTER_BASE			0x000000
#define DICE_CLOCK_BASE				0x010000
#define DICE_AESRX_BASE				0x020000
#define DICE_AESTX_BASE				0x030000
#define DICE_ADATRX_BASE			0x040000
#define DICE_ADATTX_BASE			0x050000
#define DICE_MIXER_BASE				0x060000
#define DICE_INS0RX_BASE			0x080000
#define DICE_INS1RX_BASE			0x0a0000
#define DICE_INS0TX_BASE			0x090000
#define DICE_INS1TX_BASE			0x0b0000

#define DICE_INSRX_LINOFS			0x20   //offset between line registers
#define DICE_INSTX_LINOFS			0x20   //offset between line registers
#define DICE_INSTX_CLK				0x80   //offset to clock register
#define DICE_INSTX_MUTE				0xfe0  //offset to mute register



#define DICE_APB_BASE				0x160000

//**********************************************************
// DICE ROUTER register definitions
#define DICE_ROUTER1_SETUP			(DICE_ROUTER_BASE+0x000)
#define DICE_ROUTER1_ENTRY			(DICE_ROUTER_BASE+0x400)




#endif
