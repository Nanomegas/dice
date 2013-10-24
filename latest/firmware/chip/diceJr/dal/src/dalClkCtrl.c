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
	
	Purpose:	Dice Audio Application Layer (DAL)- Clock controller FSM
				This module waits for clock source selection fro
				the user. Then it starts to measure the rate
				and then setting the clock routing and rate.
				There are 2 clock domains one for router 1 and 2.
				Both sample rate counters are used in this module.
	
	Revisions:
		created 09/03/2005 ML

****************************************************************************/

#include <string.h>
#include "TCTypes.h"
#include "TCDebug.h"
#include "TCTasking.h"
#include "ErrorCodes.h"
//#include "dice.h"
//#include "avs.h"
//#include "targetBoard.h" //for LED
#include "dal.h"
#include "dalRates.h"
#include "dalAES.h"
#include "dalADAT.h"
#include "dalAVS.h"
#include "dalWCEXT.h"
#include "dalClkCtrl.h"


static char 	moduleName[] = __FILE__ ;

#define TICKS100MS (100/(DAL_TICK_MS)+1)
#define TICSFROMMS(ms) ((ms)/(DAL_TICK_MS)+1)

typedef struct
{
	uint32	ticksWaitStable;		// Ticks to wait for rate to be stable
	uint32	ticksWaitLock;			// Ticks to wait for HPLL to lock
	uint8	bPLLoffWhenNoSource;	// How to deal with HPLL when no valid source
} DAL_CLOCKCTRL_CONFIG;

static DAL_CLOCKCTRL_CONFIG dalClockConfiguration =
{
	TICSFROMMS(100),
	TICSFROMMS(800),
	FALSE
};



#if defined(_SYSDEBUG) || defined(_CLI)

const char * dalStateNames[eDAL_STATE_LOCKED+1] =
{
	"STATE_DISABLED",
	"STATE_SETUP_SRC",
	"STATE_WAIT_SRC_LOCK",
	"STATE_WAIT_RATE_STABLE",
	"STATE_CHECK_RATE",
	"STATE_LOCKING",
	"STATE_ILLEGAL_RATE",
	"STATE_RATE_IS_FLAKY",
	"STATE_LOCKED"
};

#endif // _SYSDEBUG
 
/*
Each of the possible clock source interfaces use different ways of determining the
rate mode in case of eDAL_RATE_MODE_LOW_MID and they each require different 
setting of the PLL. The interface will be called with the DAL_RX_PLL_DATA data
pointer and asked to fill out this information.
Before the interface is called the counted reference rate and the interface
rate mode is filled out. The call back should fill out the following entries:
rateHZ = the actual sample rate calculated based on ref8Hz
reatID = the ID of the nearest matching rate found. If not found this field should be
		 set to eDAL_NOMINAL_RATE_NONE.
preDiv = the pre division factor needed for this interface. This would be set different from
		 1 if the interface delivers a rate which is a multiple of the base rate.
addDiv = the additional feedback division factor. This is set different from 1 if the 
		 interface delivers a rate which is a fraction of the base rate.


The structure DAL_RX_INTERFACE should be filled out with the corresponding callback functions
for each clock source interface.

*/


typedef void (*DAL_RX_PLL_FUNC)(uint32 data, DAL_RX_PLL_DATA * pPllData);
typedef uint8 (*DAL_RX_LOCK_FUNC)(uint32 data);

typedef struct
{
	DAL_RX_PLL_FUNC			pllFunc;	// Function to obtain rate and pll information
	DAL_RX_LOCK_FUNC		lockFunc;	// Function to enquire the source locked status
	uint32					pllSource;	// The actual HPLL source used for this interface
	uint32					data;		// Private data passed to the function
} DAL_RX_INTERFACE;

static uint8 defaultIsLocked (uint32 data)
{
	return TRUE;
}

static DAL_RX_INTERFACE dalInterfaces[eDAL_CLK_SRC_INTERNAL] =
{
	{dalAESPllSettings, dalAESIsLocked, HPLL_CLK_AESRX, AES_ANY},  //eDAL_CLK_SRC_AESRX
	{dalAESPllSettings, dalAESIsLocked, HPLL_CLK_AESRX, AES_ID0},  //eDAL_CLK_SRC_AESRX0
	{dalAESPllSettings, dalAESIsLocked, HPLL_CLK_AESRX, AES_ID1},  //eDAL_CLK_SRC_AESRX1
	{dalAESPllSettings, dalAESIsLocked, HPLL_CLK_AESRX, AES_ID2},  //eDAL_CLK_SRC_AESRX2
	{dalAESPllSettings, dalAESIsLocked, HPLL_CLK_AESRX, AES_ID3},  //eDAL_CLK_SRC_AESRX3
	{dalADATPllSettings, dalADATIsLocked, HPLL_CLK_ADATRX, 0},  //eDAL_CLK_SRC_ADATRX
	{dalADATPllSettings, dalADATIsLocked, HPLL_CLK_ADATRX_AUX, 1},  //eDAL_CLK_SRC_ADATRX
	{dalWCPllSettings, defaultIsLocked , HPLL_CLK_WORD, 0},  //eDAL_CLK_SRC_WORD
#ifdef _AVS
	{dalAVSPllSettings, dalAVSIsLocked, HPLL_CLK_AVS_SYT1, AVS_PLUG_ID1},  //eDAL_CLK_SRC_AVS_SYT1
	{dalAVSPllSettings, dalAVSIsLocked, HPLL_CLK_AVS_SYT2, AVS_PLUG_ID2},  //eDAL_CLK_SRC_AVS_SYT2
#else
	{dalAVSPllSettings, dalAVSIsLocked, HPLL_CLK_AVS_SYT1, 0},  //eDAL_CLK_SRC_AVS_SYT1
	{dalAVSPllSettings, dalAVSIsLocked, HPLL_CLK_AVS_SYT2, 1},  //eDAL_CLK_SRC_AVS_SYT2
#endif
	{dalExtPllSettings, defaultIsLocked , HPLL_CLK_EXTERNAL, 0} //eDAL_CLK_SRC_EXTERNAL
};


uint8 dalIsInterfaceLocked (DAL_CLOCK_SOURCE_ENUM source)
{
	DAL_RX_INTERFACE * pRx = &dalInterfaces[source];
	if (source < eDAL_CLK_SRC_INTERNAL)
		return pRx->lockFunc(pRx->data);
	else
		return TRUE;
}

static void dalGetRxPLLData (DAL_CLOCK_SOURCE_ENUM source, DAL_RX_PLL_DATA * pPllData)
{
	DAL_RX_INTERFACE * pRx = &dalInterfaces[source];
	if (source < eDAL_CLK_SRC_INTERNAL)
		pRx->pllFunc(pRx->data,pPllData);
	
	return;
}


// This is a hack to handle the local setting of the AES receiver source selection
static void dalHandleAESClockSelection (DAL_CLOCKCTRL_INTERFACE * pInterface)
{
	switch (pInterface->clockSource)
	{
		case eDAL_CLK_SRC_AESRX0:
			dalAESSetMasterClk(AES_ID0);
			break;
		case eDAL_CLK_SRC_AESRX1:
			dalAESSetMasterClk(AES_ID1);
			break;
		case eDAL_CLK_SRC_AESRX2:
			dalAESSetMasterClk(AES_ID2);
			break;
		case eDAL_CLK_SRC_AESRX3:
			dalAESSetMasterClk(AES_ID3);
			break;
		case eDAL_CLK_SRC_AESRX:
			dalAESSetMasterClk(AES_ANY);
			break;
		default:
		    //when AES is not master default to ANY
		    //ML??? Should that be user selectable??
		    if (DEVICE_BIT(DEV_AES_RX) & pInterface->inputDevices)
		    {
				dalAESSetMasterClkDefault();
			}
			break;
	}
}

// Set the PLL to generate a fixed rate based on the last good nominal rate.
// In some cases the last good rate is not within the rate mode if the 
// rate mode has changed. In that case the default nominal rate for that
// rate mode is selected.
static void dalFixPLLToLastGoodClock (DAL_CLOCKCTRL_INTERFACE * pInterface)
{
	RATE_DESC * pRate;
	
	pRate = dalRateGetDefaultRate (pInterface->rateMode, pInterface->actualNominalRate);
    
    //we never call this function when internal rate is selected so we can use prescaler
    hpllCoast (pInterface->ID, 1);
	clkSetPrescaler(pInterface->ID, pRate->denom);
	hpllSetDividers  (pInterface->ID, 1, pRate->nom);
	//we are using other means to get the HPLL to lock to the prescaler
	//so our measurement wont be disturbed
	hpllSetReference (pInterface->ID, HPLL_REF_IN_PRESCL);
	hpllCoast (pInterface->ID, 0);			
    
    if (pRate->rateMode == eDAL_RATE_MODE_LOW)
		clkSetInRouter(pInterface->ID, LOW_RATE);
	else if (pRate->rateMode == eDAL_RATE_MODE_MID)
		clkSetInRouter(pInterface->ID, MID_RATE);
	else
		clkSetInRouter(pInterface->ID, HIGH_RATE);

	hpllEnableClocks (pInterface->ID, TRUE);
}
	
// This function is called to disable the HPLL in case the
// selected rate isn't any good. This function will either
// force the HPLL to generate a fixed rate or turn the clock
// off depending on a user setting
static void dalDisableHPLL (DAL_CLOCKCTRL_INTERFACE * pInterface)
{	
	if (dalClockConfiguration.bPLLoffWhenNoSource)
		hpllEnableClocks (pInterface->ID, FALSE);
	else
		dalFixPLLToLastGoodClock (pInterface);
}

static void dalEnableHPLL (DAL_CLOCKCTRL_INTERFACE * pInterface)
{
	hpllEnableClocks (pInterface->ID, TRUE);
	hpllSetReference (pInterface->ID, HPLL_REF_IN_CLKCTRL);
}


static void dalSelectSource (DAL_CLOCKCTRL_INTERFACE * pInterface)
{
	
	// This function selects the right source for the JET PLL. The AES
	// shares one PLL so we need to select which AES source to use
	dalHandleAESClockSelection (pInterface);
	
	if (pInterface->clockSource < eDAL_CLK_SRC_INTERNAL)
	{	
		dalDisableHPLL  (pInterface); // not unleashed until rate is validated
		clkSetInHybridPll(dalInterfaces[pInterface->clockSource].pllSource);
		
	}
	else
	{
		// setup internal rate
		RATE_DESC * pRate = dalRateByID (pInterface->sampleRate);
		if (pRate)
		{
			//we set that right away, it is always good
			hpllCoast (pInterface->ID, 1);
			clkSetPrescaler(pInterface->ID, pRate->denom);
			hpllSetDividers  (pInterface->ID, 1, pRate->nom);
			clkSetInHybridPll(HPLL_CLK_PRESCALER);
			dalEnableHPLL  (pInterface);
			hpllCoast (pInterface->ID, 0);			
			if (pRate->rateMode == eDAL_RATE_MODE_LOW)
				clkSetInRouter(pInterface->ID, LOW_RATE);
			else if (pRate->rateMode == eDAL_RATE_MODE_MID)
				clkSetInRouter(pInterface->ID, MID_RATE);
			else
				clkSetInRouter(pInterface->ID, HIGH_RATE);

		}
		else
		{			
			// this is an assertion, should never happen
			sysLogError(E_DAL_ILLEGAL_NOMINAL_RATE, __LINE__, moduleName);
		}
	}
}
	
// This function gets the ref count and asks the clock source interface
// to determine the rate and rate mode. It also asks the interface to 
// return the settings to be used for the HPLL dividers
// everything is recorded in rxPllData in the interface structure
static BOOL dalGetSamplerate(DAL_CLOCKCTRL_INTERFACE * pInterface)
{
	uint16 srValue, diff;
    uint32 hpllClk;
    RATE_DESC * pRate;
	
	hpllGetClock(&hpllClk); //get the HPLL clock rate
	hpllGetPeriod(pInterface->ID, &srValue, &diff);
  
	// Avoid div by zero
	if (srValue == 0) srValue = 1;
	
	//get the rate in 8th of Hz
	pInterface->rxPllData.ref8Hz = (hpllClk<<3)/srValue;
	pInterface->rxPllData.rateMode = pInterface->rateMode;	
	
	if (pInterface->clockSource == eDAL_CLK_SRC_INTERNAL)
	{
		pRate = dalRateByID (pInterface->sampleRate);		
		pInterface->rxPllData.rateHZ = pRate->rate;
		pInterface->rxPllData.rateID = pRate->ID;
	}
	else
	{
		//ask the receiver to find the rate
		dalGetRxPLLData (pInterface->clockSource, &pInterface->rxPllData);
	}		
	if (pInterface->rxPllData.rateID != eDAL_NOMINAL_RATE_NONE)
	{
		// we found one, check if it is legal in this rate mode
		pRate = dalRateByID (pInterface->rxPllData.rateID);
		if ((pInterface->rateMode == pRate->rateMode) ||
		    ((pInterface->rateMode == eDAL_RATE_MODE_LOW_MID) &&
		    ((pRate->rateMode == eDAL_RATE_MODE_LOW) || (pRate->rateMode == eDAL_RATE_MODE_MID))) ||
		    (pInterface->rateMode == eDAL_RATE_MODE_ALL)) 
		{
			// So the rate mode is ok, what about the nominal rate
			if ((pInterface->sampleRate == eDAL_NOMINAL_RATE_ANY) ||
			    (pInterface->sampleRate == pRate->sysNominalID) ||
			    (pInterface->sampleRate == pRate->ID))			    
			    return TRUE;
		}
		return FALSE; //the rate was not legal
	}
	else
	{
		// we did not find a valid rateID
		return FALSE;
	}
	
}

// This function is called whenever the state is changed
static void dalClockCtrlSetState  (DAL_CLOCKCTRL_INTERFACE * pInterface, DAL_STATE_ENUM newState)	
{
	
	if (pInterface->state == newState) return;
	
	SYS_DEBUG(SYSDEBUG_TRACE_DAL, "DAL-%i State Change to: %s\n\r", pInterface->ID,  dalStateNames[newState]);
	
	// handle notifications on leaving states
	if (pInterface->state == eDAL_STATE_LOCKED)
	{
		//we lost lock
		pInterface->events |= DAL_CB_LOCK1_LOST;
	}
	if (pInterface->state == eDAL_STATE_DISABLED)
	{
		//interface was created
		pInterface->events |= DAL_CB_CREATED1;
	}
	pInterface->ticksInState = 0;
	switch (newState)
	{
		case eDAL_STATE_DISABLED:
			pInterface->events |= DAL_CB_DESTROYED1;
			break;
			
		case eDAL_STATE_SETUP_SRC:
			break;
			
		case eDAL_STATE_WAIT_SRC_LOCK:
			break;
	
		case eDAL_STATE_WAIT_RATE_STABLE:
			break;

		case eDAL_STATE_CHECK_RATE:
			break;
			
	
		case eDAL_STATE_LOCKING:
			break;
	
		case eDAL_STATE_LOCKED:
			pInterface->events |= DAL_CB_LOCK1_AQUIRED;
#ifdef _MIXFIX_FIQ
			*mDicePtr(DICE_MIXER_BASE+0x8) = 16;
#endif
			break;
	
		case eDAL_STATE_ILLEGAL_RATE:
		    pInterface->events |= DAL_CB_RATE1_REJECTED;
			break;

		case eDAL_STATE_RATE_IS_FLAKY:
			break;
		
	}
	pInterface->state = newState;
}
		
		
/*
// Degenerated version of the state machine used when the interface is in slave mode
static void dalClockCtrlFSMSlave  (DAL_CLOCKCTRL_INTERFACE * pInterface, DAL_CLOCKCTRL_INTERFACE * pMasterInterface)
{
	dalClockCtrlSetState  (pInterface, pMasterInterface->state);
	if (pInterface->events | DAL_CB_CREATED1)
	{
		// the interface was created
		if (pInterface->rateMode == eDAL_RATE_MODE_SLAVE_LOW)
			clkSetInRouter(pInterface->ID, ROUTER_CLK_48K_OTHER);
		else
			clkSetInRouter(pInterface->ID, ROUTER_CLK_96K_OTHER);		
	}
	pInterface->events |= pMasterInterface->events;
	
	if (pMasterInterface->state == eDAL_STATE_LOCKED)
	{
		if ((pInterface->events | DAL_CB_RATE1_NOM_CHANGED) ||
			(pInterface->events | DAL_CB_RATE1_CHANGED) ||
			(pInterface->events | DAL_CB_CREATED1))
		{
			// need to calculate the actual values, fist find the base rate of the master
			uint32 rate;
			uint8  mode = eDAL_RATE_MODE_LOW;
			RATE_DESC * pRate;

			pRate = dalRateByID (pMasterInterface->actualNominalRate);
			rate = pMasterInterface->actualRateHz;
			if (!pRate) return; // this should not happen
			if (pRate->rateMode == eDAL_RATE_MODE_MID)
			{
				rate >>=1; //base rate is half
			}
			else if (pRate->rateMode == eDAL_RATE_MODE_HIGH)
			{
				rate >>=2; //base rate is quarter
			}
			// now find the actual rate for this interface
			if (pInterface->rateMode == eDAL_RATE_MODE_SLAVE_MID)
			{
				rate <<=1; //base rate is 2xbase
				mode = eDAL_RATE_MODE_MID;

			} 
			else if (pInterface->rateMode == eDAL_RATE_MODE_SLAVE_HIGH)
			{
				rate <<=2; //base rate is 4xbase
				mode = eDAL_RATE_MODE_HIGH;
			}
			// let's see if that exist
			pRate = dalRateFindNearest (&pInterface->rxPllData.rateID, rate);
			if (pRate)
			{
				pInterface->actualNominalRate = pRate->ID;
				pInterface->actualRateHz = rate;
				pInterface->actualRateMode = pRate->rateMode;
			}
			else
			{
				// this could happen if slave is running 32k as the higher rates
				// does not exist. We will do our best
				pInterface->actualRateHz = rate;
				pInterface->actualNominalRate = eDAL_NOMINAL_RATE_ANY;
				pInterface->actualRateMode = mode;
			}
		}
	}
}
*/

// The main state machine
HRESULT dalClockCtrlFSM  (DAL_CLOCKCTRL_INTERFACE * pInterface, DAL_CLOCKCTRL_INTERFACE * pMasterInterface)
{
	RATE_DESC * pRate;
					
	pInterface->events = 0;
	pInterface->ticksInState ++;
	// Look for asynchronous request to disable interface
	if (pInterface->reqEnable == eDAL_REQ_DISABLE)
	{
		dalClockCtrlSetState  (pInterface, eDAL_STATE_DISABLED);
		pInterface->reqEnable = eDAL_REQ_NONE;
		dalDisableHPLL  (pInterface);
	}
	
	// check for slave mode
	/*
	if ((pInterface->rateMode >= eDAL_RATE_MODE_SLAVE_LOW) && (pInterface->rateMode <= eDAL_RATE_MODE_SLAVE_HIGH))
	{
		if (pInterface->state != eDAL_STATE_DISABLED)
		{
			dalClockCtrlFSMSlave  (pInterface, pMasterInterface);
	    }
	    else
	    {
			if (pInterface->reqEnable == eDAL_REQ_ENABLE) 
			{
				pInterface->reqEnable = eDAL_REQ_NONE;
				// only enable interface if Master is enabled
			    if (pMasterInterface->state != eDAL_STATE_DISABLED)
				{
					dalClockCtrlFSMSlave  (pInterface, pMasterInterface);
				}				
			}
	    }
	    return NO_ERROR;
	}
	*/
	
	// Look for asynchronous request to change clock source
	if ((pInterface->clockSource != pInterface->reqClockSource) || (pInterface->sampleRate != pInterface->reqSampleRate))
	{
		pInterface->clockSource = pInterface->reqClockSource;
		pInterface->sampleRate = pInterface->reqSampleRate;
		if (pInterface->state != eDAL_STATE_DISABLED)
		{
			dalClockCtrlSetState  (pInterface, eDAL_STATE_SETUP_SRC);
			dalDisableHPLL  (pInterface);
		}
	}

	switch (pInterface->state)
	{
		case eDAL_STATE_DISABLED:
			if (pInterface->reqEnable == eDAL_REQ_ENABLE) dalClockCtrlSetState  (pInterface, eDAL_STATE_SETUP_SRC);
			pInterface->reqEnable = eDAL_REQ_NONE;
			break;
			
		case eDAL_STATE_SETUP_SRC:
			dalSelectSource (pInterface);
			dalClockCtrlSetState  (pInterface, eDAL_STATE_WAIT_SRC_LOCK);
			break;
			
		case eDAL_STATE_WAIT_SRC_LOCK:
			if (dalIsInterfaceLocked (pInterface->clockSource))
			{
				dalClockCtrlSetState  (pInterface, eDAL_STATE_WAIT_RATE_STABLE);
			}
			break;

		case eDAL_STATE_WAIT_RATE_STABLE:
			if (pInterface->clockSource == eDAL_CLK_SRC_INTERNAL)
			{
				dalClockCtrlSetState  (pInterface, eDAL_STATE_CHECK_RATE);
				break;
			}
			if (!dalIsInterfaceLocked (pInterface->clockSource))
			{
				dalClockCtrlSetState  (pInterface, eDAL_STATE_WAIT_SRC_LOCK);
			}
			else
			{
				uint16 dummy;
				//clear the counters (min and max since last call)
				hpllGetPeriod(pInterface->ID, &dummy, &dummy);
				if (pInterface->ticksInState > dalClockConfiguration.ticksWaitStable)
					dalClockCtrlSetState  (pInterface, eDAL_STATE_CHECK_RATE);
			}
			break;
					
		case eDAL_STATE_CHECK_RATE:
			if (pInterface->clockSource == eDAL_CLK_SRC_INTERNAL)
			{			  
			  dalClockCtrlSetState  (pInterface, eDAL_STATE_LOCKING);
			  break;
			}
			if (!dalIsInterfaceLocked (pInterface->clockSource))
			{
				dalClockCtrlSetState  (pInterface, eDAL_STATE_WAIT_SRC_LOCK);
				break;
			}
			// Check the source rate
			if (dalGetSamplerate(pInterface))
			{
				//rate is good
				//coast the PLL while changing
				pRate = dalRateByID (pInterface->rxPllData.rateID);
				hpllCoast (pInterface->ID, 1);
				hpllSetDividers  (pInterface->ID, pInterface->rxPllData.preDiv, pInterface->rxPllData.addDiv);
				dalEnableHPLL  (pInterface);
				hpllCoast (pInterface->ID, 0);	
				if (pRate->rateMode == eDAL_RATE_MODE_LOW)
					clkSetInRouter(pInterface->ID, LOW_RATE);
				else if (pRate->rateMode == eDAL_RATE_MODE_MID)
					clkSetInRouter(pInterface->ID, MID_RATE);
				else
					clkSetInRouter(pInterface->ID, HIGH_RATE);
				dalClockCtrlSetState  (pInterface, eDAL_STATE_LOCKING);
			} 
			else
			{
				dalClockCtrlSetState  (pInterface, eDAL_STATE_ILLEGAL_RATE);				
			}
			break;
		
		case eDAL_STATE_LOCKING:
			if (!dalIsInterfaceLocked (pInterface->clockSource))
			{
				dalDisableHPLL  (pInterface);
				dalClockCtrlSetState  (pInterface, eDAL_STATE_WAIT_SRC_LOCK);
				break;
			}
			if (pInterface->ticksInState > dalClockConfiguration.ticksWaitLock)
			{
				if (pInterface->clockSource == eDAL_CLK_SRC_INTERNAL)
				{
#ifdef _SYSDEBUG
					sysDebugPrintf("ERROR: Internal rate not locked???\n\r");
#endif //_SYSDEBUG
					dalClockCtrlSetState  (pInterface, eDAL_STATE_WAIT_SRC_LOCK);
				}
				else
				{
					dalDisableHPLL  (pInterface);
					dalClockCtrlSetState  (pInterface, eDAL_STATE_RATE_IS_FLAKY);
					break;
				}
				
			}						
			if (hpllIsLocked(pInterface->ID))
			{
				// Check the source rate
				if (dalGetSamplerate(pInterface))
				{
					//We are ready to transition into the locked state
					pRate = dalRateByID (pInterface->rxPllData.rateID);
					pInterface->actualRateMode = pRate->rateMode; //LOW, MID or HIGH
					if (pInterface->actualNominalRate != pInterface->rxPllData.rateID)
					{
						pInterface->actualNominalRate = pInterface->rxPllData.rateID;
						pInterface->events |= DAL_CB_RATE1_NOM_CHANGED;
	                }
	                if (pInterface->actualRateHz != pInterface->rxPllData.rateHZ)
	                {
						pInterface->actualRateHz = pInterface->rxPllData.rateHZ;
						pInterface->events |= DAL_CB_RATE1_CHANGED;
					}
					dalClockCtrlSetState  (pInterface, eDAL_STATE_LOCKED);
				}
			}
			break;
		
		case eDAL_STATE_ILLEGAL_RATE:
			if (!dalIsInterfaceLocked (pInterface->clockSource))
			{
				dalClockCtrlSetState  (pInterface, eDAL_STATE_WAIT_SRC_LOCK);
				break;
			}
			if (pInterface->ticksInState > TICKS100MS)
			{
				pInterface->ticksInState = 0;
				if (dalGetSamplerate(pInterface))
				{
					dalClockCtrlSetState  (pInterface, eDAL_STATE_CHECK_RATE);
				}
			}			
			break;

		case eDAL_STATE_RATE_IS_FLAKY:
			if (pInterface->ticksInState > TICKS100MS)
				dalClockCtrlSetState  (pInterface, eDAL_STATE_CHECK_RATE);
			break;
			
		case eDAL_STATE_LOCKED:
			if (!dalIsInterfaceLocked (pInterface->clockSource) || !hpllIsLocked(pInterface->ID))
			{
				if (pInterface->clockSource != eDAL_CLK_SRC_INTERNAL) dalDisableHPLL  (pInterface);
				dalClockCtrlSetState  (pInterface, eDAL_STATE_WAIT_SRC_LOCK);
				break;
			}
			if (pInterface->ticksInState > TICKS100MS)
			{
				pInterface->ticksInState = 0;
				if (dalGetSamplerate(pInterface))
				{
					uint32 diff;
					
					// rate is still good, check for changes
					pRate = dalRateByID (pInterface->rxPllData.rateID);
					if (pInterface->actualRateMode != pRate->rateMode)
					{
						//this can happen in LOW_MID mode, we need to unlock to make
						//sure the pll is set correctly.
						//the rate is actually good so we don't disablePLL
						dalClockCtrlSetState  (pInterface, eDAL_STATE_CHECK_RATE);
					}
					if (pInterface->actualNominalRate != pInterface->rxPllData.rateID)
					{						
						pInterface->actualNominalRate = pInterface->rxPllData.rateID;
						pInterface->actualRateHz = pInterface->rxPllData.rateHZ;
						pInterface->events |= DAL_CB_RATE1_NOM_CHANGED;
						pInterface->events |= DAL_CB_RATE1_CHANGED;
	                }
	                else
	                {
						if (pInterface->actualRateHz > pInterface->rxPllData.rateHZ)
							diff = pInterface->actualRateHz - pInterface->rxPllData.rateHZ;
						else
							diff = pInterface->rxPllData.rateHZ - pInterface->actualRateHz;
		                
						if (diff > 200)
						{
							pInterface->actualRateHz = pInterface->rxPllData.rateHZ;
							pInterface->events |= DAL_CB_RATE1_CHANGED;
						}
					}
					
				}
				else
				{
					// rate changed without an unlock
					if (pInterface->clockSource != eDAL_CLK_SRC_INTERNAL) dalDisableHPLL  (pInterface);
					dalClockCtrlSetState  (pInterface, eDAL_STATE_ILLEGAL_RATE);
				}
			}
			break;
					
	}
	pInterface->reqEnable = eDAL_REQ_NONE;
	return NO_ERROR;
}
		


HRESULT dalClockCtrlInitialize (DAL_CLOCKCTRL_INTERFACE * pInterface, DAL_INTERFACE_ID_ENUM interfaceId)
{
	HRESULT hResult = NO_ERROR;
	
	if (!pInterface)
	{
		hResult = E_DAL_BAD_INPUT_PARAM;	
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	memset (pInterface, 0, sizeof(DAL_CLOCKCTRL_INTERFACE));
	pInterface->ID = interfaceId;
	pInterface->state = eDAL_STATE_DISABLED;
	pInterface->rateMode = eDAL_RATE_MODE_LOW;
	pInterface->reqClockSource = eDAL_CLK_SRC_INTERNAL;
	pInterface->reqSampleRate = eDAL_NOMINAL_RATE_48;
	pInterface->actualRateMode = eDAL_RATE_MODE_LOW;
	pInterface->actualNominalRate = eDAL_NOMINAL_RATE_48;
	pInterface->actualRateHz = 48000;
	return hResult;
}

HRESULT dalClockCtrlDisable (DAL_CLOCKCTRL_INTERFACE * pInterface)
{
	HRESULT hResult = NO_ERROR;
	
	if (!pInterface)
	{
		hResult = E_DAL_BAD_INPUT_PARAM;	
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	dalClockCtrlSetState  (pInterface, eDAL_STATE_DISABLED);
	pInterface->reqEnable = eDAL_REQ_NONE;
	dalDisableHPLL  (pInterface);

	return hResult;
}

HRESULT dalClockCtrlEnable (DAL_CLOCKCTRL_INTERFACE * pInterface)
{
	HRESULT hResult = NO_ERROR;
	
	if (!pInterface)
	{
		hResult = E_DAL_BAD_INPUT_PARAM;	
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	pInterface->reqEnable = eDAL_REQ_ENABLE;
	return hResult;
}			

HRESULT dalClockCtrlInputDevices (DAL_CLOCKCTRL_INTERFACE * pInterface, uint32 inputDevices)
{
	HRESULT hResult = NO_ERROR;
	
	if (!pInterface)
	{
		hResult = E_DAL_BAD_INPUT_PARAM;	
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	} 
	if ((pInterface->state != eDAL_STATE_DISABLED) && (pInterface->reqEnable != eDAL_REQ_DISABLE))
	{
		//the interface has not been and is not being disabled
		hResult = E_DAL_CONTEXT_ERROR;		
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	pInterface->inputDevices = inputDevices;	
	
	return hResult;
}
		
HRESULT dalClockCtrlSetRateMode (DAL_CLOCKCTRL_INTERFACE * pInterface, DAL_RATE_MODE_ENUM rateMode)
{
	HRESULT hResult = NO_ERROR;

	if ((rateMode >= eDAL_RATE_MODE_COUNT) || (!pInterface))
	{
		hResult = E_DAL_BAD_INPUT_PARAM;
	}
	else if ((pInterface->state != eDAL_STATE_DISABLED) && (pInterface->reqEnable != eDAL_REQ_DISABLE))
	{
		//the interface has not been and is not being disabled
		hResult = E_DAL_CONTEXT_ERROR;		
	} 
	else
	{
		pInterface->rateMode = rateMode;
	}
	
	if (hResult != NO_ERROR)
	{
		sysLogError(hResult, __LINE__, moduleName);
	}
	return hResult;
}

HRESULT dalClockCtrlSetClockSource (DAL_CLOCKCTRL_INTERFACE * pInterface, DAL_CLOCK_SOURCE_ENUM clockSource, DAL_NOMINAL_RATE_ENUM sampleRate)
{
	HRESULT hResult = NO_ERROR;
	
	if ((clockSource > eDAL_CLK_SRC_INTERNAL) || (sampleRate > eDAL_NOMINAL_RATE_ANY) || (!pInterface))
	{
		hResult = E_DAL_BAD_INPUT_PARAM;
		goto errorReturn;
	}
	if (sampleRate < eDAL_NOMINAL_RATE_ANY)
	{
		// check if sample rate is legal
		RATE_DESC * pRate;
		pRate = dalRateByID (sampleRate);
		if (!pRate)
		{
			hResult = E_DAL_ILLEGAL_NOMINAL_RATE;
			goto errorReturn;
		}
		if (!dalRateModeCompatible (pRate->rateMode, pInterface->rateMode))
		{
			hResult = E_DAL_ILLEGAL_NOMINAL_RATE;
			goto errorReturn;
		}
	}
	else
	{
		if (clockSource == eDAL_CLK_SRC_INTERNAL)
		{
			hResult = E_DAL_ILLEGAL_NOMINAL_RATE;
			goto errorReturn;
		}
	}
	// done with the checking, lets assign it
	pInterface->reqClockSource = clockSource;
	pInterface->reqSampleRate = sampleRate;
	return hResult;
	
errorReturn:	
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

HRESULT dalClockCtrlSetConfig (uint32 msWaitStable, uint32 msWaitLock, BOOL bPLLoffWhenNoSource)
{
	dalClockConfiguration.ticksWaitLock = TICSFROMMS(msWaitLock);
	dalClockConfiguration.ticksWaitStable = TICSFROMMS(msWaitStable);
	dalClockConfiguration.bPLLoffWhenNoSource = bPLLoffWhenNoSource;
	return NO_ERROR;
}

/************************** Information fonctions ***************************/

DAL_NOMINAL_RATE_ENUM dalClockCtrlGetSysNominal (DAL_CLOCKCTRL_INTERFACE * pInterface)
{
	return dalRatesGetSystemNominal (pInterface->actualNominalRate);
}



/*
Check the sync source selected
if internal return always locked and selected rate
if AES return AES locked status and pllInfo rateHz
if ADAT:
	if locked return ref8Hz>>3, actual adatUserbits
	else if synced return ref8Hz>>3, adatUserbits=0
	else return eDAL_NOMINAL_RATE_NONE
if WC/Ext
	return ref8Hz>>3, locked if rate match is found

*/

HRESULT dalClockCtrlGetSyncSrcInfo (DAL_CLOCKCTRL_INTERFACE * pInterface, DAL_SYNC_SRC_INFO * pInfo)
{
	uint8 rxid;
	pInfo->src = pInterface->clockSource;
	pInfo->adatUser = 0x10;
	switch (pInterface->clockSource)
	{
		case eDAL_CLK_SRC_AESRX:
		case eDAL_CLK_SRC_AESRX0:
		case eDAL_CLK_SRC_AESRX1:
		case eDAL_CLK_SRC_AESRX2:
		case eDAL_CLK_SRC_AESRX3:
			pInfo->locked = dalIsInterfaceLocked (pInterface->clockSource);
			pInfo->nomRate = pInterface->rxPllData.rateID;
			SYS_TRACE3(SYSDEBUG_TRACE_GENERAL, pInfo->locked, pInfo->nomRate, pInterface->rxPllData.rateHZ);
			break;
						
		case eDAL_CLK_SRC_ADATRX:
		case eDAL_CLK_SRC_ADATRXAUX:
			pInfo->locked = 0;
			pInfo->nomRate = eDAL_NOMINAL_RATE_NONE;
			rxid = (pInterface->clockSource == eDAL_CLK_SRC_ADATRX) ? 0 : 1;
			if (adatIsSynced(rxid))
			{
				pInfo->locked = 1;
				dalRateFindNearest (&pInfo->nomRate, pInterface->rxPllData.ref8Hz>>3);				
				if (adatIsLocked(rxid))
				{
					adatRxGetUserData(rxid, &pInfo->adatUser);
				}
			}
			break;
					
		case eDAL_CLK_SRC_WORD:
		case eDAL_CLK_SRC_EXTERNAL:
			dalRateFindNearest (&pInfo->nomRate, pInterface->rxPllData.ref8Hz>>3);
			pInfo->locked = (pInfo->nomRate != eDAL_NOMINAL_RATE_NONE);
			break;

		default:
		case eDAL_CLK_SRC_AVS_SYT1:
		case eDAL_CLK_SRC_AVS_SYT2:
		case eDAL_CLK_SRC_INTERNAL:
			pInfo->locked = true;
			pInfo->nomRate = pInterface->actualNominalRate;
			break;
	}
	return NO_ERROR;
}


HRESULT dalClockCtrlUpdateInternalRate (DAL_CLOCKCTRL_INTERFACE * pInterface)
{
	//update if we are using internal rate
	if (pInterface->clockSource == eDAL_CLK_SRC_INTERNAL)
	{
		// update internal rate
		RATE_DESC * pRate = dalRateByID (pInterface->sampleRate);
		if (pRate)
		{
			//we set that right away, it is always good
			clkSetPrescaler(pInterface->ID, pRate->denom);
		}
	}
	return NO_ERROR;
}
