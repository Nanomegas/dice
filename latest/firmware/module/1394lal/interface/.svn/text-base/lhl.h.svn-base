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
 * DESCRIPTION: Link Hardware Abstraction Layer

	Purpose:	Link Hardware Abstraction Layer
	
	Revisions:
		created 04/03/2002 lm
****************************************************************************/

#ifndef _LHL_H
#define _LHL_H

#include "1394.h"
#include "packetBlock.h"
#include "TCTypes.h"
#include "bmdefs.h"
#include "lhlDefs.h"
#include "lhlLLCDefs.h"

#ifdef _WINMP
#pragma warning( disable : 4514 )
#endif //_WINMP

// Initializes this module (messageQueues, semaphores, AR/AT Tasks (and resources) etc.
HRESULT			lhlInitializeTasks(void);
HRESULT			lhlResumeTasks(void);

HRESULT			lhlInitialize(void);
HRESULT			lhlDisable(void);

// misc utility functions
_TC_INLINE uint32	lhlGetMaxTxPayloadSizeBytes(void) _TC_INLINE_ALWAYS;
_TC_INLINE uint32	lhlGetMaxTxPayloadSizeBytes(void)
								{
									return LINK_MAXTXPAYLOAD_SIZE_BYTES;
								}


BOOL			lhlIsPayloadSizeValid(PB* packetBlock, uint16 numBytes, BOOL brx);

HRESULT			lhlGetHeaderQuadSizeFromTCode(uint16 axType, uint16 tCode, uint16 *headerNumQuads);
HRESULT			lhl1394GetHeaderQuadSizeFromTCode(uint16 axType, uint16 tCode, uint16 *headerNumQuads); // must be called from lhlLLCGetHeaderQuadSizeFromTCode (for handling LLC specific cases)
HRESULT			lhlGetHeaderSizeFromTCode(uint16 axType, uint16 tCode, uint16 *headerNumBytes);

HRESULT			lhlGetPayloadSize(uint16 tCode, uint16 headerNumQuads, QUADLET *qHeader, uint16 *payloadNumBytes);
HRESULT			lhlGetPayloadSizeFromTCode(uint16 tCode, uint16 *payloadNumBytes);

HRESULT			lhlCheckPacketType(uint32 genType, PB_PACKETTYPE packetType);
HRESULT			lhlGetRequestPacketType(uint32 genType, PB_PACKETTYPE packetTypeOrig, PB_PACKETTYPE *packetType);
HRESULT			lhlGetResponsePacketType(uint32 genType, PB_PACKETTYPE packetTypeOrig, PB_PACKETTYPE *packetType);

#endif
