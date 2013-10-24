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
 * DESCRIPTION: Defines for the AVSystem API (AVS)

				Environment Specific Stuff (e.g. platform, os, compiler,...)

	Revisions:
		created 09/19/2002 lm

****************************************************************************/

#ifndef _AVSENVDEFS_H
#define _AVSENVDEFS_H

#include "TCTypes.h"

#include "coreDefs.h"

#define NUM_AVS_EVENTS			 2

// AVS INT1 bits
#define IRQ_ADO2_STREAM_UNLOCK			BIT31
#define IRQ_ADO2_STREAM_LOCK			  BIT30
#define IRQ_ADO2_REPEAT				      BIT29
#define IRQ_ADO2_SLIP				        BIT28
#define IRQ_ARXDO2_SYT_OVERFLOW		  BIT27
#define IRQ_ADO2_SYT_AGEOUT			    BIT26
#define IRQ_ARX2_LONG_PKT			      BIT25
#define IRQ_ARX2_PKT_ABORT			    BIT24
#define IRQ_ARX2_STATUS_ERR			    BIT23
#define IRQ_ADO2_NOT_COMPL          BIT22
#define IRQ_ADO2_MISSED_SYNC        BIT21

#define IRQ_ARX2_CFG_FAIL			      BIT19
#define IRQ_ARX2_CIP_FAIL			      BIT18
#define IRQ_ARX2_DBC_FAIL 			    BIT17
#define IRQ_ARXDO2_SYT_UNDERFLOW	  BIT16

#define IRQ_ADO1_STREAM_UNLOCK			BIT15
#define IRQ_ADO1_STREAM_LOCK			  BIT14
#define IRQ_ADO1_REPEAT				      BIT13
#define IRQ_ADO1_SLIP				        BIT12
#define IRQ_ARXDO1_SYT_OVERFLOW		  BIT11
#define IRQ_ADO1_SYT_AGEOUT			    BIT10
#define IRQ_ARX1_LONG_PKT			      BIT9
#define IRQ_ARX1_PKT_ABORT			    BIT8
#define IRQ_ARX1_STATUS_ERR			    BIT7
#define IRQ_ADO1_NOT_COMPL          BIT6
#define IRQ_ADO1_MISSED_SYNC        BIT5

#define IRQ_ARX1_CFG_FAIL			      BIT3
#define IRQ_ARX1_CIP_FAIL			      BIT2
#define IRQ_ARX1_DBC_FAIL 			    BIT1
#define IRQ_ARXDO1_SYT_UNDERFLOW	  BIT0


// AVS INT2 bits
#define IRQ_ATX2_STREAM_END			    BIT15
#define IRQ_ATX2_STREAM_START		    BIT14
#define IRQ_ATXDI2_SYT_OVERFLOW		  BIT13
#define IRQ_ATXDI2_SYT_UNDERFLOW	  BIT12
#define IRQ_ATX2_BOUNDRY_ERR		    BIT11
#define IRQ_ATX2_FRAME_AGEOUT		    BIT10
#define IRQ_ATX2_PKT_ABORT			    BIT9

#define IRQ_ATX1_STREAM_END			    BIT7
#define IRQ_ATX1_STREAM_START		    BIT6
#define IRQ_ATXDI1_SYT_OVERFLOW		  BIT5
#define IRQ_ATXDI1_SYT_UNDERFLOW	  BIT4
#define IRQ_ATX1_BOUNDRY_ERR		    BIT3
#define IRQ_ATX1_FRAME_AGEOUT		    BIT2
#define IRQ_ATX1_PKT_ABORT			    BIT1



#define NUM_AVS_RX					2		// audio only
#define NUM_AVS_TX					2		// audio only
#define NUM_PARTITIONS			4		// Number of partitions in Media FIFO

#define NO_PARTITION				0xff	// Flag to indicate partition not selected

// Macro to convert register to memory pointer
#define mAvsPtr(x)					((volatile int *)(AVSBASE+(x)))
#define mAvsCfgPtr					((volatile int *) PDBINTBASE)

#define AVS_INT_EVENT_ITEMS			30
#define AVS_EVENT_TEXT_SIZE			40 

extern uint32 avsIntMask[NUM_AVS_EVENTS];
extern uint32 avsIntItems[NUM_AVS_EVENTS];
extern uint32 avsIntCounter[NUM_AVS_EVENTS][AVS_INT_EVENT_ITEMS]; 
extern uint32 avsIntEvents[NUM_AVS_EVENTS][AVS_INT_EVENT_ITEMS];
extern char avsIntTexts[NUM_AVS_EVENTS][AVS_INT_EVENT_ITEMS][AVS_EVENT_TEXT_SIZE];

HRESULT avsIntEventHandleEvent(uint32 eventReg, uint32 index);

#ifdef _AVSINT_POLLING
void	avsIntPollHandlers(void);
#else //_AVSINT_POLLING
HRESULT	avsIntInstallHandlers(void);
#endif //_AVSINT_POLLING

_TC_INLINE void avsEnvRegRead(uint32 registerOffset, uint32 *value) _TC_INLINE_ALWAYS;
_TC_INLINE void avsEnvRegRead(uint32 registerOffset, uint32 *value)
// Read a value from the AVS
{
	*value = *mAvsPtr(registerOffset);
}

_TC_INLINE void avsEnvRegWrite(uint32 registerOffset, uint32 value) _TC_INLINE_ALWAYS;
_TC_INLINE void avsEnvRegWrite(uint32 registerOffset, uint32 value)
// Read a value from the AVS
{
	*mAvsPtr(registerOffset) = value;
}

_TC_INLINE void avsEnvConfigRegWrite(uint32 value) _TC_INLINE_ALWAYS;
_TC_INLINE void avsEnvConfigRegWrite(uint32 value)
// Write a value to the AVS configuration register 
{
	*mAvsCfgPtr = value;
}


#endif //_AVSENVDEFS_H
