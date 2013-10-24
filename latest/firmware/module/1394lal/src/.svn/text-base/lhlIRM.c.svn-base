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
 * DESCRIPTION: Link Hardware Abstraction Layer Isochronous Resource Manager

	Purpose: Isochronous Resource Manager functions

	Revisions:
		created 05/14/2002 lm

***********************************************************************/

#include "TCTasking.h"
#include "lhl.h"
#include "lhlIRM.h"
#include "lhlPhy.h"
#include "nciIRM.h"

// Set IRM Enabled mode
HRESULT lhlSetIRMEnabled(BOOL enabled)
{
	HRESULT		hResult = NO_ERROR;
#ifdef _IRMC_CAPS

	hResult = lhlPhySetIRMEnabled(enabled);
#endif
	return hResult;
}
