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
 * DESCRIPTION: Target Config CSR Handling

	Configuration of target specific CSR handling
	Under normal circumstances there is no need to modify this file. It will 
	build the unit directory based on information in targetVendorDefs.h. This
	module uses the compiler defines to determine which unit directories are
	needed and it currently supports DiceDriver, AV/C, two flavors of mLan and
	DiceSimple.
	The system will call the functions defined below when the ROM is requested
	to be created and whenever parts of the rom are requested for read.

	Revisions:
		created 10/23/2003 lm

****************************************************************************/

#ifndef _TARGETCONFIGCSR_H
#define _TARGETCONFIGCSR_H

#include "TCTypes.h"

////////////////////// Functions called by the system /////////////////////////////

/// The system uses this function to enquire about the size of the current config rom
uint32		targetConfigGetROMQuadletSize(void);									// return number of quadlet values in Configuration ROM

/// The system uses this function to enquire about ROM content when requested from the network
QUADLET		targetConfigReadConfigROMIndex(uint32 index);							// read quadlet value in Configuration ROM based on quadlet index

/// The system calls this function during early boot before enabeling the link
/// a valid config rom shoud be created here.
HRESULT		targetConfigBuildConfigROM(uint32 chip_id_low, uint32 chip_id_hi);		// generate the Configuration ROM based on chip_id


////////////////////// Functions available to the application //////////////////////

/// This function is not used by the system but the application can call this to
/// make sure the ROM is changed before reboot. This is typically used in 
/// targetBoard.c when preparing for reset.
HRESULT		targetSetMinConfigROM(void);



////////////////////// Functions for debug (CRB) /////////////////////////////////

/// This function is for debugging purposes and not used by the general system
HRESULT		targetConfigUpdateConfigROM(uint32 *data, uint32 size);					// complete update the Configuration ROM 

/// This function is for debugging purposes and not used by the general system
uint32		targetConfigGetConfigROMAddress(void);										// return base address of Configuration ROM



#endif //_TARGETCONFIGCSR_H
