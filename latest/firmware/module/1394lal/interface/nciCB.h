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
 * DESCRIPTION: Node Controller Interface Callbacks

	Purpose: Node Controller Interface callbacks

	Callbacks for addresses handled in low level

	Revisions:
		created 04/30/2002 lm
****************************************************************************/

#ifndef _NCICB_H
#define _NCICB_H

#include "TCTypes.h"
#include "1394.h"
#include "1394defs.h"
#include "ErrorCodes.h"
#include "packetBlock.h"
#include "bmdefs.h"

enum
{
	NCI_CB_ARCH_SPACE,
	NCI_CB_SB_SPACE,
	NCI_CB_CONFIG_ROM,
	NCI_CB_TOPOLOGY_MAP,
	NCI_CB_SPEED_MAP
};

/*************************************************************

	nciCBInitialize

*****************************

	Description:

	initializes this modules callback

****************************/

HRESULT	nciCBInitialize(void);

/*************************************************************

	nciCBSBSpace

*****************************

	Description:

	used by NCI for trapping 1394 CSR Serial Bus space register access.
	Serial Bus Address Space includes offsets 0xF0000200 thru 0xF0000400
	includes Bus Mgr and Isoc Resource Mgr (IRM) registers

****************************/

HRESULT nciCBSBSpace(PB *packetBlock);


/*************************************************************

	nciCBConfigROM

*****************************

	Description:

	used by NCI for trapping Config ROM read requests.
	Configuration ROM Address Space includes offsets 0xF0000400 thru 0xF00007FC.

	Implemented Serial Bus Registers
		Configuration ROM is implemented from 0xF0000400 to the extent of
		thisNode's actual ROM length.

****************************/

HRESULT nciCBConfigROM(PB *packetBlock);


/*************************************************************

	nciCBArchSpace

*****************************

	Description:

	used by NCI for trapping 1394 CSR Architecture space register access.
	Architecture Space includes offsets 0xF0000000 thru 0xF00001FC.

****************************/

HRESULT nciCBArchSpace(PB *packetBlock);


/*************************************************************

	nciCBTopologyMap

*****************************

	Description:

	used by NCI for trapping 1394 CSR Topology Map register access.
	The Topology Map includes offsets 0xF0001000 thru 0xF00013FC.

	Implemented Topology Map Registers
		None, pending Bus Manager implementation.

****************************/

HRESULT nciCBTopologyMap(PB *packetBlock);


/*************************************************************

	nciCBSpeedMap

*****************************

	Description:

	used by NCI for trapping 1394 CSR Topology Map register access.
	The Speed Map includes offsets 0xF0002000 thru 0xF0002FFC.

	Implemented Speed Map Registers
		None, pending Bus Manager implementation.

****************************/

HRESULT nciCBSpeedMap(PB *packetBlock);

HRESULT nciCBGeneric(PB *packetBlock, uint32 nciCBtype);
HRESULT nciCBAddressError(PB *packetBlock, uint32 nciCBtype);

#endif
