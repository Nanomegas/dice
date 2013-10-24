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
	            This example creates a simple break-out-box with 8 channels
	            of AES I/O
	
	Revisions:
		created 12/20/2005 bk
		

****************************************************************************/
#ifdef _AVC

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "dal.h"
#include "avcDriver.h"
#include "coreDefs.h" //for GPIO definition for DIP switch
#include "targetBoard.h"
#include "avcDriverInterface.h"
#include "myApp.h"
#include "dalADAT.h"
#include "i2c.h"
#ifdef _CLI_TOOL_MYAPP
#include "myAppCli.h"
#endif //_CLI_TOOL_MYAPP

#ifdef _AXM20
#include "gray.h"
#include "dalRemoteInterface.h"
#include "axm20.h"
#endif

#include "avcAudio.h"
#include "avcMusic.h"
#include "avs.h"
#include "avsMidi.h"
#include "avsFmt.h"
#include "TCInterrupt.h"

// We need to define how we present ourselves to the PC and Mac driver. We will
// need to specify our Isco channel layout, names for ASIO and CoreAudio etc.
// Starting and stopping the 1394 Isoc streams will be handled by the DiceDriver
// module.
//
//===========================================================
// Forward declarations used below
//===========================================================
static HRESULT myAppChangeSR(AVC_DRIVER_CB_DATA * pData);
static void myAppCreateLowRateDAL(void);
static void myAppCreateMidRateDAL(void);
static void myAppCreateHighRateDAL(void);
static void myAppInitializeI2S(void);
static void myAppSetI2SLowMid(void);
static void myAppSetI2SHigh(void);

// Configuration of Receive channels in Low Mid Mode(from computer to device)
#define MY_NB_RX_ISOC_STREAMS_LOW 1
AVC_STREAM_CONFIG myAvcDriverRxConfigLow[MY_NB_RX_ISOC_STREAMS_LOW] =
{
//	Audio  Midi	
	{8,    1}
};

// Configuration of Transmit channels in Low Mid Mode(from this device to computer)
#define MY_NB_TX_ISOC_STREAMS_LOW 1
AVC_STREAM_CONFIG myAvcDriverTxConfigLow[MY_NB_TX_ISOC_STREAMS_LOW] =
{
//	Audio  Midi	
	{8,    1}
};

// Configuration of Receive channels in Mid Mode(from computer to device)
#define MY_NB_RX_ISOC_STREAMS_MID 1
AVC_STREAM_CONFIG myAvcDriverRxConfigMid[MY_NB_RX_ISOC_STREAMS_MID] =
{
//	Audio  Midi	
	{8,    1}
};

// Configuration of Transmit channels in Mid Mode(from this device to computer)
#define MY_NB_TX_ISOC_STREAMS_MID 1
AVC_STREAM_CONFIG myAvcDriverTxConfigMid[MY_NB_TX_ISOC_STREAMS_MID] =
{
//	Audio  Midi	
	{8,    1}
};

// Configuration of Receive channels in Low Mid Mode(from computer to device)
#define MY_NB_RX_ISOC_STREAMS_HIGH 1
AVC_STREAM_CONFIG myAvcDriverRxConfigHigh[MY_NB_RX_ISOC_STREAMS_HIGH] =
{
//	Audio  Midi	
	{4,    1}
};

// Configuration of Transmit channels in Low Mid Mode(from this device to computer)
#define MY_NB_TX_ISOC_STREAMS_HIGH 1
AVC_STREAM_CONFIG myAvcDriverTxConfigHigh[MY_NB_TX_ISOC_STREAMS_HIGH] =
{
//	Audio  Midi	
	{4,    1}
};

// This application uses the AES input and output interfaces, let's define that
#define MY_INPUT_DEVICES_LOW  (DEVICE_BIT(DEV_AES_RX) | DEVICE_BIT(DEV_AVS_ADO1))
#define MY_OUTPUT_DEVICES_LOW (DEVICE_BIT(DEV_AES_TX) | DEVICE_BIT(DEV_AVS_ADI1))

#define MY_INPUT_DEVICES_MID  (DEVICE_BIT(DEV_AES_RX) | DEVICE_BIT(DEV_AVS_ADO1))
#define MY_OUTPUT_DEVICES_MID (DEVICE_BIT(DEV_AES_TX) | DEVICE_BIT(DEV_AVS_ADI1))

#define MY_INPUT_DEVICES_HIGH  (DEVICE_BIT(DEV_AES_RX) | DEVICE_BIT(DEV_AVS_ADO1))
#define MY_OUTPUT_DEVICES_HIGH (DEVICE_BIT(DEV_AES_TX) | DEVICE_BIT(DEV_AVS_ADI1))

// task (thread) ID for this module									
uint32						myAppThreadID = 0;				// clock controller thread
const char*					myAppThreadName = "myAppAvcThread";
//Variable used for checking DAL changes
static int	OldRate = eDAL_NOMINAL_RATE_48;
//Flag for previous DAL Creation
int	PreviousDAL = false;

// The AvcDriver module does not know which clock domain we want to use and how we intend
// to handle clock rate and source change. It will have to call us to do that work. The
// call back function defined below takes care of that.

HRESULT myAvcDriverCallBack (AVC_DRIVER_MSG_ENUM msg, AVC_DRIVER_CB_DATA * pData)
{
	uint32 notify;
	// why did the driver call us?
	switch (msg)
	{
	case eAVCDR_MSG_CLOCK_CHG:
		// The computer has requested a change of clock and source. We
		// should honour that by issuing a call to "dal"
		//Check if DAL needs to be changed
		notify = myAppChangeSR(pData);
		//Change the Clock Now
		dalSetClockSource (eDAL_INTERFACE_1, pData->source, pData->rate);
		break;

	case eAVCDR_MSG_ATTACH:
		// The computer attached itself to this device
		// this typically happens after the computer boots, but also after
		// bus resets or the first time the driver is installed
		// In this application we don't care			
		LED_ON(DRIVER_ATCH_LED_ID);
		break;

	case eAVCDR_MSG_DETACH:
		// The computer detached itself from this device
		// this typically happens when the computer shuts down
		// or at a bus reset. 
		// In this application we don't care			
		LED_OFF(DRIVER_ATCH_LED_ID);
		break;

	case eAVCDR_MSG_ENABLE:
		// The computer has enabled streaming			
		LED_ON(DRIVER_EN_LED_ID);
		break;

	case eAVCDR_MSG_DISABLE:
		// The computer has disabled streaming
		LED_OFF(DRIVER_EN_LED_ID);
		break;
		}
		return NO_ERROR;
}

// The AvcDriver module also needs to know when the status of "dal" is changing so
// it can adjust streams accordingly. We will install a call back function
// in "dal" to notify us of such changes

static const DAL_EVENTS myDalSubscribedEvents = 
{
	//state machine events we want to subscribe to
	DAL_CB_LOCK1_AQUIRED | DAL_CB_LOCK1_LOST | DAL_CB_RATE1_NOM_CHANGED | DAL_CB_RATE1_CHANGED | DAL_CB_CREATED1,
	//AES events we want to subscribe to
	DAL_CB_AES0_LOCK_ACQUIRED | DAL_CB_AES0_LOCK_LOST | DAL_CB_AES0_SLIP |DAL_CB_AES0_RPT |
	DAL_CB_AES1_LOCK_ACQUIRED | DAL_CB_AES1_LOCK_LOST | DAL_CB_AES1_SLIP |DAL_CB_AES1_RPT |
	DAL_CB_AES2_LOCK_ACQUIRED | DAL_CB_AES2_LOCK_LOST | DAL_CB_AES2_SLIP |DAL_CB_AES2_RPT |
	DAL_CB_AES3_LOCK_ACQUIRED | DAL_CB_AES3_LOCK_LOST | DAL_CB_AES3_SLIP |DAL_CB_AES3_RPT,
	//ADAT events we want to subscribe to
	0,
	0,
	DAL_CB_AVS1_SLIP | DAL_CB_AVS1_RPT | DAL_CB_AVS1_LOCK_ACQUIRED | 
	DAL_CB_AVS1_LOCK_LOST | DAL_CB_AVS1_ENABLE | DAL_CB_AVS1_DISABLE |
	DAL_CB_AVS2_SLIP | DAL_CB_AVS2_RPT | DAL_CB_AVS2_LOCK_ACQUIRED | 
	DAL_CB_AVS2_LOCK_LOST | DAL_CB_AVS2_ENABLE | DAL_CB_AVS2_DISABLE |
	DAL_CB_AVS3_SLIP | DAL_CB_AVS3_RPT | DAL_CB_AVS3_LOCK_ACQUIRED | 
	DAL_CB_AVS3_LOCK_LOST | DAL_CB_AVS3_ENABLE | DAL_CB_AVS3_DISABLE |
	DAL_CB_AVS4_SLIP | DAL_CB_AVS4_RPT | DAL_CB_AVS4_LOCK_ACQUIRED | 
	DAL_CB_AVS4_LOCK_LOST |DAL_CB_AVS4_ENABLE | DAL_CB_AVS4_DISABLE ,
	0
};

static HRESULT myDalCallBack (DAL_CB_EVENT_MASK events, DAL_EVENTS * pExtEvents, uint32 data)
{
	uint16			aesStatus;
	uint32			extStatus=0; //extended status to the driver
	DAL_STATUS		dalStatus;
	
	// We want to tell the avcDriver about the locked status of the clock domain
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
	if (adatIsLocked()) extStatus |= DD_EXT_STATUS_ADAT_LOCKED;
	//let's update the slip information
	if (pExtEvents->adat_events & (DAL_CB_ADAT_SLIP | DAL_CB_ADAT_RPT)) extStatus |= DD_EXT_STATUS_ADAT_SLIP;
	
	//let's collect AVS interface lock status
	if (avsRxIsLocked(AVS_PLUG_ID1)) extStatus |= DD_EXT_STATUS_ARX1_LOCKED;
	if (avsRxIsLocked(AVS_PLUG_ID2)) extStatus |= DD_EXT_STATUS_ARX2_LOCKED;
	if (avsRxIsLocked(AVS_PLUG_ID3)) extStatus |= DD_EXT_STATUS_ARX3_LOCKED;
	if (avsRxIsLocked(AVS_PLUG_ID4)) extStatus |= DD_EXT_STATUS_ARX4_LOCKED;
	//let's update the slip information	
	if (pExtEvents->avs_events & (DAL_CB_AVS1_SLIP | DAL_CB_AVS1_RPT)) extStatus |= DD_EXT_STATUS_ARX1_SLIP;
	if (pExtEvents->avs_events & (DAL_CB_AVS2_SLIP | DAL_CB_AVS2_RPT)) extStatus |= DD_EXT_STATUS_ARX2_SLIP;
	if (pExtEvents->avs_events & (DAL_CB_AVS3_SLIP | DAL_CB_AVS3_RPT)) extStatus |= DD_EXT_STATUS_ARX3_SLIP;
	if (pExtEvents->avs_events & (DAL_CB_AVS4_SLIP | DAL_CB_AVS4_RPT)) extStatus |= DD_EXT_STATUS_ARX4_SLIP;
	

	// At last the most important thing, get the "dal" status and call diceDriver
	dalGetCurrentStatus  (eDAL_INTERFACE_1, &dalStatus);
	avcDriverSetStatus (dalStatus.state == eDAL_STATE_LOCKED, dalStatus.lockedNominalRate, 
		dalStatus.lockedRateHz);
	
	// Actually now that we are at it, let's indicate lock on an LED
	if (dalStatus.state == eDAL_STATE_LOCKED)
		LED_ON(LOCKUP_LED_ID);
	else
		LED_OFF(LOCKUP_LED_ID);
	return NO_ERROR;
}

// This function is called when the system is initializing. At this point no threads are running and no audio is
// passed. When this function returns all threads are started. It is not legal to wait on semaphores etc at this
// point in the initialization sequence.

// Example of myAppThread which does not do anything.

void myAppThread(void *dummy)
{
	uint32 prevTime = TCTimeGet();

	DO_FOREVER
	{	
		TCTaskWait(100);
		//just wake up every 100ms and do the following

		if ((TCTimeGet() - prevTime) > 1000)
		{

		}
#ifdef _AXM20
		AXM20Update();
#endif //_AXM20
	}
}

HRESULT myAppInitializeTasks(void)
{	
	HRESULT hResult = NO_ERROR;
	
	hResult = TCTaskCreate(&myAppThreadID, myAppThreadName, myAppThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
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

	return hResult;
}

/*********************************************************
	Initializations for this module
*/
HRESULT	myAppInitialize(void)
{
	HRESULT hResult = NO_ERROR;	
	//Create Low Rate DAL
	myAppCreateLowRateDAL();
	//Set flag for creation of DAL
	PreviousDAL = true; 			

	// After application initialization is done resume tasks to start them
	hResult = myAppResumeTasks();
	if (hResult != NO_ERROR) return hResult;

	// If the developer wishes to add Command Line Interface, define _CLI_TOOL_MYAPP in make.params
#ifdef _CLI_TOOL_MYAPP
	myAppCliInstallTools();
#endif //_CLI_TOOL_MYAPP
	return hResult;
}

static HRESULT myAppChangeSR(AVC_DRIVER_CB_DATA * pData)
{
	HRESULT hResult = NO_ERROR;
	uint32 notify = 0;
	
	cliPrintf("Change SR: old=%x, new=%x\n\r", OldRate, pData->rate);

	switch(pData->rate)
	{
	case eDAL_NOMINAL_RATE_32:
	case eDAL_NOMINAL_RATE_44_1:
	case eDAL_NOMINAL_RATE_48:
		if(OldRate > eDAL_NOMINAL_RATE_48)
		{
			cliPrintf(" low rate\n\r");
			myAppCreateLowRateDAL();
			//Send the RX and TX Configuration Notification
			notify |= DD_NOTIFY_RX_CFG_CHG_BIT  | DD_NOTIFY_TX_CFG_CHG_BIT;			
		}
		break;
	case eDAL_NOMINAL_RATE_88_2:
	case eDAL_NOMINAL_RATE_96:
		if((OldRate < eDAL_NOMINAL_RATE_88_2) || (OldRate > eDAL_NOMINAL_RATE_96))
		{
			myAppCreateMidRateDAL();
			//Send the RX and TX Configuration Notification
			notify |= DD_NOTIFY_RX_CFG_CHG_BIT  | DD_NOTIFY_TX_CFG_CHG_BIT;						
		}
		break;
	case eDAL_NOMINAL_RATE_176_4:
	case eDAL_NOMINAL_RATE_192:
		if(OldRate < eDAL_NOMINAL_RATE_176_4)
		{
			myAppCreateHighRateDAL();
			//Send the RX and TX Configuration Notification
			notify |= DD_NOTIFY_RX_CFG_CHG_BIT  | DD_NOTIFY_TX_CFG_CHG_BIT;			
		}
		break;
	default:
		cliPrintf("No change.\n\r");
		break;
	}
	OldRate = pData->rate;

	if(hResult == NO_ERROR)
	{		
		//Change the Clock Now
		dalSetClockSource (eDAL_INTERFACE_1, pData->source, pData->rate);
		//Send the Clock Setting Accept Notification
		notify |= DD_NOTIFY_CLOCK_ACP;
		OldRate = pData->rate;
	}
	return hResult;
}

static void myAppCreateLowRateDAL(void)
{
	cliPrintf("low rate dal.\n\r");

	if(PreviousDAL)
		dalUninstallCallback(myDalCallBack);

	//Let's build our audio configuration with "dal", We want to allow all rates from 32KHz to 96KHz
	dalCreateInterface(eDAL_INTERFACE_1, eDAL_RATE_MODE_LOW, MY_INPUT_DEVICES_LOW, MY_OUTPUT_DEVICES_LOW);
	//default we will run at internal rate 48KHz, the computer will change that when attached
	dalSetClockSource (eDAL_INTERFACE_1, eDAL_CLK_SRC_AVS_SYT1, eDAL_NOMINAL_RATE_ANY);		
	//we need to setup our routing
	//let the 8 AES channels take data from the first Isoc receiver
	dalSetRoute(eDAL_INTERFACE_1, TX_AES_CH0_7, RX_AVS1_CH0_7);		
	//let the first 8 Isoc 1 transmitter channels take data from the 8 AES channels
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_7, RX_AES_CH0_7);
	//the call back function defined above needs to be installed into "dal"		
	dalInstallCallback(&myDalSubscribedEvents, 200, myDalCallBack, 0);
	//configure device for MIDI based on a function in targetBoard. For the EVAL
	//board this is determined by DIP SW 1
	myAvcDriverTxConfigLow[0].midiChannels = 1;
	myAvcDriverRxConfigLow[0].midiChannels = 1;

	avcDriverConfigure (myAvcDriverTxConfigLow, MY_NB_TX_ISOC_STREAMS_LOW, 
		myAvcDriverRxConfigLow, MY_NB_RX_ISOC_STREAMS_LOW,
		myAvcDriverCallBack);

	myAppInitializeI2S();
	myAppSetI2SLowMid();
	// now we are ready to start the whole thing, it actually wont start until we return from
	// this function as threads are not enabled yet. If we want to have some code running after
	// this we need to create a thread. We don't need that here, our call backs will do things
	// for us.	
	dalStartInterface(eDAL_INTERFACE_1);
}

static void myAppCreateMidRateDAL(void)
{
	cliPrintf("mid rate dal.\n\r");

	if(PreviousDAL)
		dalUninstallCallback(myDalCallBack);

	//Let's build our audio configuration with "dal", We want to allow all rates from 32KHz to 96KHz
	dalCreateInterface(eDAL_INTERFACE_1, eDAL_RATE_MODE_MID, MY_INPUT_DEVICES_MID, MY_OUTPUT_DEVICES_MID);
	//default we will run at internal rate 48KHz, the computer will change that when attached
	dalSetClockSource (eDAL_INTERFACE_1, eDAL_CLK_SRC_AVS_SYT1, eDAL_NOMINAL_RATE_96);		
	//we need to setup our routing
	//let the 8 AES channels take data from the first Isoc receiver
	dalSetRoute(eDAL_INTERFACE_1, TX_AES_CH0_7, RX_AVS1_CH0_7);		
	//let the first 8 Isoc 1 transmitter channels take data from the 8 AES channels
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_7, RX_AES_CH0_7);
	//the call back function defined above needs to be installed into "dal"		
	dalInstallCallback(&myDalSubscribedEvents, 200, myDalCallBack, 0);
	//configure device for MIDI based on a function in targetBoard. For the EVAL
	//board this is determined by DIP SW 1
	myAvcDriverTxConfigLow[0].midiChannels = 1;
	myAvcDriverRxConfigLow[0].midiChannels = 1;

	avcDriverConfigure (myAvcDriverTxConfigLow, MY_NB_TX_ISOC_STREAMS_MID, 
		myAvcDriverRxConfigLow, MY_NB_RX_ISOC_STREAMS_MID,
		myAvcDriverCallBack);

	myAppInitializeI2S();
	myAppSetI2SLowMid();
	// now we are ready to start the whole thing, it actually wont start until we return from
	// this function as threads are not enabled yet. If we want to have some code running after
	// this we need to create a thread. We don't need that here, our call backs will do things
	// for us.	
	dalStartInterface(eDAL_INTERFACE_1);
}

static void myAppCreateHighRateDAL(void)
{
	cliPrintf("high rate dal.\n\r");

	if(PreviousDAL)
		dalUninstallCallback(myDalCallBack);

	//Let's build our audio configuration with "dal", We want to allow all rates above 96KHz
	dalCreateInterface(eDAL_INTERFACE_1, eDAL_RATE_MODE_HIGH, MY_INPUT_DEVICES_HIGH, MY_OUTPUT_DEVICES_HIGH);
	//default we will run at internal rate 48KHz, the computer will change that when attached
	dalSetClockSource (eDAL_INTERFACE_1, eDAL_CLK_SRC_AVS_SYT1, eDAL_NOMINAL_RATE_192);		
	//we need to setup our routing
	//let the 4 AES channels take data from the first Isoc receiver
	dalSetRoute(eDAL_INTERFACE_1, TX_AES_CH0_3, RX_AVS1_CH0_3);		
	//let the first 8 Isoc 1 transmitter channels take data from the 8 AES channels
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_3, RX_AES_CH0_3);
	//the call back function defined above needs to be installed into "dal"		
	dalInstallCallback(&myDalSubscribedEvents, 200, myDalCallBack, 0);
	//configure device for MIDI based on a function in targetBoard. For the EVAL
	//board this is determined by DIP SW 1
	myAvcDriverTxConfigLow[0].midiChannels = 1;
	myAvcDriverRxConfigLow[0].midiChannels = 1;

	avcDriverConfigure (myAvcDriverTxConfigLow, MY_NB_TX_ISOC_STREAMS_HIGH, 
		myAvcDriverRxConfigLow, MY_NB_RX_ISOC_STREAMS_HIGH,
		myAvcDriverCallBack);

	myAppInitializeI2S();
	myAppSetI2SHigh();
	// now we are ready to start the whole thing, it actually wont start until we return from
	// this function as threads are not enabled yet. If we want to have some code running after
	// this we need to create a thread. We don't need that here, our call backs will do things
	// for us.	
	dalStartInterface(eDAL_INTERFACE_1);
}

static void myAppInitializeI2S(void)
{
	// I2S Receivers
	i2sRxSetMckFreq		(I2S_ID0, 0);
	i2sRxSetM192		(I2S_ID0, 0);	
	i2sRxSetClock		(I2S_ID0, TRUE, 0);
	i2sRxSetClockInv	(I2S_ID0, 0, 0);
	i2sRxSetDataFormat	(I2S_ID0, I2S_CH0, 1, 1, 0, 0);
	i2sRxSetDataFormat	(I2S_ID0, I2S_CH1, 1, 1, 0, 0);
	i2sRxSetDataFormat	(I2S_ID0, I2S_CH2, 1, 1, 0, 0);
	i2sRxSetDataFormat	(I2S_ID0, I2S_CH3, 1, 1, 0, 0);
	
	i2sRxSetMckFreq		(I2S_ID1, 0);
	i2sRxSetM192		(I2S_ID1, 0);	
	i2sRxSetClock		(I2S_ID1, TRUE, 0);
	i2sRxSetClockInv	(I2S_ID1, 0, 0);
	i2sRxSetDataFormat	(I2S_ID1, I2S_CH0, 1, 1, 0, 0);
	i2sRxSetDataFormat	(I2S_ID1, I2S_CH1, 1, 1, 0, 0);

	i2sRxSetMckFreq		(I2S_ID2, 0);
	i2sRxSetM192		(I2S_ID2, 0);	
	i2sRxSetClock		(I2S_ID2, TRUE, 0);
	i2sRxSetClockInv	(I2S_ID2, 0, 0);
	i2sRxSetDataFormat	(I2S_ID2, I2S_CH0, 1, 1, 0, 0);
	i2sRxSetDataFormat	(I2S_ID2, I2S_CH1, 1, 1, 0, 0);

	// I2S Transmitters
	i2sTxSetMckFreq		(I2S_ID0, 0);
	i2sTxSetM192		(I2S_ID0, 0);	
	i2sTxSetClock		(I2S_ID0, TRUE, 0);
	i2sTxSetClockInv	(I2S_ID0, 0, 0);
	i2sTxSetDataFormat	(I2S_ID0, I2S_CH0, 1, 1, 0, 0);
	i2sTxSetDataFormat	(I2S_ID0, I2S_CH1, 1, 1, 0, 0);
	i2sTxSetDataFormat	(I2S_ID0, I2S_CH2, 1, 1, 0, 0);
	i2sTxSetDataFormat	(I2S_ID0, I2S_CH3, 1, 1, 0, 0);

	i2sTxSetMckFreq		(I2S_ID1, 0);
	i2sTxSetM192		(I2S_ID1, 0);	
	i2sTxSetClock		(I2S_ID1, TRUE, 0);
	i2sTxSetClockInv	(I2S_ID1, 0, 0);
	i2sTxSetDataFormat	(I2S_ID1, I2S_CH0, 1, 1, 0, 0);
	i2sTxSetDataFormat	(I2S_ID1, I2S_CH1, 1, 1, 0, 0);

	i2sTxSetMckFreq		(I2S_ID2, 0);
	i2sTxSetM192		(I2S_ID2, 0);	
	i2sTxSetClock		(I2S_ID2, TRUE, 0);
	i2sTxSetClockInv	(I2S_ID2, 0, 0);
	i2sTxSetDataFormat	(I2S_ID2, I2S_CH0, 1, 1, 0, 0);
	i2sTxSetDataFormat	(I2S_ID2, I2S_CH1, 1, 1, 0, 0);
}

static void myAppSetI2SLowMid(void)
{
		i2sRxSetM192		(I2S_ID0, 0);	
		i2sRxSetM192		(I2S_ID1, 0);	
		i2sRxSetM192		(I2S_ID2, 0);	
		i2sTxSetM192		(I2S_ID0, 0);	
		i2sTxSetM192		(I2S_ID1, 0);	
		i2sTxSetM192		(I2S_ID2, 0);	
}

static void myAppSetI2SHigh(void)
{
		i2sRxSetM192		(I2S_ID0, 1);	
		i2sRxSetM192		(I2S_ID1, 1);	
		i2sRxSetM192		(I2S_ID2, 1);	
		i2sTxSetM192		(I2S_ID0, 1);	
		i2sTxSetM192		(I2S_ID1, 1);	
		i2sTxSetM192		(I2S_ID2, 1);	
}

#endif // _AVC
