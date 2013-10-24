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
		created 12/17/2002 kbj

***********************************************************************/

#include "ErrorCodes.h"
#include "lhl.h"
#include "lhlPHY.h"
#include "llcPHY.h"
#include "llcReg.h"

//static char 		moduleName[] = __FILE__ ;

// Read from PHY registers
HRESULT lhlReadPhy(QUADLET qAddr, QUADLET *qData, BOOL bPoll)
{
	UNUSED_ARG(bPoll);
	return llcReadPhy(qAddr, qData);
}

// Write to PHY registers
HRESULT lhlWritePhy(QUADLET qAddr, QUADLET qData)
{
	return llcWritePhy(qAddr, qData);
}

#ifdef _IRMC_CAPS // iso_resource_manager capable
// Set IRM Enabled mode
HRESULT lhlPhySetIRMEnabled(BOOL enabled)
{
	HRESULT		hResult = NO_ERROR;
	uint32		phyReg = 0;

	hResult = lhlReadPhy(PHY_LC_PWR, &phyReg, FALSE);

	if (hResult == NO_ERROR)
	{
#if 1
		if (enabled)	phyReg = (phyReg | PHY_LC_PWR_L) | (PHY_LC_PWR_C);
		else			phyReg = (phyReg | PHY_LC_PWR_L) & (~PHY_LC_PWR_C);
#else
		if (enabled)	phyReg |= PHY_LC_PWR_C | PHY_LC_PWR_L;
		else			phyReg &= ~(PHY_LC_PWR_C | PHY_LC_PWR_L);
#endif
		hResult = lhlWritePhy(PHY_LC_PWR, phyReg);
	}

	return hResult;
}
#endif //_IRMC_CAPS
