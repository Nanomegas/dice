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
 * DESCRIPTION: Node CSR support

	Purpose:	

		Configuration ROM ("ROM Space")

	Revisions:
		created 04/11/2002 lm
****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "csr.h"
#include "targetConfigCSR.h"

uint32 csrGetConfigROMAddress()
{
	return targetConfigGetConfigROMAddress();
}

QUADLET csrReadConfigROMQuadlet(uint32 offset)
{
	QUADLET quadlet = 0;

	if (offset >= CSR_ROM_SPACE_START && 
		offset < CSR_ROM_SPACE_START + csrGetConfigROMQuadletSize() * 4)
	{
		quadlet = targetConfigReadConfigROMIndex((offset - CSR_ROM_SPACE_START) / 4);
	}

	return quadlet;
}

uint32 csrGetConfigROMQuadletSize(void)
{
	return targetConfigGetROMQuadletSize();
}

HRESULT csrBuildConfigROM(uint32 chip_id_low, uint32 chip_id_hi)
{
	return targetConfigBuildConfigROM(chip_id_low, chip_id_hi);
}

HRESULT csrUpdateConfigROM(uint32 *data, uint32 size)
{
	return targetConfigUpdateConfigROM(data, size);
}

BOOL csrIsCMCCapable(void)
{
	return ((csrReadConfigROMQuadlet(CSR_ROM_BUS_INFO_2ND) & BUSINFO_CMC) == BUSINFO_CMC);
}
