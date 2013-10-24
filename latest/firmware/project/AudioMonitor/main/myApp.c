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
	            This example creates a simple audio monitor for the DICE MINI
	            Audio Monitor target board. It has 2 channels of Headphone out
	            and a stereo SPDIF input.
	
	Revisions:
		created 10/11/2008 ml
		
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
#include "display.h"
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



uint32						myAppThreadID = 0;		
const char*					myAppThreadName = "myAppThread";

uint32						myMidiIndicateThreadID = 0;		
const char*					myMidiIndicateThreadName = "myMidiIndicateThread";


//we need a few other status things to handle the application specific LED stuff
static bool driverAttached;



// The myMode system returns this pointer defining the mode, on this board the mode is
// static.
static MODE_CFG * pCurrentMode;

//persistent storage
EVM_CFG evmCfg; //initialized to all zero, persistent storage of HP gain


// The default name our device will appear as under ASIO and CoreAudio
#define MY_DEVICE_NICK_NAME "AudioMonitor"

//gain table for the AKM4384 codec
static const uint8 gainTable[39] = {
0	,1	,2	,3	,4	,5	,6	,7	,
8	,9	,10	,11	,13	,14	,16	,18	,
20	,23	,26	,29	,32	,36	,40	,45	,
51	,57	,64	,72	,81	,90	,102	,
114	,128,143,161,181,203,227	,
255};

#define DAC_GAIN (evmCfg.mute?0:gainTable[evmCfg.hpgain])



// forward declaration
static HRESULT myDiceDriverCallBack (DICE_DRIVER_MSG_ENUM msg, DICE_DRIVER_CB_DATA * pData);
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
		pCurrentMode->fMsg(MM_SET_OUTGAIN,DAC_GAIN,0);
		isLocked = true;
		lastLockedMode = pStatus->lockedRateMode;
	}
	else if ((pStatus->state != eDAL_STATE_LOCKED) && isLocked)
	{
		//we just lost lock
		//Call the codec function for the selected mode
		pCurrentMode->fMsg(MM_LOST_LOCK,0,0);
		isLocked = false;
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
	if (adatIsLocked(0)) extStatus |= DD_EXT_STATUS_ADAT_LOCKED;
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
	amDisplayLed (3,isLocked);
	amDisplayLed (4,driverAttached);
}



/*********************************************************
	GUI state machine
**********************************************************/

static void setDispValue (uint8 val)
{
	amDisplaySetDigit0(val%10);
    amDisplaySetDigit1(val/10);
}

static void updateUI (void)
{
	if (evmCfg.mute)
	{
		amDisplaySetDigit0(AM_DISPLAY_VAL_DASH);
		amDisplaySetDigit1(AM_DISPLAY_VAL_DASH);
	}
	else
		setDispValue (evmCfg.hpgain);
}		  

static void uiFSMdo (void)
{ 
	int delta;
	uint8 chg=0;
	
	if (displayClicked ())
	{
		//toggle mute
		evmCfg.mute = !evmCfg.mute;
		chg=1;		
	}
	delta = displayEncoderDelta ();
	if (delta) 
	{
		//if muted, exit mute
		if (evmCfg.mute)
		{
			evmCfg.mute = 0;
			chg=1;
		}
		delta += evmCfg.hpgain;
		if (delta < 0) delta=0;
		if (delta > 38) delta=38;
		if (delta != evmCfg.hpgain)
		{
			evmCfg.hpgain = delta;
			chg=1;
		}
	}
	if (chg)
	{
		updateUI();
		//tell CODEC
		pCurrentMode->fMsg(MM_SET_OUTGAIN,DAC_GAIN,0);
		spsChanged(); //schedule update of flash				
	}
}



// This thread drives the GUI statemachine
static void myAppThread(void *dummy)
{	
	updateUI ();
	DO_FOREVER
	{	
		TCTaskWait(30);	
		uiFSMdo();	
	}
}


// This thread handles the MIDI indicate LED's
// It also makes sure that the codec gets initialized.
static void myMidiIndicateThread(void *dummy)
{	
	//make sure update codec get's called if we already have lock
	DAL_STATUS		dalStatus;
	dalGetCurrentStatus  (eDAL_INTERFACE_1, &dalStatus);
	updateCodec(&dalStatus);

	DO_FOREVER
	{	
		TCTaskWait(200); //every 200ms
		uint8 act = uart1_activity ();
		amDisplayLed (1,act&UART_RX_ACTIVITY);
		amDisplayLed (2,act&UART_TX_ACTIVITY);
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





// This function is called when the system is initializing. At this point no threads are running and no audio is
// passed. After this function returns all threads are started. It is not legal to wait on semaphores etc at this
// point in the initialization sequence.
HRESULT	myAppInitialize(void)
{
	HRESULT hResult = NO_ERROR;	
	
	// Initialize the SPI devices (targetBoard.c)
	targetSpiInit();
	// Show the initial switch settings
	
	
	//Initialize the persistent storage system and check the stored mode
	hResult = spsInitialize ("am_sps", 2, sizeof(evmCfg), &evmCfg, 100);
	if (hResult!=NO_ERROR)
	{
		sysLogError(hResult, __LINE__, __FILE__);
	}
	if (evmCfg.hpgain > 38) evmCfg.hpgain = 38;

	//Initialize the mode abstraction system
	pCurrentMode = &modeCfg0_Mini;
	pCurrentMode->fInit(0);
	
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
#ifdef _CLI_TOOL_MYAPP
	myAppCliInstallTools();
#endif //_CLI_TOOL_MYAPP
	return hResult;
}


HRESULT myAppInitializeTasks(void)
{	
	HRESULT hResult = NO_ERROR;
	
	hResult = TCTaskCreate(&myAppThreadID, myAppThreadName, myAppThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	hResult = TCTaskCreate(&myMidiIndicateThreadID, myMidiIndicateThreadName, myMidiIndicateThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
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
	hResult = TCTaskResume(myMidiIndicateThreadID);	
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

	hResult = TCTaskSuspend(myMidiIndicateThreadID);
	if (hResult != NO_ERROR) return hResult;
	
	return hResult;
}

// This function can dynamically change the configuration of the streams
// it is currently called from an application specific CLI command and
// or through the driver test interface implemented in frmvload.c
// This function recall's a new mode and invokes it by notifying the driver

HRESULT	myAppChangeConfig(uint32 mode)
{
	return NO_ERROR;
}



// Called from the driver test interface implemented in frmvload.c
// If your application does not inplement modes just leave this
// function empty and return NO_ERROR; This function must be
// defined otherwise a link error will be generated.

HRESULT myModeSetMode (uint32 mode)
{
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
