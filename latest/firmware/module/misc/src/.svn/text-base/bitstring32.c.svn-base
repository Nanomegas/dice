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

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "bitstring32.h"

#ifdef _CLI
#include "cli.h"

void bitstring32Display(uint32 val)
{
	cliPrintf("%i%i%i%i%i%i%i%i ", ISBITSET(val,BIT31),ISBITSET(val,BIT30),ISBITSET(val,BIT29),ISBITSET(val,BIT28),ISBITSET(val,BIT27),ISBITSET(val,BIT26),ISBITSET(val,BIT25),ISBITSET(val,BIT24));
	cliPrintf("%i%i%i%i%i%i%i%i ", ISBITSET(val,BIT23),ISBITSET(val,BIT22),ISBITSET(val,BIT21),ISBITSET(val,BIT20),ISBITSET(val,BIT19),ISBITSET(val,BIT18),ISBITSET(val,BIT17),ISBITSET(val,BIT16));
	cliPrintf("%i%i%i%i%i%i%i%i ", ISBITSET(val,BIT15),ISBITSET(val,BIT14),ISBITSET(val,BIT13),ISBITSET(val,BIT12),ISBITSET(val,BIT11),ISBITSET(val,BIT10),ISBITSET(val,BIT9) ,ISBITSET(val,BIT8));
	cliPrintf("%i%i%i%i%i%i%i%i ", ISBITSET(val,BIT7) ,ISBITSET(val,BIT6) ,ISBITSET(val,BIT5) ,ISBITSET(val,BIT4) ,ISBITSET(val,BIT3) ,ISBITSET(val,BIT2) ,ISBITSET(val,BIT1) ,ISBITSET(val,BIT0));
}

void bitstring8Display(uint8 val)
{
	cliPrintf("%i%i%i%i%i%i%i%i ", ISBITSET(val,BIT7) ,ISBITSET(val,BIT6) ,ISBITSET(val,BIT5) ,ISBITSET(val,BIT4) ,ISBITSET(val,BIT3) ,ISBITSET(val,BIT2) ,ISBITSET(val,BIT1) ,ISBITSET(val,BIT0));
}
#endif //_CLI
