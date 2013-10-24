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

		Implementation of CLI for the global callback registry.

  Revisions:
		created 21/03/2003 LM

****************************************************************************/

#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_AVC_CR
#include "TCTasking.h"
#include "cr.h"
#include "crCli.h"
#include "cli.h"

//static char 		moduleName[] = __FILE__ ;

HRESULT crCliList(void)
{
	CALLBACK_DESCRIPTOR*	cur = NULL;
	CALLBACK_DESCRIPTOR*	next = NULL;
	char*					name = NULL;
	uint32					type = 0;

	cliPrintf("Displaying contents of all callback descriptors\n\r");
	cliOutputLine("Address      Type      Name");
		
	if (crFindCallback(CR_FIND_CALLBACK_FIRST, 0, 0, 0, &cur) == TRUE)
	{
		kvGetValue(cur, CALLBACK_DESCRIPTOR_NAME, (uint32 *) &name);
		kvGetValue(cur, CALLBACK_DESCRIPTOR_TYPE, &type);
		cliPrintf("0x%08x   0x%04x    %s\n\r", cur, type, (name) ? name : "un-named");
	}

	while (crFindCallback(CR_FIND_CALLBACK_NEXT, 0, 0, cur, &next) == TRUE)
	{
		kvGetValue(next, CALLBACK_DESCRIPTOR_NAME, (uint32*) &name);
		kvGetValue(next, CALLBACK_DESCRIPTOR_TYPE, &type);
		cliPrintf("0x%08x   0x%04x    %s\n\r", next, type, (name) ? name : "un-named");
		cur = next;
		if (cur == NULL) break;
	}
		
	return NO_ERROR;			
}

HRESULT crCliShow(KEYVALUE* descriptor)
{
	cliPrintf("Displaying contents of callback descriptor 0x%08x\n\r", descriptor);

	while (descriptor->key != KV_TERMINATEKEY)
	{
		cliPrintf("key:0x%08x val:0x%08x\n\r",descriptor->key, descriptor->value);
		descriptor++;	
	}
	
	return NO_ERROR;
}

// List Tool
static  char	crList_UsageString[]=\
				"cr:list: \n\r"\
				"use: cr:list\n\r"\
				"------------------------------------------------------------\n\r"\
				"This function shows all registered callbacks";
static	CLIDescriptor	crList_Descriptor[]=
{
	{kCLI_Name,				(uint32)"list"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&crCliList},
	{kCLI_UsageString, 		(uint32)crList_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"List all callbacks"},
	{kCLI_GroupName,		(uint32)"cr"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// Show Tool
static  char	crShow_UsageString[]=\
				"cr:show: \n\r"\
				"use: cr:show <address>\n\r"\
				"------------------------------------------------------------\n\r"\
				"shows key value pairs for a callback descriptor\n\r";
				
static	CLIDescriptor	crShow_Descriptor[]=
{
	{kCLI_Name,				(uint32)"show"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&crCliShow},
	{kCLI_UsageString, 		(uint32)crShow_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"a"},
	{kCLI_ModuleName,		(uint32)"Show a specific callback"},
	{kCLI_GroupName,		(uint32)"cr"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

void crCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(crList_Descriptor);
	hResult += cliInstallCLIDescriptor(crShow_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing cr cli tools");
	}
}
#endif //_CLI_TOOL_AVC_CR
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
