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
	
	Purpose:	This is an example of a very simple connection management system.
	            The transmitters can be configured by a controller to use a specific
	            isoc channel and receivers can be configured to receive on
	            specific channels. The system contains simple internal routing control
	            as well.
	
	Revisions:
		

****************************************************************************/

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "dal.h"
#include "dalRates.h"
#include "diceRouterReg.h"
#include "coreDefs.h" //for GPIO definition for DIP switch
#include "targetBoard.h"
#include "myApp.h"
#include "diceI2S.h"
#include "cms.h"
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

// We need to define how we present ourselves to the PC and Mac driver. We will
// need to specify our Isco channel layout, names for ASIO and CoreAudio etc.
// Starting and stopping the 1394 Isoc streams will be handled by the DiceDriver
// module.
//
// This file uses myModes to select between a number of configurations of the board
// The current mode is stored in the EEPROM unless MYAPP_MODE0_ONLY is defined in 
// which case the myMode0.c file defines the mode.


uint32						myAppThreadID = 0;		
const char*					myAppThreadName = "myAppThread";




///////////////////////// CMS Configuration

static uint8 cmsIndicateCnt;

static HRESULT myCmsCallback (CMS_EVT_ENUM event, uint32 data)
{
	switch (event)
	{
		case eCMS_EVT_INDICATE:
			cmsIndicateCnt = (uint8)data&0x0F;
			break;
		case eCMS_EVT_PERSIST_HOST_CHG: break;
		case eCMS_EVT_RATEMODE_CHG:break;
	}
	return NO_ERROR;
}


#define INPUT_DEVICES  (DEVICE_BIT(DEV_AES_RX) | \
	                    DEVICE_BIT(DEV_AVS_ADO1) | DEVICE_BIT(DEV_AVS_ADO2) | DEVICE_BIT(DEV_AVS_ADO3) | DEVICE_BIT(DEV_AVS_ADO4) | \
	                    DEVICE_BIT(DEV_I2S_RX0) | \
	                    DEVICE_BIT(DEV_ADAT_RX))

#define OUTPUT_DEVICES (DEVICE_BIT(DEV_AES_TX) | \
	                    DEVICE_BIT(DEV_AVS_ADI1) | DEVICE_BIT(DEV_AVS_ADI2) | \
	                    DEVICE_BIT(DEV_I2S_TX0) | \
	                    DEVICE_BIT(DEV_ADAT_TX))

static const SRC_DESCRIPTION analogSrc[] = {RX_I2S0_CH0_7};
static const SRC_DESCRIPTION aesSrc[] = {RX_AES_CH0_7};
static const SRC_DESCRIPTION adatLowSrc[] = {RX_ADAT_CH0_7};
static const SRC_DESCRIPTION adatMidSrc[] = {RX_ADAT_CH0_3};
static const SRC_DESCRIPTION aesADATSrc[] = {RX_AES_CH0_7,RX_ADAT_CH0_7};
static const SRC_DESCRIPTION aesADATMidSrc[] = {RX_AES_CH0_7,RX_ADAT_CH0_3};
static const SRC_DESCRIPTION aesAnalogSrc[] = {RX_AES_CH0_7,RX_I2S0_CH0_7};

static const CMS_AUDIO_TX_CFG lowAudioCfgs[] = 
{
	{4,analogSrc},
	{8,aesSrc},
	{8,adatLowSrc},
	{16,aesADATSrc},
	{12,aesAnalogSrc}
};

static const CMS_COMM_TX_CFG anyCommCfgs[] =
{
	0,NULL
};
	

static const CMS_TX_CFGS txLowCfgs = 
{
	5, //number of audio modes
	1, //number of comm modes
	lowAudioCfgs,
	anyCommCfgs
};

static const CMS_AUDIO_TX_CFG midAudioCfgs[] = 
{
	{4,analogSrc},
	{8,aesSrc},
	{4,adatMidSrc},
	{12,aesADATMidSrc},
	{12,aesAnalogSrc}
};

static const CMS_TX_CFGS txMidCfgs = 
{
	5, //number of audio modes
	1, //number of comm modes
	midAudioCfgs,
	anyCommCfgs
};

//The output configurations in the different rate modes
#define DST(dev,ch) (((dev)<<4) | (ch))

static const CMS_AUDIO_DST_CFG dstLowCfg =
{
	4+8+8, //analog,aes,adat
	{DST(DEV_I2S_TX0,0),DST(DEV_I2S_TX0,1),DST(DEV_I2S_TX0,2),DST(DEV_I2S_TX0,3),
	 DST(DEV_AES_TX,0),DST(DEV_AES_TX,1),DST(DEV_AES_TX,2),DST(DEV_AES_TX,3),
	 DST(DEV_AES_TX,4),DST(DEV_AES_TX,5),DST(DEV_AES_TX,6),DST(DEV_AES_TX,7),
	 DST(DEV_ADAT_TX,0),DST(DEV_ADAT_TX,1),DST(DEV_ADAT_TX,2),DST(DEV_ADAT_TX,3),
	 DST(DEV_ADAT_TX,4),DST(DEV_ADAT_TX,5),DST(DEV_ADAT_TX,6),DST(DEV_ADAT_TX,7)
	}
};

static const CMS_AUDIO_DST_CFG dstMidCfg =
{
	4+8+4, //analog,aes,adat
	{DST(DEV_I2S_TX0,0),DST(DEV_I2S_TX0,1),DST(DEV_I2S_TX0,2),DST(DEV_I2S_TX0,3),
	 DST(DEV_AES_TX,0),DST(DEV_AES_TX,1),DST(DEV_AES_TX,2),DST(DEV_AES_TX,3),
	 DST(DEV_AES_TX,4),DST(DEV_AES_TX,5),DST(DEV_AES_TX,6),DST(DEV_AES_TX,7),
	 DST(DEV_ADAT_TX,0),DST(DEV_ADAT_TX,1),DST(DEV_ADAT_TX,2),DST(DEV_ADAT_TX,3),
	}
};

////////////////// Information for CLI only /////////////////////////////////
const char * lowModeNames[] =
{
	"Line1-4","AES","ADAT","AES + Line1-4","AES + ADAT"
};

const char * midModeNames[] =
{
	"Line1-4","AES","ADAT","AES + Line1-4","AES + ADAT"
};


const char * lowDstNames[] =
{
	"Line1","Line2","Line3","Line4",
	"AES1","AES2","AES3","AES4","AES5","AES6","AES7","AES8",
	"ADAT1","ADAT2","ADAT3","ADAT4","ADAT5","ADAT6","ADAT7","ADAT8"
};

const char * midDstNames[] =
{
	"Line1","Line2","Line3","Line4",
	"AES1","AES2","AES3","AES4","AES5","AES6","AES7","AES8",
	"ADAT1","ADAT2","ADAT3","ADAT4"
};

const CMS_TEXT_DESC textDesc =
{
	{lowModeNames,midModeNames,midModeNames},
	{lowDstNames,midDstNames,midDstNames}
};

static const CMS_INIT_DESC cmsInit =
{
	CMS_CAP_MAXRATE96K,
	"EVM002 CMS",
	"EVM002 CMS",
	{&txLowCfgs,&txMidCfgs,&txMidCfgs},
	{&dstLowCfg,&dstMidCfg,&dstMidCfg},
	{INPUT_DEVICES,INPUT_DEVICES,INPUT_DEVICES},
	{OUTPUT_DEVICES,OUTPUT_DEVICES,OUTPUT_DEVICES},
	myCmsCallback,
	0,
	NULL,
	&textDesc
};




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
		akm4620_msgFunc (MM_GOT_LOCK,pStatus->lockedRateMode,0);
		isLocked = true;
		targetSetLED (TGT_LED4, TGT_LED_ON);
		lastLockedMode = pStatus->lockedRateMode;
	}
	else if ((pStatus->state != eDAL_STATE_LOCKED) && isLocked)
	{
		//we just lost lock
		akm4620_msgFunc (MM_LOST_LOCK,0,0);
		isLocked = false;
		targetSetLED (TGT_LED4, TGT_LED_OFF);
	}
}




// The diceDriver module also needs to know when the status of "dal" is
// changing so it can adjust streams accordingly and so it can provide
// correct status information to the computer. We will install a callback
// function in "dal" to notify us of such changes
// this callback is also used to notify codec's so they can react to rate
// changes, lock and unlock

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
	DAL_CB_AVS1_SLIP | DAL_CB_AVS1_RPT | DAL_CB_AVS1_LOCK_ACQUIRED | 
	DAL_CB_AVS1_LOCK_LOST | DAL_CB_AVS1_ENABLE | DAL_CB_AVS1_DISABLE |
	DAL_CB_AVS2_SLIP | DAL_CB_AVS2_RPT | DAL_CB_AVS2_LOCK_ACQUIRED | 
	DAL_CB_AVS2_LOCK_LOST | DAL_CB_AVS2_ENABLE | DAL_CB_AVS2_DISABLE |
	DAL_CB_AVS3_SLIP | DAL_CB_AVS3_RPT | DAL_CB_AVS3_LOCK_ACQUIRED | 
	DAL_CB_AVS3_LOCK_LOST | DAL_CB_AVS3_ENABLE | DAL_CB_AVS3_DISABLE |
	DAL_CB_AVS4_SLIP | DAL_CB_AVS4_RPT | DAL_CB_AVS4_LOCK_ACQUIRED | 
	DAL_CB_AVS4_LOCK_LOST | DAL_CB_AVS4_ENABLE | DAL_CB_AVS4_DISABLE
};


static HRESULT myDalCallBack (DAL_CB_EVENT_MASK events, DAL_EVENTS * pExtEvents, uint32 data)
{
	uint16			aesStatus;
	uint32			extStatus=0; //extended status to the driver
	DAL_STATUS		dalStatus;
	
	// At last the most important thing, get the "dal" status and call diceDriver
	dalGetCurrentStatus  (eDAL_INTERFACE_1, &dalStatus);
	//diceDriverSetStatus (dalStatus.state == eDAL_STATE_LOCKED, dalStatus.lockedNominalRate, 
	//	dalStatus.lockedRateHz, extStatus);
	
	// And make sure the codecs gets updated
	updateCodec(&dalStatus);

	// why don't we indicate ADAT and AES lock on LED's
	targetSetLED (TGT_LED5, aesIsMasterLocked () ? TGT_LED_ON : TGT_LED_OFF);
	targetSetLED (TGT_LED6, adatIsLocked ? TGT_LED_ON : TGT_LED_OFF);
	return NO_ERROR;
}






static void myAppThread(void *dummy)
{	
	BOOL everyOther=FALSE;
	DO_FOREVER
	{	
		TCTaskWait(250);
		everyOther = !everyOther;
		if (everyOther)
			targetSetLED (TGT_LED1, TGT_LED_TGL);
		if (cmsIndicateCnt)
		{
			targetSetLED (TGT_LED2, TGT_LED_TGL);
			cmsIndicateCnt--;
		}
		else
			targetSetLED (TGT_LED2, TGT_LED_OFF);
		
	}
}

/*********************************************************
	Initializations for this module
*/

#ifdef _AML

//This function creates virtual and physical MIDI ports and link them together
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
		
	//create the call back from DAL
	dalInstallCallback(&myDalSubscribedEvents, 200, myDalCallBack, 0);
	
	//Create Low Rate DAL for default initialization
	cmsInitializeInterface(&cmsInit);

#ifdef _AML
	myAppAMLInitialize();
#endif

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

// Called from the driver test interface implemented in frmvload.c
// If your application does not inplement modes just leave this
// function empty and return NO_ERROR; This function must be
// defined otherwise a link error will be generated.

HRESULT myModeSetMode (uint32 mode)
{
	//the special 'firmwareLoader' command is setting the mode
	return NO_ERROR;
}

// Called from the driver test interface implemented in frmvload.c
// It is highly recomended that this function is implemented as
// it allows some test diagnostic tools to run on the host computer.
// This function must be defined otherwise a link error will be generated.
//
HRESULT myModeBypassMidi (uint32 bOn)
{
	return NO_ERROR;
}


