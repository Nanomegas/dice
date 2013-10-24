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

	Implementation of avc connection commands
	AV/C Digital Interface Command Set General (Ver. 4.1) section 12
	
										ctype			target
										C,S,N			U,S
		PLUG INFO						-,O,-			+,+
		CHANNEL USAGE					-,R,R			+.-
		CONNECT							O,O,R			+,-
	%	CONNECT AV						O,O,O			+,-	
		CONNECTIONS						-,O,-			+,-
		DIGITAL INPUT					O,O,-			+,-
		DIGITAL OUTPUT					O,O,-			+,-
		DISCONNECT						O,-,-			+,-
	%	DISCONNECT AV					O,-,-			+,-
		INPUT PLUG SIGNAL FORMAT		O,R,O			+,-
		OUTPUT PLUG SIGNAL FORMAT		O,R,O			+,-
		GENERAL BUS SETUP				O,O,O			+,-

			ctype:
			C:CONTROL, S:STATUS, N:NOTIFY
			M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			target
			U:UNIT, S:SUBUNIT

  Revisions:
		created 08/04/2003 LM

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "avcCmdConnection.h"
#include "avcUnitCli.h"
#include "avcUnitUtils.h"
#include "avcUtils.h"
#include "avcUnitChannelUsage.h"
#include "avcUnitConnect.h"
#include "avcUnitConnectAV.h"
#include "avcUnitSignalFormat.h"				// inputPlugSignalFormat, outputPlugSignalFormat
#include "avcDefs.h"
#include "avc.h"
#include "avcDriver.h"
#include "targetPlugs.h"
#include "cmp.h"
#include "cli.h"

HRESULT avcCmdPlugInfo(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT		hResult = NO_ERROR;
	HRESULT		hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	BYTE		subfunction = 0;
	BYTE		plugs[AVC_PLUG_INFO_PLUGS_SIZE];
	uint32		i;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_CTYPE_STATUS:	// O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			// operand[0]: subfunction
			hResult = dsRead8Bits(pStream, &subfunction);
			if (hResult != NO_ERROR) return hResultError;

			for (i = 0; i < AVC_PLUG_INFO_PLUGS_SIZE; i++)
			{
				hResult = dsRead8Bits(pStream, &plugs[i]);
				if (hResult != NO_ERROR) return hResultError;
				if (plugs[i] != 0xFF) return hResultError;
			}

			if (avcSubunitIsUnit(&pHeader->addrSubunit) == FALSE)	// Plug Info on Subunit
			{
				if (subfunction == 0x00)		// Subfunction: must be 0x00
				{
					// get the data from the target plug manager module
					hResult = targetGetSubunitPlugs(&pHeader->addrSubunit, &plugs[1] /*numSrcPlugs*/, &plugs[0] /*numDstPlugs*/);
					if (hResult != NO_ERROR) return hResultError;
				}
				else
				{
					return hResultError;
				}
			}
			else													// Plug Info on Unit
			{
				if (subfunction == 0x00)		// Subfunction: Serial Bus Isochronous and External Plug
				{
					// get the data from the target plug manager module
					plugs[0] = (BYTE) plugsGetNumIsochInPlugs();		// numSBIsochInPlugs
					plugs[1] = (BYTE) plugsGetNumIsochOutPlugs();		// numSBIsochOutPlugs
					plugs[2] = (BYTE) plugsGetNumExtInPlugs();			// numExtInPlugs
					plugs[3] = (BYTE) plugsGetNumExtOutPlugs();			// numExtOutPlugs
				}
				else if (subfunction == 0x01)	// Subfunction: Serial Bus Asynchronous Plug
				{
					// get the data from the target plug manager module
					plugs[0] = (BYTE) plugsGetNumAsyncInPlugs();		// numSBAsyncInPlugs
					plugs[1] = (BYTE) plugsGetNumAsyncOutPlugs();		// numSBAsyncOutPlugs
					plugs[2] = 0xFF;
					plugs[3] = 0xFF;
				}
				else if (subfunction <= 0x3F)	// Subfunction: (0x02-0x3F) Reserved
				{
					return hResultError;
				}
				else if (subfunction <= 0x7F)	// Subfunction: (0x40-0x7F) General Bus Plug
				{
												// 0x40:			bluetooth
												// 0x41,..,0x7f:	reserved
					return hResultError;
				}
				else							// Subfunction: (0x80-0xFF) Reserved
				{
					return hResultError;
				}
			}			
			// incoming status command has parsed successfully

#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				avcUnitCliPrintPlugInfo(&pHeader->addrSubunit, subfunction, plugs);
			}
#endif //_SYSDEBUG

			// reply with the appropriate contents - go back to beginning of operand[0]
			hResult = dsGotoMarker(pStream, DSMARKER_OPERAND_0);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsSwitchMode(pStream, dsMODE_WRITE);
			if (hResult != NO_ERROR) return hResultError;

			// operand[0]: unchanged
			hResult = dsWrite8Bits(pStream, subfunction);
			if (hResult != NO_ERROR) return hResultError;
			
			for (i = 0; i < AVC_PLUG_INFO_PLUGS_SIZE; i++)
			{
				hResult = dsWrite8Bits(pStream, plugs[i]);
				if (hResult != NO_ERROR) return hResultError;
			}

			hResult = E_PKT_AVC_STABLE;
			break;

		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			// don't even bother parsing
			hResult = E_PKT_AVC_IMPLEMENTED;
			break;
		
		case AVC_CTYPE_CONTROL: // - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		case AVC_CTYPE_NOTIFY:	// - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcCmdChannelUsage(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT		hResult = NO_ERROR;
	HRESULT		hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	BYTE		channel = 0;
	uint16		nodeAddr;
	BYTE		oPCRNumber;
	BOOL		bBroadCast = FALSE;
	BOOL		bUsingChannel = FALSE;

	switch (pHeader->ctype)
	{
		case AVC_CTYPE_STATUS:	// R : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		case AVC_CTYPE_NOTIFY:	// R : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			hResultError = E_PKT_AVC_REJECTED;

			// operand[0]: channel
			hResult = dsRead8Bits(pStream, &channel);
			if (hResult != NO_ERROR) return hResultError;
			if (channel > 0x3F) return hResultError;

			// operand[1,..,2]: nodeID
			hResult = dsRead16Bits(pStream, &nodeAddr);
			if (hResult != NO_ERROR) return hResultError;
			if (nodeAddr != 0xFFFF) return hResultError;
			
			// operand[3]: oPCRNumber
			hResult = dsRead8Bits(pStream, &oPCRNumber);
			if (hResult != NO_ERROR) return hResultError;
			if (oPCRNumber != 0xFF) return hResultError;

			// incoming status command has parsed successfully

			bBroadCast = pbPacketIsBroadcast(packetBlock);
	
			bUsingChannel = avcSubunitChannelIsConnected(channel, &nodeAddr, &oPCRNumber);

#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				avcUnitCliPrintChannelUsage(channel, nodeAddr, oPCRNumber);
			}
#endif //_SYSDEBUG

			if (bBroadCast == TRUE &&
				bUsingChannel == FALSE) return NO_ERROR;	//avcHandler shouldn't send a response either

			// reply with the appropriate contents - go back to beginning of operand[0]
			hResult = dsGotoMarker(pStream, DSMARKER_OPERAND_0);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsSwitchMode(pStream, dsMODE_WRITE);
			if (hResult != NO_ERROR) return hResultError;

			// operand[0]: channel (unchanged)
			hResult = dsWrite8Bits(pStream, channel);
			if (hResult != NO_ERROR) return hResultError;

			// operand[1,..,2]: nodeID
			hResult = dsWrite16Bits(pStream, nodeAddr);
			if (hResult != NO_ERROR) return hResultError;
			
			// operand[3]: oPCRNumber
			hResult = dsWrite8Bits(pStream, oPCRNumber);
			if (hResult != NO_ERROR) return hResultError;

			if (pHeader->ctype == AVC_CTYPE_STATUS)
			{
				hResult = E_PKT_AVC_STABLE;
			}
			else // ctype == AVC_CTYPE_NOTIFY
			{
				if (bUsingChannel == FALSE) return hResultError;

				hResult = avcUnitChannelUsageNotifyAdd(packetBlock, channel, nodeAddr, oPCRNumber);
				if (hResult != NO_ERROR) return hResultError;

				hResult = E_PKT_AVC_INTERIM;
			}
			break;

		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			// don't even bother parsing
			hResult = E_PKT_AVC_IMPLEMENTED;
			break;
		
		case AVC_CTYPE_CONTROL: // - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcCmdConnect(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	AVC_CONNECTION	connection;
	AVC_CONNECTION	command;
	uint32			count = 0;

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

			hResult = dsGotoMarker(pStream, DSMARKER_OPERAND_0);
			if (hResult != NO_ERROR) return hResult;
			hResult = dsSwitchMode(pStream, dsMODE_WRITE);
			if (hResult != NO_ERROR) return hResult;

			hResult = avcSubunitEncodeConnection(pStream, &connection);
			if (hResult != NO_ERROR) return hResultError;

			hResult = E_PKT_AVC_ACCEPTED;
			break;

		case AVC_CTYPE_STATUS:	// O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		case AVC_CTYPE_NOTIFY:	// R : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

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

			if (pHeader->ctype == AVC_CTYPE_NOTIFY && 
				connection.bPerm == TRUE)							// notify on perm connections not allowed
			{
				return hResultError;
			}

			// update pStream with actual lock/perm and Src/Dst subunit type/ID or plugID or FE (or FD if Src and multiple Plugs)

			hResult = dsGotoMarker(pStream, DSMARKER_OPERAND_0);
			if (hResult != NO_ERROR) return hResult;
			hResult = dsSwitchMode(pStream, dsMODE_WRITE);
			if (hResult != NO_ERROR) return hResult;

			hResult = avcSubunitEncodeConnection(pStream, &connection);
			if (hResult != NO_ERROR) return hResultError;

			if (pHeader->ctype == AVC_CTYPE_STATUS)
			{
				hResult = E_PKT_AVC_STABLE;
			}
			else // ctype == AVC_CTYPE_NOTIFY
			{
				hResult = avcUnitConnectNotifyAdd(packetBlock, &command, &connection);
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

HRESULT avcCmdConnectAV(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT				hResult = NO_ERROR;
	HRESULT				hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
#if 0
	AVC_ADDRESS_SUBUNIT addrSubunit[4];
	uint32				type[4];
	uint32				i;
	BYTE				ignore;
#endif

	UNUSED_ARG(pStream);
	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_CTYPE_CONTROL: // O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
#if 0
			// operand[0]: [video_src_type,audio_src_type,video_dst_type,audio_dst_type]
			for (i = 0; i < 4; i++)
			{
				hResult = dsReadBits(pStream, 2, &type[i]);
				if (hResult != NO_ERROR) return hResultError;
			}

			for (i = 0; i < 4; i++)
			{
				switch (type)
				{
					case 0:	// subunit
						hResult = avcSubunitDecodeSubunit(pStream, &addrSubunit[i]);
						if (hResult != NO_ERROR) return hResult;
						break;
					
					case 1:	// ignore
						hResult = dsRead8Bits(pStream, &ignore);
						break;
					
					case 2:	// isoch or extern plug
						break;
					
					case 3:	// reserved
						return hResultError;
				}
			}

			//....

			for (i = 0; i < 4; i++)
			{
				hResult = dsWriteBits(pStream, 2, &type[i]);
				if (hResult != NO_ERROR) return hResultError;
			}

			for (i = 0; i < 4; i++)
			{
				switch (type)
				{
					case 0:	// subunit
						hResult = avcSubunitEncodeSubunit(pStream, &addrSubunit[i]);
						if (hResult != NO_ERROR) return hResult;
						break;
					
					case 1:	// ignore
						hResult = dsWrite8Bits(pStream, &ignore);
						break;
					
					case 2:	// isoch or extern plug
						break;
					
					case 3:	// reserved
						return hResultError;
				}
			}

#endif
			
			hResult = hResultError;
			break;

		case AVC_CTYPE_STATUS:	// O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
			hResult = hResultError;
			break;

		case AVC_CTYPE_NOTIFY:	// O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
			hResult = hResultError;
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

HRESULT avcCmdConnections(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	uint32			dsPos_operand0 = 0;
	uint8			total_connections = 0;
	AVC_CONNECTION	connectionArray[0xFF];
	uint32			payloadSize	= 0;
	uint32			i	= 0;
	PB*				respPacket = NULL;
	pDataStream		pStreamResp = NULL;

	switch (pHeader->ctype)
	{
		case AVC_CTYPE_STATUS:	// O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			// stream is positioned at operand[0]
			dsPos_operand0 = dsGetPosition(pStream);

			hResult = dsRead8Bits(pStream, &total_connections);
			if (hResult != NO_ERROR) return hResultError;
			if (total_connections != 0xFF) return hResultError;

			hResult = avcSubunitGetConnections(&total_connections, 0xFF, connectionArray);
			if (hResult != NO_ERROR) return hResultError;

			// Create a new (larger) packet for the response
			hResult = pbGetPayloadSize (packetBlock, &payloadSize);

			for (i = 0; i < total_connections; i++)
			{
				payloadSize += avcSubunitGetConnectionByteSize(&connectionArray[i]);
			}

			hResult = avcCreateResponsePacket(packetBlock, &respPacket, TRUE, payloadSize);
			if (hResult != NO_ERROR) return hResultError;

			// write the requested information into the packet
	
			hResult = pbGetApplicationDatastream (respPacket, &pStreamResp);
			if (hResult != NO_ERROR) goto ErrorExit;

			dsSetPosition(pStreamResp, dsPos_operand0);	// ff to operand[0]

			hResult = dsSwitchMode(pStreamResp, dsMODE_WRITE);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsWrite8Bits(pStreamResp, total_connections);
			if (hResult != NO_ERROR) goto ErrorExit;

			for (i = 0; i < total_connections; i++)
			{
				hResult = avcSubunitEncodeConnection(pStreamResp, &connectionArray[i]);
				if (hResult != NO_ERROR) goto ErrorExit;
			}

#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				avcUnitCliPrintConnections(total_connections, connectionArray);
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
		
		case AVC_CTYPE_CONTROL: // - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		case AVC_CTYPE_NOTIFY:	// - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		default:
			hResult = hResultError;
			break;
	}

	return hResult;

ErrorExit:
	avcDestroyResponsePacket(respPacket);

	return hResult;
}

HRESULT avcCmdDigitalInput(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT		hResult = NO_ERROR;
	HRESULT		hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	BYTE		connection_state = 0;
	BOOL		bEstablished = FALSE;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_CTYPE_CONTROL: // O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			hResultError = E_PKT_AVC_REJECTED;

			hResult = dsRead8Bits(pStream, &connection_state);
			if (hResult != NO_ERROR) return hResultError;

#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				cliOutputLine("Digital Input");
				avcUnitCliPrintConnectionState(connection_state);
			}
#endif //_SYSDEBUG

			if (connection_state == AVC_CONNECTION_ESTABLISH)
			{
				bEstablished = TRUE;
			}
			else if (connection_state == AVC_CONNECTION_BREAK)
			{
				bEstablished = FALSE;
			}
			else
			{
				return hResultError;
			}
			
			avcSubunitSetBroadcastInEstablished(bEstablished);

			hResult = E_PKT_AVC_ACCEPTED;
			break;

		case AVC_CTYPE_STATUS:	// O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			hResultError = E_PKT_AVC_REJECTED;

			hResult = dsRead8Bits(pStream, &connection_state);
			if (hResult != NO_ERROR) return hResultError;
			
#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				cliOutputLine("Digital Input");
				avcUnitCliPrintConnectionState(connection_state);
			}
#endif //_SYSDEBUG

			if (connection_state != AVC_CONNECTION_STATUS) return hResultError;
			
			// incoming status command has parsed successfully

			hResult = avcSubunitIsBroadcastInEstablished(&bEstablished);
			if (hResult != NO_ERROR) return hResultError;

			if (bEstablished == TRUE)
			{
				connection_state = AVC_CONNECTION_ESTABLISH;
			}
			else
			{
				connection_state = AVC_CONNECTION_BREAK;
			}

			// reply with the appropriate contents - go back to beginning of operand[0]
			hResult = dsGotoMarker(pStream, DSMARKER_OPERAND_0);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsSwitchMode(pStream, dsMODE_WRITE);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsWrite8Bits(pStream, connection_state);
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

	return hResult;
}

HRESULT avcCmdDigitalOutput(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT		hResult = NO_ERROR;
	HRESULT		hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	BYTE		connection_state = 0;
	BOOL		bEstablished = FALSE;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_CTYPE_CONTROL: // O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			hResultError = E_PKT_AVC_REJECTED;

			hResult = dsRead8Bits(pStream, &connection_state);
			if (hResult != NO_ERROR) return hResultError;

#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				cliOutputLine("Digital Output");
				avcUnitCliPrintConnectionState(connection_state);
			}
#endif //_SYSDEBUG

			if (connection_state == AVC_CONNECTION_ESTABLISH)
			{
				bEstablished = TRUE;
			}
			else if (connection_state == AVC_CONNECTION_BREAK)
			{
				bEstablished = FALSE;
			}
			else
			{
				return hResultError;
			}
			
			avcSubunitSetBroadcastOutEstablished(bEstablished);

			hResult = E_PKT_AVC_ACCEPTED;
			break;

		case AVC_CTYPE_STATUS:	// O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			hResultError = E_PKT_AVC_REJECTED;

			hResult = dsRead8Bits(pStream, &connection_state);
			if (hResult != NO_ERROR) return hResultError;
			
#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				cliOutputLine("Digital Output");
				avcUnitCliPrintConnectionState(connection_state);
			}
#endif //_SYSDEBUG

			if (connection_state != AVC_CONNECTION_STATUS) return hResultError;
			
			// incoming status command has parsed successfully

			hResult = avcSubunitIsBroadcastOutEstablished(&bEstablished);
			if (hResult != NO_ERROR) return hResultError;

			if (bEstablished == TRUE)
			{
				connection_state = AVC_CONNECTION_ESTABLISH;
			}
			else
			{
				connection_state = AVC_CONNECTION_BREAK;
			}

			// reply with the appropriate contents - go back to beginning of operand[0]
			hResult = dsGotoMarker(pStream, DSMARKER_OPERAND_0);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsSwitchMode(pStream, dsMODE_WRITE);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsWrite8Bits(pStream, connection_state);
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

	return hResult;
}

HRESULT avcCmdDisconnect(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	AVC_CONNECTION	connection;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_CTYPE_CONTROL: // O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
			hResult = avcSubunitDecodeConnection(pStream, &connection);
			if (hResult != NO_ERROR) return hResultError;

#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				cliOutputLine("Disconnect");
				avcUnitCliPrintConnect(&connection);
			}
#endif //_SYSDEBUG

			hResultError = E_PKT_AVC_REJECTED;

			hResult = avcSubunitValidatePlugs(&connection);
			if (hResult != NO_ERROR) return hResultError;

			hResult = avcSubunitDisconnectPlugs(&connection);
			if (hResult != NO_ERROR) return hResultError;

			avcUnitConnectStateChanged();

			// no update to pStream since response frame has same format

			hResult = E_PKT_AVC_ACCEPTED;
			break;

		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			// don't even bother parsing
			hResult = E_PKT_AVC_IMPLEMENTED;
			break;
		
		case AVC_CTYPE_STATUS:	// - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		case AVC_CTYPE_NOTIFY:	// - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcCmdDisconnectAV(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT		hResult = NO_ERROR;
	HRESULT		hResultError = E_PKT_AVC_NOT_IMPLEMENTED;

	UNUSED_ARG(pStream);
	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_CTYPE_CONTROL: // O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
			hResult = hResultError;
			break;

		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			// don't even bother parsing
			hResult = E_PKT_AVC_IMPLEMENTED;
			break;
		
		case AVC_CTYPE_STATUS:	// - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		case AVC_CTYPE_NOTIFY:	// - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcCmdSignalFormat(BOOL bIn, AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	BYTE			plugID = 0;
	uint32			format = 0;
	BYTE			readByte = 0;
	uint32			OldFormat = 0;
	uint32			NewFormat = 0;

	switch (pHeader->ctype)
	{
		case AVC_CTYPE_CONTROL: // O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			hResultError = E_PKT_AVC_REJECTED;

			hResult = dsRead8Bits(pStream, &plugID);
			if (hResult != NO_ERROR) return hResultError;
			if (plugID > 0x1E) return hResultError;							// AVC_PLUG_ISOCH_0,..,AVC_PLUG_ISOCH_30

			hResult = dsRead32Bits(pStream, &NewFormat);
			if (hResult != NO_ERROR) return hResultError;
			if ((NewFormat & 0xC0000000) != 0x80000000) return hResultError;	// eoh:1=1, form:1=0, fmt:6, fdf:24

			NewFormat &= 0x3FFFFFFF;											// strip eoh and form (keeping fmt and fdf)
			
#ifdef _SYSDEBUG
//			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				avcUnitCliPrintSignalFormat(bIn, plugID, NewFormat);
			}
#endif //_SYSDEBUG

			cmpGetiPCRFormat(plugID, &OldFormat);
			if (OldFormat != NewFormat)
			{
				if (bIn)
				{
					hResult = cmpSetiPCRFormat(plugID, NewFormat);
					if (hResult != NO_ERROR) return hResultError;
				}
				else
				{
					hResult = cmpSetoPCRFormat(plugID, NewFormat);
					if (hResult != NO_ERROR) return hResultError;
				}
			}

			hResult = E_PKT_AVC_ACCEPTED;
			break;

		case AVC_CTYPE_STATUS:	// R : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
		case AVC_CTYPE_NOTIFY:	// 0 : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			hResultError = E_PKT_AVC_REJECTED;

			hResult = dsRead8Bits(pStream, &plugID);
			if (hResult != NO_ERROR) return hResultError;
			if (plugID > 0x1E) return hResultError;							// AVC_PLUG_ISOCH_0,..,AVC_PLUG_ISOCH_30

			hResult = dsRead32Bits(pStream, &format);
			if (hResult != NO_ERROR) return hResultError;
			if (format != 0xFFFFFFFF) return hResultError;					// eoh:1=1, form:1=1, fmt:6:3F, fdf:24:FFFFFF
		 			 
			if (bIn)
			{
				hResult = cmpGetiPCRFormat(plugID, &format);
				if (hResult != NO_ERROR) return hResultError;
			}
			else
			{
				hResult = cmpGetoPCRFormat(plugID, &format);
				if (hResult != NO_ERROR) return hResultError;
			}

#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				avcUnitCliPrintSignalFormat(bIn, plugID, format);
			}
#endif //_SYSDEBUG

			// update pStream with actual format

			hResult = dsGotoMarker(pStream, DSMARKER_OPERAND_0);
			if (hResult != NO_ERROR) return hResult;
			hResult = dsSwitchMode(pStream, dsMODE_WRITE);
			if (hResult != NO_ERROR) return hResult;

			hResult = dsWrite8Bits(pStream, plugID);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsWrite32Bits(pStream, (format & 0x3FFFFFFF) | 0x80000000);	// eoh:1=1, form:1=0, (keeping fmt and fdf)
			if (hResult != NO_ERROR) return hResultError;

			if (pHeader->ctype == AVC_CTYPE_STATUS)
			{
				hResult = E_PKT_AVC_STABLE;
			}
			else // ctype == AVC_CTYPE_NOTIFY
			{
				hResult = avcUnitSignalFormatNotifyAdd(packetBlock, bIn, plugID, format);
				if (hResult != NO_ERROR) return hResultError;

				hResult = E_PKT_AVC_INTERIM;
			}
			break;

		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_GENERAL_INQUIRY:
			hResultError = E_PKT_AVC_REJECTED;

			hResult = dsRead8Bits(pStream, &plugID);
			if (hResult != NO_ERROR) return hResultError;
			if (plugID > 0x1E) return hResultError;							// AVC_PLUG_ISOCH_0,..,AVC_PLUG_ISOCH_30

			hResult = dsRead8Bits(pStream, &readByte);
			if (hResult != NO_ERROR) return hResultError;
			if ((readByte & 0x3f) != 0x10) return hResultError;

			hResult = dsRead8Bits(pStream, &readByte);
			if (hResult != NO_ERROR) return hResultError;
			if ((readByte < 1) || (readByte > 4))
			{
				return E_PKT_AVC_NOT_IMPLEMENTED;
			}

			hResult = E_PKT_AVC_IMPLEMENTED;
			break;
		
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcCmdGeneralBusSetup(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT		hResult = NO_ERROR;
	HRESULT		hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	BYTE		bus_type = 0;

	UNUSED_ARG(packetBlock);

	// operand[0]:
	hResult = dsRead8Bits(pStream, &bus_type);
	if (hResult != NO_ERROR) return hResultError;
	
	if (bus_type >= 0x40 &&
		bus_type <= 0x7F)		// bus_type: (0x40-0x7F) General Bus Plug
	{
								// 0x40:			bluetooth
								// 0x41,..,0x7f:	reserved
#ifdef _SYSDEBUG
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
		{
			cliPrintf("General Bus Plug (%s)\n\r", (bus_type == 0x40) ? "Bluetooth" : "reserved");
		}
#endif //_SYSDEBUG
	}
	
	switch (pHeader->ctype)
	{
		case AVC_CTYPE_CONTROL: // O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
			hResult = hResultError;
			break;

		case AVC_CTYPE_STATUS:	// O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
			hResult = hResultError;
			break;

		case AVC_CTYPE_NOTIFY:	// O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
			hResult = hResultError;
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
