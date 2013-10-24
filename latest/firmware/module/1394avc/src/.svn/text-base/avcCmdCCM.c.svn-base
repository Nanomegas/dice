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

	Implementation of avc connection and compatibility management (CCM) commands
	AV/C Connection and Compatibility Management Specification (Ver. 1.0) section 7
	
										ctype			target
										C,S,N			U,S
		SIGNAL SOURCE				(?)	M,M,R			+,-
		INPUT SELECT				(?)	M,M,-			+,-
		OUTPUT PRESET				(?)	O,O,-			+,-
		CCM PROFILE						-,O,-			+,-

			ctype:
			C:CONTROL, S:STATUS, N:NOTIFY
			M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			target
			U:UNIT, S:SUBUNIT

  Revisions:
		created 26/05/2003 LM

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "avcCmdCCM.h"
#include "avcUnitCli.h"
#include "avcUnitUtils.h"
#include "avcUtils.h"
#include "avcUnitSignalSource.h"
#include "avcDefs.h"
#include "avcDriver.h"
#include "targetPlugs.h"
#include "targetBoard.h"
#include "cmp.h"
#include "cli.h"

HRESULT avcCmdSignalSource(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT		hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	HRESULT		hResult = NO_ERROR;
	uint8		inByte = 0;
	uint16		inVal = 0;
	uint8		dest_su = 0;
	uint8		dest_plug_id = 0;
	uint8		source_su = 0;
	uint8		source_plug_id = 0;
	uint8		status = 0;
	AVC_SIGNAL_PATH	*path = NULL;

	switch (pHeader->ctype)
	{
		case AVC_CTYPE_NOTIFY:	// - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
			hResultError = E_PKT_AVC_REJECTED;
			break;

		case AVC_CTYPE_STATUS:	// - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
			hResultError = E_PKT_AVC_REJECTED;

			// operand[0,1]: 0xFFFF
			hResult = dsRead16Bits(pStream, &inVal);
			if (hResult != NO_ERROR) return hResultError;
			if (inVal != 0xFFFF) return hResultError;

			// operand[2]: 0xFE
			hResult = dsRead8Bits(pStream, &inByte);
			if (hResult != NO_ERROR) return hResultError;
			if (inByte != 0xFE) return hResultError;
			
			// operand[3]: signal_dest_su
			hResult = dsRead8Bits(pStream, &dest_su);
			if (hResult != NO_ERROR) return hResultError;

			// operand[4]: signal_dest_plug_id
			hResult = dsRead8Bits(pStream, &dest_plug_id);
			if (hResult != NO_ERROR) return hResultError;

			// incoming status command has parsed successfully

			SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "Sig Src status: dest_su: 0x%02x, dest_plug_id 0x%02x\n\r", dest_su, dest_plug_id);
			hResult = targetSignalSourceFindPath(dest_plug_id, dest_su, &source_plug_id, &source_su, &status, &path);
			if (NO_ERROR != hResult)
			{
				return E_PKT_AVC_REJECTED;
			}

			// reply with the appropriate contents - go back to beginning of operand[0]
			hResult = dsGotoMarker(pStream, DSMARKER_OPERAND_0);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsSwitchMode(pStream, dsMODE_WRITE);
			if (hResult != NO_ERROR) return hResultError;

			// operand[0]: 3:output_status, 1:conv, 4:signal_status
			hResult = dsWrite8Bits(pStream, status);
			if (hResult != NO_ERROR) return hResultError;

			// operand[1]: source_su
			hResult = dsWrite8Bits(pStream, source_su);
			if (hResult != NO_ERROR) return hResultError;
			
			// operand[2]: source_plug_id
			hResult = dsWrite8Bits(pStream, source_plug_id);
			if (hResult != NO_ERROR) return hResultError;
			
			// operand[3]: dest_su
			hResult = dsWrite8Bits(pStream, dest_su);
			if (hResult != NO_ERROR) return hResultError;
			
			// operand[4]: dest_plug_id
			hResult = dsWrite8Bits(pStream, dest_plug_id);
			if (hResult != NO_ERROR) return hResultError;
			
			if (pHeader->ctype == AVC_CTYPE_STATUS)
			{
				hResult = E_PKT_AVC_STABLE;
			}
			break;

		case AVC_CTYPE_SPECIFIC_INQUIRY:	// asking if a plug is a possible clock sync source
		case AVC_CTYPE_CONTROL:				// setting the clock sync source to a plug
			{
				BYTE src_su = 0;
				BYTE src_plug_id = 0;
				BYTE dst_su = 0;
				BYTE dst_plug_id = 0;

				hResultError = E_PKT_AVC_NOT_IMPLEMENTED;

				// operand[1]: 0x0F
				hResult = dsRead8Bits(pStream, &inByte);
				if (hResult != NO_ERROR) return hResultError;
				
				// operand[2]: signal_src subunit
				hResult = dsRead8Bits(pStream, &src_su);
				if (hResult != NO_ERROR) return hResultError;

				// operand[3]: signal_src plug id
				hResult = dsRead8Bits(pStream, &src_plug_id);
				if (hResult != NO_ERROR) return hResultError;

				// operand[4]: signal_dest subunit
				hResult = dsRead8Bits(pStream, &dst_su);
				if (hResult != NO_ERROR) return hResultError;
				
				// operand[5]: signal_dest plug id
				hResult = dsRead8Bits(pStream, &dst_plug_id);
				if (hResult != NO_ERROR) return hResultError;

				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "Sig Src s_su 0x%02x, s_id 0x%02x, d_su 0x%02x, d_id 0x%02x\n\r", src_su, src_plug_id, dst_su, dst_plug_id);

				if ( avcDriverPlugIsSyncPath(dst_su, dst_plug_id, src_su, src_plug_id) )
				{
					// host is asking if it can connect a unit input plug to a subunit output plug (i.e. for sync)
					hResult = dsGotoMarker(pStream, DSMARKER_OPERAND_0);
					if (hResult != NO_ERROR) return hResultError;

					hResult = dsSwitchMode(pStream, dsMODE_WRITE);
					if (hResult != NO_ERROR) return hResultError;

					hResult = dsWrite8Bits(pStream, 0x00); // accepted
					if (hResult != NO_ERROR) return hResultError;

					hResult = dsWrite8Bits(pStream, 0xff);
					if (hResult != NO_ERROR) return hResultError;

					if (AVC_CTYPE_SPECIFIC_INQUIRY == pHeader->ctype)
					{
						SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, " accepted clock source inquiry, src_plug_id: %d\n\r", src_plug_id);
						return E_PKT_AVC_ACCEPTED; 
					}
					else if (AVC_CTYPE_CONTROL == pHeader->ctype)		// set clock source
					{
						hResult = avcDriverSetClockSrc(src_su, src_plug_id);
						if (hResult != NO_ERROR) return hResultError;

						SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, " accepted set clock source, src_plug_id: %d\n\r", src_plug_id);
					}
					return E_PKT_AVC_ACCEPTED; 
				}

				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, " rejected clock source inquiry\n\r");
				hResult = E_PKT_AVC_REJECTED;
			}
			break;

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

HRESULT avcCmdInputSelect(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT		hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	HRESULT		hResult = NO_ERROR;

	UNUSED_ARG(pHeader);
	UNUSED_ARG(packetBlock);
	UNUSED_ARG(pStream);

	hResult = hResultError;
/*
	BYTE		channel = 0;
	uint16		nodeAddr;
	BYTE		oPCR;
	BOOL		bBroadCast = FALSE;
	BOOL		bUsingChannel = FALSE;

	switch (pHeader->ctype)
	{
		case AVC_CTYPE_CONTROL: // M : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			hResult = avcSubunitDecodeConnection(pStream, &connection);
			if (hResult != NO_ERROR) return hResultError;

#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				avcUnitCliPrintConnect(&connection);
			}
#endif //_SYSDEBUG

			hResultError = E_PKT_AVC_REJECTED;

			hResult = avcSubunitValidatePlugs(&connection);
			if (hResult != NO_ERROR) return hResultError;

			hResult = avcSubunitConnectPlugs(&connection);
			if (hResult != NO_ERROR) return hResultError;

			// update pStream with actual plugIDs since plugIDs may have been "any available plug"

			hResult = avcSubunitEncodeConnection(pStream, &connection);
			if (hResult != NO_ERROR) return hResultError;

			hResult = E_PKT_AVC_ACCEPTED;
			break;

		case AVC_CTYPE_STATUS:	// M : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			hResult = avcSubunitDecodeConnection(pStream, &connection);
			if (hResult != NO_ERROR) return hResultError;

			if (connection.bPerm != TRUE || connection.bLock != TRUE) return hResultError;

			// Exactly one of the Src/Dst must be 0xFFFE (status command for Dst/Src plug)
			count = avcSubunitPlugIsInvalidPlug(&connection.plugs.src);
			count += avcSubunitPlugIsInvalidPlug(&connection.plugs.dst);

			if (count != 1) return hResultError;
			
			// valid status command for either Src/Dst plug, so specify what connects to it

#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				avcUnitCliPrintConnect(&connection);
			}
#endif //_SYSDEBUG

			avcSubunitCopyConnection(&command, &connection);		// make a copy of connect command
			avcSubunitFindPlug(&connection);						// Overwrites connection with the missing fields

			// update pStream with actual lock/perm and Src/Dst subunit type/ID or plugID or FE (or FD if Src and multiple Plugs)

			hResult = avcSubunitEncodeConnection(pStream, &connection);
			if (hResult != NO_ERROR) return hResultError;

			hResult = E_PKT_AVC_STABLE;
			break;

		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			// don't even bother parsing
			hResult = E_PKT_AVC_IMPLEMENTED;
			break;
		
		case AVC_CTYPE_NOTIFY:	// - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		default:
			hResult = hResultError;
			break;
	}
*/
	return hResult;
}

HRESULT avcCmdOutputPreset(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT		hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	HRESULT		hResult = NO_ERROR;

	UNUSED_ARG(pHeader);
	UNUSED_ARG(packetBlock);
	UNUSED_ARG(pStream);

	hResult = hResultError;
/*
	AVC_CONNECTION	connection;
	AVC_CONNECTION	command;
	uint32			count = 0;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_CTYPE_CONTROL: // O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			hResult = avcSubunitDecodeConnection(pStream, &connection);
			if (hResult != NO_ERROR) return hResultError;

#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				avcUnitCliPrintConnect(&connection);
			}
#endif //_SYSDEBUG

			hResultError = E_PKT_AVC_REJECTED;

			hResult = avcSubunitValidatePlugs(&connection);
			if (hResult != NO_ERROR) return hResultError;

			hResult = avcSubunitConnectPlugs(&connection);
			if (hResult != NO_ERROR) return hResultError;

			// update pStream with actual plugIDs since plugIDs may have been "any available plug"

			hResult = avcSubunitEncodeConnection(pStream, &connection);
			if (hResult != NO_ERROR) return hResultError;

			hResult = E_PKT_AVC_ACCEPTED;
			break;

		case AVC_CTYPE_STATUS:	// O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			hResult = avcSubunitDecodeConnection(pStream, &connection);
			if (hResult != NO_ERROR) return hResultError;

			if (connection.bPerm != TRUE || connection.bLock != TRUE) return hResultError;

			// Exactly one of the Src/Dst must be 0xFFFE (status command for Dst/Src plug)
			count = avcSubunitPlugIsInvalidPlug(&connection.plugs.src);
			count += avcSubunitPlugIsInvalidPlug(&connection.plugs.dst);

			if (count != 1) return hResultError;
			
			// valid status command for either Src/Dst plug, so specify what connects to it

#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				avcUnitCliPrintConnect(&connection);
			}
#endif //_SYSDEBUG

			avcSubunitCopyConnection(&command, &connection);		// make a copy of connect command
			avcSubunitFindPlug(&connection);						// Overwrites connection with the missing fields

			// update pStream with actual lock/perm and Src/Dst subunit type/ID or plugID or FE (or FD if Src and multiple Plugs)

			hResult = avcSubunitEncodeConnection(pStream, &connection);
			if (hResult != NO_ERROR) return hResultError;

			hResult = E_PKT_AVC_STABLE;
			break;

		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			// don't even bother parsing
			hResult = E_PKT_AVC_IMPLEMENTED;
			break;
		
		case AVC_CTYPE_NOTIFY:	// - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		default:
			hResult = hResultError;
			break;
	}
*/
	return hResult;
}

HRESULT avcCmdCCMProfile(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;

	UNUSED_ARG(pHeader);
	UNUSED_ARG(packetBlock);
	UNUSED_ARG(pStream);
/*
	HRESULT			hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	AVC_CONNECTION	connection;
	AVC_CONNECTION	command;
	uint32			count = 0;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_CTYPE_CONTROL: // O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			hResult = avcSubunitDecodeConnection(pStream, &connection);
			if (hResult != NO_ERROR) return hResultError;

#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				avcUnitCliPrintConnect(&connection);
			}
#endif //_SYSDEBUG

			hResultError = E_PKT_AVC_REJECTED;

			hResult = avcSubunitValidatePlugs(&connection);
			if (hResult != NO_ERROR) return hResultError;

			hResult = avcSubunitConnectPlugs(&connection);
			if (hResult != NO_ERROR) return hResultError;

			// update pStream with actual plugIDs since plugIDs may have been "any available plug"

			hResult = avcSubunitEncodeConnection(pStream, &connection);
			if (hResult != NO_ERROR) return hResultError;

			hResult = E_PKT_AVC_ACCEPTED;
			break;

		case AVC_CTYPE_STATUS:	// O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			hResult = avcSubunitDecodeConnection(pStream, &connection);
			if (hResult != NO_ERROR) return hResultError;

			if (connection.bPerm != TRUE || connection.bLock != TRUE) return hResultError;

			// Exactly one of the Src/Dst must be 0xFFFE (status command for Dst/Src plug)
			count = avcSubunitPlugIsInvalidPlug(&connection.plugs.src);
			count += avcSubunitPlugIsInvalidPlug(&connection.plugs.dst);

			if (count != 1) return hResultError;
			
			// valid status command for either Src/Dst plug, so specify what connects to it

#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				avcUnitCliPrintConnect(&connection);
			}
#endif //_SYSDEBUG

			avcSubunitCopyConnection(&command, &connection);		// make a copy of connect command
			avcSubunitFindPlug(&connection);						// Overwrites connection with the missing fields

			// update pStream with actual lock/perm and Src/Dst subunit type/ID or plugID or FE (or FD if Src and multiple Plugs)

			hResult = avcSubunitEncodeConnection(pStream, &connection);
			if (hResult != NO_ERROR) return hResultError;

			hResult = E_PKT_AVC_STABLE;
			break;

		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			// don't even bother parsing
			hResult = E_PKT_AVC_IMPLEMENTED;
			break;
		
		case AVC_CTYPE_NOTIFY:	// - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		default:
			hResult = hResultError;
			break;
	}
*/
	return hResult;
}

