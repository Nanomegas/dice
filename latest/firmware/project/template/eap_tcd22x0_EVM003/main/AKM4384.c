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
//===========================================================================
// $Id: myModes.c,v 1.3 2007/02/08 17:59:33 zluo Exp $
//===========================================================================
/****************************************************************************
	
	Purpose:	Defines the specific settings for the AMK4610
				The functions in this module are used by the 
				myMode files.
	
	Revisions:
		created 19/03/2005 ml
		

****************************************************************************/

#include "AKM4384.h"
#include "dal.h"
#include "targetboard.h"

/*
The codec requires a certain InS setup and corresponding SPI commands.
The SPI command from targetBoard is used.

*/

//This function is called at startup, we only set the clock ports. SPI commands
//are sent in the message function based on lock.

HRESULT akm4384_InitI2S (void)
{
	insSetClockPortI2S    (INS_ID0, INS_MCK_512BR, true);
	insTxSetDataFormatI2S (INS_ID0, INS_LN0, true);
	insRxSetDataFormatI2S (INS_ID0, INS_LN0, true);
	return NO_ERROR;
}


HRESULT akm4384_msgFunc (CODEC_MSG msg, uint32 data0, uint32 data1)
{
	switch (msg)
	{
		case MM_LOST_LOCK:
			targetWriteCodecSPIWord(0x608c); //pwr down, set for I2C
			break;
				
		case MM_GOT_LOCK:
			targetWriteCodecSPIWord(0x608f); //pwr up, set for I2C
			break;		
			
		case MM_SET_OUTGAIN:
			targetWriteCodecSPIWord(0x6300 | (data0&0xff)); //left gain
  			targetWriteCodecSPIWord(0x6400 | (data0&0xff)); //right gain
			break;
	
	}
	return NO_ERROR;
	
}
