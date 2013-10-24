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
	
	Purpose:	LLC SelfID DMA controller interface
				for the Samsung 1394 LLC
	
	Revisions:
		created 12/16/2002 kbj


****************************************************************************/
#ifndef _LLCSELFID_H
#define _LLCSELFID_H

#include "bmdefs.h"

// Initialize SelfID controller and buffer
HRESULT llcInitSelfIDController(void);

// Reads selfID buffer 
HRESULT llcGetSelfIDs(SELFID_BUNDLE *pSelfID);

// Verifies the selfID buffer and status from LLC
HRESULT llcVerifySelfIDs(SELFID_BUNDLE *pSelfID);

// Reads and verifies SelfID generation ID
HRESULT llcGetSelfIDGeneration(uint16 *generationID);

// Returns bus number and node number, root = 1 else 0 
HRESULT llcGetBusNodeNumber(uint16 *pBusNumber, uint16 *pNodeNumber, uint8 *pRoot);

#endif
