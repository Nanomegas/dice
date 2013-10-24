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

	Implementation of avc unit vendor specific stuff

	Vendor dependent
		VENDOR DEPENDENT

	VENDOR SPECIFIC CALLBACKS
	
	All vendor specific callbacks are registered using a "class id" which allows us
	to callback the appropriate code for handling this packet.
	
	CALLBACK_REGISTRY	AVC_VENDORSPECIFIC_CALLBACK[]=
	{
		{CALLBACK_DESCRIPTOR_TYPE,AVC_VENDOR_SPECIFIC_TYPE},
		{CALLBACK_DESCRIPTOR_NEXT,0},
		{AVC_VENDOR_SPECIFIC_CALLBACK_KEY,(DH_UINT32)xxx}, // CALLBACK PTR of TYPE AVC_VENDOR_SPECIFIC_CALLBACK},
		{AVC_VENDOR_SPECIFIC_CLASS_KEY,0},
		{KV_TERMINATEKEY,KV_TERMINATEVALUE}
	};

  Revisions:
		created 08/04/2003 LM

****************************************************************************/

#ifndef _AVCUNITVENDOR_H
#define _AVCUNITVENDOR_H

#include "TCTypes.h"
#include "datastream.h"
#include "packetblock.h"
#include "avcUtils.h"

HRESULT		avcUnitVendorInitialize(void);

HRESULT		avcUnitVendorDependentTC(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);
HRESULT		avcUnitVendorDependent(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream, uint32 company_ID);

// TC Vendor Dependent callbacks
HRESULT		avcUnitVendorFirmwareInfo(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);

#endif //_AVCUNITVENDOR_H

