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
	
	Purpose:	metering.c - Example of a configurable metering system which
				utilize the DICE Jr/Mini metering functionality.
	
	Revisions:
		created 30/10/2007 ml
		

	Use of LED's and Switches in this application
			Single	Eight
	LED1	 0dB	-40dB (signal)
	LED2	-3dB	-40dB (signal)
	LED3	-6dB	-40dB (signal)
	LED4	-12dB	-40dB (signal)
	LED5	-24dB	-40dB (signal)
	LED6	-40dB	-40dB (signal)
	LED7	Off		-40dB (signal)
	LED8	Off		-40dB (signal)
	
	Commands
	>meter.mode  off,single,multi
	>meter.cfg	meter dev ch
	

****************************************************************************/

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "coreDefs.h"
#include "diceDefs.h"
#include "diceRouterReg.h"
#include "targetBoard.h"


typedef struct
{
	uint8	in; //1 if in, 0 if out
	uint8	dev;
	uint8	ch;
	uint8	idx; //index found in router
}METER_DEF;

static uint8 multi; 

static METER_DEF defs[8] = {
	{0,DEV_INS_RX0,0},
	{0,DEV_INS_RX0,1},
	{0,DEV_INS_RX0,2},
	{0,DEV_INS_RX0,3},
	{1,DEV_INS_TX0,0},
	{1,DEV_INS_TX0,1},
	{1,DEV_INS_TX0,2},
	{1,DEV_INS_TX0,3}
};
	

//need a pointer to the peaks
static uint32 * pEntry = (uint32 * )mDicePtr(DICE_ROUTER1_ENTRY);
static uROUTER_SETUP * pSetup = (uROUTER_SETUP * )mDicePtr(DICE_ROUTER1_SETUP);



static void findMeter(METER_DEF * pDef)
{	
	uint8 i=0;
	
	uint8 dev = (pDef->dev<<4) | pDef->ch;
	
	while (i <=pSetup->bit.numEntry)
	{
		if (pDef->in)
		{
			if ((pEntry[i]>>8) == dev) 
			{
				pDef->idx = i;
				return;
			}
		}
		else
		{
			if ((pEntry[i]&0xff) == dev)
			{
				pDef->idx = i;
				return;
			}

		}
		i++;
	}
	pDef->idx = 0xff;
}

void meterUpdateIndices (void)
{
	uint8 i;
	for (i=0; i<8; i++)	findMeter(&defs[i]);
}


#define PEAK_0DB	0xfff
#define PEAK_3DB	0xB50
#define PEAK_6DB	0x800
#define PEAK_12DB	0x400
#define PEAK_24DB	0x100
#define PEAK_40DB	0x28


static uint8 peakToId (uint32 val)
{
	val = val>>16;
	if (val>=PEAK_0DB)  return 0x3f; //all on
	if (val>=PEAK_3DB)  return 0x3e;
	if (val>=PEAK_6DB)  return 0x3c;
	if (val>=PEAK_12DB) return 0x38;
	if (val>=PEAK_24DB) return 0x30;
	if (val>=PEAK_40DB) return 0x20;
	return 0;
}

uint8 meterGetLED (void)
{
	uint32 v;
	uint8  idx,i;
	
	if (!multi)
	{ //single meter
		idx = defs[0].idx;
		if (idx==0xff)
			v = 0;
		else
			v = pEntry[idx];
		return (peakToId (v));
	}
	else
	{ //multi meter
		v=0;
		for (i=0; i<8; i++)
		{
			idx = defs[i].idx;
			v>>=1;
			if ((idx!=0xff) && ((pEntry[idx]>>16)>PEAK_40DB))
				v|=0x80;
		}
		return (uint8)(v);
	}
}

void meterMulti(uint8 ismulti)
{
	multi = ismulti;
}

HRESULT meterSetCh (uint8 meter, uint8 in,uint8 dev, uint8 ch)
{
	HRESULT hRes = NO_ERROR;
	if (meter >=8)
	{
		hRes = E_GEN_ILLEGAL_PARAM;
		sysLogError(hRes, __LINE__, __FILE__);
		return hRes;
	}
	defs[meter].in = in;
	defs[meter].dev = dev;
	defs[meter].ch = ch;
	meterUpdateIndices();
	return hRes;
}

