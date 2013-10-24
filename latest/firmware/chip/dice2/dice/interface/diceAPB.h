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
	
	Purpose:	DICE - APB receiver and transmitter API
	
	Revisions:
		created 26/06/2003 kbj

****************************************************************************/

#ifndef _DICEAPB_H
#define _DICEAPB_H

#include "diceAPBReg.h"

// If new APB data then copy received samples to transmitter
void apbLoopTest(void);

// Multiply data with volume divided with 256 (volume 0 - 256)
HRESULT apbSetVolume(int32 volume);

HRESULT apbInitialize(void);

#endif
