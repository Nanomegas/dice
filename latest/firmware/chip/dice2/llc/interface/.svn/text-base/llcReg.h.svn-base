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
	
	Purpose:	Link Layer register initialization and access
				for the Samsung 1394 Device Controller Core
	
	Revisions:
		created 12/10/2002 kbj


****************************************************************************/
#ifndef _LLCREG_H
#define _LLCREG_H

#include "llcDefs.h"

_TC_INLINE void llcLinkRegRead(uint32 registerOffset, uint32 *value) _TC_INLINE_ALWAYS;
_TC_INLINE void llcLinkRegRead(uint32 registerOffset, uint32 *value) 
// Read a value from the LLC
{
	*value = *mPtr(registerOffset);
}

_TC_INLINE void llcLinkRegWrite(uint32 registerOffset, uint32 value) _TC_INLINE_ALWAYS;
_TC_INLINE void llcLinkRegWrite(uint32 registerOffset, uint32 value)
// Write a value to the LLC
{
	*mPtr(registerOffset) = value;
}

// Do an atomic read-modify-write of the required register.
// Reads register, masks in WriteValue using Mask and writes it back
void llcLinkMaskAndChg(uint32 registerOffset, uint32 Mask, uint32 WriteValue);

// Do an atomic read-modify-write of the required register
void llcLinkSetBits(uint32 registerOffset, uint32 bitSet);

HRESULT llcReadPhy(uint8 phyAddr, uint32 *value);
// Read the value in the given phy register.
// This can fail or time out. Checking the return value is
// a GOOD idea.

HRESULT llcWritePhy(uint8 phyAddr, uint32 value);
// Write the value to the given PHY register.
// This can fail or time out. Checking the return value is
// a GOOD idea.

// Reads all Phy register into memory phyBaseReg, phyPageXReg 0,1 and 7
void llcReadAllPhyReg(void);

/*********************************************************
 Initialization sequence:
*/
HRESULT llcInitialize(void);

// Called when bus reset detected
HRESULT llcBusResetInitialize(void);

// Called when bus reset completed
HRESULT llcBusResetComplete(void);

// Force a bus reset
HRESULT llcBusReset(uint8 forceRoot);

// Returns TRUE if bus reset is inprogress
BOOL llcIsBusReset(void);

// Returns TRUE if a new bus reset in progress
BOOL llcIsNewReset(void);

BOOL llcIsRoot(void);

// Get the current bus cycle time
HRESULT llcGetCycleTime(uint32* time);

// Set Cycle Master function of the Link chip
HRESULT llcSetCycleMaster(BOOL enabled);

// Returns TRUE if local node is CM
BOOL llcIsCycleMaster(void);

// Set Fairness Control value for this node
void llcSetFairnessControl(uint8 pri_reg);

HRESULT llcHandleCSRarch(uint32 llcArch, uint32 llcType, QUADLET *data);

#ifdef _IRMC_CAPS //LM???
HRESULT llcHandleCSRrsrc(uint32 llcRsrc, uint32 llcType, QUADLET *data);
#endif

HRESULT llcSoftwareReset(void);

#endif
