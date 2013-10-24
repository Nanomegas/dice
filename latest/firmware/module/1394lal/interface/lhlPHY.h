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
 * DESCRIPTION: Link Hardware Abstraction Layer (PHY Packet Handling)

	Purpose:	Link Hardware Abstraction Layer 
				PHY Packet Interface helper functions
	
	Revisions:
		created 05/15/2002 lm

****************************************************************************/

#ifndef _LHLPHY_H
#define _LHLPHY_H

#include "TCTypes.h"

HRESULT		lhlReadPhy(QUADLET qAddr, QUADLET *qData, BOOL bPoll);	// Read from PHY registers
HRESULT		lhlWritePhy(QUADLET qAddr, QUADLET qData);				// Write to PHY registers
HRESULT		lhlPhySetIRMEnabled(BOOL enabled);						// Set IRM Enabled mode
#endif
