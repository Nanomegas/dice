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

/** @addtogroup FCP
	@{

	The Function Control Protocol (FCP) provides a simple command/response transport
	mechanism.

	This protocol is defined in IEC 61883-1.

	A node that wishes to initiate an action (the controller) in another node (the target), 
	performs an asynchronous write addressed to the FCP_COMMAND_REGISTER in the target.

	The target responds to the command by sending an asynchronous write to the FCP_RESPONSE_REGISTER
	in the controller.

	This protocol is used mainly by AV/C.

	Can also be used for HAVi messaging, and CAL.   
*/



#ifndef _FCP_H
#define _FCP_H

#include "1394.h"
#include "packetBlock.h"
#include "TCTypes.h"
#include "lal.h"

#ifdef _WINMP
#pragma warning( disable : 4514 )
#endif //_WINMP

// oMPR and iMPR data rate capability encoding

#define FCP_DATA_RATE_S100			0
#define FCP_DATA_RATE_S200			1
#define FCP_DATA_RATE_S400			2

// cts
#define FCP_CTS_CODE_AVC			0
#define FCP_CTS_CODE_CAL			1
#define FCP_CTS_CODE_EHS			2
#define FCP_CTS_CODE_HAVI			3

#define FCP_MAX_PACKET_SIZE			512
// Define our initialization 

#define	FCP_OFFSET_COMMAND			{FCP_COMMAND_BASE_ADDR, REGISTER_SPACE_ADDR_HI}
#define	FCP_OFFSET_COMMAND_END		{FCP_COMMAND_BASE_ADDR + FCP_MAX_PACKET_SIZE, REGISTER_SPACE_ADDR_HI}
#define	FCP_OFFSET_RESPONSE			{FCP_RESPONSE_BASE_ADDR, REGISTER_SPACE_ADDR_HI}
#define	FCP_OFFSET_RESPONSE_END		{FCP_RESPONSE_BASE_ADDR + FCP_MAX_PACKET_SIZE, REGISTER_SPACE_ADDR_HI}

#define MAX_FCP_CBS 1   // change this to support other FCP using protocols



/** FCP callback functions look like this
*/
typedef HRESULT (*FCP_CB_FUNCTION)(PB *);

// structures for storing callback information  

typedef struct
{
	uint8  							ctsCode;
	FCP_CB_FUNCTION 				callback;
} FCP_CB_ELEMENT;					
									
extern OFFSET_1394					fcpOffsetCommand;
extern OFFSET_1394					fcpOffsetCommandEnd;
extern OFFSET_1394					fcpOffsetResponse;
extern OFFSET_1394					fcpOffsetResponseEnd;

/** Initialize the FCP module
*/
HRESULT		fcpInitialize(void);


/** Registers a callback function that will be called when an asynchronous write 
	request addressed to either the FCP_COMMAND_REGISTER, FCP_COMMAND_REGISTER is received,
	with a matching ctsCode.
    
	Only one callback per cts-code is permitted.

	If the callback function returns NO_ERROR, the callback function assumes ownership of the packet block.
	@param ctsCode		The ctsCode to register a callback for. FCP_CTS_CODE_AVC is the most common.
	@param callback		The callback function to call.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".	
*/
HRESULT				fcpRegisterCallback(uint8 ctsCode, FCP_CB_FUNCTION callback);

HRESULT				fcpCallback(PB *packetBlock);


/**	Sends the packet block to the FC_RESPONSE_REGISTER or FCP_COMMAND_REGISTER in the specified target.
	If the node handle specified in the target block is valid, this function
	together with the lower layers will construct all the necessary header 
	information for the 1394 asynchronous packet.

	An application, such as an AV/C subunit needs only to provide the AV/C content.
	
	@param packetBlock	The address of the packet block to use.
	@param bResponse	TRUE if this is an FCP response, FALSE if a FCP request.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	fcpSend(PB *packetBlock, BOOL bResponse) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	fcpSend(PB *packetBlock, BOOL bResponse)
								{
									return lalSendWriteRequest(packetBlock, (bResponse) ? fcpOffsetResponse : fcpOffsetCommand);
								}
/**	Sends the specified data to the FC_RESPONSE_REGISTER or FCP_COMMAND_REGISTER target identified
	by a handle.
	If the node handle specified in the target block is valid, this function
	together with the lower layers will construct all the necessary header 
	information for the 1394 asynchronous packet.

	An application, such as an AV/C subunit needs only to provide the AV/C content.
	
	@param handle		The handle for the node to send the packet to.
	@param byteLength	The number of bytes to send.
	@param bufPtr		The address of the data to send.
	@param bResponse	TRUE if this is an FCP response, FALSE if a FCP request.
	@return				NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
_TC_INLINE HRESULT	fcpBlockingSend(LAL_NODE_HANDLE handle, uint16 byteLength, uint32 *bufPtr, BOOL bResponse) _TC_INLINE_ALWAYS;
_TC_INLINE HRESULT	fcpBlockingSend(LAL_NODE_HANDLE handle, uint16 byteLength, uint32 *bufPtr, BOOL bResponse)
								{
									return lalWrite1394(handle, (bResponse) ? fcpOffsetResponse : fcpOffsetCommand, byteLength, bufPtr);
								}
//! @}
#endif //_FCP_H
