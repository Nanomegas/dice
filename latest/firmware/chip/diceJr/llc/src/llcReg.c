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
	
	Purpose:	LLC register initialization and access
				for the Samsung 1394 LLC
	
	Revisions:
		created 12/10/2002 kbj


****************************************************************************/

// 1394Core includes
#include "1394.h"
#include "TCTypes.h"
#include "TCTasking.h"
#include "TCDebug.h"
#include "ErrorCodes.h"
#include "llcEnvDefs.h"
#include "llcReg.h"
#include "llcPHY.h"
#include "llcInt.h"
#include "llcSelfID.h"
#include "bmdefs.h"
#include "csr.h"
#include "lal.h"
#include "lhl.h"
#include "targetVendorDefs.h"

static char 		moduleName[] = __FILE__ ;

// Number of times to retry sending a request or response packet
#define RETRY_COUNT 2

// Number of arbitrations gabs to wait before retry packet 
#define RETRY_TIME  2

/*=======================================================================================*/
// LLC register access functions

/* Phy register values updated by readPhyReg */
QUADLET phyBaseReg[8];
QUADLET phyPage0Port0Reg[2];
QUADLET phyPage0Port1Reg[2];
QUADLET phyPage0Port2Reg[2];
QUADLET phyPage1Reg[8];
QUADLET phyPage7Reg[8];


void llcLinkMaskAndChg(uint32 registerOffset, uint32 Mask, uint32 WriteValue)
// Do an atomic read-modify-write of the required register.
// Reads register, masks in WriteValue using Mask and writes it back
{
	//KBJ??? llcRegWrCnt++;
	*mPtr(registerOffset) = (*mPtr(registerOffset) & ~Mask) | (WriteValue & Mask); 
	//llcRegWrCnt--;
}

void llcLinkSetBits(uint32 registerOffset, uint32 bitSet)
// Do an atomic read-modify-write of the required register
{
	//KBJ??? llcRegWrCnt++;
	*mPtr(registerOffset) = *mPtr(registerOffset) | bitSet;
	//llcRegWrCnt--;
}

static HRESULT llcStartLink(void)
{
	uint32 regVal;

	llcLinkRegRead(VERSION_REG_DP, &regVal);
	if (regVal != 0x0a200100)
	{
		sysLogError(E_LLC_WRONG_VERSION, __LINE__, moduleName);
		sysDebugPrintf("regVal is 0x%x\n", regVal);
		return E_LLC_WRONG_VERSION;
	}
		
	// LCSR_REG_DP Link Control and Status information Reg.
	llcLinkRegWrite(LCSR_REG_DP, 
					LLC_FIFO_CONFIG_EN | 
					/*LLC_STORE_BAD_ASY_HDR |*/ 
					LLC_AUTO_CY_MAST | 
					LLC_ACCLM_ENABLED |
					LLC_MAN_MODE | 
					(SPEED_400<<LLC_MAX_LINK_SPEED_SHIFT));
  
	// Node Identification and Status reg. (Default 0x3FF and 0x3F)
	llcLinkRegWrite(ND_ID_REG_DP, LLC_BUS_ADDR_MASK | LLC_ND_ADDR_MASK);
	
	// Setting FIFO depths of receiver and transmitter to 256 quadlets
	llcLinkRegWrite(ASY_RX_FIFO_DEPTH_REG_DP, LLC_ASY_RX_DEPTH);
	llcLinkRegWrite(ASY_TX_FIFO_DEPTH_REG_DP, LLC_ASY_TX_DEPTH);
		
	// LNK_CTRL_REG_DP Link Control reg. disable configuration of FIFO depths 
	// and enable receiving self_id packets
	llcLinkRegWrite(LNK_CTRL_REG_DP, LLC_CY_TIMER_EN | LLC_SELF_ID_RXD);

	// Setting retry counters (Default is 7)
	llcLinkRegWrite(RET_CT_REG_DP, (RETRY_COUNT << LLC_RETRY_COUNT_SHIFT) | 
								   (RETRY_TIME << LLC_RETRY_TIME_SHIFT));
	
	// Disable changing of FIFO depths 
	llcLinkMaskAndChg(LCSR_REG_DP, LLC_FIFO_CONFIG_EN, 0);
	
	// Clear the diagnostics register and set 
	// cy_timer_out[0:19] = cycle time counter count[8] + offset[12]
	llcLinkRegWrite(DIAG_REG_DP, 0x00800000);

	// Check if any host access errors occured
	llcLinkRegRead(HST_ACC_ERR_REG_DP, &regVal); 
	if (regVal != 0)
	{
		sysDebugPrintf("Host access error 0x%x\n\r", regVal);
		sysLogError(E_LLC_HOST_ACC_ERR, __LINE__, moduleName);
		return E_LLC_HOST_ACC_ERR;
	}

	return NO_ERROR;
}

static HRESULT llcWait4PhyRW(void)
// Wait for the link to report that a phy read or write is done
// Returns NO_ERROR on a successful wait. E_FAIL if it times out.
{
	uint32 loopCnt;
	uint32 tempVal;

	// Loop until we know we are allowed to write to the PHY
	loopCnt = 1000;
	do
	{
		loopCnt--;
		llcLinkRegRead(PHY_CTRL_REG_DP, &tempVal);
	} while ((loopCnt > 0) && 
  			(tempVal & (LLC_RD_REG | LLC_WR_REG))); 

	// Exit happy if we didnt time out
	if (loopCnt != 0)
	{
		return NO_ERROR;
	} 
	else
	{
		return E_FAIL;
	}
}

HRESULT llcWritePhy(uint8 phyAddr, uint32 value)
// Write the value in the given phy register.
// This can fail or timeout. 
// Checking the return value is a GOOD idea.
{
	uint8 val = value & 0xff;

	// Bad address?
	if (phyAddr & 0xF0) return E_BAD_INPUT_PARAMETERS;

	// Leave early if we give up waiting for the PHY read/write
	if (llcWait4PhyRW() == E_FAIL) return E_LHL_PHY_IO_ERROR;

	llcLinkRegWrite(PHY_CTRL_REG_DP, 
					LLC_WR_REG |
					((phyAddr << LLC_REG_ADDR_SHIFT) & LLC_REG_ADDR_MASK) |
					(val & LLC_WR_DATA_MASK)
					);

	// Complain if we give up waiting for the PHY read/write
	if (llcWait4PhyRW() == E_FAIL) return E_LHL_PHY_IO_ERROR;

	return NO_ERROR;
}

HRESULT llcReadPhy(uint8 phyAddr, uint32 *value)
// Read the value in the given phy register.
// This can fail or timeout. 
// Checking the return value is a GOOD idea.
{
	uint32 tempVal;
	uint32 loopCnt;
	
	// Bad address ?
	if (phyAddr & 0xF0) return E_BAD_INPUT_PARAMETERS;

	// Leave early if we give up waiting for the PHY read/write
	if (llcWait4PhyRW() == E_FAIL) return E_LHL_PHY_IO_ERROR;

	// Read given PHY Addr
	llcLinkRegWrite(PHY_CTRL_REG_DP, LLC_RD_REG | (phyAddr << LLC_REG_ADDR_SHIFT));

	// Leave early if we give up waiting for the PHY read/write
	if (llcWait4PhyRW() == E_FAIL) return E_LHL_PHY_IO_ERROR;

	// Wait for the read to completed
	loopCnt = 1000;
	do
	{ 
		llcLinkRegRead(PHY_CTRL_REG_DP, &tempVal);
		loopCnt--;
	} while (((tempVal & LLC_RD_DONE) == 0) && (loopCnt > 0));

	if (loopCnt == 0) return E_LHL_PHY_IO_ERROR;
	
	if (((tempVal & LLC_RD_ADDR_MASK) >> LLC_RD_ADDR_SHIFT) == phyAddr)
	{
		// Phy reports that the address returned is the one we asked for.
		*value = (tempVal & LLC_RD_DATA_MASK) >> LLC_RD_DATA_SHIFT;
	} 
	else
	{
		// We asked for address x and got from address y. Huh?
		return E_FAIL;
	}

  return NO_ERROR;
}

/*********************************************************
 Write CSR register from by use of compare-and-swap transaction
*/
// csrSel values:
/* KBJ ???
#define BUS_MANGER_ID			0
#define BANDWIDTH_AVAILABLE		1
#define CHANNELS_AVAILABLE_HI	2
#define CHANNELS_AVAILABLE_LO	3
HRESULT CSRRegWrite(uint32 oldValue, uint32 newValue, uint8 csrSel)
{
	HRESULT	hResult = NO_ERROR;

	return hResult;
}
*/

/*********************************************************
 Initialization of General ROM Format - Bus_Info_Block
*/
static void initConfigROM(void) 
{
	HRESULT hResult;
	uint32  WWUIDHigh;
	uint32  WWUIDLow;

	// Build this node's Configuration ROM based on this nodes WWUID
	hResult = lalGetWWUID(&WWUIDHigh, &WWUIDLow);
	if (hResult == NO_ERROR)
	{
		// Create Bus_Info_Block
		csrBuildConfigROM(WWUIDLow, WWUIDHigh);
	}
	else
	{		
		// Create Bus_Info_Block
		WWUIDHigh = DEFAULT_DEVICE_ID_HIGH;
		WWUIDLow = DEFAULT_DEVICE_ID_LOW;
		lalSetWWUID(WWUIDHigh, WWUIDLow);
		csrBuildConfigROM(WWUIDLow, WWUIDHigh);
	}
}

/**************************** LLC initialization *********************/
// Initialization sequence:
HRESULT llcInitialize(void)
{
	HRESULT hResult = NO_ERROR;
	
	llcEnvInitialize();
	
	llcInitSelfIDController();
	initConfigROM();

	// Initiate Samsung LLC
	hResult = llcStartLink();
	if (hResult != NO_ERROR) return hResult;

	// Initiate interrupt registers
	llcIntInitialize();

	// PHY Clear power fail and enable acceleration arbitration
	hResult = llcWritePhy(PHY_BUS_STATUS,  PHY_BUS_STATUS_EAA | PHY_BUS_STATUS_CPSI);
	if (hResult != NO_ERROR)
	{
		sysLogError(hResult, __LINE__, moduleName);
	}
	return hResult;
}

// Called when bus reset detected
HRESULT	llcBusResetInitialize(void)
{
	return NO_ERROR;
}

// Called when bus reset completed
// Checking the Node Identification and Status Register
HRESULT llcBusResetComplete(void)
{
	uint32 nodeStatusReg;
	
	llcLinkRegRead(ND_ID_REG_DP, &nodeStatusReg);

	if ((nodeStatusReg & LLC_VALID_ND_ID) == 0)
	{
		sysLogError(E_LLC_NODE_ID_INVALID, __LINE__, moduleName);
		return E_LLC_NODE_ID_INVALID;
	}
	if ((nodeStatusReg & LLC_ND_ADDR_MASK) == LLC_ND_ADDR_MASK)
	{
		sysLogError(E_LHL_NODEID_NOT_AVAIL, __LINE__, moduleName);
		return E_LHL_NODEID_NOT_AVAIL;
	}

#if 0
	// This happens because no cable was connected - this is OK
	if ((nodeStatusReg & LLC_CBL_PWR_ST) == 0)
	{
		sysLogError(E_LLC_CABLE_PWR_ERROR, __LINE__, moduleName);
		return E_LLC_CABLE_PWR_ERROR;
	}
#endif

#if 0
	llcReadAllPhyReg();
#endif

	return NO_ERROR;
}


// Force a bus reset
HRESULT llcBusReset(uint8 forceRoot)
{
	HRESULT hResult;
	uint32 phyGCReg;

 	// Set the Initiate bus reset bit
	// and reset the gap count to be consistent after the bus reset
	phyGCReg = PHY_GC_IBR | PHY_GC_GAP_COUNT;
 	if (forceRoot && csrIsCMCCapable())
 	{
 		// Set the Root-holdoff bit RHB in the PHY register
 		phyGCReg |= PHY_GC_RHB;
 	}
	hResult = llcWritePhy(PHY_GC_ADR, phyGCReg);
		
	return hResult;
}

// Returns TRUE if bus reset is inprogress
BOOL llcIsBusReset(void)
{
	uint32 intReg;

	llcLinkRegRead(INTERRUPT_REG_DP, &intReg);
	if (intReg & LLC_INT_BIT(LLCID_BUS_RST_RXD))
	{
		return TRUE;
	}
	else
		return FALSE;
}

// Returns TRUE if a new bus reset in progress
// To be called during bus reset after selfID complete
BOOL llcIsNewReset(void)
{
	uint32 intReg;
	llcLinkRegRead(INTERRUPT_REG_DP, &intReg);
	if (intReg & LLC_INT_BIT(LLCID_BUS_RST_RXD))
	{
		return TRUE;
	}
	else
		return FALSE;
}

/*
	test to see if this node is root
*/
BOOL llcIsRoot(void)
{
	HRESULT hResult;
	uint32 phyIDAdr;

	hResult = llcReadPhy(PHY_ID_ADR, &phyIDAdr); 
	if (hResult != NO_ERROR) 
	{
		sysLogError(hResult, __LINE__, moduleName);
		return FALSE;
	}
	return (phyIDAdr & PHY_ID_ROOT);
}

/*
	get the current bus cycle time
*/
HRESULT llcGetCycleTime(uint32* time)
{
	llcLinkRegRead(CY_TMR_REG_DP, time);
	return NO_ERROR;
}

/*
	set Cycle Master function of the Link chip
*/
HRESULT llcSetCycleMaster(BOOL enabled)
{
	HRESULT	hResult = NO_ERROR;
	uint32 regVal;

	llcLinkRegRead(LCSR_REG_DP, &regVal);

	if ((regVal & LLC_AUTO_CY_MAST) == 0) 
	{	
		// Manual handling of setting cycle master
		if (enabled && csrIsCMCCapable())
		{
			// Set Cycle Master Capable bit
			llcLinkSetBits(LNK_CTRL_REG_DP, LLC_CMSTR_EN);
		}
		else
		{
			// Clear Cycle Master Capable bit
			llcLinkMaskAndChg(LNK_CTRL_REG_DP, LLC_CMSTR_EN, 0);
		}
	}
	return hResult;
}

BOOL llcIsCycleMaster(void)
{
	uint32	regVal;

	llcLinkRegRead(LNK_CTRL_REG_DP, &regVal);
	return (regVal & LLC_CMSTR_EN);
}

/*
	set Fairness Control value for this node
*/
void llcSetFairnessControl(uint8 pri_reg)
{
	UNUSED_ARG(pri_reg);
	//KBJ??? llcLinkRegWrite(LLCPRIBUDGET, pri_reg & LLCPRIREQMASK); // Set default Fairness Control Register 
}

/*
	Reads all Phy register into memory phyBaseReg, phyPageXReg 0,1 and 7
*/
void llcReadAllPhyReg(void) 
{
	uint8 i;

	for (i = 0; i < 8; i++) 
	{
		phyBaseReg[i] = 0;
		llcReadPhy(i, &phyBaseReg[i]); 
	}

	llcWritePhy(PHY_PAGE_SELECT, 0x00);
	for (i = 0; i < 2; i++)
	{
		phyPage0Port0Reg[i] = 0;
		llcReadPhy(i+8, &phyPage0Port0Reg[i]); 
	}

	llcWritePhy(PHY_PAGE_SELECT, 0x01);
	for (i = 0; i < 2; i++)
	{
		phyPage0Port1Reg[i] = 0;
		llcReadPhy(i+8, &phyPage0Port1Reg[i]); 
	}
	
	llcWritePhy(PHY_PAGE_SELECT, 0x02);
	for (i = 0; i < 2; i++)
	{
		phyPage0Port2Reg[i] = 0;
		llcReadPhy(i+8, &phyPage0Port2Reg[i]); 
	}

	llcWritePhy(PHY_PAGE_SELECT, 0x20);
	for (i = 0; i < 8; i++)
	{
		phyPage1Reg[i] = 0;
		llcReadPhy(i+8, &phyPage1Reg[i]); 
	}

	llcWritePhy(PHY_PAGE_SELECT, 0xe0);
	for (i = 0; i < 8; i++)
	{
		phyPage7Reg[i] = 0;
		llcReadPhy(i+8, &phyPage7Reg[i]); 
	}

}

HRESULT llcHandleCSRarch(uint32 llcArch, uint32 llcType, QUADLET *data)
{
	HRESULT		hResult = NO_ERROR;
	uint32		nodeIDReg;

	if (llcType == LLC_CSR_READ)
	{
		data[0] = 0;
		switch (llcArch)
		{
			case LLC_ARCH_STATE_CLEAR:
			case LLC_ARCH_STATE_SET:
				if (llcIsRoot() && llcIsCycleMaster())
				{
					*data |= CYCLE_MASTER_START;
				}
				break;
			case LLC_ARCH_NODEIDS:
				llcLinkRegRead(ND_ID_REG_DP, &nodeIDReg);
				*data |= (nodeIDReg&(LLC_BUS_ADDR_MASK|LLC_ND_ADDR_MASK))<<16;
				break;
			case LLC_ARCH_SPLIT_TO_HI:
				*data = LHL_TX_REQ_TIMEOUT_MSECS/1000 & 0x7;
				break;
			case LLC_ARCH_SPLIT_TO_LO:
				*data = (((LHL_TX_REQ_TIMEOUT_MSECS%1000) * 8 ) % 8000) << 19;
				break;
			case LLC_ARCH_RESET_START:
				hResult = E_NOT_IMPLEMENTED; //LM???
				break;
			default:
				hResult = E_NOT_IMPLEMENTED;
				return hResult;
		}
	}
	else if (llcType == LLC_CSR_WRITE)
	{
		switch (llcArch)
		{
#if 1 //LM??? allow writes to CSR Arch?
			case LLC_ARCH_STATE_CLEAR:
				if (llcIsCycleMaster())
				{
					llcSetCycleMaster(FALSE); //LM??? remember to update strategy for re-enabling CM
				}
				break;
			case LLC_ARCH_STATE_SET:
				if (llcIsRoot() && (llcIsCycleMaster() == FALSE))
				{
					llcSetCycleMaster(TRUE); //LM??? remember to update strategy for re-enabling CM
				}
				break;
			//SPLIT TIMEOUT WRITE is yet to decided
			case LLC_ARCH_SPLIT_TO_HI:
			case LLC_ARCH_SPLIT_TO_LO:
			case LLC_ARCH_NODEIDS:
				hResult = E_NOT_IMPLEMENTED; //LM???
				break;
			case LLC_ARCH_RESET_START:
				hResult = E_NOT_IMPLEMENTED; //LM???
				break;
#endif
			default:
				hResult = E_NOT_IMPLEMENTED;
				return hResult;
		}
	}
	else
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

HRESULT llcSoftwareReset(void)
{
	uint32 regVal = LLC_SOFT_RST;
	uint16 loop;

	llcLinkSetBits(LCSR_REG_DP, LLC_SOFT_RST);
	
	// Wait max. 1 sec. for LLC to be reset
	loop = 0;
	while ((regVal&LLC_SOFT_RST) && loop < 1000)
	{
		loop++;
		TCTaskWait(1);
		llcLinkRegRead(LCSR_REG_DP, &regVal);
	}
	return llcInitialize();
}
