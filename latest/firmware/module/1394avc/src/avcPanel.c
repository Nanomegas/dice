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

	Implementation of avc panel subunit commands

	AV/C Panel Subunit Specification 1.21
	TA Document 2002009
	
	Panel Subunit Commands	(Direct Mode)

										ctype			target
										C,S,N			U,S
		GUI UPDATE						M,M,-			-,+
		PUSH GUI DATA					M,M,-			-,+
		USER ACTION						O,-,-			-,+
		PASS THROUGH					O,-,-			-,+


	Panel Subunit Commands	(Indirect Mode)

										ctype			target
										C,S,N			U,S
		PASS THROUGH					M,-,-			-,+

			ctype:
			C:CONTROL, S:STATUS, N:NOTIFY
			M:Mandatory, R:Recommended, O:Optional, -:Not Defined, V:Vendor dependent

			target
			U:UNIT, S:SUBUNIT

  Revisions:
		created 17/09/2003 LM

****************************************************************************/

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "avcDefs.h"
#include "avcPanel.h"
#include "avcPanelCli.h"
#include "avcPanelCmd.h"
#include "avcPanelDefs.h"
#include "avcPanelRsp.h"
#include "avcUnitCli.h"
#include "avcUnitUtils.h"
#include "avcUtils.h"
#include "cli.h"
#include "cr.h"
#include "systemCallbacks.h"

// Subunit Callback Descriptor for the PANEL subunit type.
static CALLBACK_DESCRIPTOR AVC_PANEL_CALLBACK[] =
{
	{CALLBACK_DESCRIPTOR_TYPE,AVC_SUBUNIT_TYPE},
	{CALLBACK_DESCRIPTOR_NEXT,0},
	{CALLBACK_DESCRIPTOR_NAME,(uint32)"AVC_PANEL_CALLBACK"},
	{AVC_SUBUNIT_CALLBACK_KEY,(uint32)&avcPanelCallback}, // CALLBACK PTR of TYPE AVC_SUBUNIT_CALLBACK},
	{AVC_SUBUNIT_SUBUNITTYPE_KEY,AVC_SU_TYPE_PANEL},
	{AVC_SUBUNIT_ID_KEY,AVC_SU_ID_IGNORE},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static AVC_PANEL_SUBUNIT avcPanelSubunit;

AVC_PANEL_MODE avcPanelGetSubunitMode(void)
{
	return avcPanelSubunit.mode;
}

HRESULT avcPanelCallback(AVC_HEADER *pHeader, PB *packetBlock)
{
	HRESULT			hResult = NO_ERROR;
	HRESULT			hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	pDataStream		pStream = NULL;
	
	hResult = pbGetApplicationDatastream(packetBlock, &pStream);
	if (hResult != NO_ERROR) return hResultError;

#ifdef _SYSDEBUG
	if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
	{
		avcUnitCliPrintOpcode(pHeader->opcode);
	}
#endif //_SYSDEBUG

	if (avcCtypeIsCommand(pHeader->ctype) == TRUE)
	{
		// commands
#ifdef _SYSDEBUG
		if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
		{
			if (avcUnitCliIsPrintMode(AVC_UNIT_PRINT_RCV_COMMAND_ALL))
			{
				avcUnitCliPrintFrame(pHeader, pStream);
			}
		}
#endif //_SYSDEBUG
		switch (pHeader->opcode)
		{
			case AVC_CMD_PANEL_GUI_UPDATE:		// M : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
				hResult = avcPanelCmdGuiUpdate(pHeader, packetBlock, pStream);
				break;

			case AVC_CMD_PANEL_PUSH_GUI_DATA:	// M : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
				hResult = avcPanelCmdPushGuiData(pHeader, packetBlock, pStream);
				break;
			
			case AVC_CMD_PANEL_USER_ACTION:		// O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
				hResult = avcPanelCmdUserAction(pHeader, packetBlock, pStream);
				break;
			
			case AVC_CMD_PANEL_PASS_THROUGH:	// O,M: M:Mandatory, R:Recommended, O:Optional, -:Not Defined
				hResult = avcPanelCmdPassThrough(pHeader, packetBlock, pStream);
				break;
			default:
				hResult = hResultError;
				break;
		}
	}
	else if (avcCtypeIsResponse(pHeader->ctype) == TRUE)
	{
		// response

		if ((avcUnitCliIsPrintMode(AVC_UNIT_PRINT_RCV_RESPONSE_ALL)) ||
			(avcUnitCliIsPrintMode(AVC_UNIT_PRINT_RCV_RESPONSE_IMPLEMENTED) && pHeader->ctype != AVC_RESPONSE_NOT_IMPLEMENTED) ||
			(avcUnitCliIsPrintMode(AVC_UNIT_PRINT_RCV_RESPONSE_NOT_IMPLEMENTED) && pHeader->ctype == AVC_RESPONSE_NOT_IMPLEMENTED))
		{
#ifdef _SYSDEBUG
			if (sysDebugIsEnabled(SYSDEBUG_TRACE_AVC))
			{
				avcUnitCliPrintFrame(pHeader, pStream);
			}
#endif //_SYSDEBUG
			switch (pHeader->opcode)
			{
				case AVC_CMD_PANEL_GUI_UPDATE:		// M : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
					hResult = avcPanelRspGuiUpdate(pHeader, packetBlock, pStream);
					break;

				case AVC_CMD_PANEL_PUSH_GUI_DATA:	// M : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
					hResult = avcPanelRspPushGuiData(pHeader, packetBlock, pStream);
					break;
				
				case AVC_CMD_PANEL_USER_ACTION:		// O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
					hResult = avcPanelRspUserAction(pHeader, packetBlock, pStream);
					break;
				
				case AVC_CMD_PANEL_PASS_THROUGH:	// O,M: M:Mandatory, R:Recommended, O:Optional, -:Not Defined
					hResult = avcPanelRspPassThrough(pHeader, packetBlock, pStream);
					break;
				default:
					hResult = hResultError;
					break;
			}
		}
	}
	else
	{
		// unknown ctype
		hResult = E_PKT_AVC_NOT_IMPLEMENTED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	return hResult;
}

// AVC Panel initialization
HRESULT avcPanelInitialize(void)
{
	HRESULT			hResult = NO_ERROR;

	hResult = crAddCallback(AVC_PANEL_CALLBACK);
	if (hResult != NO_ERROR) return hResult;

#ifdef _CLI_TOOL_AVC_PANEL
	avcPanelCliInstallTools();
#endif //_CLI_TOOL_AVC_PANEL

#if 1 //LM???
	avcPanelSubunit.status = PANEL_CLOSED;
	avcPanelSubunit.mode = PANEL_DIRECT_MODE;
#endif

	return hResult;
}
