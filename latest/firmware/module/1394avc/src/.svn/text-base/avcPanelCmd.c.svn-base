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

	Implementation of avc panel subunit commands (Commands)

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
#include "avcPanelCmd.h"
#include "avcPanelDefs.h"

HRESULT avcPanelCmdGuiUpdate(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT					hResult = NO_ERROR;
	HRESULT					hResultError = E_PKT_AVC_NOT_IMPLEMENTED;

	if (avcPanelGetSubunitMode() != PANEL_DIRECT_MODE) return hResultError;

	UNUSED_ARG(pHeader);
	UNUSED_ARG(packetBlock);
	UNUSED_ARG(pStream);

	return hResult;
}

HRESULT avcPanelCmdPushGuiData(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT					hResult = NO_ERROR;
	HRESULT					hResultError = E_PKT_AVC_NOT_IMPLEMENTED;

	if (avcPanelGetSubunitMode() != PANEL_DIRECT_MODE) return hResultError;

	UNUSED_ARG(pHeader);
	UNUSED_ARG(packetBlock);
	UNUSED_ARG(pStream);

	return hResult;
}

HRESULT avcPanelCmdUserAction(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT					hResult = NO_ERROR;
	HRESULT					hResultError = E_PKT_AVC_NOT_IMPLEMENTED;

	if (avcPanelGetSubunitMode() != PANEL_DIRECT_MODE) return hResultError;

	UNUSED_ARG(pHeader);
	UNUSED_ARG(packetBlock);
	UNUSED_ARG(pStream);

	return hResult;
}

HRESULT avcPanelCmdPassThrough(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT					hResult = NO_ERROR;

	UNUSED_ARG(pHeader);
	UNUSED_ARG(packetBlock);
	UNUSED_ARG(pStream);

	return hResult;
}

