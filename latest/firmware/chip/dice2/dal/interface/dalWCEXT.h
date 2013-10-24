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
	
	Purpose:	Dice Audio Application Layer (DAL)- WC EXT interface
  
  Description:       
  
	Revisions:
		created 09/03/2005 ML

****************************************************************************/

#ifndef _DALWCEXT_H
#define _DALWCEXT_H

#include "dal.h"
#include "dalClkCtrl.h"

/*********** Info functions used by the clock state machine ****************/

//should not be called from outside DAL

// Obtain interface specific PLL settings and rate mode estimate
void dalWCPllSettings (uint32 data, DAL_RX_PLL_DATA * pPllData);
void dalExtPllSettings (uint32 data, DAL_RX_PLL_DATA * pPllData);


/*********** Functions which can be used by the application ****************/

typedef enum
{
	eDAL_WCIN_ACTUAL_RATE = 0,	// The WC rate is the actual rate
	eDAL_WCIN_BASE_RATE,		// Always expected to be base rate. If rate mode is low-mid system will default to low
	eDAL_WCIN_COUNT
} DAL_WCIN_ENUM;

HRESULT dalSetWcInMode (DAL_WCIN_ENUM mode);
HRESULT dalSetExtInMode (DAL_WCIN_ENUM mode);


#endif // _DALWCEXT_H

