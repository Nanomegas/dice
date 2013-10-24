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

 DESCRIPTION:

	AV/C command/response handler

	Revisions:
	created 21/03/2003 KBJ/LM

****************************************************************************/

/** @addtogroup AVC 

	Interfaces the AVC layer to the FCP layer and provides an interface for subunits to use.

	This module can be used from the command prompt by the @ref cli_avc commands.
	
	Receive packets:
		Handler dispatches the packet via callbacks registered for subunit type and id.
		If no callback is registered for the packet's subunit type and ID, this handler
		sends the 'not implemented' response.

	Transmit packets:
		Handler sends the packet to the appropriate FCP destination address, depending
		on whether the packet is a COMMAND or RESPONSE packet.

	The file /1394AVC/avcDefs.h contains most AV/C definitions.

	<h2>
	Subunit Callbacks
	</h2>
	AV/C subunits must use the @ref CallbackRegistry to register with this AV/C module using 
	the function crAddCallback()

	@sa CallbackRegistry

	The following are example registry descriptors which describe
	an AV/C Subunit Callback.

	@code
	CALLBACK_REGISTRY	AVC_CD_PLAYER_CALLBACK[]=
	{
		{CALLBACK_DESCRIPTOR_TYPE,AVC_SUBUNIT_TYPE},
		{CALLBACK_DESCRIPTOR_NEXT,0},
		{AVC_SUBUNIT_CALLBACK_KEY,(uint32)xxx}, // CALLBACK PTR of TYPE AVC_SUBUNIT_CALLBACK},
		{AVC_SUBUNIT_SUBUNITTYPE_KEY,AVC_SU_TYPE_AV_DISC},
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};
	@endcode


	This example describes a callback for AVC_SECURITY functions.

	@code
	CALLBACK_REGISTRY	AVC_AKE_SECURITY_CALLBACK[]=
	{
		{CALLBACK_DESCRIPTOR_TYPE,AVC_SECURITY_TYPE},
		{CALLBACK_DESCRIPTOR_NEXT,0},
		{AVC_SECURITY_CALLBACK_KEY,(uint32)xxx}, // CALLBACK PTR of TYPE AVC_SECURITY_CALLBACK},
		{AVC_SECURITY_CATEGORY_KEY,AKE_TYPE_5C},
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};
	@endcode

	The AV/C receive packet callback is called when the handler receives an av/c packet for the 
	specified subunit type and id.

	Callback functions must have the following format:

	@code
	HRESULT avcCallback(AVC_HEADER *pHeader, PB *packetBlock)
	@endcode

		Parameters:
				pHeader		Structure containing the avc header information extracted
							from the packet block.
				packetBlock	Pointer to the packet block.  Inside the packet block is the 
							AVC datastream which has been opened and its current context
							is that returned by the DecodeAVCPayloadHeader function. 	
				
				Note that header info is passed by value rather than by reference.  This is 
				to avoid persistence problems when the subunit process runs as its own thread.
							 
		The callback function can return the following values:

				NO_ERROR	
							Command is supported and response has/will be sent to controller.
							Subunit will handle memory deallocation of packet block.
	  						For all other return values, the subunit must not alter the packet block.

				E_PKT_AVC_ACCEPTED
							Packet was accepted and requested action is expected to complete without failure.
							The avc handler will send an ACCEPTED response frame and free the packet block.
							For STATUS ctypes, the callback function may first perform the necessary writes
							on the packet block payload before returning.

				E_PKT_AVC_INTERIM
							Receiving subunit cannot determine response at this time.  Receiving subunit will
							ensure that a final response is sent.  Note the difference between this and SUBUNIT_BUSY.
							AVC handler will send an INTERIM response.

				E_PKT_AVC_IMPLEMENTED 	
							Inquiry CTYPE only: opcode is supported.  This AVC command handler will
							send the appropriate response and be responsible for freeing the packet block.

				E_PKT_AVC_SUBUNIT_BUSY	
							Subunit could not handle the packet.  The buck has been passed back to the
							avc handler, both to respond (or not) and to free the packetBlock.
							A subunit implemented as a seprate thread could return this value if the
							thread's queue is full.

							The AV/C command handler will take the appropriate action.
							There are various actions the avc handler could take:
								- send an interim response and keep trying the callback
								- ignore the command and dump the packet block.
								- send a 'rejected' response
								
				E_PKT_AVC_NOT_IMPLEMENTED
							The command is not implemented.  Command handler returns the appropriate
							response and will be responsible for freeing the packetBlock.

							Note that the subunit may directly return the AVC response of NOT_IMPLEMENTED
							to the controller, and the callback functions would return with NO_ERROR.
		
				E_PKT_AVC_REJECTED
							The command is implemented, but subunit state prevents it from being executed
							at this time.  Command handler will send a 'rejected' response and be 
							responsible for freeing the packetBlock.

							Note that the subunit may directly return the AVC response of REJECTED
							to the controller, and the callback functions would return with NO_ERROR.

		It is the responsibility of callback functions to ensure that either the function returns
		with a non-NO_ERROR value, or a response is sent to the requesting node, within 100ms.

		PacketBlock disposal, to summarize:
				if the subunit callback returns NO_ERROR, the subunit is responsible for disposing
					the packetBlock.
				if the subunit callback returns anything else, the handler is responsible for disposing the
					packetBlock.

	<h2>
	Subunit algorithms
	</h2>
	
	The following suggests algorithms for implementing av/c unit and subunit module which 
	use this handler.

	
			AV/C Command type support in subunit implementations:

				All subunits MUST support SPECIFIC_INQUIRY and GENERAL_INQUIRY.
				CONTROL, STATUS, and NOTIFY are supported as per subunit requirements.

			A suggested algorithm for a subunit callback function is described below 
	@code
	HRESULT subunitCallback(..........)
	{
		switch (ctype)
		{
			case (ctype == AVC_CONTROL) :
				{	parse data
					if command implemented
					{
						avcSendRequest(AVC_ACCEPTED,packetBlock)
						send command to host
						return NO_ERROR;
					}
					else
					{
						avcSendRequest(AVC_NOT_IMPLEMENTED,packetBlock)
						return NO_ERROR;
					}
				}
			case (ctype == AVC_GENERAL_INQUIRY)  ........							
												
			case (ctype == AVC_SPECIFIC_INQUIRY) ........

			...

			default: return FW_PKT_AVC_NOT_IMPLEMENTED;
		}
		etc
		@code
		
			A suggested rough algorith for a threaded subunit implementation callback is described below

			subunitQ is the queue that the subunit receives on.
		@code

		HRESULT subunitCallback(.....)
		{
			HRESULT hResult;

			hResult = ( put packetBlock into thread queue )
			if (hResult != NO_ERROR)
			{
				if (hResult == TX_QUEUE_FULL) 
				{
					return FW_PKT_AVC_SUBUNIT_BUSY;
				}
				sysLogError( problem with queue )
			}
			return NO_ERROR;
		}
		@endcode

		A suggested algorithm for the subunit thread is described below:

	@code
		HRESULT subunitThreadEntryPoint()
		{
			.
			.
			( get packet from the queue )
			switch (ctype)
			{
				case (ctype == AVC_CONTROL) :
					{	parse data
						if command implemented
						{
							send command to host
							avcSendRequest(AVC_ACCEPTED,packetBlock)
						}
						else
						{
							avcSendRequest(AVC_NOT_IMPLEMENTED,packetBlock)
						}
					}
				case (ctype == AVC_GENERAL_INQUIRY)  ........							
												
				case (ctype == AVC_SPECIFIC_INQUIRY) ........

				default: {
						avcSendRequest(AVC_NOT_IMPLEMENTED,packetBlock)
						 }	
				}
				etc
		@endcode

			Note: In above algorithms, the packet block must be released if any of the avcSendRequest function
			calls fail.  If re-using the command packet for the response, you must also put your
			own callback into the 'packet owner callback' field of the packetBlock.

			<h2>
			Subunit Packet Parsing
			</h2>
			Subunit implementations can use the @ref DataStream utility to parse received packets.  This @ref DataStream
			nodule allows for tightly packed fields to be easily extracted.

			The data stream in a received packet block can be obtained by calling:

			pbGetApplicationDatastream()

			@sa PacketBlocks
			@sa DataStreams
			@sa CallbackRegistry

			@{
		
*/

#ifndef _AVC_H
#define _AVC_H

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "packetBlock.h"
#include "datastream.h"
#include "avcUtils.h"
#include "lal.h"

/**		Sends a packet to the appropriate destination FCP register, depending on command type.
		Will call tranport layer to send the FCP frame.

		Command codes are sent to the FCP_COMMAND_REGISTER
		Response codes are sent to the FCP_RESPONSE_REGISTER

		Before calling avcSendRequest, the caller must:
			- fill out the avc header portion of the packet itself - specifically, the subunit
				type and Id fields (with extensions if any).  Note that this will already be done
				if you re-use the command packet for the response
			- fill out the ownerCallback field.  This is done for you if you create a new packet.
				If you are re-using a command packet for the response, you need to explicitly
				fill in the callback.

		@param	ctype			One of the AV/C command or response values (see avcDefs.h)
		@param	packetBlock 	Pointer to the packet block

		@return		NO_ERROR if successful, IE packet has been given to the FCP layer.  When packet has
 					finished moving through the layers, the ownerCallback is called - this would usually
 					be the one that called this avcSendRequest.

*/
HRESULT		avcSendRequest(uint32 ctype, PB *packetBlock);


/** This is similar to avsSendRequest() with the exception that the response code is written into the packet
    block before the response is sent.  Refer to avsSendRequest()
	@param	response		One of the AV/C response values (see avcDefs.h)
	@param	packetBlock 	Pointer to the packet block

	@return		NO_ERROR if successful, IE packet has been given to the FCP layer.  When packet has
 				finished moving through the layers, the ownerCallback is called - this would usually
 				be the one that called this avcSendRequest.
*/
HRESULT		avcSendResponsePacket(uint32 response, PB *packetBlock);

/** Creates a response packet to send.  The source and destination addresses are swapped.  This is commonly used
	by AVC_CTYPE_STATUS responses.
	@param packetBlock		The original packet block
	@param respPacket		Address of pointer that will be set to new response packet block.
	@param bNewPayloadSize	TRUE if the size of the payload is to be changed.
	@param payloadSize		The size of the new payload (if bNewPayloadSize == TRUE)
	@return					NO_ERROR (0) if successful.
*/
HRESULT		avcCreateResponsePacket(PB *packetBlock, PB **respPacket, BOOL bNewPayloadSize, uint32 payloadSize);

void		avcDestroyResponsePacket(PB *packetBlock);

/* These functions are mainly used for NOTIFY responses */
HRESULT		avcReplyResponse(uint32 response, PB* packetBlock);
HRESULT		avcBlockingSendRequest(NODEHANDLE hHandle, uint8 ctype, uint16 byteLength, uint32 *bufPtr);

// RL: This seems to be only called by avcSendResponsePacket
HRESULT		avcSendResponse(uint32 response, PB *packetBlock);

HRESULT		avcCallDescriptorHandler(AVC_HEADER *pHeader, PB* packetBlock);
HRESULT		avcHandleCallback(AVC_HEADER *pHeader, PB *packetBlock);
HRESULT		avcHandlePacketBlock(PB* packetBlock);
HRESULT		avcCallback(PB *packetBlock);
void		avcHandlerThread(void *dummy);

HRESULT		avcPadToNextQuadletInStream(pDataStream pStream);
HRESULT		avcPadToNextQuadlet(PB *packetBlock);

HRESULT		avcCreateMessageQueues(void);

HRESULT		avcResumeTasks(void);									// Resume tasks for this module
HRESULT		avcSuspendTasks(void);									// Suspend tasks for this module
HRESULT		avcInitializeTasks(void);								// Create tasks for this module
HRESULT		avcInitialize(void);									// Initialization of this module

#endif	//_AVC_H

