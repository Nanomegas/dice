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
 * DESCRIPTION: Command Line Interface

	Purpose: 

		This file describes the Command Line Interpreter (cli) API.
	
  Revisions:
		created 05/01/2002 lm
****************************************************************************/

/** @addtogroup CLI

	The Command Line Interpreter enables modules to install tools that can be invoked
	from the command prompt via a serial terminal.  This enables modules to be tested
	and debugged.

	For details on using the existing command prompt tools, please see @ref CliUsage "Command Line Prompt Usage".

	@{
*/

#ifndef _CLI_H
#define _CLI_H

#ifdef _CLI

#include "TCTypes.h"
#include "keyValue.h"
#include "cliSIO.h"

//	Defines
#define	kCLIMaxArguments			6

//	Types: These are the types of CLIDescriptors, you can create:
#define kCLI_TYPE_VARIABLE			'vari' // Value associated with a kCLI_TYPE key, this makes the descriptor an Environment Variable.
#define kCLI_TYPE_TOOL				'tool' // Value associated with a kCLI_TYPE key, this makes the descriptor a TOOL reference. 

//	DisplayTypes: These are the different ways the results of an environment variable can be displayed.
#define kCLI_DISPLAYTYPE_HEX		0x01 //	Associated with a variable to make it display in hex, when listed in the environment list.
#define kCLI_DISPLAYTYPE_DECIMAL	0x00 // Associated with a variable to make it display in decimal mode. Not necessary to include this, as the default is decimal mode. 
#define kCLI_DISPLAYTYPE_BINARY		0x02 // Associated with a variable to make it display in binary, (a list of 0's and 1's).
#define kCLI_DISPLAYTYPE_BOOLEAN	0x03 // Associated with a variable to make it display in a Boolean.  

/**
    These keys are used when describing an external command for
    the CLI environment. 
*/ 
enum CLIDescriptorKeys
{
	kCLI_Name=1,					//!< Specify the NAME of the variable or the Tool
	kCLI_Type,						//!< Specify the TYPE, either a kCLI_TYPE_VARIABLE, or a kCLI_TYPE_TOOL
	kCLI_Function,					//!< When a tool, this defines the function to call
	kCLI_Variable,					//!< When a variable, this is a pointer to a uint32 which can be modified.
	kCLI_VariableDisplayType,		//!< When shown, in the environment type, display as hex or decimal, or binary.
	kCLI_VariableSize,				//!< Default is 32bits, but this can describe the number of bits, in muliples of 8.
	kCLI_VariableBitFields,			//!< When included, this key allows for bits in a variable to be defined as fields.  
	kCLI_UsageString,				//!< When a tool or variable this describes the usage of the variable.
	kCLI_ArgumentPatternString,		//!< When a tool, this describes the ARGUMENTS which are expected.
	kCLI_ConstantsString,			//!< When a tool or variable, this describes CONSTANTS which can be used, and their valus. Constants associated with this are formatted as "const1=1,const2=2,..."
	kCLI_ModuleName,				//!< Name of the module or file which contains this tool.
	kCLI_GroupName,					//!< Name of the group this command is associated with.
	kCLI_Hidden						//!< When included, this key hides the command in the menus.
};

/**	
	VariableCallback Operations
*/
#define	kCLI_VariableCallbackOperation_GET		'get '
/**	
	VariableCallback Operations
*/
#define kCLI_VariableCallbackOperation_SET		'set '

// definitions
#if defined (_AVC) //LM??? increase max cli tools items when adding other modules
/** The maximum number of tools that can be installed.  increase max cli tools items when adding other modules
*/
#define kMaxTools					512
#else //_AVC
#define kMaxTools					256 // 100
#endif //_AVC
#define kMaxGroups					25	//!< The maximum number of tool groups
#define kMaxEnvironment				70  //!< The maximum number of environment variables
#define kMaxArgElements				30  //!< The maximum number of argument elements
#define kMaxCommandLine				79	//!< The maximum command buffer size
#define kTempBufferSize				80
#define CLI_DISPLAY_COLUMNSIZE		20  //!< Used to define column sizes.
#define CLI_DISPLAY_GROUPCOLUMNSIZE 12  //!< used to define the column size for groups	

//! Use the CLIDescriptor instead of KEYVALUE* because it may change in the future.
typedef	KEYVALUE	CLIDescriptor;

/**
	Command Function Callback
	
	This is what the function prototype for a Tool looks like., this is used when a tool
	does not specify an ArgumentPatternString, in the descriptor it installs. 
*/
typedef  HRESULT (*CLIToolCallback)(uint32 argc, char** argv);

/**
	Variable Changed Callback
	
	This is called when the user changes a value from the CLI. 
*/
typedef  HRESULT (*CLIVariableChangedCallback)(uint32 operation, char* varname, uint32	oldvalue, uint32* newvalue);

/*
	TYPEDEFS FOR MAKING THE CALLBACKS

	These are used to make the callbacks from the CLI into the CLITool.  Don't
	use these yourself, they are only here to coerce the C runtime into doing
	what we need it to do.
*/
typedef	 HRESULT (*CLIToolCallback_0ARG)(void);
typedef  HRESULT (*CLIToolCallback_1ARG)(void* );
typedef  HRESULT (*CLIToolCallback_2ARG)(void*, void*);
typedef  HRESULT (*CLIToolCallback_3ARG)(void*, void*, void*);
typedef  HRESULT (*CLIToolCallback_4ARG)(void*, void*, void*, void*);
typedef  HRESULT (*CLIToolCallback_5ARG)(void*, void*, void*, void*, void*);
typedef  HRESULT (*CLIToolCallback_6ARG)(void*, void*, void*, void*, void*, void*);

/**
	This function registers a tool within the CLI, so that it may be referred
	to from the command line.  This enables you to install either an environment
	variable, or a Tool. 
	
	The following is a sample tool definition:

	@code
	char*				SimpleToolUsageString=\
	"simple\n\r"\
	"------------------------------------------------------------\n\r"\
	"Usage: simple\n\r"\
	"This tool actually does nothing, and intends to accomplish about"\
	"the same thing. It is primarily meant to be used for nothing\n\r"; 
	
	CLIDescriptor		SimpleTool=
	{
		{kCLI_Name,				(uint32)"simple"},
		{kCLI_Type, 			kCLI_TYPE_TOOL},
		{kCLI_Function, 		(uint32)&SimpleToolCallback},
		{kCLI_UsageString, 		(uint32)SimpleToolUsageString},
		{kCLI_ConstantsString,	(uint32)"TRUE=1,FALSE=0,YES=1,NO=0,ON=1,OFF=0"},
		{KV_TERMINATORKEY,KV_TERMINATORVALUE}
	};
	@endcode
	
	
	The following is a sample environment variable definition:

	@code
	char*	Register1Usage=\
	"Register1:\n\r"\
	"bit0 = Led 1 Setting, may be toggled\n\r"\
	"bit2 = Led 2 Setting, may be toggled\n\r";
	
	BYTE	registers[10];
	
	CLIDescriptor		Register1=
	{
		{kCLI_Name,					(uint32)"Register1"},
		{kCLI_Type, 				kCLI_TYPE_VARIABLE},
		{kCLI_Function,				(uint32)&SimpleVariableChangedCallback,
		{kCLI_Variable,				(uint32)&(registers[0])},
		{kCLI_VariableSize,			8},
		{kCLI_VariableDisplayType,  kCLI_DISPLAYTYPE_BINARY},
		{kCLI_UsageString, 			(uint32)Register1Usage},
		{kCLI_ConstantsString,		(uint32)"TRUE=1,FALSE=0,YES=1,NO=0,ON=1,OFF=0"},
		{KV_TERMINATORKEY,KV_TERMINATORVALUE} 
	};
	@endcode
	<h2>
	Rules
	</h2>

	UsageString:
		The contents of a Usage String can be about just anything. However there are certain
		things which will cause particular behavior in the cli.
		
		1. Usage of "\n\r" to terminate lines in usage string.
		2. Usage of a single "-" on a line prints an entire line. 
		
	ArgumentPatternString:
		The contents of the argument pattern string, are that each character in the 
		string represents an argument and it's type in your tool's function.
		The following are the valid characters allowed in the ArgumentPatternString:
		'i' - any integer value, including constants defined in the ConstantsString.
		's' - any string value.
		'k' - only a constant value may be used for this argument.
		'a' - only a fully qualified hexadecimal number can be used. (ie 0x12345678)
		'h' - only a hexadecimal nubmer cab be used.
		'd' - only an integer value may be used.
	
		All uppercase values are also valid. They have the special meaning that the 
		rest of parameters in the ArgumentPatternString is optional.

	 ConstantsString:
		This declares constants which are in the scope of the current tool. This string
		must have the following format:
		"variable1=1,variable2=2,......"
		The strings may have no spaces in them, and the constant value must be of a NUMBER. It can
		be a hex number, but it must be a number.
	

	@param newEntry			 The new entry you wish to install into the CLI.

*/
HRESULT			cliInstallCLIDescriptor(CLIDescriptor* newEntry);


HRESULT			cliInitializeTasks(void);						//!< Initializes of this module's tasks
HRESULT 		cliResumeTasks(void);							//!< resume of this module's tasks
HRESULT 		cliSuspendTasks(void);							//!<  suspend of this module's tasks

HRESULT			cliInitialize(void);							//!<  Initializes this module
HRESULT			cliProcessCommand(char* cliStr);				//!< This function process a cli command described by cliStr

// private
HRESULT			cliLookUpConstant(CLIDescriptor* tool,char* constantname, uint32* returnedvalue);
HRESULT			cliStrToArgs(char* cl,char** arglist,uint32* argcount);
CLIDescriptor*	cliLookupDescriptorByName(char* group, char* name);
HRESULT			cliCallCommand(CLIDescriptor* tool, uint32 argc, char** argv);
BOOL			cliDisplayCommands(char* group);
HRESULT			cliProcessArgs(uint32 count, char** args);
void			cliParseGroupCommand(char* arg, char* groupname, char* commandname);
int32			cliFindChar(char* src, char findchar);
void			cliPrintVariableFields(uint32 value,char* variablefields);

//! @}
#endif //_CLI

#endif //_CLI_H


