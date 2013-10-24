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

	Implementation of CLI for the avc unit response 
	
  Revisions:
		created 08/04/2003 LM

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#ifdef _CLI
#include "TCTasking.h"
#include "avc.h"
#include "avcRspCli.h"
#include "avcUnitCli.h"
#include "avcUnit.h"
#include "avcUnitReserve.h"
#include "avcDefs.h"
#include "cli.h"
#include "datastream.h"
#include "lal.h"
#include "targetVendorDefs.h"

// AV/C General Commands functions

HRESULT avcRspCliPower(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultError = NO_ERROR;
	BYTE			power_state = 0;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_IMPLEMENTED:
//		case AVC_RESPONSE_STABLE:
		case AVC_RESPONSE_CHANGED:
		case AVC_RESPONSE_INTERIM:
			hResult = dsRead8Bits(pStream, &power_state);
			if (hResult != NO_ERROR) return hResultError;
			avcUnitCliPrintPowerState(power_state);
			break;
/*
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_reserved:
*/
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcRspCliUnitInfo(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT				hResult = NO_ERROR;
	HRESULT				hResultError = NO_ERROR;
	uint32				company_ID = 0;
	AVC_ADDRESS_SUBUNIT	subunit;
	BYTE				markerID;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_RESPONSE_IMPLEMENTED:
//		case AVC_RESPONSE_STABLE:

			hResult = dsRead8Bits(pStream, &markerID);
			if (hResult != NO_ERROR) return hResultError;

			hResult = avcSubunitDecodeSubunit(pStream, &subunit);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsRead24Bits(pStream, &company_ID);
			if (hResult != NO_ERROR) return hResultError;

			cliOutputLine("-----------------------------------------------------------------------");
			cliPrintf("unitinfo\n\r");
			cliPrintf("marker id:\t%i\n\r", markerID);
			avcUnitCliPrintSubunitType(subunit.subunit_type);
			cliPrintf("unit:\t\t%i (vendor-dependent)\n\r", subunit.subunit_ID);
			avcUnitCliPrintCompanyID(company_ID);
			break;
/*
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_CHANGED:
		case AVC_RESPONSE_INTERIM:
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_reserved:
*/
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcRspCliSubunitInfo(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT				hResult = NO_ERROR;
	HRESULT				hResultError = NO_ERROR;
	uint32				zeroBit = 0;
	uint32				page = 0;
	uint32				extension_code;
	AVC_ADDRESS_SUBUNIT	subunit;
	uint32				i;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_RESPONSE_IMPLEMENTED:
//		case AVC_RESPONSE_STABLE:

			hResult = dsReadBits(pStream, 1, &zeroBit);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsReadBits(pStream, 3, &page);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsReadBits(pStream, 1, &zeroBit);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsReadBits(pStream, 3, &extension_code);
			if (hResult != NO_ERROR) return hResultError;

			cliOutputLine("-----------------------------------------------------------------------");
			cliPrintf("subunit info\n\r");
			cliPrintf("page:\t\t%i\n\r", page);
			cliPrintf("extension_code:\t%i\n\r", extension_code);
			// parsing page_data: contains 4 entries from the subunit table (0xff when n/a)
			for (i = 0; i < 4; i++)
			{
				hResult = avcSubunitDecodeSubunit(pStream, &subunit);
				if (hResult != NO_ERROR) return hResultError;
				if (avcSubunitIsUnit(&subunit) == FALSE) // 0xFF
				{
					avcUnitCliPrintSubunitType(subunit.subunit_type);
					cliPrintf("max_subunit_ID:\t%i\n\r", subunit.subunit_ID);
				}
			}
			cliOutputLine("-----------------------------------------------------------------------");
			break;
/*
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_CHANGED:
		case AVC_RESPONSE_INTERIM:
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_reserved:
*/
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcRspCliReserve(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultError = NO_ERROR;
	BYTE			priority = 0;
	BYTE			text_data[AVC_RESERVE_TEXT_SIZE];

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_IMPLEMENTED:
//		case AVC_RESPONSE_STABLE:
		case AVC_RESPONSE_CHANGED:
		case AVC_RESPONSE_INTERIM:
			hResult = dsRead8Bits(pStream, &priority);
			if (hResult != NO_ERROR) return hResultError;
			hResult = dsReadChunk(pStream, AVC_RESERVE_TEXT_SIZE, (char *) text_data);
			if (hResult != NO_ERROR) return hResultError;
			avcUnitCliPrintReserve(priority, (char *) text_data);
			break;
/*
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_reserved:
*/
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcRspCliVersion(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT				hResult = NO_ERROR;
	HRESULT				hResultError = NO_ERROR;
	uint32				i;
	BYTE				subfunction;
	BYTE				version_information;
	BYTE				su_dependent_field[AVC_VERSION_SU_DEP_FIELD_SIZE];

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_RESPONSE_IMPLEMENTED:
//		case AVC_RESPONSE_STABLE:

			hResult = dsRead8Bits(pStream, &subfunction);
			if (hResult != NO_ERROR) return hResultError;
			
			hResult = dsRead8Bits(pStream, &version_information);
			if (hResult != NO_ERROR) return hResultError;
			
			for (i = 0; i < AVC_VERSION_SU_DEP_FIELD_SIZE; i++)
			{
				hResult = dsRead8Bits(pStream, &(su_dependent_field[i]));
				if (hResult != NO_ERROR) return hResultError;
			}

			avcUnitCliPrintVersionInfo(subfunction, version_information, su_dependent_field);
			break;
/*
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_CHANGED:
		case AVC_RESPONSE_INTERIM:
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_reserved:
*/
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcRspCliVendor(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT					hResult = NO_ERROR;
	HRESULT					hResultError = NO_ERROR;
	uint32					company_ID = 0;
	uint8					classId = 0;
	uint8					seqId = 0;
	uint16					cmdId = 0;
	TC_PROGRAM_ATTRIBUTES	programAttributes;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_RESPONSE_IMPLEMENTED:
//		case AVC_RESPONSE_STABLE:

			hResult = dsRead24Bits(pStream, &company_ID);
			if (hResult != NO_ERROR) return hResultError;

			cliOutputLine("-----------------------------------------------------------------------");
			cliPrintf("vendor dependent\n\r");
			avcUnitCliPrintCompanyID(company_ID);
			
			if (company_ID != kTCAT_DICE_VENDOR_ID) return hResultError;

			hResult = dsRead8Bits(pStream, &classId);		// operand[3]
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsRead8Bits(pStream, &seqId);			// operand[4]
			if (hResult != NO_ERROR) return hResultError;
			
			hResult = dsRead16Bits(pStream, &cmdId);		// operand[5,..,6]
			if (hResult != NO_ERROR) return hResultError;

			cliPrintf("classId:\t%i %s\n\r", classId, (classId == TC_VSAVC_CLASS_GENERAL) ? "TC_VSAVC_CLASS_GENERAL" : "");
			cliPrintf("seqId:\t\t%i\n\r", seqId);
			cliPrintf("cmdId:\t\t%i %s\n\r", cmdId, (cmdId == TC_VSAVC_CMD_PGM_IDENTIFY) ? "TC_VSAVC_CMD_PGM_IDENTIFY" : "");
			cliOutputLine("-----------------------------------------------------------------------");

			if (classId != TC_VSAVC_CLASS_GENERAL) return hResultError;

			if (cmdId != TC_VSAVC_CMD_PGM_IDENTIFY) return hResultError;

			hResult = dsReadChunk(pStream, sizeof(TC_PROGRAM_ATTRIBUTES), (char *) &programAttributes);
			if (hResult != NO_ERROR) return hResultError;

			avcUnitCliPrintVendorFirmwareInfo(&programAttributes);
			break;
/*
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_CHANGED:
		case AVC_RESPONSE_INTERIM:
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_reserved:
*/
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

// AV/C Connection Commands functions

HRESULT avcRspCliPlugInfo(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT		hResult = NO_ERROR;
	HRESULT		hResultError = NO_ERROR;
	BYTE		subfunction = 0;
	BYTE		plugs[AVC_PLUG_INFO_PLUGS_SIZE];
	uint32		i;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_RESPONSE_IMPLEMENTED:
//		case AVC_RESPONSE_STABLE:

			// operand[0]: subfunction
			hResult = dsRead8Bits(pStream, &subfunction);
			if (hResult != NO_ERROR) return hResultError;

			for (i = 0; i < AVC_PLUG_INFO_PLUGS_SIZE; i++)
			{
				hResult = dsRead8Bits(pStream, &plugs[i]);
				if (hResult != NO_ERROR) return hResultError;
			}

			avcUnitCliPrintPlugInfo(&pHeader->addrSubunit, subfunction, plugs);
			break;
/*
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_CHANGED:
		case AVC_RESPONSE_INTERIM:
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_reserved:
*/
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcRspCliChannelUsage(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT		hResult = NO_ERROR;
	HRESULT		hResultError = NO_ERROR;
	BYTE		channel = 0;
	uint16		nodeAddr;
	BYTE		oPCR;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_RESPONSE_IMPLEMENTED:
//		case AVC_RESPONSE_STABLE:
		case AVC_RESPONSE_CHANGED:
		case AVC_RESPONSE_INTERIM:

			// operand[0]: channel
			hResult = dsRead8Bits(pStream, &channel);
			if (hResult != NO_ERROR) return hResultError;

			// operand[1,..,2]: nodeID
			hResult = dsRead16Bits(pStream, &nodeAddr);
			if (hResult != NO_ERROR) return hResultError;
			
			// operand[3]: oPCR number
			hResult = dsRead8Bits(pStream, &oPCR);
			if (hResult != NO_ERROR) return hResultError;

			avcUnitCliPrintChannelUsage(channel, nodeAddr, oPCR);
			break;
/*
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_reserved:
*/
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcRspCliConnect(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultError = NO_ERROR;
	AVC_CONNECTION	connection;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_IMPLEMENTED:
//		case AVC_RESPONSE_STABLE:
		case AVC_RESPONSE_CHANGED:
		case AVC_RESPONSE_INTERIM:

			hResult = avcSubunitDecodeConnection(pStream, &connection);
			if (hResult != NO_ERROR) return hResultError;

			avcUnitCliPrintConnect(&connection);
			break;
/*
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_reserved:
*/
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcRspCliConnectAV(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultError = NO_ERROR;
//	AVC_CONNECTION	connection;

	UNUSED_ARG(packetBlock);
	UNUSED_ARG(pStream);

	switch (pHeader->ctype)
	{
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_IMPLEMENTED:
//		case AVC_RESPONSE_STABLE:
		case AVC_RESPONSE_CHANGED:
		case AVC_RESPONSE_INTERIM:
#if 0 //LM???
			hResult = avcSubunitDecodeConnection(pStream, &connection);
			if (hResult != NO_ERROR) return hResultError;

			avcUnitCliPrintConnectAV(&connection);
			break;
#endif
/*
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_reserved:
*/
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcRspCliConnections(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultError = NO_ERROR;
	uint8			total_connections = 0;
	AVC_CONNECTION	connectionArray[0xFF];
	uint32			i	= 0;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_RESPONSE_IMPLEMENTED:
//		case AVC_RESPONSE_STABLE:

			hResult = dsRead8Bits(pStream, &total_connections);
			if (hResult != NO_ERROR) return hResultError;

			for (i = 0; i < total_connections; i++)
			{
				hResult = avcSubunitDecodeConnection(pStream, &connectionArray[i]);
				if (hResult != NO_ERROR) return hResultError;
			}

			avcUnitCliPrintConnections(total_connections, connectionArray);

			break;
/*
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_CHANGED:
		case AVC_RESPONSE_INTERIM:
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_reserved:
*/
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcRspCliDigitalInput(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT		hResult = NO_ERROR;
	HRESULT		hResultError = NO_ERROR;
	BYTE		connection_state = 0;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_IMPLEMENTED:
//		case AVC_RESPONSE_STABLE:
		case AVC_RESPONSE_INTERIM:

			hResult = dsRead8Bits(pStream, &connection_state);
			if (hResult != NO_ERROR) return hResultError;

			cliOutputLine("-----------------------------------------------------------------------");
			cliOutputLine("Digital Input");
			avcUnitCliPrintConnectionState(connection_state);
			break;
/*
		case AVC_RESPONSE_CHANGED:
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_reserved:
*/
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcRspCliDigitalOutput(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT		hResult = NO_ERROR;
	HRESULT		hResultError = NO_ERROR;
	BYTE		connection_state = 0;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_IMPLEMENTED:
//		case AVC_RESPONSE_STABLE:
		case AVC_RESPONSE_INTERIM:

			hResult = dsRead8Bits(pStream, &connection_state);
			if (hResult != NO_ERROR) return hResultError;

			cliOutputLine("-----------------------------------------------------------------------");
			cliOutputLine("Digital Output");
			avcUnitCliPrintConnectionState(connection_state);
			break;
/*
		case AVC_RESPONSE_CHANGED:
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_reserved:
*/
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcRspCliDisconnect(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultError = NO_ERROR;
	AVC_CONNECTION	connection;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_IMPLEMENTED:
//		case AVC_RESPONSE_STABLE:
		case AVC_RESPONSE_INTERIM:

			hResult = avcSubunitDecodeConnection(pStream, &connection);
			if (hResult != NO_ERROR) return hResultError;

			avcUnitCliPrintConnect(&connection);
			break;
/*
		case AVC_RESPONSE_CHANGED:
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_reserved:
*/
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcRspCliDisconnectAV(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultError = NO_ERROR;
//	AVC_CONNECTION	connection;

	UNUSED_ARG(packetBlock);
	UNUSED_ARG(pStream);

	switch (pHeader->ctype)
	{
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_IMPLEMENTED:
//		case AVC_RESPONSE_STABLE:
		case AVC_RESPONSE_INTERIM:
#if 0 //LM???
			hResult = avcSubunitDecodeConnection(pStream, &connection);
			if (hResult != NO_ERROR) return hResultError;

			avcUnitCliPrintConnect(&connection);
			break;
#endif

/*
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_reserved:
*/
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

static HRESULT avcRspCliSignalFormat(BOOL bIn, AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultError = NO_ERROR;
	BYTE			plugID = 0;
	uint32			format = 0;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_IMPLEMENTED:
//		case AVC_RESPONSE_STABLE:
		case AVC_RESPONSE_CHANGED:
		case AVC_RESPONSE_INTERIM:
	
			hResult = dsRead8Bits(pStream, &plugID);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsRead32Bits(pStream, &format);
			if (hResult != NO_ERROR) return hResultError;
			if ((format & 0xC0000000) != 0x80000000) return hResultError;	// eoh:1=1, form:1=0, fmt:6, fdf:24
		 			 
			format &= 0x3FFFFFFF;											// strip eoh and form (keeping fmt and fdf)

			avcUnitCliPrintSignalFormat(bIn, plugID, format);
			break;
/*
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_reserved:
*/
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcRspCliInputPlugSignalFormat(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	return avcRspCliSignalFormat(TRUE, pHeader, packetBlock, pStream);
}

HRESULT avcRspCliOutputPlugSignalFormat(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	return avcRspCliSignalFormat(FALSE, pHeader, packetBlock, pStream);
}

HRESULT avcRspCliGeneralBusSetup(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;

	UNUSED_ARG(pHeader);
	UNUSED_ARG(packetBlock);
	UNUSED_ARG(pStream);

	return hResult;
}

// AV/C CCM Commands functions
HRESULT avcRspCliSignalSource(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT				hResult = NO_ERROR;
	HRESULT				hResultError = NO_ERROR;
	BYTE				status = 0;
	AVC_PLUG_CONNECTION plugs;

	UNUSED_ARG(packetBlock);
	UNUSED_ARG(pStream);

	switch (pHeader->ctype)
	{
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_IMPLEMENTED:
//		case AVC_RESPONSE_STABLE:
		case AVC_RESPONSE_CHANGED:


			hResult = dsRead8Bits(pStream, &status);
			if (hResult != NO_ERROR) return hResultError;

			hResult = avcSubunitDecodePlugConnection(pStream, &plugs);
			if (hResult != NO_ERROR) return hResult;

			avcUnitCliPrintSignalSource(status, &plugs);
			break;
/*
		case AVC_RESPONSE_INTERIM:
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_reserved:
*/
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcRspCliInputSelect(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT				hResult = NO_ERROR;
	HRESULT				hResultError = NO_ERROR;
	BYTE				status = 0;
	uint16				nodeID = 0;
	BYTE				oPlugID = 0;
	BYTE				iPlugID = 0;
	AVC_ADDRESS_PLUG	signalDst;

	UNUSED_ARG(packetBlock);

	switch (pHeader->ctype)
	{
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_IMPLEMENTED:
//		case AVC_RESPONSE_STABLE:
		case AVC_RESPONSE_CHANGED:


			hResult = dsRead8Bits(pStream, &status);
			if (hResult != NO_ERROR) return hResultError;
			if (status != 0xFF) return hResultError;

			hResult = dsRead8Bits(pStream, &status);
			if (hResult != NO_ERROR) return hResultError;
			if ((status & 0xF) != 0xF) return hResultError;
			status = (BYTE) (status >> 4);

			hResult = dsRead16Bits(pStream, &nodeID);
			if (hResult != NO_ERROR) return hResultError;

			hResult = dsRead8Bits(pStream, &oPlugID);
			if (hResult != NO_ERROR) return hResult;

			hResult = dsRead8Bits(pStream, &iPlugID);
			if (hResult != NO_ERROR) return hResult;

			hResult = avcSubunitDecodePlug(pStream, &signalDst);
			if (hResult != NO_ERROR) return hResult;

			avcUnitCliPrintInputSelect(status, nodeID, oPlugID, iPlugID, &signalDst);
			break;
/*
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_reserved:
		case AVC_RESPONSE_INTERIM:
*/
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcRspCliOutputPreset(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultError = NO_ERROR;

	UNUSED_ARG(packetBlock);
	UNUSED_ARG(pStream);

	switch (pHeader->ctype)
	{
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_IMPLEMENTED:
//		case AVC_RESPONSE_STABLE:
		case AVC_RESPONSE_CHANGED:
		case AVC_RESPONSE_INTERIM:

			avcUnitCliPrintOutputPreset(); //LM??? not done
			break;
/*
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_reserved:
*/
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

HRESULT avcRspCliCCMProfile(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultError = NO_ERROR;

	UNUSED_ARG(packetBlock);
	UNUSED_ARG(pStream);

	switch (pHeader->ctype)
	{
		case AVC_RESPONSE_ACCEPTED:
		case AVC_RESPONSE_IMPLEMENTED:
//		case AVC_RESPONSE_STABLE:
		case AVC_RESPONSE_CHANGED:
		case AVC_RESPONSE_INTERIM:

			avcUnitCliPrintCCMProfile(); //LM??? not done
			break;
/*
		case AVC_RESPONSE_NOT_IMPLEMENTED:
		case AVC_RESPONSE_REJECTED:
		case AVC_RESPONSE_IN_TRANSITION:
		case AVC_RESPONSE_reserved:
*/
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}

#endif //_CLI

