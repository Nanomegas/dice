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
#include "dalRates.h"
#include "eap.h"
#include "eapCfg.h"
#include "diceDefs.h"
#include "standAlone.h"
#include "targetBoard.h"

#define STDA_INPUT_DEVICES  (DEVICE_BIT(DEV_AES_RX) | DEVICE_BIT(DEV_ADAT_RX) | DEVICE_BIT(DEV_INS_RX0) | DEVICE_BIT(DEV_INS_RX1))

#define STDA_OUTPUT_DEVICES (DEVICE_BIT(DEV_AES_TX) | DEVICE_BIT(DEV_ADAT_TX) | DEVICE_BIT(DEV_INS_TX0) | DEVICE_BIT(DEV_INS_TX1))


extern uint32	CLOCK_SOURCE[];

void StandAloneInitialize (void)
{
	//we need to figure out what dal to create
	EAP_STND_ALONE_CFG const * pStdaCfg;
	DAL_RATE_MODE_ENUM rate_mode = eDAL_RATE_MODE_LOW_MID;
	DAL_NOMINAL_RATE_ENUM rate = eDAL_NOMINAL_RATE_ANY;
	DAL_CLOCK_SOURCE_ENUM clksrc;
	
	pStdaCfg = eapGetStandaloneCfg();
	clksrc = CLOCK_SOURCE[pStdaCfg->clkSrc];
	if (clksrc == eDAL_CLK_SRC_AESRX0)
	{
		rate_mode = eDAL_RATE_MODE_ALL; //auto lock to 32k-192k
	}
	else if (clksrc == eDAL_CLK_SRC_ADATRX)
	{
		if (pStdaCfg->extADAT == 0) //Normal
			rate_mode = eDAL_RATE_MODE_LOW;
		else if (pStdaCfg->extADAT == 1) //SMUX II
			rate_mode = eDAL_RATE_MODE_MID;
		else if (pStdaCfg->extADAT == 2) //SMUX IV
			rate_mode = eDAL_RATE_MODE_HIGH;
		else //Auto
			rate_mode = eDAL_RATE_MODE_LOW_MID;
	}
	else if (clksrc == eDAL_CLK_SRC_INTERNAL)
	{
		rate_mode = dalRateByID (pStdaCfg->extInt)->rateMode;
		rate = pStdaCfg->extInt;
	}
	else
	{
		//illegal, do something
		rate = eDAL_NOMINAL_RATE_48;
	}
	dalCreateInterface(eDAL_INTERFACE_1, rate_mode, STDA_INPUT_DEVICES,STDA_OUTPUT_DEVICES);
	dalSetClockSource (eDAL_INTERFACE_1, clksrc, rate);
	dalStartInterface(eDAL_INTERFACE_1);
	//update the routing now according to the currently locked rate mode, in auto modes this might change
	//so in that case this function must be called.
	//updateStdaRouting(); Happens later
}

#define ROUTE(srcDev,srcCh,dstDev,dstCh) (((srcDev)<<12) + ((srcCh)<<8) + ((dstDev)<<4) + (dstCh))


//
// There might be many different stand alone routings and they could be selected by UI or by
// persistent storage etc.
// In this example there are one
static const uint16 stdAloneRouting[] = 
{
	//From analog to SPDIF
	ROUTE(DEV_INS_RX1,0,DEV_AES_TX,6),
	ROUTE(DEV_INS_RX1,1,DEV_AES_TX,7),

	//Fron Analog to Analog
	ROUTE(DEV_INS_RX1,0,DEV_INS_TX1,0),
	ROUTE(DEV_INS_RX1,1,DEV_INS_TX1,1)


};


static void updateRouter (uint16 const * pRoutes, uint16 nbRoutes)
{
	uROUTER_SETUP		setup;
	uROUTER_SETUP	*	pSetup = (uROUTER_SETUP *) mDicePtr(DICE_ROUTER1_SETUP);
	uROUTER_ENTRY	*	pEntry = (uROUTER_ENTRY *) mDicePtr(DICE_ROUTER1_ENTRY);
	uint16              n = nbRoutes;
	
	
	while (n--)
	{
		pEntry->reg = (uint32)(*pRoutes++);
		pEntry++;
	}
	setup.reg = 0;
	if (!nbRoutes)
	{
		setup.bit.on = 0;
	}
	else
	{
		setup.bit.numEntry = nbRoutes-1;
		setup.bit.peakon = 1;
		setup.bit.on = 1;
	}
	*pSetup = setup;
}

void StandAloneUpdateRouting (DAL_RATE_MODE_ENUM rate_mode)
{
	updateRouter (stdAloneRouting, sizeof(stdAloneRouting)/sizeof(uint16));
}

