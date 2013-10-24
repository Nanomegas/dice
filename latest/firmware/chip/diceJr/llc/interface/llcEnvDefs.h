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
	
	Purpose:	Defines for Link access for the Samsung 1394 LLC
				
				Environment Specific Stuff (e.g. platform, os, compiler,...)

	Revisions:
		created 23/09/2004 lm


****************************************************************************/

#ifndef _LLCENVDEFS_H_
#define _LLCENVDEFS_H_

/****************************************************************************/

#include "TCTasking.h"
#include "llcDefs.h"

#ifdef _DICE2_FPGA
#ifdef GHS
#include <Ncc_io.h>
#endif
#else //_DICE2_FPGA
//nothing
#endif //_DICE2_FPGA

_TC_INLINE void llcEnvInitialize(void) _TC_INLINE_ALWAYS;
_TC_INLINE void llcEnvInitialize(void)
{
	// Reset FPGA
#ifdef _DICE2_FPGA
#ifdef GHS
	(*NCC_GEN).portb.bits.data = 0;
	TCTaskWait(500); // Reset 0.5 sec.
	(*NCC_GEN).portb.bits.data = 0x04;
#elif defined ECOS
	*((volatile uint32 *) (0xffb00024)) &= ~0x4;
	TCTaskWait(500); // Reset 0.5 sec.
	*((volatile uint32 *) (0xffb00024)) |= 0x4;
#endif
#else //_DICE2_FPGA
//nothing
#endif //_DICE2_FPGA

	// Configurate interrupts from FPGA's
	// Set FPGA 1 ints to be input IRQ1 and IRQ2
#ifdef _DICE2_FPGA
	*((volatile uint32 *) (0x03001800)) =  0x00000940;
#else //_DICE2_FPGA
//nothing
#endif //_DICE2_FPGA
}

#endif // _LLCENVDEFS_H_
