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
 * DESCRIPTION: Link Abstraction Layer Interfaces Async Streaming

	Purpose:	Link Abstraction Layer 
				Async Streaming (Loose Isoch)
	
	Revisions:
		created 04/22/2002 lm
****************************************************************************/

/** @addtogroup LinkAbstractionLayer
	@{
*/

/** @defgroup LALAsyncStream Asynchronous Stream Interface

	This modules provides the interface for asychronous streams.  Asynchronous streams are
	isoc packets that are sent asynchronously (and are subject to asynchronous arbitration).

	Typically, async streams are used as a multi-cast method.  An example protocol that uses 
	asynchronous streams is IP over 1394.

	@{
*/
	

#ifndef _LALASYNCSTREAM_H
#define _LALASYNCSTREAM_H

#include "TCTypes.h"
#include "1394.h"
#include "1394defs.h"
#include "packetBlock.h"
#include "lalDefs.h"


#ifdef _LOOSE_ISO
/* Async stream control */

//! Enable/disable all channels
#define ASYNC_STREAM_ALL_CH			64

/** Initialize the hardware to support asynchronous streams.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT	lalAsyncStreamInitialize(void);

/** Enable async streams.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT lalAsyncStreamEnable(void);

/** Disable async streams.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT lalAsyncStreamDisable(void);

/** Check if async streams are enabled/
	@return				TRUE is async streams are enabled.
*/
BOOL	lalAsyncStreamIsEnabled(void);

/** Enable a particular async stream channel.  Typically channel 31 is used.
	@param channel		The channel to enable for async streams.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT lalAsyncStreamEnableChannel(uint8 channel);

/** Disable a particular async stream channel.
	@param channel		The channel to disable for async streams.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT lalAsyncStreamDisableChannel(uint8 channel);

/** Register a callback to receive packets on an asynchrous stream channel.
	@param channel		The channel to receive on.
	@param callback		The callback function to be called.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT lalAsyncStreamRegisterChannelCB(uint8 channel, LAL_CALLBACK callback);

/* RL: This function is not declared anywhere - perhaps remove? */
HRESULT lalAsyncStreamChannelCB(uint8 channel, 
								uint8 tag, 
								uint8 sy,
								uint16 byteLength,
								uint32* bufPtr);

BOOL	lalAsyncStreamLookupChannelCB(uint8 channel, LAL_CALLBACK *callback);

/** Send data on an asynchronous stream.
	@param channel		The channel to send on.
	@param tag			The tag field to use (packet header).
	@param sy			The sy field to use (packet header).
	@param bytelength	The number of bytes to send.
	@param bufPtr		The address of the data to send.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT lalAsyncStreamSendData(uint8 channel, uint8 tag, uint8 sy, uint16 bytelength, uint32* bufPtr);

//! @}
//! @}
#endif //_LOOSE_ISO
#endif
