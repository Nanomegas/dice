//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2005 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
	
	Purpose:	myAppCli.c - Initialization of OEMs Command Line Interface
	
	Revisions:
		created 02/02/2005 arg

****************************************************************************/
#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_MYAPP
#include "TCTasking.h"
#include "myApp.h"
#include "myAppCli.h"
#include "cli.h"

// Below is a sample CLI descriptor.	SEE THE RULES IN cli.h	See more examples in other module's Cli files.

#if 0

// My Application's CLI Useage String (help for the end user)
static  char	myAppCli_UsageString[]=\
				"myCli: \n\r"\
				"use: myCli <arg> <arg> <...>\n\r"\
				"you can put more descriptive help here\n\r"\
				"------------------------------------------------------------\n\r"\
				"Describe the function here";

static	CLIDescriptor	myAppCli_Descriptor[]=
{
	{kCLI_Name,				(uint32)"myCli"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&myAppCliFunction},
	{kCLI_UsageString, 		(uint32)myAppCli_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},					
	{kCLI_ConstantsString,	(uint32)"myConst=0"},
	{kCLI_ModuleName,		(uint32)"My Applications Command Line Command"},
	{kCLI_GroupName,		(uint32)"myApp"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

#endif // #if 0

void myAppCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	// This is where the developer will put her CLI tool installs
	// for example:
	//	hResult += cliInstallCLIDescriptor(MyAppCli_Descriptor);
	//  ...
	
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing myApp cli tools");
	}
}

#endif //_CLI_TOOL_MYAPP
#endif //_CLI
