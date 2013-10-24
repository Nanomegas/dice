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
	            This example creates a simple break-out-box with the following
	            default configuration:
	            AES 8 channels I/O
	            Analog 4 channels I/O
	            ADAT 16/8/4 channels I/O
	
	Revisions:
		created 03/03/2007 ml
		

****************************************************************************/

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "dal.h"
#include "dalRates.h"
#include "diceDriver.h"
#include "coreDefs.h" //for GPIO definition for DIP switch
#include "targetBoard.h"
#include "dicedriverInterface.h"
#include "myApp.h"
#include "diceInS.h"
#include "eapCfg.h"
#include "standAlone.h"
#include "AKM4620.h"

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

// This example uses EAP to handle the configuration of streams and routing. The
// EAP will expose the mixer, router, channel configuration and peak to a control
// panel using a standard generic protocol.                                     

// It also shows how to utilize Stand Alone configuration modes.

bool devStandAlone=true;


// task (thread) ID for this module									
uint32						myAppThreadID = 0;		
const char*					myAppThreadName = "myAppThread";

//LEDmode determines if the board use LED for status or metering. 
static bool LEDmode=0; //if 0 LEDs show status otherwise they show meter

//we need a few other status things to handle the application specific LED stuff
static bool driverAttached;

// forward declaration
static HRESULT myDiceDriverCallBack (DICE_DRIVER_MSG_ENUM msg, DICE_DRIVER_CB_DATA * pData);

// update the status LED's, called whenever the status affecting the LED's has changed
static void updateStatusLEDs(void);


#define DEVICE_SUPPORTED_CLK_CAPS (	DD_CP_RATE_32   | DD_CP_RATE_44_1    |\
									DD_CP_RATE_48   | DD_CP_RATE_88_2    |\
									DD_CP_RATE_96   | DD_CP_RATE_176_4   |\
									DD_CP_RATE_192 |\
									DD_CP_SOURCE_AES1 | DD_CP_SOURCE_AES2 | DD_CP_SOURCE_AES3 | DD_CP_SOURCE_AES4 |\
									DD_CP_SOURCE_ARX1 | DD_CP_SOURCE_ARX2   |\
									DD_CP_SOURCE_INT  | DD_CP_SOURCE_WC | DD_CP_SOURCE_ADAT)

#define DEVICE_SUPPORTED_CLK_SRC_NAMES "AES34\\AES56\\AES78\\AES12(SPDIF)\\AES_ANY\\ADAT\\ADAT_AUX\\Word Clock\\Unused\\Unused\\Unused\\Unused\\Internal\\\\"

#define INPUT_DEVICES  (DEVICE_BIT(DEV_AES_RX) | \
	                    DEVICE_BIT(DEV_ADAT_RX) | \
	                    DEVICE_BIT(DEV_AVS_ADO1) | \
	                    DEVICE_BIT(DEV_AVS_ADO2) | \
	                    DEVICE_BIT(DEV_MIXER_RX) | \
	                    DEVICE_BIT(DEV_INS_RX0))

#define OUTPUT_DEVICES (DEVICE_BIT(DEV_AES_TX) | \
						DEVICE_BIT(DEV_ADAT_TX) | \
	                    DEVICE_BIT(DEV_AVS_ADI1) | \
	                    DEVICE_BIT(DEV_AVS_ADI2) | \
	                    DEVICE_BIT(DEV_INS_TX0))


// The default name our device will appear as under ASIO and CoreAudio
#define MY_DEVICE_NICK_NAME "EAP"



//===========================================================================
// Function to update codec when the dal locks to another rate mode
//
// The first initialization is assumed done elsewhere (myAppInitialize)
// 
//===========================================================================
//

static BOOL isLocked;
static DAL_RATE_MODE_ENUM lastLockedMode = eDAL_RATE_MODE_COUNT;

static void updateCodec(DAL_STATUS * pStatus)
{	
	if (((pStatus->state == eDAL_STATE_LOCKED) && !isLocked) || ((pStatus->state == eDAL_STATE_LOCKED) && (pStatus->lockedRateMode != lastLockedMode)))
	{
		//if in stand alone mode update the routing when the mode is changing
		if (devStandAlone) StandAloneUpdateRouting (pStatus->lockedRateMode);
		//we just obtained lock or the rate mode changed and we stayed in lock
		akm4620_msgFunc (MM_GOT_LOCK,pStatus->lockedRateMode,0);
		isLocked = true;
		if (!LEDmode)targetSetLED (TGT_LED4, TGT_LED_ON);
		lastLockedMode = pStatus->lockedRateMode;
	}
	else if ((pStatus->state != eDAL_STATE_LOCKED) && isLocked)
	{
		//we just lost lock
		akm4620_msgFunc (MM_LOST_LOCK,0,0);
		isLocked = false;
		if (!LEDmode)targetSetLED (TGT_LED4, TGT_LED_OFF);
	}
}





// We need to keep track of clock rate changes resulting in
// the rate mode changing so we can create a 'new' dal and
// a new router configuration

static DAL_RATE_MODE_ENUM lastDriverRateMode = eDAL_RATE_MODE_COUNT; 

static HRESULT myAppChangeSR(DICE_DRIVER_CB_DATA * pData)
{
	HRESULT hResult = NO_ERROR;
	uint32 notify = 0;

	if (dalRateByID (pData->rate)->rateMode != lastDriverRateMode)
	{
		lastDriverRateMode = dalRateByID (pData->rate)->rateMode;
		
		dalCreateInterface(eDAL_INTERFACE_1, lastDriverRateMode, INPUT_DEVICES,OUTPUT_DEVICES);
		eapDriverChangeRateMode (lastDriverRateMode);
	    dalStartInterface(eDAL_INTERFACE_1);
		//Send the RX and TX Configuration Notification
		notify |= DD_NOTIFY_RX_CFG_CHG_BIT  | DD_NOTIFY_TX_CFG_CHG_BIT;
	}
	//Change the Clock Now
	dalSetClockSource (eDAL_INTERFACE_1, pData->source, pData->rate);
	//Send the Clock Setting Accept Notification
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
		devStandAlone = false;
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
	DAL_CB_ADAT_LOCK_ACQUIRED | DAL_CB_ADAT_LOCK_LOST | DAL_CB_ADAT_AUX_LOCK_ACQUIRED | DAL_CB_ADAT_AUX_LOCK_LOST,
	DAL_CB_AVS1_SLIP | DAL_CB_AVS1_RPT | DAL_CB_AVS1_LOCK_ACQUIRED | 
	DAL_CB_AVS1_LOCK_LOST | DAL_CB_AVS1_ENABLE | DAL_CB_AVS1_DISABLE |
	DAL_CB_AVS2_SLIP | DAL_CB_AVS2_RPT | DAL_CB_AVS2_LOCK_ACQUIRED | 
	DAL_CB_AVS2_LOCK_LOST | DAL_CB_AVS2_ENABLE | DAL_CB_AVS2_DISABLE
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
	diceDriverSetStatus (dalStatus.state == eDAL_STATE_LOCKED, dalStatus.lockedNominalRate, 
		dalStatus.lockedRateHz, extStatus);
	
	// And make sure the codecs gets updated	
	updateCodec(&dalStatus);

	// why don't we indicate ADAT and AES lock on LED's
	if (!LEDmode)targetSetLED (TGT_LED5, aesIsMasterLocked () ? TGT_LED_ON : TGT_LED_OFF);
	if (!LEDmode)targetSetLED (TGT_LED6, adatIsLocked(0) ? TGT_LED_ON : TGT_LED_OFF);
	if (!LEDmode)targetSetLED (TGT_LED7, adatIsLocked(1) ? TGT_LED_ON : TGT_LED_OFF);
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
	targetSetLED (TGT_LED5, adatIsLocked(0) ? TGT_LED_ON : TGT_LED_OFF);
	targetSetLED (TGT_LED6, adatIsLocked(1) ? TGT_LED_ON : TGT_LED_OFF);
}

		

static void myAppThread(void *dummy)
{	
	DO_FOREVER
	{	
		TCTaskWait(500);
				
		if (!LEDmode)
			targetSetLED (TGT_LED1, TGT_LED_TGL);
	}
}



#ifdef _AML
static void myAppAMLInitialize(void)
{
	uint32 uart1AmlRxId, uart1AmlTxId, avsAmlRx1Id, avsAmlTx1Id, avsAmlRx2Id, avsAmlTx2Id;
	uint32 port = 0;


	amlCreateDev(&avsAmlRx1Id, &avs_midi_dev_cbs, AML_DEV_RX, AVS_AML_RX1 + (port&0x7), "avsRx1Port0");		//rx1, port0
	amlCreateDev(&avsAmlTx1Id, &avs_midi_dev_cbs, AML_DEV_TX, AVS_AML_TX1 + (port&0x7), "avsTx1Port0");		//tx1, port0
#ifdef USE_UART1_FOR_MIDI
	amlCreateDev(&uart1AmlTxId,  &uart1_midi_dev_cbs, AML_DEV_TX, 0, "uart1_tx");
	amlCreateDev(&uart1AmlRxId,  &uart1_midi_dev_cbs, AML_DEV_RX, 0, "uart1_rx");

	amlSetDestination(uart1AmlRxId, avsAmlTx1Id);
	amlSetDestination(avsAmlRx1Id, uart1AmlTxId);
#endif


	amlCreateDev(&avsAmlRx2Id, &avs_midi_dev_cbs, AML_DEV_RX, AVS_AML_RX2 + (port&0x7), "avsRx2Port0");		//rx2, port0
	amlCreateDev(&avsAmlTx2Id, &avs_midi_dev_cbs, AML_DEV_TX, AVS_AML_TX2 + (port&0x7), "avsTx2Port0");		//tx2, port0

	amlStartDev(avsAmlTx1Id);
	amlStartDev(avsAmlRx1Id);

#ifdef USE_UART1_FOR_MIDI
	amlStartDev(uart1AmlTxId);
	amlStartDev(uart1AmlRxId);
#endif
}

#endif



/*********************************************************
	Initializations for this module
*/

// This function is called when the system is initializing. At this point no threads are running and no audio is
// passed. When this function returns all threads are started. It is not legal to wait on semaphores etc at this
// point in the initialization sequence.

HRESULT	myAppInitialize(void)
{
	HRESULT hResult = NO_ERROR;	

	//Enable Automatic Hi/Lo detection in AES receiver for stand-alone mode.
	aesRxAutoHiLo (TRUE);	

	targetSpiInit (); //initialize SPI

	uint32 sw = targetGetInitialSW ();
	sysDebugPrintf("Initial SW Settings\r\n");
	sysDebugPrintf(" 1  2  3  4\r\n");
	sysDebugPrintf("[%c][%c][%c][%c]\r\n",(sw&1)?'1':'0',(sw&2)?'1':'0',(sw&4)?'1':'0',(sw&8)?'1':'0');

	//initialize audio ports
	akm4620_InitI2S (); 

	//configure static part of driver
	diceDriverConfigureInitial   (MY_DEVICE_NICK_NAME,
								  myDiceDriverCallBack,
							      DEVICE_SUPPORTED_CLK_CAPS,
							      DEVICE_SUPPORTED_CLK_SRC_NAMES);

	//Initialize eap.
	eapInitializeInterface(&eapInitDesc); //actually configures router, but that is overwritten by stand alone mode init later

	//create the call back from DAL
	dalInstallCallback(&myDalSubscribedEvents, 200, myDalCallBack, 0);
		
	//Creates the appropriate DAL
	StandAloneInitialize (); //routing still not done, will be done in DAL callback

	

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

HRESULT eapCallBack (EAP_EVT_ENUM event, uint32 data)
{
	//host has changed the persistent configuration
	//might act on it here or might just be polled.
	return NO_ERROR;
}

//Legacy function called by firmware loader module.
HRESULT myModeSetMode (uint32 mode)
{
	return NO_ERROR;
}

HRESULT myModeBypassMidi (uint32 bOn)
{
	return NO_ERROR;
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


////////// Debugging commands used from CLI for testing


HRESULT myAppSetStdAlone(uint32 sync, uint32 intRate)
{
	EAP_STND_ALONE_CFG * pStdaCfg;

	
	pStdaCfg = eapGetStandaloneCfg();
	pStdaCfg->clkSrc = sync;
	pStdaCfg->extADAT = 0;
	pStdaCfg->extAES = 0;
	pStdaCfg->extInt = intRate;
	return eapStoreFlash ();
}

HRESULT myAppDumpStdAlone(void)
{
	EAP_STND_ALONE_CFG const * pStdaCfg;
	pStdaCfg = eapGetStandaloneCfg();
	
	
	cliPrintf("STAND ALONE CONFIGURATION\n\r");
	cliPrintf("Verbose debug:\n\r");
	cliPrintf("\tpStdaCfg->clkSrc   : %i\n\r",pStdaCfg->clkSrc );
	cliPrintf("\tpStdaCfg->extADAT  : %i\n\r",pStdaCfg->extADAT);
	cliPrintf("\tpStdaCfg->extAES   : %i\n\r",pStdaCfg->extAES );
	cliPrintf("\tpStdaCfg->extInt   : %i\n\r",pStdaCfg->extInt );
	cliPrintf("\tpStdaCfg->extWC    : %i\n\r",pStdaCfg->extWC);
	cliPrintf("\n\r");

	switch (pStdaCfg->clkSrc)
	{
		case DD_CLK_SRC_AES1:
			cliPrintf("\tsync    : spdif\n\r");
			cliPrintf("\trate    : %s\n\r","32k-192k");
			break;
		case DD_CLK_SRC_ADAT:
			cliPrintf("\tsync    : adat\n\r");
			cliPrintf("\tsmux    : %s\n\r",(pStdaCfg->extADAT==0) ? "none":
										  (pStdaCfg->extADAT==1) ? "SMUX II":
										  (pStdaCfg->extADAT==3) ? "SMUX IV":
										  "illegal");
			break;
		case DD_CLK_SRC_WC:
			cliPrintf("\tsync    : Word Clock\n\r");
			cliPrintf("\trate    : %s\n\r","32k-192k");
			break;
		
		case DD_CLK_SRC_INTERNAL:
			cliPrintf("\tsync    : internal\n\r");
			switch (pStdaCfg->extInt)
			{
				case eDAL_NOMINAL_RATE_32:		cliPrintf("\trate    : 32k\n\r");break;
				case eDAL_NOMINAL_RATE_44_1:	cliPrintf("\trate    : 44.1k\n\r");break;
				case eDAL_NOMINAL_RATE_48:		cliPrintf("\trate    : 48k\n\r");break;
				case eDAL_NOMINAL_RATE_88_2:	cliPrintf("\trate    : 88.2k\n\r");break;
				case eDAL_NOMINAL_RATE_96:		cliPrintf("\trate    : 96k\n\r");break;
				case eDAL_NOMINAL_RATE_176_4:	cliPrintf("\trate    : 176.4k\n\r");break;
				case eDAL_NOMINAL_RATE_192:		cliPrintf("\trate    : 192k\n\r");break;
				default: cliPrintf("\trate    : Illegal\n\r");break;
			}
			break;

		default:
			cliPrintf("\tsync    : Illegal\n\r");
	}
	cliPrintf("\n\r");
	return NO_ERROR;
}

