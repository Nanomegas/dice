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
	
	Purpose:	Dice Audio Application Layer (DAL)- DSAI interface
  
  Description:       
  
	Revisions:
		created 09/03/2005 ML

****************************************************************************/


#ifndef _DALDSAI_H
#define _DALDSAI_H

#include "dal.h"
#include "dalClkCtrl.h"
#include "diceDSAI.h"


/*********** Info functions used by the clock state machine ****************/

//should not be called from outside DAL


_TC_INLINE void dalDSAITxMute (uint32 dsaiID, BOOL bMute) _TC_INLINE_ALWAYS;
_TC_INLINE void dalDSAITxMute (uint32 dsaiID, BOOL bMute)
{
	dsaiTxSetMute(dsaiID, ALL_CH, bMute);
}


_TC_INLINE void dalDSAITxEnable (uint32 dsaiID, BOOL bMute) _TC_INLINE_ALWAYS;
_TC_INLINE void dalDSAITxEnable (uint32 dsaiID, BOOL bMute)
{
}

_TC_INLINE void dalDSAIRxEnable (uint32 dsaiID, BOOL bEnable) _TC_INLINE_ALWAYS;
_TC_INLINE void dalDSAIRxEnable (uint32 dsaiID, BOOL bEnable)
{
}


// assume that DSAI rate enumeration matches DAL_NOMINAL_RATE_ENUM
_TC_INLINE void dalDSAITxSetNominalRate (uint32 dsaiID, DAL_NOMINAL_RATE_ENUM rate) _TC_INLINE_ALWAYS;
_TC_INLINE void dalDSAITxSetNominalRate (uint32 dsaiID, DAL_NOMINAL_RATE_ENUM rate)
{
}


_TC_INLINE void dalDSAIRxSetNominalRate (uint32 dsaiID, DAL_NOMINAL_RATE_ENUM rate) _TC_INLINE_ALWAYS;
_TC_INLINE void dalDSAIRxSetNominalRate (uint32 dsaiID, DAL_NOMINAL_RATE_ENUM rate)
{
}



#endif // _DALDSAI_H

