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
	
	Purpose:	myMode0_JR.c, this is the channel configuration for DICE JR
	            in mode 0.
	            Configuration:  28x28 AES, Analog and ADAT @32k-48k
								20x20 AES, Analog and ADAT @88.2k-96k
								16x16 AES, Analog and ADAT @176.4k-192k
	
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
										DD_CP_RATE_192  | DD_CP_SOURCE_AES1    |\
										DD_CP_SOURCE_AES2 | DD_CP_SOURCE_AES3	   |\
										DD_CP_SOURCE_AES4 | DD_CP_SOURCE_AES_ANY |\
										DD_CP_SOURCE_ADAT | DD_CP_SOURCE_ADATAUX  |\
										DD_CP_SOURCE_WC   | DD_CP_SOURCE_ARX1    |\
										DD_CP_SOURCE_ARX2 | DD_CP_SOURCE_INT       )

#define MY_DEVICE_SUPPORTED_CLK_SRC_NAMES "AES34\\AES56\\AES78\\AES12/SPDIF\\AES_ANY\\ADAT\\ADAT_AUX\\Word Clock\\Unused\\Unused\\Unused\\Unused\\Internal\\\\"

#define MY_INPUT_DEVICES  (DEVICE_BIT(DEV_AES_RX) | DEVICE_BIT(DEV_ADAT_RX) | \
	                       DEVICE_BIT(DEV_AVS_ADO1) | DEVICE_BIT(DEV_AVS_ADO2) | \
	                       DEVICE_BIT(DEV_MIXER_RX) | DEVICE_BIT(DEV_APB_RX) | \
	                       DEVICE_BIT(DEV_INS_RX0))

#define MY_OUTPUT_DEVICES (DEVICE_BIT(DEV_AES_TX) | DEVICE_BIT(DEV_ADAT_TX) | \
	                       DEVICE_BIT(DEV_AVS_ADI1) | DEVICE_BIT(DEV_AVS_ADI2) | \
	                       DEVICE_BIT(DEV_MIXER_TX0) | DEVICE_BIT(DEV_MIXER_TX1) | DEVICE_BIT(DEV_INS_TX0))



//===========================================================================
// LOW MODE CONFIGURATIONS
//===========================================================================

#define MY_NB_TX_ISOC_STREAMS_LOW 2
static STREAM_CONFIG myDriverTxConfigLow[MY_NB_TX_ISOC_STREAMS_LOW] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{12,   1,    "AES1/SPDIF L\\AES2/SPDIF R\\AES3\\AES4\\AES5\\AES6\\AES7\\AES8\\ANA1 L\\ANA1 R\\ANA2 L\\ANA2 R\\\\",0x0000},
	{16,   0,    "ADAT1\\ADAT2\\ADAT3\\ADAT4\\ADAT5\\ADAT6\\ADAT7\\ADAT8\\ADAT9\\ADAT10\\ADAT11\\ADAT12\\ADAT13\\ADAT14\\ADAT15\\ADAT16\\\\",0x0000}	
};


#define MY_NB_RX_ISOC_STREAMS_LOW 2
static STREAM_CONFIG myDriverRxConfigLow[MY_NB_RX_ISOC_STREAMS_LOW] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"	
	{12,   1,    "AES1/SPDIF L\\AES2/SPDIF R\\AES3\\AES4\\AES5\\AES6\\AES7\\AES8\\ANA1 L\\ANA1 R\\ANA2 L\\ANA2 R\\\\",0x0003},
	{16,   0,    "ADAT1\\ADAT2\\ADAT3\\ADAT4\\ADAT5\\ADAT6\\ADAT7\\ADAT8\\ADAT9\\ADAT10\\ADAT11\\ADAT12\\ADAT13\\ADAT14\\ADAT15\\ADAT16\\\\",0x0000}	
};

static HRESULT routeLow (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_AES3_CHL,  RX_AVS1_CH0);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES3_CHR,  RX_AVS1_CH1);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES0_CHL,  RX_AVS1_CH2);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES0_CHR,  RX_AVS1_CH3);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES1_CHL,  RX_AVS1_CH4);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES1_CHR,  RX_AVS1_CH5);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES2_CHL,  RX_AVS1_CH6);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES2_CHR,  RX_AVS1_CH7);
	dalSetRoute(eDAL_INTERFACE_1, TX_INS0_CH0_3,  RX_AVS1_CH8_11);		
	dalSetRoute(eDAL_INTERFACE_1, TX_ADAT_CH0_7, RX_AVS2_CH0_7);
	dalSetRoute(eDAL_INTERFACE_1, TX_ADAT_CH8_15, RX_AVS2_CH8_15);
	
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0, RX_AES3_CHL);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH1, RX_AES3_CHR);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH2, RX_AES0_CHL);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH3, RX_AES0_CHR);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH4, RX_AES1_CHL);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH5, RX_AES1_CHR);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH6, RX_AES2_CHL);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH7, RX_AES2_CHR);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH8_11, RX_INS0_CH0_3);		
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS2_CH0_7, RX_ADAT_CH0_7);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS2_CH8_15, RX_ADAT_CH8_15);
	return NO_ERROR;
}


static STREAM_CFG myDriverConfigureLow =
{
	myDriverTxConfigLow,
	MY_NB_TX_ISOC_STREAMS_LOW,
	AKM4620_AD_LATENCY_LOW + DICE_TX_INS_ROUTE_LATENCY_LOW,

	myDriverRxConfigLow,
	MY_NB_RX_ISOC_STREAMS_LOW,
	DICE_RX_INS_ROUTE_LATENCY_LOW + AKM4620_DA_LATENCY_LOW,
};






//===========================================================================
// MID MODE CONFIGURATIONS
//===========================================================================

#define MY_NB_TX_ISOC_STREAMS_MID 2
static STREAM_CONFIG myDriverTxConfigMid[MY_NB_TX_ISOC_STREAMS_MID] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"	
	{12,   1,    "AES1/SPDIF L\\AES2/SPDIF R\\AES3\\AES4\\AES5\\AES6\\AES7\\AES8\\ANA1 L\\ANA1 R\\ANA2 L\\ANA2 R\\\\",0x0000},
	{8,    0,    "ADAT1\\ADAT2\\ADAT3\\ADAT4\\ADAT5\\ADAT6\\ADAT7\\ADAT8\\\\",0x0000}	
};

#define MY_NB_RX_ISOC_STREAMS_MID 2
static STREAM_CONFIG myDriverRxConfigMid[MY_NB_RX_ISOC_STREAMS_MID] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{12,   1,    "AES1/SPDIF L\\AES2/SPDIF R\\AES3\\AES4\\AES5\\AES6\\AES7\\AES8\\ANA1 L\\ANA1 R\\ANA2 L\\ANA2 R\\\\",0x0003},
	{8,    0,    "ADAT1\\ADAT2\\ADAT3\\ADAT4\\ADAT5\\ADAT6\\ADAT7\\ADAT8\\\\",0x0000}	
};

static HRESULT routeMid (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_AES3_CHL,  RX_AVS1_CH0);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES3_CHR,  RX_AVS1_CH1);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES0_CHL,  RX_AVS1_CH2);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES0_CHR,  RX_AVS1_CH3);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES1_CHL,  RX_AVS1_CH4);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES1_CHR,  RX_AVS1_CH5);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES2_CHL,  RX_AVS1_CH6);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES2_CHR,  RX_AVS1_CH7);
	dalSetRoute(eDAL_INTERFACE_1, TX_INS0_CH0_3,RX_AVS1_CH8_11);		
	dalSetRoute(eDAL_INTERFACE_1, TX_ADAT_CH0_7,RX_AVS2_CH0_7);
	
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0, RX_AES3_CHL);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH1, RX_AES3_CHR);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH2, RX_AES0_CHL);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH3, RX_AES0_CHR);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH4, RX_AES1_CHL);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH5, RX_AES1_CHR);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH6, RX_AES2_CHL);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH7, RX_AES2_CHR);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH8_11, RX_INS0_CH0_3);		
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS2_CH0_7, RX_ADAT_CH0_7);
	return NO_ERROR;
}

static STREAM_CFG myDriverConfigureMid =
{
	myDriverTxConfigMid,
	MY_NB_TX_ISOC_STREAMS_MID,
	AKM4620_AD_LATENCY_MID + DICE_TX_INS_ROUTE_LATENCY_MID,

	myDriverRxConfigMid,
	MY_NB_RX_ISOC_STREAMS_MID,
	DICE_RX_INS_ROUTE_LATENCY_MID + AKM4620_DA_LATENCY_MID,
};



//===========================================================================
// HIGH MODE CONFIGURATIONS
//===========================================================================

#define MY_NB_TX_ISOC_STREAMS_HIGH 2
static STREAM_CONFIG myDriverTxConfigHigh[MY_NB_TX_ISOC_STREAMS_HIGH] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{8,   1,    "AES1/SPDIF L\\AES2/SPDIF R\\AES3\\AES4\\AES5\\AES6\\AES7\\AES8\\\\",0x0000},
	{8,   0,    "ANA1 L\\ANA1 R\\ANA2 L\\ANA2 R\\ADAT1\\ADAT2\\ADAT3\\ADAT4\\\\",0x0000}	
};

#define MY_NB_RX_ISOC_STREAMS_HIGH 2
static STREAM_CONFIG myDriverRxConfigHigh[MY_NB_RX_ISOC_STREAMS_HIGH] =
{
	{8,   1,    "AES1/SPDIF L\\AES2/SPDIF R\\AES3\\AES4\\AES5\\AES6\\AES7\\AES8\\\\",0x0003},
	{8,   0,    "ANA1 L\\ANA1 R\\ANA2 L\\ANA2 R\\ADAT1\\ADAT2\\ADAT3\\ADAT4\\\\",0x0000}	
};

static HRESULT routeHigh (void)
{
	dalSetRoute(eDAL_INTERFACE_1, TX_AES3_CHL,  RX_AVS1_CH0);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES3_CHR,  RX_AVS1_CH1);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES0_CHL,  RX_AVS1_CH2);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES0_CHR,  RX_AVS1_CH3);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES1_CHL,  RX_AVS1_CH4);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES1_CHR,  RX_AVS1_CH5);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES2_CHL,  RX_AVS1_CH6);
	dalSetRoute(eDAL_INTERFACE_1, TX_AES2_CHR,  RX_AVS1_CH7);

	dalSetRoute(eDAL_INTERFACE_1, TX_INS0_CH0_3, RX_AVS2_CH0_3);		
	dalSetRoute(eDAL_INTERFACE_1, TX_ADAT_CH0_3, RX_AVS2_CH4_7);
	

	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH0, RX_AES3_CHL);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH1, RX_AES3_CHR);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH2, RX_AES0_CHL);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH3, RX_AES0_CHR);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH4, RX_AES1_CHL);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH5, RX_AES1_CHR);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH6, RX_AES2_CHL);
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS1_CH7, RX_AES2_CHR);

	dalSetRoute(eDAL_INTERFACE_1, TX_AVS2_CH0_3, RX_INS0_CH0_3);		
	dalSetRoute(eDAL_INTERFACE_1, TX_AVS2_CH4_7, RX_ADAT_CH0_3);
	return NO_ERROR;
}

static STREAM_CFG myDriverConfigureHigh =
{
	myDriverTxConfigHigh,
	MY_NB_TX_ISOC_STREAMS_HIGH,
	AKM4620_AD_LATENCY_HIGH + DICE_TX_INS_ROUTE_LATENCY_HIGH,

	myDriverRxConfigHigh,
	MY_NB_RX_ISOC_STREAMS_HIGH,
	DICE_RX_INS_ROUTE_LATENCY_HIGH + AKM4620_DA_LATENCY_HIGH,
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


static HRESULT initMode(uint32 initFlags)
{
	bInitFlags = initFlags;

	//this mode uses Optical as ADAT
	targetChangeAudioPorts (APM_NORMAL);

	//this mode uses the AKM4620
	akm4620_InitI2S ();
	return NO_ERROR;
}

static HRESULT msgFunc (MYMODE_MSG msg, uint32 data0, uint32 data1)
{
	//this mode uses the AKM4620
	return akm4620_msgFunc (msg, data0, data1);
}

MODE_CFG modeCfg0_JR = {
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
	"DICE JR EVM 28/28"
};


