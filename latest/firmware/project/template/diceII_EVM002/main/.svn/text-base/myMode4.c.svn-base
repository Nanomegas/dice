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
	
	Purpose:	myMode0.c, this is the channel configuration for DICE II
	            in mode 4.
	            Configuration:  16x16 I2S @32k-48k
								16x16 I2S @88.2k-96k
								16x16 I2S @176.4k-192k
	
	Revisions:
		created 13/06/2007 ml
		

****************************************************************************/

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "dal.h"
#include "diceDriver.h"
#include "targetBoard.h"
#include "dicedriverInterface.h"
#include "myModes.h"
#include "AKM4620.h"




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
										DD_CP_SOURCE_ADAT |\
										DD_CP_SOURCE_WC   | DD_CP_SOURCE_ARX1    |\
										DD_CP_SOURCE_ARX2 | DD_CP_SOURCE_INT       )

#define MY_DEVICE_SUPPORTED_CLK_SRC_NAMES "AES12/SPDIF\\AES34\\AES56\\AES78\\AES_ANY\\ADAT\\Unused\\Word Clock\\Unused\\Unused\\Unused\\Unused\\Internal\\\\"


#define MY_INPUT_DEVICES  (DEVICE_BIT(DEV_AES_RX) | DEVICE_BIT(DEV_ADAT_RX) | \
	                       DEVICE_BIT(DEV_AVS_ADO1) | DEVICE_BIT(DEV_AVS_ADO2) | \
	                       DEVICE_BIT(DEV_I2S_RX0) | DEVICE_BIT(DEV_I2S_RX1) | DEVICE_BIT(DEV_I2S_RX2))

#define MY_OUTPUT_DEVICES (DEVICE_BIT(DEV_AES_TX) | DEVICE_BIT(DEV_ADAT_TX) | \
	                       DEVICE_BIT(DEV_AVS_ADI1) | DEVICE_BIT(DEV_AVS_ADI2) | \
	                       DEVICE_BIT(DEV_I2S_TX0) | DEVICE_BIT(DEV_I2S_TX1) | DEVICE_BIT(DEV_I2S_TX2))


//===========================================================================
// LOW MODE CONFIGURATIONS
//===========================================================================

#define MY_NB_TX_ISOC_STREAMS_LOW 1
static STREAM_CONFIG myDriverTxConfigLow[MY_NB_TX_ISOC_STREAMS_LOW] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{16,   1,    "I2S1-0\\I2S1-1\\I2S1-2\\I2S1-3\\I2S2-0\\I2S2-1\\I2S2-2\\I2S2-3\\I2S0-0\\I2S0-1\\I2S0-2\\I2S0-3\\I2S0-4\\I2S0-5\\I2S0-6\\I2S0-7\\\\",0x0000}
};


#define MY_NB_RX_ISOC_STREAMS_LOW 1
static STREAM_CONFIG myDriverRxConfigLow[MY_NB_RX_ISOC_STREAMS_LOW] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"	
	{16,   1,    "I2S1-0\\I2S1-1\\I2S1-2\\I2S1-3\\I2S2-0\\I2S2-1\\I2S2-2\\I2S2-3\\I2S0-0\\I2S0-1\\I2S0-2\\I2S0-3\\I2S0-4\\I2S0-5\\I2S0-6\\I2S0-7\\\\",0x0000}
};

static HRESULT routeLow (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_I2S1_CH0_3,  RX_AVS1_CH0_3);
	dalSetRoute(eDAL_INTERFACE_1, TX_I2S2_CH0_3,  RX_AVS1_CH4_7);
	dalSetRoute(eDAL_INTERFACE_1, TX_I2S0_CH0_7,  RX_AVS1_CH8_15);

	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_3 , RX_I2S1_CH0_3);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH4_7 , RX_I2S2_CH0_3);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH8_15, RX_I2S0_CH0_7);
		
	return NO_ERROR;
}


static STREAM_CFG myDriverConfigureLow =
{
	myDriverTxConfigLow,
	MY_NB_TX_ISOC_STREAMS_LOW,
	myDriverRxConfigLow,
	MY_NB_RX_ISOC_STREAMS_LOW
};






//===========================================================================
// MID MODE CONFIGURATIONS
//===========================================================================

#define MY_NB_TX_ISOC_STREAMS_MID 1
static STREAM_CONFIG myDriverTxConfigMid[MY_NB_TX_ISOC_STREAMS_MID] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"	
	{16,   1,    "I2S1-0\\I2S1-1\\I2S1-2\\I2S1-3\\I2S2-0\\I2S2-1\\I2S2-2\\I2S2-3\\I2S0-0\\I2S0-1\\I2S0-2\\I2S0-3\\I2S0-4\\I2S0-5\\I2S0-6\\I2S0-7\\\\",0x0000}
};

#define MY_NB_RX_ISOC_STREAMS_MID 1
static STREAM_CONFIG myDriverRxConfigMid[MY_NB_RX_ISOC_STREAMS_MID] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{16,   1,    "I2S1-0\\I2S1-1\\I2S1-2\\I2S1-3\\I2S2-0\\I2S2-1\\I2S2-2\\I2S2-3\\I2S0-0\\I2S0-1\\I2S0-2\\I2S0-3\\I2S0-4\\I2S0-5\\I2S0-6\\I2S0-7\\\\",0x0000}
};

static HRESULT routeMid (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_I2S1_CH0_3,  RX_AVS1_CH0_3);
	dalSetRoute(eDAL_INTERFACE_1, TX_I2S2_CH0_3,  RX_AVS1_CH4_7);
	dalSetRoute(eDAL_INTERFACE_1, TX_I2S0_CH0_7,  RX_AVS1_CH8_15);

	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_3 , RX_I2S1_CH0_3);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH4_7 , RX_I2S2_CH0_3);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH8_15, RX_I2S0_CH0_7);
	return NO_ERROR;
}

static STREAM_CFG myDriverConfigureMid =
{
	myDriverTxConfigMid,
	MY_NB_TX_ISOC_STREAMS_MID,
	myDriverRxConfigMid,
	MY_NB_RX_ISOC_STREAMS_MID
};



//===========================================================================
// HIGH MODE CONFIGURATIONS
//===========================================================================

#define MY_NB_TX_ISOC_STREAMS_HIGH 2
static STREAM_CONFIG myDriverTxConfigHigh[MY_NB_TX_ISOC_STREAMS_HIGH] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{8,   1,    "I2S1-0\\I2S1-1\\I2S1-2\\I2S1-3\\I2S2-0\\I2S2-1\\I2S2-2\\I2S2-3\\\\",0x0000},
	{4,   0,    "I2S0-0\\I2S0-1\\I2S0-2\\I2S0-3\\\\",0x0000}	
};

#define MY_NB_RX_ISOC_STREAMS_HIGH 2
static STREAM_CONFIG myDriverRxConfigHigh[MY_NB_RX_ISOC_STREAMS_HIGH] =
{
	{8,   1,    "I2S1-0\\I2S1-1\\I2S1-2\\I2S1-3\\I2S2-0\\I2S2-1\\I2S2-2\\I2S2-3\\\\",0x0000},
	{4,   0,    "I2S0-0\\I2S0-1\\I2S0-2\\I2S0-3\\\\",0x0000}	
};

static HRESULT routeHigh (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_I2S1_CH0_3,  RX_AVS1_CH0_3);
	dalSetRoute(eDAL_INTERFACE_1, TX_I2S2_CH0_3,  RX_AVS1_CH4_7);
	dalSetRoute(eDAL_INTERFACE_1, TX_I2S0_CH0_3,  RX_AVS2_CH0_3);

	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_3 , RX_I2S1_CH0_3);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH4_7 , RX_I2S2_CH0_3);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_3 , RX_I2S0_CH0_3);
	return NO_ERROR;
}

static STREAM_CFG myDriverConfigureHigh =
{
	myDriverTxConfigHigh,
	MY_NB_TX_ISOC_STREAMS_HIGH,
	myDriverRxConfigHigh,
	MY_NB_RX_ISOC_STREAMS_HIGH
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
		case eDAL_RATE_MODE_MID: return routeMid();
		case eDAL_RATE_MODE_HIGH: return routeHigh();
	}
	return NO_ERROR;
}

static const uint8 i2sLines[NUM_ID_I2S] = {4,2,2};

static HRESULT initMode(uint32 initFlags)
{
	uint8 i,c;
	bInitFlags = initFlags;
  
	for (i=I2S_ID0; i<NUM_ID_I2S; i++)
	{
		i2sRxSetMckFreq    (i, false);
		i2sRxSetM192       (i, false);
		i2sRxSetClock      (i, true, 0); //256 fs @32k-96k, 128fs @176.4-192k
		i2sRxSetClockInv   (i, true, true);
		i2sTxSetMckFreq    (i, false);
		i2sTxSetM192       (i, false);
		i2sTxSetClock      (i, true, 0); //256 fs @32k-96k, 128fs @176.4-192k
		i2sTxSetClockInv   (i, true, true);
		for (c=0; c<i2sLines[i]; c++)
		{
			i2sRxSetDataFormat (i, c, true, true, 0, true);
			i2sTxSetDataFormat (i, c, true, true, 0, true);
		}
	}	
	return NO_ERROR;
}

static HRESULT msgFunc (MYMODE_MSG msg, uint32 data0, uint32 data1)
{
	//this mode does nothing in this case.
	//Add code here to send commands to codec's or change the 
	//MCLK.
	uint8 i;
	
	switch (msg)
	{
		case MM_LOST_LOCK:break;				
		case MM_GOT_LOCK:
			switch (data0)
			{
				default:
				case eDAL_RATE_MODE_LOW: 	//32k-48k
				case eDAL_RATE_MODE_MID:    //88.2k-96k				
					for (i=I2S_ID0; i<NUM_ID_I2S; i++) i2sRxSetM192 (i, false);
					break;
					
				case eDAL_RATE_MODE_HIGH:   //176.4k-192k
					for (i=I2S_ID0; i<NUM_ID_I2S; i++) i2sRxSetM192 (i, true);
					break;
			}
			break;					
	}
	return NO_ERROR;
}

MODE_CFG modeCfg4 = {
	&myDriverConfigureLow,
	&myDriverConfigureMid,
	&myDriverConfigureHigh,
	MY_INPUT_DEVICES,
	MY_OUTPUT_DEVICES,
	MY_DEVICE_SUPPORTED_CLK_CAPS,
	MY_DEVICE_SUPPORTED_CLK_SRC_NAMES,
	routeMode,
	initMode,
	msgFunc,
	"DICE II EVM ALL I2S"
};


