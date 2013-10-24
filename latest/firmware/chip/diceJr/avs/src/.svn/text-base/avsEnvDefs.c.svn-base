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
		created 26/09/2004 lm

****************************************************************************/

#include "TCTypes.h"
#include "TCDebug.h"
#include "TCTasking.h"
#include "TCInterrupt.h"
#include "avs.h"
#include "avsDefs.h"


#include "avsEnvDefs.h"
#include "avsInt.h"

uint32 avsIntCounter[NUM_AVS_EVENTS][AVS_INT_EVENT_ITEMS]; 


#define	AVS_INT0_EVENT_ITEMS	30		// list of prioritised active interrupt 0
#define	AVS_INT1_EVENT_ITEMS	14		// list of prioritised active interrupt 1

uint32 avsIntMask[NUM_AVS_EVENTS] =
{
	APBA_INT0_MASK,
	APBA_INT1_MASK,
};

uint32 avsIntItems[NUM_AVS_EVENTS] =
{
	AVS_INT0_EVENT_ITEMS,
	AVS_INT1_EVENT_ITEMS,
}; 

uint32 avsIntEvents[NUM_AVS_EVENTS][AVS_INT_EVENT_ITEMS] = 
{
	// AVS INT0 bits
	{
		IRQ_ADO2_STREAM_UNLOCK		 ,
		IRQ_ADO2_STREAM_LOCK			 ,
		IRQ_ADO2_REPEAT				     ,
		IRQ_ADO2_SLIP				       ,
		IRQ_ARXDO2_SYT_OVERFLOW		 ,
		IRQ_ADO2_SYT_AGEOUT			   ,
		IRQ_ARX2_LONG_PKT			     ,
		IRQ_ARX2_PKT_ABORT			   ,
		IRQ_ARX2_STATUS_ERR			   ,
		IRQ_ADO2_NOT_COMPL         ,
		IRQ_ADO2_MISSED_SYNC       ,

		IRQ_ARX2_CFG_FAIL			     ,
		IRQ_ARX2_CIP_FAIL			     ,
		IRQ_ARX2_DBC_FAIL 			   ,
		IRQ_ARXDO2_SYT_UNDERFLOW	 ,

		IRQ_ADO1_STREAM_UNLOCK		 ,
		IRQ_ADO1_STREAM_LOCK			 ,
		IRQ_ADO1_REPEAT				     ,
		IRQ_ADO1_SLIP				       ,
		IRQ_ARXDO1_SYT_OVERFLOW		 ,
		IRQ_ADO1_SYT_AGEOUT			   ,
		IRQ_ARX1_LONG_PKT			     ,
		IRQ_ARX1_PKT_ABORT			   ,
		IRQ_ARX1_STATUS_ERR			   ,
		IRQ_ADO1_NOT_COMPL         ,
		IRQ_ADO1_MISSED_SYNC       ,

		IRQ_ARX1_CFG_FAIL			     ,
		IRQ_ARX1_CIP_FAIL			     ,
		IRQ_ARX1_DBC_FAIL 			   ,
		IRQ_ARXDO1_SYT_UNDERFLOW	 
	},
	// AVS INT1 bits
	{		
		IRQ_ATX2_STREAM_END			  ,
		IRQ_ATX2_STREAM_START		  ,
		IRQ_ATXDI2_SYT_OVERFLOW		,
		IRQ_ATXDI2_SYT_UNDERFLOW	,
		IRQ_ATX2_BOUNDRY_ERR		  ,
		IRQ_ATX2_FRAME_AGEOUT		  ,
		IRQ_ATX2_PKT_ABORT			  ,

		IRQ_ATX1_STREAM_END			  ,
		IRQ_ATX1_STREAM_START		  ,
		IRQ_ATXDI1_SYT_OVERFLOW		,
		IRQ_ATXDI1_SYT_UNDERFLOW	,
		IRQ_ATX1_BOUNDRY_ERR		  ,
		IRQ_ATX1_FRAME_AGEOUT		  ,
		IRQ_ATX1_PKT_ABORT			  
		
	},
};

#ifdef _SYSDEBUG
char avsIntTexts[NUM_AVS_EVENTS][AVS_INT_EVENT_ITEMS][AVS_EVENT_TEXT_SIZE] = 
{
	// AVS INT0 bits
	{
		"IRQ_ADO2_STREAM_UNLOCK" ,
		"IRQ_ADO2_STREAM_LOCK"	 ,
		"IRQ_ADO2_REPEAT"			   ,
		"IRQ_ADO2_SLIP"				   ,
		"IRQ_ARXDO2_SYT_OVERFLOW",
		"IRQ_ADO2_SYT_AGEOUT"		 ,
		"IRQ_ARX2_LONG_PKT"			 ,
		"IRQ_ARX2_PKT_ABORT"		 ,
		"IRQ_ARX2_STATUS_ERR"		 ,
		"IRQ_ADO2_NOT_COMPL"     ,
		"IRQ_ADO2_MISSED_SYNC"   ,

		"IRQ_ARX2_CFG_FAIL"		   ,
		"IRQ_ARX2_CIP_FAIL"		   ,
		"IRQ_ARX2_DBC_FAIL" 	   ,
		"IRQ_ARXDO2_SYT_UNDERFLW",

		"IRQ_ADO1_STREAM_UNLOCK" ,
		"IRQ_ADO1_STREAM_LOCK"	 ,
		"IRQ_ADO1_REPEAT"				 ,
		"IRQ_ADO1_SLIP"				   ,
		"IRQ_ARXDO1_SYT_OVERFLOW",
		"IRQ_ADO1_SYT_AGEOUT"		 ,
		"IRQ_ARX1_LONG_PKT"			 ,
		"IRQ_ARX1_PKT_ABORT"		 ,
		"IRQ_ARX1_STATUS_ERR"		 ,
		"IRQ_ADO1_NOT_COMPL"     ,
		"IRQ_ADO1_MISSED_SYNC"   ,

		"IRQ_ARX1_CFG_FAIL"			 ,
		"IRQ_ARX1_CIP_FAIL"			 ,
		"IRQ_ARX1_DBC_FAIL"			 ,
		"IRQ_ARXDO1_SYT_UNDERFLOW"	 
	},
	// AVS INT1 bits
	{
		"IRQ_ATX2_STREAM_END"			  ,
		"IRQ_ATX2_STREAM_START"		  ,
		"IRQ_ATXDI2_SYT_OVERFLOW"		,
		"IRQ_ATXDI2_SYT_UNDERFLOW"	,
		"IRQ_ATX2_BOUNDRY_ERR"		  ,
		"IRQ_ATX2_FRAME_AGEOUT"		  ,
		"IRQ_ATX2_PKT_ABORT"			  ,

		"IRQ_ATX1_STREAM_END"			  ,
		"IRQ_ATX1_STREAM_START"		  ,
		"IRQ_ATXDI1_SYT_OVERFLOW"		,
		"IRQ_ATXDI1_SYT_UNDERFLOW"	,
		"IRQ_ATX1_BOUNDRY_ERR"		  ,
		"IRQ_ATX1_FRAME_AGEOUT"		  ,
		"IRQ_ATX1_PKT_ABORT"			  
	}
};
#endif //_SYSDEBUG


BOOL TCInterruptPollAVS0(void)
{
	return TRUE;
}
BOOL TCInterruptPollAVS1(void)
{
	//zluo, for dice jr fpga
	return TRUE;
}

HRESULT avsIntEventHandleEvent(uint32 eventReg, uint32 index)
{
	HRESULT	hResult = NO_ERROR;
	uint32	bit;

	bit = avsIntEventState[eventReg].avsIntEvents[index];
	
	switch (eventReg)
	{
		case 0: //AVS INT 0 bits
			switch (bit)
			{
			}
			break;
		case 1: //AVS INT 1 bits
			switch (bit)
			{
      }
			break;
		case 2: //AVS INT 2 bits
			switch (bit)
			{
			}
			break;
	}

	return hResult;
}


void avsIntEventHandler0(void)
{
	uint32 intEventReg;

	// Service INT0 interrupts
	avsRegRead(APBA_INT0_STATUS, &intEventReg);

	// Clear all pending 0 interrupts to be serviced
	avsRegWrite(APBA_INT0_STATUS, intEventReg);

	if (intEventReg != 0)
	{
		avsIntEventState[0].avsIntEventCopy |= intEventReg;
		avsIntSignalSemaphore();
	}
}

void avsIntEventHandler1(void)
{
	uint32 intEventReg;

	// Service INT1 interrupts
	avsRegRead(APBA_INT1_STATUS, &intEventReg);

	// Clear all pending 1 interrupts to be serviced
	avsRegWrite(APBA_INT1_STATUS, intEventReg);

	if (intEventReg != 0)
	{
		avsIntEventState[1].avsIntEventCopy |= intEventReg;
		avsIntSignalSemaphore();
	}
}



#ifdef _AVSINT_POLLING
void avsIntPollHandlers(void)
{
	if (TCInterruptPollAVS0())
	{
		avsIntEventHandler0();
	}
	if (TCInterruptPollAVS1())
	{
		avsIntEventHandler1();
	}
	TCTaskWait(10);
}

#else //_AVSINT_POLLING
HRESULT avsIntInstallHandlers(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = TCInterruptInstallHandler (CYGNUM_HAL_INTERRUPT_AVS0, NULL, &avsIntEventHandler0);
	if (hResult != NO_ERROR) return hResult;
	hResult = TCInterruptInstallHandler (CYGNUM_HAL_INTERRUPT_AVS1, NULL, &avsIntEventHandler1);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}
#endif //_AVSINT_POLLING
