//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2005 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
	
	Purpose:	Workaround for mixer bug at 176.4 and 192kHz

	Description:
						This module contains the basic FIQ routines to implement the two
						workarounds for the mixer hardware errata.
						1) if _MIXFIX_PERFECT4 defined
							Takes 8 mixer outputs and combine them into 4 perfect mixed channels.
							In order to use this route:
								Mix ch 0-3  to APB ch 0-3
								Mix ch 8-11 to APB ch 4-7
							The reconstructed channels will be available on:
								APB Ch 0-3
							NOTE:
								This fix requires the routing of Mix Ch 8-11 to be placed after
								position 20 in the router!!!!!
						
						2) if _MIXFIX_PERFECT4 NOT defined
							Takes 8 mixer outputs and combine them into 8 downsampled mixed channels.
							In order to use this route:
								Mix ch 0-7  to APB ch 0-7
							The reconstructed channels will be available on:
								APB Ch 0-7
							
						This module will automatically create a simple bypass in the APB algo. when
						not in HIGH rate mode. It is however recomended to not route through the APB
						when running at lower rate modes and to disable the FIQ routine.
						The FIQ routine can be enabled/disabled with mixfixEnable.

	Revisions:
		created 17/02/2008 ml

	
****************************************************************************/

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCInterrupt.h"
#include <cyg/hal/plf_io.h>
#include "diceDefs.h"
#include "mixfix.h"

#ifdef _MIXFIX_FIQ

#ifndef _FIQ
#error "_FIQ must be defined with _MIXFIX_FIQ!!!"
#endif

#define	MIXFIX_SRAM_DATA			__attribute__  ((section (".sram.data")))
#define	MIXFIX_SRAM_CODE			__attribute__  ((section (".sram.code")))


static int		idxgood		MIXFIX_SRAM_DATA;

static void apb_fiq_mixfix(void) MIXFIX_SRAM_CODE;

// PLEASE NOTE: mute should be routed to unused channels in the APB interface
//              otherwise the auto detect routine will be confused
//

#ifndef _MIXFIX_PERFECT4

static void apb_fiq_mixfix(void)
{	
  
	asm volatile(
		"	ldr	r2,=0xCE160000   \n\t"		// r2 = pointer to APB audio buffer

		//check if we are in high mode
		"	ldr r3,=0xCE010004 \n\t"			// r3 = pointer to Clock controller DOMAIN_CTRL
		"	ldr r3,[r3] \n\t"							
		"	subs r3,r3,#0x20 \n\t"				
		"	beq L001 \n\t"								// if (RTR_FS!=2) //high mode
		//Just do a straight bypass
		"	ldmia r2,{r4-r11}   \n\t"			//   move input data 8 ch 4 samples
		" stmia r2!,{r4-r11}   \n\t"		//	 to output data
		"	ldmia r2,{r4-r11}   \n\t"
		" stmia r2!,{r4-r11}   \n\t"
		"	ldmia r2,{r4-r11}   \n\t"
		" stmia r2!,{r4-r11}   \n\t"
		"	ldmia r2,{r4-r11}   \n\t"
		" stmia r2!,{r4-r11}   \n\t"
		//signal that we handled the interrupt
		"	ldr	r0,=0xCE160100		\n\t"		//   clear interrupt
		"	mov	r1,#0				\n\t"
		"	str	r1,[r0]				\n\t"
		"	mov pc,lr			\n\t"	          //   return
		
		"L001: \n\t"										// else
		//r2-> good data, r3->bad data
		"	add r3,r2,#32   \n\t"					//	r3 = pointer so second row of samples
		//check if we need to swap r2 and r3
		"	ldr r0,=idxgood   \n\t"				//	if (idxgood)
		"	ldr r1,[r0]   \n\t"
		"	tst	r1,#1   \n\t"
		"	movne r4,r2   \n\t"						//		swap r2 and r3
		"	movne	r2,r3   \n\t"
		" movne r3,r4   \n\t"

		//check if r3-> is constant			
		"	ldmia r3,{r4-r11}   \n\t"			//  check if bad rows are still bad rows
		"	add	r4,r4,r5   \n\t"
		"	add	r4,r4,r6   \n\t"
		"	add	r4,r4,r7   \n\t"					//  by summing 8 channels of first row
		"	add	r4,r4,r8   \n\t"          //  and subtracting sum of 8 channels next row
		"	add	r4,r4,r9   \n\t"					//	If sum is zero we are pretty confident that
		"	add	r4,r4,r10   \n\t"					//  this is constant data and therefore the bad ones
		"	add	r12,r4,r11   \n\t"
		" add r3,r3,#64   \n\t"
		"	ldmia r3,{r4-r11}   \n\t"
		"	add	r4,r4,r5   \n\t"
		"	add	r4,r4,r6   \n\t"
		"	add	r4,r4,r7   \n\t"
		"	add	r4,r4,r8   \n\t"
		"	add	r4,r4,r9   \n\t"
		"	add	r4,r4,r10   \n\t"
		"	add	r4,r4,r11   \n\t"
		" subs r4,r4,r12   \n\t"
		" eorne r1,r1,#1   \n\t"
		" strne r1,[r0]   \n\t"					//	If not zero, flip the idxgood bit so next time around
																		//  we will consider these rows the good ones.

		//done with that check, let's move the data
		"	ldr	r3,=0xCE160000   \n\t"		//	Move the two good rows of data into the for output rows
		"	ldmia r2,{r4-r11}   \n\t"
		" stmia r3!,{r4-r11}   \n\t"
		" stmia r3!,{r4-r11}   \n\t"
		" add  r2,r2,#64   \n\t"

		"	ldmia r2,{r4-r11}   \n\t"
		" stmia r3!,{r4-r11}   \n\t"
		" stmia r3!,{r4-r11}	\n\t"

		//signal that we handled the interrupt
		"	ldr	r0,=0xCE160100		\n\t"		//  Clear interrupt
		"	mov	r1,#0				\n\t"
		"	str	r1,[r0]				\n\t"
		"	mov pc,lr			\n\t"						//  Return

		);
}

#else //if _MIXFIX_PERFECT4

static void apb_fiq_mixfix(void)
{	
  
	asm volatile(
		"	ldr	r2,=0xCE160000   \n\t"		// r2 = pointer to APB audio buffer
																		
		//check if we are in high mode	
		"	ldr r3,=0xCE010004 \n\t"			// r3 = pointer to Clock controller DOMAIN_CTRL
		"	ldr r3,[r3] \n\t"							
		"	subs r3,r3,#0x20 \n\t"				
		"	beq L002 \n\t"								// if (RTR_FS!=2) //high mode
		//Just do a straight bypass			
		"	ldmia r2,{r4-r11}   \n\t"			//   move input data 8 ch 4 samples
		" stmia r2!,{r4-r11}   \n\t"		//	 to output data
		"	ldmia r2,{r4-r11}   \n\t"			
		" stmia r2!,{r4-r11}   \n\t"		
		"	ldmia r2,{r4-r11}   \n\t"			
		" stmia r2!,{r4-r11}   \n\t"		
		"	ldmia r2,{r4-r11}   \n\t"			
		" stmia r2!,{r4-r11}   \n\t"		
		//signal that we handled the interrupterrupt and return
		"	ldr	r0,=0xCE160100		\n\t"		//   clear interrupt
		"	mov	r1,#0				\n\t"					
		"	str	r1,[r0]				\n\t"				
		"	mov pc,lr			\n\t"						//   return
																		
		"L002: \n\t"										// else
				//r2-> good data, r3->bad data
		"	add r3,r2,#32   \n\t"					//	r3 = pointer so second row of samples
		//check if we need to swap r2 and r3
		"	ldr r0,=idxgood   \n\t"				//	if (idxgood)
		"	ldr r1,[r0]   \n\t"						
		"	tst	r1,#1   \n\t"							
		"	movne r4,r2   \n\t"						//		swap r2 and r3
		"	movne	r2,r3   \n\t"						
		" movne r3,r4   \n\t"						
																		
		//check if r3-> is constant			
		"	ldmia r3,{r4-r11}   \n\t"			//  check if bad rows are still bad rows
		"	add	r4,r4,r5   \n\t"					
		"	add	r4,r4,r6   \n\t"					
		"	add	r4,r4,r7   \n\t"					//  by summing 8 channels of first row
		"	add	r4,r4,r8   \n\t"					//  and subtracting sum of 8 channels next row
		"	add	r4,r4,r9   \n\t"					//	If sum is zero we are pretty confident that
		"	add	r4,r4,r10   \n\t"					//  this is constant data and therefore the bad ones
		"	add	r12,r4,r11   \n\t"				
		" add r3,r3,#64   \n\t"					
		"	ldmia r3,{r4-r11}   \n\t"			
		"	add	r4,r4,r5   \n\t"					
		"	add	r4,r4,r6   \n\t"					
		"	add	r4,r4,r7   \n\t"					
		"	add	r4,r4,r8   \n\t"					
		"	add	r4,r4,r9   \n\t"					
		"	add	r4,r4,r10   \n\t"					
		"	add	r4,r4,r11   \n\t"					
		" subs r4,r4,r12   \n\t"				
		" eorne r1,r1,#1   \n\t"				
		" strne r1,[r0]   \n\t"					//	If not zero, flip the idxgood bit so next time around
																		//  we will consider these rows the good ones.
		//done with that check, let's move the data
		"	ldr	r3,=0xCE160000   \n\t"
		"	ldmia r2,{r4-r11}   \n\t"			//	8 good samples (last 4 only good if fetched late)
		" stmia r3!,{r4-r7}  \n\t"			//  Store first 4 as 4 channels of row 0
		" add r3,r3,#16  \n\t"					//
		" stmia r3!,{r8-r11}   \n\t"		//  Store last 4 as 4 channels of row 1
		" add  r2,r2,#64   \n\t"
		" add r3,r3,#16  \n\t"					//
		"	ldmia r2,{r4-r11}   \n\t"			//	8 good samples (last 4 only good if fetched late)
		" stmia r3!,{r4-r7}   \n\t"			//	Store first 4 as 4 channels of row 2
		" add r3,r3,#16  \n\t"					//
		" stmia r3!,{r8-r11}	\n\t"			//	Store last 4 as 4 channels of row 3

		//signal that we handled the interrupt
		"	ldr	r0,=0xCE160100		\n\t"		//	Clear interrupt
		"	mov	r1,#0				\n\t"
		"	str	r1,[r0]				\n\t"
		"	mov pc,lr			\n\t"						//	Return

		);
}

#endif //_MIXFIX_PERFECT4


HRESULT mixfixEnable (BOOL bEnable)
{
	*((volatile uint32*)ICTL_FIQ_MASK) = bEnable ? 0x00 : 0x01;
	//we need to bring the mixer back to 16 ch
	if (!bEnable)
		*mDicePtr(DICE_MIXER_BASE+0x8) = 16;
	return NO_ERROR;
}



HRESULT mixfixInitialize (void)
{
	//we need to set-up the FIQ mux for APB int.
	*((volatile uint32*)GPCSR_FIQ_MUX_5_TO_0) = 0xe; //FIQ0=ARM_AUDIO_IRQ
	//let's mask the FIQ0 so we can use the mask bit to turn this functionality off and on
	*((volatile uint32*)ICTL_FIQ_MASK) = 0x1;
	idxgood = 0;
	//now it is safe to install the handler, it will not run now
	return TCInterruptCreateFIQ(&apb_fiq_mixfix);
}

#endif //_MIXFIX_FIQ

