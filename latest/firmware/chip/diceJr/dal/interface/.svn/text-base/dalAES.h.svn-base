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
	
	Purpose:	Dice Audio Application Layer (DAL)- AES interface
  
  Description:       
  
	Revisions:
		created 09/03/2005 ML

****************************************************************************/


#ifndef _DALAES_H
#define _DALAES_H

#include "dal.h"
#include "dalClkCtrl.h"
#include "diceAES.h"


/*********** Info functions used by the clock state machine ****************/

//should not be called from outside DAL

// Obtain interface specific PLL settings and rate mode estimate
void dalAESPllSettings (uint32 aesID, DAL_RX_PLL_DATA * pPllData);

// Is the AES interface locked
uint8 dalAESIsLocked (uint32 aesID);

// When the AES is not clock master call this function to set the default local master
void dalAESSetMasterClkDefault(void);

// When the AES is clock master select which sub-interface is selected
void dalAESSetMasterClk (uint32 aesID);


_TC_INLINE void dalAESTxMute (BOOL bMute) _TC_INLINE_ALWAYS;
_TC_INLINE void dalAESTxMute (BOOL bMute)
{
	aesTxSetMute(ALL_CH, bMute);
}

_TC_INLINE void dalAESTxEnable (BOOL bEnable) _TC_INLINE_ALWAYS;
_TC_INLINE void dalAESTxEnable (BOOL bEnable)
{
	aesTxSetValidAudio(ALL_CH, !bEnable);
}

_TC_INLINE void dalAESRxEnable (BOOL bEnable) _TC_INLINE_ALWAYS;
_TC_INLINE void dalAESRxEnable (BOOL bEnable)
{
	aesRxEnable (bEnable);
}


// assume that AES rate enumeration matches DAL_NOMINAL_RATE_ENUM
void dalAESTxSetNominalRate (DAL_NOMINAL_RATE_ENUM rate);

void dalAESRxSetNominalRate (DAL_NOMINAL_RATE_ENUM rate);


/*********** Functions which can be used by the application ****************/

// When AES is not selected as clock master it will need to know which of the 4 interfaces to receive sync from
// default is AES_ANY.
HRESULT dalAESDefaultSyncMode (uint8 syncFrom);


HRESULT dalAESDualwireMode (uint8 bDualWire);

HRESULT dalAESDualwireModeTx (uint8 bDualWire);

HRESULT dalAESDualwireModeRx (uint8 bDualWire);


#endif // _DALAES_H

