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
 * DESCRIPTION: Utilities for fast handling of LLc packets

	Purpose:	
	
	Revisions:
		created 11/02/08 ml

****************************************************************************/

#include "llcUtils.h"


#pragma long_calls


//r0=dst, r1=src, r2 = n
void fastCopyQuads(uint32 * dst, uint32 * src, uint32 n)
{ 
	asm volatile(
		"	stmfd	sp!,{r4-r10}			\n\t"
		"	subs r2,r2,#0x40	\n\t"
		"	bmi skip64	\n\t"
		"loop64:	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	subs	r2,r2,#0x40	\n\t"
		"	bpl loop64			\n\t"
		"skip64:	\n\t"
		"	add r2,r2,#0x40			\n\t"

		"	subs r2,r2,#0x20	\n\t"
		"	bmi skip32	\n\t"
		"loop32:	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	subs	r2,r2,#0x20	\n\t"
		"	bpl loop32	\n\t"
		"skip32:	\n\t"
		"	add r2,r2,#0x20			\n\t"

		"	subs r2,r2,#0x8	\n\t"
		"	bmi skip8	\n\t"
		"loop8:	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	subs	r2,r2,#0x8	\n\t"
		"	bpl loop8	\n\t"
		"skip8:	\n\t"
		"	adds r2,r2,#0x8	\n\t"
		"	beq skip1			\n\t"
		"loop1:	\n\t"
		"	ldmia r1!,{r3}	\n\t"
		"	stmia r0!,{r3}	\n\t"
		"	subs r2,r2,#1	\n\t"
		"	bne loop1	\n\t"
	
		"skip1:	\n\t"
		"	ldmfd	sp!,{r4-r10}			\n\t"
		//"	mov	pc, lr	\n\t"
	);
}

/*
//move the packet to the LLC
				uint32 * pSrc=pContext->header.quadlets;
				uint16 n = pContext->headerLen;
				uint16 h = pContext->payloadLen;
				llcLinkRegWrite(ASY_TX_FIRST_REG_DP, *pSrc++); n--;
				if (!h) n--;
				while (n--) llcLinkRegWrite(ASY_CONTINUE_REG_DP, *pSrc++);
				if (h)
				{
					h--;
					pSrc = pContext->pPayload;
					while (h--) llcLinkRegWrite(ASY_CONTINUE_REG_DP, *pSrc++);
				}
				llcLinkRegWrite(ASY_CONTINUE_UPDATE_REG_DP, *pSrc++);
*/


//r0=pPayload, r1=pHeader, r2=nPayload, r3 = nHeader
void llcSendContext(uint32 * pPayload, uint32 * pHeader, uint32 nPayload, uint32 nHeader)
{ 
	asm volatile(
		"	stmfd	sp!,{r4-r10}			\n\t"
		"	mov r5,r0								\n\t" //r5=pPayload
		"	ldr r0,=0x82000074			\n\t" //r0->LLC first
		" ldr r4,[r1],#4					\n\t" //r4 = *pSrc++ (pSrc = PHeader)
		" str r4,[r0],#4					\n\t"	//*ASY_TX_FIRST_REG_DP++ = r4
		" subs r3,r3,#1						\n\t" //nHeader--;
		" cmp  r2,#0							\n\t" //if (!nPayload)
		" subeq r3,r3,#1					\n\t" //  nHeader--;
		//handle up to 3 more header quads
		" ldr r4,[r1],#4					\n\t" //r4 = *pSrc++
		" str r4,[r0]							\n\t" //*ASY_CONTINUE_REG_DP = r4
		" subs r3,r3,#1						\n\t" //nHeader--
		"	beq _doneHead						\n\t" //if (!nHeader) goto _doneHead
		" ldr r4,[r1],#4					\n\t" //r4 = *pSrc++
		" str r4,[r0]							\n\t" //*ASY_CONTINUE_REG_DP = r4
		" subs r3,r3,#1						\n\t" //nHeader--
		"	beq _doneHead						\n\t" //if (!nHeader) goto _doneHead
		" ldr r4,[r1],#4					\n\t" //r4 = *pSrc++
		" str r4,[r0]							\n\t" //*ASY_CONTINUE_REG_DP = r4
		"_doneHead:								\n\t" //We are done with header now
		" cmp  r2,#0							\n\t" //if (!nPayload)
		"	beq  _doneAll						\n\t" //{
		"	subs r2,r2,#1						\n\t" //	nPayload--;
		"	mov r1,r5								\n\t" //  r1=pSrc=pPayload
		//r0->LLC continue, r1=pPayload, r2=n less one		
		"	subs r2,r2,#0x40				\n\t"
		"	bmi _skip64							\n\t"
		"_loop64:							\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	str   r3,[r0]     	\n\t"
		"	str   r4,[r0]     	\n\t"
		"	str   r5,[r0]     	\n\t"
		"	str   r6,[r0]     	\n\t"
		"	str   r7,[r0]     	\n\t"
		"	str   r8,[r0]     	\n\t"
		"	str   r9,[r0]     	\n\t"
		"	str   r10,[r0]     	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	str   r3,[r0]     	\n\t"
		"	str   r4,[r0]     	\n\t"
		"	str   r5,[r0]     	\n\t"
		"	str   r6,[r0]     	\n\t"
		"	str   r7,[r0]     	\n\t"
		"	str   r8,[r0]     	\n\t"
		"	str   r9,[r0]     	\n\t"
		"	str   r10,[r0]     	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	str   r3,[r0]     	\n\t"
		"	str   r4,[r0]     	\n\t"
		"	str   r5,[r0]     	\n\t"
		"	str   r6,[r0]     	\n\t"
		"	str   r7,[r0]     	\n\t"
		"	str   r8,[r0]     	\n\t"
		"	str   r9,[r0]     	\n\t"
		"	str   r10,[r0]     	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	str   r3,[r0]     	\n\t"
		"	str   r4,[r0]     	\n\t"
		"	str   r5,[r0]     	\n\t"
		"	str   r6,[r0]     	\n\t"
		"	str   r7,[r0]     	\n\t"
		"	str   r8,[r0]     	\n\t"
		"	str   r9,[r0]     	\n\t"
		"	str   r10,[r0]     	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	str   r3,[r0]     	\n\t"
		"	str   r4,[r0]     	\n\t"
		"	str   r5,[r0]     	\n\t"
		"	str   r6,[r0]     	\n\t"
		"	str   r7,[r0]     	\n\t"
		"	str   r8,[r0]     	\n\t"
		"	str   r9,[r0]     	\n\t"
		"	str   r10,[r0]     	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	str   r3,[r0]     	\n\t"
		"	str   r4,[r0]     	\n\t"
		"	str   r5,[r0]     	\n\t"
		"	str   r6,[r0]     	\n\t"
		"	str   r7,[r0]     	\n\t"
		"	str   r8,[r0]     	\n\t"
		"	str   r9,[r0]     	\n\t"
		"	str   r10,[r0]     	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	str   r3,[r0]     	\n\t"
		"	str   r4,[r0]     	\n\t"
		"	str   r5,[r0]     	\n\t"
		"	str   r6,[r0]     	\n\t"
		"	str   r7,[r0]     	\n\t"
		"	str   r8,[r0]     	\n\t"
		"	str   r9,[r0]     	\n\t"
		"	str   r10,[r0]     	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	str   r3,[r0]     	\n\t"
		"	str   r4,[r0]     	\n\t"
		"	str   r5,[r0]     	\n\t"
		"	str   r6,[r0]     	\n\t"
		"	str   r7,[r0]     	\n\t"
		"	str   r8,[r0]     	\n\t"
		"	str   r9,[r0]     	\n\t"
		"	str   r10,[r0]     	\n\t"
		"	subs	r2,r2,#0x40		\n\t"
		"	bpl _loop64					\n\t"
		"_skip64:							\n\t"
		"	add r2,r2,#0x40			\n\t"

		"	subs r2,r2,#0x20		\n\t"
		"	bmi _skip32					\n\t"
		"_loop32:							\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	str   r3,[r0]     	\n\t"
		"	str   r4,[r0]     	\n\t"
		"	str   r5,[r0]     	\n\t"
		"	str   r6,[r0]     	\n\t"
		"	str   r7,[r0]     	\n\t"
		"	str   r8,[r0]     	\n\t"
		"	str   r9,[r0]     	\n\t"
		"	str   r10,[r0]     	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	str   r3,[r0]     	\n\t"
		"	str   r4,[r0]     	\n\t"
		"	str   r5,[r0]     	\n\t"
		"	str   r6,[r0]     	\n\t"
		"	str   r7,[r0]     	\n\t"
		"	str   r8,[r0]     	\n\t"
		"	str   r9,[r0]     	\n\t"
		"	str   r10,[r0]     	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	str   r3,[r0]     	\n\t"
		"	str   r4,[r0]     	\n\t"
		"	str   r5,[r0]     	\n\t"
		"	str   r6,[r0]     	\n\t"
		"	str   r7,[r0]     	\n\t"
		"	str   r8,[r0]     	\n\t"
		"	str   r9,[r0]     	\n\t"
		"	str   r10,[r0]     	\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	str   r3,[r0]     	\n\t"
		"	str   r4,[r0]     	\n\t"
		"	str   r5,[r0]     	\n\t"
		"	str   r6,[r0]     	\n\t"
		"	str   r7,[r0]     	\n\t"
		"	str   r8,[r0]     	\n\t"
		"	str   r9,[r0]     	\n\t"
		"	str   r10,[r0]     	\n\t"
		"	subs	r2,r2,#0x20		\n\t"
		"	bpl _loop32					\n\t"
		"_skip32:							\n\t"
		"	add r2,r2,#0x20			\n\t"

		"	subs r2,r2,#0x8			\n\t"
		"	bmi _skip8					\n\t"
		"_loop8:							\n\t"
		"	ldmia r1!,{r3-r10}	\n\t"
		"	str   r3,[r0]     	\n\t"
		"	str   r4,[r0]     	\n\t"
		"	str   r5,[r0]     	\n\t"
		"	str   r6,[r0]     	\n\t"
		"	str   r7,[r0]     	\n\t"
		"	str   r8,[r0]     	\n\t"
		"	str   r9,[r0]     	\n\t"
		"	str   r10,[r0]     	\n\t"
		"	subs	r2,r2,#0x8		\n\t"
		"	bpl _loop8					\n\t"
		"_skip8:							\n\t"
		"	adds r2,r2,#0x8			\n\t"
		"	beq _skip1					\n\t"
		"_loop1:							\n\t"
		"	ldr   r3,[r1],#4		\n\t"
		"	str   r3,[r0]     	\n\t"
		"	subs r2,r2,#1				\n\t"
		"	bne _loop1					\n\t"
		"_skip1:							\n\t"
		"_doneAll:						\n\t" //}
		//we just need to write the last one now
		"	ldr   r3,[r1]				\n\t" //r3 = *pSrc; //last payload or header
		"	str   r3,[r0,#4]    \n\t" //*ASY_CONTINUE_UPDATE_REG_DP = r3
		"	ldmfd	sp!,{r4-r10}	\n\t"
		//"	mov	pc, lr	\n\t"
	);
}


//r0=pPayload, r1=nPayload
void llcGetContext(uint32 * pPayload, uint32 nPayload)
{ 
	asm volatile(
		"	stmfd	sp!,{r4-r10}			\n\t"
		"	ldr r2,=0x82000084			\n\t" //r2->LLC Rx FIFO
		
		//r0->buffer, r1->n, r2->LLC Rx FIFO
		"	subs r1,r1,#0x40				\n\t"
		"	bmi __skip64							\n\t"
		"__loop64:							\n\t"
		"	ldr   r3,[r2]     	\n\t"
		"	ldr   r4,[r2]     	\n\t"
		"	ldr   r5,[r2]     	\n\t"
		"	ldr   r6,[r2]     	\n\t"
		"	ldr   r7,[r2]     	\n\t"
		"	ldr   r8,[r2]     	\n\t"
		"	ldr   r9,[r2]     	\n\t"
		"	ldr   r10,[r2]     	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	ldr   r3,[r2]     	\n\t"
		"	ldr   r4,[r2]     	\n\t"
		"	ldr   r5,[r2]     	\n\t"
		"	ldr   r6,[r2]     	\n\t"
		"	ldr   r7,[r2]     	\n\t"
		"	ldr   r8,[r2]     	\n\t"
		"	ldr   r9,[r2]     	\n\t"
		"	ldr   r10,[r2]     	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	ldr   r3,[r2]     	\n\t"
		"	ldr   r4,[r2]     	\n\t"
		"	ldr   r5,[r2]     	\n\t"
		"	ldr   r6,[r2]     	\n\t"
		"	ldr   r7,[r2]     	\n\t"
		"	ldr   r8,[r2]     	\n\t"
		"	ldr   r9,[r2]     	\n\t"
		"	ldr   r10,[r2]     	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	ldr   r3,[r2]     	\n\t"
		"	ldr   r4,[r2]     	\n\t"
		"	ldr   r5,[r2]     	\n\t"
		"	ldr   r6,[r2]     	\n\t"
		"	ldr   r7,[r2]     	\n\t"
		"	ldr   r8,[r2]     	\n\t"
		"	ldr   r9,[r2]     	\n\t"
		"	ldr   r10,[r2]     	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	ldr   r3,[r2]     	\n\t"
		"	ldr   r4,[r2]     	\n\t"
		"	ldr   r5,[r2]     	\n\t"
		"	ldr   r6,[r2]     	\n\t"
		"	ldr   r7,[r2]     	\n\t"
		"	ldr   r8,[r2]     	\n\t"
		"	ldr   r9,[r2]     	\n\t"
		"	ldr   r10,[r2]     	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	ldr   r3,[r2]     	\n\t"
		"	ldr   r4,[r2]     	\n\t"
		"	ldr   r5,[r2]     	\n\t"
		"	ldr   r6,[r2]     	\n\t"
		"	ldr   r7,[r2]     	\n\t"
		"	ldr   r8,[r2]     	\n\t"
		"	ldr   r9,[r2]     	\n\t"
		"	ldr   r10,[r2]     	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	ldr   r3,[r2]     	\n\t"
		"	ldr   r4,[r2]     	\n\t"
		"	ldr   r5,[r2]     	\n\t"
		"	ldr   r6,[r2]     	\n\t"
		"	ldr   r7,[r2]     	\n\t"
		"	ldr   r8,[r2]     	\n\t"
		"	ldr   r9,[r2]     	\n\t"
		"	ldr   r10,[r2]     	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	ldr   r3,[r2]     	\n\t"
		"	ldr   r4,[r2]     	\n\t"
		"	ldr   r5,[r2]     	\n\t"
		"	ldr   r6,[r2]     	\n\t"
		"	ldr   r7,[r2]     	\n\t"
		"	ldr   r8,[r2]     	\n\t"
		"	ldr   r9,[r2]     	\n\t"
		"	ldr   r10,[r2]     	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	subs	r1,r1,#0x40		\n\t"
		"	bpl __loop64					\n\t"
		"__skip64:							\n\t"
		"	add r1,r1,#0x40			\n\t"

		"	subs r1,r1,#0x20		\n\t"
		"	bmi __skip32					\n\t"
		"__loop32:							\n\t"
		"	ldr   r3,[r2]     	\n\t"
		"	ldr   r4,[r2]     	\n\t"
		"	ldr   r5,[r2]     	\n\t"
		"	ldr   r6,[r2]     	\n\t"
		"	ldr   r7,[r2]     	\n\t"
		"	ldr   r8,[r2]     	\n\t"
		"	ldr   r9,[r2]     	\n\t"
		"	ldr   r10,[r2]     	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	ldr   r3,[r2]     	\n\t"
		"	ldr   r4,[r2]     	\n\t"
		"	ldr   r5,[r2]     	\n\t"
		"	ldr   r6,[r2]     	\n\t"
		"	ldr   r7,[r2]     	\n\t"
		"	ldr   r8,[r2]     	\n\t"
		"	ldr   r9,[r2]     	\n\t"
		"	ldr   r10,[r2]     	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	ldr   r3,[r2]     	\n\t"
		"	ldr   r4,[r2]     	\n\t"
		"	ldr   r5,[r2]     	\n\t"
		"	ldr   r6,[r2]     	\n\t"
		"	ldr   r7,[r2]     	\n\t"
		"	ldr   r8,[r2]     	\n\t"
		"	ldr   r9,[r2]     	\n\t"
		"	ldr   r10,[r2]     	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	ldr   r3,[r2]     	\n\t"
		"	ldr   r4,[r2]     	\n\t"
		"	ldr   r5,[r2]     	\n\t"
		"	ldr   r6,[r2]     	\n\t"
		"	ldr   r7,[r2]     	\n\t"
		"	ldr   r8,[r2]     	\n\t"
		"	ldr   r9,[r2]     	\n\t"
		"	ldr   r10,[r2]     	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	subs	r1,r1,#0x20		\n\t"
		"	bpl __loop32					\n\t"
		"__skip32:							\n\t"
		"	add r1,r1,#0x20			\n\t"

		"	subs r1,r1,#0x8			\n\t"
		"	bmi __skip8					\n\t"
		"__loop8:							\n\t"
		"	ldr   r3,[r2]     	\n\t"
		"	ldr   r4,[r2]     	\n\t"
		"	ldr   r5,[r2]     	\n\t"
		"	ldr   r6,[r2]     	\n\t"
		"	ldr   r7,[r2]     	\n\t"
		"	ldr   r8,[r2]     	\n\t"
		"	ldr   r9,[r2]     	\n\t"
		"	ldr   r10,[r2]     	\n\t"
		"	stmia r0!,{r3-r10}	\n\t"
		"	subs	r1,r1,#0x8		\n\t"
		"	bpl __loop8					\n\t"
		"__skip8:							\n\t"
		"	adds r1,r1,#0x8			\n\t"
		"	beq __skip1					\n\t"
		"__loop1:							\n\t"
		"	ldr   r3,[r2]   		\n\t"
		"	str   r3,[r0],#4   	\n\t"
		"	subs r1,r1,#1				\n\t"
		"	bne __loop1					\n\t"
		"__skip1:							\n\t"
		"	ldmfd	sp!,{r4-r10}	\n\t"
		//"	mov	pc, lr	\n\t"
	);
}


#pragma long_calls_off

