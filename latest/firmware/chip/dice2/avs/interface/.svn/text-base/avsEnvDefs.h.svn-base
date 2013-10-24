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

#ifdef _DICE2_ASIC
#include "coreDefs.h"
#elif defined _DICE2_FPGA			
#include "coreDefs.h"
#elif defined _DICE2_WIN
#define AVSBASE		0
#else
#error "missing case"
#endif

#ifdef _DICE2_ASIC
#define NUM_AVS_EVENTS			 3
#elif _DICE2_FPGA
#define NUM_AVS_EVENTS			 1
#elif _DICE2_WIN
#define NUM_AVS_EVENTS			 1
#else
#error "missing case"
#endif

#ifdef _DICE2_ASIC
// AVS INT0 bits
#define IRQ_VRX_CFG_FAIL			BIT31
#define IRQ_VRX_CIP_FAIL			BIT30
#define IRQ_VRX_DBC_FAIL 			BIT29
#define IRQ_VRX_LONG_PKT			BIT28
#define IRQ_VRX_PKT_ABORT			BIT27
#define IRQ_VRX_STATUS_ERR			BIT26
#define IRQ_VTX_BOUNDRY_ERR			BIT25
#define IRQ_VTX_FRAME_AGEOUT		BIT24
#define IRQ_VTX_PKT_ABORT			BIT23
#define IRQ_CIPHER0_KEY_REQ			BIT22
#define IRQ_CIPHER1_KEY_REQ			BIT21
#define IRQ_CIPHER2_KEY_REQ			BIT20
#define IRQ_ADO1_LOCKED				BIT19
#define IRQ_ADO1_STREAM_END			BIT18
#define IRQ_ADO1_STREAM_START		BIT17
#define IRQ_ADO2_LOCKED				BIT16
#define IRQ_ADO2_STREAM_END			BIT15
#define IRQ_ADO2_STREAM_START		BIT14
#define IRQ_ADO3_LOCKED				BIT13
#define IRQ_ADO3_STREAM_END			BIT12
#define IRQ_ADO3_STREAM_START		BIT11
#define IRQ_ADO4_LOCKED				BIT10
#define IRQ_ADO4_STREAM_END			BIT9
#define IRQ_ADO4_STREAM_START		BIT8
#define IRQ_ATX1_STREAM_END			BIT7
#define IRQ_ATX1_STREAM_START		BIT6
#define IRQ_ATX2_STREAM_END			BIT5
#define IRQ_ATX2_STREAM_START		BIT4
#define IRQ_VDO_STREAM_END			BIT3
#define IRQ_VDO_STREAM_START		BIT2
#define IRQ_VTX_STREAM_END			BIT1
#define IRQ_VTX_STREAM_START		BIT0

// AVS INT1 bits
#define IRQ_ARXDO1_SYT_OVERFLOW		BIT31
#define IRQ_ARXDO1_SYT_UNDERFLOW	BIT30
#define IRQ_ARXDO2_SYT_OVERFLOW		BIT29
#define IRQ_ARXDO2_SYT_UNDERFLOW	BIT28
#define IRQ_ARXDO3_SYT_OVERFLOW		BIT27
#define IRQ_ARXDO3_SYT_UNDERFLOW	BIT26
#define IRQ_ARXDO4_SYT_OVERFLOW		BIT25
#define IRQ_ARXDO4_SYT_UNDERFLOW	BIT24
#define IRQ_ATXDI1_SYT_OVERFLOW		BIT23
#define IRQ_ATXDI1_SYT_UNDERFLOW	BIT22
#define IRQ_ATXDI2_SYT_OVERFLOW		BIT21
#define IRQ_ATXDI2_SYT_UNDERFLOW	BIT20
#define IRQ_VRXDO_SYT_OVERFLOW		BIT19
#define IRQ_VRXDO_SYT_UNDERFLOW		BIT18
#define IRQ_VTXDI_SYT_OVERFLOW		BIT17
#define IRQ_VTXDI_SYT_UNDERFLOW		BIT16
#define IRQ_ADO1_REPEAT				BIT15
#define IRQ_ADO1_SLIP				BIT14
#define IRQ_ADO1_SYT_AGEOUT			BIT13
#define IRQ_ADO2_REPEAT				BIT12
#define IRQ_ADO2_SLIP				BIT11
#define IRQ_ADO2_SYT_AGEOUT			BIT10
#define IRQ_ADO3_REPEAT				BIT9
#define IRQ_ADO3_SLIP				BIT8
#define IRQ_ADO3_SYT_AGEOUT			BIT7
#define IRQ_ADO4_REPEAT				BIT6
#define IRQ_ADO4_SLIP				BIT5
#define IRQ_ADO4_SYT_AGEOUT			BIT4
#define IRQ_VDO_AGEOUT				BIT3
#define IRQ_UNUSED1					BIT2
#define IRQ_UNUSED2					BIT1
#define IRQ_UNUSED3					BIT0

// AVS INT2 bits
#define IRQ_ITP_EP_TOO_BIG			BIT31
#define IRQ_ARX1_CFG_FAIL			BIT30
#define IRQ_ARX1_CIP_FAIL			BIT29
#define IRQ_ARX1_DBC_FAIL 			BIT28
#define IRQ_ARX1_LONG_PKT			BIT27
#define IRQ_ARX1_PKT_ABORT			BIT26
#define IRQ_ARX1_STATUS_ERR			BIT25
#define IRQ_ARX2_CFG_FAIL			BIT24
#define IRQ_ARX2_CIP_FAIL			BIT23
#define IRQ_ARX2_DBC_FAIL 			BIT22
#define IRQ_ARX2_LONG_PKT			BIT21
#define IRQ_ARX2_PKT_ABORT			BIT20
#define IRQ_ARX2_STATUS_ERR			BIT19
#define IRQ_ARX3_CFG_FAIL			BIT18
#define IRQ_ARX3_CIP_FAIL			BIT17
#define IRQ_ARX3_DBC_FAIL 			BIT16
#define IRQ_ARX3_LONG_PKT			BIT15
#define IRQ_ARX3_PKT_ABORT			BIT14
#define IRQ_ARX3_STATUS_ERR			BIT13
#define IRQ_ARX4_CFG_FAIL			BIT12
#define IRQ_ARX4_CIP_FAIL			BIT11
#define IRQ_ARX4_DBC_FAIL 			BIT10
#define IRQ_ARX4_LONG_PKT			BIT9
#define IRQ_ARX4_PKT_ABORT			BIT8
#define IRQ_ARX4_STATUS_ERR			BIT7
#define IRQ_ATX1_BOUNDRY_ERR		BIT6
#define IRQ_ATX1_FRAME_AGEOUT		BIT5
#define IRQ_ATX1_PKT_ABORT			BIT4
#define IRQ_ATX2_BOUNDRY_ERR		BIT3
#define IRQ_ATX2_FRAME_AGEOUT		BIT2
#define IRQ_ATX2_PKT_ABORT			BIT1
#define IRQ_UNUSED4					BIT0

#elif _DICE2_FPGA

// AVS INT0 bits
#define IRQ_ARX1_CFG_FAIL			BIT31
#define IRQ_ARX1_CIP_FAIL			BIT30
#define IRQ_ARX1_DBC_FAIL 			BIT29
#define IRQ_ARX1_LONG_PKT			BIT28
#define IRQ_ARX1_PKT_ABORT			BIT27
#define IRQ_ARX1_STATUS_ERR			BIT26
#define IRQ_ADO1_LOCKED				BIT25
#define IRQ_ADO1_REPEAT				BIT24
#define IRQ_ADO1_SLIP				BIT23
#define IRQ_ADO1_STREAM_END			BIT22
#define IRQ_ADO1_STREAM_START		BIT21
#define IRQ_ADO1_SYT_AGEOUT			BIT20
#define IRQ_ARXDO1_SYT_OVERFLOW		BIT19
#define IRQ_ARXDO1_SYT_UNDERFLOW	BIT18
#define IRQ_ATX1_BOUNDRY_ERR		BIT17
#define IRQ_ATX1_FRAME_AGEOUT		BIT16
#define IRQ_ATX1_PKT_ABORT			BIT15
#define IRQ_ATX1_STREAM_END			BIT14
#define IRQ_ATX1_STREAM_START		BIT13
#define IRQ_ATX1_SYT_OVERFLOW		BIT12
#define IRQ_ATX1_SYT_UNDERFLOW		BIT11
#define IRQ_CIPHER0_KEY_REQ			BIT10
#define IRQ_CIPHER1_KEY_REQ			BIT9
#define IRQ_CIPHER2_KEY_REQ			BIT8
#define IRQ_ITP_EP_TOO_BIG			BIT7
#define IRQ_ECPM					BIT6
#elif _DICE2_WIN
//nothing 
#else
#error "missing case"
#endif

#ifdef _DICE2_ASIC
#define NUM_AVS_RX					4		// audio only
#define NUM_AVS_TX					2		// audio only
#define NUM_PARTITIONS				6		// Number of partitions in Media FIFO
#define M6_PARTITIONS				3		// Number of partitions with M6 (first partitions)
#elif defined _DICE2_FPGA			
#define NUM_AVS_RX					1
#define NUM_AVS_TX					1
#define NUM_PARTITIONS				1		// Number of partitions in Media FIFO
#define M6_PARTITIONS				1		// Number of partitions with M6 (first partitions)
#elif defined _DICE2_WIN
#define NUM_AVS_RX					4		// audio only
#define NUM_AVS_TX					2		// audio only
#define NUM_PARTITIONS				6		// Number of partitions in Media FIFO
#define M6_PARTITIONS				3		// Number of partitions with M6 (first partitions)
#else
#error "missing case"
#endif

#define NO_PARTITION				0xff	// Flag to indicate partition not selected

// Macro to convert register to memory pointer
#define mAvsPtr(x)					((volatile int *)(AVSBASE+(x)))
#define mAvsCfgPtr					((volatile int *) PDBINTBASE)

#define AVS_INT_EVENT_ITEMS			32
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

#ifdef _DICE2_ASIC
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

#elif defined _DICE2_FPGA			
extern uint32 avsEnvReadDummy;

_TC_INLINE void avsEnvRegRead(uint32 registerOffset, uint32 *value)
// Read a value from the AVS
{
	avsEnvReadDummy = *mAvsPtr(registerOffset);
	*value = *mAvsPtr(registerOffset); // Read 2 times due to "hack" in AVS format FPGA
}

_TC_INLINE void avsEnvRegWrite(uint32 registerOffset, uint32 value)
// Read a value from the AVS
{
	*mAvsPtr(registerOffset) = value;
}

_TC_INLINE void avsEnvConfigRegWrite(uint32 value)
// Write a value to the AVS configuration register 
{
	*mAvsCfgPtr = value;
}
#elif defined _DICE2_WIN

#pragma warning( disable : 4514 )

extern uint32 avsEnvReg[1024];
extern uint32 avsEnvLLCReg[256];
extern uint32 avsEnvCfgReg;

_TC_INLINE void avsEnvRegRead(uint32 registerOffset, uint32 *value)
// Read a value from the AVS
{
	*value = avsEnvReg[registerOffset/4];
}

_TC_INLINE void avsEnvRegWrite(uint32 registerOffset, uint32 value)
// Write a value to the AVS
{
	avsEnvReg[registerOffset/4] = value;
}

_TC_INLINE void avsEnvConfigRegWrite(uint32 value)
{
	avsEnvCfgReg = value;
}

#define ISO_RX_ENB_REG_1_DP			0x24		// Isoch channel enable reg. 1
#define ISO_RX_ENB_REG_2_DP			0x28		// Isoch channel enable reg. 2
#define ISO_TX_ENB_REG_1_DP			0xb0		// Isoch. Channel Tx enable reg. 1
#define ISO_TX_ENB_REG_2_DP			0xb4		// Isoch. Channel Tx enable reg. 2

_TC_INLINE void llcLinkRegRead(uint32 registerOffset, uint32 *value)
// Read a value from the LLC
{
	*value = avsEnvLLCReg[registerOffset/4];
}

_TC_INLINE void llcLinkRegWrite(uint32 registerOffset, uint32 value)
// Write a value to the LLC
{
	avsEnvLLCReg[registerOffset/4] = value;
}

#else
#error "missing case"
#endif

#endif //_AVSENVDEFS_H
