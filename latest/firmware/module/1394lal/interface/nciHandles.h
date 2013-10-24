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
  * DESCRIPTION: Node Controller Interface 

	Purpose: Node Controller Interface functions for handling Handles

	Revisions:
		created 04/30/2002 lm
****************************************************************************/

#ifndef _NCIHANDLES_H
#define _NCIHANDLES_H

#include "TCTypes.h"
#include "1394.h"
#include "1394defs.h"
#include "ErrorCodes.h"
#include "packetBlock.h"
#include "bmdefs.h"
#include "lhl.h"

#define	NCI_MAX_NODE_HANDLES				MAX_NODES_PER_BUS

#define NCI_HANDLE_STATUS_UNUSED			0
#define NCI_HANDLE_STATUS_WWUID				1
#define NCI_HANDLE_STATUS_NODE				2

typedef struct
{
	BOOL			bActive;		// node active
	uint32			status;			// node handle status
	uint16			numInst;		// number of instances of this handle
	WWUID			wwuid;			// wwuid that corresponds to this handle
	NODEHANDLE		handle;			// unique, persisent value for all handle-based transactions
	uint16			nodeAddr;		// corresponding nodeAddress
	LAL_SPEED		speed;			// signaling speed to use for packet tx
	PB_QTYPE		qType;			// transmit queue to use
} NCI_HANDLE_STRUCT;

typedef struct
{
	BOOL				bValid;			// valid handle map
	uint32				generation;		// bus generation for which this info is valid
	uint16				count;			// number of unique handles
	NCI_HANDLE_STRUCT	handleRec[NCI_MAX_NODE_HANDLES];
} NCI_HANDLE_TABLE;

extern NCI_HANDLE_TABLE		nciHandles;

/*****************************************************************************

	nciHandlesInitialize

	initializes this module

******************************************************************************/

HRESULT	nciHandlesInitialize(void);

/*************************************************************

	nciFixupNodeHandles

*****************************

	Description:
		remap node handles to nodeAddr after bus reset.  requires a valid bus info
		block cache. when a handle is created for a node that has a wwuid, it will
		persist across bus resets. Handles created for a devices that do not have 
		a wwuid are invalidated on bus reset and will have to be recreated.
	Return:
		NO_ERROR indicates success.  Any other values indicate that the function
		could not complete handle fixups.

	External services used:
		sysLogError

****************************/

HRESULT nciFixupNodeHandles(void);

/*************************************************************

	nciCreateHandle

*****************************

	Description:
		provide a handle to a bus node from its WWUID that persists across bus reset.
		to create a handle t a non-compliant device (no WWUID) use 0 for wwuid_hi and
		the node address (busId | NodeId) for wwuid_lo.  These handles are invalidated
		when a bus reset occurs.
	Return:
		NO_ERROR indicates success.  Any other values indicate that the function
		could not create the handle.

	External services used:
		sysLogError

****************************/

HRESULT nciCreateHandle(uint32 wwuid_hi, uint32  wwuid_lo, LAL_NODE_HANDLE* pHandle);

/*************************************************************

	nciCloseHandle

*****************************

	Description:
		given a handle, decrement its instance count in the handle table.  if
		the instance count becomes zero, invalidate the handle.
	Return:
		NO_ERROR indicates success.  Any other values indicate an error.

	External services used:
		sysLogError

****************************/

HRESULT nciCloseHandle(LAL_NODE_HANDLE hHandle);


/*************************************************************

	nciGetUniqueHandleID

*****************************

	Description:
		obtain a handle value that is not currenly in use.
	Return:
		NO_ERROR indicates success.  Any other values indicate an error.

	External services used:
		sysLogError

****************************/

HRESULT nciGetUniqueHandleID(LAL_NODE_HANDLE* hHandle);

/*************************************************************

	nciIsValidHandle


*****************************

	Description:
		verify a handle
	Return:
		TRUE indicates the handle is valid, FALSE otherwise.

	External services used:

****************************/

BOOL nciIsValidHandle(LAL_NODE_HANDLE handle);


/*************************************************************

	nciGetHandleFromNodeAddr

*****************************

	Description:
		given a nodeAddr, provide a handle to the node if previously created.
	Return:
		NO_ERROR indicates success.  Any other values indicate an error.

	External services used:
		sysLogError

****************************/

HRESULT nciGetHandleFromNodeAddr(uint32 nodeAddr, LAL_NODE_HANDLE* pHandle);


/*************************************************************

	nciGetAddrFromHandle

*****************************

	Description:
		given a handle, provide the corresponding nodeAddr.
	Return:
		NO_ERROR indicates success.  Any other values indicate an error.

	External services used:
		sysLogError

****************************/

HRESULT nciGetAddrFromHandle(LAL_NODE_HANDLE hHandle, uint32* pNodeAddr);


/*************************************************************

	nciSetSpeed

*****************************

	Description:
		given a handle, set the signaling speed to use for transactions.
	Return:
		NO_ERROR indicates success.  Any other values indicate an error.

	External services used:
		sysLogError

****************************/

HRESULT nciSetSpeed(LAL_NODE_HANDLE hHandle, LAL_SPEED speed);


/*************************************************************

	nciGetSpeed

*****************************

	Description:
		given a handle, get the signaling speed used for transactions.
	Return:
		NO_ERROR indicates success.  Any other values indicate an error.

	External services used:
		sysLogError

****************************/

HRESULT nciGetSpeed(LAL_NODE_HANDLE hHandle, LAL_SPEED* speed);

/*************************************************************

	nciSetQueuePriority

*****************************

	Description:
		given a handle, set the queueing priority to use for transactions.
	Return:
		NO_ERROR indicates success.  Any other values indicate an error.

	External services used:
		sysLogError

****************************/

HRESULT nciSetQueuePriority(LAL_NODE_HANDLE hHandle, PB_QTYPE qType);

/*************************************************************

	nciGetQueuePriority

*****************************

	Description:
		given a handle, get the queueing priority used for transactions.
	Return:
		NO_ERROR indicates success.  Any other values indicate an error.

	External services used:
		sysLogError

****************************/

HRESULT nciGetQueuePriority(LAL_NODE_HANDLE hHandle, PB_QTYPE* qType);

#endif
