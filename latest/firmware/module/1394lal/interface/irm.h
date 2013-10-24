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
 * DESCRIPTION: Isochronous Resource Manager (IRM) 

	This module implements the Isochronous Resource Manager functions

	Revisions:
		created 09/17/2002 lm

****************************************************************************/

/** @addtogroup IRM
	This module implements functions to manage isochronous channels and bandwidth.

	This module can be used from the command prompt by the @ref cli_irm commands.
	@{
*/

#ifndef _IRM_H
#define _IRM_H

#ifdef _IRM

#include "packetBlock.h"
#include "TCTypes.h"
#include "lalDefs.h"
#include "nciIRM.h"

#ifdef _WINMP
#pragma warning( disable : 4514 )
#endif //_WINMP

#define	IRM_MAX_NUMBER_ISOCH_CHANNEL	63
#define	IRM_ANY_AVAIL_ISOCH_CHANNEL		64

enum
{
	IRM_ALLOCATE = 0,
	IRM_FREE
};

/** Allocates the specified channel.  If channel == IRM_ANY_AVAIL_ISOCH_CHANNEL, the next available channel will be allocated.
	@param channel		The address of the location containing the channel to allocate. If channel == IRM_ANY_AVAIL_ISOCH_CHANNEL,
						this location will be updated with the actual channel allocated.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
HRESULT				irmAllocateSpecifiedChannel(uint32 *channel); //channel == IRM_ANY_AVAIL_ISOCH_CHANNEL returns irmAllocateNextAvailableChannel()

/** Allocates the next available channel. 
	@param channel		The address of the location that will be updated with the actual channel allocated.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
HRESULT				irmAllocateNextAvailableChannel(uint32 *channel);

/** Finds an available channel.
	@param channel		The address to place the available channel in.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
HRESULT				irmFindChannelAvailable(uint32 *channel);

HRESULT				irmHandleChannel(uint32 channel, uint32 mode);

/** Allocates the specified channel.
	@param channel		The isochronous channel to allocate.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
_TC_INLINE HRESULT	irmAllocateChannel(uint32 channel) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	irmAllocateChannel(uint32 channel)
								{
									return irmHandleChannel(channel, IRM_ALLOCATE);
								}
/** Frees the specified channel.
	@param channel		The isochronous channel to free.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
_TC_INLINE HRESULT	irmFreeChannel(uint32 channel) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	irmFreeChannel(uint32 channel)
								{
									return irmHandleChannel(channel, IRM_FREE);
								}

HRESULT				irmHandleBandwidth(uint32 bwu, uint32 mode);

/** Allocates the specified number of bandwidth units.
	@param bwu			The number of bandwidth units to allocate.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
_TC_INLINE HRESULT	irmAllocateBandwidth(uint32 bwu) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	irmAllocateBandwidth(uint32 bwu)
								{
									return irmHandleBandwidth(bwu, IRM_ALLOCATE);
								}

/** Frees the specified number of bandwidth units.
	@param bwu			The number of bandwidth units to free.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
_TC_INLINE HRESULT	irmFreeBandwidth(uint32 bwu) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	irmFreeBandwidth(uint32 bwu)
								{
									return irmHandleBandwidth(bwu, IRM_FREE);
								}

/**	Obtain the node ID of the Bus Manager
	@param nodeId		The address to place the node ID of the bus manager node in.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
HRESULT				irmGetBMNodeId(uint32 *nodeId);

HRESULT				irmSetBMNodeId(uint32 nodeId);

/**	Obtain the node ID of the Cycle Master
	@param nodeId		The address to place the node ID of the Cycle Master node in.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
HRESULT				irmGetCMNodeId(uint32 *nodeId);

/**	Obtain the node ID of the Isochronous Resource Manager
	@param nodeId		The address to place the node ID of the Isochronous Resource Manager node in.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
HRESULT				irmGetIRMNodeId(uint32 *nodeId);

/** Initialization of this module.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code"
*/
HRESULT				irmInitialize(void);

//! @}

#endif //_IRM

#endif //_IRM_H
