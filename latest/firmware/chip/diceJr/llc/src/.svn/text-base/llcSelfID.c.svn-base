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
	
	Purpose:	LLC SelfID DMA controller interface
				for the Samsung 1394 LLC
	
	Revisions:
		created 12/16/2002 kbj

****************************************************************************/

#include "1394.h"
#include "ErrorCodes.h"
#include "TCTypes.h"
#include "TCDebug.h"
#include "llcReg.h"
#include "lhl.h"
#include "lhlLLCDefs.h"
#include "lhlStatistics.h"

#define LLC_SELFID_SIZE		LLC_ASY_RX_DEPTH

static char 		moduleName[] = __FILE__ ;

// Allocating aligned SelfID data segment
QUADLET llcSelfIDBuffer[LLC_SELFID_SIZE];

// Incremented for each bus reset
static uint16 selfIDGenerationID;

static uint8  resetCount = 0;
static uint32 BusStatus = 0;

// Initialize SelfID controller and buffer
HRESULT llcInitSelfIDController(void)
{
	HRESULT		hResult = NO_ERROR;
	int16 i;
	// Clear SelfID buffer
	for (i = 0; i < LLC_SELFID_SIZE; i++)
		llcSelfIDBuffer[i] = 0;
	
	selfIDGenerationID = 0;

	return hResult;
}

/*
When a Bus Reset occurs, all the async DMA buffers are cleared by hardware.  
Immediately following the bus reset, each node on the bus will transmit 
SELF-ID packets in order according to the bus topology.  
SELF-ID packet transmission is concluded with a sub-action gap.
Between the Bus Reset event and the first sub-action gap, the Async 
DMA will inhibit the receive interrupt.  

This will cause all the SELF-ID packets to be bundled into the request 
receive buffer as if they were a single packet.  After the sub-action gap, 
the receive interrupt will become uninhibited and software will be interrupted.
Software must ensure that sufficient buffer space is allocated in the request 
receive buffer to accept the maximum expected SELF-ID bundle.  If the bundle is 
larger than expected, software will receive a buffer overflow interrupt and can 
respond by allocating a larger buffer then issuing another bus reset. 	
*/

/* Observation, ML 17.12.04
This routine is called when a BusReset is over and the LLC indicates that there are
a SELF-ID bundle in the FIFO. There is however no guarantee that it is the first
packege in the FIFO so code has been added to skip other packages.
It should be OK to discard those as we are looking for a bus reset
*/



// Reads and verifies selfID buffer 
HRESULT llcGetSelfIDs(SELFID_BUNDLE *pSelfID)
{
	HRESULT		hResult = NO_ERROR;
	uint16		tCode = TCODE_PHY_PACKET+1;
	int16		i = 0;
	int16		j = 0;
	uint32		dataCount;
	QUADLET		PacketId;
	QUADLET		SelfIDPacket;
	uint8		first;
	uint32		fifoStat;

	pSelfID->bValid = FALSE;
	pSelfID->numSIDQuads = 0;
	pSelfID->numNodes = 0;

	resetCount = 0;
	BusStatus = 0;

	// Checking bus status register bits
	llcLinkRegRead(BUS_STAT_REG_DP, &BusStatus);
	
	llcLinkRegRead(ASY_RX_FIFO_QLETS_REG_DP, &dataCount);
	dataCount &= LLC_RX_QLT_AVAIL_MASK;

	while(dataCount)
	{
		// Look for a headet which is a PHy package
		while ((tCode != TCODE_PHY_PACKET) && (dataCount))
		{
			llcLinkRegRead(ASY_RX_FIFO_REG_DP, &PacketId);
			dataCount--;
			llcLinkRegRead(ARFIFO_STAT_REG_DP, &fifoStat);
			if (fifoStat & LLC_ARFIFO_HEAD)
			{
				// this quadlet is a head
				tCode = (uint16)((PacketId&MASK_TCODE)>>SHIFT_TCODE);
			}
		}
		
		if (tCode == TCODE_PHY_PACKET) // Check for PHY packets 
		{
			llcSelfIDBuffer[0] = PacketId;
			i = 1;
			first = TRUE;
			while (dataCount) // Skip trailer 
			{
				llcLinkRegRead(ASY_RX_FIFO_REG_DP, &SelfIDPacket);
				dataCount--;
				llcSelfIDBuffer[i++] = SelfIDPacket;
				if ((SelfIDPacket & LLC_SELFID_MASK) == LLC_SELFID_PACKET)	// SelfIDPacket
				{
					if (first)
					{
						// Look for more SelfIDPacket bundles
						j = 0;
						pSelfID->numSIDQuads = 0;
						pSelfID->numNodes = 0;
						first = FALSE;
					}
					// Save self-ids
					pSelfID->sid[j++] = SelfIDPacket;
					// count self-ids after verified
					pSelfID->numSIDQuads++;
					// if a first-packet, count it as a unique node
					if ((SelfIDPacket & SELF_ID_CONT_PACKET) == 0)
					{
						pSelfID->numNodes++;
					}
				} 
				else if ((SelfIDPacket & LLC_TRAILER_MASK) == LLC_TRAILER_PACKET) // Trailer
				{
					first = TRUE;
					resetCount = (uint8)((SelfIDPacket&LLC_BUS_RESET_CNT_MASK) >> LLC_BUS_RESET_CNT_SHIFT);
					//ML: Leave new packets in the buffer
					//	  This has been verified to fix problems with the first ReadRequests received after
					//    a bus reset.
					//break;
					//so after reading the first Bundle we return.
					return hResult;
				}
				else // Any other packet
				{
					// Skip remaining packets !!!!
					// ML: we should never end here!!
				}
			}
		} 
		else 
		{
			//ML: This should be fixed by using the header status bit in the LLC
			//    It will only be a problem if there is heavy trafic just before a
			//    bus reset.
#ifdef _STATISTICS
			lhlStatistics.SelfIDErrs++;
#endif // _STATISTICS
		}
	}

	return hResult;	
}

// Reads and verifies selfID buffer 
HRESULT llcVerifySelfIDs(SELFID_BUNDLE *pSelfID)
{
	HRESULT		hResult = NO_ERROR;

	// Checking bus status register bits
	if ((BusStatus & LLC_ND_ID_OVR) == 0)
	{
		hResult = E_LLC_NODE_ID_NOT_OVER;
		sysLogError(hResult, __LINE__, moduleName);
	}
	if ((BusStatus & LLC_ROOT_ID_OVR) == 0)
	{
		hResult = E_LLC_ROOT_ID_NOT_OVER;
		sysLogError(hResult, __LINE__, moduleName);
	}
	if (BusStatus & LLC_ROOT_ID_ERR)
	{
		hResult = E_LLC_ROOT_ID_ERR;
		sysLogError(hResult, __LINE__, moduleName);
	}
	if (BusStatus & LLC_ND_ID_ERR)
	{
		hResult = E_LLC_NODE_ID_ERR;
		sysLogError(hResult, __LINE__, moduleName);
	}
	if (BusStatus & LLC_TIME_OUT)
	{
		hResult = E_LLC_CABLE_LOOP;
		sysLogError(hResult, __LINE__, moduleName);
	}
	if (BusStatus & LLC_SELF_ID_INCR_ERR)
	{
		hResult = E_LLC_SELF_ID_INCR_ERR;
		sysLogError(hResult, __LINE__, moduleName);
	}
	if (BusStatus & LLC_SEL_ID_PAR_ERR)
	{
		hResult = E_LLC_SELF_ID_PAR_ERR;
		sysLogError(hResult, __LINE__, moduleName);
	}
	if (BusStatus & LLC_SELFID_RX_FAIL)
	{
		hResult = E_LLC_SELF_ID_RX_FAIL;
		sysLogError(hResult, __LINE__, moduleName);
	}
	if ((BusStatus & LLC_NO_ERR) == 0)
	{
		hResult = E_LLC_BUS_ERROR;
		sysLogError(hResult, __LINE__, moduleName);
	}

#if 1 //LM???
	hResult = NO_ERROR;
#endif

	// make sure there is a sane number of nodes found.
	if (pSelfID->numNodes > MAX_NODES_PER_BUS)
	{
		hResult = E_LHL_SELF_ID_TOO_MANY;
		sysLogError(hResult, __LINE__, moduleName);
	}
	
	if (pSelfID->numNodes == 0)
	{
		hResult = E_LHL_SELF_ID_NONE;
		sysLogError(hResult, __LINE__, moduleName);
	}
	
	selfIDGenerationID = (BusStatus & LLC_RESET_COUNT_MASK) >> LLC_RESET_COUNT_SHIFT;
	if (resetCount != selfIDGenerationID)
	{
		hResult = E_LLC_BUS_RESET_CNT_ERR;
		sysLogError(hResult, __LINE__, moduleName);
	}

	if (hResult != NO_ERROR) return hResult;

	pSelfID->bValid = TRUE;

	return hResult;
}

// Reads SelfID generation ID
HRESULT llcGetSelfIDGeneration(uint16 *generationID)
{
	HRESULT		hResult = NO_ERROR;

	*generationID = selfIDGenerationID;
	return hResult;
}

HRESULT llcGetBusNodeNumber(uint16 *pBusNumber, uint16 *pNodeNumber, uint8 *pRoot)
{
	HRESULT		hResult = NO_ERROR;
	uint32		nodeIDReg;

	*pRoot = 0;
	llcLinkRegRead(ND_ID_REG_DP, &nodeIDReg);
	if ((nodeIDReg & LLC_VALID_ND_ID) == 0)
	{
		hResult = E_LLC_INVALID_NODE_ID;
	}
	if (nodeIDReg&LLC_ROOT) *pRoot = 1;
	*pBusNumber = (uint16) (nodeIDReg&LLC_BUS_ADDR_MASK);
	*pNodeNumber = (uint16) (nodeIDReg&LLC_ND_ADDR_MASK);

	return hResult;
}
