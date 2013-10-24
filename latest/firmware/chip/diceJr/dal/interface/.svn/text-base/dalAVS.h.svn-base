//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
//===========================================================================
/****************************************************************************
	
	Purpose:	Dice Audio Application Layer (DAL)- AVS interface
  
  Description:       
  
	Revisions:
		created 09/03/2005 ML

****************************************************************************/

#ifndef _DALAVS_H
#define _DALAVS_H

#include "dal.h"
#include "dalClkCtrl.h"
//#include "avs.h"

/*********** Info functions used by the clock state machine ****************/

//should not be called from outside DAL

// Obtain interface specific PLL settings and rate mode estimate
void dalAVSPllSettings (uint32 plugID, DAL_RX_PLL_DATA * pPllData);

// Is the TDIF interface locked
uint8 dalAVSIsLocked (uint32 plugID);


void dalAVSTxMute (uint32 plugID, BOOL bMute);


// assume that AVS rate enumeration matches DAL_NOMINAL_RATE_ENUM
void dalAVSTxSetNominalRate (uint32 plugID, DAL_NOMINAL_RATE_ENUM rate);


void dalAVSRxSetNominalRate (uint32 plugID, DAL_NOMINAL_RATE_ENUM rate);

void dalAVSTxEnable (uint32 plugID, BOOL bEnable);

void dalAVSRxEnable (uint32 plugID, BOOL bEnable);



/*********** Functions which can be used by the application ****************/



#endif // _DALAVS_H

