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

#include <cyg/hal/hal_intr.h>

#ifdef _DTCP
#include "avsDTCP.h"
#endif //_DTCP

#include "avsEnvDefs.h"
#include "avsInt.h"

uint32 avsIntCounter[NUM_AVS_EVENTS][AVS_INT_EVENT_ITEMS]; 

#ifdef _DICE2_ASIC

#define	AVS_INT0_EVENT_ITEMS	32		// list of prioritised active interrupt 1
#define	AVS_INT1_EVENT_ITEMS	29		// list of prioritised active interrupt 2
#define	AVS_INT2_EVENT_ITEMS	31		// list of prioritised active interrupt 3

uint32 avsIntMask[NUM_AVS_EVENTS] =
{
	APBA_INT0_MASK,
	APBA_INT1_MASK,
	APBA_INT2_MASK
};

uint32 avsIntItems[NUM_AVS_EVENTS] =
{
	AVS_INT0_EVENT_ITEMS,
	AVS_INT1_EVENT_ITEMS,
	AVS_INT2_EVENT_ITEMS
}; 

uint32 avsIntEvents[NUM_AVS_EVENTS][AVS_INT_EVENT_ITEMS] = 
{
	// AVS INT0 bits
	{
		IRQ_VRX_CFG_FAIL,
		IRQ_VRX_CIP_FAIL,
		IRQ_VRX_DBC_FAIL,
		IRQ_VRX_LONG_PKT,
		IRQ_VRX_PKT_ABORT,
		IRQ_VRX_STATUS_ERR,
		IRQ_VTX_BOUNDRY_ERR,
		IRQ_VTX_FRAME_AGEOUT,
		IRQ_VTX_PKT_ABORT,
		IRQ_CIPHER0_KEY_REQ,
		IRQ_CIPHER1_KEY_REQ,
		IRQ_CIPHER2_KEY_REQ,
		IRQ_ADO1_LOCKED,
		IRQ_ADO1_STREAM_END,
		IRQ_ADO1_STREAM_START,
		IRQ_ADO2_LOCKED,
		IRQ_ADO2_STREAM_END,
		IRQ_ADO2_STREAM_START,
		IRQ_ADO3_LOCKED,
		IRQ_ADO3_STREAM_END,
		IRQ_ADO3_STREAM_START,
		IRQ_ADO4_LOCKED,
		IRQ_ADO4_STREAM_END,
		IRQ_ADO4_STREAM_START,
		IRQ_ATX1_STREAM_END,
		IRQ_ATX1_STREAM_START,
		IRQ_ATX2_STREAM_END,
		IRQ_ATX2_STREAM_START,
		IRQ_VDO_STREAM_END,
		IRQ_VDO_STREAM_START,
		IRQ_VTX_STREAM_END,
		IRQ_VTX_STREAM_START
	},
	// AVS INT1 bits
	{
		IRQ_ARXDO1_SYT_OVERFLOW,
		IRQ_ARXDO1_SYT_UNDERFLOW,
		IRQ_ARXDO2_SYT_OVERFLOW,
		IRQ_ARXDO2_SYT_UNDERFLOW,
		IRQ_ARXDO3_SYT_OVERFLOW,
		IRQ_ARXDO3_SYT_UNDERFLOW,
		IRQ_ARXDO4_SYT_OVERFLOW,
		IRQ_ARXDO4_SYT_UNDERFLOW,
		IRQ_ATXDI1_SYT_OVERFLOW,
		IRQ_ATXDI1_SYT_UNDERFLOW,
		IRQ_ATXDI2_SYT_OVERFLOW,
		IRQ_ATXDI2_SYT_UNDERFLOW,
		IRQ_VRXDO_SYT_OVERFLOW,
		IRQ_VRXDO_SYT_UNDERFLOW,
		IRQ_VTXDI_SYT_OVERFLOW,
		IRQ_VTXDI_SYT_UNDERFLOW,
		IRQ_ADO1_REPEAT,
		IRQ_ADO1_SLIP,
		IRQ_ADO1_SYT_AGEOUT,
		IRQ_ADO2_REPEAT,
		IRQ_ADO2_SLIP,
		IRQ_ADO2_SYT_AGEOUT,
		IRQ_ADO3_REPEAT,
		IRQ_ADO3_SLIP,
		IRQ_ADO3_SYT_AGEOUT,
		IRQ_ADO4_REPEAT,
		IRQ_ADO4_SLIP,
		IRQ_ADO4_SYT_AGEOUT,
		IRQ_VDO_AGEOUT
	},
	// AVS INT2 bits
	{
		IRQ_ITP_EP_TOO_BIG,
		IRQ_ARX1_CFG_FAIL,
		IRQ_ARX1_CIP_FAIL,
		IRQ_ARX1_DBC_FAIL,
		IRQ_ARX1_LONG_PKT,
		IRQ_ARX1_PKT_ABORT,
		IRQ_ARX1_STATUS_ERR,
		IRQ_ARX2_CFG_FAIL,
		IRQ_ARX2_CIP_FAIL,
		IRQ_ARX2_DBC_FAIL,
		IRQ_ARX2_LONG_PKT,
		IRQ_ARX2_PKT_ABORT,
		IRQ_ARX2_STATUS_ERR,
		IRQ_ARX3_CFG_FAIL,
		IRQ_ARX3_CIP_FAIL,
		IRQ_ARX3_DBC_FAIL,
		IRQ_ARX3_LONG_PKT,
		IRQ_ARX3_PKT_ABORT,
		IRQ_ARX3_STATUS_ERR,
		IRQ_ARX4_CFG_FAIL,
		IRQ_ARX4_CIP_FAIL,
		IRQ_ARX4_DBC_FAIL,
		IRQ_ARX4_LONG_PKT,
		IRQ_ARX4_PKT_ABORT,
		IRQ_ARX4_STATUS_ERR,
		IRQ_ATX1_BOUNDRY_ERR,
		IRQ_ATX1_FRAME_AGEOUT,
		IRQ_ATX1_PKT_ABORT,
		IRQ_ATX2_BOUNDRY_ERR,
		IRQ_ATX2_FRAME_AGEOUT,
		IRQ_ATX2_PKT_ABORT
	},
};

#ifdef _SYSDEBUG
char avsIntTexts[NUM_AVS_EVENTS][AVS_INT_EVENT_ITEMS][AVS_EVENT_TEXT_SIZE] = 
{
	// AVS INT0 bits
	{
		"IRQ_VRX_CFG_FAIL",
		"IRQ_VRX_CIP_FAIL",
		"IRQ_VRX_DBC_FAIL",
		"IRQ_VRX_LONG_PKT",
		"IRQ_VRX_PKT_ABORT",
		"IRQ_VRX_STATUS_ERR",
		"IRQ_VTX_BOUNDRY_ERR",
		"IRQ_VTX_FRAME_AGEOUT",
		"IRQ_VTX_PKT_ABORT",
		"IRQ_CIPHER0_KEY_REQ",
		"IRQ_CIPHER1_KEY_REQ",
		"IRQ_CIPHER2_KEY_REQ",
		"IRQ_ADO1_LOCKED",
		"IRQ_ADO1_STREAM_END",
		"IRQ_ADO1_STREAM_START",
		"IRQ_ADO2_LOCKED",
		"IRQ_ADO2_STREAM_END",
		"IRQ_ADO2_STREAM_START",
		"IRQ_ADO3_LOCKED",
		"IRQ_ADO3_STREAM_END",
		"IRQ_ADO3_STREAM_START",
		"IRQ_ADO4_LOCKED",
		"IRQ_ADO4_STREAM_END",
		"IRQ_ADO4_STREAM_START",
		"IRQ_ATX1_STREAM_END",
		"IRQ_ATX1_STREAM_START",
		"IRQ_ATX2_STREAM_END",
		"IRQ_ATX2_STREAM_START",
		"IRQ_VDO_STREAM_END",
		"IRQ_VDO_STREAM_START",
		"IRQ_VTX_STREAM_END",
		"IRQ_VTX_STREAM_START"
	},
	// AVS INT1 bits
	{
		"IRQ_ARXDO1_SYT_OVERFLOW",
		"IRQ_ARXDO1_SYT_UNDERFLOW",
		"IRQ_ARXDO2_SYT_OVERFLOW",
		"IRQ_ARXDO2_SYT_UNDERFLOW",
		"IRQ_ARXDO3_SYT_OVERFLOW",
		"IRQ_ARXDO3_SYT_UNDERFLOW",
		"IRQ_ARXDO4_SYT_OVERFLOW",
		"IRQ_ARXDO4_SYT_UNDERFLOW",
		"IRQ_ATXDI1_SYT_OVERFLOW",
		"IRQ_ATXDI1_SYT_UNDERFLOW",
		"IRQ_ATXDI2_SYT_OVERFLOW",
		"IRQ_ATXDI2_SYT_UNDERFLOW",
		"IRQ_VRXDO_SYT_OVERFLOW",
		"IRQ_VRXDO_SYT_UNDERFLOW",
		"IRQ_VTXDI_SYT_OVERFLOW",
		"IRQ_VTXDI_SYT_UNDERFLOW",
		"IRQ_ADO1_REPEAT",
		"IRQ_ADO1_SLIP",
		"IRQ_ADO1_SYT_AGEOUT",
		"IRQ_ADO2_REPEAT",
		"IRQ_ADO2_SLIP",
		"IRQ_ADO2_SYT_AGEOUT",
		"IRQ_ADO3_REPEAT",
		"IRQ_ADO3_SLIP",
		"IRQ_ADO3_SYT_AGEOUT",
		"IRQ_ADO4_REPEAT",
		"IRQ_ADO4_SLIP",
		"IRQ_ADO4_SYT_AGEOUT",
		"IRQ_VDO_AGEOUT"
	},
	// AVS INT2 bits
	{
		"IRQ_ITP_EP_TOO_BIG",
		"IRQ_ARX1_CFG_FAIL",
		"IRQ_ARX1_CIP_FAIL",
		"IRQ_ARX1_DBC_FAIL",
		"IRQ_ARX1_LONG_PKT",
		"IRQ_ARX1_PKT_ABORT",
		"IRQ_ARX1_STATUS_ERR",
		"IRQ_ARX2_CFG_FAIL",
		"IRQ_ARX2_CIP_FAIL",
		"IRQ_ARX2_DBC_FAIL",
		"IRQ_ARX2_LONG_PKT",
		"IRQ_ARX2_PKT_ABORT",
		"IRQ_ARX2_STATUS_ERR",
		"IRQ_ARX3_CFG_FAIL",
		"IRQ_ARX3_CIP_FAIL",
		"IRQ_ARX3_DBC_FAIL",
		"IRQ_ARX3_LONG_PKT",
		"IRQ_ARX3_PKT_ABORT",
		"IRQ_ARX3_STATUS_ERR",
		"IRQ_ARX4_CFG_FAIL",
		"IRQ_ARX4_CIP_FAIL",
		"IRQ_ARX4_DBC_FAIL",
		"IRQ_ARX4_LONG_PKT",
		"IRQ_ARX4_PKT_ABORT",
		"IRQ_ARX4_STATUS_ERR",
		"IRQ_ATX1_BOUNDRY_ERR",
		"IRQ_ATX1_FRAME_AGEOUT",
		"IRQ_ATX1_PKT_ABORT",
		"IRQ_ATX2_BOUNDRY_ERR",
		"IRQ_ATX2_FRAME_AGEOUT",
		"IRQ_ATX2_PKT_ABORT"
	},
};
#endif //_SYSDEBUG

#elif _DICE2_FPGA

#ifdef _ECPMINT_POLLING
#define	AVS_INT0_EVENT_ITEMS		25			// list of prioritised active interrupts
#else //_ECPMINT_POLLING
#define	AVS_INT0_EVENT_ITEMS		26			// list of prioritised active interrupts
#define	AVS_INT0_EVENT_ECPM_INDEX	25			// ECPM entry in list of prioritised active interrupts
#endif //_ECPMINT_POLLING

uint32 avsIntMask[NUM_AVS_EVENTS] =
{
	APBA_INT0_MASK
};

uint32 avsIntItems[NUM_AVS_EVENTS] =
{
	AVS_INT0_EVENT_ITEMS
}; 

uint32 avsIntEvents[NUM_AVS_EVENTS][AVS_INT_EVENT_ITEMS] = 
{
	// AVS INT0 bits
	{
		IRQ_ARX1_CFG_FAIL,
		IRQ_ARX1_CIP_FAIL,
		IRQ_ARX1_DBC_FAIL,
		IRQ_ARX1_LONG_PKT,
		IRQ_ARX1_PKT_ABORT,
		IRQ_ARX1_STATUS_ERR,
		IRQ_ADO1_LOCKED,
		IRQ_ADO1_REPEAT,
		IRQ_ADO1_SLIP,
		IRQ_ADO1_STREAM_END,
		IRQ_ADO1_STREAM_START,
		IRQ_ADO1_SYT_AGEOUT,
		IRQ_ARXDO1_SYT_OVERFLOW,
		IRQ_ARXDO1_SYT_UNDERFLOW,
		IRQ_ATX1_BOUNDRY_ERR,
		IRQ_ATX1_FRAME_AGEOUT,
		IRQ_ATX1_PKT_ABORT,
		IRQ_ATX1_STREAM_END,
		IRQ_ATX1_STREAM_START,
		IRQ_ATX1_SYT_OVERFLOW,
		IRQ_ATX1_SYT_UNDERFLOW,
		IRQ_CIPHER0_KEY_REQ,
		IRQ_CIPHER1_KEY_REQ,
		IRQ_CIPHER2_KEY_REQ,
		IRQ_ITP_EP_TOO_BIG
#ifdef _ECPMINT_POLLING
#else //_ECPMINT_POLLING
		,IRQ_ECPM
#endif //_ECPMINT_POLLING
	}
};

#ifdef _SYSDEBUG
char avsIntTexts[NUM_AVS_EVENTS][AVS_INT_EVENT_ITEMS][AVS_EVENT_TEXT_SIZE] = 
{
	// AVS INT0 bits
	{
		"AVS_IRQ_ARX1_CFG_FAIL",
		"AVS_IRQ_ARX1_CIP_FAIL",
		"AVS_IRQ_ARX1_DBC_FAIL",
		"AVS_IRQ_ARX1_LONG_PKT",
		"AVS_IRQ_ARX1_PKT_ABORT",
		"AVS_IRQ_ARX1_STATUS_ERR",
		"AVS_IRQ_ADO1_LOCKED",
		"AVS_IRQ_ADO1_REPEAT",
		"AVS_IRQ_ADO1_SLIP",
		"AVS_IRQ_ADO1_STREAM_END",
		"AVS_IRQ_ADO1_STREAM_START",
		"AVS_IRQ_ADO1_SYT_AGEOUT",
		"AVS_IRQ_ARXDO1_SYT_OVERFLOW",
		"AVS_IRQ_ARXDO1_SYT_UNDERFLOW",
		"AVS_IRQ_ATX1_BOUNDRY_ERR",
		"AVS_IRQ_ATX1_FRAME_AGEOUT",
		"AVS_IRQ_ATX1_PKT_ABORT",
		"AVS_IRQ_ATX1_STREAM_END",
		"AVS_IRQ_ATX1_STREAM_START",
		"AVS_IRQ_ATX1_SYT_OVERFLOW",
		"AVS_IRQ_ATX1_SYT_UNDERFLOW",
		"AVS_IRQ_CIPHER0_KEY_REQ",
		"AVS_IRQ_CIPHER1_KEY_REQ",
		"AVS_IRQ_CIPHER2_KEY_REQ",
		"AVS_IRQ_ITP_EP_TOO_BIG"
#ifdef _ECPMINT_POLLING
#else //_ECPMINT_POLLING
		,"AVS_IRQ_ECPM",
#endif //_ECPMINT_POLLING
	}
};
#endif //_SYSDEBUG
#elif _DICE2_WIN
//nothing 
#else
#error "missing case"
#endif


#ifdef _DICE2_ASIC
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
#ifdef _DTCP																				
				//LM??? should be added to avs FSM instead
				case IRQ_ADO1_STREAM_START:	avsDTCPHandleRxCheckDecryptionNeeded(AVS_PLUG_ID1);		break;
				case IRQ_ADO2_STREAM_START:	avsDTCPHandleRxCheckDecryptionNeeded(AVS_PLUG_ID2);		break;
				case IRQ_ADO3_STREAM_START:	avsDTCPHandleRxCheckDecryptionNeeded(AVS_PLUG_ID3);		break;
				case IRQ_ADO4_STREAM_START:	avsDTCPHandleRxCheckDecryptionNeeded(AVS_PLUG_ID4);		break;
#endif //_DTCP

#ifdef _DTCP																				
				case IRQ_CIPHER0_KEY_REQ:	avsDTCPHandleRxCipherNewContentKeyRequest(0);			break;	// New Content Key Request for cipher 0
				case IRQ_CIPHER1_KEY_REQ:	avsDTCPHandleRxCipherNewContentKeyRequest(1);			break;	// New Content Key Request for cipher 1
				case IRQ_CIPHER2_KEY_REQ:	avsDTCPHandleRxCipherNewContentKeyRequest(2);			break;	// New Content Key Request for cipher 2
#endif //_DTCP
			}
			break;
		case 1: //AVS INT 1 bits
			switch (bit)
			{
				case IRQ_ADO1_SYT_AGEOUT:   avsHandleRxSytAgeOut (AVS_PLUG_ID1);					break;
				case IRQ_ADO2_SYT_AGEOUT:   avsHandleRxSytAgeOut (AVS_PLUG_ID2);					break;
				case IRQ_ADO3_SYT_AGEOUT:   avsHandleRxSytAgeOut (AVS_PLUG_ID3);					break;
				case IRQ_ADO4_SYT_AGEOUT:   avsHandleRxSytAgeOut (AVS_PLUG_ID4);					break;
				case IRQ_ADO1_REPEAT:		avsHandleRxRepeat (AVS_PLUG_ID1);						break;
				case IRQ_ADO2_REPEAT:		avsHandleRxRepeat (AVS_PLUG_ID2);						break;
				case IRQ_ADO3_REPEAT:		avsHandleRxRepeat (AVS_PLUG_ID3);						break;
				case IRQ_ADO4_REPEAT:		avsHandleRxRepeat (AVS_PLUG_ID4);						break;
				case IRQ_ADO1_SLIP:			avsHandleRxSlip   (AVS_PLUG_ID1);						break;
				case IRQ_ADO2_SLIP:			avsHandleRxSlip   (AVS_PLUG_ID2);						break;
				case IRQ_ADO3_SLIP:			avsHandleRxSlip   (AVS_PLUG_ID3);						break;
				case IRQ_ADO4_SLIP:			avsHandleRxSlip   (AVS_PLUG_ID4);						break;
			}
			break;
		case 2: //AVS INT 2 bits
			switch (bit)
			{
#ifdef _DTCP
				case IRQ_ARX1_CFG_FAIL:		avsDTCPHandleRxCheckDecryptionChanged(AVS_PLUG_ID1);	break;	// Source Id or EMI bits have changed for plug ID 1
				case IRQ_ARX2_CFG_FAIL:		avsDTCPHandleRxCheckDecryptionChanged(AVS_PLUG_ID2);	break;	// Source Id or EMI bits have changed for plug ID 2
				case IRQ_ARX3_CFG_FAIL:		avsDTCPHandleRxCheckDecryptionChanged(AVS_PLUG_ID3);	break;	// Source Id or EMI bits have changed for plug ID 3
				case IRQ_ARX4_CFG_FAIL:		avsDTCPHandleRxCheckDecryptionChanged(AVS_PLUG_ID4);	break;	// Source Id or EMI bits have changed for plug ID 4
#endif //_DTCP
				case IRQ_ARX1_DBC_FAIL:     avsHandleRxDbcFail   (AVS_PLUG_ID1);					break;
				case IRQ_ARX2_DBC_FAIL:     avsHandleRxDbcFail   (AVS_PLUG_ID2);					break;
				case IRQ_ARX3_DBC_FAIL:     avsHandleRxDbcFail   (AVS_PLUG_ID3);					break;
				case IRQ_ARX4_DBC_FAIL:     avsHandleRxDbcFail   (AVS_PLUG_ID4);					break;
																									
				case IRQ_ARX1_PKT_ABORT:    avsHandleRxPktAbort  (AVS_PLUG_ID1);					break;
				case IRQ_ARX2_PKT_ABORT:    avsHandleRxPktAbort  (AVS_PLUG_ID2);					break;
				case IRQ_ARX3_PKT_ABORT:    avsHandleRxPktAbort  (AVS_PLUG_ID3);					break;
				case IRQ_ARX4_PKT_ABORT:    avsHandleRxPktAbort  (AVS_PLUG_ID4);					break;
			}
			break;
	}

	return hResult;
}
#elif defined _DICE2_FPGA
HRESULT avsIntEventHandleEvent(uint32 eventReg, uint32 index)
{
	HRESULT	hResult = NO_ERROR;
	uint32	bit;

	if (eventReg != 0)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	bit = avsIntEventState[eventReg].avsIntEvents[index];
	
	//AVS INT 0 bits
	switch (bit)
	{
#ifdef _DTCP																			
		case IRQ_ADO1_STREAM_START:	avsDTCPHandleRxCheckDecryptionNeeded(AVS_PLUG_ID1);				break;
		case IRQ_ARX1_CFG_FAIL:		avsDTCPHandleRxCheckDecryptionChanged(AVS_PLUG_ID1);			break;	// Source Id has changed or EMI bits have changed for plug ID 1

		case IRQ_CIPHER0_KEY_REQ:	avsDTCPHandleRxCipherNewContentKeyRequest(0);					break;	// New Content Key Request for cipher 0
		case IRQ_CIPHER1_KEY_REQ:	avsDTCPHandleRxCipherNewContentKeyRequest(1);					break;	// New Content Key Request for cipher 1
		case IRQ_CIPHER2_KEY_REQ:	avsDTCPHandleRxCipherNewContentKeyRequest(2);					break;	// New Content Key Request for cipher 2
																									
#endif //_DTCP

		case IRQ_ADO1_SYT_AGEOUT:   avsHandleRxSytAgeOut (AVS_PLUG_ID1);							break;
		case IRQ_ARX1_DBC_FAIL:     avsHandleRxDbcFail   (AVS_PLUG_ID1);							break;
		case IRQ_ARX1_PKT_ABORT:    avsHandleRxPktAbort  (AVS_PLUG_ID1);							break;
	}

	return hResult;
}
#elif defined _DICE2_WIN
//nothing
#else
#error "missing case"
#endif


#ifdef _DICE2_ASIC
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

void avsIntEventHandler2(void)
{
	uint32 intEventReg;

	// Service INT2 interrupts
	avsRegRead(APBA_INT2_STATUS, &intEventReg);

	// Clear all pending 2 interrupts to be serviced
	avsRegWrite(APBA_INT2_STATUS, intEventReg);

	if (intEventReg != 0)
	{
		avsIntEventState[2].avsIntEventCopy |= intEventReg;
		avsIntSignalSemaphore();
	}
}

#elif defined _DICE2_FPGA
#ifdef _DTCP
#ifdef _ECPMINT_POLLING
#include "dtcpECCDice2Defs.h"
#endif //_ECPMINT_POLLING
#endif //_DTCP

void avsIntEventHandler0(void)
{
	uint32	intEventReg;

	UNUSED_ARG(pVal);

	// Service INT0 interrupts
	avsRegRead(APBA_INT0_STATUS, &intEventReg);

	// Clear all pending 0 interrupts to be serviced
	avsRegWrite(APBA_INT0_STATUS, intEventReg);

#ifdef _DTCP
#ifdef _ECPMINT_POLLING
#else //_ECPMINT_POLLING
	if (intEventReg != 0)
	{
		if (intEventReg & IRQ_ECPM)
		{
			// Increment event counter
			avsIntEventState[0].avsIntCounter[AVS_INT0_EVENT_ECPM_INDEX]++;
			// Clear AVS event
			intEventReg &= ~IRQ_ECPM;

			dtcpECPMIntPart1();		//clear start bit, get results

			// Clear ECPM pending 0 interrupts to be serviced
			avsRegWrite(APBA_INT0_STATUS, IRQ_ECPM);

			dtcpECPMIntPart2();		//signal thread semaphore

		}
	}
#endif //_ECPMINT_POLLING
#endif //_DTCP

	if (intEventReg != 0)
	{
		avsIntEventState[0].avsIntEventCopy |= intEventReg;
		avsIntSignalSemaphore();
	}
}
#elif defined _DICE2_WIN
//nothing
#else
#error "missing case"
#endif

#ifdef _DICE2_ASIC
#ifdef _AVSINT_POLLING

BOOL TCInterruptPollAVS0(void)
{
#ifdef _DICE2_ASIC 
	return ((*((volatile uint32 *) ICTL_RAWSTAT)) & (1 << CYGNUM_HAL_INTERRUPT_AVS0));
#elif defined _DICE2_FPGA
	return ((*((volatile uint32 *) 0xffb00028)) & 0x04) == 0x04);
#endif
	return FALSE;
}
BOOL TCInterruptPollAVS1(void)
{
#ifdef _DICE2_ASIC 
	return ((*((volatile uint32 *) ICTL_RAWSTAT)) & (1 << CYGNUM_HAL_INTERRUPT_AVS1));
#elif defined _DICE2_FPGA
	return FALSE;
#endif
}
BOOL TCInterruptPollAVS2(void)
{
#ifdef _DICE2_ASIC 
	return ((*((volatile uint32 *) ICTL_RAWSTAT)) & (1 << CYGNUM_HAL_INTERRUPT_AVS2));
#elif defined _DICE2_FPGA
	return FALSE;
#endif
}

void avsIntPollHandlers(void)
{
#if 1 //LM???
	if (TCInterruptPollAVS0())
	{
		avsIntEventHandler0();
	}
	if (TCInterruptPollAVS1())
	{
		avsIntEventHandler1();
	}
	if (TCInterruptPollAVS2())
	{
		avsIntEventHandler2();
	}
	TCTaskWait(10);
#else
	TCTaskWait(10000);
#endif
}
#else //_AVSINT_POLLING
HRESULT avsIntInstallHandlers(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = TCInterruptInstallHandler (CYGNUM_HAL_INTERRUPT_AVS0, NULL, &avsIntEventHandler0);
	if (hResult != NO_ERROR) return hResult;
	hResult = TCInterruptInstallHandler (CYGNUM_HAL_INTERRUPT_AVS1, NULL, &avsIntEventHandler1);
	if (hResult != NO_ERROR) return hResult;
	hResult = TCInterruptInstallHandler (CYGNUM_HAL_INTERRUPT_AVS2, NULL, &avsIntEventHandler2);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}
#endif //_AVSINT_POLLING
#elif defined _DICE2_FPGA
#ifdef _AVSINT_POLLING
void avsIntPollHandlers(void)
{
	if (TCInterruptPollAVS0())
	{
		avsIntEventHandler0();
	}
}
#else //_AVSINT_POLLING
HRESULT avsIntInstallHandlers(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = TCInterruptCreateAVS0(&avsIntEventHandler0);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}
#endif //_AVSINT_POLLING
#elif defined _DICE2_WIN
#ifdef _AVSINT_POLLING
void avsIntPollHandlers(void)
{
}
#else //_AVSINT_POLLING
HRESULT avsIntInstallHandlers(void)
{
	HRESULT		hResult = NO_ERROR;

	return hResult;
}
#endif //_AVSINT_POLLING
#else
#error "missing case"
#endif


#ifdef _DICE2_ASIC
//nothing
#elif defined _DICE2_FPGA
// Variable used in AVS register double dummy reads
uint32 avsEnvReadDummy;
#elif defined _DICE2_WIN
uint32 avsEnvCfgReg;
uint32 avsEnvReg[1024];
uint32 avsEnvLLCReg[256];
#else
#error "missing case"
#endif
