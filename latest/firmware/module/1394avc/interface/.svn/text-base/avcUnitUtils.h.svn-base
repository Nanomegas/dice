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

#ifndef _AVCUNITUTILS_H
#define _AVCUNITUTILS_H

#include "TCTypes.h"
#include "packetBlock.h"
#include "datastream.h"
#include "avcDefs.h"

typedef struct
{
	SUBUNIT_TYPE				subunit_type;
	SUBUNIT_ID					subunit_ID;
} AVC_ADDRESS_SUBUNIT;

typedef struct
{
	AVC_ADDRESS_SUBUNIT			addrSubunit;
	uint8						plugID;
} AVC_ADDRESS_PLUG;

typedef struct
{
	AVC_ADDRESS_PLUG			src;
	AVC_ADDRESS_PLUG			dst;
} AVC_PLUG_CONNECTION;

typedef struct												// See AV/C defines of CONNECT command to see what each of these are
{
	BOOL						bLock;						// lock bit: connection is locked
	BOOL						bPerm;						// perm bit: connection is permanent
	AVC_PLUG_CONNECTION			plugs;
} AVC_CONNECTION;

HRESULT		avcSubunitDecodeSubunit(pDataStream pStream, AVC_ADDRESS_SUBUNIT *addr);
HRESULT		avcSubunitEncodeSubunit(pDataStream pStream, AVC_ADDRESS_SUBUNIT *subunit);
HRESULT		avcSubunitDecodePlug(pDataStream pStream, AVC_ADDRESS_PLUG *plug);
HRESULT		avcSubunitEncodePlug(pDataStream pStream, AVC_ADDRESS_PLUG *plug);
HRESULT		avcSubunitDecodeLockPerm(pDataStream pStream, AVC_CONNECTION *connection);
HRESULT		avcSubunitEncodeLockPerm(pDataStream pStream, AVC_CONNECTION *connection);
HRESULT		avcSubunitDecodePlugConnection(pDataStream pStream, AVC_PLUG_CONNECTION *plugs);
HRESULT		avcSubunitEncodePlugConnection(pDataStream pStream, AVC_PLUG_CONNECTION *plugs);
HRESULT		avcSubunitDecodeConnection(pDataStream pStream, AVC_CONNECTION *connection);
HRESULT		avcSubunitEncodeConnection(pDataStream pStream, AVC_CONNECTION *connection);

BOOL		avcSubunitIsSubunitExtended(AVC_ADDRESS_SUBUNIT *subunit, BOOL *bTypeExt, BOOL *bIDExt);
HRESULT		avcSubunitGetTypeID(AVC_ADDRESS_SUBUNIT *subunit, uint8 *typeID);
HRESULT		avcSubunitSetTypeID(AVC_ADDRESS_SUBUNIT *subunit, uint8 typeID);
void		avcSubunitGetUnitAddress(AVC_ADDRESS_SUBUNIT *subunit);
BOOL		avcSubunitIsUnit(AVC_ADDRESS_SUBUNIT *addr);

BOOL		avcSubunitEqualToTypeID(AVC_ADDRESS_SUBUNIT *subunit, uint8 typeID);
BOOL		avcSubunitEqual(AVC_ADDRESS_SUBUNIT *subunit1, AVC_ADDRESS_SUBUNIT *subunit2);
BOOL		avcSubunitPlugsEqual(AVC_ADDRESS_PLUG *plug1, AVC_ADDRESS_PLUG *plug2);
BOOL		avcSubunitPlugIsInvalidPlug(AVC_ADDRESS_PLUG *plug);
BOOL		avcSubunitConnectionPlugsEqual(AVC_PLUG_CONNECTION *plugs1, AVC_PLUG_CONNECTION *plugs2);
BOOL		avcSubunitConnectionsEqual(AVC_CONNECTION *connection1, AVC_CONNECTION *connection2);

uint32		avcSubunitGetSubunitByteSize(AVC_ADDRESS_SUBUNIT *subunit);
uint32		avcSubunitGetConnectionByteSize(AVC_CONNECTION *connectionArray);

void		avcSubunitCopySubunit(AVC_ADDRESS_SUBUNIT *dst, AVC_ADDRESS_SUBUNIT *src);
void		avcSubunitCopyPlug(AVC_ADDRESS_PLUG *dst, AVC_ADDRESS_PLUG *src);
void		avcSubunitCopyConnection(AVC_CONNECTION *dst, AVC_CONNECTION *src);

HRESULT		avcSubunitValidatePlug(AVC_CONNECTION *connection, BOOL bSource);

HRESULT		avcSubunitValidatePlugs(AVC_CONNECTION *connection);
HRESULT		avcSubunitConnectPlugs(AVC_CONNECTION *connection);
HRESULT		avcSubunitDisconnectPlugs(AVC_CONNECTION *connection);
HRESULT		avcSubunitGetUnitInfoAddress(AVC_ADDRESS_SUBUNIT *subunit);
HRESULT		avcSubunitGetUnitInfoCompanyID(uint32 *company_ID);
HRESULT		avcSubunitGetSubunitInfoPageData(pDataStream pStream, uint32 page, uint32 extension_code);
HRESULT		avcSubunitGetSubunitVersionInfo(AVC_ADDRESS_SUBUNIT *subunit, BYTE *version_information, BYTE *su_dependent_field);
HRESULT		avcSubunitFindPlug(AVC_CONNECTION *connection);

BOOL		avcSubunitChannelIsConnected(BYTE channel, uint16 *nodeAddr, BYTE *oPCRNumber);

HRESULT		avcSubunitGetConnections(uint8 *numConnections, uint8 maxConnections, AVC_CONNECTION *connectionArray);

HRESULT		avcSubunitSetBroadcastInEstablished(BOOL bEstablished);
HRESULT		avcSubunitIsBroadcastInEstablished(BOOL *bEstablished);
HRESULT		avcSubunitSetBroadcastOutEstablished(BOOL bEstablished);
HRESULT		avcSubunitIsBroadcastOutEstablished(BOOL *bEstablished);

#endif //_AVCUNITUTILS_H

