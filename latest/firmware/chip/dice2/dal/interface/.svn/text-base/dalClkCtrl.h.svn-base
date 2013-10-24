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
//===========================================================================
/****************************************************************************
	
	Purpose:	DICE  clock controller API
	
	Revisions:
		created 09/03/2005 ML

****************************************************************************/

#ifndef _DALCLKCTRL_H
#define _DALCLKCTRL_H


#include "dal.h"

#define DAL_TICK_MS	20

// Internal state machine states, find description of the state and their transitions below.

/* Defined in dal.h to allow external access
typedef enum 
{
	eDAL_STATE_DISABLED = 0,
	eDAL_STATE_SETUP_SRC,
	eDAL_STATE_WAIT_SRC_LOCK,
	eDAL_STATE_WAIT_RATE_STABLE,
	eDAL_STATE_CHECK_RATE,
	eDAL_STATE_LOCKING,
	eDAL_STATE_ILLEGAL_RATE,
	eDAL_STATE_RATE_IS_FLAKY,
	eDAL_STATE_LOCKED
} DAL_STATE_ENUM;
*/

// Structure used by clock source Rx interface

typedef struct
{
	uint32					ref8Hz;		// counted reference in eights of Hz, filled out before call
	DAL_RATE_MODE_ENUM		rateMode;	// interface rate mode, filled out before call
	uint32					rateHZ;		// calculated rate, filled out by function	
	DAL_NOMINAL_RATE_ENUM	rateID;		// ID of the detected rate found by function, contains current at call
	uint32					preDiv;		// pre-divider setting filled out by function
	uint32					addDiv;		// feed back divider, filled out by function
} DAL_RX_PLL_DATA;


// Enumeration type used for requests to the state machine.
typedef enum
{
	eDAL_REQ_NONE = 0,
	eDAL_REQ_ENABLE,
	eDAL_REQ_DISABLE
} DAL_CLOCK_REQ;

typedef struct tDAL_CLOCKCTRL_INTERFACE
{
	//Current state
	DAL_INTERFACE_ID_ENUM	ID;				// Id of the interface, 
	DAL_STATE_ENUM			state;			// current state of the FSM
	uint32					ticksInState;	// used for sub state processing
	DAL_RATE_MODE_ENUM		rateMode;		// current rateMode selection
	DAL_CLOCK_SOURCE_ENUM	clockSource;	// current clock source selected
	DAL_NOMINAL_RATE_ENUM	sampleRate;		// current sample rate selected
	uint8					events;			// collects the events for this tick

	//Requested state, actual change will be done by FSM
	DAL_CLOCK_SOURCE_ENUM	reqClockSource;	// will be effective at next FSM tick
	DAL_NOMINAL_RATE_ENUM	reqSampleRate;	// will be effective at next FSM tick
	DAL_CLOCK_REQ			reqEnable;		// asynchronous request to FSM
	
	//Actual detected status of the interface
	DAL_RATE_MODE_ENUM		actualRateMode;		// LOW, MID or HIGH, updated in locked state only
	DAL_NOMINAL_RATE_ENUM	actualNominalRate;	// nominal rate, updated in locked state only
	uint32					actualRateHz;		// rate in Hz, updated in locked state only
	DAL_RX_PLL_DATA			rxPllData;	        // Last PLL and measurement data obtained from interface
	
	//Input membership
	uint32					inputDevices;		// Needs to know if AESRX is a member
} DAL_CLOCKCTRL_INTERFACE;





// dalClockCtrlInitialize 
// Description:
//  Initialize the clock controller for a specific domain specified by interfaceId. This
//  function should only be called once. It will initialize the corresponding HPLL to be
//  in either fixed rate mode or off mode depending on the preferences.
//
//	This function is an internal DAL function and should not be called from outside DAL.  
//  
// Parameters:
//	pInterface		- Pointer to structure holding the configuration.
//  interfaceId		- The interface to initialize
//
// Returns:
//  NO_ERROR                        - no errors occured
//  E_DAL_INTERFACE_NOT_DEFINED     - dalInterfaceId is not a valid interface id.
//	E_DAL_BAD_INPUT_PARAM			- input parameter out of range.
//  
//
// 
HRESULT dalClockCtrlInitialize (DAL_CLOCKCTRL_INTERFACE * pInterface, DAL_INTERFACE_ID_ENUM interfaceId);

// dalClockCtrlDisable 
// Description:
//  Disable the interface identified by pInterface. If the interface is already in the
//  disabled state this function will have no effect.
//  If the interface is enabled it will be disabled and the HPLL will be set to the
//  disabled state.
//
// Parameters:
//	pInterface		- Pointer to structure holding the configuration.
//
// Returns:
//  NO_ERROR                        - no errors occured
//	E_DAL_BAD_INPUT_PARAM			- input parameter out of range.
//  
//
// 
HRESULT dalClockCtrlDisable (DAL_CLOCKCTRL_INTERFACE * pInterface);


// dalClockCtrlEnable 
// Description:
//  Enable the interface identified by pInterface. If the interface is already in the
//  enabled state this function will have no effect.
//  If the interface is disabled this function will start the state machine and try
//  to acquire lock to the selected source. If the interface rateMode is set to one
//	of the slave modes the master interface must be enabled.
//
// Parameters:
//	pInterface		- Pointer to structure holding the configuration.
//
// Returns:
//  NO_ERROR                        - no errors occured
//	E_DAL_BAD_INPUT_PARAM			- input parameter out of range.
//
// 
HRESULT dalClockCtrlEnable (DAL_CLOCKCTRL_INTERFACE * pInterface);

// dalClockCtrlInputDevices 
// Description:
//  Tell the clock controller which input devices are members of this domain
//  This information is needed in order to handle the source selection of the
//  AES receiver.
//  This call is only leagal if the interface is disabled.
//
// Parameters:
//	pInterface		- Pointer to structure holding the configuration.
//	inputDevices	- bit field indicating device membership
//
// Returns:
//  NO_ERROR                        - no errors occured
//	E_DAL_CONTEXT_ERROR				- illegal call while interface is enabled
//	E_DAL_BAD_INPUT_PARAM			- input parameter out of range.
//
// 
HRESULT dalClockCtrlInputDevices (DAL_CLOCKCTRL_INTERFACE * pInterface, uint32 inputDevices);

// dalClockCtrlSetRateMode 
// Description:
//	Set the rate mode used by this interface.
//  This call is only leagal if the interface is disabled.
//
// Parameters:
//	pInterface		- Pointer to structure holding the configuration.
//	rateMode		- The rate mode for this interface
//
// Returns:
//  NO_ERROR                        - no errors occured
//	E_DAL_CONTEXT_ERROR				- illegal call while interface is enabled
//	E_DAL_BAD_INPUT_PARAM			- input parameter out of range.
// 
HRESULT dalClockCtrlSetRateMode (DAL_CLOCKCTRL_INTERFACE * pInterface, DAL_RATE_MODE_ENUM rateMode);


// dalClockCtrlSetClockSource 
// Description:
//	Sets the clock source for this interface.
//  The clock source must be valid in relation to the selected rate mode.
//	If the source is set to eDAL_CLK_SRC_INTERNAL the specified rate can
//	not be eDAL_NOMINAL_RATE_ANY.
//
// Parameters:
//	pInterface		- Pointer to structure holding the configuration.
//	clockSource		- The clock source to select for this interface
//	sampleRate		- The nominal sample tate to select for this interface
//
// Returns:
//  NO_ERROR                        - no errors occured
//	E_DAL_BAD_INPUT_PARAM			- input parameter out of range.
//	E_DAL_ILLEGAL_NOMINAL_RATE		- nominal rate not legal in specific configuration
// 
HRESULT dalClockCtrlSetClockSource (DAL_CLOCKCTRL_INTERFACE * pInterface, DAL_CLOCK_SOURCE_ENUM clockSource, DAL_NOMINAL_RATE_ENUM sampleRate);


// dalClockCtrlFSM
// Description:
//	This call drives the clock state FSM. This should be called every DAL_TICK_MS
//  milliseconds.
//	The calling function should inspect the 'event' member of the interface
//	structure and notify accordingly.
//
// Parameters:
//	pInterface			- Pointer to structure holding the configuration.
//	pMasterInterface	- If slave configuration is used this parameter must
//						  point to a valid initialized interface.
//
// Returns:
//  NO_ERROR                        - no errors occured
// 
HRESULT dalClockCtrlFSM  (DAL_CLOCKCTRL_INTERFACE * pInterface, DAL_CLOCKCTRL_INTERFACE * pMasterInterface);


/*
The clock control FSM makes transitions between states based on the change in settings and the state of
the clock source.
All states will transition to eDAL_STATE_DISABLED if the interface is disabled and to eDAL_STATE_SETUP_SRC
if the source parameters are changed.

eDAL_STATE_DISABLED:
	This is the initial state, a call to dalClockCtrlEnable will make a transition to
	eDAL_STATE_SETUP_SRC.
	
eDAL_STATE_SETUP_SRC:
	This is a transitional state used when clock source parameters are changing. This
	state is always followed by eDAL_STATE_WAIT_SRC_LOCK (unless interface is disabled
	or clock parameters are changing again in which case it will either transition
	to disabled or reenter eDAL_STATE_SETUP_SRC.
	Unless the rate is INTERNAL the HPLL will disabled (behaviour depends on user setting)
	
	
eDAL_STATE_WAIT_SRC_LOCK:
	This state monitors the source until it is locked. If locked it will transition to 
	eDAL_CHECK_RATE. It will stay in the current state until either the source is locked or
	the interface is disabled or clock parameters are changing.
	
eDAL_STATE_WAIT_RATE_STABLE:
	This state will wait a while enabling the rate to become stable. It will monitor the 
	state of the source and	if the source becomes unlocked it will transition to 
	eDAL_STATE_WAIT_SRC_LOCK. After waiting it will transition to eDAL_STATE_CHECK_RATE.

eDAL_STATE_CHECK_RATE:
	This state will count the rate of the source. It will monitor the state of the source and
	if the source becomes unlocked it will transition to eDAL_STATE_WAIT_SRC_LOCK. When a stable
	count is obtained it will check it against the rateMode and the nominal_rate to determine if
	it is valid. If an invalid rate is detected it will transition to eDAL_STATE_ILLEGAL_RATE. If a valid 
	rate is detected it will unforce the HPLL and transition to eDAL_STATE_LOCKING.
	
eDAL_STATE_LOCKING:
	This state will wait a finite amount of time for the HPLL to lock. As the rate is counted it is 
	expected to lock quickly. It will monitor the state of the source and if the source becomes unlocked 
	it will transition to eDAL_STATE_WAIT_SRC_LOCK. It will also check the rate to determine if it is
	still the same as obtained by the previous state. If the rate changed it will transition back to 
	eDAL_STATE_CHECK_RATE.
	If the HPLL is not locked after when this state times out it will transition to eDAL_STATE_RATE_IS_FLAKY. 
	If it is locked it will transition to eDAL_STATE_LOCKED.
	Any transition out of this state except for eDAL_STATE_LOCKED will result in the PLL being forced.
		
	
eDAL_STATE_ILLEGAL_RATE:
	This state will continously check if the rate has become valid. In the case the rate becomes valid 
	it will transition to eDAL_STATE_CHECK_RATE. If the source becomes unlocked it will transition to 
	eDAL_STATE_WAIT_SRC_LOCK.
	
eDAL_STATE_RATE_IS_FLAKY:
	This state will wait for a predetermined period and then transition to eDAL_STATE_CHECK_RATE for
	another try. The intention of this state is to make sure we don't waste too much time trying to lock to 
	a bad source.
	
eDAL_STATE_LOCKED:
	This is the locked state. This state will monitor warious states of the source:
	If source lock is lost it will transition to eDAL_STATE_WAIT_SRC_LOCK.
	If the rate becomes illegal it will transition to eDAL_ILLEGAL_RATE.
	This state will monitor the rate every 100ms and will report the following
	events if applicable:
	DAL_CB_RATE1_NOM_CHANGED
    DAL_CB_RATE1_CHANGED


The concept of "forcing" the HPLL.
	Before the source is counted to be valid the HPLL is forced. When the rate is counted to be
	within legal range the HPLL will actually be unforced. The force function can optionally
	turn the HPLL clock off instead of forcing it to a valid clock in the current selected
	rate mode.
	The PLL is unforced when entering eDAL_STATE_LOCKING and will be disabled in any state except
	eDAL_STATE_LOCKING and eDAL_STATE_LOCKED.

The event notifications are sent on the following conditions:
	DAL_CB_LOCK1_AQUIRED
		Sent whenever the eDAL_LOCKED state is reached
		
	DAL_CB_LOCK1_LOST
		Sent whenever the eDAL_LOCKED state is left
		
	DAL_CB_RATE1_REJECTED
		Sent whenever the eDAL_ILLEGAL_RATE is reached
		
	DAL_CB_RATE1_NOM_CHANGED
		Only sent in eDAL_LOCKED state and only if the nominal rate has changed.
		
	DAL_CB_RATE1_CHANGED
		Only sent in eDAL_LOCKED state and only if the rate has changed more than 10Hz.


Procedure for handling rate detection
	When loosing lock the following options are available:
		If nominalRate is specific set PLL to generate nominal
		If nominal rate is any set PLL to generate last nominal if valid
		  if not valid set default
		  
		  

*/

DAL_NOMINAL_RATE_ENUM dalClockCtrlGetSysNominal (DAL_CLOCKCTRL_INTERFACE * pInterface);

HRESULT dalClockCtrlGetSyncSrcInfo (DAL_CLOCKCTRL_INTERFACE * pInterface, DAL_SYNC_SRC_INFO * pInfo);

HRESULT dalClockCtrlSetConfig (uint32 msWaitStable, uint32 msWaitLock, BOOL bPLLoffWhenNoSource);

HRESULT dalClockCtrlUpdateInternalRate (DAL_CLOCKCTRL_INTERFACE * pInterface);

#endif // _DALCLKCTRL_H

