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
 * DESCRIPTION: Bitstring32
 
	macros and functions for handling bitstrings (32 bit)

	Revisions:
		created 09/26/2002 lm

****************************************************************************/

#ifndef _BITSTRING32_H
#define _BITSTRING32_H

#include "TCTypes.h"

//***************************************************************************
// bit values

#define BITSTRING32(	b31,b30,b29,b28,b27,b26,b25,b24,	\
						b23,b22,b21,b20,b19,b18,b17,b16,	\
						b15,b14,b13,b12,b11,b10, b9, b8,	\
						 b7, b6, b5, b4, b3, b2, b1, b0)	\
(\
((b31) ? BIT31 : 0) |\
((b30) ? BIT30 : 0) |\
((b29) ? BIT29 : 0) |\
((b28) ? BIT28 : 0) |\
((b27) ? BIT27 : 0) |\
((b26) ? BIT26 : 0) |\
((b25) ? BIT25 : 0) |\
((b24) ? BIT24 : 0) |\
((b23) ? BIT23 : 0) |\
((b22) ? BIT22 : 0) |\
((b21) ? BIT21 : 0) |\
((b20) ? BIT20 : 0) |\
((b19) ? BIT19 : 0) |\
((b18) ? BIT18 : 0) |\
((b17) ? BIT17 : 0) |\
((b16) ? BIT16 : 0) |\
((b15) ? BIT15 : 0) |\
((b14) ? BIT14 : 0) |\
((b13) ? BIT13 : 0) |\
((b12) ? BIT12 : 0) |\
((b11) ? BIT11 : 0) |\
((b10) ? BIT10 : 0) |\
(( b9) ?  BIT9 : 0) |\
(( b8) ?  BIT8 : 0) |\
(( b7) ?  BIT7 : 0) |\
(( b6) ?  BIT6 : 0) |\
(( b5) ?  BIT5 : 0) |\
(( b4) ?  BIT4 : 0) |\
(( b3) ?  BIT3 : 0) |\
(( b2) ?  BIT2 : 0) |\
(( b1) ?  BIT1 : 0) |\
(( b0) ?  BIT0 : 0))

#ifdef _CLI
void	bitstring32Display(uint32 bitstring);
void	bitstring8Display(uint8 bitstring);
#endif //_CLI

#endif
