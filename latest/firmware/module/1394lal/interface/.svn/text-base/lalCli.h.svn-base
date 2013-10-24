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
  * DESCRIPTION: Link Abstraction Layer Command Line Interface

		lal CLI Tools and Variables
	
		These are used by the CLI to view lal info.

	Revisions:
		created 09/09/2002 lm

***********************************************************************/

#ifndef _LALCLI_H
#define _LALCLI_H
#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_LAL

#include "packetBlock.h"
#include "TCTypes.h"
#include "lalDefs.h"

HRESULT		lalCliRead(LAL_NODE_HANDLE hHandle, uint16 nodeAddr, uint16 destOffsetHi, uint32 destOffsetLow, uint16 numBytes);
HRESULT		lalCliRead1394(LAL_NODE_HANDLE hHandle, uint16 destOffsetHi, uint32 destOffsetLow, uint16 numBytes);
HRESULT		lalCliReadNode(uint16 nodeAddr, uint16 destOffsetHi, uint32 destOffsetLow, uint16 numBytes);

HRESULT		lalCliWrite(LAL_NODE_HANDLE hHandle, uint16 nodeAddr, uint16 destOffsetHi, uint32 destOffsetLow, uint32 numBytes, uint32 data);
HRESULT		lalCliWrite1394(LAL_NODE_HANDLE hHandle, uint16 destOffsetHi, uint32 destOffsetLow, uint32 numBytes, uint32 data);
HRESULT		lalCliWriteNode(uint16 nodeAddr, uint16 destOffsetHi, uint32 destOffsetLow, uint32 numBytes, uint32 data);

HRESULT		lalCliTestWrite(LAL_NODE_HANDLE hHandle, uint16 nodeAddr, uint16 destOffsetHi, uint32 destOffsetLow, uint16 numBytes);
HRESULT		lalCliTestWrite1394(LAL_NODE_HANDLE hHandle, uint16 destOffsetHi, uint32 destOffsetLow, uint16 numBytes);
HRESULT		lalCliTestWriteNode(uint16 nodeAddr, uint16 destOffsetHi, uint32 destOffsetLow, uint16 numBytes);

HRESULT		lalCliLock(LAL_NODE_HANDLE hHandle, uint16 nodeAddr, uint16 destOffsetHi, uint32 destOffsetLow, uint32 arg, uint32 data);
HRESULT		lalCliLock1394(LAL_NODE_HANDLE hHandle, uint16 destOffsetHi, uint32 destOffsetLow, uint32 arg, uint32 data);
HRESULT		lalCliLockNode(uint16 nodeAddr, uint16 destOffsetHi, uint32 destOffsetLow, uint32 arg, uint32 data);

HRESULT		lalCliSMR(uint32 nodeAddr);
HRESULT		lalCliDisplayAllocatedRanges(void);
#ifdef _LOOSE_ISO
HRESULT		lalCliAsyncStream1394(uint32 channel, uint32 tag, uint32 sy, uint32 numBytes);
HRESULT		lalCliAsyncStreamMonitor1394(uint32 channel);
HRESULT		lalCliAsyncStreamMonitorCB(PB *packetBlock);
#endif //_LOOSE_ISO

void		lalCliInstallTools(void);

#endif //_CLI_TOOL_LAL
#endif //_CLI
#endif //_LALCLI_H
