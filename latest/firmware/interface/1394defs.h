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
 * DESCRIPTION: Defines from the 1394 specification

	Purpose:	This file contains helpful defines 
				corresponding to various 1394 constructs
	
	Revisions:
		created 04/23/2002 lm

****************************************************************************/

#ifndef _1394DEFS_H
#define _1394DEFS_H

#include "TCTypes.h"
#include "1394.h"

/*
	WWUID : vVV4 3210
	World Wide Unique Id
		the hi word contains
			the vendor_id (v above)
			the upper byte of the serial number (4 above)
		the low word contains
			the lower 4 bytes of the serial number (3-0 above)
*/

typedef struct
{
	uint32	hi ;
	uint32	low ;
} WWUID ;

/* RCODE_1394: Response code definitions */

typedef enum
{
	RSP_COMPLETE =					RESP_CODE_COMPLETE,
	RSP_CONFLICT_ERROR = 			RESP_CODE_CONFLICT,
	RSP_DATA_ERROR =				RESP_CODE_DATA_ERROR,
	RSP_TYPE_ERROR =				RESP_CODE_TYPE_ERROR,
	RSP_ADDRESS_ERROR =				RESP_CODE_ADDRESS_ERROR
} RCODE_1394;

typedef struct 
{
	QUADLET Low;
	DOUBLET High;
} OFFSET_1394;

#define MAX_NODES_PER_BUS			63
#define MAX_SID_QUADS_PER_NODE		3
#define MAX_SELF_ID_PACKETS			(MAX_NODES_PER_BUS * MAX_SID_QUADS_PER_NODE)

#define	BROADCAST_NODE_ID			0x003F
#define	BROADCAST_NODE_ADDR			0xFFFF
									
#define ROOT_ID_UNDEF				0x003F
#define BUS_MANAGER_ID_UNDEF		0x003F
#define CYCLE_MASTER_ID_UNDEF		0x003F
									
#define ISOCH_CHANNELS_ITEMS		64			
#define MAX_BWU 					6144		// maximum theoretical value (at 1600Mb/s)
#define MAX_BWU_ISOCH 				4915		// bandwidth set-aside for isochronous transfers
#define MAX_BWU_ASYNCH 				1229		// bandwidth set-aside for asynchronous transfers
#define BW_REMAINING_MASK			0x1FFF
#define INITIAL_CHANNELS_AVAIL_HI	0xFFFFFFFE	// channel 31 is reserved for async stream channel
#define INITIAL_CHANNELS_AVAIL_LO	0xFFFFFFFF	// 
#define DEFAULT_BROADCAST_CHAN		31			// default broadcast channel for async stream

#define CYCLE_MASTER_START			BIT8		// CMSTR

/* Register space starts at address offset 0xFFFF F000 0000
*/

#define REGISTER_SPACE_ADDR_HI		0xFFFF
									
									
/* CSR and config ROM address space.  
	Are in the register address space and so the high order bits are 0xFFFF
	
	So in this file, we treat addresses as the low-order 32-bits only.
*/

									
#define	CONFIG_ROM_BASE_ADDR		0xF0000400
#define	BUS_INFO_BLOCK_ADDR			0xF0000404
									

/* FCP register addresses

	FCP register is also in the register space, so the high-order 16 bits are 0xFFFF

*/ 
									
#define FCP_COMMAND_BASE_ADDR		0xF0000B00
#define FCP_RESPONSE_BASE_ADDR		0xF0000D00
									
/***************************************************************************/

#endif
