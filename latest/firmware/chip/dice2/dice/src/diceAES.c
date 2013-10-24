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

****************************************************************************/

#include <string.h>
#include "TCTypes.h"
#include "TCDebug.h"
#include "TCTasking.h"
#include "ErrorCodes.h"
#include "diceDefs.h"
#include "diceAES.h"
#include "diceClock.h"
#include "targetBoard.h" //for LED
#include "sysCli.h"

static char 	moduleName[] = __FILE__ ;

DICE_AESRX *pDiceAESRx = (DICE_AESRX *)(DICEBASE + DICE_AESRX_BASE);
DICE_AESTX *pDiceAESTx = (DICE_AESTX *)(DICEBASE + DICE_AESTX_BASE);



/************************* LOW LEVEL FUNCTIONS *****************************/

// Set AES receiver PLL pulse width
HRESULT aesRxSetPLLPulse(uint8 pulseWidthUp, uint8 pulseWidthDown)
{
	HRESULT hResult = NO_ERROR;

	pDiceAESRx->pllPulse.bit.down_pulse_w = pulseWidthDown;
	pDiceAESRx->pllPulse.bit.up_pulse_w = pulseWidthUp;
	return hResult;
}

// Set AES receiver VCO min. frequence
HRESULT aesRxSetVCOFq(uint16 minFq)
{
	HRESULT hResult = NO_ERROR;

	pDiceAESRx->minVCOFqLSB = minFq & 0xff;
	pDiceAESRx->minVCOFqMSB = (minFq>>8) & 0xff;
	return hResult;
}


// Values for force mode
#define VCO_FORCE_NONE		0
#define VCO_FORCE_UP		1
#define VCO_FORCE_DOWN		2


// Set AES receiver VCO force mode
static void aesRxForceVCO(uint8 forceMode)
{
	//LM??? use shadow to only access once

	switch (forceMode)
	{
		case VCO_FORCE_NONE:
			pDiceAESRx->forceVCO.bit.force_down = 0;
			pDiceAESRx->forceVCO.bit.force_up = 0;
			break;
		case VCO_FORCE_UP:
			pDiceAESRx->forceVCO.bit.force_down = 0;
			pDiceAESRx->forceVCO.bit.force_up = 1;
			break;
		case VCO_FORCE_DOWN:
			pDiceAESRx->forceVCO.bit.force_up = 0;
			pDiceAESRx->forceVCO.bit.force_down = 1;
			break;
	}
}



/************************* HIGH LEVEL FUNCTIONS *****************************/
// AES Rx State machine functions.
// Lock state machine
// The lock state machine monitors the AES PLL and makes sure that lock is
// obtained by pumping the PLL.
// aesRxStateMachine function should be called at least every 20ms.

#define FSM_TICKSIZE	20  // run every 20ms
#define FSM_FORCE_UP	100/FSM_TICKSIZE //force up for 100ms
#define FSM_FORCE_WAIT	100/FSM_TICKSIZE //after force wait for 100ms

typedef enum
{
	AES_STATE_DISABLED,  //The AES Rx is not started
	AES_STATE_FORCE_PLL, //The AES PLL is being forced
	AES_STATE_CHECK,     //The AES PLL is not forced, check for lock
	AES_STATE_LOCKED     //The Rx or Tx is enabled and running
} AES_STATE;

typedef struct 
{
	AES_STATE		state;
	uint8			lockmask;			// running locked status of the interfaces
	uAESRX_ERROR	rxError[NUM_AES];	//sticky 
	uAESRX_ERROR	_rxError[NUM_AES];	//non-sticky 
	uint8			rxSelMaster;			//selected master if NUM_AES then auto search
	uint8			rxMaster;			//current master
	uint32			stateVal;			//countig sub states
	uint32			lastTime;
} AES_RX_FSM;

static AES_RX_FSM aesFSM;


#define DIRECT_EV_MASK  (EVT_AES1_CRC | EVT_AES1_PARITY | EVT_AES1_VALIDITY	| EVT_AES1_SLIP | EVT_AES1_RPT)


static void aesRxSetState (AES_STATE state)
{
	if (aesFSM.state == state) return;
  
	switch (state)
	{
		case AES_STATE_DISABLED:
			break;
    
		case AES_STATE_FORCE_PLL:
			aesRxForceVCO(VCO_FORCE_UP);
			aesFSM.stateVal = FSM_FORCE_UP; // wait 100 ms ticks for PLL to stabelize
			break;       
    
		case AES_STATE_CHECK:
			aesRxForceVCO(VCO_FORCE_NONE);
			aesFSM.stateVal = FSM_FORCE_WAIT; // wait extra ticks for PLL to stabelize
			break;
        
		case AES_STATE_LOCKED:
			break;
	}
	aesFSM.state = state;
	if (aesFSM.state == AES_STATE_LOCKED)
		LED_ON(AES_LOCK_LED_ID);
	else
		LED_OFF(AES_LOCK_LED_ID);

}

void aesRxStateMachine (uint8 events[NUM_AES])
{
	uint8 i, mask;
	uint8 * pEvents = events;
	BOOL bLocked;
	uAESRX_ERROR mstrError;
		
	memset (events, 0, NUM_AES);
	
	if (TCTimeElapsed(aesFSM.lastTime) < FSM_TICKSIZE) return;
	aesFSM.lastTime = TCTimeGet();	
	if (aesFSM.state == AES_STATE_DISABLED) return;

	mstrError = pDiceAESRx->error;
	for (i = 0 ; i < NUM_AES; i++) 
	{
		aesFSM._rxError[i].reg = pDiceAESRx->rxError[i].reg; // read and clear sticky bits
		if (i == aesFSM.rxMaster)
		{
			// read the master overrun underrun
			aesFSM._rxError[i].bit.o_run = mstrError.bit.o_run;
			aesFSM._rxError[i].bit.u_run = mstrError.bit.u_run;
		}
		aesFSM.rxError[i].reg |= aesFSM._rxError[i].reg; // accumulate sticky bits
	}
	
	
	switch (aesFSM.state)
	{
		case AES_STATE_FORCE_PLL:
			if (!aesFSM.stateVal)
				aesRxSetState (AES_STATE_CHECK);
			else
				aesFSM.stateVal--;
			break;
    
		case AES_STATE_CHECK:
			if (!aesFSM.stateVal)
			{
				if (aesFSM._rxError[aesFSM.rxMaster].bit.lock == 1) 
				{
					aesRxSetState (AES_STATE_LOCKED);
					break;
				}
			    
				// we need to pump again or try another source
				if (aesFSM.rxSelMaster < NUM_AES)
				{
					aesRxSetState (AES_STATE_FORCE_PLL);
				}
				else
				{
					aesFSM.rxMaster++;
					if (aesFSM.rxMaster >= NUM_AES) aesFSM.rxMaster = 0;
					pDiceAESRx->setup.bit.master_clock = aesFSM.rxMaster;
					aesRxSetState (AES_STATE_FORCE_PLL);
				}
			}
			else
			{
				aesFSM.stateVal--;
			}
			break;      
      
		case AES_STATE_LOCKED:
			if (aesFSM._rxError[aesFSM.rxMaster].bit.no_lock == 1)
			{
				aesRxSetState (AES_STATE_FORCE_PLL);
			}
			break;
	}
	// collect lock status events and other events if locked
	mask = 0x1;
	for (i = 0 ; i < NUM_AES; i++) 
	{
		if (i == aesFSM.rxMaster)
			bLocked = (aesFSM.state == AES_STATE_LOCKED);
		else
			bLocked = (aesFSM.state == AES_STATE_LOCKED) && ((aesFSM._rxError[i].reg & 0x03) == 0x01);
		
		// now check if we need to notify
		if (bLocked && !(aesFSM.lockmask & mask))
		{
			*pEvents = EVT_AES1_LOCK_ACQUIRED;
			aesFSM.lockmask |= mask;
		}
		else if (!bLocked && (aesFSM.lockmask & mask))
		{
			*pEvents = EVT_AES1_LOCK_LOST;
			aesFSM.lockmask &= ~mask;
		}
		if (bLocked)
		{
			*pEvents |= aesFSM._rxError[i].reg & DIRECT_EV_MASK;
		}
		mask <<= 1;
		pEvents++;
	}
}

HRESULT aesRxEnable (BOOL bEnable)
{
	if (bEnable)
	{
		aesRxSetState (AES_STATE_FORCE_PLL);
		clkSetVCO(DEV_AES_RX, ENABLE_RX_VCO, INTERNAL_VCO);
	}
	else
	{
		aesRxSetState (AES_STATE_DISABLED);
		clkSetVCO(DEV_AES_RX, DISABLE_RX_VCO, INTERNAL_VCO);
	}			
	return NO_ERROR;
}

HRESULT aesRxGetEnable (BOOL * bEnable)
{
	*bEnable = (aesFSM.state != AES_STATE_DISABLED);
	return NO_ERROR;
}


// Select master clock from AES receive X
HRESULT aesSetMasterClk(uint8 aesRxId)
{
	HRESULT hResult = NO_ERROR;

	if (aesRxId <= NUM_AES)
	{
		if (aesRxId != NUM_AES) {
			pDiceAESRx->setup.bit.master_clock = aesRxId;
			aesFSM.rxMaster = aesRxId;
		}
		aesFSM.rxSelMaster = aesRxId;

		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Get actual master clock for AES receive
HRESULT aesGetMasterClk(uint8 *aesRxId)
{
	HRESULT hResult = NO_ERROR;
	
    //in any mode return the actual
	*aesRxId = aesFSM.rxMaster;

	return hResult;
}

// Read AES status flags
HRESULT aesGetStatus (uint16 *aesStatus)
{
	HRESULT hResult = NO_ERROR;
	
	*aesStatus = 0;	
	if (aesFSM.state == AES_STATE_LOCKED)
	{
		*aesStatus = AES_STAT_MASTERLOCK;
		*aesStatus |= aesFSM.lockmask;
		*aesStatus |= aesFSM.rxError[0].bit.v_high ? AES_STAT_VALIDITY0 : 0;
		*aesStatus |= aesFSM.rxError[1].bit.v_high ? AES_STAT_VALIDITY1 : 0;
		*aesStatus |= aesFSM.rxError[2].bit.v_high ? AES_STAT_VALIDITY2 : 0;
		*aesStatus |= aesFSM.rxError[3].bit.v_high ? AES_STAT_VALIDITY3 : 0;
    }
	return hResult;
}
    
BOOL aesIsMasterLocked (void)
{
	return (aesFSM.state == AES_STATE_LOCKED);
}

// Read AES error flags for AES receiver given by ID
HRESULT aesGetRxError(uint8 aesRxId, uAESRX_ERROR *pRxError, BOOL clearCondition)
{
	HRESULT hResult = NO_ERROR;

	if (aesRxId < NUM_AES)
	{
		pRxError->reg = aesFSM.rxError[aesRxId].reg;
		if (clearCondition) aesFSM.rxError[aesRxId].reg = 0x0;
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Set AES transmitter mute for channel 0-7, if ALL_CH then mute/unmute all
HRESULT aesTxSetMute(uint8 aesTxCh, BOOL mute)
{
	HRESULT hResult = NO_ERROR;
	uint32 muteBit;

	if (aesTxCh < NUM_AES_CHANNELS)
	{
		muteBit = 1 << aesTxCh;
	}
	else
	{
		muteBit = 0xff;
	}

	if (mute)
		pDiceAESTx->chMute.reg |= muteBit;  // Set mute
	else
		pDiceAESTx->chMute.reg &= ~muteBit; // Clear mute
	return hResult;
}


/************************* LOW LEVEL C,U FUNCTIONS *****************************/

// Select AES receive channel to collect channel status bytes 0-23
// Input aesRxCh: 0 - 7
HRESULT aesRxSetCSChannel(uint8 aesRxCh)
{
	HRESULT hResult = NO_ERROR;

	if (aesRxCh < NUM_AES_CHANNELS)
	{
		pDiceAESRx->setup.bit.cs_channel= aesRxCh;
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

HRESULT aesRxGetIndividualChannelStatus (uint8 aesRxCh, uint8 chStat[NUM_AES_CS_BYTES])
{
	HRESULT hResult = NO_ERROR;
	uint8 i;

	if (aesRxCh < NUM_AES_CHANNELS)
	{
		for (i=0; i<NUM_AES_CS_BYTES; i++)
		{
			chStat[i] = pDiceAESRx->csChannel[aesRxCh][i];
		}
		return hResult;
	}

	hResult = E_DICE_BAD_INPUT_PARAM;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

HRESULT aesRxGetSelectedChannelStatus (uint8 chStat[NUM_AES_SEL_CS_BYTES])
{
	HRESULT hResult = NO_ERROR;
	uint8 i;
	
	for (i=0; i<NUM_AES_SEL_CS_BYTES; i++)
	{
		chStat[i] = pDiceAESRx->csChannelSel[i];
	}
	return hResult;
}


// Set AES transmitter in master mode with intern or extern blocksync
HRESULT aesTxSetMasterMode(BOOL externBlocksync)
{
	HRESULT hResult = NO_ERROR;

	pDiceAESTx->modeSetup.bit.mode = AESTX_MASTER_MODE;
	pDiceAESTx->cblSetup.bit.master_mode = externBlocksync;
	return hResult;
}

// Set AES transmitter in slave mode with blocksync from channel 0-7
HRESULT aesTxSetSlaveMode(uint8 aesTxCh)
{
	HRESULT hResult = NO_ERROR;
	
	pDiceAESTx->modeSetup.bit.mode = AESTX_SLAVE_MODE;
	pDiceAESTx->cblSetup.bit.slave_mode = aesTxCh;
	return hResult;
}


// Set AES transmitter validity bit for channel 0-7, if ALL_CH then mute/unmute all
// Validity bit setting only used in master mode
HRESULT aesTxSetValidAudio(uint8 aesTxCh, BOOL valid)
{
	HRESULT hResult = NO_ERROR;
	uint32 VBit;

	if (aesTxCh < NUM_AES_CHANNELS)
	{
		VBit = 1 << aesTxCh;
	}
	else
	{
		VBit = 0xff;
	}

	if (valid)
		pDiceAESTx->chValidity.reg |= VBit;  // Set V Bit
	else
		pDiceAESTx->chValidity.reg &= ~VBit; // Clear V Bit

	return hResult;
}


// Enable or disable auto CRC for AES transmitters 0-3 
HRESULT aesTxSetAutoCRC(BOOL enable)
{
	HRESULT hResult = NO_ERROR;

	if (enable)
	{
		pDiceAESTx->modeSetup.bit.crc_disable = AESTX_CRC_ENABLE;
	}
	else
	{
		pDiceAESTx->modeSetup.bit.crc_disable = AESTX_CRC_DISABLE;
	}
	return hResult;
}



static HRESULT aesTxSetUser(uint8 aesTxCh, uint8 select)
{
	HRESULT hResult = NO_ERROR;
	int i;
	
	if (aesTxCh < NUM_AES_CHANNELS)
	{
		i = aesTxCh%NUM_AES;
		if (aesTxCh%2 == 0)
		{
			pDiceAESTx->userSelCh[i].bit.ch0_cs_user = select;
		}
		else
		{
			pDiceAESTx->userSelCh[i].bit.ch1_cs_user = select;
		}
	}
	else
	{
		for (i = 0; i < NUM_AES; i++)
		{
			pDiceAESTx->userSelCh[i].bit.ch0_cs_user = select;
			pDiceAESTx->userSelCh[i].bit.ch1_cs_user = select;
		}
	}

	return hResult;
}

// Set user bit selection from AESRX channel 
// if aesTxCh = ALL_CH then all Tx channels set to AESRX channel
HRESULT aesTxSetUserFromAESRXCh(uint8 aesTxCh, uint8 aesRxCh)
{
	HRESULT hResult = NO_ERROR;
	
	if (aesRxCh >= NUM_AES_CHANNELS)
	{
		hResult = E_DICE_BAD_INPUT_PARAM;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return aesTxSetUser(aesTxCh, aesRxCh);
}

// Set user bit selection from AVS receiver, 
// if aesTxCh = ALL_CH then all Tx channels set to AVSRX plug
HRESULT aesTxSetUserFromAVSRX(uint8 aesTxCh, uint8 plugID)
{
	uint8 userSelect = (plugID & 0x7) | 0x8; 
	return aesTxSetUser(aesTxCh, userSelect);
}

// Set user bit to zero, if ALL_CH then all Tx channels set to zero 
HRESULT aesTxSetUserZero(uint8 aesTxCh)
{
	return aesTxSetUser(aesTxCh, 0);
}



// Higher level Channel Status functions. 
//   All functions with the prefix aesTxCS will assume that
//   Channel status is register based and not based on the meta data in the upper 8 bits of the
//   audio channel. Those functions will work both in slave and master mode, but in slave mode only
//   the block sync (Z- preamble) and possibly the U bit will be extracted from the audio stream.
// 
//   Be aware that the shared CS registers are used and channel status will be the same for all
//   8 channels.
//


typedef struct
{
    uint8  bProUse;
    uint8  cpy;
    uint8  rate;
    uint8  preEmp;
    uint8  category;
} CS_DATA;


static CS_DATA aesCSData;



static void aesTxUpdateCS (void)
{
  uint8 b0,b1,b2,b3,b4,L,b;
  
  if (aesCSData.bProUse)
  {
    b0 = 0x01; //pro use, no pre emph.
    b1 = 0x00; //mode not indicated, def. U data undefined
    b2 = 0x2c; //24 bit data mode
    b3 = 0x00; //reserved, should be 0
    b4 = 0x00; //not reference
    switch (aesCSData.preEmp)
    {
      case AES_CS_PEMP_NIND:
        break;
      case AES_CS_PEMP_NONE:
        b0 |= 0x04;
        break;        
      case AES_CS_PEMP_50_15US:
        b0 |= 0x0c;
        break;
      case AES_CS_PEMP_J17:
        b0 |= 0x1c;
        break;
    }
    switch (aesCSData.rate)
    {
      case AES_CS_RATE_32K:
        b0 |= 0xc0;
        break;
      case AES_CS_RATE_44K1:
        b0 |= 0x40;
        break;
      case AES_CS_RATE_48K:
        b0 |= 0x80;
        break;
      case AES_CS_RATE_88K2:
        b4 |= 0x50;
        break;
      case AES_CS_RATE_96K:
        b4 |= 0x10;
        break;
      case AES_CS_RATE_176K4:
        b4 |= 0x58;
        b1 |= 0x0e;
        break;        
      case AES_CS_RATE_192K:
        b4 |= 0x18;
        b1 |= 0x0e;
        break;
    }
    pDiceAESTx->csChannelSel[0] = (uint32)b0;
    pDiceAESTx->csChannelSel[1] = (uint32)b1;
    pDiceAESTx->csChannelSel[2] = (uint32)b2;
    pDiceAESTx->csChannelSel[3] = (uint32)b3;
    pDiceAESTx->csChannelSel[4] = (uint32)b4;
    
    pDiceAESTx->modeSetup.bit.crc_disable |= 0xf; //enable auto CRC
    
  }
  else
  {
    b0 = 0x00;  //Cons use, no pre emph.
    b1 = aesCSData.category;
    b2 = 0x00; 
    b3 = 0x00;
    b4 = 0x0b; //indicate 24 bit data
    switch (aesCSData.preEmp)
    {
      case AES_CS_PEMP_NIND:
      case AES_CS_PEMP_NONE:
        break;        
      case AES_CS_PEMP_50_15US:
      case AES_CS_PEMP_J17:
        b0 |= 0x80;
        break;
    }
    switch (aesCSData.rate)
    {
      case AES_CS_RATE_32K:
        b3 |= 0x03;
        break;
      case AES_CS_RATE_44K1:
      case AES_CS_RATE_88K2:
      case AES_CS_RATE_176K4:
        b3 |= 0x00;
        break;
      
      case AES_CS_RATE_48K:
      case AES_CS_RATE_96K:
      case AES_CS_RATE_192K:
        b3 |= 0x02;
        break;
    }
    //Set the "L" and "C" copy generation bits
    if (aesCSData.cpy == AES_CS_CPY_INF)
    {
      b0 |= 0x04; 
    } 
    else
    {
        L = (aesCSData.cpy == AES_CS_CPY_ONE);
        b = (aesCSData.category & AES_CS_CAT_CPYREV_MASK);
        if ((b == AES_CS_CAT_CPYREV_EQ1) ||
            (b == AES_CS_CAT_CPYREV_EQ2) ||
            (b == AES_CS_CAT_CPYREV_EQ3) ||
            (b == AES_CS_CAT_CPYREV_EQ4) ||
            (b == AES_CS_CAT_CPYREV_EQ5))
        {
          //special legacy case for the definition of the "L" bit
          L = !L;
        }
        if (L) b1 |= 0x80;
    }
      
    pDiceAESTx->csChannelSel[0] = (uint32)b0;
    pDiceAESTx->csChannelSel[1] = (uint32)b1;
    pDiceAESTx->csChannelSel[2] = (uint32)b2;
    pDiceAESTx->csChannelSel[3] = (uint32)b3;
    pDiceAESTx->csChannelSel[4] = (uint32)b4;

    pDiceAESTx->modeSetup.bit.crc_disable  &= ~0xf; //disable auto CRC    
  }
}
    
HRESULT aesTxCSEnableRegisterMode (void)
{
	HRESULT hResult = NO_ERROR;

    //in slave mode, use APB registers
    pDiceAESTx->csSelCh0_3.bit.ch0_cs = 2;
    pDiceAESTx->csSelCh0_3.bit.ch1_cs = 2;
    pDiceAESTx->csSelCh0_3.bit.ch2_cs = 2;
    pDiceAESTx->csSelCh0_3.bit.ch3_cs = 2;
    pDiceAESTx->csSelCh4_7.bit.ch0_cs = 2;
    pDiceAESTx->csSelCh4_7.bit.ch1_cs = 2;
    pDiceAESTx->csSelCh4_7.bit.ch2_cs = 2;
    pDiceAESTx->csSelCh4_7.bit.ch3_cs = 2;
    
    //All channels use common register structure
    pDiceAESTx->csSelApb.bit.ch0_cs_apb = 0x1;
    pDiceAESTx->csSelApb.bit.ch1_cs_apb = 0x1;
    pDiceAESTx->csSelApb.bit.ch2_cs_apb = 0x1;
    pDiceAESTx->csSelApb.bit.ch3_cs_apb = 0x1;
    pDiceAESTx->csSelApb.bit.ch4_cs_apb = 0x1;
    pDiceAESTx->csSelApb.bit.ch5_cs_apb = 0x1;
    pDiceAESTx->csSelApb.bit.ch6_cs_apb = 0x1;
    pDiceAESTx->csSelApb.bit.ch7_cs_apb = 0x1;

    aesCSData.bProUse    = TRUE;
    aesCSData.cpy        = AES_CS_CPY_INF;
    aesCSData.rate       = AES_CS_RATE_48K;
    aesCSData.preEmp     = AES_CS_PEMP_NONE;
    aesCSData.category   = AES_CS_CAT_AD;

    aesTxUpdateCS ();
    
    return hResult;
}


HRESULT aesTxCSSetSampleRate (uint8 rateId)
{
	aesCSData.rate = rateId;
	aesTxUpdateCS ();
    
	return NO_ERROR;
}


HRESULT aesTxCSSetMode (uint8 bProUse)
{
	aesCSData.bProUse = bProUse;
	aesTxUpdateCS ();
    
	return NO_ERROR;
}

HRESULT aesTxCSSetCpy (uint8 cpy)
{
	aesCSData.cpy = cpy;
	aesTxUpdateCS ();
    
	return NO_ERROR;
}

HRESULT aesTxCSSetPreEmphasis (uint8 preEmp)
{
	aesCSData.preEmp = preEmp;
	aesTxUpdateCS ();
    
	return NO_ERROR;
}

HRESULT aesTxCSSetCategory (uint8 category)
{
	aesCSData.category = category;
	aesTxUpdateCS ();
    
	return NO_ERROR;
}


// Clear everything 
HRESULT aesInitialize(void)
{
	HRESULT hResult = NO_ERROR;
	static uint32 regVal;

	// Clear entier AES transmitter register area
	memset(pDiceAESTx, 0, sizeof(DICE_AESTX));

	// Clear receive setup register
	pDiceAESRx->setup.reg = 0;

	// Dummy read error flags to be reset
	regVal = pDiceAESRx->error.reg;
	regVal = pDiceAESRx->rxError[0].reg;
	regVal = pDiceAESRx->rxError[1].reg;
	regVal = pDiceAESRx->rxError[2].reg;
	regVal = pDiceAESRx->rxError[3].reg;

	// Avoid warning
	if (regVal) hResult = NO_ERROR;

	// Mute all transmitters
	pDiceAESTx->chMute.reg = 0xff; 
	// Set all validity bits to valid audio
	pDiceAESTx->chValidity.reg = 0xff;

	aesFSM.state      = AES_STATE_DISABLED;
	aesFSM.stateVal   = 0;
	aesFSM.rxSelMaster= NUM_AES; // Lock to any AES
	aesFSM.rxMaster   = 0;       // Lock to any AES
	aesFSM.lastTime = TCTimeGet();
	
	//Default settings for AES module
	aesRxSetPLLPulse (7, 7);
	aesRxSetVCOFq (0x06DB);
	aesRxForceVCO (VCO_FORCE_NONE);
	aesTxCSEnableRegisterMode ();
	
	return hResult;
}
