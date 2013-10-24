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
	
	Purpose:	Dice Audio Application Layer (DAL)- TDIF interface
  
  Description:       
  
	Revisions:
		created 09/03/2005 ML

****************************************************************************/

#ifndef _DALTDIF_H
#define _DALTDIF_H

#include "dal.h"
#include "dalClkCtrl.h"
#include "diceTDIF.h"

/*********** Info functions used by the clock state machine ****************/

//should not be called from outside DAL

// Obtain interface specific PLL settings and rate mode estimate
void dalTDIFPllSettings (uint32 data, DAL_RX_PLL_DATA * pPllData);

// Is the TDIF interface locked
uint8 dalTDIFIsLocked (uint32 data);


_TC_INLINE void dalTDIFTxMute (BOOL bMute) _TC_INLINE_ALWAYS;
_TC_INLINE void dalTDIFTxMute (BOOL bMute)
{
	tdifTxSetMute(ALL_CH, bMute);
}


_TC_INLINE void dalTDIFTxEnable (BOOL bMute) _TC_INLINE_ALWAYS;
_TC_INLINE void dalTDIFTxEnable (BOOL bMute)
{
}

_TC_INLINE void dalTDIFRxEnable (BOOL bEnable) _TC_INLINE_ALWAYS;
_TC_INLINE void dalTDIFRxEnable (BOOL bEnable)
{
	tdifRxEnable (bEnable);
}


// assume that TDIF rate enumeration matches DAL_NOMINAL_RATE_ENUM
_TC_INLINE void dalTDIFTxSetNominalRate (DAL_NOMINAL_RATE_ENUM rate) _TC_INLINE_ALWAYS;
_TC_INLINE void dalTDIFTxSetNominalRate (DAL_NOMINAL_RATE_ENUM rate)
{
	tdifTxSetSetup (rate, FALSE);
}

_TC_INLINE void dalTDIFRxSetNominalRate (DAL_NOMINAL_RATE_ENUM rate) _TC_INLINE_ALWAYS;
_TC_INLINE void dalTDIFRxSetNominalRate (DAL_NOMINAL_RATE_ENUM rate)
{
}




#endif // _DALTDIF_H

