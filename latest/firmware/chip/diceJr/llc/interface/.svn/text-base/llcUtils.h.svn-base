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
	
	Purpose:	Samsung LLC interrupt handler
	
	Revisions:
		created 12/17/2002 kbj


****************************************************************************/
#ifndef _LLCUTILS_H
#define _LLCUTILS_H

#include "TCTypes.h"

#pragma long_calls

#define	LLC_SRAM_DATA			__attribute__((section (".sram.llcdata")))
#define	LLC_SRAM_CODE			__attribute__((section (".sram.llccode")))


void fastCopyQuads(uint32 * dst, uint32 * src, uint32 n) LLC_SRAM_CODE;
void llcSendContext(uint32 * pPayload, uint32 * pHeader, uint32 nPayload, uint32 nHeader) LLC_SRAM_CODE;
void llcGetContext(uint32 * pPayload, uint32 nPayload) LLC_SRAM_CODE;

//void llcStQuads(uint32 dst, uint32 src, uint32 n) LLC_SRAM_CODE;
#pragma long_calls_off

#endif
