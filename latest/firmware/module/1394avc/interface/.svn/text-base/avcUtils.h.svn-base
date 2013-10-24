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
  FILE PURPOSE:  
 
 		Functions for gaining access to the different fields in the AVC
 		payload packets.
 		
 		
 		****** WARNING *******
 		This code is written to parse AVC payloads which match the 
		AV/C Digital Interface Command Set General Specification Ver. 4.1
 		If the AVC payload does not match this, or contains in appropriate values, 
		then the parsing and packing functions actually FAIL.

		
  Revisions:
		created 20/03/2003 KBJ/LM

*****************************************************************************/
#ifndef _AVCUTILS_H
#define _AVCUTILS_H

#include "TCTypes.h"
#include "avcDefs.h"
#include "avcUnitUtils.h"
#include "datastream.h"
#ifdef _WINMP
#pragma warning( disable : 4514 )
#endif //_WINMP

// This a group of functions which will parse out, and return the header data found in the beginning of the AVC packets.

typedef struct
{
	uint32						fcptype;					// FCP_CTS_CODE_AVC for AVC packets
	uint32   					ctype;					
	AVC_ADDRESS_SUBUNIT			addrSubunit;
	uint32   					opcode;
} AVC_HEADER;

// For the DataStream Marking feature, we define the following:
#define		DSMARKER_OPERAND_0		'oper'		// Beginning of the operands

/* 
	FUNCTION:  avcDecodeHeader
	
		Uses the pDataStream to read in data from a packet, and then decodes the data into
		an AVC_HEADER structure, which can be used.
		
		
		params:

			incomingFCPPacketStream    : Pointer to a DataStream with the stream's context being set to the first byte
								 		 of the FCP Packet of one of our PacketBlocks. 
			
		    payloadheader	   		   : This is the payload header which you intend to put into your AVC packet.
		    					    
		  
	***** IMPORTANT *****
	
		If the result is NO_ERROR then the FCPPacketStream, has been advanced to the OPERAND[0]
		in the AVC packet. Thus the next write to the stream, will write INTO OPERAND[0]
		
		If the result is NOT NO_ERROR then FCPPacketStream, is at an unknown state. 
		
*/
HRESULT	avcDecodeHeader(pDataStream  pStream,  AVC_HEADER *header);

/* 
	FUNCTION:  avcEncodeHeader
	
		This function uses takes an AVC_HEADER, and packs it appropriately as a header for an
		AVC Payload, in the passed in pDataStream.
		
		
		params:

			pStream					: Pointer to a DataStream with the stream's context being set to the beginning
								 	  of the FCP Packet of one of our PacketBlocks. 
			
		    payloadheader	   		: This is the payload header which you intend to put into your AVC packet.
		    					    
		  
	***** IMPORTANT *****
	
		If the result is NO_ERROR then the FCPPacketStream, has been advanced to the OPERAND[0]
		in the AVC packet. Thus the next write to the stream, will write INTO OPERAND[0]
		
		If the result is NOT NO_ERROR then FCPPacketStream, is at an unknown state. 
		
*/
HRESULT	avcEncodeHeader(pDataStream pStream, AVC_HEADER *header);

/* 
	FUNCTION:  avcDecodeFunctionBlockType
	
		Function uses dataStream utilities to parse Function Block type from a packet.
		The 1394ta spec (audio subunit) says that the FB type can be extended, sort
		of like the subunit type, only different.
		
		params:

			incomingFCPPacketStream 	Pointer to a DataStream with the stream's context being set to 
											 the first byte of the function_block_type, IE Operand[0].
		    pFbType						pointer to where the fb type is to be stored

		returns:
		   	NO_ERROR - FB type successfully parsed.  pFbType contains the type and the dataStream context is
		   				now at the next byte following the FB type, IE FB id.
		   	any other result is an error and dataStream is in an unknown state			 					    
		
*/
HRESULT	avcDecodeFunctionBlockType(pDataStream pStream, uint32 *pDbType);

/* 
	FUNCTION:  avcDecodeFunctionBlockID
	
		Function uses dataStream utilities to parse Function Block id from a packet.
		The 1394ta spec (audio subunit) says that the FB ID can be extended, sort
		of like the subunit id, only different.
		
		params:

			incomingFCPPacketStream 	Pointer to a DataStream with the stream's context being set to 
											 the first byte of the function_block_id, IE. Operand[1].
		    pFbId						Pointer to where the fb id is to be stored

		returns:
		   	NO_ERROR - FB type successfully parsed.  pFbId contains the id and the dataStream context is
		   				now at the next byte following the FB id, IE Operand[2].
		   	any other result is an error and dataStream is in an unknown state			 					    
		
*/
HRESULT	avcDecodeFunctionBlockID(pDataStream pStream, uint32 *pDbId);

/* 
	FUNCTION:  avcCtypeIsCommand
	
		Given a ctype, returns whether or not it is a command ctype
		
		params:
			ctype - ctype value
		returns:
		   	TRUE if it is a command value, FALSE otherwise.  Note that FALSE does not necessarily
		   	mean that ctype is a reponse value, there are possible values that are neither
			command nor response.
*/
_TC_INLINE BOOL avcCtypeIsCommand(uint32 ctype) _TC_INLINE_ALWAYS;
_TC_INLINE BOOL avcCtypeIsCommand(uint32 ctype)
{
	return ((ctype & AVC_RESPONSE_MASK_BIT) != AVC_RESPONSE_MASK_BIT); // simple version
}

/* 
	FUNCTION:  avcCtypeIsResponse
	
		Given a ctype, returns whether or not it is a response ctype
		
		params:
			ctype - ctype value
		returns:
		   	TRUE if it is a response value, FALSE otherwise.  Note that FALSE does not necessarily
		   	mean that ctype is a command value, there are possible values that are neither
			command nor response.
*/
_TC_INLINE BOOL avcCtypeIsResponse(uint32 ctype) _TC_INLINE_ALWAYS;
_TC_INLINE BOOL avcCtypeIsResponse(uint32 ctype)
{
	return ((ctype & AVC_RESPONSE_MASK_BIT) == AVC_RESPONSE_MASK_BIT); // simple version
}

#endif //_AVCUTILS_H

