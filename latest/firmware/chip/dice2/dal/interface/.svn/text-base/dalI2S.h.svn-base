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
	
	Purpose:	Dice Audio Application Layer (DAL)- I2S interface
  
  Description:       
  
	Revisions:
		created 09/03/2005 ML

****************************************************************************/


#ifndef _DALI2S_H
#define _DALI2S_H

#include "dal.h"
#include "dalClkCtrl.h"
#include "diceI2S.h"


/*********** Info functions used by the clock state machine ****************/

//should not be called from outside DAL


_TC_INLINE void dalI2STxMute (uint32 i2sID, BOOL bMute) _TC_INLINE_ALWAYS;
_TC_INLINE void dalI2STxMute (uint32 i2sID, BOOL bMute)
{
	i2sTxSetMute(i2sID, ALL_CH, bMute);
}

_TC_INLINE void dalI2STxEnable (uint32 i2sID, BOOL bMute) _TC_INLINE_ALWAYS;
_TC_INLINE void dalI2STxEnable (uint32 i2sID, BOOL bMute)
{
}

_TC_INLINE void dalI2SRxEnable (uint32 i2sID, BOOL bEnable) _TC_INLINE_ALWAYS;
_TC_INLINE void dalI2SRxEnable (uint32 i2sID, BOOL bEnable)
{
}


// assume that I2S rate enumeration matches DAL_NOMINAL_RATE_ENUM
_TC_INLINE void dalI2STxSetNominalRate (uint32 i2sID, DAL_NOMINAL_RATE_ENUM rate) _TC_INLINE_ALWAYS;
_TC_INLINE void dalI2STxSetNominalRate (uint32 i2sID, DAL_NOMINAL_RATE_ENUM rate)
{
}

_TC_INLINE void dalI2SRxSetNominalRate (uint32 i2sID, DAL_NOMINAL_RATE_ENUM rate) _TC_INLINE_ALWAYS;
_TC_INLINE void dalI2SRxSetNominalRate (uint32 i2sID, DAL_NOMINAL_RATE_ENUM rate)
{
}



#endif // _DALI2S_H

