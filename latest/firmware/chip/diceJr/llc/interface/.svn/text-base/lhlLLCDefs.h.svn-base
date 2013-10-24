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
//===========================================================================
/****************************************************************************
 * DESCRIPTION: Link Hardware Abstraction Layer LLC Defines and Structures

	Purpose:	Link Hardware Abstraction Layer
				Link Layer Controller Specific
				Defines, Typedefs and Structs
	
	Revisions:
		created 12/17/2002 kbj
****************************************************************************/

#ifndef _LHLLLCDEFS_H
#define _LHLLLCDEFS_H

#include "llcDefs.h"

#define		BUSINFO_LLC_SPECIFIC	(BUSINFO_NOCMC_ACC | BUSINFO_MAXREC512 | BUSINFO_MAX_ROM_64 | BUSINFO_GEN_INIT | BUSINFO_MAX_SPD_400)
//#define	BUSINFO_LLC_SPECIFIC	(BUSINFO_NOCMC_ACC | BUSINFO_MAXREC1024 | BUSINFO_MAX_ROM_4 | BUSINFO_GEN_MIN | BUSINFO_MAX_SPD_400)

// link max packet size based on OHCI controller 
#define		LINK_MAXTXPACKET_SIZE_QUADS		LLC_ASY_TX_DEPTH
#define		LINK_MAXTXPACKET_SIZE_BYTES		(LLC_ASY_TX_DEPTH*4)
#define		LINK_MAXTXPAYLOAD_SIZE_BYTES	((LLC_ASY_TX_DEPTH-4)*4)

#define		LINK_MAXRXPACKET_SIZE_QUADS		LLC_ASY_RX_DEPTH
#define		LINK_MAXRXPACKET_SIZE_BYTES		(LLC_ASY_RX_DEPTH*4)
#define		LINK_MAXRXPAYLOAD_SIZE_BYTES	((LLC_ASY_RX_DEPTH-4)*4)


#define		MASK_AS_CHANNEL				0x3F
#define		SHIFT_AS_CHANNEL			8
#define		MASK_AS_TAG					0x3
#define		SHIFT_AS_TAG				14
#define		MASK_AS_SY					0xF

#endif //_LHLLLCDEFS_H
