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

	Implementation of avc unit utilities
	
  Revisions:
		created 08/04/2003 LM

****************************************************************************/

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "avcDefs.h"
#include "avcUtils.h"
#include "avcUnitUtils.h"
#include "targetPlugs.h"
#include "cmp.h"

// read subunit from stream
HRESULT avcSubunitDecodeSubunit(pDataStream pStream, AVC_ADDRESS_SUBUNIT *subunit)
{
	HRESULT		hResult = NO_ERROR;
	uint8		parse8 = 0;

	hResult = dsReadBits(pStream, 5, &subunit->subunit_type);
	if (hResult != NO_ERROR) return hResult;
	
	hResult = dsReadBits(pStream, 3, &subunit->subunit_ID);
	if (hResult != NO_ERROR) return hResult;

	if (subunit->subunit_type == AVC_SU_TYPE_UNIT)				// type is unit
	{
		if (subunit->subunit_ID != AVC_SU_ID_IGNORE)
		{
			hResult = E_BAD_INPUT_PARAMETERS;					// Reserved
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}
	}
	else														// type is subunit
	{															
		if (subunit->subunit_type == AVC_SU_TYPE_EXTEND_FIRST)		// subunit type extended to next byte
		{
			// handling of extended subunit type is unclear ?

			subunit->subunit_type = (AVC_SU_TYPE_EXTEND_FIRST-1);

			DO_FOREVER									// only twice ?
			{
				hResult = dsRead8Bits(pStream, &parse8);
				if (hResult != NO_ERROR) return hResult;

				if (parse8 >= AVC_SU_TYPE_MIN_TYPE_EXTENDED && 
					parse8 <= AVC_SU_TYPE_MAX_TYPE_EXTENDED)		// extended subunit type
				{
					subunit->subunit_type += parse8;
					break;
				}
				else if (parse8 == AVC_SU_TYPE_EXTEND_EXTENDED)		// extended subunit type extended to next byte
				{
					subunit->subunit_type += (AVC_SU_TYPE_MAX_TYPE_EXTENDED-1);
				}
				else												// extended subunit type == 0 // reserved for future 
				{
					hResult = E_FAIL;
					sysLogError(hResult, __LINE__, moduleName);
					return hResult;
				}
			}
		}

		if (subunit->subunit_ID == AVC_SU_ID_EXTEND_FIRST3)		// subunit ID extended to next byte
		{
			DO_FOREVER									// only twice ?
			{
				hResult = dsRead8Bits(pStream, &parse8);
				if (hResult != NO_ERROR) return hResult;

				// handling of extended subunit ID is unclear ?

				if (parse8 < AVC_SU_ID_EXTEND_EXTENDED)				// extended subunit ID
				{
					subunit->subunit_ID += parse8;
					break;
				}
				else if (parse8 == AVC_SU_ID_EXTEND_EXTENDED)		// extended subunit ID extended to next byte
				{
					subunit->subunit_ID += AVC_SU_ID_MAX_INSTANCE_EXTENDED;
				}
			}
		}
	}

	return hResult;
}

// write subunit to stream
HRESULT avcSubunitEncodeSubunit(pDataStream pStream, AVC_ADDRESS_SUBUNIT *subunit)
{
	HRESULT		hResult = NO_ERROR;
	uint8		parse8 = 0;
	BOOL		bSubunitTypeExt = FALSE;
	BOOL		bSubunitIDExt = FALSE;
	uint32		subunit_type = subunit->subunit_type;
	uint32		subunit_ID = subunit->subunit_ID;
	uint32		type = subunit_type;
	uint32		id = subunit_ID;

	avcSubunitIsSubunitExtended(subunit, &bSubunitTypeExt, &bSubunitIDExt);

	if (bSubunitTypeExt) type = AVC_SU_TYPE_EXTEND_FIRST;
	if (bSubunitIDExt) id = AVC_SU_ID_EXTEND_FIRST3;

	hResult = dsWriteBits(pStream, 5, type);
	if (hResult != NO_ERROR) return hResult;
	
	hResult = dsWriteBits(pStream, 3, id);
	if (hResult != NO_ERROR) return hResult;

	if (bSubunitTypeExt == TRUE)					// subunit type extended to next byte
	{
		// handling of extended subunit type is unclear ?

		subunit_type -= (AVC_SU_TYPE_MAX_TYPE_EXTENDED-1);
		
		while (bSubunitTypeExt)						// only twice ?
		{
			if (subunit_type > (AVC_SU_TYPE_MAX_TYPE_EXTENDED-1))
			{
				parse8 = AVC_SU_TYPE_EXTEND_EXTENDED;
				subunit_type -= (AVC_SU_TYPE_MAX_TYPE_EXTENDED-1);
			}
			else
			{
				parse8 = (uint8) subunit_type;
				bSubunitTypeExt = FALSE;
			}
			
			hResult = dsWrite8Bits(pStream, parse8);
			if (hResult != NO_ERROR) return hResult;
		}
	}

	if (bSubunitIDExt == TRUE)						// subunit ID extended to next byte
	{
		// handling of extended subunit ID is unclear ?

		subunit_ID -= AVC_SU_ID_EXTEND_FIRST3;
		
		while (bSubunitTypeExt)						// only twice ?
		{
			if (subunit_ID > AVC_SU_ID_MAX_INSTANCE_EXTENDED)
			{
				parse8 = AVC_SU_ID_EXTEND_EXTENDED;
				subunit_ID -= AVC_SU_ID_MAX_INSTANCE_EXTENDED;
			}
			else
			{
				parse8 = (uint8) subunit_ID;
				bSubunitIDExt = FALSE;
			}
			
			hResult = dsWrite8Bits(pStream, parse8);
			if (hResult != NO_ERROR) return hResult;
		}
	}

	return hResult;
}

HRESULT avcSubunitDecodePlug(pDataStream pStream, AVC_ADDRESS_PLUG *plug)
{
	HRESULT		hResult = NO_ERROR;

	hResult = avcSubunitDecodeSubunit(pStream, &plug->addrSubunit);
	if (hResult != NO_ERROR) return hResult;
	hResult = dsRead8Bits(pStream, &plug->plugID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcSubunitEncodePlug(pDataStream pStream, AVC_ADDRESS_PLUG *plug)
{
	HRESULT		hResult = NO_ERROR;

	hResult = avcSubunitEncodeSubunit(pStream, &plug->addrSubunit);
	if (hResult != NO_ERROR) return hResult;
	hResult = dsWrite8Bits(pStream, plug->plugID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcSubunitDecodeLockPerm(pDataStream pStream, AVC_CONNECTION *connection)
{
	HRESULT		hResult = NO_ERROR;
	BYTE		parse8 = 0;

	// [111111--]: 0x3f, lock, perm
	hResult = dsRead8Bits(pStream, &parse8);
	if (hResult != NO_ERROR) return hResult;

	if ((parse8 & 0xFC) != 0xFC) return hResult;		// [111111--]: 0x3f, lock, perm
	connection->bPerm = ((parse8 & 0x01) == 0x01);		// check perm bit
	connection->bLock = ((parse8 & 0x02) == 0x02);		// check lock bit

	return hResult;
}

HRESULT avcSubunitEncodeLockPerm(pDataStream pStream, AVC_CONNECTION *connection)
{
	HRESULT		hResult = NO_ERROR;
	BYTE		parse8 = 0;

	// [111111--]: 0x3f, lock, perm
	parse8 = 0xFC;										// [111111--]: 0x3f, lock, perm
	if (connection->bLock) parse8 |= 0x02;				// Set lock bit
	if (connection->bPerm) parse8 |= 0x01;				// Set perm bit

	hResult = dsWrite8Bits(pStream, parse8);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

// read plug connection from stream
HRESULT avcSubunitDecodePlugConnection(pDataStream pStream, AVC_PLUG_CONNECTION *plugs)
{
	HRESULT		hResult = NO_ERROR;

	hResult = avcSubunitDecodePlug(pStream, &plugs->src);
	if (hResult != NO_ERROR) return hResult;
	hResult = avcSubunitDecodePlug(pStream, &plugs->dst);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

// write plug connection to stream
HRESULT avcSubunitEncodePlugConnection(pDataStream pStream, AVC_PLUG_CONNECTION *plugs)
{
	HRESULT		hResult = NO_ERROR;

	//LM??? possible problem with extended subunit type/IDs and need to extend dataStream size

	hResult = avcSubunitEncodePlug(pStream, &plugs->src);
	if (hResult != NO_ERROR) return hResult;
	hResult = avcSubunitEncodePlug(pStream, &plugs->dst);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

// read connection from stream
HRESULT avcSubunitDecodeConnection(pDataStream pStream, AVC_CONNECTION *connection)
{
	HRESULT		hResult = NO_ERROR;

	hResult = avcSubunitDecodeLockPerm(pStream, connection);
	if (hResult != NO_ERROR) return hResult;
	hResult = avcSubunitDecodePlugConnection(pStream, &connection->plugs);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

// write connection to stream
HRESULT avcSubunitEncodeConnection(pDataStream pStream, AVC_CONNECTION *connection)
{
	HRESULT		hResult = NO_ERROR;

	hResult = avcSubunitEncodeLockPerm(pStream, connection);
	if (hResult != NO_ERROR) return hResult;
	hResult = avcSubunitEncodePlugConnection(pStream, &connection->plugs);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

BOOL avcSubunitIsSubunitExtended(AVC_ADDRESS_SUBUNIT *subunit, BOOL *bTypeExt, BOOL *bIDExt)
{
	BOOL	bSubunitExt = FALSE;
	BOOL	bSubunitTypeExt = FALSE;
	BOOL	bSubunitIDExt = FALSE;

	if (avcSubunitIsUnit(subunit)) return bSubunitExt;

	if (subunit->subunit_type >= AVC_SU_TYPE_EXTEND_FIRST)			// subunit type extended to next byte
	{
		bSubunitTypeExt = TRUE;
		bSubunitExt = TRUE;
	}
	
	if (subunit->subunit_ID > AVC_SU_ID_MAX_INSTANCE_FIRST3)		// subunit id extended to next byte
	{
		bSubunitIDExt = TRUE;
		bSubunitExt = TRUE;
	}
	
	if (bTypeExt)
	{
		*bTypeExt = bSubunitTypeExt;
	}
	if (bIDExt)
	{
		*bIDExt = bSubunitIDExt;
	}

	return (bSubunitExt);
}

void avcSubunitGetUnitAddress(AVC_ADDRESS_SUBUNIT *subunit)
{
	subunit->subunit_type = AVC_SU_TYPE_UNIT;
	subunit->subunit_ID = AVC_SU_ID_IGNORE;
}

BOOL avcSubunitIsUnit(AVC_ADDRESS_SUBUNIT *subunit)
{
	// [tttttiii]:[11111111] (type:5 ID:3)
	return ((subunit->subunit_type == AVC_SU_TYPE_UNIT) && (subunit->subunit_ID == AVC_SU_ID_IGNORE));
}

BOOL avcSubunitEqualToTypeID(AVC_ADDRESS_SUBUNIT *subunit, uint8 typeID)
{
	BOOL	bEqual = FALSE;

	// [tttttiii]:[typeID] (type:5 ID:3)
	bEqual = (subunit->subunit_type == (uint32) (typeID >> 3)); 
	if (bEqual != TRUE) return bEqual;
	bEqual = (subunit->subunit_ID != (uint32) (typeID & 0x07));
	if (bEqual != TRUE) return bEqual;

	return bEqual;
}

HRESULT avcSubunitGetTypeID(AVC_ADDRESS_SUBUNIT *subunit, uint8 *typeID)
{
	HRESULT		hResult = NO_ERROR;

	if (avcSubunitIsSubunitExtended(subunit, NULL, NULL) == TRUE)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// [tttttiii]:[typeID] (type:5 ID:3)
	*typeID = (uint8) ((subunit->subunit_type << 3) + (subunit->subunit_ID & 0x07));

	return hResult;
}

HRESULT avcSubunitSetTypeID(AVC_ADDRESS_SUBUNIT *subunit, uint8 typeID)
{
	HRESULT		hResult = NO_ERROR;

	// [tttttiii]:[typeID] (type:5 ID:3)
	subunit->subunit_type = (uint32) (typeID >> 3);
	subunit->subunit_ID = (uint32) (typeID & 0x07);

	return hResult;
}

BOOL avcSubunitEqual(AVC_ADDRESS_SUBUNIT *subunit1, AVC_ADDRESS_SUBUNIT *subunit2)
{
	BOOL	bEqual = FALSE;

	bEqual = (subunit1->subunit_type == subunit2->subunit_type);
	if (bEqual != TRUE) return bEqual;
	bEqual = (subunit1->subunit_ID == subunit2->subunit_ID);
	if (bEqual != TRUE) return bEqual;

	return bEqual;
}

BOOL avcSubunitPlugsEqual(AVC_ADDRESS_PLUG *plug1, AVC_ADDRESS_PLUG *plug2)
{
	BOOL	bEqual = FALSE;

	bEqual = avcSubunitEqual(&plug1->addrSubunit, &plug2->addrSubunit);
	if (bEqual) bEqual = (plug1->plugID == plug2->plugID);
	
	return bEqual;
}

BOOL avcSubunitPlugIsInvalidPlug(AVC_ADDRESS_PLUG *plug)
{
	return (avcSubunitIsUnit(&plug->addrSubunit) && (plug->plugID == AVC_PLUG_ID_INVALID));
}

BOOL avcSubunitConnectionPlugsEqual(AVC_PLUG_CONNECTION *plugs1, AVC_PLUG_CONNECTION *plugs2)
{
	BOOL	bEqual = FALSE;

	bEqual = avcSubunitPlugsEqual(&plugs1->src, &plugs2->src);
	if (bEqual) bEqual = avcSubunitPlugsEqual(&plugs1->dst, &plugs2->dst);
	
	return bEqual;
}

BOOL avcSubunitConnectionsEqual(AVC_CONNECTION *connection1, AVC_CONNECTION *connection2)
{
	BOOL	bEqual = FALSE;

	bEqual = (connection1->bLock == connection2->bLock);
	if (bEqual) bEqual = (connection1->bPerm == connection2->bPerm);
	if (bEqual) bEqual = avcSubunitConnectionPlugsEqual(&connection1->plugs, &connection2->plugs);
	
	return bEqual;
}

uint32 avcSubunitGetSubunitByteSize(AVC_ADDRESS_SUBUNIT *subunit)
{
	uint32		size = 0;
	BOOL		bSubunitExt = FALSE;
	BOOL		bSubunitTypeExt = FALSE;
	BOOL		bSubunitIDExt = FALSE;
	uint32		subunit_type = subunit->subunit_type;
	uint32		subunit_ID = subunit->subunit_ID;

	bSubunitExt = avcSubunitIsSubunitExtended(subunit, &bSubunitTypeExt, &bSubunitIDExt);

	if (bSubunitExt == FALSE)
	{
		size = 1;
	}
	else
	{
		size++;
		if (bSubunitTypeExt == TRUE)					// subunit type extended to next byte
		{
			// handling of extended subunit type is unclear ?
			subunit_type -= (AVC_SU_TYPE_MAX_TYPE_EXTENDED-1);
			while (bSubunitTypeExt)						// only twice ?
			{
				if (subunit_type > (AVC_SU_TYPE_MAX_TYPE_EXTENDED-1))
				{
					subunit_type -= (AVC_SU_TYPE_MAX_TYPE_EXTENDED-1);
				}
				else
				{
					bSubunitTypeExt = FALSE;
				}
				size++;
			}
		}

		if (bSubunitIDExt == TRUE)						// subunit ID extended to next byte
		{
			// handling of extended subunit ID is unclear ?
			subunit_ID -= AVC_SU_ID_EXTEND_FIRST3;
			while (bSubunitTypeExt)						// only twice ?
			{
				if (subunit_ID > AVC_SU_ID_MAX_INSTANCE_EXTENDED)
				{
					subunit_ID -= AVC_SU_ID_MAX_INSTANCE_EXTENDED;
				}
				else
				{
					bSubunitIDExt = FALSE;
				}
				size++;
			}
		}

	}
	
	return size;
}

uint32 avcSubunitGetConnectionByteSize(AVC_CONNECTION *connection)
{
	uint32	size = 0;

	size += 1;		// lock,perm
	size += avcSubunitGetSubunitByteSize(&connection->plugs.src.addrSubunit);
	size += 1;		// connection->plugs.src.plugID
	size += avcSubunitGetSubunitByteSize(&connection->plugs.dst.addrSubunit);
	size += 1;		// connection->plugs.dst.plugID

	return size;
}


void avcSubunitCopySubunit(AVC_ADDRESS_SUBUNIT *dst, AVC_ADDRESS_SUBUNIT *src)
{
	dst->subunit_type = src->subunit_type;
	dst->subunit_ID = src->subunit_ID;
}

void avcSubunitCopyPlug(AVC_ADDRESS_PLUG *dst, AVC_ADDRESS_PLUG *src)
{
	avcSubunitCopySubunit(&dst->addrSubunit, &src->addrSubunit);
	dst->plugID = src->plugID;
}

void avcSubunitCopyConnection(AVC_CONNECTION *dst, AVC_CONNECTION *src)
{
	dst->bLock = src->bLock;
	dst->bPerm = src->bPerm;
	avcSubunitCopyPlug(&dst->plugs.src, &src->plugs.src);
	avcSubunitCopyPlug(&dst->plugs.dst, &src->plugs.dst);
}

HRESULT avcSubunitValidatePlug(AVC_CONNECTION *connection, BOOL bSource)
{
	HRESULT				hResult = NO_ERROR;
	AVC_ADDRESS_PLUG	*plug = NULL;
	BOOL				bChoosePlug = FALSE;
	BOOL				bMultiplePlugs = FALSE;

	// verify src/dst plugIDs - if PlugID = "any available plug" update connection with the actual plugID if available

	if (bSource)
	{
		plug = &connection->plugs.src;
	}
	else
	{
		plug = &connection->plugs.dst;
	}

	if (avcSubunitIsUnit(&plug->addrSubunit))
	{	
		// Unit plugs
	 	if		(plug->plugID <= 0x1E)	hResult = NO_ERROR;		// 00-1E: Isoch I/O Plug iPCR/oPCR [0;30]
	 	else if (plug->plugID <= 0x3F)	hResult = E_FAIL;		// 1F-3F: Reserved
	 	else if (plug->plugID <= 0x5E)	hResult = NO_ERROR;		// 40-5E: General Bus I/O Plug [0;30]
	 	else if (plug->plugID <= 0x7E)	hResult = E_FAIL;		// 5F-7E: Reserved
		else if (plug->plugID == 0x7F)	bChoosePlug = TRUE;		//    7F: Any available Isoch I/O Plug
		else if (plug->plugID <= 0x9E)	hResult = NO_ERROR;		// 80-9E: External I/O Plug [0;30]
		else if (plug->plugID == 0x9F)	hResult = E_FAIL;		//    9F: Reserved
		else if (plug->plugID <= 0xBE)	hResult = NO_ERROR;		// A0-BE: Async I/O Plug [0;30]
		else if (plug->plugID == 0xBF)	bChoosePlug = TRUE;		//    BF: Any available Async I/O Plug
		else if (plug->plugID <= 0xFC)	hResult = E_FAIL;		// C0-FC: Reserved
		else if (plug->plugID == 0xFD)	bMultiplePlugs = TRUE;	//    FD: I:Reserved, O:Multiple Plugs
		else if (plug->plugID == 0xFE)	hResult = E_FAIL;		//    FE: Invalid
		else if (plug->plugID == 0xFF)	bChoosePlug = TRUE;		//    FF: Any available External I/O Plug
		else							hResult = E_UNREACHABLE_CODE_REACHED;
	}
	else
	{
		// Subunit plugs
	 	if		(plug->plugID <= 0x1E)	hResult = NO_ERROR;		// 00-1E: Src/Dst Plug [0;30]
	 	else if (plug->plugID <= 0x3F)	hResult = E_FAIL;		// 1F-FC: Reserved
		else if (plug->plugID == 0xFD)	bMultiplePlugs = TRUE;	//    FD: I:Reserved, O:Multiple Plugs
		else if (plug->plugID == 0xFE)	hResult = E_FAIL;		//    FE: Invalid
		else if (plug->plugID == 0xFF)	bChoosePlug = TRUE;		//    FF: Any available Src/Dst I/O Plug
		else							hResult = E_UNREACHABLE_CODE_REACHED;
	}

	if (bMultiplePlugs == TRUE)
	{
		if (bSource == TRUE)	hResult = E_FAIL;
		else					hResult = NO_ERROR;
	}

	if (hResult != NO_ERROR)
	{
		sysLogError(hResult, __LINE__, moduleName);
	}
	
	if (bChoosePlug == TRUE)
	{
		hResult = targetChoosePlug(connection, bSource);
	}

	return hResult;
}

HRESULT avcSubunitValidatePlugs(AVC_CONNECTION *connection)
{
	HRESULT		hResult = NO_ERROR;

	hResult = avcSubunitValidatePlug(connection, TRUE);		// plugs.src
	if (hResult != NO_ERROR) return hResult;
	hResult = avcSubunitValidatePlug(connection, FALSE);	// plugs.dst
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcSubunitConnectPlugs(AVC_CONNECTION *connection)
{
	return targetConnectPlugs(connection);
}

HRESULT avcSubunitDisconnectPlugs(AVC_CONNECTION *connection)
{
	return targetDisconnectPlugs(connection);
}

HRESULT avcSubunitGetUnitInfoAddress(AVC_ADDRESS_SUBUNIT *subunit)
{
	return targetGetUnitInfoAddress(subunit);
}

HRESULT avcSubunitGetUnitInfoCompanyID(uint32 *company_ID)
{
#if 1 //LM???
	return targetGetUnitInfoCompanyID(company_ID);
#else
	//LM??? read from the AVC csr
#endif
}

HRESULT avcSubunitGetSubunitInfoPageData(pDataStream pStream, uint32 page, uint32 extension_code)
{
	HRESULT					hResult = NO_ERROR;
	HRESULT					hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	AVC_ADDRESS_SUBUNIT		subunit;
	uint8					typeID;
	uint32					index;
	uint32					zeroBit = 0;

	//LM??? won't work for extended subunit type/IDs (not enough space in page_data/packetBlock payload?)
	/*
		current avcSubunitGetSubunitInfoPageData() returns E_BAD_INPUT_PARAMETERS
		
		update to check if page contains extended subunit type/IDs 
		then create new packet with larger payload

		hResult = avcCreateResponsePacket(packetBlock, &respPacket, TRUE, payloadSize);
		if (hResult != NO_ERROR) return hResultError;

		... like below but update avcSubunitGetSubunitInfoPageData to handle extended correctly

		avcSendResponsePacket(AVC_RESPONSE_STABLE, respPacket);

		hResult = E_PKT_AVC_HANDLED_BY_CALLBACK; //avcHandler shouldn't send a response too
	*/

													// operand[0]: unchanged
	hResult = dsWriteBits(pStream, 1, zeroBit);
	if (hResult != NO_ERROR) return hResultError;

	hResult = dsWriteBits(pStream, 3, page);
	if (hResult != NO_ERROR) return hResultError;

	hResult = dsWriteBits(pStream, 1, zeroBit);
	if (hResult != NO_ERROR) return hResultError;

	hResult = dsWriteBits(pStream, 3, extension_code);
	if (hResult != NO_ERROR) return hResultError;

	for (index = 0; index < 4; index++)
	{
		hResult = targetGetSubunitInfoEntry(page * 4 + index, &subunit);
		if (hResult != NO_ERROR) 
		{
			// no more subunit table entries
			hResult = NO_ERROR;
			break;
		}

		hResult = avcSubunitGetTypeID(&subunit, &typeID);
		if (hResult != NO_ERROR) return hResultError;

		//LM??? won't work for extended subunit type/IDs (not enough space in page_data/packetBlock payload?)
		if (avcSubunitIsSubunitExtended(&subunit, NULL, NULL) == TRUE)
		{
			hResult = E_BAD_INPUT_PARAMETERS;				
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}

		hResult = dsWrite8Bits(pStream, typeID);
		if (hResult != NO_ERROR) return hResultError;
	}

	if (index == 0) return hResultError; // page contains no data

	return hResult;
/*
	uint32	 				index = 0;
	uint32					count = 0;
	uint32					pages[8];  
	uint32   				subunit_type = 0;
	CALLBACK_DESCRIPTOR*	cb = NULL;
	DataStream				ds;

	// Now fill in the packet's avc stream with the entire page which was requested
		
			// initialize local variables
			memset(pages, 0xFF, sizeof(pages));

			hResult = dsOpenStream(&ds, pages, sizeof(pages), dsMODE_WRITE);
			if (hResult != NO_ERROR) return hResultError;

#if 0
			// construct all the pages based on the information contained in the callback registry
			DO_FOREVER
			{
				hResult = crGetCallback(AVC_SUBUNIT_TYPE, index++, &cb);
				if (hResult != NO_ERROR) break;

				hResult = kvGetValue(cb, AVC_SUBUNIT_SUBUNITTYPE_KEY, &subunit_type);
				if (hResult == NO_ERROR)

			}
#endif
			// construct all the pages based on the information contained in the callback registry
			for (subunit_type = AVC_SU_TYPE_MONITOR; subunit_type <= AVC_SU_TYPE_PANEL; subunit_type++) //LM???
			{
				// Look up all the subunit callbacks, and check to see if they match the current type.	
				index = 0;
				count = 0;
				DO_FOREVER
				{
					hResult = crGetCallback(AVC_SUBUNIT_TYPE, index++, &cb);
					if (hResult != NO_ERROR) break;

					hResult = kvIsValue(cb, AVC_SUBUNIT_SUBUNITTYPE_KEY, subunit_type);
					if (hResult == NO_ERROR) count++;
				}
				
				// don't put data into the page data unless a particular subunittype exists
				if (count)
				{
					dsWriteBits(&ds, 5, subunit_type);
					dsWriteBits(&ds, 3, count - 1);// max_subunit_id is count-1 of the subunit's
				}
			}

			if (avcSubunitIsSubunitExtended(subunit_data, NULL, NULL) == FALSE)
			{
				page_data
			}


			// Now fill in the packet's avc stream with the entire page which was requested
			page_data = pages[page];

			if (page_data != 0xFFFFFFFF)
			{
				return E_PKT_AVC_NOT_IMPLEMENTED;
			}
*/
}

HRESULT	avcSubunitGetSubunitVersionInfo(AVC_ADDRESS_SUBUNIT *subunit, BYTE *version_information, BYTE *su_dependent_field)
{
	// version_information: 0xFF: get latest version to which the unit or subunit complies
	// version_information: 0x00-0xFE: get the support level of the specified version
	return targetGetSubunitVersionInfo(subunit, version_information, su_dependent_field);
}

HRESULT avcSubunitFindPlug(AVC_CONNECTION *connection)
{
	return targetFindPlug(connection);
}

BOOL avcSubunitChannelIsConnected(BYTE channel, uint16 *nodeAddr, BYTE *oPCRNumber)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		bIsConnected = FALSE;
	uint32		plugRegister = 0;
	uint32		index;
	uint32		thisNodeAddr = 0;

	if (*nodeAddr == 0xFFFF &&
		*oPCRNumber == 0xFF)
	{
		for (index = 0; index < plugsGetNumIsochOutPlugs(); index++)
		{
			hResult = cmpGetoPCR(index, &plugRegister);
			if (hResult == NO_ERROR)
			{
				if (channel == cmpGetPCRChannel(plugRegister))
				{
					hResult = lalGetThisNodeAddr(&thisNodeAddr);
					if (hResult != NO_ERROR) return hResult;

					bIsConnected = (cmpGetPCRConnCount(plugRegister) > 0);

					if (bIsConnected)
					{
						*oPCRNumber = (BYTE) index;
						*nodeAddr = (uint16) thisNodeAddr;
						break;
					}
				}
			}
		}
	}
	else
	{
		// check specific oPCR
		hResult = cmpGetoPCR(*oPCRNumber, &plugRegister);
		if (hResult == NO_ERROR)
		{
			if (channel == cmpGetPCRChannel(plugRegister))
			{
				bIsConnected = (cmpGetPCRConnCount(plugRegister) > 0);
			}
		}
	}

	return bIsConnected;
}

HRESULT avcSubunitGetConnections(uint8 *numConnections, uint8 maxConnections, AVC_CONNECTION *connectionArray)
{
	return targetGetConnections(numConnections, maxConnections, connectionArray);
}

HRESULT avcSubunitSetBroadcastInEstablished(BOOL bEstablished)
{
	HRESULT		hResult = NO_ERROR;
	uint32		iPCRNumber = 0;
	
	hResult = plugsGetBroadcastInPCRNumber(&iPCRNumber);
	if (hResult != NO_ERROR) return hResult;

	if (bEstablished)
	{
		hResult = cmpBCInConnectionEstablish(iPCRNumber);
	}
	else
	{
		hResult = cmpBCInConnectionBreak(iPCRNumber);
	}

	return hResult;
}

HRESULT avcSubunitIsBroadcastInEstablished(BOOL *bEstablished)
{
	HRESULT		hResult = NO_ERROR;
	uint32		iPCRNumber = 0;
	
	hResult = plugsGetBroadcastInPCRNumber(&iPCRNumber);
	if (hResult != NO_ERROR) return hResult;
	
	hResult = cmpBCInIsConnectionEstablished(iPCRNumber, bEstablished);

	return hResult;
}

HRESULT avcSubunitSetBroadcastOutEstablished(BOOL bEstablished)
{
	HRESULT		hResult = NO_ERROR;
	uint32		oPCRNumber = 0;
	
	hResult = plugsGetBroadcastOutPCRNumber(&oPCRNumber);
	if (hResult != NO_ERROR) return hResult;

	if (bEstablished)
	{
		hResult = cmpBCOutConnectionEstablish(oPCRNumber);
	}
	else
	{
		hResult = cmpBCOutConnectionBreak(oPCRNumber);
	}

	return hResult;
}

HRESULT avcSubunitIsBroadcastOutEstablished(BOOL *bEstablished)
{
	HRESULT		hResult = NO_ERROR;
	uint32		oPCRNumber = 0;
	
	hResult = plugsGetBroadcastOutPCRNumber(&oPCRNumber);
	if (hResult != NO_ERROR) return hResult;

	hResult = cmpBCInIsConnectionEstablished(oPCRNumber, bEstablished);

	return hResult;
}
