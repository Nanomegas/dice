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

	Implementation of avc unit commands
	AV/C Digital Interface Command Set General (Ver. 4.1) section 12
	
  Revisions:
		created 08/04/2003 LM

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "avc.h"
#include "avcUnitVendor.h"
#include "avcUnitCli.h"
#include "avcUnitVendorDefs.h"
#include "cr.h"
#include "systemCallbacks.h"

static HRESULT avcUnitGetProgramAttributes(TC_PROGRAM_ATTRIBUTES *programAttributes)
{
	HRESULT		hResult = NO_ERROR;

	programAttributes->attributeVersion = 0x00000001;
	programAttributes->programType = PROG_AVR_DRD;
	programAttributes->programVersion = 0x01030000 + 0x00000011; // major + build
	programAttributes->reserved[0] = 0xffffffff;
	programAttributes->reserved[1] = 0xffffffff;
	programAttributes->reserved[2] = 0xffffffff;
	programAttributes->reserved[3] = 0xffffffff;
	programAttributes->reserved[4] = 0xffffffff;

	return hResult;
}

// Callback descriptor for the specific command 
static CALLBACK_DESCRIPTOR AVC_FIRMWAREID_CALLBACK[] =
{
	{CALLBACK_DESCRIPTOR_TYPE, AVC_TC_VENDOR_TYPE},
	{CALLBACK_DESCRIPTOR_NEXT, 0},
	{CALLBACK_DESCRIPTOR_NAME, (uint32)"AVC_FIRMWAREID_CALLBACK"},
	{AVC_TC_VENDOR_CALLBACK_KEY, (uint32)&avcUnitVendorFirmwareInfo}, // CALLBACK PTR of TYPE AVC_SUBUNIT_CALLBACK},
	{AVC_TC_VENDOR_CLASSID_KEY, TC_VSAVC_CLASS_GENERAL},
	{AVC_TC_VENDOR_COMMAND_KEY, TC_VSAVC_CMD_PGM_IDENTIFY},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/*****************************************************************************
 
	Send response to the vendor-specific 'firmware ID' AV/C command

******************************************************************************/

HRESULT avcUnitVendorFirmwareInfo(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT					hResult = NO_ERROR;
	HRESULT					hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	uint32					i = 0;
	uint32					dsPos_operand7 = 0;
	uint32					payloadSize = 0;
	PB*						respPacket = NULL;
	pDataStream				pStreamResp = NULL;
	TC_PROGRAM_ATTRIBUTES	programAttributes;
	uint32					*pInt = NULL;
	switch (pHeader->ctype)
	{
		case AVC_CTYPE_STATUS:	// V : M:Mandatory, R:Recommended, O:Optional, V:Vendordependent, -:Not Defined

			hResultError = E_PKT_AVC_SUBUNIT_BUSY;
			
			// stream is positioned at operand[7]
			dsPos_operand7 = dsGetPosition(pStream);
	
			hResult = dsReadChunk(pStream, sizeof(TC_PROGRAM_ATTRIBUTES), (char *) &programAttributes);
			if (hResult != NO_ERROR) return hResultError;

			pInt = (uint32 *) &programAttributes;
			for (i = 0; i < sizeof(TC_PROGRAM_ATTRIBUTES) / 4; i++)
			{
				if (pInt[i] != 0xffffffff) return hResultError;
			}

			hResult = avcUnitGetProgramAttributes(&programAttributes);
			if (hResult != NO_ERROR) return hResultError;

			// Create a new (larger) packet for the response
			hResult = pbGetPayloadSize (packetBlock, &payloadSize) ;
			if (hResult != NO_ERROR) return hResultError;

			payloadSize += sizeof(TC_PROGRAM_ATTRIBUTES);

			hResult = avcCreateResponsePacket(packetBlock, &respPacket, TRUE, payloadSize);
			if (hResult != NO_ERROR) return hResultError;

			// write the requested information into the packet
	
			hResult = pbGetApplicationDatastream (respPacket, &pStreamResp);
			if (hResult != NO_ERROR) goto ErrorExit;

			dsSetPosition(pStreamResp, dsPos_operand7);	// ff to operand[7]

			hResult = dsSwitchMode(pStreamResp, dsMODE_WRITE);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsWriteChunk(pStreamResp, sizeof(TC_PROGRAM_ATTRIBUTES), (char *) &programAttributes);
			if (hResult != NO_ERROR) goto ErrorExit;

#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				avcUnitCliPrintVendorFirmwareInfo(&programAttributes);
			}
#endif //_SYSDEBUG

			avcSendResponsePacket(AVC_RESPONSE_STABLE, respPacket);

			hResult = NO_ERROR; //avcHandler shouldn't send a response too
			break;

		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			// don't even bother parsing
			hResult = E_PKT_AVC_IMPLEMENTED;
			break;
		
		case AVC_CTYPE_NOTIFY:	// - : M:Mandatory, R:Recommended, O:Optional, V:Vendordependent, -:Not Defined
		case AVC_CTYPE_CONTROL: // - : M:Mandatory, R:Recommended, O:Optional, V:Vendordependent, -:Not Defined
		default:
			hResult = hResultError;
			break;
	}

	return hResult;

ErrorExit:
	avcDestroyResponsePacket(respPacket);

	return hResult;
}	

/*****************************************************************************
 
	Vendor-specific AV/C command

	TC-specific commands are implemented right here

******************************************************************************/
HRESULT avcUnitVendorDependentTC(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT					hResult = NO_ERROR;
	HRESULT					hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	AVC_TC_VENDOR_CALLBACK	callback = NULL;
	CALLBACK_DESCRIPTOR*	cb = NULL;
	uint32					index = 0;
	uint8					classId = 0;
	uint8					seqId = 0;
	uint16					cmdId = 0;
	uint32					command = 0;

	// stream is positioned at the class id - operand[3]
	
	hResult = dsRead8Bits(pStream, &classId);		// operand[3]
	if (hResult != NO_ERROR) return hResultError;

	hResult = dsRead8Bits(pStream, &seqId);			// operand[4]
	if (hResult != NO_ERROR) return hResultError;
	
	hResult = dsRead16Bits(pStream, &cmdId);		// operand[5,..,6]
	if (hResult != NO_ERROR) return hResultError;

	DO_FOREVER
	{
		hResult = crGetCallback(AVC_TC_VENDOR_TYPE, index++, &cb);
		if (hResult != NO_ERROR) return hResultError;

		hResult = kvIsValue(cb, AVC_TC_VENDOR_CLASSID_KEY, classId);
		if (hResult != NO_ERROR) continue;

		hResult = kvGetValue(cb, AVC_TC_VENDOR_CALLBACK_KEY, (uint32 *) &callback);
		if (hResult != NO_ERROR) continue;
	
		hResult = kvGetValue(cb, AVC_TC_VENDOR_COMMAND_KEY, &command);
		
		// cmdId is optional
		if (hResult == NO_ERROR)
		{
			if (command != cmdId) continue;
		}

		hResult = (*callback) (pHeader, packetBlock, pStream);
		break;
	}
	
	return hResult;
}	

HRESULT avcUnitVendorDependent(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream, uint32 company_ID)
{
	HRESULT							hResult = NO_ERROR;
	HRESULT							hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	AVC_VENDOR_SPECIFIC_CALLBACK	callback = NULL;
	CALLBACK_DESCRIPTOR*			cb = NULL;
	uint32							index = 0;
	
	DO_FOREVER
	{
		hResult = crGetCallback(AVC_VENDOR_SPECIFIC_TYPE, index++, &cb);
		if (hResult != NO_ERROR) return hResultError;

		hResult = kvIsValue(cb, AVC_VENDOR_SPECIFIC_VENDORID_KEY, company_ID);
		if (hResult != NO_ERROR) continue;

		hResult = kvGetValue(cb, AVC_VENDOR_SPECIFIC_CALLBACK_KEY, (uint32 *) &callback);
		if (hResult != NO_ERROR) continue;
	
		hResult = (*callback) (pHeader, packetBlock, pStream);
		break;
	}

	return hResult;
}

// Unit vendor initialization
HRESULT avcUnitVendorInitialize(void)
{
	HRESULT		hResult = NO_ERROR;
	
	hResult = crAddCallback(AVC_FIRMWAREID_CALLBACK);
	if (hResult != NO_ERROR) return hResult;
	
	return hResult;
}
