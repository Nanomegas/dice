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
		

****************************************************************************/

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "dal.h"
#include "diceDriver.h"
#include "coreDefs.h" //for GPIO definition for DIP switch
#include "targetBoard.h"
#include "dicedriverInterface.h"
#include "myApp.h"
#include "dalADAT.h"
#include "i2c.h"
#include "mixer8.h"
#include "mixer8DAL.h"
#include "mixer8firewire.h"
#include "myModes.h"

#ifdef _CLI_TOOL_MYAPP
#include "myAppCli.h"
#endif //_CLI_TOOL_MYAPP

#ifdef _AXM20
#include "gray.h"
#include "dalRemoteInterface.h"
#include "axm20.h"
#endif

#ifdef _AML
#include "aml.h"
#include "avsAml.h"
#include "uartAml.h"
#endif //_AML

// We need to define how we present ourselves to the PC and Mac driver. We will
// need to specify our Isco channel layout, names for ASIO and CoreAudio etc.
// Starting and stopping the 1394 Isoc streams will be handled by the DiceDriver
// module.
//
// This file uses myModes to select between a number of configurations of the board
// The current mode is stored in the EEPROM unless MYAPP_MODE0_ONLY is defined in 
// which case the myMode0.c file defines the mode.

//===========================================================
// Forward declarations used below
//===========================================================
static HRESULT myAppChangeSR(DICE_DRIVER_CB_DATA * pData);
static void myAppCreateLowRateDAL(void);
static void myAppCreateMidRateDAL(void);
static void myAppCreateHighRateDAL(void);
static bool myAppCheckForExpanderBoard(void);
static void myAppSetupi2c(void);
static void myAppInitializeI2S(void);
static void myAppSetI2SLowMid(void);
static void myAppSetI2SHigh(void);


// task (thread) ID for this module									
uint32						myAppThreadID = 0;				// clock controller thread
const char*					myAppThreadName = "myAppThread";
//Variable used for checking DAL changes
static int	OldRate = eDAL_NOMINAL_RATE_48;
//Flag for previous DAL Creation

MODE_CFG * pCurrentMode;


// The default name our device will appear as under ASIO and CoreAudio
#define MY_DEVICE_NICK_NAME "MyProduct"

// The DiceDriver module does not know which clock domain we want to use and how we intend
// to handle clock rate and source change. It will have to call us to do that work. The
// call back function defined below takes care of that.

HRESULT myDiceDriverCallBack (DICE_DRIVER_MSG_ENUM msg, DICE_DRIVER_CB_DATA * pData)
{	
	// why did the driver call us?
	switch (msg)
	{
	case eDDR_MSG_CLOCK_CHG:
		// The computer has requested a change of clock and source
		myAppChangeSR(pData);
		break;

	case eDDR_MSG_ATTACH:
		// The computer attached itself to this device
		// this typically happens after the computer boots, but also after
		// bus resets or the first time the driver is installed
		// In this application we don't care			
		LED_ON(DRIVER_ATCH_LED_ID);
		break;

	case eDDR_MSG_DETACH:
		// The computer detached itself from this device
		// this typically happens when the computer shuts down
		// or at a bus reset. 
		// In this application we don't care			
		LED_OFF(DRIVER_ATCH_LED_ID);
		break;

	case eDDR_MSG_ENABLE:
		// The computer has enabled streaming			
		LED_ON(DRIVER_EN_LED_ID);
		break;

	case eDDR_MSG_DISABLE:
		// The computer has disabled streaming
		LED_OFF(DRIVER_EN_LED_ID);
		break;
	case eDDR_MSG_AC3_CHG:
		//AC3 flags for one of the transmitter or receiver has changed
		break;
	}
	return NO_ERROR;
}


// The diceDriver module also needs to know when the status of "dal" is
// changing so it can adjust streams accordingly and so it can provide
// correct status information to the computer. We will install a call back
// function in "dal" to notify us of such changes

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
	DAL_CB_ADAT_SLIP | DAL_CB_ADAT_RPT | DAL_CB_ADAT_LOCK_ACQUIRED | DAL_CB_ADAT_LOCK_LOST,
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


// The diceDriver also needs to be notified when the DAL locked state is changing.

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
	diceDriverSetStatus (dalStatus.state == eDAL_STATE_LOCKED, dalStatus.lockedNominalRate, 
		dalStatus.lockedRateHz, extStatus);
	
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
	DO_FOREVER
	{	
		TCTaskWait(100);
		//just wake up every 100ms and do the following
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

#ifdef _AML
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
	if(targetBoardIsMIDIEnabled()) 
	{
		amlCreateDev(&uart1AmlTxId,  &uart1_midi_dev_cbs, AML_DEV_TX, 0, "uart1_tx");
		amlCreateDev(&uart1AmlRxId,  &uart1_midi_dev_cbs, AML_DEV_RX, 0, "uart1_rx");

		//The Physical UART is linked to the first plug of the first isoc.
		amlSetDestination(uart1AmlRxId, avsAmlTx1Ids[0]);
		amlSetDestination(avsAmlRx1Ids[0], uart1AmlTxId);
	}
	else
	{
	//in this case just loop back the midi plug
		amlSetDestination(avsAmlRx1Ids[0], avsAmlTx1Ids[0]);
	}
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
	if(targetBoardIsMIDIEnabled()) 
	{
		amlStartDev(uart1AmlTxId);
		amlStartDev(uart1AmlRxId);
	}
}

#endif

/*********************************************************
	Initializations for this module
*/
HRESULT	myAppInitialize(void)
{
	HRESULT hResult = NO_ERROR;	
	uint32 initFlags = 0;
	
	//initialize I2C for the expander board
	myAppSetupi2c();
	
	//get the hw configuration options
	if (myAppCheckForExpanderBoard()) initFlags |= MODE_INIT_EXPN;
	if (targetBoardIsMIDIEnabled()) initFlags |= MODE_INIT_MIDI;
	
	//Initialize the mode abstraction system
	pCurrentMode = myModesInitialize (initFlags);
	
	//create the call back from DAL
	dalInstallCallback(&myDalSubscribedEvents, 200, myDalCallBack, 0);
	
	//Create Low Rate DAL for default initialization
	myAppCreateLowRateDAL();

#ifdef _AML
	myAppAMLInitialize();
#endif

	//Set flag for creation of DAL
	// After application initialization is done resume tasks to start them
	hResult = myAppResumeTasks();
	if (hResult != NO_ERROR) return hResult;
	// If the developer wishes to add Command Line Interface, define _CLI_TOOL_MYAPP in make.params
#ifdef _MIXER8
	hResult = mixer8Initialize();
	if (hResult != NO_ERROR) return hResult;
	hResult = mixer8DALInitialize();
	if (hResult != NO_ERROR) return hResult;
	hResult = mixer8firewireInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif//_MIXER8
#ifdef _CLI
#ifdef _CLI_TOOL_MYAPP
	myAppCliInstallTools();
#endif //_CLI_TOOL_MYAPP
#endif // _CLI
	return hResult;
}

static HRESULT myAppChangeSR(DICE_DRIVER_CB_DATA * pData)
{
	HRESULT hResult = NO_ERROR;
	uint32 notify = 0;

	switch(pData->rate)
	{
	case eDAL_NOMINAL_RATE_32:
	case eDAL_NOMINAL_RATE_44_1:
	case eDAL_NOMINAL_RATE_48:
		if(OldRate > eDAL_NOMINAL_RATE_48)
		{
			myAppCreateLowRateDAL();
			//Send the RX and TX Configuration Notification
			notify |= DD_NOTIFY_RX_CFG_CHG_BIT  | DD_NOTIFY_TX_CFG_CHG_BIT;
		}
		//Report error if a sampling rate range is not supported
		hResult = NO_ERROR;	
		break;
	case eDAL_NOMINAL_RATE_88_2:
	case eDAL_NOMINAL_RATE_96:
		if((OldRate < eDAL_NOMINAL_RATE_88_2) || (OldRate > eDAL_NOMINAL_RATE_96))
		{
			myAppCreateMidRateDAL();
			//Send the RX and TX Configuration Notification
			notify |= DD_NOTIFY_RX_CFG_CHG_BIT  | DD_NOTIFY_TX_CFG_CHG_BIT;
		}
		//Report error if a sampling rate range is not supported
		hResult = NO_ERROR;
		break;
	case eDAL_NOMINAL_RATE_176_4:
	case eDAL_NOMINAL_RATE_192:
		if(OldRate < eDAL_NOMINAL_RATE_176_4)
		{
			myAppCreateHighRateDAL();
			//Send the RX and TX Configuration Notification
			notify |= DD_NOTIFY_RX_CFG_CHG_BIT  | DD_NOTIFY_TX_CFG_CHG_BIT;
		}
		//Report error if a sampling rate range is not supported
		hResult = NO_ERROR;		
		break;
	default:
		//hResult = ERROR;
		break;
	}
	if(hResult == NO_ERROR)
	{		
		//Change the Clock Now
		dalSetClockSource (eDAL_INTERFACE_1, pData->source, pData->rate);
		//Send the Clock Setting Accept Notification
		notify |= DD_NOTIFY_CLOCK_ACP;
		diceDriverNotifyHost(notify);
		OldRate = pData->rate;
	}
	return hResult;
}

static void myAppCreateLowRateDAL(void)
{
	//Let's build our audio configuration with "dal", We want to allow all rates from 32KHz to 96KHz
	dalCreateInterface(eDAL_INTERFACE_1, eDAL_RATE_MODE_LOW,pCurrentMode->inputDevices,pCurrentMode->outputDevices);
	//default we will run at internal rate 48KHz, the computer will change that when attached
	dalSetClockSource (eDAL_INTERFACE_1, eDAL_CLK_SRC_INTERNAL, eDAL_NOMINAL_RATE_48);		
	pCurrentMode->fRouting(0);
	diceDriverConfigureNew(pCurrentMode->low->pTxStreamCfg, pCurrentMode->low->nbTxStreams, 
			               pCurrentMode->low->pRxStreamCfg, pCurrentMode->low->nbRxStreams,
			               MY_DEVICE_NICK_NAME,
			               myDiceDriverCallBack,
			               pCurrentMode->clocksCaps,
			               pCurrentMode->clockNames);
	diceDriverSetHardwareLatencies( pCurrentMode->low->txHwLatency, pCurrentMode->low->rxHwLatency );
	myAppInitializeI2S();
	myAppSetI2SLowMid();
	// now we are ready to start the whole thing, it actually won't start until we return from
	// this function as threads are not enabled yet. If we want to have some code running after
	// this we need to create a thread. We don't need that here, our call backs will do things
	// for us.	
	dalStartInterface(eDAL_INTERFACE_1);
}

static void myAppCreateMidRateDAL(void)
{
	//Let's build our audio configuration with "dal", We want to allow all rates from 32KHz to 96KHz
	dalCreateInterface(eDAL_INTERFACE_1, eDAL_RATE_MODE_MID,pCurrentMode->inputDevices,pCurrentMode->outputDevices);
	//default we will run at internal rate 48KHz, the computer will change that when attached
	dalSetClockSource (eDAL_INTERFACE_1, eDAL_CLK_SRC_INTERNAL, eDAL_NOMINAL_RATE_96);		
	pCurrentMode->fRouting(1);
	diceDriverConfigureNew(pCurrentMode->mid->pTxStreamCfg, pCurrentMode->mid->nbTxStreams, 
			               pCurrentMode->mid->pRxStreamCfg, pCurrentMode->mid->nbRxStreams,
			               MY_DEVICE_NICK_NAME,
			               myDiceDriverCallBack,
			               pCurrentMode->clocksCaps,
			               pCurrentMode->clockNames);
	diceDriverSetHardwareLatencies( pCurrentMode->mid->txHwLatency, pCurrentMode->mid->rxHwLatency );
	myAppInitializeI2S();
	myAppSetI2SLowMid();
	// now we are ready to start the whole thing, it actually won't start until we return from
	// this function as threads are not enabled yet. If we want to have some code running after
	// this we need to create a thread. We don't need that here, our call backs will do things
	// for us.	
	dalStartInterface(eDAL_INTERFACE_1);
}


static void myAppCreateHighRateDAL(void)
{
	//Let's build our audio configuration with "dal", We want to allow all rates from 32KHz to 96KHz
	dalCreateInterface(eDAL_INTERFACE_1, eDAL_RATE_MODE_HIGH,pCurrentMode->inputDevices,pCurrentMode->outputDevices);
	//default we will run at internal rate 48KHz, the computer will change that when attached
	dalSetClockSource (eDAL_INTERFACE_1, eDAL_CLK_SRC_INTERNAL, eDAL_NOMINAL_RATE_192);		
	pCurrentMode->fRouting(2);
	diceDriverConfigureNew(pCurrentMode->high->pTxStreamCfg, pCurrentMode->high->nbTxStreams, 
			               pCurrentMode->high->pRxStreamCfg, pCurrentMode->high->nbRxStreams,
			               MY_DEVICE_NICK_NAME,
			               myDiceDriverCallBack,
			               pCurrentMode->clocksCaps,
			               pCurrentMode->clockNames);
	diceDriverSetHardwareLatencies( pCurrentMode->high->txHwLatency, pCurrentMode->high->rxHwLatency );
	myAppInitializeI2S();
	myAppSetI2SHigh();
	// now we are ready to start the whole thing, it actually won't start until we return from
	// this function as threads are not enabled yet. If we want to have some code running after
	// this we need to create a thread. We don't need that here, our call backs will do things
	// for us.	
	dalStartInterface(eDAL_INTERFACE_1);

}

//======================================================================================
// Expander board specific setup
//======================================================================================

static bool myAppCheckForExpanderBoard(void)
{
	char read_buf[2];

	i2cWriteValue(0x49,0x01);
	i2cRead(0x49, &read_buf[0], 0x01);
	i2cWriteValue(0x4A,0x01);
	i2cRead(0x4A, &read_buf[1], 0x01);
	SYS_DEBUG(SYSDEBUG_TRACE_INIT, "check for expander board. %x %x\n", read_buf[0], read_buf[1]);
	if((read_buf[0]==0x01) && (read_buf[1]==0x01))
		return true;
	else
		return false;
}

static void myAppSetupi2c(void)
{
	i2cWrite2Value(0x49, 0x4, 0x0);
	i2cWrite2Value(0x49, 0x5, 0x1C);
	i2cWrite2Value(0x49, 0x18, 0x08);
	i2cWrite2Value(0x49, 0x1A, 0x07);
	
	i2cWrite2Value(0x4A, 0x4, 0x0);
	i2cWrite2Value(0x4A, 0x5, 0x1C);
	i2cWrite2Value(0x4A, 0x18, 0x08);
	i2cWrite2Value(0x4A, 0x1A, 0x07);
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

HRESULT myModeBypassMidi (uint32 bOn)
{
	return NO_ERROR;
}
