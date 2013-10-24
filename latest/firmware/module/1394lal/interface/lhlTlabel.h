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
  * DESCRIPTION: Link Hardware Abstraction Layer Transaction Label 

	Purpose:	Link Hardware Abstraction Layer 
						handling of transaction labels
	
	Revisions:
		created 28/08/2002 lm
****************************************************************************/

#ifndef _LHL_TLABEL_H
#define _LHL_TLABEL_H

#include "TCTypes.h"

#define TLABEL_VALUE_MIN					1	 // tLabel = 0 not used for tLabel (for read, write, lock)
#define TLABEL_VALUE_MAX					62 //ML: reserve label 63 for FIQ context.
#define TLABEL_VALUE_ITEMS					64

#ifdef _LOOSE_ISO
#define TLABEL_VALUE_ASYNC_STREAM_MIN		0
#define TLABEL_VALUE_ASYNC_STREAM_MAX		63
#endif //_LOOSE_ISO

// maintain an pool of available transaction labels uniquely identifying the transaction
typedef struct
{
	BOOL	allocated[TLABEL_VALUE_ITEMS];
	uint32	ptr;
} TLABEL_POOL;


HRESULT			lhlTLabelInitialize(void);			// Initializes this module
HRESULT			lhlTLabelCreateSemaphores(void);
HRESULT			lhlTLabelInitializeTLabelPool(void);

HRESULT			lhlTLabelGetShifted(uint32 *tLabel);
HRESULT			lhlTLabelGet(uint32 *tLabel);
HRESULT			lhlTLabelPutShifted(uint32 tLabel);
HRESULT			lhlTLabelPut(uint32 tLabel);

#ifdef _LOOSE_ISO
HRESULT			lhlTLabelGetShiftedAsyncStream(uint8 channel, uint32 *tLabel);
HRESULT			lhlTLabelGetAsyncStream(uint8 channel, uint32 *tLabel);
HRESULT			lhlTLabelPutShiftedAsyncStream(uint32 tLabel);
HRESULT			lhlTLabelPutAsyncStream(uint32 tLabel);
#endif //_LOOSE_ISO

#endif
