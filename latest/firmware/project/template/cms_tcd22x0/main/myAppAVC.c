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
	
	Purpose:	myAppAVC.c - Example of a user application using the AV/C
	            and dal interfaces.
	
	Revisions:
		created 12/20/2005 bk
		

****************************************************************************/
#ifdef _AVC

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "dal.h"
#include "dalRates.h"
#include "avcDriver.h"
#include "diceRouterReg.h"
#include "coreDefs.h" //for GPIO definition for DIP switch
#include "targetBoard.h"
#include "avcDriverInterface.h"
#include "myApp.h"
#include "diceInS.h"
#include "myModes.h"

#include "avcAudio.h"
#include "avcMusic.h"

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
// need to specify our Isco channel layout, etc.
// Starting and stopping the 1394 Isoc streams will be handled by the AvcDriver
// module.
//
// This file uses myModes to select between a number of configurations of the board


uint32						myAppThreadID = 0;
const char*					myAppThreadName = "myAppThread";

uint32						myMeterThreadID = 0;		
const char*					myMeterThreadName = "myMeterThread";



//SW1 determines if the board use LED for status or metering
static bool bLEDMeterMode=0; //if 0 LEDs show status otherwise they show meter
static uint16 meterRouterIndex; //we need to find the router index with the meters
static bool isEnabled = FALSE;
static bool isAttached = FALSE;

//need a pointer to the peaks
uint32 * pEntry = (uint32 * )mDicePtr(DICE_ROUTER1_ENTRY);


static void findMeters(void)
{
	uROUTER_SETUP * pSetup = (uROUTER_SETUP * )mDicePtr(DICE_ROUTER1_SETUP);
	uint16 i=0;
	
	while (i <=pSetup->bit.numEntry)
	{
//		if ((pEntry[i] & 0xff00) == 0xb000)	// AVS1 in ch 0
		if ((pEntry[i] & 0xff00) == 0x4000)	// INS0 in ch 0
		{
			meterRouterIndex = i;
			return;
		}
		i++;
	}
	meterRouterIndex = 0;
}

// The myMode system returns this pointer defining the mode
static MODE_CFG * pCurrentMode;


// The default name our device will appear as under ASIO and CoreAudio
#define MY_DEVICE_NICK_NAME "MyProduct"


// forward declaration
static HRESULT myAvcDriverCallBack (AVC_DRIVER_MSG_ENUM msg, AVC_DRIVER_CB_DATA * pData);

// This function creates the DAL based on a rateMode. This is typically
// called as a result of the host/driver requesting a rateMode change
// it can also be called asynchronously but the driver should then be notified
// of the change.
// This function is also called at boot initialization

static void myAppCreateDAL(DAL_RATE_MODE_ENUM ratemode)
{
	//Let's build our audio configuration with "dal", We want to allow all rates from 32KHz to 96KHz
	dalCreateInterface(eDAL_INTERFACE_1, ratemode,pCurrentMode->inputDevices,pCurrentMode->outputDevices);
	pCurrentMode->fRouting(ratemode);
	findMeters();
	STREAM_CFG * pCfg;
	switch (ratemode)
	{
	    default:
		case eDAL_RATE_MODE_LOW: pCfg = pCurrentMode->low;break;
		case eDAL_RATE_MODE_MID: pCfg = pCurrentMode->mid;break;
		case eDAL_RATE_MODE_HIGH: pCfg = pCurrentMode->high;break;
	}
	avcDriverConfigureNew(pCfg->pTxStreamCfg, pCfg->nbTxStreams, 
			               pCfg->pRxStreamCfg, pCfg->nbRxStreams,
			               MY_DEVICE_NICK_NAME,
			               myAvcDriverCallBack,
			               pCurrentMode->clocksCaps,
			               pCurrentMode->clockNames);
	dalStartInterface(eDAL_INTERFACE_1);
}


//Variable used for checking DAL changes
static DAL_RATE_MODE_ENUM lastDriverRatemode = eDAL_RATE_MODE_LOW;

//Variable to make sure we reset the nominal rate on async mode changes.
static DAL_NOMINAL_RATE_ENUM lastDriverNominalRate = eDAL_NOMINAL_RATE_48;

static HRESULT myAppChangeSR(AVC_DRIVER_CB_DATA * pData)
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
	    //Send the RX and TX Configuration Notification
		notify |= DD_NOTIFY_RX_CFG_CHG_BIT  | DD_NOTIFY_TX_CFG_CHG_BIT;
	}

	//Change the Clock as instructed by driver
	lastDriverNominalRate = pData->rate;
	dalSetClockSource (eDAL_INTERFACE_1, pData->source, pData->rate);
	//Send the Clock Setting Accept Notification
	notify |= DD_NOTIFY_CLOCK_ACP;
	return hResult;
}

// The AvcDriver module does not know which clock domain we want to use and how we intend
// to handle clock rate and source change. It will have to call us to do that work. The
// call back function defined below takes care of that.

HRESULT myAvcDriverCallBack (AVC_DRIVER_MSG_ENUM msg, AVC_DRIVER_CB_DATA * pData)
{	
	// why did the driver call us?
	switch (msg)
	{
	case eAVCDR_MSG_CLOCK_CHG:
		// The computer has requested a change of clock and source
		//SYS_TRACE1(SYSDEBUG_TRACE_GENERAL, pData->rate);
		myAppChangeSR(pData);
		break;

	case eAVCDR_MSG_ATTACH:
		// The computer attached itself to this device
		// this typically happens after the computer boots, but also after
		// bus resets or the first time the driver is installed
		// In this application we don't care			
		//SYS_TRACE1(SYSDEBUG_TRACE_GENERAL, 0);
		isAttached = TRUE;
		break;

	case eAVCDR_MSG_DETACH:
		// The computer detached itself from this device
		// this typically happens when the computer shuts down
		// or at a bus reset. 
		// In this application we don't care			
		//SYS_TRACE1(SYSDEBUG_TRACE_GENERAL, 0);
		isAttached = FALSE;
		break;

	case eAVCDR_MSG_ENABLE:
		// The computer has enabled streaming			
		//SYS_TRACE1(SYSDEBUG_TRACE_GENERAL, 0);
		isEnabled = TRUE;
		isAttached = TRUE;
		break;

	case eAVCDR_MSG_DISABLE:
		// The computer has disabled streaming
		//SYS_TRACE1(SYSDEBUG_TRACE_GENERAL, 0);
		isEnabled = FALSE;
		isAttached = FALSE;
		break;
	case eAVCDR_MSG_AC3_CHG:
		//AC3 flags for one of the transmitter or receiver has changed
		break;
	}
	return NO_ERROR;
}




/*
We need to keep track of unlock/lock/ratemode changes so we can notify codec, setup I2S etc
*/

static BOOL isLocked;
static DAL_RATE_MODE_ENUM lastLockedMode = eDAL_RATE_MODE_COUNT;

static void updateCodec(DAL_STATUS * pStatus)
{	
	if (((pStatus->state == eDAL_STATE_LOCKED) && !isLocked) || ((pStatus->state == eDAL_STATE_LOCKED) && (pStatus->lockedRateMode != lastLockedMode)))
	{
		//we just obtained lock or the rate mode changed and we stayed in lock
		pCurrentMode->fMsg(MM_GOT_LOCK,pStatus->lockedRateMode,0);
		isLocked = true;
		lastLockedMode = pStatus->lockedRateMode;
	}
	else if ((pStatus->state != eDAL_STATE_LOCKED) && isLocked)
	{
		//we just lost lock
		pCurrentMode->fMsg(MM_LOST_LOCK,0,0);
		isLocked = false;
	}
}


// The AvcDriver module also needs to know when the status of "dal" is 
// changing so it can adjust streams accordingly. We will install a callback 
// function in "dal" to notify us of such changes this callback is also used 
// to notify codec's so they can react to rate changes, lock and unlock

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
	DAL_CB_ADAT_LOCK_ACQUIRED | DAL_CB_ADAT_LOCK_LOST | DAL_CB_ADAT_AUX_LOCK_ACQUIRED | DAL_CB_ADAT_AUX_LOCK_LOST,
	DAL_CB_AVS1_SLIP | DAL_CB_AVS1_RPT | DAL_CB_AVS1_LOCK_ACQUIRED | 
	DAL_CB_AVS1_LOCK_LOST | DAL_CB_AVS1_ENABLE | DAL_CB_AVS1_DISABLE |
	DAL_CB_AVS2_SLIP | DAL_CB_AVS2_RPT | DAL_CB_AVS2_LOCK_ACQUIRED | 
	DAL_CB_AVS2_LOCK_LOST | DAL_CB_AVS2_ENABLE | DAL_CB_AVS2_DISABLE
};

static HRESULT myDalCallBack (DAL_CB_EVENT_MASK events, DAL_EVENTS * pExtEvents, uint32 data)
{
	uint16			aesStatus;
	uint32			extStatus=0; //extended status to the driver
	DAL_STATUS		dalStatus;
	
	// We want to tell the diceDriver about the locked status of the clock domain
	// and sample rate etc.
	
	//let's collect AES interface lock status.
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
	
	//let's collect AVS interface lock status
	if (avsRxIsLocked(AVS_PLUG_ID1)) extStatus |= DD_EXT_STATUS_ARX1_LOCKED;
	if (avsRxIsLocked(AVS_PLUG_ID2)) extStatus |= DD_EXT_STATUS_ARX2_LOCKED;
	//let's update the slip information	
	if (pExtEvents->avs_events & (DAL_CB_AVS1_SLIP | DAL_CB_AVS1_RPT)) extStatus |= DD_EXT_STATUS_ARX1_SLIP;
	if (pExtEvents->avs_events & (DAL_CB_AVS2_SLIP | DAL_CB_AVS2_RPT)) extStatus |= DD_EXT_STATUS_ARX2_SLIP;
	

	// At last the most important thing, get the "dal" status and call diceDriver
	dalGetCurrentStatus  (eDAL_INTERFACE_1, &dalStatus);
	avcDriverSetStatus (dalStatus.state == eDAL_STATE_LOCKED, dalStatus.lockedNominalRate, 
		dalStatus.lockedRateHz, extStatus);
	
	// And make sure the codecs gets updated
	updateCodec(&dalStatus);

	return NO_ERROR;
}



// This function is called when the system is initializing. At this point no threads are running and no audio is
// passed. When this function returns all threads are started. It is not legal to wait on semaphores etc at this
// point in the initialization sequence.

#define PEAK_0DB  0xfff
#define PEAK_HIGH 0x204 //-12
#define PEAK_MID  0x102 //-24
#define PEAK_LOW  0x28 //-40


static uint8 peakToId (uint32 val)
{
	val = val>>16;
	if (val>=PEAK_0DB) return 0xf;
	if (val>=PEAK_HIGH) return 0x7;
	if (val>=PEAK_MID) return 0x3;
	if (val>=PEAK_LOW) return 0x1;
	return 0;
}

static void myMeterThread(void *dummy)
{
	uint32 v;
	uint8 msk;

	DO_FOREVER
	{	
		TCTaskWait(80);
		if (bLEDMeterMode)
		{
			v =pEntry[meterRouterIndex];
			msk = peakToId (v);
			v =pEntry[meterRouterIndex+1];
			msk |= (peakToId (v)<<4);
			targetSetAllLED (msk);
		}
	}
}
		


void myAppThread(void *dummy)
{
	static BOOL ledTgl = 0;
	uint8 swSetting = targetGetSw();
	bLEDMeterMode = (swSetting & TGT_SW1) ? ON : OFF;

	DO_FOREVER
	{	
		TCTaskWait(500);

		swSetting = targetGetSw ();

		// check if SW1 has changed
		if (bLEDMeterMode != (swSetting & TGT_SW1) ? ON : OFF)
		{
			bLEDMeterMode = (swSetting & TGT_SW1) ? ON : OFF;
			if (bLEDMeterMode) {
				//reset status LED's for VU's
				targetSetAllLED (0);
			}
		}
		
		if (!bLEDMeterMode)
		{
			targetSetLED (TGT_LED1, (ledTgl ^= 1) ? TGT_LED_ON : TGT_LED_OFF);
			targetSetLED (TGT_LED2, isAttached ? TGT_LED_ON : TGT_LED_OFF);
			targetSetLED (TGT_LED3, isEnabled ? TGT_LED_ON : TGT_LED_OFF);
			targetSetLED (TGT_LED4, isLocked ? TGT_LED_ON : TGT_LED_OFF);
			targetSetLED (TGT_LED5, aesIsMasterLocked () ? TGT_LED_ON : TGT_LED_OFF);
			targetSetLED (TGT_LED6, adatIsLocked(0) ? TGT_LED_ON : TGT_LED_OFF);
			targetSetLED (TGT_LED7, adatIsLocked(1) ? TGT_LED_ON : TGT_LED_OFF);
		}
	}
}


//This function can dynamically change the configuration of the streams
//it is currently called from an application specific CLI command
//This function recales a new mode and invokes it by notifying the driver

HRESULT	myAppChangeConfig(uint32 mode)
{
	// pCurrentMode = myModesInitialize (mode);
	sysDebugPrintf("**** Forcing Mode 1 in this build. ****\r\n");
	pCurrentMode = myModesInitialize (1);
	myAppCreateDAL(lastDriverRatemode);
	//SYS_TRACE1(SYSDEBUG_TRACE_GENERAL, 0);
	return NO_ERROR;
}


/*********************************************************
	Initializations for this module
*/

#ifdef _AML

//This function creates virtual and physical MIDI ports and link them together
static void myAppAMLInitialize(void)
{
	uint32 uart1AmlRxId, uart1AmlTxId, avsAmlRx1Ids[8], avsAmlTx1Ids[8],avsAmlRx2Ids[8], avsAmlTx2Ids[8];
	uint32 port = 0;
	char  name[15];
	
	//let's create all the MIDI plug devices for two Isoc channels, 16 in and 16 out
	//These are virtual devices ready to process MIDI input and output but only the
	//ones specified in the mode files are being used.
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
	amlSetDevMonitorMode(avsAmlTx1Ids[0], TRUE);
	amlSetDevMonitorMode(avsAmlRx1Ids[0], TRUE);
	amlSetDevMonitorMode(avsAmlTx2Ids[0], TRUE);
	amlSetDevMonitorMode(avsAmlRx2Ids[0], TRUE);
	amlSetDevMonitorMode(uart1AmlTxId, TRUE);
	amlSetDevMonitorMode(uart1AmlRxId, TRUE);
	amlStartDev(uart1AmlTxId);
	amlStartDev(uart1AmlRxId);
#endif
}

#endif



HRESULT	myAppInitialize(void)
{
	HRESULT hResult = NO_ERROR;	

	targetSpiInit();
	uint32 sw = targetGetInitialSW ();
	sysDebugPrintf("Initial SW Settings\r\n");
	sysDebugPrintf(" 1  2  3  4\r\n");
	sysDebugPrintf("[%c][%c][%c][%c]\r\n",(sw&1)?'1':'0',(sw&2)?'1':'0',(sw&4)?'1':'0',(sw&8)?'1':'0');
	
	sysDebugPrintf("**** Forcing Mode 8 in this build. ****\r\n");
	uint32 mode = 8; //(sw >> 1) & 0x07;

	//Initialize the mode abstraction system
	pCurrentMode = myModesInitialize (mode);
	
	//create the call back from DAL
	dalInstallCallback(&myDalSubscribedEvents, 200, myDalCallBack, 0);
	
	//Create Low Rate DAL for default initialization
	myAppCreateDAL(eDAL_RATE_MODE_LOW);

#ifdef _AML
	myAppAMLInitialize();
#endif

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

#endif // _AVC
