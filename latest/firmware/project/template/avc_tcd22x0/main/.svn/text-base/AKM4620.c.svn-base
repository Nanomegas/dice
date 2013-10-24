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
	insSetClockPortI2S    (INS_ID0, INS_MCK_512BR, TRUE);//always use 512 Base Rate for all modes
	insRxSetDataFormatI2S (INS_ID0, INS_LN0, TRUE);
	insRxSetDataFormatI2S (INS_ID0, INS_LN1, TRUE);
	insTxSetDataFormatI2S (INS_ID0, INS_LN0, TRUE);
	insTxSetDataFormatI2S (INS_ID0, INS_LN1, TRUE);
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
					targetWriteCodecSPIWord (0xa264);
					break;
					
				case eDAL_RATE_MODE_MID:
					targetWriteCodecSPIWord (0xa265);
					break;
					
				case eDAL_RATE_MODE_HIGH:
					targetWriteCodecSPIWord (0xa266);
					break;
			}
			targetWriteCodecSPIWord (0xa103); //un-reset	
			break;					
	
	}
	return NO_ERROR;
	
}
