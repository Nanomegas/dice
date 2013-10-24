//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2005 TC Applied Technologies  Ltd.(ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
//===========================================================================
// $Id: myModes.h,v 1.3 2007/02/08 17:59:33 zluo Exp $
//===========================================================================
/****************************************************************************
	
	Purpose:	Defines the specific settings for the AMK4610
				The functions in this module are used by the 
				myMode files.

	
	Revisions:
		created 02/02/2005 arg



****************************************************************************/
#ifndef _AKM4384_H_
#define	_AKM4384_H_

#include "TCTypes.h"
#include "ErrorCodes.h"

#define AKM4384_AD_LATENCY_LOW		(43 + 2)
#define AKM4384_AD_LATENCY_MID		(43 + 3)
#define AKM4384_AD_LATENCY_HIGH		(38 + 3)

#define AKM4384_DA_LATENCY_LOW		(2 + 28)
#define AKM4384_DA_LATENCY_MID		(2 + 28)
#define AKM4384_DA_LATENCY_HIGH		(2 + 28)

typedef enum
{
	MM_LOST_LOCK, //typically used to mute codec's
	MM_GOT_LOCK, //set clock port (if changed) umute/config codec's, data0= actual rate mode
	MM_SET_OUTGAIN
}CODEC_MSG;

extern HRESULT akm4384_InitI2S (void);
extern HRESULT akm4384_msgFunc (CODEC_MSG msg, uint32 data0, uint32 data1);

#endif//_AKM4384_H_
