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
	
	Purpose:	myApp.c - Example of a user application using the DiceDriver
	            and dal interfaces.
	            This example creates a simple break-out-box with 8 channels
	            of AES I/O and 8 channels of ADAT I/O
	
	Revisions:
		created 19/03/2005 ml
		

	Use of LED's and Switches in this application
	LED1	Toggle 1Hz when firmware is running
	LED2	Computer is attached and streaming enabled
	LED3	Main clock domain is locked
	LED4	The AES Master device is locked
	LED5	The ADAT Receiver is locked
	LED6	Unused
	LED7	Unused
	LED8	Unused
	
	SW1		On, Modes selected by flash
			Off, Modes selected by SW (see below)
	SW2-4	Off	Off	Off	Mode0
			On	Off	Off	Mode1
			Off	On	Off	Mode2
			On	On	Off	Mode3
			Off	Off On	Mode4
			On	Off	On	Mode5
			Off	On	On	Mode6
			On	On	On	Mode7

****************************************************************************/

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "dal.h"
#include "dalRates.h"
#include "diceDriver.h"
#include "diceRouterReg.h"
#include "coreDefs.h" //for GPIO definition for DIP switch
#include "targetBoard.h"
#include "dicedriverInterface.h"
#include "myApp.h"
#include "myModes.h"
#include "sps.h"

#ifdef _CLI_TOOL_MYAPP
#include "myAppCli.h"
#endif //_CLI_TOOL_MYAPP

#ifdef _AML
#include "aml.h"
#include "avsAml.h"
#include "uartAml.h"
#endif //_AML

#ifdef _SPI
#include "spi.h"
#endif //_SPI

// We need to define how we present ourselves to the PC and Mac driver. We will
// need to specify our Isco channel layout, names for ASIO and CoreAudio etc.
// Starting and stopping the 1394 Isoc streams will be handled by the DiceDriver
// module.
//
// This file uses myModes to select between a number of configurations of the board
// The current mode is stored in the FLASH using SPS. When the board boots it will
// check the switch settings. If SW1 is on it will use the last mode stored in the
// FLASH SPS system otherwise it will use SW2-4 to select between mode0-3. 
//



uint32						myAppThreadID = 0;		
const char*					myAppThreadName = "myAppThread";

uint32						myMeterThreadID = 0;		
const char*					myMeterThreadName = "myMeterThread";



//LEDmode determines if the board use LED for status or metering. Metering is not
//possible with the DICE II, this variable is keept to make the module as close as 
//possible to the DICE Jr/Mini version
static bool LEDmode=0; //if 0 LEDs show status otherwise they show meter

//we need a few other status things to handle the application specific LED stuff
static bool driverAttached;


//need a pointer to the peaks
uint32 * pEntry = (uint32 * )mDicePtr(DICE_ROUTER1_ENTRY);



// The myMode system returns this pointer defining the mode
static MODE_CFG * pCurrentMode;


// The default name our device will appear as under ASIO and CoreAudio
#define MY_DEVICE_NICK_NAME "MyProduct"


// forward declaration
static HRESULT myDiceDriverCallBack (DICE_DRIVER_MSG_ENUM msg, DICE_DRIVER_CB_DATA * pData);

// update the status LED's, called whenever the status affecting the LED's has changed
static void updateStatusLEDs(void);



// This function creates the DAL based on a rateMode. This is typically
// called as a result of the host/driver requesting a rateMode change.
// Under driver comtrol this should not be called except as a result of
// the driver requesting it.
// This function is also called at boot initialization and could be used to 
// initialize a 'stand alone' mode of the device in case no computer is
// connected.
static void myAppCreateDAL(DAL_RATE_MODE_ENUM ratemode)
{
	//Let's build our audio configuration with "dal", We will only allow rates within the window of
	//the nominal rate requested by the driver.
	dalCreateInterface(eDAL_INTERFACE_1, ratemode,pCurrentMode->inputDevices,pCurrentMode->outputDevices);
	
	//Execute the routing for this mode
	pCurrentMode->fRouting(ratemode);

	STREAM_CFG * pCfg;
	switch (ratemode)
	{
	    default:
		case eDAL_RATE_MODE_LOW: pCfg = pCurrentMode->low;break;
		case eDAL_RATE_MODE_MID: pCfg = pCurrentMode->mid;break;
		case eDAL_RATE_MODE_HIGH: pCfg = pCurrentMode->high;break;
	}
	//This might have resulted in a configuration change so we call
	//the diceDriver module to update the configuration
	diceDriverConfigureNew(pCfg->pTxStreamCfg, pCfg->nbTxStreams, 
			               pCfg->pRxStreamCfg, pCfg->nbRxStreams,
			               MY_DEVICE_NICK_NAME,
			               myDiceDriverCallBack,
			               pCurrentMode->clocksCaps,
			               pCurrentMode->clockNames);
	
	//Now we can start the interface
	dalStartInterface(eDAL_INTERFACE_1);
}


//Variable used for checking DAL changes
static DAL_RATE_MODE_ENUM lastDriverRatemode = eDAL_RATE_MODE_LOW;


// This function handles the rate change request from the driver.
// If the rate mode has changed it will create a new DAL.
static HRESULT myAppChangeSR(DICE_DRIVER_CB_DATA * pData)
{
	HRESULT hResult = NO_ERROR;
	uint32 notify = 0;
	DAL_RATE_MODE_ENUM driverRateMode;

	driverRateMode = dalRateByID(pData->rate)->rateMode;
	if (driverRateMode != lastDriverRatemode)
	{
		//Driver asked for a change of rate mode
		lastDriverRatemode = driverRateMode;
	    myAppCreateDAL(driverRateMode);
	    // Send the RX and TX Configuration Notification as this might
	    // have resulted in a change of the Rx and Tx streams going
	    // to the computer. This will for example happen if the device
	    // has ADAT, when going from 48k to 96k ADAT will be reduced
	    // to 4 channels (SMUXII)
		notify |= DD_NOTIFY_RX_CFG_CHG_BIT  | DD_NOTIFY_TX_CFG_CHG_BIT;
	}

	//Change the Clock as instructed by driver, this could be a change
	//in clock source, nominal rate or both.
	dalSetClockSource (eDAL_INTERFACE_1, pData->source, pData->rate);
	
	//Send the Clock Setting Accept Notification, this should always be send
	notify |= DD_NOTIFY_CLOCK_ACP;
	diceDriverNotifyHost(notify);
	return hResult;
}



// The DiceDriver module needs to be able to tell us what is happening. The most 
// important information is the request for a change of clock rate or source.
// The eDDR_MSG_CLOCK_CHG is the only one we are required to handle.
// The other messages are used to show the connection status on some LED's,
// and those are of course application dependent.
static HRESULT myDiceDriverCallBack (DICE_DRIVER_MSG_ENUM msg, DICE_DRIVER_CB_DATA * pData)
{	
	// why did the driver call us?
	switch (msg)
	{
	case eDDR_MSG_CLOCK_CHG:
		// The computer has requested a change of clock and source
		//SYS_TRACE1(SYSDEBUG_TRACE_GENERAL, pData->rate);
		myAppChangeSR(pData);
		break;

	case eDDR_MSG_ATTACH:
		// The computer attached itself to this device
		// this typically happens after the computer boots, but also after
		// bus resets or the first time the driver is installed
		// In this application we don't care			
		//SYS_TRACE1(SYSDEBUG_TRACE_GENERAL, 0);
		break;

	case eDDR_MSG_DETACH:
		// The computer detached itself from this device
		// this typically happens when the computer shuts down
		// or at a bus reset. 
		// In this application we don't care			
		//SYS_TRACE1(SYSDEBUG_TRACE_GENERAL, 0);
		break;

	case eDDR_MSG_ENABLE:
		// The computer has enabled streaming			
		//SYS_TRACE1(SYSDEBUG_TRACE_GENERAL, 0);
		driverAttached = true;
		updateStatusLEDs();
		break;

	case eDDR_MSG_DISABLE:
		// The computer has disabled streaming
		//SYS_TRACE1(SYSDEBUG_TRACE_GENERAL, 0);
		driverAttached = false;
		updateStatusLEDs();
		break;
	case eDDR_MSG_AC3_CHG:
		//AC3 flags for one of the transmitter or receiver has changed
		sysDebugPrintf("AC3 Enable Change\r\n");
		sysDebugPrintf("Stream0: 0x%04x\r\nStream1: 0x%04x\r\n", diceDriverGetState()->RxParameters.IsocReceiver[0].AC3Enable,
																															diceDriverGetState()->RxParameters.IsocReceiver[1].AC3Enable);
		break;
	}
	return NO_ERROR;
}






// These static variables keep track of the changes
static BOOL isLocked;
static DAL_RATE_MODE_ENUM lastLockedMode = eDAL_RATE_MODE_COUNT;


// updateCodec is called whenever DAL tells us that things have changed.
// The function checks if any important stuff changed such as the rateMode
// or the locked state.
// We also handle some appliction specific LED stuff here.
static void updateCodec(DAL_STATUS * pStatus)
{	
	if (((pStatus->state == eDAL_STATE_LOCKED) && !isLocked) || ((pStatus->state == eDAL_STATE_LOCKED) && (pStatus->lockedRateMode != lastLockedMode)))
	{
		//So either we became locked, or we slid into a new rate mode, in all cases
		//the codec/ADC/DAC should be told.
		
		//Call the codec function for the selected mode
		pCurrentMode->fMsg(MM_GOT_LOCK,pStatus->lockedRateMode,0);
		isLocked = true;
		lastLockedMode = pStatus->lockedRateMode;
		updateStatusLEDs();
	}
	else if ((pStatus->state != eDAL_STATE_LOCKED) && isLocked)
	{
		//we just lost lock
		//Call the codec function for the selected mode
		pCurrentMode->fMsg(MM_LOST_LOCK,0,0);
		isLocked = false;
		updateStatusLEDs();
	}
}


// We use PAL notifications to tell us when the status has changed, such
// as receivers being locked/unlocked and sample rate changing. We need
// that information so we can setup Codec's correctly but we also need
// to tell diceDriver so it can tell the computer.
// In order to get those notifications we install a callback into DAL.
// Below is the structure describing which events to subscribe to and the
// actual call back function.


//subscribe to all relevant events
static const DAL_EVENTS myDalSubscribedEvents = 
{
	//state machin events we want to subscribe to
	DAL_CB_LOCK1_AQUIRED | DAL_CB_LOCK1_LOST | DAL_CB_RATE1_NOM_CHANGED | DAL_CB_RATE1_CHANGED | DAL_CB_CREATED1,
	//AES events we want to subscribe to
	DAL_CB_AES0_LOCK_ACQUIRED | DAL_CB_AES0_LOCK_LOST | DAL_CB_AES0_SLIP |DAL_CB_AES0_RPT |
	DAL_CB_AES1_LOCK_ACQUIRED | DAL_CB_AES1_LOCK_LOST | DAL_CB_AES1_SLIP |DAL_CB_AES1_RPT |
	DAL_CB_AES2_LOCK_ACQUIRED | DAL_CB_AES2_LOCK_LOST | DAL_CB_AES2_SLIP |DAL_CB_AES2_RPT |
	DAL_CB_AES3_LOCK_ACQUIRED | DAL_CB_AES3_LOCK_LOST | DAL_CB_AES3_SLIP |DAL_CB_AES3_RPT,
	//ADAT events we want to subscribe to
	DAL_CB_ADAT_LOCK_ACQUIRED | DAL_CB_ADAT_LOCK_LOST,
	//TDIF events to subscribe to
	0,
	//AVS (1394 Receiver) events we want to subscribe to
	DAL_CB_AVS1_SLIP | DAL_CB_AVS1_RPT | DAL_CB_AVS1_LOCK_ACQUIRED | 
	DAL_CB_AVS1_LOCK_LOST | DAL_CB_AVS1_ENABLE | DAL_CB_AVS1_DISABLE |
	DAL_CB_AVS2_SLIP | DAL_CB_AVS2_RPT | DAL_CB_AVS2_LOCK_ACQUIRED | 
	DAL_CB_AVS2_LOCK_LOST | DAL_CB_AVS2_ENABLE | DAL_CB_AVS2_DISABLE
};

// The actual callback function called by DAL
static HRESULT myDalCallBack (DAL_CB_EVENT_MASK events, DAL_EVENTS * pExtEvents, uint32 data)
{
	uint16			aesStatus;
	uint32			extStatus=0; //extended status to the driver
	DAL_STATUS		dalStatus;
	
	// We want to tell the diceDriver about the locked status of the clock domain
	// and sample rate etc.
	
	// let's collect AES/ADAT/AVS interface lock status.
	// We pass this on to the driver. It is not needed for the driver/streaming operation
	// but it can be obtained by control panels to provide visual feedback to the user
	aesGetStatus (&aesStatus);
	if (aesStatus & AES_STAT_LOCK0) extStatus |= DD_EXT_STATUS_AES0_LOCKED;
	if (aesStatus & AES_STAT_LOCK1) extStatus |= DD_EXT_STATUS_AES1_LOCKED;
	if (aesStatus & AES_STAT_LOCK2) extStatus |= DD_EXT_STATUS_AES2_LOCKED;
	if (aesStatus & AES_STAT_LOCK3) extStatus |= DD_EXT_STATUS_AES3_LOCKED;
	
	//let's update the slip information
	if (pExtEvents->aes_events & (DAL_CB_AES0_SLIP | DAL_CB_AES0_RPT)) extStatus |= DD_EXT_STATUS_AES0_SLIP;
	if (pExtEvents->aes_events & (DAL_CB_AES1_SLIP | DAL_CB_AES1_RPT)) extStatus |= DD_EXT_STATUS_AES1_SLIP;
	if (pExtEvents->aes_events & (DAL_CB_AES2_SLIP | DAL_CB_AES2_RPT)) extStatus |= DD_EXT_STATUS_AES2_SLIP;
	if (pExtEvents->aes_events & (DAL_CB_AES3_SLIP | DAL_CB_AES3_RPT)) extStatus |= DD_EXT_STATUS_AES3_SLIP;
	
	//let's collect ADAT interface lock status.	
	if (adatIsLocked()) extStatus |= DD_EXT_STATUS_ADAT_LOCKED;
	//let's update the slip information
	//if (pExtEvents->adat_events & (DAL_CB_ADAT_SLIP | DAL_CB_ADAT_RPT)) extStatus |= DD_EXT_STATUS_ADAT_SLIP;
	
	//let's collect AVS interface lock status (for debuggin purposes)
	if (avsRxIsLocked(AVS_PLUG_ID1)) extStatus |= DD_EXT_STATUS_ARX1_LOCKED;
	if (avsRxIsLocked(AVS_PLUG_ID2)) extStatus |= DD_EXT_STATUS_ARX2_LOCKED;
	//let's update the slip information	
	if (pExtEvents->avs_events & (DAL_CB_AVS1_SLIP | DAL_CB_AVS1_RPT)) extStatus |= DD_EXT_STATUS_ARX1_SLIP;
	if (pExtEvents->avs_events & (DAL_CB_AVS2_SLIP | DAL_CB_AVS2_RPT)) extStatus |= DD_EXT_STATUS_ARX2_SLIP;
	

	// At last the most important thing, get the "dal" status and call diceDriver
	// The diceDriver must know when the device is unlocked and locked so it can perform
	// proper synchronization of streams.
	dalGetCurrentStatus  (eDAL_INTERFACE_1, &dalStatus);
	diceDriverSetStatus  (dalStatus.state == eDAL_STATE_LOCKED, 
						  dalStatus.lockedNominalRate, 
						  dalStatus.lockedRateHz, extStatus);
	
	// And make sure the codecs gets updated, see function defined above.
	updateCodec(&dalStatus);

	// Now some application specific stuff, we do the LED's
	updateStatusLEDs();
	return NO_ERROR;
}

// This function updates the status LED's, this way we can get the status
// back after leaving Meter mode.
static void updateStatusLEDs(void)
{
	if (LEDmode) return;
	targetSetLED (TGT_LED2, driverAttached ? TGT_LED_ON : TGT_LED_OFF);
	targetSetLED (TGT_LED3, isLocked  ? TGT_LED_ON : TGT_LED_OFF);
	targetSetLED (TGT_LED4, aesIsMasterLocked () ? TGT_LED_ON : TGT_LED_OFF);
	targetSetLED (TGT_LED5, adatIsLocked() ? TGT_LED_ON : TGT_LED_OFF);
}

HRESULT myAppSetLEDMode (bool mode)
{
	LEDmode = mode;
	updateStatusLEDs();
	return NO_ERROR;
}

static void myMeterThread(void *dummy)
{	
	DO_FOREVER
	{	
		TCTaskWait(100); //every 100ms
		if (LEDmode)
		{
			targetSetAllLED (0x00/*meterGetLED()*/);
		}
	}
}
		


// This thread just toggles the LED1
static void myAppThread(void *dummy)
{	
	DO_FOREVER
	{	
		TCTaskWait(500);		
		if (!LEDmode)
			targetSetLED (TGT_LED1, TGT_LED_TGL);
	}
}

		



// MIDI is handled through an abstraction layer called AML
// The initialization function below creates all the MIDI
// interfaces for the UART1 and the first two Isoc Rx and
// Tx streams. Each stream has 8 midi interfaces as per the
// AM824 protocol.
//
// The default is that the first port of the first stream
// is routed to and from the UART and the rest are looped
// right back.
// In order to use UART1 for debugging the flag USE_UART1_FOR_MIDI
// (see targetBoard.h) should be undefined. In that case 
// UART1 is not created and the first MIDI port is also 
// looped back.
//
// For testing purposes the first MIDI port can be set 
// to be looped back for testing purposes. This is handled
// through the bypassMIDI function which can be invoked
// through the CLI command >evm.midibp or through the driver
// test interface implemented in frmvload.c
// 
#ifdef _AML

//definitions for all the midi ports
static uint32 uart1AmlRxId, uart1AmlTxId, avsAmlRx1Ids[8], avsAmlTx1Ids[8],avsAmlRx2Ids[8], avsAmlTx2Ids[8];

//This function creates virtual and physical MIDI ports and link them together
static void myAppAMLInitialize(void)
{
	uint32 port = 0;
	char  name[15];
	
	//let's create all the MIDI plug devices for two Isoc channels, 16 in and 16 out
	//These are virtual devices ready to process MIDI input and output but only the
	//ones specified in the mode files are being used so there is no harm in opening
	//all of them.
	for (port=0; port<8; port++)
	{
		sprintf(name,"avsRx1Port%i",port);
		amlCreateDev(&avsAmlRx1Ids[port], &avs_midi_dev_cbs, AML_DEV_RX, AVS_AML_RX1 + (port&0x7), name);
		sprintf(name,"avsTx1Port%i",port);
		amlCreateDev(&avsAmlTx1Ids[port], &avs_midi_dev_cbs, AML_DEV_TX, AVS_AML_TX1 + (port&0x7), name);
		sprintf(name,"avsRx2Port%i",port);
		amlCreateDev(&avsAmlRx2Ids[port], &avs_midi_dev_cbs, AML_DEV_RX, AVS_AML_RX2 + (port&0x7), name);
		sprintf(name,"avsTx2Port%i",port);
		amlCreateDev(&avsAmlTx2Ids[port], &avs_midi_dev_cbs, AML_DEV_TX, AVS_AML_TX2 + (port&0x7), name);		
	}
#ifdef USE_UART1_FOR_MIDI	
	amlCreateDev(&uart1AmlTxId,  &uart1_midi_dev_cbs, AML_DEV_TX, 0, "uart1_tx");
	amlCreateDev(&uart1AmlRxId,  &uart1_midi_dev_cbs, AML_DEV_RX, 0, "uart1_rx");

	//The Physical UART is linked to the first plug of the first isoc.
	amlSetDestination(uart1AmlRxId, avsAmlTx1Ids[0]);
	amlSetDestination(avsAmlRx1Ids[0], uart1AmlTxId);
#else
	//in this case just loop back the midi plug
	amlSetDestination(avsAmlRx1Ids[0], avsAmlTx1Ids[0]);
#endif
    //Now loop all the other plugs
    for (port=1; port<8; port++)
		amlSetDestination(avsAmlRx1Ids[port], avsAmlTx1Ids[port]);

    for (port=0; port<8; port++)
		amlSetDestination(avsAmlRx2Ids[port], avsAmlTx2Ids[port]);

	//start all the devices
	for (port=0; port<8; port++)
	{
		amlStartDev(avsAmlTx1Ids[port]);
		amlStartDev(avsAmlRx1Ids[port]);
		amlStartDev(avsAmlTx2Ids[port]);
		amlStartDev(avsAmlRx2Ids[port]);
	}
#ifdef USE_UART1_FOR_MIDI	
	amlStartDev(uart1AmlTxId);
	amlStartDev(uart1AmlRxId);
#endif
}

#endif

//For testing purposes, this enables a host command to bypass the midi for testing
static void bypassMIDI(BOOL bypass)
{
#ifdef USE_UART1_FOR_MIDI	
	if (bypass)	
	{
		amlSetDestination(avsAmlRx1Ids[0], avsAmlTx1Ids[0]);
		amlSetDestination(uart1AmlRxId, 0);
	}
	else
	{
		amlSetDestination(avsAmlRx1Ids[0], uart1AmlTxId);		
		amlSetDestination(uart1AmlRxId, avsAmlTx1Ids[0]);
	}
#endif
}


// We use SPS to store settings in FLASH. In this example we
// only store the current mode. The structure below can be expanded
// in order to store other settings.
typedef struct
{
	uint32	mode;
	uint32	ac3Out; //ac3 bits lower 16 for first isoc, upper for second
	uint32  reserved[8];
} EVM_CFG;

static EVM_CFG evmCfg; //initialized to all zero


// This function is called when the system is initializing. At this point no threads are running and no audio is
// passed. After this function returns all threads are started. It is not legal to wait on semaphores etc at this
// point in the initialization sequence.
HRESULT	myAppInitialize(void)
{
	HRESULT hResult = NO_ERROR;	
	
	// Initialize the SPI devices (targetBoard.c)
	targetSpiInit();
	// Show the initial switch settings
	uint32 sw = targetGetInitialSW ();
	sysDebugPrintf("Initial SW Settings\r\n");
	sysDebugPrintf(" 1  2  3  4\r\n");
	sysDebugPrintf("[%c][%c][%c][%c]\r\n",(sw&1)?'1':'0',(sw&2)?'1':'0',(sw&4)?'1':'0',(sw&8)?'1':'0');
	
	// Initialize the mode based on switch settings
	uint32 mode = (sw >> 1) & 0x07;

	//Initialize the persistent storage system and check the stored mode
	hResult = spsInitialize ("evm_sps", 2, sizeof(evmCfg), &evmCfg, 100);
	if (hResult!=NO_ERROR)
	{
		sysLogError(hResult, __LINE__, __FILE__);
	}
	if (sw&1) //use Flash for current mode
	{
		sysDebugPrintf("Using Flash based mode\r\n");

		mode = evmCfg.mode;
	}
	else
		sysDebugPrintf("Using Switch based mode\r\n");

	//Initialize the mode abstraction system
	pCurrentMode = myModesInitialize (mode, evmCfg.ac3Out);
	
	//create the call back from DAL
	dalInstallCallback(&myDalSubscribedEvents, 200, myDalCallBack, 0);
	
	//Create Low Rate DAL for default initialization
	myAppCreateDAL(eDAL_RATE_MODE_LOW);

#ifdef _AML
	myAppAMLInitialize();
#endif

	//Set flag for creation of DAL
	// After application initialization is done resume tasks to start them
	hResult = myAppResumeTasks();
	if (hResult != NO_ERROR) return hResult;
	// If the developer wishes to add Command Line Interface, define _CLI_TOOL_MYAPP in make.params
#ifdef _CLI
#ifdef _CLI_TOOL_MYAPP
	myAppCliInstallTools();
#endif //_CLI_TOOL_MYAPP
#endif // _CLI
	return hResult;
}


HRESULT myAppInitializeTasks(void)
{	
	HRESULT hResult = NO_ERROR;
	
	hResult = TCTaskCreate(&myAppThreadID, myAppThreadName, myAppThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	hResult = TCTaskCreate(&myMeterThreadID, myMeterThreadName, myMeterThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT myAppResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;


	hResult = TCTaskResume(myAppThreadID);
	if (hResult != NO_ERROR) return hResult;
	hResult = TCTaskResume(myMeterThreadID);	
	if (hResult != NO_ERROR) return hResult;
	
	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT myAppSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = TCTaskSuspend(myAppThreadID);
	if (hResult != NO_ERROR) return hResult;

	hResult = TCTaskSuspend(myMeterThreadID);
	if (hResult != NO_ERROR) return hResult;
	
	return hResult;
}

// This function can dynamically change the configuration of the streams
// it is currently called from an application specific CLI command and
// or through the driver test interface implemented in frmvload.c
// This function recall's a new mode and invokes it by notifying the driver

HRESULT	myAppChangeConfig(uint32 mode)
{
	evmCfg.mode = mode;
	spsChanged (); //store to flash
	pCurrentMode = myModesInitialize (mode, evmCfg.ac3Out);

	myAppCreateDAL(lastDriverRatemode);
	diceDriverNotifyHost(DD_NOTIFY_RX_CFG_CHG_BIT  | DD_NOTIFY_TX_CFG_CHG_BIT);	
	return NO_ERROR;
}

// This function can dynamically change the AC3 configuration for modes 6 and 7
// For other modes this has no effect except that the driver is notified of a change

HRESULT	myAppChangeAC3(uint32 isoc, uint32 flags)
{
	if (!isoc)
	{
		evmCfg.ac3Out = (evmCfg.ac3Out & 0xffff0000) | (flags & 0xffff);
	}
	else
	{
		evmCfg.ac3Out = (evmCfg.ac3Out & 0x0000ffff) | ((flags & 0xffff)<<16);
	}
	return myAppChangeConfig(evmCfg.mode);
}


// Called from the driver test interface implemented in frmvload.c
// If your application does not inplement modes just leave this
// function empty and return NO_ERROR; This function must be
// defined otherwise a link error will be generated.

HRESULT myModeSetMode (uint32 mode)
{
	//the special 'firmwareLoader' command is setting the mode
	myAppChangeConfig(mode);
	return NO_ERROR;
}

// Called from the driver test interface implemented in frmvload.c
// It is highly recomended that this function is implemented as
// it allows some test diagnostic tools to run on the host computer.
// This function must be defined otherwise a link error will be generated.
//
HRESULT myModeBypassMidi (uint32 bOn)
{
	bypassMIDI(bOn?1:0);
	return NO_ERROR;
}
