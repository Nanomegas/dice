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
	
	Purpose:	Defines the specific settings for the AMK4610
				The functions in this module are used by the 
				myMode files.
	
	Revisions:
		created 19/03/2005 ml
		

****************************************************************************/

#include "AKM4620.h"
#include "dal.h"
#include "targetboard.h"

/*
The codec requires a certain InS setup and corresponding SPI commands.
The SPI command from targetBoard is used.

*/

//This function is called at startup (or at mode change). This function will
//initialize the DICE<->Codec interface (I2S in this example) and initialize
//the codec if necessary. With this AKM codec we just initialize it the first
//time it has a good clock in the msgFunc below.
HRESULT akm4620_InitI2S (void)
{
	i2sRxSetMckFreq    (I2S_ID0, FALSE); //normal 256 clock
	i2sTxSetMckFreq    (I2S_ID0, FALSE);

	i2sRxSetM192       (I2S_ID0, FALSE); //default to low/mid mode for now
	i2sTxSetM192       (I2S_ID0, FALSE);

	i2sRxSetClock      (I2S_ID0, TRUE, 0); //256 fs @32k-96k, 128fs @176.4-192k
	i2sTxSetClock      (I2S_ID0, TRUE, 0); 

	i2sRxSetClockInv   (I2S_ID0, TRUE, FALSE); //invert BCLK and LRCLK
	i2sTxSetClockInv   (I2S_ID0, TRUE, FALSE);


	i2sRxSetDataFormat (I2S_ID0, I2S_CH0, TRUE, TRUE, 0, TRUE);
	i2sTxSetDataFormat (I2S_ID0, I2S_CH0, TRUE, TRUE, 0, TRUE);
	i2sRxSetDataFormat (I2S_ID0, I2S_CH1, TRUE, TRUE, 0, TRUE);
	i2sTxSetDataFormat (I2S_ID0, I2S_CH1, TRUE, TRUE, 0, TRUE);

	return NO_ERROR;
}

//This function is called when lock is obtained, lost or when the
//rate mode has changed.
//This particular codec is programmed to use I2S and to use the
//following MCLK frequency:
//@32k-48k use MCLK=256fs (~12MHz range)
//@88.2k-96k use MCLK=245fs (~24MHz range)
//@176.4-192k use MCLK=128gs (~24MHz range)
HRESULT akm4620_msgFunc (MYMODE_MSG msg, uint32 data0, uint32 data1)
{
	switch (msg)
	{
		case MM_LOST_LOCK:
			targetWriteCodecSPIWord (0xa100); //reset AKM codec, this will also mute
			break;
				
		case MM_GOT_LOCK:
			//some codec's might need the I2S setup to change when the mode changes
			//that is not the case with our AKM4620 as we always run 512 base rate.
			
			targetWriteCodecSPIWord (0xa100); //reset	
			switch (data0)
			{
				default:
				case eDAL_RATE_MODE_LOW:
					i2sRxSetM192       (I2S_ID0, FALSE); //default to low/mid mode for now
					i2sTxSetM192       (I2S_ID0, FALSE);
					targetWriteCodecSPIWord (0xa260);
					break;
					
				case eDAL_RATE_MODE_MID:
					i2sRxSetM192       (I2S_ID0, FALSE); //default to low/mid mode for now
					i2sTxSetM192       (I2S_ID0, FALSE);
					targetWriteCodecSPIWord (0xa265);
					break;
					
				case eDAL_RATE_MODE_HIGH:
					i2sRxSetM192       (I2S_ID0, TRUE); //default to low/mid mode for now
					i2sTxSetM192       (I2S_ID0, TRUE);
					targetWriteCodecSPIWord (0xa266);
					break;
			}
			targetWriteCodecSPIWord (0xa103); //un-reset	
			break;					
	
	}
	return NO_ERROR;
	
}
