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
 * DESCRIPTION: Link Abstraction Layer

	Purpose:	Link Abstraction Layer
	
	Revisions:
		created 04/09/2002 lm
****************************************************************************/

#ifndef _LAL_H
#define _LAL_H

#include "TCTypes.h"
#include "1394.h"
#include "1394defs.h"
#include "packetBlock.h"
#include "bmDefs.h"
#include "lalDefs.h"
#include "lalAsync.h"
#include "lalAsyncStream.h"
#include "lalAddrRng.h"
#include "lhlbri.h"
#include "bri.h"
#include "nci.h"
#ifdef _BM
#include "nciBM.h"
#endif //_BM
#ifdef _IRM
#include "nciIRM.h"
#endif //_IRM

/** @addtogroup LinkAbstractionLayer 


	This module provides all necessary interfaces for upper layers to access
	nodes on the bus and to handle requests from other nodes in any desired 
	1394 Register Space address range.

	This module can be used from the command prompt by the @ref cli_lhl and @ref cli_lal commands.

	Note: nodeAddr here refers to the devices node address (nodeId | busId)
	which is a dynamic value arbitrated by each node during bus reset.  The
	value is unique for each bus or bridged network.
	
	<H2>
	Initialization
	</H2>
		This module must be initialized once with
		
			lalInitialize()

		After the node has been fully initialized on reset, the function

			lalForceBusReset()

		should be called once to cause any other bus nodes to see this node 
		as a fully operational bus node.  This is handled in target.c
		after the main_entry thread is running and has completed all final
		initializations.
	<H2>
	Bus Reset Callbacks
	</H2>


		Modules that need to be made aware of bus reset activity may install
		callbacks for bus reset indication and bus reset complete events
		using

			lalRegisterBRIndicationCallback()
			lalRegisterBRCompletionCallback()

		Indication callbacks are called once a valid bus reset has been
		detected.

		Complete callbacks are called after all bus reset processing is
		complete, which means the selfId's are verified, maps are completed,
		bus info blocks have been scanned, handles have been remapped to
		the correct node addresses, etc.

		Note that all general packet queueing is halted until bus reset
		complete.  See the section on Queueing below for details.
	<H2>
	Address Range Callbacks
	</H2>

		@sa AddressRangeCallback module

		Address Range Callbacks are used when another node issues a request
		to thisNode in an address range of interest to a module.  This is
		done with

			lalAddressRangeRegisterCB()

		@sa LockHandler

		Callbacks should not "block"
			This means that a callback should normally put the packet block
			in a queue and return immediately and should not wait on any
			OS resource.  This way, the packet is not handled in the thread
			of the lower layers.

		Sparse Ranges
			Some protocols may be interested in a sparse set of ranges
			within a larger address range.  For efficiency, the module that
			implements the protocol should allocate the entire range, where
			the range does not overlap with other module's needs of course.
			The callback may return an address error (see below) for
			addresses within the larger range that do not apply.  This will
			cause the transaction layer to return the appropriate
			confirmation to the requester node.

		Callback return codes
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
	
	<H2>
	Bus Info and Device Info
	</H2>
		@sa InfoAndHandles

		Modules that initiate transactions to other nodes need access to
		information about the current bus topology, such as the number of
		nodes present, the bus generation, etc, and info about each specific
		device.  Modules will generally initiate transactions with other bus
		devices via nodeAddr-independent handles, as described below.
		Handles are created using information discovered using the functions:

			lalGetBusInfo()
				this function provides information such as the current
				number of nodes on the bus, the bus generation (number of
				bus resets seen by thisNode since reset), thisNode's nodeAddr
				and the current bus time value.

				this is one of the first functions a module may wish to use when finding a
				target node.  this funciton returns information about the bus after the
				most recent bus reset.  the lalInfoBlock structure contains a field which 
				shows the number of nodes on the bus, and a field containing the current 
				busId. a module can then iterate thru the found nodes on the bus given 
				this info, using lalGetDeviceInfo, to determine specific info about each
				bus node.

			lalGetDeviceInfo()
				this function provides information about a specific device
				by nodeAddr, such as its WWUID and active state.

				discover information about a bus device, such as WWUID and active state, in 
				order to create a handle for 1394 i/o.  nodeAddr is (busId | nodeID), i.e.
				node 2 has a node address of 0xffc2.
	<h2>
	Handles and Node Addresses
	</h2>

		As mentioned before, devices are generally accessed by handles.  A
		device's nodeAddr may change after a bus reset, whereas its WWUID will
		never change.  A handle allows access to a node regardless of its
		nodeAddr.  Note that not all devices (such as repeaters, or nodes
		that are powered off) support the General ROM Format, which
		specifies a WWUID.  Handles may still be opened to such devices, but
		will be invalidated after each bus reset.  Also note that a handle
		created with a non-compliant device will simply be the nodeAddr
		of the device.  Using a handle for both compliant and non-compliant
		devices will efficiently let modules know when the nodeAddr is no 
		longer useable as a handle.

			lalCreateHandle()
				this function uses a WWUID for handle creation.  WWUID can
				be discovered using lalGetBusInfo and then lalGetDeviceInfo.
				Note that a node that does not respond to bus info read
				requests during bus reset processing will not have a WWUID.
				In this case, the nodeAddress can be used instead of a WWUID
				to create a handle.  This handle will be invalidated after
				a bus reset.

				create a handle to a bus device for read/write/lock access. use lalGetDeviceInfo
				to determine if the desired WWUID is on the bus. handles created using a WWUID
				will persist across bus reset, unless the node has left the bus or stops responding.
				alternately, use 0 for wwuid_hi and the node address for wwuid_lo to create a
				node_id-based handle.  node_id-based handles do not persist across bus reset.

			lalCloseHandle()
				this function decrements the instance count for the handle.
				If the instance count for a handle becomes zero, it is
				invalidated.

			lalGetIDFromHandle()
				this function provides the current nodeAddr that corresponds
				to the handle

			lalGetHandleFromNodeAddr()
				this function provides a handle, if previously created and
				valid, for a nodeAddr
	<h2>
	1394 I/O
	</h2>
		@sa LALAsync module

		These are the main device access APIs for use by upper layers.

		lalRead1394()
			performs a handle-based read of register space in a target
			device.

		lalWrite1394()
			performs a handle-based write of register space in a target
			device.

		lalLock1394()
			performs a handle-based lock of register space in a target
			device.
		

	<h2>
	Async Streams
	</h2>
		lalAsyncStreamSendData()
			sends a packet as an async stream packet to the given channel

		lalAsyncStreamEnable()
			enables reception of async streams

		lalAsyncStreamDisable()
			disables reception of async streams
			
		lalAsyncStreamRegisterChannelCB()
			sets a callback for incoming data on the given stream
	<h2>
	thisNode (a.k.a. "the local node")
	</h2>
		thisNode refers to the node that is currently running this software.
		Some transactions (handle-based or nodeAddr based) are initiated by
		thisNode to thisNode, such as ConfigROM read, IRM and CMP
		transactions.  Not all register space addresses to thisNode are
		supported natively by the lower layers.  If your module needs
		access to other areas, this must be specifically implemented.
	<h2>
	Miscellaneous
	</h2>

		lalGetBusGeneration()
			this function returns the number of bus resets seen since reset.
			This value can be used to compare bus generations of packet
			blocks if desired.

		lalGetBusID()
			this function returns the current busId that thisNode resides
			on.  This will usually be 0xffc0, the "local bus," unless the 
			node is in a bridged environment.

		lalIsThisNodeAddr() 
		lalGetThisNodeAddr() 
		lalGetThisHandle() 
			returns the current nodeAddr, handle of the node this software is running
			on.

		lalGetIRMNodeAddr()
		lalGetIRMHandle()
			returns the current nodeAddr, handle of the Isochronous Resource Manager.

		lalGetCycleTime()
			returns a snapshot of the bus cycle time value at/near the time
			the function was called.

	<h2>
	Queueing
	</h2>
		During bus reset, the lower layers must do some housekeeping in
		order to maintain device handles and allocated bus resources before
		the upper layer messaging can continue.  The lower layers use NORMAL
		and IMMEDIATE queueing to accomplish this.  In almost all cases,
		modules that use this API will use normal queuing.  An example
		exception is a module that implements CMP.  This module must
		reallocate Isochronous resources in a fixed amount of time, and can
		perform this with nodeAddr based transactions and therefore does not
		need to wait for the bus reset to be fully complete to do its tasks.
		These modules can directly call the IMMEDIATE queueing API's in the
		LHL.  In all other cases, use handle based 1394 I/O API's.
	<h2>
	Tools
	</h2>
		A number of CLI tools are provided for excercizing the LAL APIs.
		See the following at the command prompt for more info.

			- hdlGBI - get bus info
			- hdlGNI - get node info based on nodeAddr
			- hdlCreate - create a handle based on WWUID
			- hdlClose - close a handle
			- hdlDump - show all handles
			- read1394 - perform a blocking read
			- write1394 - perform a blocking write
			- lock1394 - perform a blocking lock
	@{
*/

/********************************
** Initialization              **
********************************/
HRESULT				lalInitialize1394Tasks(void);											// called upon power-up, or reset
HRESULT				lalResume1394Tasks(void);												// called upon power-up, or reset
					
HRESULT				lalInitialize1394(void);												// called upon power-up, or reset
HRESULT				lalInitialize1394Finish(void);											// called upon power-up at the end of 1394 initialization forcing a BR
HRESULT				lalInitialize(void);													//!< initialization of this module
void				lalSetWWUID(uint32 WWUIDHigh, uint32 WWUIDLow);
HRESULT				lalGetWWUID(uint32 *WWUIDHigh, uint32 *WWUIDLow);
HRESULT				lalDisable1394(void);

/********************************
** Bus Reset                   **
********************************/

/**	Cause a bus reset, used to announce the node on the bus after boot/inits
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	lalForceBusReset(void) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalForceBusReset(void)
								{
									return lhlBriForceBusReset();
								}

/** Delay sending of self-ID packets to force this node to be root.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	lalForceRoot(void) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalForceRoot(void)
								{
									return lhlBriForceRoot();
								}



_TC_INLINE HRESULT	lalSetCycleMaster(BOOL enabled) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalSetCycleMaster(BOOL enabled)
								{
									return lhlBriSetCycleMaster(enabled);
								}

/** Obtain the current cycle time
	@param pCycleTime	The address of the location to place the current cycle time in.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	lalGetCycleTime(uint32* pCycleTime) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalGetCycleTime(uint32* pCycleTime)
								{
									return lhlBriGetCycleTime(pCycleTime);
								}

/** Obtain the current bus ID
	@param pBusId		The address to place the current busID in.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	lalGetBusID(uint16 *pBusId) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalGetBusID(uint16 *pBusId)
								{
									return briGetBusID(pBusId);
								}

/** @defgroup BusResetInterface Bus Reset Interface

	Modules that need to be made aware of bus reset activity may install
		callbacks for bus reset indication and bus reset complete events
		using

			lalRegisterBRIndicationCallback()
			lalRegisterBRCompletionCallback()

		Indication callbacks are called once a valid bus reset has been
		detected.

		Complete callbacks are called after all bus reset processing is
		complete, which means the selfId's are verified, maps are completed,
		bus info blocks have been scanned, handles have been remapped to
		the correct node addresses, etc.
	@{
*/

/** Returns the current bus generation. 
    This is the number of times the node has seen a bus reset since hardware reset.
	@param pCurBusGen	The memory address to place the current bus generation in.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	lalGetBusGeneration(uint32* pCurBusGen) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalGetBusGeneration(uint32* pCurBusGen)
								{
									return briGetCurrentBusGeneration(pCurBusGen);
								}

/** Registers a callback to be invoked when a bus reset has begun.
	The callee must not depend on being called at any particular point in the reset process.
	@param lalCBProc	The function to call. 
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
_TC_INLINE HRESULT	lalRegisterBRIndicationCallback(lalBusResetCB lalCBProc)	 _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalRegisterBRIndicationCallback(lalBusResetCB lalCBProc)	
								{
									return briRegisterBRIndicationCallback((BRI_CALLBACK) lalCBProc);
								}
/** Registers a callback to be invoked when a bus reset has completed.
	@param lalCBProc		The function to call.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
_TC_INLINE HRESULT	lalRegisterBRCompletionCallback(lalBusResetCB lalCBProc)	 _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalRegisterBRCompletionCallback(lalBusResetCB lalCBProc)	
								{
									return briRegisterBRCompletionCallback((BRI_CALLBACK) lalCBProc);
								}
/** Wait for a bus reset indication.
*/
_TC_INLINE HRESULT	lalWaitOnResetIndication (BOOL *bResetDetected)				 _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalWaitOnResetIndication (BOOL *bResetDetected)				
								{
									return briWaitOnResetIndication (bResetDetected);
								}
/** Wait until bus reset processing has completed. If a bus reset is in progress, this function suspends the 
	caller until the reset has been completely handled.  If no reset is in progress, this
	function returns immediately.
	@param bResetDetected	The memory address to indicate if a bus reset was in progress.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
_TC_INLINE HRESULT	lalWaitOnResetCompletion (BOOL *bResetDetected)				 _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalWaitOnResetCompletion (BOOL *bResetDetected)				
								{
									return briWaitOnResetCompletion (bResetDetected);
								}

_TC_INLINE HRESULT	lalWaitOnResetPreCompletion (BOOL *bResetDetected) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalWaitOnResetPreCompletion (BOOL *bResetDetected)			//!< wait for a bus reset pre completion (before BM and handles - don't use handles at this stage)
								{
									return briWaitOnResetPreCompletion (bResetDetected);
								}

_TC_INLINE HRESULT	lalElapsedTimeSinceResetIndication(uint32 *elapsedTime) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalElapsedTimeSinceResetIndication(uint32 *elapsedTime)		//!< time elapsed since last bus reset indication
								{
									return briElapsedTimeSinceResetIndication(elapsedTime);
								}

_TC_INLINE HRESULT	lalElapsedTimeSinceResetCompletion(uint32 *elapsedTime) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalElapsedTimeSinceResetCompletion(uint32 *elapsedTime)		//!< time elapsed since last bus reset completion
								{
									return briElapsedTimeSinceResetCompletion(elapsedTime);
								}

_TC_INLINE HRESULT	lalElapsedTimeSinceResetPreCompletion(uint32 *elapsedTime) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalElapsedTimeSinceResetPreCompletion(uint32 *elapsedTime)		//!< time elapsed since last bus reset pre completion
								{
									return briElapsedTimeSinceResetPreCompletion(elapsedTime);
								}

_TC_INLINE HRESULT	lalWaitTimeSinceResetIndication(uint32 msec) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalWaitTimeSinceResetIndication(uint32 msec)				//!< wait until msec has elapsed since the bus reset indication, or since another reset indications occurs
								{
									return briWaitTimeSinceResetIndication(msec);
								}

_TC_INLINE HRESULT	lalWaitTimeSinceResetCompletion(uint32 msec) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalWaitTimeSinceResetCompletion(uint32 msec)				//!< wait until msec has elapsed since the bus reset completion, or since another reset completions occurs
								{
									return briWaitTimeSinceResetCompletion(msec);
								}
	
_TC_INLINE HRESULT	lalWaitTimeSinceResetPreCompletion(uint32 msec) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalWaitTimeSinceResetPreCompletion(uint32 msec)				//!< wait until msec has elapsed since the bus reset pre completion, or since another reset completions occurs
								{
									return briWaitTimeSinceResetPreCompletion(msec);
								}
//! @}

/********************************
** Device Info and Handles     **
********************************/

/** @defgroup InfoAndHandles Device Information and Handles

		Devices are generally accessed by handles.  A
		device's nodeAddr may change after a bus reset, whereas its WWUID will
		never change.  A handle allows access to a node regardless of its
		nodeAddr.  Note that not all devices (such as repeaters, or nodes
		that are powered off) support the General ROM Format, which
		specifies a WWUID.  Handles may still be opened to such devices, but
		will be invalidated after each bus reset.  Also note that a handle
		created with a non-compliant device will simply be the nodeAddr
		of the device.  Using a handle for both compliant and non-compliant
		devices will efficiently let modules know when the nodeAddr is no 
		longer useable as a handle.
	@{
*/


/** Obtains the bus information
	@param plalInfoBlock	The address to place the bus information.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code"
*/

_TC_INLINE HRESULT	lalGetBusInfo(LAL_BUS_RESET_INFO* plalInfoBlock) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalGetBusInfo(LAL_BUS_RESET_INFO* plalInfoBlock)
								{
									return nciGetBusInfo(plalInfoBlock);
								}

/** Obtains device information
	@param nodeAddr			The node address of the device.
	@param plalDevInfo		The address to place the device information.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
_TC_INLINE HRESULT	lalGetDeviceInfo(uint32 nodeAddr, LAL_DEVICE_INFO* plalDevInfo) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalGetDeviceInfo(uint32 nodeAddr, LAL_DEVICE_INFO* plalDevInfo)
								{
									return nciGetNodeInfo(nodeAddr, plalDevInfo);
								}

/** Creates a handle for a node
	@param wwuid_hi			The HI bits of the WWUID
	@param wwuid_lo			The LO bits of the WWUID
	@param pHandle			The memory address to place the handle in.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/	
_TC_INLINE HRESULT	lalCreateHandle(uint32 wwuid_hi, uint32  wwuid_lo, LAL_NODE_HANDLE* pHandle) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalCreateHandle(uint32 wwuid_hi, uint32  wwuid_lo, LAL_NODE_HANDLE* pHandle)
								{
									return nciCreateHandle(wwuid_hi, wwuid_lo, pHandle);
								}
/** Closes a device handle
	@param handle			The handle to the device.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	lalCloseHandle(LAL_NODE_HANDLE handle) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalCloseHandle(LAL_NODE_HANDLE handle)
								{
									return nciCloseHandle(handle);
								}
/** Obtain a node's address from it handle
	@param handle			The handle to the device
	@param pNodeAddr		The address to place the node address in.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	lalGetNodeAddrFromHandle(LAL_NODE_HANDLE handle, uint32* pNodeAddr) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalGetNodeAddrFromHandle(LAL_NODE_HANDLE handle, uint32* pNodeAddr)
								{
									return nciGetAddrFromHandle(handle, pNodeAddr);
								}
/** Obtain a node's handle from its node address
	@param nodeAddr			The node address for the device
	@param pHandle			The address to place the handle in
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT				lalGetHandleFromNodeAddr(uint32 nodeAddr, LAL_NODE_HANDLE* pHandle);

/** Obtain a handle from an incoming packet block
	@param packetBlock		A received packet block.
	@param pHandle			The address to place the handle in
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT				lalGetHandleFromPacketSrcNodeAddr(PB* packetBlock, LAL_NODE_HANDLE* pHandle);

/** Obtain a device's WWUID from its node address
	@param nodeAddr			The node address of the device
	@param wwuid			The address to place the wwuid in.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT				lalGetWWUIDFromNodeAddr(uint32 nodeAddr, WWUID *wwuid);

/** Obtain a device's WWUID from its handle
	@param handle			The handle of the device
	@param wwuid			The address to place the wwuid in.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT				lalGetWWUIDFromHandle(LAL_NODE_HANDLE handle, WWUID *wwuid);
				
HRESULT				lalGetNodeAddrFromWWUIDSpecial(WWUID wwuid, uint32* pNodeAddr, BOOL bMatchWWUIDHi, BOOL bMatchWWUIDLo, BOOL bAllowLocalNode);

HRESULT				lalGetHandleFromWWUIDSpecial(WWUID wwuid, LAL_NODE_HANDLE* pHandle, BOOL bMatchWWUIDHi, BOOL bMatchWWUIDLo, BOOL bAllowLocalNode);

/** Obtain a devices node address from its wwuid.
	@param wwuid			The wwuid for the device
	@param pNodeAddr		The address to place the node address in.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	lalGetNodeAddrFromWWUID(WWUID wwuid, uint32* pNodeAddr) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalGetNodeAddrFromWWUID(WWUID wwuid, uint32* pNodeAddr)
								{
									return lalGetNodeAddrFromWWUIDSpecial(wwuid, pNodeAddr, TRUE, TRUE, TRUE);
								}

/** Obtain a device's handle from its wwuid.
	@param wwuid			The wwuid for the device.
	@param pHandle			The address to place the handle in.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	lalGetHandleFromWWUID(WWUID wwuid, LAL_NODE_HANDLE* pHandle) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalGetHandleFromWWUID(WWUID wwuid, LAL_NODE_HANDLE* pHandle)
								{
									return lalGetHandleFromWWUIDSpecial(wwuid, pHandle, TRUE, TRUE, TRUE);
								}

HRESULT				lalGetMaxRecordSizeBytes(uint32 nodeAddr, uint16* maxSizeInBytes);

/** Obtain the maximum payload size that can be used.
	@return					The maximum number of bytes that a payload can contain.
*/
_TC_INLINE uint32		lalGetMaxTxPayloadSizeBytes(void) _TC_INLINE_ALWAYS;
_TC_INLINE uint32		lalGetMaxTxPayloadSizeBytes(void)
								{
									return lhlGetMaxTxPayloadSizeBytes();
								}

/**************
** Utilities **
**************/

/** Determine if a node address belongs to this node
	@param nodeAddr			The node address to check.
	@return					TRUE if the node address is this node's address.
*/
_TC_INLINE BOOL		lalIsThisNodeAddr(uint32 nodeAddr) _TC_INLINE_ALWAYS;
_TC_INLINE BOOL		lalIsThisNodeAddr(uint32 nodeAddr)
								{
									return briIsThisNodeAddr(nodeAddr);
								}
/** Determine if a node address is the broadcast node address
	@param nodeAddr			The node address to check.
	@return					TRUE if the node addres is the broadcast address.
*/
_TC_INLINE BOOL		lalIsBroadcastNodeAddr(uint32 nodeAddr) _TC_INLINE_ALWAYS;
_TC_INLINE BOOL		lalIsBroadcastNodeAddr(uint32 nodeAddr)
								{
									return briIsBroadcastNodeAddr(nodeAddr);
								}
/**	Obtain the address of this node
	@param pNodeAddr		The address to place this node's address in.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	lalGetThisNodeAddr(uint32* pNodeAddr) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalGetThisNodeAddr(uint32* pNodeAddr)
								{
									return briGetThisNodeAddr(pNodeAddr);
								}
/** Obtain the handle to this node
	@param pHandle			The address to place the handle in.
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT				lalGetThisHandle(LAL_NODE_HANDLE* pHandle);

/** Obtain the number of nodes on the bus.
	@return					The number of nodes.
*/
_TC_INLINE uint32		lalGetNumNodes(void) _TC_INLINE_ALWAYS;
_TC_INLINE uint32		lalGetNumNodes(void)
								{
									return briGetNumNodes();
								}
//! @}

#ifdef _BM
_TC_INLINE HRESULT	lalGetNodeAddrBM(uint32* pNodeAddr) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalGetNodeAddrBM(uint32* pNodeAddr)
								{
									return nciBMGetNodeAddrBM(pNodeAddr);
								}

HRESULT				lalGetHandleBM(LAL_NODE_HANDLE* pHandle);

_TC_INLINE BOOL		lalIsThisNodeBM(void) _TC_INLINE_ALWAYS;
_TC_INLINE BOOL		lalIsThisNodeBM(void)
								{
									return nciBMIsThisNodeBM();
								}

HRESULT				lalGetHandleSpeed(LAL_NODE_HANDLE handle, LAL_SPEED *speed);

_TC_INLINE HRESULT	lalGetNodeAddrSpeed(uint32 nodeAddr, LAL_SPEED *speed) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalGetNodeAddrSpeed(uint32 nodeAddr, LAL_SPEED *speed)
								{
									return nciBMGetNodeSpeed(nodeAddr & MASK_NODEID, speed);
								}
#endif //_BM

#ifdef _IRM

/** Obtain the node address of the IRM
	@param pNodeAddr		The address to place the node address in of the IRM
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/

_TC_INLINE HRESULT	lalGetNodeAddrIRM(uint32* pNodeAddr) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalGetNodeAddrIRM(uint32* pNodeAddr)
								{
									return nciIRMGetNodeAddrIRM(pNodeAddr);
								}
/** Obtain the handle for the IRM
	@param pHandle			The address to place the handle for the IRM
	@return					NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT				lalGetHandleIRM(LAL_NODE_HANDLE* pHandle);

_TC_INLINE BOOL		lalIsThisNodeIRM(void) _TC_INLINE_ALWAYS;
_TC_INLINE BOOL		lalIsThisNodeIRM(void)
								{
									return(nciIRMIsThisNodeIRM());
								}
#endif //_IRM

_TC_INLINE HRESULT	lalGetNodeAddrRoot(uint32* pNodeAddr) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	lalGetNodeAddrRoot(uint32* pNodeAddr)
								{
									return briGetNodeAddrRoot(pNodeAddr);
								}

HRESULT				lalGetHandleRoot(LAL_NODE_HANDLE* pHandle);

_TC_INLINE BOOL		lalIsThisNodeRoot(void) _TC_INLINE_ALWAYS;
_TC_INLINE BOOL		lalIsThisNodeRoot(void)
								{
									return(briIsThisNodeRoot());
								}

_TC_INLINE void		lalEnQPacketBlock(PB *packetBlock) _TC_INLINE_ALWAYS;
_TC_INLINE void		lalEnQPacketBlock(PB *packetBlock)
								{
									lhlMsgQueueEnQPacketBlock(LHL_QUEUE_GENERAL, packetBlock);
								}

_TC_INLINE void		lalEnQPriorityPacketBlock(PB *packetBlock) _TC_INLINE_ALWAYS;
_TC_INLINE void		lalEnQPriorityPacketBlock(PB *packetBlock)
								{
									lhlMsgQueueEnQPacketBlock(LHL_QUEUE_PRIORITY, packetBlock);
								}
//! @}

#endif //_LAL_H
