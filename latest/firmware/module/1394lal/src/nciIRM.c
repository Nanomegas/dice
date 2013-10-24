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
 * DESCRIPTION: Node Controller Interface Isochronous Resource Manager

	Purpose: Node Controller Interface 
				Isochronous Resource Manager functions

	Revisions:
		created 04/30/2002 lm
****************************************************************************/

#include "TCTasking.h"
#include "lalDefs.h"
#include "lhl.h"
#include "nciIRM.h"
#include "nci.h"
#include "csr.h"
#include "bri.h"

#ifdef _IRM

static char 		moduleName[] = __FILE__;

extern LAL_BUS_RESET_INFO		nciInfoBlock;

BOOL nciIRMIsThisNodeIRM(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		nodeAddr = 0;

	hResult = nciIRMGetNodeAddrIRM(&nodeAddr);
	if (hResult != NO_ERROR) return FALSE;

	return briIsThisNodeAddr(nodeAddr);
}

HRESULT nciIRMGetNodeAddrIRM(uint32 *pAddr)
{
	HRESULT		hResult = NO_ERROR;

	if (nciInfoBlock.bValid != TRUE)
	{
		hResult = E_NCI_NODE_INFO_NOT_AVAILABLE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*pAddr = nciInfoBlock.irmAddr;

	return hResult;
}

#endif //_IRM

#ifdef _IRMC_CAPS

// Serial Bus IRM Registers
static IRM_REGS IrmRegs;

static uint32 nciIRMMutexSemID; // mutual exclusion semaphore for lock access (mutex)

/*****************************************************************************

	FUNCTION: nciIRMBRInitialize

	Description:
		Reset Isoc Resource Manager registers to bus-reset values
		this is an Isochronous Resource Manager function

******************************************************************************/

HRESULT	nciIRMReset(void)
{
	HRESULT		hResult = NO_ERROR;

	IrmRegs.BM_Id = BUS_MANAGER_ID_UNDEF;
	IrmRegs.BW_Avail = MAX_BWU_ISOCH;
	IrmRegs.CH_AvailHi = INITIAL_CHANNELS_AVAIL_HI;
	IrmRegs.CH_AvailLo = INITIAL_CHANNELS_AVAIL_LO;
	IrmRegs.Broadcast_Chan = DEFAULT_BROADCAST_CHAN;
	return hResult;
}

HRESULT	nciIRMInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

	// exclusive access for lock access (mutex)
	hResult = TCMutexOpen(&nciIRMMutexSemID);
	if (hResult != NO_ERROR) return hResult;

	hResult = nciIRMReset();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*****************************************************************************

	FUNCTION: nciIRMRead

	Description:
		Quadlet read access to IRM registers

*****************************************************************************/

RCODE_1394	nciIRMRead(uint32 irmReg, uint32* readVal)
{
	HRESULT		hResult = NO_ERROR;
	RCODE_1394	rCode = RSP_COMPLETE;

	if (nciIRMIsThisNodeIRM() == FALSE)
	{
		rCode = RSP_ADDRESS_ERROR;
		hResult = E_LHL_RSP_BASE + rCode;
		sysLogError(hResult, __LINE__, moduleName);
		return rCode;
	}

	switch (irmReg)
	{
		case CSR_SB_BM_ID:
			*readVal = IrmRegs.BM_Id;
			break;

		case CSR_SB_BW_AVAIL:
			*readVal = IrmRegs.BW_Avail;
			break;

		case CSR_SB_CH_AVAIL_HI:
			*readVal = IrmRegs.CH_AvailHi;
			break;

		case CSR_SB_CH_AVAIL_LO:
			*readVal = IrmRegs.CH_AvailLo;
			break;

		case CSR_SB_BROADCAST_CH:
			*readVal = IrmRegs.Broadcast_Chan;
			break;

		default:
			rCode = RSP_ADDRESS_ERROR;
			break;
	}

	return rCode;
}


/*****************************************************************************

	FUNCTION: nciIRMLock

	Description:
		Threadsafe lock access to IRM registers

*****************************************************************************/

RCODE_1394 nciIRMLock(uint32 irmReg, uint32 lockArg, uint32 lockData, uint32 *lockReturnData)
{
	HRESULT		hResult = NO_ERROR;
	RCODE_1394	rCode = RSP_COMPLETE;

	if (nciIRMIsThisNodeIRM() == FALSE)
	{
		rCode = RSP_ADDRESS_ERROR;
		hResult = E_LHL_RSP_BASE + rCode;
		sysLogError(hResult, __LINE__, moduleName);
		return rCode;
	}

	// Exclusive access for lock access (mutex)
	hResult = TCMutexLock(nciIRMMutexSemID);
	if (hResult != NO_ERROR)
	{
		rCode = RSP_ADDRESS_ERROR;
		return rCode;
	}

	switch (irmReg)
	{
		case CSR_SB_BM_ID:
			*lockReturnData = IrmRegs.BM_Id;
			if (lockArg == IrmRegs.BM_Id)
			{
				IrmRegs.BM_Id = lockData;
			}
			break;

		case CSR_SB_BW_AVAIL:
			*lockReturnData = IrmRegs.BW_Avail;
			if (lockArg == IrmRegs.BW_Avail)
			{
				IrmRegs.BW_Avail = lockData;
			}
			break;

		case CSR_SB_CH_AVAIL_HI:
			*lockReturnData = IrmRegs.CH_AvailHi;
			if (lockArg == IrmRegs.CH_AvailHi)
			{
				IrmRegs.CH_AvailHi = lockData;
			}
			break;

		case CSR_SB_CH_AVAIL_LO:
			*lockReturnData = IrmRegs.CH_AvailLo;
			if (lockArg == IrmRegs.CH_AvailLo)
			{
				IrmRegs.CH_AvailLo = lockData;
			}
			break;
		case CSR_SB_BROADCAST_CH:
			*lockReturnData = IrmRegs.Broadcast_Chan;
			if (lockArg == IrmRegs.Broadcast_Chan)
			{
				IrmRegs.Broadcast_Chan = lockData;;
			}
			break;

		default:
			rCode = RSP_ADDRESS_ERROR;
			break;
	}

	if (lockArg != *lockReturnData)
	{
		SYS_DEBUG(SYSDEBUG_TRACE_WARNINGS, "irm lock failed on irmReg: 0x%08x arg: 0x%08x, data: 0x%08x, returndata: 0x%08x\n\r", irmReg, lockArg, lockData, *lockReturnData);
	}

	// Release exclusive for lock access (mutex)
	TCMutexUnlock(nciIRMMutexSemID);

	return rCode;
}
/*****************************************************************************

	FUNCTION: nciIRMWrite

	Description:
		Threadsafe write access to IRM registers

*****************************************************************************/

RCODE_1394 nciIRMWrite(uint32 irmReg, uint32 data)
{
	HRESULT		hResult = NO_ERROR;
	RCODE_1394	rCode = RSP_COMPLETE;

	if (nciIRMIsThisNodeIRM() == FALSE)
	{
		rCode = RSP_ADDRESS_ERROR;
		hResult = E_LHL_RSP_BASE + rCode;
		sysLogError(hResult, __LINE__, moduleName);
		return rCode;
	}

	// Exclusive access for lock access (mutex)
	hResult = TCMutexLock(nciIRMMutexSemID);
	if (hResult != NO_ERROR)
	{
		rCode = RSP_ADDRESS_ERROR;
		return rCode;
	}

	switch (irmReg)
	{
		case CSR_SB_BROADCAST_CH:
			IrmRegs.Broadcast_Chan = data;;
			break;

		default:
			rCode = RSP_ADDRESS_ERROR;
			break;
	}

	// Release exclusive for lock access (mutex)
	TCMutexUnlock(nciIRMMutexSemID);

	return rCode;
}
#endif //_IRMC_CAPS
