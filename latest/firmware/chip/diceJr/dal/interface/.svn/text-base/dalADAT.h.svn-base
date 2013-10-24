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
	
	Purpose:	Dice Audio Application Layer (DAL)- ADAT interface
  
  Description:       
  
	Revisions:
		created 09/03/2005 ML

****************************************************************************/

#ifndef _DALADAT_H
#define _DALADAT_H

#include "dal.h"
#include "dalClkCtrl.h"
#include "diceADAT.h"

/*********** Info functions used by the clock state machine ****************/

//should not be called from outside DAL

// Obtain interface specific PLL settings and rate mode estimate
void dalADATPllSettings (uint32 data, DAL_RX_PLL_DATA * pPllData);

// Is the TDIF interface locked
uint8 dalADATIsLocked (uint32 rxid);

_TC_INLINE void dalADATTxMute (BOOL bMute) _TC_INLINE_ALWAYS;
_TC_INLINE void dalADATTxMute (BOOL bMute)
{
	adatTxSetMute(0, ALL_CH, bMute);
	adatTxSetMute(1, ALL_CH, bMute);
}

_TC_INLINE void dalADATTxEnable (BOOL bMute) _TC_INLINE_ALWAYS;
_TC_INLINE void dalADATTxEnable (BOOL bMute)
{
}

_TC_INLINE void dalADATRxEnable (BOOL bEnable) _TC_INLINE_ALWAYS;
_TC_INLINE void dalADATRxEnable (BOOL bEnable)
{
	adatRxEnable (bEnable);
}


// assume that ADAT rate enumeration matches DAL_NOMINAL_RATE_ENUM
_TC_INLINE void dalADATTxSetNominalRate (DAL_NOMINAL_RATE_ENUM rate) _TC_INLINE_ALWAYS;
_TC_INLINE void dalADATTxSetNominalRate (DAL_NOMINAL_RATE_ENUM rate)
{
}


_TC_INLINE void dalADATRxSetNominalRate (DAL_NOMINAL_RATE_ENUM rate) _TC_INLINE_ALWAYS;
_TC_INLINE void dalADATRxSetNominalRate (DAL_NOMINAL_RATE_ENUM rate)
{
}



/*********** Functions which can be used by the application ****************/

#define ADAT_SMUX_USER_MASK	0x04 //bit 2 is the SMUX bit

typedef enum
{
	eDAL_ADAT_SMUX_NORMAL = 0,	// follow rate mode, in low-mid follow USRBIT2 if set, if clear follow user selection
	eDAL_ADAT_SMUX_FORCE_LM,	// follow rate mode, in low-mid follow user selection
	eDAL_ADAT_SMUX_FORCE,		// follow user selection regardles of rate mode and user bit.
	eDAL_ADAT_SMUX_COUNT	
 } DAL_ADAT_SMUX_ENUM;

HRESULT dalSetAdatRxSmuxMode  (DAL_ADAT_SMUX_ENUM mode, uint8 usersel);



#endif // _DALADAT_H

