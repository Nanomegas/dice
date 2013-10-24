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
										DD_CP_RATE_48   | DD_CP_SOURCE_AES1    |\
										DD_CP_SOURCE_AES2 | DD_CP_SOURCE_AES3	   |\
										DD_CP_SOURCE_AES4 | DD_CP_SOURCE_AES_ANY |\
										DD_CP_SOURCE_ADAT | DD_CP_SOURCE_TDIF    |\
										DD_CP_SOURCE_WC   | DD_CP_SOURCE_ARX1    |\
										DD_CP_SOURCE_ARX2 | DD_CP_SOURCE_INT       )

#define MY_DEVICE_SUPPORTED_CLK_SRC_NAMES "SPDIF\\AES\\AES2\\AES3\\AES_ANY\\ADAT\\TDIF\\Word Clock\\Unused\\Unused\\Unused\\Unused\\Internal\\\\"

#define MY_INPUT_DEVICES  (DEVICE_BIT(DEV_AES_RX) | DEVICE_BIT(DEV_ADAT_RX) | \
	                       DEVICE_BIT(DEV_AVS_ADO1) | DEVICE_BIT(DEV_AVS_ADO2) | DEVICE_BIT(DEV_APB_RX) | \
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
	{12,    0,    "SPDIF L\\SPDIF R\\AES L\\AES R\\AES2 L\\AES2 R\\AES3 L\\AES3 R\\ADAT1\\ADAT2\\ADAT3\\ADAT4\\\\",0x0000},
	{2,     0,    "SPDIF L\\SPDIF R\\\\",0x0000}
};


#define MY_NB_RX_ISOC_STREAMS_LOW 1
static STREAM_CONFIG myDriverRxConfigLow[MY_NB_RX_ISOC_STREAMS_LOW] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{2,    0,    "SPDIF L\\SPDIF R\\\\",0x0000}

};

static HRESULT routeLow (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_AES_CH0_7, RX_AVS1_CH0_7);

	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_7,  RX_AES_CH0_7);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH8_15, RX_ADAT_CH0_7);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS2_CH0_7,  RX_AES_CH0_7);
	return NO_ERROR;
}


static STREAM_CFG myDriverConfigureLow =
{
	myDriverTxConfigLow,
	MY_NB_TX_ISOC_STREAMS_LOW,
	DICE_TX_AES_ROUTE_LATENCY_LOW,

	myDriverRxConfigLow,
	MY_NB_RX_ISOC_STREAMS_LOW,
	DICE_RX_AES_ROUTE_LATENCY_LOW,
};


// Configuration of 'low' mode for expander case
#define MY_NB_TX_ISOC_STREAMS_LOW_EXP 2

static STREAM_CONFIG myDriverTxConfigLowExp[MY_NB_TX_ISOC_STREAMS_LOW_EXP] =
{
	{12,    0,    "Analog 01\\Analog 02\\Analog 03\\Analog 04\\Analog 11\\Analog 12\\Analog 13\\Analog 14\\Analog 05\\Analog 06\\Analog 09\\Analog 10\\\\",0x0000},
	{2,     0,    "SPDIF L\\SPDIF R\\\\",0x0000}
};

#define MY_NB_RX_ISOC_STREAMS_LOW_EXP 1
static STREAM_CONFIG myDriverRxConfigLowExp[MY_NB_RX_ISOC_STREAMS_LOW_EXP] =
{
	{2,    0,    "Analog 01\\Analog 02\\\\",0x0000}
};

static HRESULT routeLowExp (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_I2S1_CH0_3, RX_AVS1_CH0_3);

	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_3,  RX_I2S1_CH0_3);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH4_7,  RX_I2S2_CH0_3);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH8_15, RX_I2S0_CH0_7);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS2_CH0_7,  RX_AES_CH0_7);
	return NO_ERROR;
}

static STREAM_CFG myDriverConfigureLowExp =
{
	myDriverTxConfigLowExp,
	MY_NB_TX_ISOC_STREAMS_LOW_EXP,
	DICE_TX_AES_ROUTE_LATENCY_LOW,

	myDriverRxConfigLowExp,
	MY_NB_RX_ISOC_STREAMS_LOW_EXP,
	DICE_RX_AES_ROUTE_LATENCY_LOW,
};




//===========================================================================
// MID MODE CONFIGURATIONS
//===========================================================================

#define MY_NB_TX_ISOC_STREAMS_MID 2
static STREAM_CONFIG myDriverTxConfigMid[MY_NB_TX_ISOC_STREAMS_MID] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"	
    {6,    0,    "SPDIF L\\SPDIF R\\AES L\\AES R\\AES2 L\\AES2 R\\\\",0x0000},
	{4,    0,    "ADAT1\\ADAT2\\ADAT3\\ADAT4\\\\",0x0000}
};

#define MY_NB_RX_ISOC_STREAMS_MID 1
static STREAM_CONFIG myDriverRxConfigMid[MY_NB_RX_ISOC_STREAMS_MID] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
    {8,    0,    "SPDIF L\\SPDIF R\\AES L\\AES R\\AES2 L\\AES2 R\\AES3 L\\AES3 R\\\\",0x00ff}
};

static HRESULT routeMid (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_AES_CH0_7, RX_AVS1_CH0_7);		
	
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_7, RX_AES_CH0_7);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS2_CH0_3, RX_ADAT_CH0_3);
	return NO_ERROR;
}

static STREAM_CFG myDriverConfigureMid =
{
	myDriverTxConfigMid,
	MY_NB_TX_ISOC_STREAMS_MID,
	DICE_TX_AES_ROUTE_LATENCY_MID,

	myDriverRxConfigMid,
	MY_NB_RX_ISOC_STREAMS_MID,
	DICE_RX_AES_ROUTE_LATENCY_MID,
};

#define MY_NB_TX_ISOC_STREAMS_MID_EXP 2
static STREAM_CONFIG myDriverTxConfigMidExp[MY_NB_TX_ISOC_STREAMS_MID_EXP] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{6,     0,    "Analog 01\\Analog 02\\Analog 03\\Analog 04\\Analog 05\\Analog 06\\\\",0x0000},
	{4,     0,    "Analog 11\\Analog 12\\Analog 13\\Analog 14\\\\",0x0000}
};
#define MY_NB_RX_ISOC_STREAMS_MID_EXP 1
static STREAM_CONFIG myDriverRxConfigMidExp[MY_NB_RX_ISOC_STREAMS_MID_EXP] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{8,     0,    "Analog 01\\Analog 02\\Analog 03\\Analog 04\\Analog 11\\Analog 12\\Analog 13\\Analog 14\\Analog 05\\Analog 06\\Analog 09\\Analog 10\\Analog 07\\Analog 08\\Analog 15\\Analog 16\\\\",0x0000}
};


static HRESULT routeMidExp (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_I2S1_CH0_3, RX_AVS1_CH0_3);
	dalSetRoute(eDAL_INTERFACE_1, TX_I2S2_CH0_3, RX_AVS1_CH4_7);
	
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_3, RX_I2S1_CH0_3);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH4_7, RX_I2S2_CH0_3);
    dalSetRoute(eDAL_INTERFACE_1, TX_AVS2_CH0_7, RX_I2S0_CH0_7);
	return NO_ERROR;
}

static STREAM_CFG myDriverConfigureMidExp =
{
	myDriverTxConfigMidExp,
	MY_NB_TX_ISOC_STREAMS_MID_EXP,
	DICE_TX_AES_ROUTE_LATENCY_MID,

	myDriverRxConfigMidExp,
	MY_NB_RX_ISOC_STREAMS_MID_EXP,
	DICE_RX_AES_ROUTE_LATENCY_MID,
};


//===========================================================================
// HIGH MODE CONFIGURATIONS
//===========================================================================

#define MY_NB_TX_ISOC_STREAMS_HIGH 1
static STREAM_CONFIG myDriverTxConfigHigh[MY_NB_TX_ISOC_STREAMS_HIGH] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{4,    0,    "SPDIF\\AES\\AES2\\AES3\\\\",0x0000}
};

#define MY_NB_RX_ISOC_STREAMS_HIGH 1
static STREAM_CONFIG myDriverRxConfigHigh[MY_NB_RX_ISOC_STREAMS_HIGH] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{8,    0,    "SPDIF\\AES\\AES2\\AES3\\nowhere1\\nowhere2\\nowhere3\\nowhere4\\\\",0x0000}
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
	DICE_TX_AES_ROUTE_LATENCY_HIGH,

	myDriverRxConfigHigh,
	MY_NB_RX_ISOC_STREAMS_HIGH,
	DICE_RX_AES_ROUTE_LATENCY_HIGH,
};



#define MY_NB_TX_ISOC_STREAMS_HIGH_EXP 1
static STREAM_CONFIG myDriverTxConfigHighExp[MY_NB_TX_ISOC_STREAMS_HIGH_EXP] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{4,    0,    "Analog 01\\Analog 02\\Analog 03\\Analog 04\\\\",0x0000}
};

#define MY_NB_RX_ISOC_STREAMS_HIGH_EXP 1
static STREAM_CONFIG myDriverRxConfigHighExp[MY_NB_RX_ISOC_STREAMS_HIGH_EXP] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{8,    0,    "Analog 01\\Analog 02\\Analog 03\\Analog 04\\Analog 11\\Analog 12\\Analog 13\\Analog 14\\\\",0x0000}
};


static HRESULT routeHighExp (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_I2S1_CH0_3, RX_AVS1_CH0_3);
	dalSetRoute(eDAL_INTERFACE_1, TX_I2S2_CH0_3, RX_AVS1_CH4_7);
	
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_3, RX_I2S1_CH0_3);
	return NO_ERROR;
}

static STREAM_CFG myDriverConfigureHighExp =
{
	myDriverTxConfigHighExp,
	MY_NB_TX_ISOC_STREAMS_HIGH_EXP,
	DICE_TX_AES_ROUTE_LATENCY_HIGH,

	myDriverRxConfigHighExp,
	MY_NB_RX_ISOC_STREAMS_HIGH_EXP,
	DICE_RX_AES_ROUTE_LATENCY_HIGH,
};


//===========================================================================
// GENERIC STUFF
//===========================================================================


extern MODE_CFG modeCfg6;

static uint32 bInitFlags;

static HRESULT initMode(uint32 initFlags)
{
  bInitFlags = initFlags;
  if (initFlags & MODE_INIT_EXPN)
  {
    modeCfg6.low = &myDriverConfigureLowExp;
    modeCfg6.mid = &myDriverConfigureMidExp;
    modeCfg6.high = &myDriverConfigureHighExp;
  }
  if (initFlags & MODE_INIT_MIDI)
  {
    modeCfg6.low->pTxStreamCfg[0].midiChannels = 1;
    modeCfg6.low->pRxStreamCfg[0].midiChannels = 1;
    modeCfg6.mid->pTxStreamCfg[0].midiChannels = 1;
    modeCfg6.mid->pRxStreamCfg[0].midiChannels = 1;
    modeCfg6.high->pTxStreamCfg[0].midiChannels = 1;
    modeCfg6.high->pRxStreamCfg[0].midiChannels = 1;  
  } 
  else
  {
    modeCfg6.low->pTxStreamCfg[0].midiChannels = 0;
    modeCfg6.low->pRxStreamCfg[0].midiChannels = 0;
    modeCfg6.mid->pTxStreamCfg[0].midiChannels = 0;
    modeCfg6.mid->pRxStreamCfg[0].midiChannels = 0;
    modeCfg6.high->pTxStreamCfg[0].midiChannels = 0;
    modeCfg6.high->pRxStreamCfg[0].midiChannels = 0;  
  } 
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


MODE_CFG modeCfg6 = {
  &myDriverConfigureLow,
  &myDriverConfigureMid,
  &myDriverConfigureHigh,
  MY_INPUT_DEVICES,
  MY_OUTPUT_DEVICES,
  MY_DEVICE_SUPPORTED_CLK_CAPS,
  MY_DEVICE_SUPPORTED_CLK_SRC_NAMES,
  routeMode,
  initMode,
  "IO 2/12 (MM)"
};


