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

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "avcDefs.h"
#include "avcUtils.h"
#include "avcUnitUtils.h"
#include "rangemaps.h"

#if 0 //LM??? (if wanted then change to handle extended data directly (or move into avcUnitUtils)
static	RANGEMAP avcValidSubunitTypesRangeMap[] =
{
#if 1
	{ AVC_SU_TYPE_MONITOR,			RMEVAL_INRANGE,	AVC_SU_TYPE_AUDIO},	// video monitor, audio
	{ AVC_SU_TYPE_DISC,				RMEVAL_INRANGE, AVC_SU_TYPE_TUNER},	// disk, tape, tuner
	{ AVC_SU_TYPE_CAMERA,			RMEVAL_ONLY,	0},					// video camera
	{ AVC_SU_TYPE_MUSIC,			RMEVAL_ONLY,	0},					// music
	{ AVC_SU_TYPE_VENDOR_UNIQUE,	RMEVAL_ONLY,	0},					// vendor unique
	{ AVC_SU_TYPE_PANEL,			RMEVAL_ONLY,	0},
	{ AVC_SU_TYPE_EXTEND_FIRST,		RMEVAL_INRANGE,	AVC_SU_TYPE_UNIT},	// subunit extend, unit
	{0,RMEVAL_END,0}														// All maps must be terminated in this fashion
#else
	{ AVC_SU_TYPE_MONITOR,			RMEVAL_INRANGE,	AVC_SU_TYPE_VIDEO_CAMERA},
	{ AVC_SU_TYPE_PANEL,			RMEVAL_INRANGE, AVC_SU_TYPE_CAMERA_STORAGE},
	{ AVC_SU_TYPE_VENDOR_UNIQUE,	RMEVAL_ONLY,	0},
	{ AVC_SU_TYPE_EXTEND_FIRST,		RMEVAL_INRANGE,	AVC_SU_TYPE_UNIT},		// subunit extend, unit
	{0,RMEVAL_END,0}														// All maps must be terminated in this fashion
#endif
};
#endif

// read avc header from stream
HRESULT	avcDecodeHeader(pDataStream pStream, AVC_HEADER *pHeader)
{
	HRESULT		hResult = NO_ERROR;
	BYTE		opcode = 0;
	
	if (pHeader == NULL || pStream == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = dsReOpenStream(pStream, dsMODE_READ);
	if (hResult != NO_ERROR) return hResult;

	// parse the fields out of the data.
	hResult = dsReadBits(pStream, 4, &pHeader->fcptype);
	if (hResult != NO_ERROR) return hResult;
	
	hResult = dsReadBits(pStream, 4, &pHeader->ctype);
	if (hResult != NO_ERROR) return hResult;
	
	hResult = avcSubunitDecodeSubunit(pStream, &pHeader->addrSubunit);
	if (hResult != NO_ERROR) return hResult;
	
#if 0 //LM??? (if wanted then change to handle extended data directly (or move into avcUnitUtils)
	// check to see if the type and ID fields contain valid data

	if (rmInMap(pHeader->subunittype, avcValidSubunitTypesRangeMap) == FALSE)
	{
		hResult = E_AVC_PP_RESERVED_SUBUNITTYPE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
#endif

	hResult = dsRead8Bits(pStream, &opcode);
	if (hResult != NO_ERROR) return hResult;

	pHeader->opcode = opcode;
	
	// set the marker in the stream so that subsequent functions can find this
	dsSetMarker(pStream, DSMARKER_OPERAND_0);
	
	return hResult;
}

// write avc header to stream
HRESULT	avcEncodeHeader(pDataStream pStream, AVC_HEADER *pHeader)
{
	HRESULT		hResult = NO_ERROR;

	if (pHeader == NULL || pStream == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	hResult = dsReOpenStream(pStream, dsMODE_WRITE);
	if (hResult != NO_ERROR) return hResult;

	// parse the fields out of the data.
	hResult = dsWriteBits(pStream, 4, pHeader->fcptype);
	if (hResult != NO_ERROR) return hResult;

	hResult = dsWriteBits(pStream, 4, pHeader->ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcSubunitEncodeSubunit(pStream, &pHeader->addrSubunit);
	if (hResult != NO_ERROR) return hResult;
	
	hResult = dsWrite8Bits(pStream, (BYTE) pHeader->opcode);
	if (hResult != NO_ERROR) return hResult;

	// set the marker in the stream so that subsequent functions can find this
	dsSetMarker(pStream, DSMARKER_OPERAND_0);
	
	return hResult;		
}

/*******************************************************************************************
 
	FUNCTION:  avcDecodeFunctionBlockType
	
		Function uses dataStream utilities to parse Function Block type from a packet.
		The 1394ta spec (audio subunit) says that the FB type can be extended, sort
		of like the subunit type, only different.
		
		params:
			pStream 	Pointer to a DataStream with the stream's context being set to 
											 the first byte of the function_block_type, IE Operand[0].
		    pDbType						pointer to where the fb type is to be stored

		returns:
		   	NO_ERROR - FB type successfully parsed.  
				pDbType contains the type and the dataStream context is
		   		now at the next byte following the FB type, IE FB id.
		   	any other hResult is an error and dataStream is in an unknown state			 					    
		
*/
HRESULT	avcDecodeFunctionBlockType(DataStream  *pStream, uint32 *pDbType)
{
	HRESULT		hResult = NO_ERROR;
	uint8		temp = 0;

	hResult = dsRead8Bits(pStream, &temp);
	if (hResult != NO_ERROR) return hResult;

	if (temp == AVC_FB_TYPE_EXTEND)
	{
		hResult = E_AVC_PKT_AVC_NOT_IMPLEMENTED;	// function block type extension not defined well enough to actually implement.
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*pDbType = (uint32) temp;

	return hResult;
}

/*******************************************************************************************

	FUNCTION:  avcDecodeFunctionBlockID
	
		Function uses dataStream utilities to parse Function Block id from a packet.
		The 1394ta spec (audio subunit) says that the FB ID can be extended, sort
		of like the subunit id, only different.
		
		params:
			pStream 	Pointer to a DataStream with the stream's context being set to 
											 the first byte of the function_block_id, IE. Operand[1].
		    pFbId						Pointer to where the fb id is to be stored

		returns:
		   	NO_ERROR - FB type successfully parsed.  pFbId contains the id and the dataStream context is
		   				now at the next byte following the FB id, IE Operand[2].
		   	any other hResult is an error and dataStream is in an unknown state			 					    
		
*/
HRESULT	avcDecodeFunctionBlockID(DataStream  *pStream, uint32 *pDbId)
{
	HRESULT		hResult = NO_ERROR;
	uint8		temp = 0;

	hResult = dsRead8Bits(pStream, &temp);
	if (hResult == NO_ERROR) return hResult;

	if (temp == AVC_FB_ID_EXTEND)
	{
		hResult = E_AVC_PKT_AVC_NOT_IMPLEMENTED;		// function block id extension not defined well enough to actually implement.
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*pDbId = (uint32) temp;

	return hResult;
}

#if 0 //LM???
/*******************************************************************************************

	FUNCTION:  avcCtypeIsCommand
	
		Given a ctype, returns whether or not it is a command ctype
		
		params:
			ctype - ctype value
		returns:
		   	TRUE if it is a command value, FALSE otherwise.  Note that FALSE does not necessarily
		   	mean that ctype is a reponse value, there are possible values that are neither
			command nor response.
*/
BOOL avcCtypeIsCommand(uint32 ctype)
{
	switch (ctype)
	{
		case AVC_CTYPE_CONTROL:
		case AVC_CTYPE_STATUS:
		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_NOTIFY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			return TRUE;
		
		default:
			return FALSE;
	}
}

/*******************************************************************************************

	FUNCTION:  avcCtypeIsResponse
	
		Given a ctype, returns whether or not it is a response ctype
		
		params:
			ctype - ctype value
		returns:
		   	TRUE if it is a response value, FALSE otherwise.  Note that FALSE does not necessarily
		   	mean that ctype is a command value, there are possible values that are neither
			command nor response.
*/
BOOL avcCtypeIsResponse(uint32 ctype)
{
	switch (ctype)
	{
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_IMPLEMENTED: //AVC_RESPONSE_STABLE
		case AVC_RESPONSE_CHANGED:
		case AVC_RESPONSE_reserved:
		case AVC_RESPONSE_INTERIM:
			return TRUE;

		default:
			return FALSE;
	}	
}

#endif

/*******************************************************************************************/
