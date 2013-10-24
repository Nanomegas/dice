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
	
	Purpose:	myMode7_Mini.c, this is the channel configuration for DICE Mini
	            in mode 7. Testmode, all channels looped back
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




// We need to define how we present ourselves to the PC and Mac driver. We will
// need to specify our Isco channel layout, names for ASIO and CoreAudio etc.
// Starting and stopping the 1394 Isoc streams will be handled by the DiceDriver
// module.
//


// The settings below are shared for all configurations within this mode.

#define MY_DEVICE_SUPPORTED_CLK_CAPS (	DD_CP_RATE_32   | DD_CP_RATE_44_1    |\
										DD_CP_RATE_48   | DD_CP_RATE_88_2    |\
										DD_CP_RATE_96   | DD_CP_RATE_176_4   |\
										DD_CP_RATE_192  | DD_CP_SOURCE_AES4    |\
										DD_CP_SOURCE_ADAT |\
										DD_CP_SOURCE_WC   | DD_CP_SOURCE_ARX1    |\
										DD_CP_SOURCE_ARX2 | DD_CP_SOURCE_INT      )

#define MY_DEVICE_SUPPORTED_CLK_SRC_NAMES "AES12\\AES34\\AES56\\SPDIF\\AES_ANY\\ADAT\\ADAT_AUX\\Word Clock\\Unused\\Unused\\Unused\\Unused\\Internal\\\\"

#define MY_INPUT_DEVICES  (DEVICE_BIT(DEV_AES_RX) | DEVICE_BIT(DEV_ADAT_RX) | \
	                       DEVICE_BIT(DEV_AVS_ADO1) | DEVICE_BIT(DEV_AVS_ADO2))

#define MY_OUTPUT_DEVICES (DEVICE_BIT(DEV_AVS_ADI1) | DEVICE_BIT(DEV_AVS_ADI2))



//===========================================================================
// LOW MODE CONFIGURATIONS
//===========================================================================

#define MY_NB_TX_ISOC_STREAMS_LOW 2
static STREAM_CONFIG myDriverTxConfigLow[MY_NB_TX_ISOC_STREAMS_LOW] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{16,   1,    "AVS1-1\\AVS1-2\\AVS1-3\\AVS1-4\\AVS1-5\\AVS1-6\\AVS1-7\\AVS1-8\\AVS1-9\\AVS1-10\\AVS1-11\\AVS1-12\\AVS1-13\\AVS1-14\\AVS1-15\\AVS1-16\\\\",0x0000},
	{16,   1,    "AVS2-1\\AVS2-2\\AVS2-3\\AVS2-4\\AVS2-5\\AVS2-6\\AVS2-7\\AVS2-8\\AVS2-9\\AVS2-10\\AVS2-11\\AVS2-12\\AVS2-13\\AVS2-14\\AVS2-15\\AVS2-16\\\\",0x0000}
};


#define MY_NB_RX_ISOC_STREAMS_LOW 2
static STREAM_CONFIG myDriverRxConfigLow[MY_NB_RX_ISOC_STREAMS_LOW] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"	
	{16,   1,    "AVS1-1\\AVS1-2\\AVS1-3\\AVS1-4\\AVS1-5\\AVS1-6\\AVS1-7\\AVS1-8\\AVS1-9\\AVS1-10\\AVS1-11\\AVS1-12\\AVS1-13\\AVS1-14\\AVS1-15\\AVS1-16\\\\",0x0000},
	{16,   1,    "AVS2-1\\AVS2-2\\AVS2-3\\AVS2-4\\AVS2-5\\AVS2-6\\AVS2-7\\AVS2-8\\AVS2-9\\AVS2-10\\AVS2-11\\AVS2-12\\AVS2-13\\AVS2-14\\AVS2-15\\AVS2-16\\\\",0x0000}
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
	{16,   1,    "AVS1-1\\AVS1-2\\AVS1-3\\AVS1-4\\AVS1-5\\AVS1-6\\AVS1-7\\AVS1-8\\AVS1-9\\AVS1-10\\AVS1-11\\AVS1-12\\AVS1-13\\AVS1-14\\AVS1-15\\AVS1-16\\\\",0x0000},
	{16,   1,    "AVS2-1\\AVS2-2\\AVS2-3\\AVS2-4\\AVS2-5\\AVS2-6\\AVS2-7\\AVS2-8\\AVS2-9\\AVS2-10\\AVS2-11\\AVS2-12\\AVS2-13\\AVS2-14\\AVS2-15\\AVS2-16\\\\",0x0000}
};

#define MY_NB_RX_ISOC_STREAMS_MID 2
static STREAM_CONFIG myDriverRxConfigMid[MY_NB_RX_ISOC_STREAMS_MID] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{16,   1,    "AVS1-1\\AVS1-2\\AVS1-3\\AVS1-4\\AVS1-5\\AVS1-6\\AVS1-7\\AVS1-8\\AVS1-9\\AVS1-10\\AVS1-11\\AVS1-12\\AVS1-13\\AVS1-14\\AVS1-15\\AVS1-16\\\\",0x0000},
	{16,   1,    "AVS2-1\\AVS2-2\\AVS2-3\\AVS2-4\\AVS2-5\\AVS2-6\\AVS2-7\\AVS2-8\\AVS2-9\\AVS2-10\\AVS2-11\\AVS2-12\\AVS2-13\\AVS2-14\\AVS2-15\\AVS2-16\\\\",0x0000}
};

static HRESULT routeMid (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_7,  RX_AVS1_CH0_7);		
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH8_15, RX_AVS1_CH8_15);

	dalSetRoute(eDAL_INTERFACE_1, TX_AVS2_CH0_7,  RX_AVS2_CH0_7);		
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS2_CH8_15, RX_AVS2_CH8_15);
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



//===========================================================================
// HIGH MODE CONFIGURATIONS
//===========================================================================

#define MY_NB_TX_ISOC_STREAMS_HIGH 2
static STREAM_CONFIG myDriverTxConfigHigh[MY_NB_TX_ISOC_STREAMS_HIGH] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{8,   1,    "AVS1-1\\AVS1-2\\AVS1-3\\AVS1-4\\AVS1-5\\AVS1-6\\AVS1-7\\AVS1-8\\\\",0x0000},
	{8,   1,    "AVS2-1\\AVS2-2\\AVS2-3\\AVS2-4\\AVS2-5\\AVS2-6\\AVS2-7\\AVS2-8\\\\",0x0000}
};

#define MY_NB_RX_ISOC_STREAMS_HIGH 2
static STREAM_CONFIG myDriverRxConfigHigh[MY_NB_RX_ISOC_STREAMS_HIGH] =
{
	{8,   1,    "AVS1-1\\AVS1-2\\AVS1-3\\AVS1-4\\AVS1-5\\AVS1-6\\AVS1-7\\AVS1-8\\\\",0x0000},
	{8,   1,    "AVS2-1\\AVS2-2\\AVS2-3\\AVS2-4\\AVS2-5\\AVS2-6\\AVS2-7\\AVS2-8\\\\",0x0000}
};

static HRESULT routeHigh (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_7,  RX_AVS1_CH0_7);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS2_CH0_7,  RX_AVS2_CH0_7);
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

extern MODE_CFG modeCfg7_Mini;

static HRESULT initMode(uint32 initFlags)
{
	bInitFlags = initFlags;

	targetChangeAudioPorts (APM_NORMAL);

	myModesApplyAC3Out (&modeCfg7_Mini, initFlags & 0xffff, initFlags>>16);

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

MODE_CFG modeCfg7_Mini = {
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
	"DICE Mini EVM Dual Test"
};


