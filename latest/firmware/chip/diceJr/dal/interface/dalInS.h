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
/****************************************************************************
	
	Purpose:	Dice Audio Application Layer (DAL)- InS interface
  
  Description:       
  
	Revisions:
		created 02/02/2007 ML

****************************************************************************/


#ifndef _DALINS_H
#define _DALINS_H

#include "dal.h"
#include "dalClkCtrl.h"
#include "diceInS.h"


/*********** Info functions used by the clock state machine ****************/

//should not be called from outside DAL

_TC_INLINE void dalInSTxMute (uint32 insID, BOOL bMute) _TC_INLINE_ALWAYS;
_TC_INLINE void dalInSTxMute (uint32 insID, BOOL bMute)
{
	insTxSetMute(insID, ALL_CH, bMute);
}


_TC_INLINE void dalInSTxEnable (uint32 insID, BOOL bMute) _TC_INLINE_ALWAYS;
_TC_INLINE void dalInSTxEnable (uint32 insID, BOOL bMute)
{
}

_TC_INLINE void dalInSRxEnable (uint32 insID, BOOL bEnable) _TC_INLINE_ALWAYS;
_TC_INLINE void dalInSRxEnable (uint32 insID, BOOL bEnable)
{
}


// assume that InS rate enumeration matches DAL_NOMINAL_RATE_ENUM
_TC_INLINE void dalInSTxSetNominalRate (uint32 insID, DAL_NOMINAL_RATE_ENUM rate) _TC_INLINE_ALWAYS;
_TC_INLINE void dalInSTxSetNominalRate (uint32 insID, DAL_NOMINAL_RATE_ENUM rate)
{
}


_TC_INLINE void dalInSRxSetNominalRate (uint32 insID, DAL_NOMINAL_RATE_ENUM rate) _TC_INLINE_ALWAYS;
_TC_INLINE void dalInSRxSetNominalRate (uint32 insID, DAL_NOMINAL_RATE_ENUM rate)
{
}



#endif // _DALINS_H

