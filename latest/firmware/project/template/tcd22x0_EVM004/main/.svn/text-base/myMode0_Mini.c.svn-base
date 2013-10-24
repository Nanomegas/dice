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
	
	Purpose:	myMode0_Mini.c, this is the channel configuration for DICE Mini
	            in mode 0.
	            Configuration:  8x8   8xI2S
	
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


// We need to define how we present ourselves to the PC and Mac driver. We will
// need to specify our Isco channel layout, names for ASIO and CoreAudio etc.
// Starting and stopping the 1394 Isoc streams will be handled by the DiceDriver
// module.
//
// The settings below are shared for all configurations within this mode.
#define MY_DEVICE_SUPPORTED_CLK_CAPS (	DD_CP_RATE_32   | DD_CP_RATE_44_1    |\
										DD_CP_RATE_48   | DD_CP_RATE_88_2    |\
										DD_CP_RATE_96   | DD_CP_RATE_176_4   |\
										DD_CP_RATE_192  |\
										DD_CP_SOURCE_INT      )



#define MY_DEVICE_SUPPORTED_CLK_SRC_NAMES "AES12\\AES34\\SPDIF\\AES78\\AES_ANY\\ADAT\\ADAT\\Word Clock\\Unused\\Unused\\Unused\\Unused\\Internal\\\\"

#define MY_INPUT_DEVICES  (DEVICE_BIT(DEV_INS_RX0) | DEVICE_BIT(DEV_AVS_ADO1))

#define MY_OUTPUT_DEVICES (DEVICE_BIT(DEV_INS_TX0) | DEVICE_BIT(DEV_AVS_ADI1))



//===========================================================================
// LOW MODE CONFIGURATIONS
//===========================================================================

#define MY_NB_TX_ISOC_STREAMS_LOW 1
static STREAM_CONFIG myDriverTxConfigLow[MY_NB_TX_ISOC_STREAMS_LOW] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{8,   1,    "Ch1\\Ch2\\Ch3\\Ch4\\Ch5\\Ch6\\Ch7\\Ch8\\\\",0x0000}
};


#define MY_NB_RX_ISOC_STREAMS_LOW 1
static STREAM_CONFIG myDriverRxConfigLow[MY_NB_RX_ISOC_STREAMS_LOW] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{8,   1,    "Ch1\\Ch2\\Ch3\\Ch4\\Ch5\\Ch6\\Ch7\\Ch8\\\\",0x0000}
};

static HRESULT routeLow (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_INS0_CH0_7,  RX_AVS1_CH0_7);	
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_7 ,  RX_INS0_CH0_7);
	return NO_ERROR;
}


static STREAM_CFG myDriverConfigureLow =
{
	myDriverTxConfigLow,
	MY_NB_TX_ISOC_STREAMS_LOW,
	DICE_TX_INS_ROUTE_LATENCY_LOW,

	myDriverRxConfigLow,
	MY_NB_RX_ISOC_STREAMS_LOW,
	DICE_RX_INS_ROUTE_LATENCY_LOW,
};


static STREAM_CFG myDriverConfigureMid =
{
	myDriverTxConfigLow,
	MY_NB_TX_ISOC_STREAMS_LOW,
	DICE_TX_INS_ROUTE_LATENCY_MID,

	myDriverRxConfigLow,
	MY_NB_RX_ISOC_STREAMS_LOW,
	DICE_RX_INS_ROUTE_LATENCY_MID,
};


static STREAM_CFG myDriverConfigureHigh =
{
	myDriverTxConfigLow,
	MY_NB_TX_ISOC_STREAMS_LOW,
	DICE_TX_INS_ROUTE_LATENCY_HIGH,

	myDriverRxConfigLow,
	MY_NB_RX_ISOC_STREAMS_LOW,
	DICE_RX_INS_ROUTE_LATENCY_HIGH,
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
	case eDAL_RATE_MODE_HIGH: return routeLow();
	} 
	return NO_ERROR;
}


static HRESULT initMode(uint32 initFlags)
{
	bInitFlags = initFlags;

	return NO_ERROR;
}

static HRESULT msgFunc (MYMODE_MSG msg, uint32 data0, uint32 data1)
{
	return 0;;
}


MODE_CFG modeCfg0_Mini = {
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
	"DICE EVM004_FW843 Default"
};


