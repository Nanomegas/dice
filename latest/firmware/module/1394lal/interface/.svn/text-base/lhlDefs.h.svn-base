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
 * DESCRIPTION: Link Hardware Abstraction Layer Defines and Structures

	Purpose:	Link Hardware Abstraction Layer
				Defines, Typedefs and Structs
	
	Revisions:
		created 05/23/2002 lm
****************************************************************************/

#ifndef _LHLDEFS_H
#define _LHLDEFS_H

#include "TCTypes.h"
#include "1394.h"
#include "1394defs.h"
#include "bmdefs.h"

// semaphore timeout for pending requests and responses
// and timeout for checking bus reset
#if 0 //LM??? Debugging increase
#define			LHL_TX_REQ_TIMEOUT_MSECS			1200000
#define			LHL_TX_REQ_SHORT_TIMEOUT_MSECS		1200000
#define			LHL_TX_RSP_TIMEOUT_MSECS			1200000
#define			BRI_BR_TIMEOUT_MSECS				200000
#else
#define			LHL_TX_REQ_TIMEOUT_MSECS			1200
#define			LHL_TX_REQ_SHORT_TIMEOUT_MSECS		100
#define			LHL_TX_RSP_TIMEOUT_MSECS			100		//LM??? 1200
#define			BRI_BR_TIMEOUT_MSECS				20000
#endif
#define			LHL_PACKET_AGEOUT_MSECS				(LHL_TX_REQ_TIMEOUT_MSECS * 10) //LM???

typedef enum
{
	LHL_GEN_UNDEF = 0,
	LHL_REQ_READ = 10,
	LHL_REQ_WRITE,
	LHL_REQ_LOCK,
#ifdef _LOOSE_ISO
	LHL_REQ_STREAM,
#endif //_LOOSE_ISO
	LHL_RSP_READ = 20,
	LHL_RSP_WRITE,
	LHL_RSP_LOCK,
	LHL_RSP_ERROR
} LHL_GEN_TYPE;	// genType: covering both request and response types

typedef enum
{
	LHL_AX = 0,
	LHL_RX,
	LHL_TX
} LHL_AX_TYPE;	// axType: async transaction covering both rx and tx types


// private to lhl modules
#define			LHL_UNKNOWN_DATA_LEN		0xffff
#define			LHL_BROADCAST_NODE_ADDR		BROADCAST_NODE_ADDR
#define			LHL_ASYNC_DUMMY_NODE_ADDR	0x80000000
// end private lhl

#endif
