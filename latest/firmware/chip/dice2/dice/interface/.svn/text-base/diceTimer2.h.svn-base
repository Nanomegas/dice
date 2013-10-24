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
	
	Purpose:	DICE - timer2 utility
	
	Revisions:
		created 27/08/2006 Zhao Luo

****************************************************************************/

#ifndef _DICE_TIMER2_H
#define _DICE_TIMER2_H

HRESULT diceTimer2InitializeTask(void);
HRESULT diceTimer2Initialize(void);


/** @addtogroup RTOSTime

	This module provides a fine timer in system clk ticks (1/49152000 sec), it 
	includes high 32 bit and low 32 bit. It uses dice2's 2nd hardware timer2 to 
	record current time value.
	If _LEAVE_TIMER2 is defined this function will only be precise to 10ms but
	timer2 will be free to use by the application.
	@{	
*/

/// Reset timer2
void diceTimer2Reset(void);

/// Get timer2's value, 
void diceTimer2Get(uint32 *high, uint32 *low);

/// This is to get the current value of timer2, in 1/49.152MHz Ticks
uint32 diceTimer2GetLow(void);

//! @}

#endif
