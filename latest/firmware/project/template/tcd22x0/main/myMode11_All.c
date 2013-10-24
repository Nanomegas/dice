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
	
	Purpose:	myMode11_any.c, this is the channel configuration for both chips
	            in mode 11 the number of channels is configurable
	            Configuration:  32x32 Loopback @32k-48k
								32x32 Loopback @88.2k-96k
								16x16 Loopback @176.4k-192k
	
	Revisions:
		created 13/06/2007 ml
		

****************************************************************************/

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "dal.h"

#include "diceDriver.h"
#include "dicedriverInterface.h"

#include "targetBoard.h"
#include "myModes.h"
#include "AKM4620.h"
#include "myApp.h"




// We need to define how we present ourselves to the PC and Mac driver. We will
// need to specify our Isco channel layout, names for ASIO and CoreAudio etc.
// Starting and stopping the 1394 Isoc streams will be handled by the DiceDriver
// module.
//


// The settings below are shared for all configurations within this mode.

#define MY_DEVICE_SUPPORTED_CLK_CAPS (	DD_CP_RATE_32   | DD_CP_RATE_44_1    |\
										DD_CP_RATE_48   | DD_CP_RATE_88_2    |\
										DD_CP_RATE_96   | DD_CP_RATE_176_4   |\
										DD_CP_RATE_192  | DD_CP_SOURCE_AES1    |\
										DD_CP_SOURCE_AES2 | DD_CP_SOURCE_AES3	   |\
										DD_CP_SOURCE_AES4 | DD_CP_SOURCE_AES_ANY |\
										DD_CP_SOURCE_ADAT | DD_CP_SOURCE_ADATAUX  |\
										DD_CP_SOURCE_WC   | DD_CP_SOURCE_ARX1    |\
										DD_CP_SOURCE_ARX2 | DD_CP_SOURCE_INT       )

#define MY_DEVICE_SUPPORTED_CLK_SRC_NAMES "AES34\\AES56\\AES78\\AES12\\AES_ANY\\ADAT\\ADAT_AUX\\Word Clock\\Unused\\Unused\\Unused\\Unused\\Internal\\\\"

#define MY_INPUT_DEVICES  (DEVICE_BIT(DEV_AES_RX) | DEVICE_BIT(DEV_ADAT_RX) | \
	                       DEVICE_BIT(DEV_AVS_ADO1) | DEVICE_BIT(DEV_AVS_ADO2))

#define MY_OUTPUT_DEVICES (DEVICE_BIT(DEV_AVS_ADI1) | DEVICE_BIT(DEV_AVS_ADI2))



//===========================================================================
// LOW/MID MODE CONFIGURATIONS
//===========================================================================
static char lowTxNames[2][256];
static char lowRxNames[2][256];

static STREAM_CONFIG myDriverTxConfigLow[2] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{0,   0,    lowTxNames[0],0x0000},
	{0,   0,    lowTxNames[1],0x0000}
};

static STREAM_CONFIG myDriverRxConfigLow[2] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"	
	{0,   0,    lowRxNames[0],0x0000},
	{0,   0,    lowRxNames[1],0x0000}
};

static HRESULT routeLow (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_7,  RX_AVS1_CH0_7);		
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH8_15, RX_AVS1_CH8_15);

	dalSetRoute(eDAL_INTERFACE_1, TX_AVS2_CH0_7,  RX_AVS2_CH0_7);		
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS2_CH8_15, RX_AVS2_CH8_15);
	return NO_ERROR;
}


static STREAM_CFG myDriverConfigureLow =
{
	myDriverTxConfigLow,
	2,
	DICE_TX_AVS_ROUTE_LATENCY_LOW,

	myDriverRxConfigLow,
	2,
	DICE_RX_AVS_ROUTE_LATENCY_LOW + 1,		// +1 for loopback in the router
};


//===========================================================================
// HIGH MODE CONFIGURATIONS
//===========================================================================

static char highTxNames[2][256];
static char highRxNames[2][256];

static STREAM_CONFIG myDriverTxConfigHigh[2] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{0,   0,    highTxNames[0],0x0000},
	{0,   0,    highTxNames[1],0x0000}
};

static STREAM_CONFIG myDriverRxConfigHigh[2] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"	
	{0,   0,    highRxNames[0],0x0000},
	{0,   0,    highRxNames[1],0x0000}
};

static HRESULT routeHigh (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_7,  RX_AVS1_CH0_7);		
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH8_15, RX_AVS1_CH8_15);

	dalSetRoute(eDAL_INTERFACE_1, TX_AVS2_CH0_7,  RX_AVS2_CH0_7);		
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS2_CH8_15, RX_AVS2_CH8_15);
	return NO_ERROR;
}


static STREAM_CFG myDriverConfigureHigh =
{
	myDriverTxConfigHigh,
	2,
	DICE_TX_AVS_ROUTE_LATENCY_HIGH,

	myDriverRxConfigHigh,
	2,
	DICE_RX_AVS_ROUTE_LATENCY_HIGH + 1,		// +1 for loopback in the router
};


//===========================================================================
// GENERIC STUFF
//===========================================================================



static uint32 bInitFlags;

static HRESULT routeMode (DAL_RATE_MODE_ENUM ratemode)
{
	switch (ratemode)
	{
		default:
		case eDAL_RATE_MODE_LOW: return routeLow();
		case eDAL_RATE_MODE_MID: return routeLow();
		case eDAL_RATE_MODE_HIGH: return routeHigh();
	}
	return NO_ERROR;
}

extern MODE_CFG modeCfg11_All;

// do the names
static void createNames(STREAM_CONFIG * pCfg, char * prefix)
{
	uint8 i;
	char * str = pCfg->names;
	*str = 0;
	for (i=0; i<pCfg->AudioChannels; i++)
	{
		str+=sprintf(str,"%s-%u\\",prefix,i);
	}
	sprintf(str,"\\");
}


static HRESULT initMode(uint32 initFlags)
{
	
	bInitFlags = initFlags;

	targetChangeAudioPorts (APM_NORMAL);

	//now build the mode form the evmCfg
	uint32 cfg = evmCfg.mode11Cfg;
	uint8 nbaudio, nbmidi;
	
	nbaudio = (uint8)(cfg&0xf)*2;
	nbmidi = (uint8)((cfg>>4)&0x7);
	if (!nbaudio) nbaudio = 2; //always at least 2 channels
	myDriverTxConfigLow[0].AudioChannels = nbaudio;
	myDriverTxConfigLow[0].midiChannels = nbmidi;
	myDriverTxConfigHigh[0].midiChannels = nbmidi;
	if (nbaudio>8) nbaudio = 8;
	myDriverTxConfigHigh[0].AudioChannels = nbaudio;
	createNames(&myDriverTxConfigLow[0],"AVS1");
	createNames(&myDriverTxConfigHigh[0],"AVS1");
	myDriverConfigureLow.nbTxStreams = 1;
	myDriverConfigureHigh.nbTxStreams = 1;
	
	cfg>>=8;
	nbaudio = (uint8)(cfg&0xf)*2;
	nbmidi = (uint8)((cfg>>4)&0x7);
	myDriverTxConfigLow[1].AudioChannels = nbaudio;
	myDriverTxConfigLow[1].midiChannels = nbmidi;
	myDriverTxConfigHigh[1].midiChannels = nbmidi;
	if (nbaudio>8) nbaudio = 8;
	myDriverTxConfigHigh[1].AudioChannels = nbaudio;
	createNames(&myDriverTxConfigLow[1],"AVS2");
	createNames(&myDriverTxConfigHigh[1],"AVS2");
	
	if (nbaudio)
	{
		myDriverConfigureLow.nbTxStreams = 2;
		myDriverConfigureHigh.nbTxStreams = 2;
	}
	
	cfg>>=8;
	nbaudio = (uint8)(cfg&0xf)*2;
	nbmidi = (uint8)((cfg>>4)&0x7);
	if (!nbaudio) nbaudio = 2; //always at least 2 channels
	myDriverRxConfigLow[0].AudioChannels = nbaudio;
	myDriverRxConfigLow[0].midiChannels = nbmidi;
	myDriverRxConfigHigh[0].midiChannels = nbmidi;
	if (nbaudio>8) nbaudio = 8;
	myDriverRxConfigHigh[0].AudioChannels = nbaudio;
	createNames(&myDriverRxConfigLow[0],"AVS1");
	createNames(&myDriverRxConfigHigh[0],"AVS1");
	myDriverConfigureLow.nbRxStreams = 1;
	myDriverConfigureHigh.nbRxStreams = 1;

	cfg>>=8;
	nbaudio = (uint8)(cfg&0xf)*2;
	nbmidi = (uint8)((cfg>>4)&0x7);
	myDriverRxConfigLow[1].AudioChannels = nbaudio;
	myDriverRxConfigLow[1].midiChannels = nbmidi;
	myDriverRxConfigHigh[1].midiChannels = nbmidi;
	if (nbaudio>8) nbaudio = 8;
	myDriverRxConfigHigh[1].AudioChannels = nbaudio;
	createNames(&myDriverRxConfigLow[1],"AVS2");
	createNames(&myDriverRxConfigHigh[1],"AVS2");

	if (nbaudio)
	{
		myDriverConfigureLow.nbRxStreams = 2;
		myDriverConfigureHigh.nbRxStreams = 2;
	}

		
	myModesApplyAC3Out (&modeCfg11_All, initFlags & 0xffff, initFlags>>16);

	return NO_ERROR;
}

static HRESULT msgFunc (MYMODE_MSG msg, uint32 data0, uint32 data1)
{
	//this mode does nothing in this case.
	//Add code here to send commands to codec's or change the 
	//MCLK.
	switch (msg)
	{
		case MM_LOST_LOCK:break;				
		case MM_GOT_LOCK:
			switch (data0)
			{
				default:
				case eDAL_RATE_MODE_LOW: break;	//32k-48k
				case eDAL_RATE_MODE_MID: break;	//88.2k-96k				
				case eDAL_RATE_MODE_HIGH: break;//176.4k-192k
			}
			break;					
	}
	return NO_ERROR;
}

MODE_CFG modeCfg11_All = {
	&myDriverConfigureLow,
	&myDriverConfigureLow,
	&myDriverConfigureHigh,
	MY_INPUT_DEVICES,
	MY_OUTPUT_DEVICES,
	MY_DEVICE_SUPPORTED_CLK_CAPS,
	MY_DEVICE_SUPPORTED_CLK_SRC_NAMES,
	routeMode,
	initMode,
	msgFunc,
	"DICE EVM Configurable Test"
};


