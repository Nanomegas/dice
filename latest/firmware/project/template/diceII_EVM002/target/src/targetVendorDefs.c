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
 /***************************************************************************
 * DESCRIPTION: 1394 Vendor and Product IDs

	Revisions:
		created 08/06/2002 lm

****************************************************************************/

#include "TCTypes.h"
#include "targetVendorDefs.h"

#ifdef _CLI
#include "cli.h"

#define	UNKNOWN_DEVICE_NAME		" unknown " // "         "


void targetVendorDefsGetNameFromWWUID(KNOWN_DEVICE_INFO *info)
{	
	//ML: Removed ability to match OUI with name, the list would be incomplete anyway.
	strcpy(info->name, UNKNOWN_DEVICE_NAME);
}

void targetVendorDefsPrintVendorID(uint32 vendorID)
{
	//ML: Removed OUI matching
	cliPrintf("vendorID: 0x%06x\n\r", vendorID);
	cliOutputLine("-----------------------------------------------------------------------");
}

#endif // _CLI

