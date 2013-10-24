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
 * DESCRIPTION: Node Controller Interface Callbacks

	Purpose: Node Controller Interface callbacks

	This modules implements callbacks for handling node-controller related
	address ranges.  
	These include:
					Configuration ROM 
					Architecture Space
					Serial Bus Space
					Isochronous Resource Manager registers
					Topology and Speed Maps

	Revisions:
		created 04/30/2002 lm
****************************************************************************/

#include "TCTasking.h"
#include "1394.h"
#include "lalAddrRng.h"
#include "lhl.h"
#include "lhlLLC.h"
#include "lhlRsp.h"
#include "nciBM.h"
#include "nciCB.h"
#include "nciIRM.h"
#include "csr.h"

#include "lhlWatch.h"
static char 		moduleName[] = __FILE__;

HRESULT	nciCBInitialize(void)
{
	HRESULT	hResult = NO_ERROR;

	OFFSET_1394	addressLow;
	OFFSET_1394	addressHi;

	// Install callbacks for address ranges handled by the NCI

	/*
		Register Space.  all NCI implemented Address ranges have this value for the
		upper 16 bits of the 48-bit address
	*/
	addressLow.High = REGISTER_SPACE_ADDR_HI;
	addressHi.High = REGISTER_SPACE_ADDR_HI;

	/*
		Architecture area, various low level registers, some already supported by
		the hardware
	*/
	addressLow.Low = CSR_ARCH_SPACE_START;
	addressHi.Low = CSR_ARCH_SPACE_END;
	hResult = lalAddressRangeRegisterCB (nciCBArchSpace, addressLow, addressHi);
	if (hResult != NO_ERROR) return hResult;

	// Serial Bus area, including IRM regs
	addressLow.Low = CSR_SB_SPACE_START;
	addressHi.Low = CSR_SB_SPACE_END;
	hResult = lalAddressRangeRegisterCB (nciCBSBSpace, addressLow, addressHi);
	if (hResult != NO_ERROR) return hResult;

	// Configuration ROM area, supports quadlet and block reads
	addressLow.Low = CSR_ROM_SPACE_START;
	addressHi.Low = CSR_ROM_SPACE_END;
	hResult = lalAddressRangeRegisterCB (nciCBConfigROM, addressLow, addressHi);
	if (hResult != NO_ERROR) return hResult;

	// TOPOLOGY_MAP, supports quadlet and block reads
	addressLow.Low = CSR_TOPOLOGY_MAP_START;
	addressHi.Low = CSR_TOPOLOGY_MAP_END;
	hResult = lalAddressRangeRegisterCB (nciCBTopologyMap, addressLow, addressHi);
	if (hResult != NO_ERROR) return hResult;

	// SPEED_MAP, supports quadlet and block reads
	addressLow.Low = CSR_SPEED_MAP_START;
	addressHi.Low = CSR_SPEED_MAP_END;
	hResult = lalAddressRangeRegisterCB (nciCBSpeedMap, addressLow, addressHi);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*****************************************************************************

	FUNCTION: nciCBSBSpace

	Description:
		Used by NCI for trapping 1394 CSR Serial Bus space register access.
		Serial Bus Address Space includes offsets 0xF0000200 thru 0xF0000400
		includes Bus Mgr and Isoc Resource Mgr (IRM) registers

	Note: This is an expample of a callback that does not enQueue the packet
	block.  Most callbacks shall enQueue the packet block so that the packet is
	not handled in the low level mosule's thread.  In the case of this callback
	it is handling leow level register access that can be handled in the caller's
	thread.

	This callback verifies:
		Supported addresses
		Transaction types within the address
		Well-formatted requests within the type
		Appropriate responses within the request

*****************************************************************************/

HRESULT nciCBSBSpace(PB *packetBlock)
{
		HRESULT			hResult = NO_ERROR;
		OFFSET_1394		destOffset;

		hResult = pbGetDestinationOffset(packetBlock, &destOffset);
		if (hResult != NO_ERROR) return hResult;

		switch (destOffset.Low)
		{
			case CSR_SB_BM_ID:
			case CSR_SB_BW_AVAIL:
			case CSR_SB_CH_AVAIL_HI:
			case CSR_SB_CH_AVAIL_LO:
			case CSR_SB_BROADCAST_CH:
#ifdef _IRMC_CAPS
				if (nciIRMIsThisNodeIRM())
				{
					return nciCBGeneric(packetBlock, NCI_CB_SB_SPACE);
				}
#endif //_IRMC_CAPS
				break;
			case CSR_SB_CYCLE_TIME:
			case CSR_SB_BUS_TIME:
			case CSR_SB_BUSY_TIME_OUT:
				return nciCBGeneric(packetBlock, NCI_CB_SB_SPACE);
				break;
	}

	return nciCBAddressError(packetBlock, NCI_CB_SB_SPACE);
}

/*****************************************************************************

	FUNCTION: nciCBConfigROM

	Description:
		Used by NCI for trapping Config ROM read requests.
		Configuration ROM Address Space includes offsets 0xF0000400 thru 0xF00007FC.

		Implemented Serial Bus Registers
			Configuration ROM is implemented from 0xF0000400 to the extent of
			thisNode's actual ROM length.

	Note: This is an expample of a callback that does not enQueue the packet
	block.  Most callbacks shall enQueue the packet block so that the packet is
	not handled in the low level mosule's thread.  In the case of this callback
	it is handling leow level register access that can be handled in the caller's
	thread.

	This callback verifies:
		Supported addresses
		Transaction types within the address
		Well-formatted requests within the type
		Appropriate responses within the request

*****************************************************************************/

HRESULT nciCBConfigROM(PB *packetBlock)
{
	return nciCBGeneric(packetBlock, NCI_CB_CONFIG_ROM);
}

/*****************************************************************************

	FUNCTION: nciCBArchSpace

	Description:
		Used by NCI for trapping 1394 CSR Architecture space register access.
		Architecture Space includes offsets 0xF0000000 thru 0xF00001FC.

		Implemented Architecture Space Registers
			None

	Note: This is an expample of a callback that does not enQueue the packet
	block.  Most callbacks shall enQueue the packet block so that the packet is
	not handled in the low level mosule's thread.  In the case of this callback
	it is handling low level register access that can be handled in the caller's
	thread.

*****************************************************************************/

HRESULT nciCBArchSpace(PB *packetBlock)
{
	return nciCBGeneric(packetBlock, NCI_CB_ARCH_SPACE);
}

/*****************************************************************************

	FUNCTION: nciCBTopologyMap

	Description:
		Used by NCI for trapping 1394 CSR Topology Map register access.
		The Topology Map includes offsets 0xF0001000 thru 0xF00013FC.

	Note: This is an expample of a callback that does not enQueue the packet
	block.  Most callbacks shall enQueue the packet block so that the packet is
	not handled in the low level mosule's thread.  In the case of this callback
	it is handling leow level register access that can be handled in the caller's
	thread.

	This callback should be implemetned for Bus Manager

*****************************************************************************/

HRESULT nciCBTopologyMap(PB *packetBlock)
{
#ifdef _BMC_CAPS
	return nciCBGeneric(packetBlock, NCI_CB_TOPOLOGY_MAP);
#else //_BMC_CAPS
	return nciCBAddressError(packetBlock, NCI_CB_TOPOLOGY_MAP);
#endif //_BMC_CAPS
}

/*****************************************************************************

	FUNCTION: nciCBSpeedMap

	Description:
			Used by NCI for trapping 1394 CSR Topology Map register access.
			The Speed Map includes offsets 0xF0002000 thru 0xF0002FFC.

		Implemented Speed Map Registers
			None, pending Bus Manager implementation.

	Note: This is an expample of a callback that does not enQueue the packet
	block.  Most callbacks shall enQueue the packet block so that the packet is
	not handled in the low level mosule's thread.  In the case of this callback
	it is handling leow level register access that can be handled in the caller's
	thread.

	This callback should be implemetned for Bus Manager

*****************************************************************************/

HRESULT nciCBSpeedMap(PB *packetBlock)
{
#ifdef _BMC_CAPS
	return nciCBGeneric(packetBlock, NCI_CB_SPEED_MAP);
#else //_BMC_CAPS
	return nciCBAddressError(packetBlock, NCI_CB_SPEED_MAP);
#endif //_BMC_CAPS
}

HRESULT nciCBGeneric(PB *packetBlock, uint32 nciCBtype)
{
	HRESULT			hResult = NO_ERROR;
	uint32			numBytes = 0;
	uint16			i;
	PB_HEADER		*pHeader = NULL;
	QUADLET			*pPayload = NULL;
	QUADLET			*pRespPayload = NULL;
	PB*				respPacket = NULL;
	PB_PACKETTYPE	packetType = PB_TYPE_UNDEF;
	PB_LOCKTYPE		lockType = PB_LOCKTYPE_NONE;
	RCODE_1394		rCode = RSP_ADDRESS_ERROR;
	OFFSET_1394		destOffset;
	uint16			reqType = LHL_REQ_READ;

	hResult = pbGetPacketHeader(packetBlock, &pHeader);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPacketType(packetBlock, &packetType);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetLockType(packetBlock, &lockType);
	if (hResult != NO_ERROR) return hResult;
	hResult = pbGetPayload(packetBlock, (void **) &pPayload);
	if (hResult != NO_ERROR) return hResult;

	// no broadcasts supported here
	if (pbPacketIsBroadcast(packetBlock))
	{
		return hResult;
	}

	switch (packetType)
	{
		case PB_TYPE_READ_REQUEST_QUADLET:
		case PB_TYPE_READ_REQUEST:
			reqType = LHL_REQ_READ;
			break;
		case PB_TYPE_WRITE_REQUEST_QUADLET:
		case PB_TYPE_WRITE_REQUEST:
			reqType = LHL_REQ_WRITE;
			break;
		case PB_TYPE_LOCK_REQUEST:
			reqType = LHL_REQ_LOCK;
			break;
	}

	switch (packetType)
	{
		case PB_TYPE_READ_REQUEST_QUADLET:
		case PB_TYPE_WRITE_REQUEST_QUADLET:
			numBytes = 4;
			break;
		case PB_TYPE_READ_REQUEST:
		case PB_TYPE_WRITE_REQUEST:
		case PB_TYPE_LOCK_REQUEST:
			hResult = pbGetDataLen(packetBlock, &numBytes);
			if (hResult != NO_ERROR) return hResult;
			break;
	}

	hResult = pbGetDestinationOffset(packetBlock, &destOffset);
	if (hResult != NO_ERROR) return hResult;

	switch (nciCBtype)
	{
		case NCI_CB_ARCH_SPACE:
			switch (packetType)
			{
				case PB_TYPE_READ_REQUEST_QUADLET:
				case PB_TYPE_READ_REQUEST:
				case PB_TYPE_WRITE_REQUEST_QUADLET:
				case PB_TYPE_WRITE_REQUEST:
					if (destOffset.Low + numBytes <= CSR_ARCH_SPACE_END + 1)
					{
						QUADLET		**pData = NULL;
						uint32		respLen = 0;

						switch (packetType)
						{
							case PB_TYPE_READ_REQUEST_QUADLET:
							case PB_TYPE_READ_REQUEST:
								pData = &pRespPayload;
								respLen = numBytes;
								break;
							case PB_TYPE_WRITE_REQUEST_QUADLET:
							case PB_TYPE_WRITE_REQUEST:
								pData = &pPayload;
								respLen = 0;
								break;
						}

						hResult = pbCreateDuplicatePacketWithSize(packetBlock, &respPacket, (uint16) respLen, NULL, PB_CREATE_NCI_CB);
						if (hResult != NO_ERROR) return hResult;

						hResult = pbGetPayload(respPacket, (void **) &pRespPayload);
						if (hResult != NO_ERROR) return hResult;

						for (i = 0; i < numBytes / 4; i++)
						{
							hResult = lhlLLCHandleCSR(destOffset, 4, *pData + i, reqType);
							if (hResult != NO_ERROR)
							{
								break;
							}
							destOffset.Low += 4;
						}
						if (hResult == NO_ERROR)
						{
							rCode = RSP_COMPLETE;
						}
						numBytes = respLen;
					}
				break;
			}
			break;

		case NCI_CB_SB_SPACE:
			switch (destOffset.Low)
			{
				case CSR_SB_CYCLE_TIME:
				case CSR_SB_BUS_TIME:
				case CSR_SB_BUSY_TIME_OUT:
					switch (packetType)
					{
						case PB_TYPE_READ_REQUEST_QUADLET:
							hResult = pbCreateDuplicatePacketWithSize(packetBlock, &respPacket, (uint16) numBytes, NULL, PB_CREATE_NCI_CB);
							if (hResult != NO_ERROR) return hResult;

							hResult = pbGetPayload(respPacket, (void **) &pRespPayload);
							if (hResult != NO_ERROR) return hResult;

							rCode = lhlWatchTransRead(destOffset.Low, pRespPayload);
							break;

						case PB_TYPE_WRITE_REQUEST_QUADLET:
							if (numBytes == 4) // only allow quadlet write
							{
									hResult = pbCreateDuplicatePacketWithSize (packetBlock, &respPacket, (uint16) 0, NULL, PB_CREATE_NCI_CB);
									if (hResult != NO_ERROR) return hResult;

									rCode = lhlWatchTransWrite(destOffset.Low, pPayload[0]);
							}

							break;
					}
					break;
				case CSR_SB_BM_ID:
				case CSR_SB_BW_AVAIL:
				case CSR_SB_CH_AVAIL_HI:
				case CSR_SB_CH_AVAIL_LO:
				case CSR_SB_BROADCAST_CH:
#ifdef _IRMC_CAPS
					if (nciIRMIsThisNodeIRM())
					{
						// handle IRM Registers here
						switch (packetType)
						{
							case PB_TYPE_READ_REQUEST_QUADLET:
								hResult = pbCreateDuplicatePacketWithSize(packetBlock, &respPacket, (uint16) numBytes, NULL, PB_CREATE_NCI_CB);
								if (hResult != NO_ERROR) return hResult;

								hResult = pbGetPayload(respPacket, (void **) &pRespPayload);
								if (hResult != NO_ERROR) return hResult;

								rCode = nciIRMRead(destOffset.Low, pRespPayload);
								break;

							case PB_TYPE_LOCK_REQUEST:
								if (numBytes == 8) // only allow quadlet lock
								{
									if (lockType == PB_LOCKTYPE_COMPARE_SWAP)
									{
										uint32		lockArg = pPayload[0];
										uint32		lockData = pPayload[1];

										// response payload length is 1/2 size of request packet
										numBytes /= 2;
										hResult = pbCreateDuplicatePacketWithSize (packetBlock, &respPacket, (uint16) numBytes, NULL, PB_CREATE_NCI_CB);
										if (hResult != NO_ERROR) return hResult;

										hResult = pbGetPayload(respPacket, (void **) &pRespPayload);
										if (hResult != NO_ERROR) return hResult;

										rCode = nciIRMLock(destOffset.Low, lockArg, lockData, pRespPayload);
									}
								}
								break;
							case PB_TYPE_WRITE_REQUEST:
								if (numBytes == 4) // only allow quadlet lock
								{
										hResult = pbCreateDuplicatePacketWithSize (packetBlock, &respPacket, (uint16) numBytes, NULL, PB_CREATE_NCI_CB);
										if (hResult != NO_ERROR) return hResult;

										rCode = nciIRMWrite(destOffset.Low, pPayload[0]);
								}
								break;
						}
					}
#endif //_IRMC_CAPS
					break;
			}
			break;
	
		case NCI_CB_CONFIG_ROM:
			switch (packetType)
			{
				case PB_TYPE_READ_REQUEST_QUADLET:
				case PB_TYPE_READ_REQUEST:
					if ((destOffset.Low >= CSR_ROM_SPACE_START) && 
						(destOffset.Low + numBytes <= CSR_ROM_SPACE_START + csrGetConfigROMQuadletSize() * 4))
					{
						hResult = pbCreateDuplicatePacketWithSize(packetBlock, &respPacket, (uint16) numBytes, NULL, PB_CREATE_NCI_CB);
						if (hResult != NO_ERROR) return hResult;

						hResult = pbGetPayload(respPacket, (void **) &pRespPayload);
						if (hResult != NO_ERROR) return hResult;

						for (i = 0; i < numBytes / 4; i++)
						{
							pRespPayload[i] = csrReadConfigROMQuadlet(destOffset.Low + (i * 4));
						}
						rCode = RSP_COMPLETE;
					}
					break;
			}
			break;

#ifdef _BMC_CAPS
		case NCI_CB_TOPOLOGY_MAP:
			switch (packetType)
			{
				case PB_TYPE_READ_REQUEST_QUADLET:
				case PB_TYPE_READ_REQUEST:
					if (destOffset.Low + numBytes <= CSR_TOPOLOGY_MAP_END + 1)
					{
						uint32	index = (destOffset.Low - CSR_TOPOLOGY_MAP_START) / 4;
						BOOL	bBusMaster = TRUE;

						hResult = pbCreateDuplicatePacketWithSize(packetBlock, &respPacket, (uint16) numBytes, NULL, PB_CREATE_NCI_CB);
						if (hResult != NO_ERROR) return hResult;

						hResult = pbGetPayload(respPacket, (void **) &pRespPayload);
						if (hResult != NO_ERROR) return hResult;

						for (i = 0; i < numBytes / 4; i++)
						{
							hResult = nciBMGetTopologyMapIndex(bBusMaster, index + i, &(pRespPayload[i]));
							if (hResult != NO_ERROR)
							{
								break;
							}
						}
						if (hResult == NO_ERROR)
						{
							rCode = RSP_COMPLETE;
						}
					}
					break;
			}
			break;

		case NCI_CB_SPEED_MAP:
			switch (packetType)
			{
				case PB_TYPE_READ_REQUEST_QUADLET:
				case PB_TYPE_READ_REQUEST:
					if (destOffset.Low + numBytes <= CSR_SPEED_MAP_END + 1)
					{
						uint32	index = (destOffset.Low - CSR_SPEED_MAP_START) / 4;
						BOOL	bBusMaster = TRUE;

						hResult = pbCreateDuplicatePacketWithSize(packetBlock, &respPacket, (uint16) numBytes, NULL, PB_CREATE_NCI_CB);
						if (hResult != NO_ERROR) return hResult;

						hResult = pbGetPayload(respPacket, (void **) &pRespPayload);
						if (hResult != NO_ERROR) return hResult;

						for (i = 0; i < numBytes / 4; i++)
						{
							hResult = nciBMGetSpeedMapIndex(bBusMaster, index + i, &(pRespPayload[i]));
							if (hResult != NO_ERROR)
							{
								break;
							}
						}
						if (hResult == NO_ERROR)
						{
							rCode = RSP_COMPLETE;
						}
					}
					break;
			}
			break;
#endif //_BMC_CAPS

		default:
			hResult = E_BAD_INPUT_PARAMETERS;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
	}

	if (rCode != RSP_COMPLETE)
	{
		lhlReplyErrorResponse(packetBlock, rCode, TRUE);
		hResult = (HRESULT) (E_LHL_RSP_BASE + rCode);
	}
	else
	{
		switch (packetType)
		{
			case PB_TYPE_READ_REQUEST_QUADLET:
			case PB_TYPE_READ_REQUEST:
				hResult = lhlSendReadResponse(respPacket, rCode, (uint16) numBytes, pRespPayload);
				break;
			case PB_TYPE_WRITE_REQUEST_QUADLET:
			case PB_TYPE_WRITE_REQUEST:
				hResult = lhlSendWriteResponse(respPacket, rCode);
				break;
			case PB_TYPE_LOCK_REQUEST:
				hResult = lhlSendLockResponse(respPacket, rCode, (uint16) numBytes, pRespPayload);
				break;
		}
	}

	// clean up the resp packetBlock
	if (pbIsValid(respPacket))
	{
		pbPacketDone(respPacket, PB_DONE_NCI_CB);
	}
	
	return hResult;
}

HRESULT nciCBAddressError(PB *packetBlock, uint32 nciCBtype)
{
	HRESULT		hResult = NO_ERROR;

	UNUSED_ARG(nciCBtype);

	// no broadcasts supported here
	if (pbPacketIsBroadcast(packetBlock))
	{
		return hResult;
	}

	// so far no implementation
	lhlReplyErrorResponse(packetBlock, RSP_ADDRESS_ERROR, TRUE);

	return hResult;
}

