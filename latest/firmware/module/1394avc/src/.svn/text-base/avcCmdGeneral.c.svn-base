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

	
										ctype			target
										C,S,N			U,S
		POWER							O,O,R			+,+
		UNIT INFO						-,M,-			+.-
		SUBUNIT INFO					-,M,-			+,-
		RESERVE							O,O,R			+,+	
		VERSION							-,O,-			+,+
		VENDOR_DEPENDENT				V,V,V			+,+

			ctype:
			C:CONTROL, S:STATUS, N:NOTIFY
			M:Mandatory, R:Recommended, O:Optional, -:Not Defined, V:Vendor dependent

			target
			U:UNIT, S:SUBUNIT

  Revisions:
		created 08/04/2003 LM

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "avcCmdGeneral.h"
#include "avcDefs.h"
#include "avcUnitCli.h"
#include "avcUnitPower.h"
#include "avcUnitNotify.h"
#include "avcUnitVendor.h"
#include "cr.h"
#include "cli.h"
#include "systemCallbacks.h"
#include "targetPlugs.h"
#include "targetVendorDefs.h"

HRESULT avcCmdPower(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	BYTE			power_state = 0;
	TC_POWERSTATE	unitpowerstate = TC_POWERSTATE_UNKNOWN;

	switch (pHeader->ctype)
	{
		case AVC_CTYPE_CONTROL: // O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			hResultError = E_PKT_AVC_REJECTED;

			hResult = dsRead8Bits(pStream, &power_state);
			if (hResult != NO_ERROR) return hResultError;
			
#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				avcUnitCliPrintPowerState(power_state);
			}
#endif //_SYSDEBUG

			if (power_state == AVC_POWER_ON)
			{
				hResult = avcUnitPowerOn();
			}
			else if (power_state == AVC_POWER_OFF)
			{
				hResult = avcUnitPowerOff();
			}
			else
			{
				return hResultError;
			}
			break;

		case AVC_CTYPE_STATUS:	// O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		case AVC_CTYPE_NOTIFY:	// R : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
				
			hResultError = E_PKT_AVC_REJECTED;

			hResult = dsRead8Bits(pStream, &power_state);
			if (hResult != NO_ERROR) return hResultError;
			
#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				avcUnitCliPrintPowerState(power_state);
			}
#endif //_SYSDEBUG

			if (power_state != AVC_POWER_STATUS) return hResultError;
			
			// incoming status command has parsed successfully

			unitpowerstate = avcUnitPowerState();

			if (unitpowerstate == TC_POWERSTATE_ON)
			{
				power_state = AVC_POWER_ON;
			}
			else if (unitpowerstate == TC_POWERSTATE_OFF)
			{
				power_state = AVC_POWER_OFF;
			}
			else //TC_POWERSTATE_UNKNOWN
			{
				return hResultError;
			}

			// reply with the appropriate contents - go back to beginning of operand[0]
			hResult = dsGotoMarker(pStream, DSMARKER_OPERAND_0);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsSwitchMode(pStream, dsMODE_WRITE);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsWrite8Bits(pStream, power_state);
			if (hResult != NO_ERROR) return hResultError;

			if (pHeader->ctype == AVC_CTYPE_STATUS)
			{
				hResult = E_PKT_AVC_STABLE;
			}
			else // ctype == AVC_CTYPE_NOTIFY
			{
				hResult = avcUnitPowerNotifyAdd(packetBlock, unitpowerstate);
				if (hResult != NO_ERROR) return hResultError;

				hResult = E_PKT_AVC_INTERIM;
			}
			break;

		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			// don't even bother parsing
			hResult = E_PKT_AVC_IMPLEMENTED;
			break;
		
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcCmdUnitInfo(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT				hResult = NO_ERROR;
	HRESULT				hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	uint32				i;
	BYTE				parse8;
	AVC_ADDRESS_SUBUNIT	subunit;
	uint32				company_ID;
	BYTE				markerID = 0x07;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_CTYPE_STATUS:	// M : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			// operands[0] through [4] must be all FF
			hResultError = E_PKT_AVC_REJECTED;

			for (i = 0; i < 5; i++)
			{
				hResult = dsRead8Bits(pStream, &parse8);
				if (hResult != NO_ERROR) return hResultError;
				if (parse8 != 0xFF) return hResultError;
			}

			// incoming status command has parsed successfully

			// reply with the appropriate contents - go back to beginning of operand[0]
			hResult = dsGotoMarker(pStream, DSMARKER_OPERAND_0);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsSwitchMode(pStream, dsMODE_WRITE);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsWrite8Bits(pStream, markerID);
			if (hResult != NO_ERROR) return hResultError;

			hResult = avcSubunitGetUnitInfoAddress(&subunit);
			if (hResult != NO_ERROR) return hResultError;

			hResult = avcSubunitEncodeSubunit(pStream, &subunit);
			if (hResult != NO_ERROR) return hResultError;

			hResult = avcSubunitGetUnitInfoCompanyID(&company_ID);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsWrite24Bits(pStream, company_ID);
			if (hResult != NO_ERROR) return hResultError;

#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				cliPrintf("marker id:\t%i\n\r", markerID);
				avcUnitCliPrintSubunitType(subunit.subunit_type);
				cliPrintf("unit:\t\t%i (vendor-dependent)\n\r", subunit.subunit_ID);
				avcUnitCliPrintCompanyID(company_ID);
			}
#endif //_SYSDEBUG

			hResult = E_PKT_AVC_STABLE;
			break;

		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			// don't even bother parsing
			hResult = E_PKT_AVC_IMPLEMENTED;
			break;
		
		case AVC_CTYPE_NOTIFY:	// - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		case AVC_CTYPE_CONTROL: // - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcCmdSubunitInfo(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	uint32			zeroBit = 0;
	uint32			page = 0;
	uint32			extension_code;
	uint32			page_data = 0;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_CTYPE_STATUS:	// M : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			hResultError = E_PKT_AVC_REJECTED;

			// page header
			hResult = dsReadBits(pStream, 1, &zeroBit);
			if (hResult != NO_ERROR) return hResultError;
			if (zeroBit != 0) return hResultError;

			hResult = dsReadBits(pStream, 3, &page);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsReadBits(pStream, 1, &zeroBit);
			if (hResult != NO_ERROR) return hResultError;
			if (zeroBit != 0) return hResultError;

			hResult = dsReadBits(pStream, 3, &extension_code);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsRead32Bits(pStream, &page_data);
			if (hResult != NO_ERROR) return hResultError;
			if (page_data != 0xFFFFFFFF) return hResultError;

			// incoming status command has parsed successfully

			// reply with the appropriate contents - go back to beginning of operand[0]
			hResult = dsGotoMarker(pStream, DSMARKER_OPERAND_0);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsSwitchMode(pStream, dsMODE_WRITE);
			if (hResult != NO_ERROR) return hResultError;

			// write the page_data to the packet block
			hResult = avcSubunitGetSubunitInfoPageData(pStream, page, extension_code);
			if (hResult != NO_ERROR) return hResult;

			hResult = E_PKT_AVC_STABLE;
			break;

		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			// don't even bother parsing
			hResult = E_PKT_AVC_IMPLEMENTED;
			break;
		
		case AVC_CTYPE_NOTIFY:	// - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		case AVC_CTYPE_CONTROL: // - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcCmdReserve(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	BYTE			priority = 0;
	BYTE			text_data[AVC_RESERVE_TEXT_SIZE];
	uint32			i;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_CTYPE_CONTROL: // O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			hResultError = E_PKT_AVC_REJECTED;

			hResult = dsRead8Bits(pStream, &priority);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsReadChunk(pStream, AVC_RESERVE_TEXT_SIZE, (char *) text_data);
			if (hResult != NO_ERROR) return hResultError;

			hResult = avcUnitReserveControl(packetBlock, &pHeader->addrSubunit, priority, text_data);
			if (hResult != NO_ERROR) return hResultError;

			hResult = E_PKT_AVC_ACCEPTED;

#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				avcUnitCliPrintReserve(priority, (char *) text_data);
			}
#endif //_SYSDEBUG
			break;

		case AVC_CTYPE_STATUS:	// O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		case AVC_CTYPE_NOTIFY:	// R : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
				
			hResultError = E_PKT_AVC_REJECTED;

			hResult = dsRead8Bits(pStream, &priority);
			if (hResult != NO_ERROR) return hResultError;
			if (priority != 0xFF) return hResultError;

			hResult = dsReadChunk(pStream, AVC_RESERVE_TEXT_SIZE, (char *) text_data);
			if (hResult != NO_ERROR) return hResultError;
			
			for (i = 0; i < AVC_RESERVE_TEXT_SIZE; i++)
			{
				if (text_data[i] != 0xFF) return hResultError;
			}

			// incoming status command has parsed successfully

#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				avcUnitCliPrintReserve(priority, (char *) text_data);
			}
#endif //_SYSDEBUG

			// reply with the appropriate contents - go back to beginning of operand[0]
			hResult = dsGotoMarker(pStream, DSMARKER_OPERAND_0);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsSwitchMode(pStream, dsMODE_WRITE);
			if (hResult != NO_ERROR) return hResultError;

			hResult = avcUnitReserveStatus(&pHeader->addrSubunit, &priority, text_data);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsWrite8Bits(pStream, priority);										// operand[0]
			if (hResult != NO_ERROR) hResultError = E_PKT_AVC_REJECTED;
			hResult = dsWriteChunk(pStream, AVC_RESERVE_TEXT_SIZE, (char *) text_data);		// operand[1,..,12]
			if (hResult != NO_ERROR) hResultError = E_PKT_AVC_REJECTED;

			if (pHeader->ctype == AVC_CTYPE_STATUS)
			{
				hResult = E_PKT_AVC_STABLE;
			}
			else // pHeader->ctype == AVC_CTYPE_NOTIFY
			{
				// store notify with current reservation state
				hResult = avcUnitReserveNotify(packetBlock, &pHeader->addrSubunit, priority, text_data);
				if (hResult != NO_ERROR) return hResultError;

				hResult = E_PKT_AVC_INTERIM;
			}
			break;

		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			// don't even bother parsing
			hResult = E_PKT_AVC_IMPLEMENTED;
			break;
		
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcCmdVersion(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT				hResult = NO_ERROR;
	HRESULT				hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	uint32				i;
	BYTE				subfunction;
	BYTE				version_information;
	BYTE				su_dependent_field[AVC_VERSION_SU_DEP_FIELD_SIZE];

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_CTYPE_STATUS:	// 0 : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			// operands[0] through [4] must be all FF
			hResultError = E_PKT_AVC_REJECTED;

			hResult = dsRead8Bits(pStream, &subfunction);
			if (hResult != NO_ERROR) return hResultError;
			
			// subfunction: 0xff: get latest version or support level of the specified version (0x00-0xfe: reserved)
			if (subfunction != 0xFF) return hResultError;

			hResult = dsRead8Bits(pStream, &version_information);
			if (hResult != NO_ERROR) return hResultError;
			
			for (i = 0; i < AVC_VERSION_SU_DEP_FIELD_SIZE; i++)
			{
				hResult = dsRead8Bits(pStream, &(su_dependent_field[i]));
				if (hResult != NO_ERROR) return hResultError;
				if (su_dependent_field[i] != 0xFF) return hResultError;
			}

			// incoming status command has parsed successfully

			// version_information: 0xFF: get latest version to which the unit or subunit complies
			// version_information: 0x00-0xFE: get the support level of the specified version
			hResult = avcSubunitGetSubunitVersionInfo(&pHeader->addrSubunit, &version_information, su_dependent_field);
			if (hResult != NO_ERROR) return E_PKT_AVC_NOT_IMPLEMENTED;

#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				// avcUnitCliPrintSubunitType(subunit->subunit_type);
				avcUnitCliPrintVersionInfo(subfunction, version_information, su_dependent_field);
			}
#endif //_SYSDEBUG

			// reply with the appropriate contents - go back to beginning of operand[0]
			hResult = dsGotoMarker(pStream, DSMARKER_OPERAND_0);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsSwitchMode(pStream, dsMODE_WRITE);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsWrite8Bits(pStream, subfunction);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsWrite8Bits(pStream, version_information);
			if (hResult != NO_ERROR) return hResultError;

			for (i = 0; i < AVC_VERSION_SU_DEP_FIELD_SIZE; i++)
			{
				hResult = dsWrite8Bits(pStream, su_dependent_field[i]);
				if (hResult != NO_ERROR) return hResultError;
			}

			hResult = E_PKT_AVC_STABLE;
			break;

		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			// don't even bother parsing
			hResult = E_PKT_AVC_IMPLEMENTED;
			break;
		
		case AVC_CTYPE_NOTIFY:	// - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		case AVC_CTYPE_CONTROL: // - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcCmdVendorDependent(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT		hResult = NO_ERROR;
	HRESULT		hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	uint32		company_ID = 0;
	uint32		unit_company_ID = 0;

	hResult = dsRead24Bits(pStream, &company_ID);
	if (hResult != NO_ERROR) return hResultError;

	if (company_ID == kTCAT_DICE_VENDOR_ID)
	{
		return avcUnitVendorDependentTC(pHeader, packetBlock, pStream);
	}

	hResult = avcSubunitGetUnitInfoCompanyID(&unit_company_ID);
	if (hResult != NO_ERROR) return hResultError;

	if (company_ID == unit_company_ID)
	{
		return avcUnitVendorDependent(pHeader, packetBlock, pStream, company_ID);
	}

	return hResult;
}

