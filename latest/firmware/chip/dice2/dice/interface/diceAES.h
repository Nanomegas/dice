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
	
	Purpose:	DICE - AES receiver and transmitter API
	
	Revisions:
		created 12/06/2003 kbj
		major overhaul 09/03/2005 ML

****************************************************************************/

#ifndef _DICEAES_H
#define _DICEAES_H

#include "diceAESReg.h"


// AES receiver and transmitter Id
#define AES_ID0		0
#define AES_ID1		1
#define AES_ID2		2
#define AES_ID3		3
#define AES_ANY		4



HRESULT aesInitialize(void);

HRESULT aesSetMasterClk(uint8 aesRxId);

HRESULT aesGetMasterClk(uint8 *aesRxId);

#define AES_STAT_LOCK0		0x0001
#define AES_STAT_LOCK1		0x0002
#define AES_STAT_LOCK2		0x0004
#define AES_STAT_LOCK3		0x0008
#define AES_STAT_VALIDITY0	0x0010
#define AES_STAT_VALIDITY1	0x0020
#define AES_STAT_VALIDITY2	0x0040
#define AES_STAT_VALIDITY3	0x0080
#define AES_STAT_MASTERLOCK	0x8000

HRESULT aesGetStatus (uint16 *aesStatus);

HRESULT aesGetRxError(uint8 aesRxId, uAESRX_ERROR *pRxError, BOOL clearCondition);

// Set AES transmitter mute for channel 0-7, if ALL_CH then mute/unmute all
HRESULT aesTxSetMute(uint8 aesTxCh, BOOL mute);

BOOL aesIsMasterLocked (void);


// Lock state machine
// The lock state machine monitors the AES PLL and makes sure that lock is
// obtained by pumping the PLL.
// This function should be called every 10ms.
#define   EVT_AES1_LOCK_ACQUIRED	0x00000001
#define   EVT_AES1_LOCK_LOST		0x00000002
#define   EVT_AES1_CRC				0x00000004
#define   EVT_AES1_PARITY			0x00000008
#define   EVT_AES1_VALIDITY			0x00000010
#define   EVT_AES1_SLIP				0x00000020
#define   EVT_AES1_RPT				0x00000040
#define   EVT_AES1_MASK				0x000000FF

void aesRxStateMachine (uint8 events[NUM_AES]);


HRESULT aesRxEnable (BOOL bEnable);

HRESULT aesRxGetEnable (BOOL * bEnable);

/////////////////////// LOW LEVEL FUNCTIONS ///////////////////////////////

// Set AES receiver PLL pulse width
HRESULT aesRxSetPLLPulse(uint8 pulseWidthUp, uint8 pulseWidthDown);

// Set AES receiver VCO min. frequence
HRESULT aesRxSetVCOFq(uint16 minFq);



/////////////////////// Receiver C and S functions ///////////////////////////////


// Select AES receive channel to collect channel status bytes 0-23
// Input aesRxCh: 0 - 7
HRESULT aesRxSetCSChannel(uint8 aesRxCh);
HRESULT aesRxGetIndividualChannelStatus (uint8 aesRxCh, uint8 chStat[NUM_AES_CS_BYTES]);
HRESULT aesRxGetSelectedChannelStatus (uint8 chStat[NUM_AES_SEL_CS_BYTES]);


/////////////////////// Transmitter C, S and Z functions ///////////////////////////////

// Set AES transmitter in master mode with intern or extern blocksync
HRESULT aesTxSetMasterMode(BOOL externBlocksync);

// Set AES transmitter in slave mode with blocksync from channel 0-7
HRESULT aesTxSetSlaveMode(uint8 aesTxCh);

// Set AES transmitter validity bit for channel 0-7, if ALL_CH then mute/unmute all
// Validity bit setting only used in master mode
HRESULT aesTxSetValidAudio(uint8 aesTxCh, BOOL valid);

// Enable or disable auto CRC for AES transmitters 0-3 (do not use if using high level CS)
HRESULT aesTxSetAutoCRC(BOOL enable);

// Set user bit selection from AESRX channel 
// if aesTxCh = ALL_CH then all Tx channels set to AESRX channel
HRESULT aesTxSetUserFromAESRXCh(uint8 aesTxCh, uint8 aesRxCh);

// Set user bit selection from AVS receiver, 
// if aesTxCh = ALL_CH then all Tx channels set to AVSRX plug
HRESULT aesTxSetUserFromAVSRX(uint8 aesTxCh, uint8 plugID);

// Set user bit to zero, if ALL_CH then all Tx channels set to zero 
HRESULT aesTxSetUserZero(uint8 aesTxCh);

// Higher level Channel Status functions. 
//   All functions with the prefix aesTxCS will assume that
//   Channel status is register based and not based on the meta data in the upper 8 bits of the
//   audio channel. Those functions will work both in slave and master mode, but in slave mode only
//   the block sync (Z- preamble) and possibly the U bit will be extracted from the audio stream.
// 
//   Be aware that the shared CS registers are used and channel status will be the same for all
//   8 channels.
//

// Pre-emphasis defines.
#define AES_CS_PEMP_NIND      0
#define AES_CS_PEMP_NONE      1
#define AES_CS_PEMP_50_15US   2
#define AES_CS_PEMP_J17       3

// Copy protect defines
#define AES_CS_CPY_INF        0
#define AES_CS_CPY_ONE        1
#define AES_CS_CPY_NO         2


// Sample rate definitions
#define AES_CS_RATE_32K       0
#define AES_CS_RATE_44K1      1
#define AES_CS_RATE_48K       2
#define AES_CS_RATE_88K2      3
#define AES_CS_RATE_96K       4
#define AES_CS_RATE_176K4     5
#define AES_CS_RATE_192K      6

// Consumer use category definitions (not complete)
#define AES_CS_CAT_GENERAL            0x00
#define AES_CS_CAT_OPTICAL_CD         0x01
#define AES_CS_CAT_OPTICAL_OTHER      0x09
#define AES_CS_CAT_OPTICAL_MINIDISC   0x49
#define AES_CS_CAT_OPTICAL_DVD        0x19
#define AES_CS_CAT_PROC_PCM           0x02
#define AES_CS_CAT_PROC_MIXER         0x18
#define AES_CS_CAT_PROC_SRC           0x1a
#define AES_CS_CAT_PROC_SMP           0x22
#define AES_CS_CAT_PROC_PROC          0x2a
#define AES_CS_CAT_TAPE_DAT           0x03
#define AES_CS_CAT_TAPE_VIDEO         0x0b
#define AES_CS_CAT_TAPE_DCC           0x43
#define AES_CS_CAT_MI_SYNTH           0x05
#define AES_CS_CAT_MI_MIC             0x0d
#define AES_CS_CAT_AD                 0x06
#define AES_CS_CAT_AD_CPY             0x16

//special cases have the copy right bit reversed
#define AES_CS_CAT_CPYREV_MASK        0x0f
#define AES_CS_CAT_CPYREV_EQ1         0x01
#define AES_CS_CAT_CPYREV_EQ2         0x09
#define AES_CS_CAT_CPYREV_EQ3         0x04
#define AES_CS_CAT_CPYREV_EQ4         0x0c
#define AES_CS_CAT_CPYREV_EQ5         0x0e


// Initialize the register based Channel Status functionallity
// This function should be called before using any of the aesTxCS functions.
// It will default to Professional use, 48K sample rate and no emphasis
HRESULT aesTxCSEnableRegisterMode (void);

// Set the CS indicated sample rate, use the AES_CS_RATE_ defines
HRESULT aesTxCSSetSampleRate (uint8 rateId);

// Set the CS indicated mode, TRUE sets pro use.
HRESULT aesTxCSSetMode (uint8 bProUse);

// Set the CS indicated copy protect bits, use the AES_CS_CPY_ defines
HRESULT aesTxCSSetCpy (uint8 cpy);

// Set the CS indicated pre-emphasis bits, use the AES_CS_PEMP_ defines
HRESULT aesTxCSSetPreEmphasis (uint8 preEmp);

// Set the CS indicated device category bits, use the AES_CS_CAT_ defines
HRESULT aesTxCSSetCategory (uint8 category);




#endif
