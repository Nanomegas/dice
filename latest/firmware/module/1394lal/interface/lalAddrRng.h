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
 * DESCRIPTION: Link Abstraction Layer Address Range Callback Mgmt 

	Purpose:	Link Abstraction Layer 
				Address Range Callback Mgmt
	
	Revisions:
		created 04/16/2002 lm
****************************************************************************/

/** @addtogroup LinkAbstractionLayer
	@{
*/

/** @defgroup AddressRangeCallback Address Range Callback Management

	Address Range Callbacks are used when another node issues a request
	to thisNode in an address range of interest to a module. Examples include:

	- CSR Core Architecture
	- Serial Bus Management
		- Bus Manager
		- Isochronous Resource Manager
	- Configuration ROM
	- Connection Management Protocol
	- FCP Command and Response
	- Serial Bus Protocol 2 (SBP-2)
	- Asynchronous Connections
		
	@sa LockHandler

	Registering a callback is done with:

		lalAddressRangeRegisterCB()
	<H2>
	Callbacks should not "block"
	</H2>
		This means that a callback should normally put the packet block
		in a queue and return immediately and should not wait on any
		OS resource.  This way, the packet is not handled in the thread
		of the lower layers.
	<H2>
	Sparse Ranges
	</H2>
		Some protocols may be interested in a sparse set of ranges
		within a larger address range.  For efficiency, the module that
		implements the protocol should allocate the entire range, where
		the range does not overlap with other module's needs of course.
		The callback may return an address error (see below) for
		addresses within the larger range that do not apply.  This will
		cause the transaction layer to return the appropriate
		confirmation to the requester node.
	<H2>
	Callback return codes
	</H2>
		The callback will be executed when a number of different types
		of requests happen in the allocated address space.  These 
		include read, write and lock requests.  The module may handle
		any of these and shall use the enumerated response types defined
		by RCODE_1394 for return values.  For instance, when the
		request type is handled and is successful, RESP_COMPLETE is
		returned.  In the case of a sparsely implemented address range,
		the module may return RESP_ADDRESS_ERROR.  In the case where a
		particular request type is not handled within the range, the
		module should return RESP_TYPE_ERROR, etc.

	@{
*/

#ifndef _LALADDRRNG_H
#define _LALADDRRNG_H

#include "TCTypes.h"
#include "1394.h"
#include "1394defs.h"
#include "packetBlock.h"
#include "lalDefs.h"


/** Initialize this module.
*/
HRESULT		lalAddressRangeInitialize(void);

/** Register a callback for a specified address range.
	@param callback		The callback to register. @sa LAL_CALLBACK
	@param addressLow	The start of the address range. @sa OFFSET_1394
	@param addressHi    The end of the address range. @sa OFFSET_1394
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		lalAddressRangeRegisterCB(	LAL_CALLBACK callback,
										OFFSET_1394	addressLow, 
										OFFSET_1394	addressHi);

/** Remove a callback for a specified address range.
	@param addressLow	The start of the address range. @sa OFFSET_1394
	@param addressHi    The end of the address range. @sa OFFSET_1394
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		lalAddressRangeRemove(		OFFSET_1394 AddrStart,
										OFFSET_1394	AddrEnd);

// Internal functions
HRESULT		lalAddressRangeFindFree(	uint16 *unusedRange);

BOOL		lalAddressRangeLookup(		OFFSET_1394 AddrStart,
										OFFSET_1394	AddrEnd,
										LAL_CALLBACK *callback);
//! @}
//! @}
#endif
