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

	Implementation of avc general commands
	AV/C Digital Interface Command Set General (Ver. 4.1) section 11

  	POWER
	UNIT INFO
	SUBUNIT INFO
	RESERVE
	VERSION
	VENDOR-DEPENDENT

  Revisions:
		created 08/04/2003 LM

****************************************************************************/

#ifndef _AVCCMDGENERAL_H
#define _AVCCMDGENERAL_H

#include "TCTypes.h"
#include "packetBlock.h"
#include "datastream.h"
#include "avcUtils.h"

HRESULT		avcCmdPower(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT		avcCmdUnitInfo(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT		avcCmdSubunitInfo(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT		avcCmdReserve(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT		avcCmdVersion(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT		avcCmdVendorDependent(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);

#endif //_AVCCMDGENERAL_H

