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
	
	Purpose:	myMode5_Mini.c, this is the channel configuration for DICE Mini
	            in mode 5.
	            Configuration:  16x16 I8S @32k-48k
								16x16 I8S @88.2k-96k
								8x8   I4S @176.4k-192k
	
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

#define MY_INPUT_DEVICES  (DEVICE_BIT(DEV_AVS_ADO1) | DEVICE_BIT(DEV_AVS_ADO2) |\
	                       DEVICE_BIT(DEV_MIXER_RX) |\
	                       DEVICE_BIT(DEV_INS_RX0))

#define MY_OUTPUT_DEVICES (DEVICE_BIT(DEV_AVS_ADI1) | DEVICE_BIT(DEV_AVS_ADI2) |\
	                       DEVICE_BIT(DEV_MIXER_TX0) | DEVICE_BIT(DEV_MIXER_TX1) |\
	                       DEVICE_BIT(DEV_INS_RX0))



//===========================================================================
// LOW MODE CONFIGURATIONS
//===========================================================================

#define MY_NB_TX_ISOC_STREAMS_LOW 1
static STREAM_CONFIG myDriverTxConfigLow[MY_NB_TX_ISOC_STREAMS_LOW] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{16,   1,    "I8S1\\I8S2\\I8S3\\I8S4\\I8S5\\I8S6\\I8S7\\I8S8\\I8S9\\I8S10\\I8S11\\I8S12\\I8S13\\I8S14\\I8S15\\I8S16\\\\",0x0000}
};


#define MY_NB_RX_ISOC_STREAMS_LOW 1
static STREAM_CONFIG myDriverRxConfigLow[MY_NB_RX_ISOC_STREAMS_LOW] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"	
	{16,   1,    "I8S1\\I8S2\\I8S3\\I8S4\\I8S5\\I8S6\\I8S7\\I8S8\\I8S9\\I8S10\\I8S11\\I8S12\\I8S13\\I8S14\\I8S15\\I8S16\\\\",0x0000}
};

static HRESULT routeLow (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_INS0_CH0_7,  RX_AVS1_CH0_7);
	dalSetRoute(eDAL_INTERFACE_1, TX_INS0_CH8_15, RX_AVS1_CH8_15);
	
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_7, RX_INS0_CH0_7);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH8_15, RX_INS0_CH8_15);
	
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




//===========================================================================
// MID MODE CONFIGURATIONS
//===========================================================================

#define MY_NB_TX_ISOC_STREAMS_MID 1
static STREAM_CONFIG myDriverTxConfigMid[MY_NB_TX_ISOC_STREAMS_MID] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"	
	{16,   1,    "I8S1\\I8S2\\I8S3\\I8S4\\I8S5\\I8S6\\I8S7\\I8S8\\I8S9\\I8S10\\I8S11\\I8S12\\I8S13\\I8S14\\I8S15\\I8S16\\\\",0x0000}
};

#define MY_NB_RX_ISOC_STREAMS_MID 1
static STREAM_CONFIG myDriverRxConfigMid[MY_NB_RX_ISOC_STREAMS_MID] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{16,   1,    "I8S1\\I8S2\\I8S3\\I8S4\\I8S5\\I8S6\\I8S7\\I8S8\\I8S9\\I8S10\\I8S11\\I8S12\\I8S13\\I8S14\\I8S15\\I8S16\\\\",0x0000}
};

static HRESULT routeMid (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_INS0_CH0_7,  RX_AVS1_CH0_7);
	dalSetRoute(eDAL_INTERFACE_1, TX_INS0_CH8_15, RX_AVS1_CH8_15);
	
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_7, RX_INS0_CH0_7);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH8_15, RX_INS0_CH8_15);
	return NO_ERROR;
}

static STREAM_CFG myDriverConfigureMid =
{
	myDriverTxConfigMid,
	MY_NB_TX_ISOC_STREAMS_MID,
	DICE_TX_INS_ROUTE_LATENCY_MID,

	myDriverRxConfigMid,
	MY_NB_RX_ISOC_STREAMS_MID,
	DICE_RX_INS_ROUTE_LATENCY_MID,
};



//===========================================================================
// HIGH MODE CONFIGURATIONS
//===========================================================================

#define MY_NB_TX_ISOC_STREAMS_HIGH 1
static STREAM_CONFIG myDriverTxConfigHigh[MY_NB_TX_ISOC_STREAMS_HIGH] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{8,   1,    "I8S1\\I8S2\\I8S3\\I8S4\\I8S5\\I8S6\\I8S7\\I8S8\\\\",0x0000}
};

#define MY_NB_RX_ISOC_STREAMS_HIGH 1
static STREAM_CONFIG myDriverRxConfigHigh[MY_NB_RX_ISOC_STREAMS_HIGH] =
{
	{8,   1,    "I8S1\\I8S2\\I8S3\\I8S4\\I8S5\\I8S6\\I8S7\\I8S8\\\\",0x0000}
};

static HRESULT routeHigh (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_INS0_CH0_7,  RX_AVS1_CH0_7);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0_7, RX_INS0_CH0_7);
	return NO_ERROR;
}

static STREAM_CFG myDriverConfigureHigh =
{
	myDriverTxConfigHigh,
	MY_NB_TX_ISOC_STREAMS_HIGH,
	DICE_TX_INS_ROUTE_LATENCY_HIGH,

	myDriverRxConfigHigh,
	MY_NB_RX_ISOC_STREAMS_HIGH,
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
	case eDAL_RATE_MODE_MID: return routeMid();
	case eDAL_RATE_MODE_HIGH: return routeHigh();
	} 
	return NO_ERROR;
}


static void setInsPorts (uint8 mode)
{
	//Set the clock ports
	if (mode==INS_MODE_I8S)
	{
		//I8S on both ports, MCLK=512xBase rate, BCLK inverted = 256FS (drive on neg, sample on pos)
		insSetClockPort       (INS_ID0, INS_MCK_512BR, INS_BCK_256FS, INS_FSYN_LEN_1BIT, 0, 1, 0, true);	
		insSetClockPort       (INS_ID1, INS_MCK_512BR, INS_BCK_256FS, INS_FSYN_LEN_1BIT, 0, 1, 0, true);
	}
	else
	{
		//I4S on both ports, MCLK=512xBase rate, BCLK inverted = 128FS (drive on neg, sample on pos)
		insSetClockPort       (INS_ID0, INS_MCK_512BR, INS_BCK_128FS, INS_FSYN_LEN_1BIT, 0, 1, 0, true);	
		insSetClockPort       (INS_ID1, INS_MCK_512BR, INS_BCK_128FS, INS_FSYN_LEN_1BIT, 0, 1, 0, true);
	}
	insRxSetDataFormat    (INS_ID0, INS_LN0, mode, 1, INS_SHFL_24MSB_FRST_LJST, true);
	insRxSetDataFormat    (INS_ID0, INS_LN1, mode, 1, INS_SHFL_24MSB_FRST_LJST, true);
	insRxSetDataFormat    (INS_ID0, INS_LN2, INS_MODE_I2S, 1, INS_SHFL_24MSB_FRST_LJST, false);
	insRxSetDataFormat    (INS_ID0, INS_LN3, INS_MODE_I2S, 1, INS_SHFL_24MSB_FRST_LJST, false);

	insTxSetDataFormat    (INS_ID0, INS_LN0, mode, 1, INS_SHFL_24MSB_FRST_LJST, true);
	insTxSetDataFormat    (INS_ID0, INS_LN1, mode, 1, INS_SHFL_24MSB_FRST_LJST, true);
	insTxSetDataFormat    (INS_ID0, INS_LN2, INS_MODE_I2S, 1, INS_SHFL_24MSB_FRST_LJST, false);
	insTxSetDataFormat    (INS_ID0, INS_LN3, INS_MODE_I2S, 1, INS_SHFL_24MSB_FRST_LJST, false);
}


static HRESULT initMode(uint32 initFlags)
{
	bInitFlags = initFlags;

	//this mode uses all ports as InS
	targetChangeAudioPorts (APM_ALL_INS);
	setInsPorts(INS_MODE_I8S);
	
	return NO_ERROR;
}

static HRESULT msgFunc (MYMODE_MSG msg, uint32 data0, uint32 data1)
{
	//this mode must change to I4S at High sample rates.
	switch (msg)
	{
		case MM_LOST_LOCK:break;				
		case MM_GOT_LOCK:
			switch (data0)
			{
				default:
				case eDAL_RATE_MODE_LOW: //32k-48k
				case eDAL_RATE_MODE_MID: //88.2k-96k				
					setInsPorts(INS_MODE_I8S);
					break;
				case eDAL_RATE_MODE_HIGH: //176.4k-192k
					setInsPorts(INS_MODE_I4S);
					break;
			}
			break;					
	}
	return NO_ERROR;
}


MODE_CFG modeCfg5_Mini = {
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
	"DICE Mini EVM ALL I8S"
};


