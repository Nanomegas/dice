//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004-2007 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
	
	Purpose:	DICE  Connection Mamagement System Protocol
	
	Revisions:
		created 27/08/2007 ml
****************************************************************************/
/** @addtogroup CMSAVS

	This is a sub-module which is part of the Connection Management System.
	This module will handle allocation of Isoc channels and bandwidth from
	the IRM node. It also handles all settings related to AVS Transmitters.
	When this module is in use AVS functions changing state should not be
	called form elsewhere except the sample rate changes handled directly
	by DAL.
	@{
*/

#ifndef _CMS_AVS_H_
#define _CMS_AVS_H_

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "dal.h"

#define CMSAVS_ISOC_DISABLE 0xff
typedef enum
{
	CMS_ISO_DISABLED, //isoc not directed to stream
	CMS_ISO_STRM, //a okay
	CMS_ISO_NO_CH, //channel not avail
	CMS_ISO_NO_BW, //bw not avail
} CMS_ISO_STAT;



extern HRESULT cmsAvsSetIsocConfig (uint8 txID, uint8 isocCh, uint8 audioChs, uint8 bCommCh, uint8 speed);
extern uint8 cmsAvsIsIsocConfigured(uint8 txID);
extern HRESULT cmsAvsSetRateMode (DAL_RATE_MODE_ENUM rateMode);
extern CMS_ISO_STAT cmsAvsGetIsoState (uint8 txID);
extern uint8 cmsAvsGetIsocCh(uint8 txID);


/* Std prototypes for thread module, called from cms */

extern HRESULT cmsAvsInitialize(void);
extern HRESULT cmsAvsResumeTasks(void);
extern HRESULT cmsAvsSuspendTasks(void);
extern HRESULT cmsAvsInitializeTasks(void);

//
#endif  //_CMS_AVS_H_
