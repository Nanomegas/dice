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
#include "targetBoard.h"
#include "dicedriverInterface.h"
#include "myModes.h"



// We need to define how we present ourselves to the PC and Mac driver. We will
// need to specify our Isco channel layout, names for ASIO and CoreAudio etc.
// Starting and stopping the 1394 Isoc streams will be handled by the DiceDriver
// module.
//
// This device is setup to stream ADAT and AES on individual Isoc channels for
// demonstration purposes. It could have been done utilizing one Isoc as well
// as the Isoc capability of the DICE II is 16 channels.


// The settings below are shared for all configurations within this mode.

#define MY_DEVICE_SUPPORTED_CLK_CAPS (	DD_CP_RATE_32   | DD_CP_RATE_44_1    |\
										DD_CP_RATE_48   | DD_CP_RATE_88_2    |\
										DD_CP_RATE_96   | DD_CP_RATE_176_4	 |\
										DD_CP_RATE_192	| DD_CP_SOURCE_AES1    |\
										DD_CP_SOURCE_AES2 | DD_CP_SOURCE_AES3	   |\
										DD_CP_SOURCE_AES4 | DD_CP_SOURCE_AES_ANY |\
										DD_CP_SOURCE_ADAT | DD_CP_SOURCE_TDIF    |\
										DD_CP_SOURCE_WC   | DD_CP_SOURCE_ARX1    |\
										DD_CP_SOURCE_ARX2 | DD_CP_SOURCE_INT       )

#define MY_DEVICE_SUPPORTED_CLK_SRC_NAMES "SPDIF\\AES\\AES2\\AES3\\AES_ANY\\ADAT\\TDIF\\Word Clock\\Unused\\Unused\\Unused\\Unused\\Internal\\\\"

#define MY_INPUT_DEVICES  (DEVICE_BIT(DEV_AES_RX) | DEVICE_BIT(DEV_ADAT_RX) | \
	                       DEVICE_BIT(DEV_AVS_ADO1) | DEVICE_BIT(DEV_AVS_ADO2) | DEVICE_BIT(DEV_APB_RX)| \
						   DEVICE_BIT(DEV_I2S_RX0) | DEVICE_BIT(DEV_I2S_RX1)| DEVICE_BIT(DEV_I2S_RX2))
#define MY_OUTPUT_DEVICES (DEVICE_BIT(DEV_AES_TX) | DEVICE_BIT(DEV_ADAT_TX) | \
	                       DEVICE_BIT(DEV_AVS_ADI1) | DEVICE_BIT(DEV_AVS_ADI2) | DEVICE_BIT(DEV_APB_TX)| \
	                       DEVICE_BIT(DEV_I2S_TX0) | DEVICE_BIT(DEV_I2S_TX1)| DEVICE_BIT(DEV_I2S_TX2))




//===========================================================================
// LOW MODE CONFIGURATIONS
//===========================================================================

// Configuration of 'low' mode for non expander case

#define MY_NB_TX_ISOC_STREAMS_LOW 2
static STREAM_CONFIG myDriverTxConfigLow[MY_NB_TX_ISOC_STREAMS_LOW] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{16,    2,    "AVS1-1\\AVS1-2\\AVS1-3\\AVS1-4\\AVS1-5\\AVS1-6\\AVS1-7\\AVS1-8\\AVS1-9\\AVS1-10\\AVS1-11\\AVS1-12\\AVS1-13\\AVS1-14\\AVS1-15\\AVS1-16\\\\",0x0000},
	{16,    2,    "AVS2-1\\AVS2-2\\AVS2-3\\AVS2-4\\AVS2-5\\AVS2-6\\AVS2-7\\AVS2-8\\AVS2-9\\AVS2-10\\AVS2-11\\AVS2-12\\AVS2-13\\AVS2-14\\AVS2-15\\AVS2-16\\\\",0x0000}
};


#define MY_NB_RX_ISOC_STREAMS_LOW 2
static STREAM_CONFIG myDriverRxConfigLow[MY_NB_RX_ISOC_STREAMS_LOW] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{16,    2,    "AVS1-1\\AVS1-2\\AVS1-3\\AVS1-4\\AVS1-5\\AVS1-6\\AVS1-7\\AVS1-8\\AVS1-9\\AVS1-10\\AVS1-11\\AVS1-12\\AVS1-13\\AVS1-14\\AVS1-15\\AVS1-16\\\\",0x0000},
	{16,    2,    "AVS2-1\\AVS2-2\\AVS2-3\\AVS2-4\\AVS2-5\\AVS2-6\\AVS2-7\\AVS2-8\\AVS2-9\\AVS2-10\\AVS2-11\\AVS2-12\\AVS2-13\\AVS2-14\\AVS2-15\\AVS2-16\\\\",0x0000}
};

static HRESULT routeLow (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_AES_CH0_7,  RX_AVS1_CH0_7);		
	dalSetRoute(eDAL_INTERFACE_1, TX_ADAT_CH0_7, RX_AVS1_CH8_15);
	
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_7, RX_AES_CH0_7);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH8_15, RX_ADAT_CH0_7);	
	return NO_ERROR;
}


static STREAM_CFG myDriverConfigureLow =
{
	myDriverTxConfigLow,
	MY_NB_TX_ISOC_STREAMS_LOW,
	DICE_TX_AVS_ROUTE_LATENCY_LOW,

	myDriverRxConfigLow,
	MY_NB_RX_ISOC_STREAMS_LOW,
	DICE_RX_AVS_ROUTE_LATENCY_LOW + 1,		// +1 for loopback in the router
};


// Configuration of 'low' mode for expander case

static HRESULT routeLowExp (void)
{
	return NO_ERROR;
}

static STREAM_CFG myDriverConfigureLowExp =
{
	myDriverTxConfigLow,
	MY_NB_TX_ISOC_STREAMS_LOW,
	DICE_TX_AVS_ROUTE_LATENCY_LOW,

	myDriverRxConfigLow,
	MY_NB_RX_ISOC_STREAMS_LOW,
	DICE_RX_AVS_ROUTE_LATENCY_LOW + 1,		// +1 for loopback in the router
};




//===========================================================================
// MID MODE CONFIGURATIONS
//===========================================================================

#define MY_NB_TX_ISOC_STREAMS_MID 2
static STREAM_CONFIG myDriverTxConfigMid[MY_NB_TX_ISOC_STREAMS_MID] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"	
	{16,    2,    "AVS1-1\\AVS1-2\\AVS1-3\\AVS1-4\\AVS1-5\\AVS1-6\\AVS1-7\\AVS1-8\\AVS1-9\\AVS1-10\\AVS1-11\\AVS1-12\\AVS1-13\\AVS1-14\\AVS1-15\\AVS1-16\\\\",0x0000},
	{16,    2,    "AVS2-1\\AVS2-2\\AVS2-3\\AVS2-4\\AVS2-5\\AVS2-6\\AVS2-7\\AVS2-8\\AVS2-9\\AVS2-10\\AVS2-11\\AVS2-12\\AVS2-13\\AVS2-14\\AVS2-15\\AVS2-16\\\\",0x0000}
};

#define MY_NB_RX_ISOC_STREAMS_MID 2
static STREAM_CONFIG myDriverRxConfigMid[MY_NB_RX_ISOC_STREAMS_MID] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{16,    2,    "AVS1-1\\AVS1-2\\AVS1-3\\AVS1-4\\AVS1-5\\AVS1-6\\AVS1-7\\AVS1-8\\AVS1-9\\AVS1-10\\AVS1-11\\AVS1-12\\AVS1-13\\AVS1-14\\AVS1-15\\AVS1-16\\\\",0x0000},
	{16,    2,    "AVS2-1\\AVS2-2\\AVS2-3\\AVS2-4\\AVS2-5\\AVS2-6\\AVS2-7\\AVS2-8\\AVS2-9\\AVS2-10\\AVS2-11\\AVS2-12\\AVS2-13\\AVS2-14\\AVS2-15\\AVS2-16\\\\",0x0000}
};

static HRESULT routeMid (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_AES_CH0_7,  RX_AVS1_CH0_7);		
	dalSetRoute(eDAL_INTERFACE_1, TX_ADAT_CH4_7, RX_AVS1_CH8_11);

	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_7, RX_AES_CH0_7);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH8_15,RX_ADAT_CH0_7);
	return NO_ERROR;
}

static STREAM_CFG myDriverConfigureMid =
{
	myDriverTxConfigMid,
	MY_NB_TX_ISOC_STREAMS_MID,
	DICE_TX_AVS_ROUTE_LATENCY_MID,

	myDriverRxConfigMid,
	MY_NB_RX_ISOC_STREAMS_MID,
	DICE_RX_AVS_ROUTE_LATENCY_MID + 1,		// +1 for loopback in the router
};


static HRESULT routeMidExp (void)
{
	return NO_ERROR;
}

static STREAM_CFG myDriverConfigureMidExp =
{
	myDriverTxConfigMid,
	MY_NB_TX_ISOC_STREAMS_MID,
	DICE_TX_AVS_ROUTE_LATENCY_MID,

	myDriverRxConfigMid,
	MY_NB_RX_ISOC_STREAMS_MID,
	DICE_RX_AVS_ROUTE_LATENCY_MID + 1,		// +1 for loopback in the router
};


//===========================================================================
// HIGH MODE CONFIGURATIONS
//===========================================================================

#define MY_NB_TX_ISOC_STREAMS_HIGH 2
static STREAM_CONFIG myDriverTxConfigHigh[MY_NB_TX_ISOC_STREAMS_HIGH] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{8,    2,    "AVS1-1\\AVS1-2\\AVS1-3\\AVS1-4\\AVS1-5\\AVS1-6\\AVS1-7\\AVS1-8\\\\",0x0000},
	{8,    2,    "AVS2-1\\AVS2-2\\AVS2-3\\AVS2-4\\AVS2-5\\AVS2-6\\AVS2-7\\AVS2-8\\\\",0x0000}
};

#define MY_NB_RX_ISOC_STREAMS_HIGH 2
static STREAM_CONFIG myDriverRxConfigHigh[MY_NB_RX_ISOC_STREAMS_HIGH] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{8,    2,    "AVS1-1\\AVS1-2\\AVS1-3\\AVS1-4\\AVS1-5\\AVS1-6\\AVS1-7\\AVS1-8\\\\",0x0000},
	{8,    2,    "AVS2-1\\AVS2-2\\AVS2-3\\AVS2-4\\AVS2-5\\AVS2-6\\AVS2-7\\AVS2-8\\\\",0x0000}
};

static HRESULT routeHigh (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_AES_CH0_3, RX_AVS1_CH0_3);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_3, RX_AES_CH0_3);
	return NO_ERROR;
}

static STREAM_CFG myDriverConfigureHigh =
{
	myDriverTxConfigHigh,
	MY_NB_TX_ISOC_STREAMS_HIGH,
	DICE_TX_AVS_ROUTE_LATENCY_HIGH,

	myDriverRxConfigHigh,
	MY_NB_RX_ISOC_STREAMS_HIGH,
	DICE_RX_AVS_ROUTE_LATENCY_HIGH + 1,		// +1 for loopback in the router
};



static HRESULT routeHighExp (void)
{
	return NO_ERROR;
}

static STREAM_CFG myDriverConfigureHighExp =
{
	myDriverTxConfigHigh,
	MY_NB_TX_ISOC_STREAMS_HIGH,
	DICE_TX_AVS_ROUTE_LATENCY_HIGH,

	myDriverRxConfigHigh,
	MY_NB_RX_ISOC_STREAMS_HIGH,
	DICE_RX_AVS_ROUTE_LATENCY_HIGH + 1,		// +1 for loopback in the router
};


//===========================================================================
// GENERIC STUFF
//===========================================================================


extern MODE_CFG modeCfg13;

static uint32 bInitFlags;

static HRESULT initMode(uint32 initFlags)
{
  bInitFlags = initFlags;
  if (initFlags & MODE_INIT_EXPN)
  {
    modeCfg13.low = &myDriverConfigureLowExp;
    modeCfg13.mid = &myDriverConfigureMidExp;
    modeCfg13.high = &myDriverConfigureHighExp;
  }
  /*
  if (initFlags & MODE_INIT_MIDI)
  {
    modeCfg13.low->pTxStreamCfg[0].midiChannels = 1;
    modeCfg13.low->pRxStreamCfg[0].midiChannels = 1;
    modeCfg13.mid->pTxStreamCfg[0].midiChannels = 1;
    modeCfg13.mid->pRxStreamCfg[0].midiChannels = 1;
    modeCfg13.high->pTxStreamCfg[0].midiChannels = 1;
    modeCfg13.high->pRxStreamCfg[0].midiChannels = 1;  
  } 
  else
  {
    modeCfg13.low->pTxStreamCfg[0].midiChannels = 0;
    modeCfg13.low->pRxStreamCfg[0].midiChannels = 0;
    modeCfg13.mid->pTxStreamCfg[0].midiChannels = 0;
    modeCfg13.mid->pRxStreamCfg[0].midiChannels = 0;
    modeCfg13.high->pTxStreamCfg[0].midiChannels = 0;
    modeCfg13.high->pRxStreamCfg[0].midiChannels = 0;  
  } 
  */
  return NO_ERROR;
}

static HRESULT routeMode (uint32 ratemode)
{
  
  switch (ratemode)
  {
    default:
    case 0: return (bInitFlags & MODE_INIT_EXPN) ? routeLowExp()  : routeLow();
    case 1: return (bInitFlags & MODE_INIT_EXPN) ? routeMidExp()  : routeMid();
    case 2: return (bInitFlags & MODE_INIT_EXPN) ? routeHighExp() : routeHigh();
  }
  
  return NO_ERROR;
}


MODE_CFG modeCfg13 = {
  &myDriverConfigureLow,
  &myDriverConfigureMid,
  &myDriverConfigureHigh,
  MY_INPUT_DEVICES,
  MY_OUTPUT_DEVICES,
  MY_DEVICE_SUPPORTED_CLK_CAPS,
  MY_DEVICE_SUPPORTED_CLK_SRC_NAMES,
  routeMode,
  initMode,
  "MIDI Test 32/32 (16/16)"
};


