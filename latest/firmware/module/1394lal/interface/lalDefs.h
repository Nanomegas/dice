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

/** @addtogroup LinkAbstractionLayer
	@{
*/


#ifndef _LALDEFS_H
#define _LALDEFS_H

#include "TCTypes.h"
#include "1394.h"
#include "1394defs.h"
#include "packetBlock.h"
#include "bmDefs.h"

/**
	NOTE: if implementing SBP-2, IP1394, or Asynchronous Connections
	this number may have to be increased significantly.
*/
#define NUM_ALLOCATABLE_ADDR_RANGES		64

//! LAL device information.
typedef struct
{
	BOOL			active;			//!< True if currently on the bus
	uint32			nodeAddr;		//!< Current (busId | nodeId)
	WWUID			WWUID;			//!< World-Wide Unique device Id
	uint32			maxRec;			//!< max_rec code for max packet size
} LAL_DEVICE_INFO;

//! LAL bus reset information
typedef  struct
{
	BOOL			bValid;			//!< indicates valid bus reset post-processing
	uint32			currentTime;	//!< the current cycle-time
	uint32			busGeneration;	//!< number of bus resets seen since power-on
	uint32			numNodes;		//!< number of nodes found on the bus
	uint32			rootAddr;		//!< current root node address
	uint32			irmAddr;		//!< current IRM node address
	uint32			numSelfIDs;		//!< number of self-id quadlets captured at last bus reset
	uint32			nodeAddr;		//!< thisNode address
	uint16			busId;			//!< the local bus address
	QUADLET*		SIDBundle;		//!< pointer to the self-id quadlets
} LAL_BUS_RESET_INFO;

/** Used for 1394 i/o to devices, persistent across bus reset
	for compliant devices (i.e. have General Config ROM format)
*/
typedef uint32 LAL_NODE_HANDLE;		

//! The function type for bus reset callbacks.
typedef void lalBusResetCB(void);

//! The function type for LAL callbacks (address ranges or asynchronous streams).
typedef HRESULT (*LAL_CALLBACK)(PB *);

//! The LAL speed codes.
typedef enum
{
	lal100 = SPEED_100, 
	lal200 = SPEED_200, 
	lal400 = SPEED_400, 
	lalSpeedFastest = SPEED_RESERVED,
	lalSpeedUndef = (SPEED_RESERVED+1),
	lalSpeedDefault = lal100 //LM??? or use lal400?
} LAL_SPEED;


//! LAL address range
typedef struct
{
	BOOL			inUse;				//!< true if currently used address range
	OFFSET_1394		AddrStart;			//!< start of address range, inclusive
	OFFSET_1394		AddrEnd;			//!< end of address range, inclusive
	LAL_CALLBACK	callback;			//!< function to call when transaction happens in the range
	uint32			numTransactions;	//!< number of transactions in this address range
} LAL_ADDR_RANGE;

typedef struct
{
	uint16			numActiveRanges;
	LAL_ADDR_RANGE	range[NUM_ALLOCATABLE_ADDR_RANGES];
} LAL_ADDR_DISPATCH_LIST;

typedef struct
{
	LAL_CALLBACK	callback[64];
} LAL_STREAM_DISPATCH_LIST;


#endif
