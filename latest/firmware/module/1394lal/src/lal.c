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
 * DESCRIPTION: Link Abstraction Layer

	Purpose:	Link Abstraction Layer
	
	Revisions:
		created 04/09/2002 lm
****************************************************************************/

#include "TCTasking.h"
#include "lal.h"
#include "lhl.h"
#include "lhlBri.h"
#include "lhlMsgQueue.h"
#include "nci.h"
#include "nciBM.h"
#include "nciIRM.h"
#include "nciHandles.h"
#include "bri.h"
#include "csr.h"

#ifdef _CLI
#include "csrCli.h"
#include "cli.h"
#include "cliCB.h"
#include "lalCli.h"
#include "lhlCli.h"
#endif //_CLI

#if defined (_SYSDEBUG) || defined (_SYSDEBUGERROR)
#include "sysDebug.h"
#endif //_SYSDEBUG || _SYSDEBUGERROR
#ifdef _PROFILE
#include "sysProfile.h"
#endif //_PROFILE

static char 		moduleName[] = __FILE__ ;

static BOOL bLalInitialized = FALSE;

static uint32 lalWWUIDHigh = 0;
static uint32 lalWWUIDLow = 0;

HRESULT lalInitialize1394Tasks(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = lhlInitializeTasks();				//
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT lalResume1394Tasks(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = lhlResumeTasks();					//
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT lalInitialize1394(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = pbInitialize() ;					//
	if (hResult != NO_ERROR) return hResult;

	hResult = lhlInitialize();					//
	if (hResult != NO_ERROR) return hResult;

	hResult = briInitialize();					//
	if (hResult != NO_ERROR) return hResult;

	hResult = lalInitialize();					// before nci
	if (hResult != NO_ERROR) return hResult;

	hResult = nciInitialize();					//
	if (hResult != NO_ERROR) return hResult;

#if defined (_SYSDEBUG) || defined (_SYSDEBUGERROR)
	hResult = sysInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_SYSDEBUG || _SYSDEBUGERROR

#ifdef _PROFILE
	hResult = sysProfileInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_PROFILE

	hResult = lalResume1394Tasks();
	if (hResult != NO_ERROR) return hResult;
	
	return hResult;
}

HRESULT lalInitialize1394Finish(void)
{
	HRESULT		hResult = NO_ERROR;

	// Force bus reset after everything is initialized
#ifdef _IRMC_CAPS // iso_resource_manager capable
	hResult = lalSetCycleMaster(TRUE);
	hResult = lalForceRoot();
#else
	hResult = lalForceBusReset(); 
#endif

	return hResult;
}

// Initialize the LAL module
HRESULT lalInitialize(void)
{
	HRESULT		hResult = NO_ERROR;

	if (bLalInitialized)
	{
		hResult = E_LAL_ALREADY_INITIALIZED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	bLalInitialized = TRUE;

	hResult = lalAddressRangeInitialize();
	if (hResult != NO_ERROR) return hResult;

#ifdef _LOOSE_ISO
	hResult = lalAsyncStreamInitialize();
	if (hResult != NO_ERROR) return hResult;
#endif //_LOOSE_ISO

#ifdef _CLI_TOOL_LAL
	lalCliInstallTools();
#endif //_CLI_TOOL_LAL

#ifdef _CLI_TOOL_CSR
	csrCliInstallTools();
#endif //_CLI_TOOL_CSR

	return hResult;
}

HRESULT lalDisable1394(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult = lhlDisable();	// Disabling of lhl layer (link layer controller)
	if (hResult != NO_ERROR) return hResult;

	//LM??? more...
	
	return hResult;
}

// To be called before initialisation
void lalSetWWUID(uint32 WWUIDHigh, uint32 WWUIDLow)
{
	lalWWUIDHigh = WWUIDHigh;
	lalWWUIDLow = WWUIDLow;
}

// Used when creating Configuration ROM
HRESULT lalGetWWUID(uint32 *WWUIDHigh, uint32 *WWUIDLow)
{
	HRESULT		hResult = NO_ERROR;

	*WWUIDHigh = lalWWUIDHigh;
	*WWUIDLow = lalWWUIDLow;

	if ((lalWWUIDHigh == 0) && (lalWWUIDLow == 0)) 
	{
		hResult = E_LAL_WWWUID_NOTFOUND;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

HRESULT lalGetThisHandle(LAL_NODE_HANDLE* pHandle)
{
	HRESULT	hResult = NO_ERROR;
	uint32	nodeAddr;

	hResult = lalGetThisNodeAddr(&nodeAddr);
	if (hResult != NO_ERROR) return hResult;

	hResult = lalGetHandleFromNodeAddr(nodeAddr, pHandle);

	return hResult;
}

#ifdef _BM
HRESULT lalGetHandleBM(LAL_NODE_HANDLE* pHandle)
{
	HRESULT	hResult = NO_ERROR;
	uint32	nodeAddr;

	hResult = lalGetNodeAddrBM(&nodeAddr);
	if (hResult != NO_ERROR) return hResult;

	hResult = lalGetHandleFromNodeAddr(nodeAddr, pHandle);

	return hResult;
}

HRESULT lalGetHandleSpeed(LAL_NODE_HANDLE handle, LAL_SPEED *speed)
{
	HRESULT		hResult = NO_ERROR;
	uint32		nodeAddr = 0;

	// verify handle
	if (nciIsValidHandle(handle))
	{
		// get nodeAddr for transaction
		hResult = lalGetNodeAddrFromHandle(handle, &nodeAddr);
		if (hResult != NO_ERROR) return hResult;
	}

	hResult = lalGetNodeAddrSpeed(nodeAddr, speed);

	return hResult;
}
#endif //_BM

#ifdef _IRM
HRESULT lalGetHandleIRM(LAL_NODE_HANDLE* pHandle)
{
	HRESULT	hResult = NO_ERROR;
	uint32	nodeAddr;

	hResult = lalGetNodeAddrIRM(&nodeAddr);
	if (hResult != NO_ERROR) return hResult;

	hResult = lalGetHandleFromNodeAddr(nodeAddr, pHandle);

	return hResult;
}
#endif //_IRM

HRESULT lalGetHandleRoot(LAL_NODE_HANDLE* pHandle)
{
	HRESULT	hResult = NO_ERROR;
	uint32	nodeAddr;

	hResult = lalGetNodeAddrRoot(&nodeAddr);
	if (hResult != NO_ERROR) return hResult;

	hResult = lalGetHandleFromNodeAddr(nodeAddr, pHandle);

	return hResult;
}

/*
	given a nodeAddress, find the corresponding device handle.  if a handle has
	not been created for the target, a new one is supplied.
*/
HRESULT lalGetHandleFromNodeAddr(uint32 nodeAddr, LAL_NODE_HANDLE* pHandle)
{
	HRESULT				hResult	= NO_ERROR;
	LAL_DEVICE_INFO		lalDevInfo;

	hResult = nciGetHandleFromNodeAddr(nodeAddr, pHandle);

	if (hResult == E_NCI_HANDLE_NOT_FOUND)
	{
		hResult = nciGetNodeInfo(nodeAddr, &lalDevInfo);

		if (hResult == NO_ERROR)
		{
			hResult = nciCreateHandle(lalDevInfo.WWUID.hi, lalDevInfo.WWUID.low, pHandle);
		}
	}

	return hResult;
}

HRESULT lalGetHandleFromPacketSrcNodeAddr(PB* packetBlock, LAL_NODE_HANDLE* pHandle)
{
	HRESULT		hResult	= NO_ERROR;
	uint32		nodeAddr = 0;

	hResult = pbGetPacketSrcNodeAddr (packetBlock, &nodeAddr);
	if (hResult != NO_ERROR) return hResult;

	hResult = lalGetHandleFromNodeAddr(nodeAddr, pHandle);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT lalGetWWUIDFromNodeAddr(uint32 nodeAddr, WWUID *wwuid)
{
	HRESULT				hResult	= NO_ERROR;
	LAL_DEVICE_INFO		devInfo;
	
	hResult = lalGetDeviceInfo(nodeAddr, &devInfo);
	if (hResult != NO_ERROR) return hResult;

	wwuid->hi = devInfo.WWUID.hi;
	wwuid->low = devInfo.WWUID.low;

	return hResult;
}

HRESULT lalGetWWUIDFromHandle(LAL_NODE_HANDLE handle, WWUID *wwuid)
{
	HRESULT				hResult	= NO_ERROR;
	uint32				nodeAddr;

	hResult = lalGetNodeAddrFromHandle(handle, &nodeAddr);
	if (hResult != NO_ERROR) return hResult;

	hResult = lalGetWWUIDFromNodeAddr(nodeAddr, wwuid);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT lalGetNodeAddrFromWWUIDSpecial(WWUID wwuid, uint32* pNodeAddr, BOOL bMatchWWUIDHi, BOOL bMatchWWUIDLo, BOOL bAllowLocalNode)
{
	HRESULT				hResult	= NO_ERROR;
	uint32				localnode = 0;
	uint16				busId = 0;
	uint16				nodeIndex;
	uint16				nodeAddr = 0;
	LAL_DEVICE_INFO		devInfo;
	BOOL				bFound = FALSE;

	if (bMatchWWUIDHi == TRUE &&
		wwuid.hi == 0)
	{
		*pNodeAddr = wwuid.low;
		return hResult;
	}

	lalGetBusID(&busId);
	lalGetThisNodeAddr(&localnode);
	
	for (nodeIndex = 0; nodeIndex < lalGetNumNodes(); nodeIndex++)
	{
		nodeAddr = (uint16) (nodeIndex | busId);

		hResult = lalGetDeviceInfo(nodeAddr, &devInfo);
		if (hResult != NO_ERROR) return hResult;

		if (bAllowLocalNode == TRUE ||
			nodeAddr != localnode)
		{
			if (bMatchWWUIDHi == FALSE ||
				devInfo.WWUID.hi == wwuid.hi)
			{
				if (bMatchWWUIDLo == FALSE ||
					devInfo.WWUID.low == wwuid.low)
				{
					*pNodeAddr = nodeAddr;
					bFound = TRUE;
					break;
				}
			}
		}
	}

	if (bFound == FALSE)
	{
		hResult = E_FAIL;
	}

	return hResult;
}

HRESULT lalGetHandleFromWWUIDSpecial(WWUID wwuid, LAL_NODE_HANDLE* pHandle, BOOL bMatchWWUIDHi, BOOL bMatchWWUIDLo, BOOL bAllowLocalNode)
{
	HRESULT		hResult = NO_ERROR;
	uint32		nodeAddr = 0;

	hResult = lalGetNodeAddrFromWWUIDSpecial(wwuid, &nodeAddr, bMatchWWUIDHi, bMatchWWUIDLo, bAllowLocalNode);
	if (hResult != NO_ERROR) return hResult;

	hResult = lalGetHandleFromNodeAddr(nodeAddr, pHandle);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/********************************
** Misc						   **
********************************/

/*
	get the maximum payload size capabilities of the specifed target node
*/
HRESULT lalGetMaxRecordSizeBytes(uint32 nodeAddr, uint16* maxSizeInBytes)
{
	HRESULT				hResult	= NO_ERROR;
	LAL_DEVICE_INFO		lalDevInfo;
	uint32				max_rec = 0;

	// get max_rec from BIB
	hResult = lalGetDeviceInfo(nodeAddr, &lalDevInfo);
	if (hResult != NO_ERROR) return hResult;

	// get max_rec and lookup in table
	max_rec = lalDevInfo.maxRec;

	switch (max_rec)
	{
		case BUSINFO_MAXREC4:
			*maxSizeInBytes = 4;
			break;

		case BUSINFO_MAXREC8:
			*maxSizeInBytes = 8;
			break;

		case BUSINFO_MAXREC16:
			*maxSizeInBytes = 16;
			break;

		case BUSINFO_MAXREC32:
			*maxSizeInBytes = 32;
			break;

		case BUSINFO_MAXREC64:
			*maxSizeInBytes = 64;
			break;

		case BUSINFO_MAXREC128:
			*maxSizeInBytes = 128;
			break;

		case BUSINFO_MAXREC256:
			*maxSizeInBytes = 256;
			break;

		case BUSINFO_MAXREC512:
			*maxSizeInBytes = 512;
			break;

		case BUSINFO_MAXREC1024:
			*maxSizeInBytes = 1024;
			break;

		case BUSINFO_MAXREC2048:
			*maxSizeInBytes = 2048;
			break;

		default:
			hResult = E_LAL_UNKNOWN_MAX_REC;
			// error
			break;
	}

	return hResult;
}

