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
	
	Purpose:	This file contains the factory default configuration for
	            streams, routing and mixing for a typical DICE JR based
	            product. (EVM002)
	
	Revisions:
		created 03/03/2007 ml
		

****************************************************************************/

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "eapCfg.h"




//===========================================================================
// LOW MODE CONFIGURATIONS
//===========================================================================

// Configuration of 'low' mode

#define NB_TX_ISOC_STREAMS_LOW 2
static const STREAM_CONFIG driverTxConfigLow[NB_TX_ISOC_STREAMS_LOW] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{6,   1,    "Mic 1\\Mic 2\\Mic 3\\Mic 4\\SPDIF L\\SPDIF R\\\\",0x0000},
	{16,   0,    "ADAT 1\\ADAT 2\\ADAT 3\\ADAT 4\\ADAT 5\\ADAT 6\\ADAT 7\\ADAT 8\\ADAT 9\\ADAT 10\\ADAT 11\\ADAT 12\\ADAT 13\\ADAT 14\\ADAT 15\\ADAT 16\\\\",0x0000}
};


#define NB_RX_ISOC_STREAMS_LOW 2
static const STREAM_CONFIG driverRxConfigLow[NB_RX_ISOC_STREAMS_LOW] =
{
	{6,   1,    "Line 1\\Line 2\\Line 3\\Line 4\\SPDIF L\\SPDIF R\\\\",0x0000},
	{16,   0,    "ADAT 1\\ADAT 2\\ADAT 3\\ADAT 4\\ADAT 5\\ADAT 6\\ADAT 7\\ADAT 8\\ADAT 9\\ADAT 10\\ADAT 11\\ADAT 12\\ADAT 13\\ADAT 14\\ADAT 15\\ADAT 16\\\\",0x0000}
};

#define ROUTE(srcDev,srcCh,dstDev,dstCh) (((srcDev)<<12) + ((srcCh)<<8) + ((dstDev)<<4) + (dstCh))

#define NB_LOW_ROUTES 44

static const uint16 lowRouting[NB_LOW_ROUTES] = 
{
	//All the routes to AVS transmitters
	ROUTE(DEV_INS_RX0,0,DEV_AVS_ADI1,0),
	ROUTE(DEV_INS_RX0,1,DEV_AVS_ADI1,1),
	ROUTE(DEV_INS_RX0,2,DEV_AVS_ADI1,2),
	ROUTE(DEV_INS_RX0,3,DEV_AVS_ADI1,3),
	ROUTE(DEV_AES_RX,6,DEV_AVS_ADI1,4),
	ROUTE(DEV_AES_RX,7,DEV_AVS_ADI1,5),
	
	ROUTE(DEV_ADAT_RX,0,DEV_AVS_ADI2,0),
	ROUTE(DEV_ADAT_RX,1,DEV_AVS_ADI2,1),
	ROUTE(DEV_ADAT_RX,2,DEV_AVS_ADI2,2),
	ROUTE(DEV_ADAT_RX,3,DEV_AVS_ADI2,3),
	ROUTE(DEV_ADAT_RX,4,DEV_AVS_ADI2,4),
	ROUTE(DEV_ADAT_RX,5,DEV_AVS_ADI2,5),
	ROUTE(DEV_ADAT_RX,6,DEV_AVS_ADI2,6),
	ROUTE(DEV_ADAT_RX,7,DEV_AVS_ADI2,7),
	ROUTE(DEV_ADAT_RX,8,DEV_AVS_ADI2,8),
	ROUTE(DEV_ADAT_RX,9,DEV_AVS_ADI2,9),
	ROUTE(DEV_ADAT_RX,10,DEV_AVS_ADI2,10),
	ROUTE(DEV_ADAT_RX,11,DEV_AVS_ADI2,11),
	ROUTE(DEV_ADAT_RX,12,DEV_AVS_ADI2,12),
	ROUTE(DEV_ADAT_RX,13,DEV_AVS_ADI2,13),
	ROUTE(DEV_ADAT_RX,14,DEV_AVS_ADI2,14),
	ROUTE(DEV_ADAT_RX,15,DEV_AVS_ADI2,15),
	
	
	//All the routes from AVS transmitters
	ROUTE(DEV_AVS_ADO1,0,DEV_INS_TX0,0),
	ROUTE(DEV_AVS_ADO1,1,DEV_INS_TX0,1),
	ROUTE(DEV_AVS_ADO1,2,DEV_INS_TX0,2),
	ROUTE(DEV_AVS_ADO1,3,DEV_INS_TX0,3),
	ROUTE(DEV_AVS_ADO1,4,DEV_AES_TX,6),
	ROUTE(DEV_AVS_ADO1,5,DEV_AES_TX,7),
	
	ROUTE(DEV_AVS_ADO2,0,DEV_ADAT_TX,0),
	ROUTE(DEV_AVS_ADO2,1,DEV_ADAT_TX,1),
	ROUTE(DEV_AVS_ADO2,2,DEV_ADAT_TX,2),
	ROUTE(DEV_AVS_ADO2,3,DEV_ADAT_TX,3),
	ROUTE(DEV_AVS_ADO2,4,DEV_ADAT_TX,4),
	ROUTE(DEV_AVS_ADO2,5,DEV_ADAT_TX,5),
	ROUTE(DEV_AVS_ADO2,6,DEV_ADAT_TX,6),
	ROUTE(DEV_AVS_ADO2,7,DEV_ADAT_TX,7),
	ROUTE(DEV_AVS_ADO2,8,DEV_ADAT_TX,8),
	ROUTE(DEV_AVS_ADO2,9,DEV_ADAT_TX,9),
	ROUTE(DEV_AVS_ADO2,10,DEV_ADAT_TX,10),
	ROUTE(DEV_AVS_ADO2,11,DEV_ADAT_TX,11),
	ROUTE(DEV_AVS_ADO2,12,DEV_ADAT_TX,12),
	ROUTE(DEV_AVS_ADO2,13,DEV_ADAT_TX,13),
	ROUTE(DEV_AVS_ADO2,14,DEV_ADAT_TX,14),
	ROUTE(DEV_AVS_ADO2,15,DEV_ADAT_TX,15)

};





//===========================================================================
// MID MODE CONFIGURATIONS
//===========================================================================

#define NB_TX_ISOC_STREAMS_MID 2
static const STREAM_CONFIG driverTxConfigMid[NB_TX_ISOC_STREAMS_MID] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"	
	{6,   1,    "Mic 1\\Mic 2\\Mic 3\\Mic 4\\SPDIF L\\SPDIF R\\\\",0x0000},
	{8,    0,    "ADAT 1\\ADAT 2\\ADAT 3\\ADAT 4\\ADAT 5\\ADAT 6\\ADAT 7\\ADAT 8\\\\",0x0000}
};

#define NB_RX_ISOC_STREAMS_MID 2
static const STREAM_CONFIG driverRxConfigMid[NB_RX_ISOC_STREAMS_MID] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{6,   1,    "Line 1\\Line 2\\Line 3\\Line 4\\SPDIF L\\SPDIF R\\\\",0x0000},
	{8,    0,    "ADAT 1\\ADAT 2\\ADAT 3\\ADAT 4\\ADAT 5\\ADAT 6\\ADAT 7\\ADAT 8\\\\",0x0000}
};


#define NB_MID_ROUTES 28

static const uint16 midRouting[NB_MID_ROUTES] = 
{
	//All the routes to AVS transmitters
	ROUTE(DEV_INS_RX0,0,DEV_AVS_ADI1,0),
	ROUTE(DEV_INS_RX0,1,DEV_AVS_ADI1,1),
	ROUTE(DEV_INS_RX0,2,DEV_AVS_ADI1,2),
	ROUTE(DEV_INS_RX0,3,DEV_AVS_ADI1,3),
	ROUTE(DEV_AES_RX,6,DEV_AVS_ADI1,4),
	ROUTE(DEV_AES_RX,7,DEV_AVS_ADI1,5),
	
	ROUTE(DEV_ADAT_RX,0,DEV_AVS_ADI2,0),
	ROUTE(DEV_ADAT_RX,1,DEV_AVS_ADI2,1),
	ROUTE(DEV_ADAT_RX,2,DEV_AVS_ADI2,2),
	ROUTE(DEV_ADAT_RX,3,DEV_AVS_ADI2,3),
	ROUTE(DEV_ADAT_RX,4,DEV_AVS_ADI2,4),
	ROUTE(DEV_ADAT_RX,5,DEV_AVS_ADI2,5),
	ROUTE(DEV_ADAT_RX,6,DEV_AVS_ADI2,6),
	ROUTE(DEV_ADAT_RX,7,DEV_AVS_ADI2,7),
	
	
	//All the routes from AVS transmitters
	ROUTE(DEV_AVS_ADO1,0,DEV_INS_TX0,0),
	ROUTE(DEV_AVS_ADO1,1,DEV_INS_TX0,1),
	ROUTE(DEV_AVS_ADO1,2,DEV_INS_TX0,2),
	ROUTE(DEV_AVS_ADO1,3,DEV_INS_TX0,3),
	ROUTE(DEV_AVS_ADO1,4,DEV_AES_TX,6),
	ROUTE(DEV_AVS_ADO1,5,DEV_AES_TX,7),
	
	ROUTE(DEV_AVS_ADO2,0,DEV_ADAT_TX,0),
	ROUTE(DEV_AVS_ADO2,1,DEV_ADAT_TX,1),
	ROUTE(DEV_AVS_ADO2,2,DEV_ADAT_TX,2),
	ROUTE(DEV_AVS_ADO2,3,DEV_ADAT_TX,3),
	ROUTE(DEV_AVS_ADO2,4,DEV_ADAT_TX,4),
	ROUTE(DEV_AVS_ADO2,5,DEV_ADAT_TX,5),
	ROUTE(DEV_AVS_ADO2,6,DEV_ADAT_TX,6),
	ROUTE(DEV_AVS_ADO2,7,DEV_ADAT_TX,7),
};


//===========================================================================
// HIGH MODE CONFIGURATIONS, Not on this device
//===========================================================================

#define NB_TX_ISOC_STREAMS_HIGH 2
static const STREAM_CONFIG driverTxConfigHigh[NB_TX_ISOC_STREAMS_HIGH] =
{
//	Audio  Midi  Names as they appear on computer sepperated by '\' and terminated by "\\"
	{6,   1,    "Mic 1\\Mic 2\\Mic 3\\Mic 4\\SPDIF L\\SPDIF R\\\\",0x0000},
	{4,    0,    "ADAT 1\\ADAT 2\\ADAT 3\\ADAT 4\\\\",0x0000}
};

#define NB_RX_ISOC_STREAMS_HIGH 2
static const STREAM_CONFIG driverRxConfigHigh[NB_RX_ISOC_STREAMS_HIGH] =
{
	{6,   1,    "Mic 1\\Mic 2\\Mic 3\\Mic 4\\SPDIF L\\SPDIF R\\\\",0x0000},
	{4,    0,    "ADAT 1\\ADAT 2\\ADAT 3\\ADAT 4\\\\",0x0000}
};

#define NB_HIGH_ROUTES 20

static const uint16 highRouting[NB_HIGH_ROUTES] = 
{
	//All the routes to AVS transmitters
	ROUTE(DEV_INS_RX0,0,DEV_AVS_ADI1,0),
	ROUTE(DEV_INS_RX0,1,DEV_AVS_ADI1,1),
	ROUTE(DEV_INS_RX0,2,DEV_AVS_ADI1,2),
	ROUTE(DEV_INS_RX0,3,DEV_AVS_ADI1,3),
	ROUTE(DEV_AES_RX,6,DEV_AVS_ADI2,4),
	ROUTE(DEV_AES_RX,7,DEV_AVS_ADI2,5),	
	
	ROUTE(DEV_ADAT_RX,0,DEV_AVS_ADI2,0),
	ROUTE(DEV_ADAT_RX,1,DEV_AVS_ADI2,1),
	ROUTE(DEV_ADAT_RX,2,DEV_AVS_ADI2,2),
	ROUTE(DEV_ADAT_RX,3,DEV_AVS_ADI2,3),
		
	//All the routes from AVS transmitters
	ROUTE(DEV_AVS_ADO1,0,DEV_INS_TX0,0),
	ROUTE(DEV_AVS_ADO1,1,DEV_INS_TX0,1),
	ROUTE(DEV_AVS_ADO1,2,DEV_INS_TX0,2),
	ROUTE(DEV_AVS_ADO1,3,DEV_INS_TX0,3),	
	ROUTE(DEV_AVS_ADO2,4,DEV_AES_TX,6),
	ROUTE(DEV_AVS_ADO2,5,DEV_AES_TX,7),	
	
	ROUTE(DEV_AVS_ADO2,0,DEV_ADAT_TX,0),
	ROUTE(DEV_AVS_ADO2,1,DEV_ADAT_TX,1),
	ROUTE(DEV_AVS_ADO2,2,DEV_ADAT_TX,2),
	ROUTE(DEV_AVS_ADO2,3,DEV_ADAT_TX,3),
};

//===========================================================================
// THE COMPLETE INITIALIZATION STRUCTURE
//===========================================================================

APP_SPACE appSpace;

const EAP_INIT_DESC eapInitDesc =
{
	{	
		NB_TX_ISOC_STREAMS_LOW, driverTxConfigLow,
		NB_RX_ISOC_STREAMS_LOW, driverRxConfigLow,
		NB_LOW_ROUTES, lowRouting
	},
	{
		NB_TX_ISOC_STREAMS_MID, driverTxConfigMid,
		NB_RX_ISOC_STREAMS_MID, driverRxConfigMid,
		NB_MID_ROUTES, midRouting
	},
	{
		NB_TX_ISOC_STREAMS_HIGH, driverTxConfigHigh,
		NB_RX_ISOC_STREAMS_HIGH, driverRxConfigHigh,
		NB_HIGH_ROUTES, highRouting
	},
	{ //stand alone configuration
		DD_CLK_SRC_INTERNAL,
		0, //default not high mode for AES
		0, //ADAT no SMUX per default
		0, //Normal WC
		eDAL_NOMINAL_RATE_44_1,
		{}
	},
	0, //no default mixer coefficients provided
	0,
	EAP_CAP_ROUTER_EXPOSED | EAP_CAP_ROUTER_FLS,
	EAP_CAP_MIXER_EXPOSED | EAP_CAP_MIXER_FLS,
	EAP_CAP_GEN_STRCFG_EN | EAP_CAP_GEN_FLASH_EN | EAP_CAP_GEN_AUTO_STORE_EN | EAP_CAP_GEN_PEAK_EN | 
	(2 << EAP_CAP_GEN_MAX_TX_SHFT) | (2 << EAP_CAP_GEN_MAX_RX_SHFT) |
	EAP_CAP_GEN_STRM_CFG_FLS,
	eapCallBack,
	0, //no private persistent data
	0,
	15000  //store every 15 seconds
};

