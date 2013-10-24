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

	Implementation of CLI for the avc panel subunit commands

  Revisions:
		created 17/09/2003 LM

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_AVC_PANEL
#include "TCTasking.h"
#include "avcDefs.h"
#include "avcPanelDefs.h"
#include "avcPanelCli.h"
#include "cli.h"
#include "datastream.h"
#include "avcCmdCli.h"

#define AVC_HEADER_BYTES				3									//  3 bytes: [fcp,ctype],[subunit type,ID],[opcode],
#define AVC_PANEL_GUI_UPDATE_BYTES		(AVC_HEADER_BYTES+ 5)				//  8 bytes: ,[operand0],..,[operand4]
#define AVC_PANEL_PUSH_GUI_DATA_BYTES	(AVC_HEADER_BYTES+ 10+3/*align*/)	// 13 bytes: ,[operand0],..,[operand9]
#define AVC_PANEL_USER_ACTION_BYTES		(AVC_HEADER_BYTES+ 10+3/*align*/)	// 13 bytes: ,[operand0],..,[operand9]
#define AVC_PANEL_PASS_THROUGH_BYTES	(AVC_HEADER_BYTES+ 3+2/*align*/)	//  6 bytes: ,[operand0],..,[operand2]

HRESULT avcPanelCliGuiUpdate(char cctype, uint8 subfunction)
{
	HRESULT				hResult = NO_ERROR;
	HRESULT				hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	DataStream			ds;
	uint8				data[AVC_PANEL_GUI_UPDATE_BYTES]; 
	uint8				opcode = AVC_CMD_PANEL_GUI_UPDATE; 
	uint8				ctype = 0;
	uint32				i;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), (uint32 *) data);
	if (hResult != NO_ERROR) return hResult;

	switch (ctype)
	{
		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_CONTROL:
			switch (subfunction)
			{
				case 0x00:	// open
					{
						hResult = dsWrite8Bits(&ds, 0xFF);			// operand[0] : source_plug (any available)
						if (hResult != NO_ERROR) return hResult;
						hResult = dsWrite8Bits(&ds, 0x00);			// operand[1] : generation_id (default)
						if (hResult != NO_ERROR) return hResult;
						hResult = dsWrite8Bits(&ds, 0x01);			// operand[2] : GUI_resolution (default)
						if (hResult != NO_ERROR) return hResult;
						hResult = dsWrite8Bits(&ds, subfunction);	// operand[3] : subfunction
						if (hResult != NO_ERROR) return hResult;
						hResult = dsWrite8Bits(&ds, 0x01);			// operand[4] : scope (current panel)
						if (hResult != NO_ERROR) return hResult;
					}
					break;
				default:
					return hResultError;
			}
			break;

		case AVC_CTYPE_GENERAL_INQUIRY:
		case AVC_CTYPE_STATUS:
			for (i = 0; i < 5; i++)
			{
				hResult = dsWrite8Bits(&ds, 0xFF);
				if (hResult != NO_ERROR) return hResult;
			}
			break;
		
		case AVC_CTYPE_NOTIFY:	
			return hResultError;
	}
	
	hResult = avcCmdCliSendRequest(ctype, sizeof(data), (uint32 *) data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcPanelCliPushGuiData(char cctype, uint8 subfunction)
{
	HRESULT				hResult = NO_ERROR;
	HRESULT				hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	DataStream			ds;
	uint8				data[AVC_PANEL_PUSH_GUI_DATA_BYTES]; 
	uint8				opcode = AVC_CMD_PANEL_PUSH_GUI_DATA; 
	uint8				ctype = 0;
	uint32				i;

	UNUSED_ARG(subfunction);

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), (uint32 *) data);
	if (hResult != NO_ERROR) return hResult;

	switch (ctype)
	{
		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_CONTROL:
			break;

		case AVC_CTYPE_GENERAL_INQUIRY:
		case AVC_CTYPE_STATUS:
			for (i = 0; i < 10; i++)
			{
				hResult = dsWrite8Bits(&ds, 0xFF);
				if (hResult != NO_ERROR) return hResult;
			}
			break;
		
		case AVC_CTYPE_NOTIFY:	
			return hResultError;
	}
	
	hResult = avcCmdCliSendRequest(ctype, sizeof(data), (uint32 *) data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcPanelCliUserAction(char cctype, uint8 subfunction)
{
	HRESULT				hResult = NO_ERROR;
	HRESULT				hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	DataStream			ds;
	uint8				data[AVC_PANEL_USER_ACTION_BYTES]; 
	uint8				opcode = AVC_CMD_PANEL_USER_ACTION; 
	uint8				ctype = 0;
	uint32				i;

	UNUSED_ARG(subfunction);

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), (uint32 *) data);
	if (hResult != NO_ERROR) return hResult;

	switch (ctype)
	{
		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_CONTROL:
			break;

		case AVC_CTYPE_GENERAL_INQUIRY:
		case AVC_CTYPE_STATUS:
			for (i = 0; i < 10; i++)
			{
				hResult = dsWrite8Bits(&ds, 0xFF);
				if (hResult != NO_ERROR) return hResult;
			}
			break;
		
		case AVC_CTYPE_NOTIFY:	
			return hResultError;
	}
	
	hResult = avcCmdCliSendRequest(ctype, sizeof(data), (uint32 *) data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avcPanelCliPassThrough(char cctype, uint8 operation_id, uint8 arg)
{
	HRESULT				hResult = NO_ERROR;
	HRESULT				hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	DataStream			ds;
	uint8				data[AVC_PANEL_PASS_THROUGH_BYTES]; 
	uint8				opcode = AVC_CMD_PANEL_PASS_THROUGH; 
	uint8				ctype = 0;
	uint32				i;
	uint8				state_flag = 0;
	uint8				operation_data_field_length = 0;

	hResult = avcCmdCliGetCType(cctype, &ctype);
	if (hResult != NO_ERROR) return hResult;

	hResult = avcCmdCliConstructRequestHeader(&ds, ctype, opcode, sizeof(data), (uint32 *) data);
	if (hResult != NO_ERROR) return hResult;

	switch (ctype)
	{
		case AVC_CTYPE_SPECIFIC_INQUIRY:
		case AVC_CTYPE_CONTROL:
			if (operation_id < 0x60)			// key pass-through functions
			{
				state_flag = (BYTE) (arg == 0);		// state_flag: button pressed

				hResult = dsWriteBits(&ds, 1, state_flag);
				if (hResult != NO_ERROR) return hResult;
			
				hResult = dsWriteBits(&ds, 7, operation_id);
				if (hResult != NO_ERROR) return hResult;

				operation_data_field_length = 0;

				hResult = dsWrite8Bits(&ds, operation_data_field_length);
				if (hResult != NO_ERROR) return hResult;
			}
			else if (operation_id <= 0x6a)		// deterministic functions
			{
				state_flag = 0;

				hResult = dsWriteBits(&ds, 1, state_flag);
				if (hResult != NO_ERROR) return hResult;
			
				hResult = dsWriteBits(&ds, 7, operation_id);
				if (hResult != NO_ERROR) return hResult;


				switch (operation_id)
				{
					case 0x60:					// play function
						{
							BYTE	playback_speed = 0;

							operation_data_field_length = 0x01;

							hResult = dsWrite8Bits(&ds, operation_data_field_length);
							if (hResult != NO_ERROR) return hResult;

							playback_speed = 0x38;		// could use arg
							hResult = dsWrite8Bits(&ds, playback_speed);
							if (hResult != NO_ERROR) return hResult;
						}
						break;
					default:
						return hResultError;
				}
			}
			else								// function keys and vendor unique functions
			{
				state_flag = 0;

				hResult = dsWriteBits(&ds, 1, state_flag);
				if (hResult != NO_ERROR) return hResult;
			
				hResult = dsWriteBits(&ds, 7, operation_id);
				if (hResult != NO_ERROR) return hResult;

				operation_data_field_length = 0;

				hResult = dsWrite8Bits(&ds, operation_data_field_length);
				if (hResult != NO_ERROR) return hResult;
			}
			break;

		case AVC_CTYPE_GENERAL_INQUIRY:
		case AVC_CTYPE_STATUS:
			for (i = 0; i < 3; i++)
			{
				hResult = dsWrite8Bits(&ds, 0xFF);
				if (hResult != NO_ERROR) return hResult;
			}
			break;
		
		case AVC_CTYPE_NOTIFY:	
			return hResultError;
	}
	
	hResult = avcCmdCliSendRequest(ctype, sizeof(data), (uint32 *) data, &ds);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

#define AVC_PANEL_CONTROL_OPERATION_ITEMS (47+1)

typedef struct
{
	uint8	id;
	char	name[16];
	char	description[64];
} AVC_PANEL_CONTROL_OPERATION;

static const AVC_PANEL_CONTROL_OPERATION avcPanelCliControlOperations[AVC_PANEL_CONTROL_OPERATION_ITEMS] =
{
	{0x00, "select", "Select"},		
	{0x01, "up", "Up"},
	{0x02, "down", "Down"},
	{0x03, "left", "Left"},
	{0x04, "right", "Right"},
	{0x05, "r-up", "Right-up"},
	{0x06, "r-down", "Right-down"},
	{0x07, "l-up", "Left-up"},
	{0x08, "l-down", "Left-down"},
	{0x09, "root", "Root menu"},
	{0x0a, "setup", "Setup menu"},
	{0x0b, "content", "Contents menu"},
	{0x0c, "favorit", "Favorite menu"},
	{0x0d, "exit", "Exit"},
	{0x20, "0", "0"},
	{0x21, "1", "1"},
	{0x22, "2", "2"},
	{0x23, "3", "3"},
	{0x24, "4", "4"},
	{0x25, "5", "5"},
	{0x26, "6", "6"},
	{0x27, "7", "7"},
	{0x28, "8", "8"},
	{0x29, "9", "9"},
	{0x2a, "dot", "Dot"},
	{0x2b, "enter", "Enter"},
	{0x2c, "clear", "Clear"},
	{0x33, "sound", "Sound Select"},
	{0x35, "display", "Display information"},
	{0x37, "p-up", "Page up"},
	{0x38, "p-down", "Page Down"},
	{0x44, "play", "Play"},
	{0x45, "stop", "Stop"},
	{0x46, "pause", "Pause"},
	{0x48, "rew", "Rewind"},
	{0x49, "ff", "Fast forward"},
	{0x4a, "eject", "Eject"},
	{0x4b, "fward", "Forward"},
	{0x4c, "bward", "Backward"},
	{0x50, "angle", "Angle"},
	{0x51, "sub-p", "Sub picture"},
	{0x71, "f1", "F1"},
	{0x72, "f2", "F2"},
	{0x73, "f3", "F3"},
	{0x74, "f4", "F4"},
	{0x75, "f5", "F5"},
	{0x7e, "vendor", "Vendor unique"},
	{0xFF, "show", "Show user operation list"}	//always last entry
};

HRESULT avcPanelCliControlShow(void)
{
	HRESULT		hResult = NO_ERROR;

	uint32		i;

	cliPrintf("User operations:\n\r");

	for (i = 0; i < AVC_PANEL_CONTROL_OPERATION_ITEMS - 1; i++)
	{
		cliPrintf("\t%s\t- %s\n\r", avcPanelCliControlOperations[i].name, avcPanelCliControlOperations[i].description);
	}

	return hResult;
}

HRESULT avcPanelCliControl(char *name)
{
	HRESULT		hResult = NO_ERROR;
	char		cctype = 'c';
	uint8		operation_id = 0;
	uint8		arg = 1;
	uint32		i;

	for (i = 0; i < AVC_PANEL_CONTROL_OPERATION_ITEMS; i++)
	{
		if (strncmp(name, avcPanelCliControlOperations[i].name, strlen(name)) == 0)
		{
			break;
		}
	}
	if (i == AVC_PANEL_CONTROL_OPERATION_ITEMS)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		return hResult;
	}
	if (i == AVC_PANEL_CONTROL_OPERATION_ITEMS - 1)
	{
		hResult = avcPanelCliControlShow();
		return hResult;
	}

	operation_id = avcPanelCliControlOperations[i].id;

	hResult = avcPanelCliPassThrough(cctype, operation_id, arg);

	return hResult;
}

// AV/C Panel Gui Update Tool
static  char	avcPanelCliGuiUpdate_UsageString[]=\
				"panel:gui: \n\r"\
				"use: panel:gui <ctype> <subfuntion>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"subfunction: ...\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc panel gui update command\n\r";

static	CLIDescriptor	avcPanelCliGuiUpdate_Descriptor[]=
{
	{kCLI_Name,				(uint32)"gui"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcPanelCliGuiUpdate},
	{kCLI_UsageString, 		(uint32)avcPanelCliGuiUpdate_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ci"},
	{kCLI_ModuleName,		(uint32)"sends avc panel gui update command"},
	{kCLI_GroupName,		(uint32)"panel"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Panel Push Gui Data Tool
static  char	avcPanelCliPushGuiData_UsageString[]=\
				"panel:push: \n\r"\
				"use: panel:push <ctype> <subfuntion>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"subfunction: ...\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc panel push gui data command\n\r";

static	CLIDescriptor	avcPanelCliPushGuiData_Descriptor[]=
{
	{kCLI_Name,				(uint32)"push"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcPanelCliPushGuiData},
	{kCLI_UsageString, 		(uint32)avcPanelCliPushGuiData_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ci"},
	{kCLI_ModuleName,		(uint32)"sends avc panel push gui data command"},
	{kCLI_GroupName,		(uint32)"panel"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Panel User Action Tool
static  char	avcPanelCliUserAction_UsageString[]=\
				"panel:user: \n\r"\
				"use: panel:user <ctype> <subfuntion>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"subfunction: ...\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc panel user action command\n\r";

static	CLIDescriptor	avcPanelCliUserAction_Descriptor[]=
{
	{kCLI_Name,				(uint32)"user"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcPanelCliUserAction},
	{kCLI_UsageString, 		(uint32)avcPanelCliUserAction_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ci"},
	{kCLI_ModuleName,		(uint32)"sends avc panel user action command"},
	{kCLI_GroupName,		(uint32)"panel"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Panel Pass Through Tool
static  char	avcPanelCliPassThrough_UsageString[]=\
				"panel:pass: \n\r"\
				"use: panel:pass <ctype> <operation_id> <arg>\n\r"\
				"ctype: control:c, status:s, specific inq.:i, notify:n, general inq.:g\n\r"\
				"operation_id: [0x00;0x60[ key pass-through functions\n\r"\
				"         arg: 1:key pressed 0:key released (state_flag)\n\r"\
				"operation_id: [0x60;0x6a] deterministic functions\n\r"\
				"         arg: 0:operation specific arguments (n/a)\n\r"\
				"operation_id: [0x71;0x75] function keys F1,..,F5\n\r"\
				"         arg: 0:(n/a)\n\r"\
				"operation_id: 0x7E vendor unique\n\r"\
				"         arg: 0:(n/a)\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc panel pass through command\n\r";

static	CLIDescriptor	avcPanelCliPassThrough_Descriptor[]=
{
	{kCLI_Name,				(uint32)"pass"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcPanelCliPassThrough},
	{kCLI_UsageString, 		(uint32)avcPanelCliPassThrough_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"cii"},
	{kCLI_ModuleName,		(uint32)"sends avc panel pass through request"},
	{kCLI_GroupName,		(uint32)"panel"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// AV/C Panel Pass Through Control Tool
static  char	avcPanelCliControl_UsageString[]=\
				"panel:ui: \n\r"\
				"use: panel:ui <user operation>\n\r"\
				"user operation: use \"show\" to get list\n\r"\
				"------------------------------------------------------------\n\r"\
				"sends avc panel pass through command\n\r";

static	CLIDescriptor	avcPanelCliControl_Descriptor[]=
{
	{kCLI_Name,				(uint32)"ui"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avcPanelCliControl},
	{kCLI_UsageString, 		(uint32)avcPanelCliControl_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"s"},
	{kCLI_ModuleName,		(uint32)"sends avc panel pass through command"},
	{kCLI_GroupName,		(uint32)"panel"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void avcPanelCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(avcPanelCliGuiUpdate_Descriptor);
	hResult += cliInstallCLIDescriptor(avcPanelCliPushGuiData_Descriptor);
	hResult += cliInstallCLIDescriptor(avcPanelCliUserAction_Descriptor);
	hResult += cliInstallCLIDescriptor(avcPanelCliPassThrough_Descriptor);
	hResult += cliInstallCLIDescriptor(avcPanelCliControl_Descriptor);
	
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing avc panel cli tools");
	}
}

#endif //_CLI_TOOL_AVC_PANEL
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
