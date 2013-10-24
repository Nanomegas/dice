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

	Implementation of avc panel subunit commands (Responses)

	AV/C Panel Subunit Specification 1.21
	TA Document 2002009
	
  Revisions:
		created 17/09/2003 LM

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "avcDefs.h"
#include "avcPanel.h"
#include "avcPanelCli.h"
#include "avcPanelDefs.h"
#include "avcPanelRsp.h"
#include "cr.h"
#include "systemCallbacks.h"

HRESULT avcPanelRspGuiUpdate(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT					hResult = NO_ERROR;

	UNUSED_ARG(pHeader);
	UNUSED_ARG(packetBlock);
	UNUSED_ARG(pStream);

	return hResult;
}

HRESULT avcPanelRspPushGuiData(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT					hResult = NO_ERROR;

	UNUSED_ARG(pHeader);
	UNUSED_ARG(packetBlock);
	UNUSED_ARG(pStream);

	return hResult;
}

HRESULT avcPanelRspUserAction(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT					hResult = NO_ERROR;

	UNUSED_ARG(pHeader);
	UNUSED_ARG(packetBlock);
	UNUSED_ARG(pStream);

	return hResult;
}

HRESULT avcPanelRspPassThrough(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
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
			UNUSED_ARG(pStream);
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
