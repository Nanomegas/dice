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
 * DESCRIPTION: Link Hardware Abstraction Layer LLC (Link Layer Control)

		lhl LLC Tools and Variables
	
		for handling LLC specific cases

	Revisions:
		created 09/06/2002 lm

****************************************************************************/

#ifndef _LHLLLC_H
#define _LHLLLC_H

#include "TCTypes.h"
#include "1394Defs.h"

HRESULT		lhlLLCGetHeaderQuadSizeFromTCode(uint16 axType, uint16 tCode, uint16 *headerNumQuads); // for handling LLC specific tcode cases

HRESULT		lhlLLCGetPayloadSize(uint16 tCode, uint16 headerNumQuads, QUADLET *qHeader, uint16 *payloadNumBytes);
HRESULT		lhlLLCSetPayloadSize(uint16 tCode, uint16 headerNumQuads, QUADLET *qHeader, uint16 payloadNumBytes);
HRESULT		lhlLLCGetPayloadSizeIndex(uint16 tCode, uint16 headerNumQuads, uint32 *index);

#ifdef _LOOSE_ISO
HRESULT		lhlLLCSetAsyncStreamChannel(uint8 channel, BOOL bEnabled);
HRESULT		lhlLLCSetAsyncStreamEnabled(BOOL bIsoEnabled);
HRESULT		lhlLLCSetAsyncStreamHeader(QUADLET *qHeader, uint8 channel, uint8 tag, uint8 sy);
HRESULT		lhlLLCGetAsyncStreamHeader(QUADLET *qHeader, uint8 *channel, uint8 *tag, uint8 *sy);
HRESULT		lhlLLCSetAsyncStreamQuad(QUADLET *quad, uint8 channel, uint8 tag, uint8 sy);
HRESULT		lhlLLCGetAsyncStreamQuad(QUADLET quad, uint8 *channel, uint8 *tag, uint8 *sy);
#endif //_LOOSE_ISO

HRESULT		lhlLLCHandleCSR(OFFSET_1394 destOffset, uint16 numBytes, QUADLET *data, uint16 reqType); // for handling LLC CSR operations
HRESULT		lhlLLCConfigROMUpdated(void);

HRESULT		lhlLLCResumeTasks(void);
HRESULT		lhlLLCSuspendTasks(void);
HRESULT		lhlLLCInitializeTasks(void);
HRESULT		lhlLLCInitialize(void);

#endif
