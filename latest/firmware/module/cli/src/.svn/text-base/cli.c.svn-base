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

static char 		moduleName[] = __FILE__ ;

#ifdef _CLI

#include <stdlib.h>

#include "TCTasking.h"
#include "cli.h"
#include "cliBuiltIn.h"

#ifdef _CLI_CB
#include "cliCB.h"
#endif

#include "cliSIO.h"
#include "cliSystem.h"
#include "sysCli.h"
#include "listManager.h"
#include "format.h"
#ifdef _SYSDEBUG
#include "ErrorText.h"
#endif //_SYSDEBUG

#ifdef _WINMP
//#pragma warning(push)
#pragma warning( disable : 4055 )
//#pragma warning(pop)
#endif

// structs
typedef	struct	group
{
	char	name[CLI_DISPLAY_COLUMNSIZE];
} group_entry;

// These lists are used to keep track of the different tools, 
// and variables which are registered with the CLI.

char*				cliConstants="true=1,false=0,on=1,off=0,TRUE=1,FALSE=0,ON=1,OFF=0";

uint32				cliToolList_BUFFER[(LM_LISTSIZE(kMaxTools,sizeof(void*))+3)/4];
LM_CONTEXT*			cliToolList = (LM_CONTEXT*)cliToolList_BUFFER;

uint32				cliEnvironmentList_BUFFER[(LM_LISTSIZE(kMaxEnvironment,sizeof(void*))+3)/4];
LM_CONTEXT*			cliEnvironmentList = (LM_CONTEXT*)cliEnvironmentList_BUFFER;

group_entry			cliGroupList[kMaxGroups];

BOOL				cliInitialized = FALSE;

/***************************************************************************
***************************************************************************/
HRESULT cliInitializeTasks(void)
{
	HRESULT	hResult = NO_ERROR;

	// cli Task Initialization
	hResult = cliSystemInitializeTasks();
	if (hResult != NO_ERROR) return hResult;

#ifdef _CLI_CB
	hResult = cliCBInitializeTasks();
	if (hResult != NO_ERROR) return hResult;
#endif

	return hResult;
}

HRESULT cliResumeTasks(void)
{
	HRESULT	hResult = NO_ERROR;

	// cli Task resume
	hResult = cliSystemResumeTasks();
	if (hResult != NO_ERROR) return hResult;


#ifdef _CLI_CB
	hResult = cliCBResumeTasks();
	if (hResult != NO_ERROR) return hResult;
#endif

	return hResult;
}

HRESULT cliSuspendTasks(void)
{
	HRESULT	hResult = NO_ERROR;

	// cli Task suspend
	hResult = cliSystemSuspendTasks();
	if (hResult != NO_ERROR) return hResult;

#ifdef _CLI_CB
	hResult = cliCBSuspendTasks();
	if (hResult != NO_ERROR) return hResult;
#endif

	return hResult;
}

HRESULT cliInitialize(void)
{
	HRESULT	hResult = NO_ERROR;

	// Don't allow double initializations
	if (cliInitialized) return hResult;
	cliInitialized = TRUE;
	
	// Initialize Lists used for storing the tools and variables in.
	lmCreateList(cliToolList, LM_LISTSIZE(kMaxTools, sizeof(void *)), sizeof(void *), NULL);
	lmCreateList(cliEnvironmentList, LM_LISTSIZE(kMaxEnvironment, sizeof(void *)), sizeof(void *), NULL);
	
	// initialize the group list
	memset(cliGroupList, 0, sizeof(cliGroupList));
	
	cliBuiltInInstallTools();

	hResult = cliResumeTasks();
	if (hResult != NO_ERROR) return hResult;
       
	return hResult;
}

/*
	This function installs a tool or variable, into the CLI so that it can be used
	by the user, from the command line.
*/
HRESULT	cliInstallCLIDescriptor(CLIDescriptor* newEntry)
{
	HRESULT			hResult = NO_ERROR;
	uint32			value = 0;
	char*			name = 0;
	char*			groupname = 0;
	group_entry*	cur_group = cliGroupList;
	BOOL			bFound = FALSE;

	// We do this for special case tools which may get initialized before the CLI interfaces do
	if (cliInitialized == FALSE)
	{
		cliInitialize();
	}

	// A type is always required.
	hResult = kvIsKey(newEntry, kCLI_Type);
	if (hResult != NO_ERROR)
	{
		hResult = E_CLI_BADDESCRIPTORTYPE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
		
	// Make sure that the descriptor is a valid type.
	hResult = kvGetValue(newEntry, kCLI_Type, &value);
	if (hResult != NO_ERROR)
	{
		hResult = E_CLI_BADDESCRIPTORTYPE;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	switch (value)
	{
		case kCLI_TYPE_VARIABLE:
			break;
		case kCLI_TYPE_TOOL:
			// if an incoming descriptor is a TOOL the it must have function associated with it
			hResult = kvIsKey(newEntry, kCLI_Function);
			if (hResult != NO_ERROR)
			{
				hResult = E_CLI_BADDESCRIPTORFUNCTION;
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
			}
			break;
		default:
			hResult = E_CLI_BADDESCRIPTORTYPE;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
	}
	
	// A name is required for either a TOOL or a Variable
	hResult = kvIsKey(newEntry, kCLI_Name);
	if (hResult != NO_ERROR)
	{
		hResult = E_CLI_BADDESCRIPTORNAME;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	kvGetValue(newEntry, kCLI_Name, (uint32*) &name);
	kvGetValue(newEntry, kCLI_GroupName, (uint32*) &groupname);

	// Check to see if this an already registered command or variable.
	if (cliLookupDescriptorByName(groupname ? groupname : "", name))
	{
		hResult = E_CLI_DUPLICATECOMMAND;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	// Loop through all groups and see if this is an already registered group.
	// If it isn't then add it's name to the group list.
	if (groupname != NULL)
	{
		bFound = FALSE;

		while ((cur_group->name)[0] != 0)
		{
			if (strcmp(cur_group->name, groupname)==0)
			{
				bFound = TRUE;
				break;
			}
			cur_group++;
		}
		
		if (bFound == FALSE)
		{
			strcpy(cur_group->name,groupname);
		}
	}	
	
	// Add the descriptor to the appropriate list.
	switch (value)
	{
		case kCLI_TYPE_VARIABLE:
			hResult = lmAddElement(cliEnvironmentList, &(newEntry), NULL);
			if (hResult != NO_ERROR)
			{
				uint32	allocated = 0;
				lmGetAllocatedCount (cliEnvironmentList, &allocated);
				if (allocated >= kMaxEnvironment)
				{
					cliOutputLine("CLI ERROR: Can't add more variables, increase kMaxEnvironment");
					hResult = E_CLI_OUTOFSPACE;
					sysLogError(hResult, __LINE__, moduleName);
				}
			}
			break;
		case kCLI_TYPE_TOOL:
			hResult = lmAddElement(cliToolList, &(newEntry), NULL);
			if (hResult != NO_ERROR)
			{
				uint32	allocated = 0;
				lmGetAllocatedCount (cliToolList, &allocated);
				if (allocated >= kMaxTools)
				{
					cliOutputLine("CLI ERROR: Can't add more tools, increase kMaxTools");
					hResult = E_CLI_OUTOFSPACE;
					sysLogError(hResult, __LINE__, moduleName);
				}
			}
			break;
	}

	return hResult;
}

HRESULT cliProcessCommand(char* cliStr)
{
	HRESULT		hResult = NO_ERROR;
	char		tempStr[kTempBufferSize];
	char*		arglist[kMaxArgElements];
	uint32		argcount = 0;

	if (strlen(cliStr) > kTempBufferSize - 1)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	strcpy(tempStr, cliStr);

	hResult = cliStrToArgs(tempStr,arglist,&argcount);
	if (hResult != NO_ERROR) return hResult;

	hResult = cliProcessArgs(argcount,arglist);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/* 
	This function does look up for a particular variable or tool. It will attempt to look up either.
*/
#if 1 //LM??? optimize
CLIDescriptor* cliLookupDescriptorByName(char* group, char* name)
{
	CLIDescriptor*	pDesc = NULL;
	uint32			counter = 0;
	char*			toolname = 0;
	char*			groupname = 0;
	char*			varname = 0;
	char			empty = 0;
	LM_ELEMENT*		element = 0;

	// First try to find the specified descriptor as a tool
	element = (LM_ELEMENT*) &(cliToolList->elements);
	for (counter = 0; counter < cliToolList->elemcount; counter++)
	{
		if (lmCheckElementAllocated(element) == NO_ERROR)
		{
			kvGetValue(element->data, kCLI_Name, (uint32*)&toolname);
			kvGetValue(element->data, kCLI_GroupName, (uint32*)&groupname);

			if (groupname == 0)
			{
				groupname = &empty;
			}
			if (strcmp(toolname, name) == 0 &&
				strcmp(groupname, group) == 0)
			{
				pDesc = (CLIDescriptor *) element->data;
				break;
			}
		}
		lmMoveAheadElementSize(cliToolList->elemsize, &element);
	}
	
	if (pDesc == NULL)
	{
		// Can't find the specified descriptor as a tool, so lookup through the environment variables.
		element = (LM_ELEMENT*) &(cliEnvironmentList->elements);
		for (counter = 0; counter < cliEnvironmentList->elemcount; counter++)
		{
			if (lmCheckElementAllocated(element) == NO_ERROR)
			{
				kvGetValue(element->data, kCLI_Name, (uint32*)&varname);

				if (strcmp(varname, name) == 0)
				{
					pDesc = (CLIDescriptor *) element->data;
					break;
				}
			}
			lmMoveAheadElementSize(cliEnvironmentList->elemsize, &element);
		}
	}

	return pDesc;
}
#else
CLIDescriptor* cliLookupDescriptorByName(char* group, char* name)
{
	CLIDescriptor*	pDesc = NULL;
	CLIDescriptor**	elem = 0;
	uint32			counter = 0;
	uint32			index = 0;

	// First try to find the specified descriptor as a tool
	while (lmGetNthElement(cliToolList, (void **) &elem, counter++, &index) == NO_ERROR)
	{
		char*	toolname = 0;
		char*	groupname = 0;
		char	empty = 0;
		
		kvGetValue(*elem, kCLI_Name, (uint32*)&toolname);
		kvGetValue(*elem, kCLI_GroupName, (uint32*)&groupname);
		
		lmReleaseElement(cliToolList, index);

		if (groupname == 0)
		{
			groupname = &empty;
		}
		if (strcmp(toolname, name) == 0 &&
			strcmp(groupname, group) == 0)
		{
			pDesc = *elem; 
			break;
		}
	}

	if (pDesc == NULL)
	{
		// Can't find the specified descriptor as a tool, so lookup through the environment variables.
		counter = 0;
		
		while (lmGetNthElement(cliEnvironmentList, (void **) &elem, counter++, &index) == NO_ERROR)
		{
			char* varname = 0;
			
			kvGetValue(*elem, kCLI_Name, (uint32*)&varname);
			
			lmReleaseElement(cliEnvironmentList, index);

			if (strcmp(varname, name) == 0)
			{
				pDesc = *elem;
				break;
			}
		}
	}
	
	return pDesc;
}
#endif
/*
	cliDisplayCommands
	
	This function looks through the list of commands and then displays them.
	This could have been implemented as a tool I suppose, but it fits in better as just a simple function.
*/
BOOL cliDisplayCommands(char* group)
{
	HRESULT			hResult = NO_ERROR;
	uint32			counter = 0;
	CLIDescriptor**	elem = 0;
	uint32			index = 0;
	char			tempstring[kTempBufferSize];
	char			spacer[40];
	uint8			groupcount = 0;
	group_entry*	curgroup = cliGroupList;
	BOOL			bFound = FALSE;

	// Display the name of the group, when we are actually looking for a group.
	if (group[0] == 0)
	{
		cliOutputLine("[Root]:");
		cliOutputLine("------------------------------------------------------------");

		// Display the group list
		while ((curgroup->name)[0] != 0)
		{
			uint32	whitecount = CLI_DISPLAY_GROUPCOLUMNSIZE - (strlen(curgroup->name) + 2);
			
			cliOutputRaw("[");
			cliOutputRaw(curgroup->name);
			cliOutputRaw("]");
			
			while (whitecount--)
			{
				cliOutputRaw(" ");
			}
			
			groupcount++;
			
			if (groupcount % 4 == 0)
			{
				cliOutputLine(" ");
			}
			
			curgroup++;
		}
		cliOutputLine(" ");
	}
	else
	{
		// Check to see if the specified group exists.
		bFound = FALSE;
		while ((curgroup->name)[0] != 0)
		{
			if (strcmp(curgroup->name, group) == 0)
			{
				bFound = TRUE;
				break;
			}
			curgroup++;
		}
		
		if (bFound == FALSE) 
		{
			return FALSE;	
		}

		cliOutputRaw("[");
		cliOutputRaw(group);
		cliOutputLine("]:");	
		cliOutputLine("------------------------------------------------------------");
	}
	
	while (lmGetNthElement(cliToolList, (void **) &elem, counter++, &index) == NO_ERROR)
	{
		char*	toolname = 0;
		char*	groupname = 0;
		char*	modulename = 0;
		uint32	clitype = 0;
		
		kvGetValue(*elem, kCLI_Name, (uint32*)&toolname);
		kvGetValue(*elem, kCLI_ModuleName, (uint32*)&modulename);
		kvGetValue(*elem, kCLI_Type, &clitype); 
		kvGetValue(*elem, kCLI_GroupName, (uint32*)&groupname);
		
		// Check to see if the tool is a hidden tool.
		hResult = kvIsKey(*elem, kCLI_Hidden);
		if (hResult == NO_ERROR)
		{
			lmReleaseElement(cliToolList,index);
			continue;	
		}
		
		// Don't display this command if it is not in the appropriate group listing.
		if (groupname)
		{
			if (strcmp(groupname,group) != 0)
			{
				lmReleaseElement(cliToolList,index);				
				continue;
			}	
		}
		else
		{
			if (group[0] != 0)
			{
				lmReleaseElement(cliToolList,index);
				continue;
			}	
		}
		
		memset(spacer, ' ', ARRAYSIZE(spacer));
		spacer[CLI_DISPLAY_COLUMNSIZE - strlen(toolname)] = 0;
		
		if (clitype == kCLI_TYPE_TOOL)
		{
			if (modulename)
			{
				sprintf(tempstring,"%s%s(%s)",toolname,spacer,modulename);
			}
			else
			{
				sprintf(tempstring,"%s%s(unknown)",toolname,spacer);
			}
		}
				
		cliOutputLine(tempstring);
		lmReleaseElement(cliToolList,index);		
	}	
	
	return TRUE;
}

/*
	This function divides the entire command line and turns it into list of char*'s.  
	
	This is accomplished by putting a "\0" in between each of the arguments as found in the command line.
*/
HRESULT cliStrToArgs(char* cl, char** arglist, uint32 *argcount)
{
	HRESULT		hResult = NO_ERROR;
	BOOL		quoteopen=0;
	char**		curarg = arglist;
	
	formatCondenseWhitespace(cl);
	
	*argcount = 1;
	*curarg = cl;
		
	while(*cl)
	{
		switch (*cl)
		{	
			case ' ':
				if (quoteopen)
				{
					break;	
				}
				*cl = 0;
				curarg++;
				(*argcount)++;
				*curarg = cl + 1;
				break;
			
			case '\"':
				if (quoteopen == FALSE)
				{
					quoteopen = TRUE;
				}
				else
				{
					quoteopen = FALSE;
				}
				break;
		}
		cl++;
	}
	
	return hResult;
}

HRESULT cliProcessArgs(uint32 count, char** args)
{
	HRESULT			hResult = NO_ERROR;
	CLIDescriptor*	tool = NULL;
	char			groupname[40];
	char			commandname[40];
	
	if ((count == 0) || 
		(count == 1 && strlen(args[0]) == 0))
	{
		return hResult;
	}
	
	// Handle the single question mark case
	if (strcmp(args[0], "?") == 0)
	{
		cliDisplayCommands("");
		return hResult;
	}
	
	cliParseGroupCommand(args[0], groupname, commandname);
	
	tool = cliLookupDescriptorByName(groupname, commandname);

	if (tool == NULL)
	{
		// if not a tool then it must be a group, display the commands associated with this group.
		if (cliDisplayCommands(commandname) == FALSE)
		{
			cliOutputLine("CLI ERROR: Command/Variable not found.  Try ? for help.");
			hResult = E_CLI_UNKNOWNCOMMAND;
			sysLogError(hResult, __LINE__, moduleName);
		}
		return hResult;
	}
	
	if (count >= 2 && strcmp(args[1], "?") == 0)
	{
		char* line = 0;
		
		hResult = kvIsKey(tool, kCLI_UsageString);
		if (hResult != NO_ERROR)
		{
			cliOutputLine("CLI ERROR: Command/Variable does not have a usage string.");
		}	
		else
		{
			kvGetValue(tool, kCLI_UsageString,(uint32*) &line);
			cliOutputLine(line);
		}
		return hResult;
	}
	
	hResult = cliCallCommand(tool, count, args);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*
	cliLookUpConstant
	
	This function checks the constantname against all the different constants
	which are registered with the tool or variable.  Also it checks to see if
	constant is a GLOBAL constant.
	
	Consts MUST be stored in the following form:
	"CONST1=1,CONST2=2,..."
*/

HRESULT cliLookUpConstant(CLIDescriptor*  tool, char* constantname, uint32* returnedvalue) //LM??? not checked or finalized
{
	HRESULT		hResult = NO_ERROR;
	char		temp[kTempBufferSize] = {0};
	char		tempstring[kTempBufferSize * 2];
	BOOL		globalcheck = FALSE;
	char*		p = 0;
	char*		toolname = 0;
	uint32		value = 0;
	char*		name = NULL;
	char*		valstr = NULL;
	BOOL		bFound = FALSE;
	BOOL		bBreakLoop = FALSE;
	
	memset(temp, 0, kTempBufferSize);
	
	kvGetValue(tool, kCLI_Name, (uint32*)&toolname);
	
	//	First portion of the problem is just to find a CONST.  Since Consts
	//	are divided by commas, we just look for the first item which is 
	//	ended by a comma, then we take that and search it for the name
	//  and value.
	
	// Check to see if there is anything in the constant string.
	hResult = kvIsKey(tool, kCLI_ConstantsString);
	if (hResult == NO_ERROR)
	{
		kvGetValue(tool, kCLI_ConstantsString, (uint32*) &p);
	}
	else
	{
		globalcheck = TRUE;
		p = cliConstants;
	}

	// Now check against the contents of the current constants string.
	DO_FOREVER
	{
		bBreakLoop = FALSE;
		
		DO_FOREVER
		{
			switch (*p)
			{
				case 0:
				case ',':
				{
					value = 0;
					name = temp;
					valstr = temp;
					bFound = FALSE;
					
					while(*valstr)
					{
						if (*valstr == '=')
						{
							*valstr = 0;
							valstr++;
							bFound = TRUE;
							break;	
						}
						valstr++;	
					}
					
					if (bFound == FALSE && strlen(name))
					{
						sprintf(tempstring,"CLI Warning: The CONST \"%s\" in the tool \"%s\" requires a value.",name,toolname);
						cliOutputLine(tempstring);	
					}
					else
					{
						if (strcmp(name,constantname) == 0)
						{
							if (is_number(valstr, strlen(valstr)))
							{
								if (is_number(valstr, strlen(valstr)) == 2)	// hex
								{
									value = hex2int(valstr, strlen(valstr));
								}	
								else										// decimal
								{
									value = (uint32) dec2int(valstr, strlen(valstr));		
								}
								
								*returnedvalue = value;
								return NO_ERROR;
							}
							else
							{
								sprintf(tempstring,"CLI Warning: The CONST \"%s\" in the tool \"%s\" is not a number.",name,toolname);
								cliOutputLine(tempstring);
							}
							
						}	
					}
					
					temp[0] = 0;
					
					if (*p == 0)
					{
						bBreakLoop = TRUE;
					}
					else
					{
						p++;
					}
				}
				break;
			
				default:
				{
					char tempstr[2] = {0,0};
					tempstr[0] = *p;
					strcat(temp, tempstr);
					p++;
				}
				break; 	
			}
			if (bBreakLoop == TRUE) break;
		}

		//	Check again, looking in the list of global constants.
		if (globalcheck)
		{
			// If we got here, then we need to break out. 
			break;	
		}
		else
		{
			globalcheck = TRUE;
			p = cliConstants;
		}
	}
	
	hResult = E_CLI_BADCONSTANTS;
	sysLogError(hResult, __LINE__, moduleName);

	return hResult;
}	

/*
	Variable fields:
		This function prints out the bit fields specified in the variable 
		field portion of the CLI variable descriptor.
		
		The variable field is defined as looking like:
		"fieldname=<mask>,fieldname2=<mask>,......."  
		 
*/
void cliPrintVariableFields(uint32 value, char* variablefields) //LM??? not checked or finalized
{
	char		temp[kTempBufferSize] = {0};
	char*		p = 0;
	char		tempoutput[kTempBufferSize] = {0};
	
	memset(temp,0,kTempBufferSize);	

	p=variablefields;	
	
	// Parse fields out and their masks, and print information about the variable fields out.
	
	DO_FOREVER
	{
		switch (*p)
		{
			case 0:
			case ',':
			{
				uint32	mask = 0;
				char*	name = temp;
				char*	valstr = temp;
				BOOL	foundval = FALSE;
				uint32	maskcopy = 0;
				uint32	shiftamount = 0;
				
				while(*valstr)
				{
					if (*valstr == '=')
					{
						*valstr = 0;
						valstr++;
						foundval = TRUE;
						break;	
					}
					valstr++;	
				}
				
				if (foundval == FALSE && strlen(name))
				{
					return;
				}
				else
				{
					if (is_number(valstr, strlen(valstr)))
					{
						if (is_number(valstr, strlen(valstr)) == 2)	// hex
						{
							mask = hex2int(valstr, strlen(valstr));
						}	
						else										// decimal
						{
							mask = (uint32) dec2int(valstr, strlen(valstr));		
						}

						// Figure out the amount we have to shift the value, when using the
						// mask.  To do this, just shift the mask to right until we find
						// that bit 0 is set.
						maskcopy = mask;
						shiftamount = 0;
						while ((maskcopy & 0x00000001) == 0)
						{
							maskcopy >>= 1;
							shiftamount++;	
							if (shiftamount > 31)
							{
								break;	
							}
						}
						
						// Mask of all 0's.  Not useful.
						if (shiftamount > 31)
						{
							return;
						}
						
						// Print the field and value out.
						sprintf(tempoutput," %s=%u", name, (value & mask) >> shiftamount); 
						cliOutputRaw(tempoutput);	

					}
				}
				
				temp[0] = 0;
				
				if (*p == 0)
				{
					return;
				}
				p++;
			}
			break;
		
			default:
			{
				char	xxx[2] = {0,0};
				xxx[0] = *p;
				strcat(temp, xxx);
				p++;
			}
			break; 	
		}
	}
}

/*	
	UTILITY FUNCTIONS
	
	These are functions which are private, and are basically just utilitarian. 
*/

/*
	Simple function for getting the group and command from a commandline.
*/
void cliParseGroupCommand(char* arg, char* groupname, char* commandname)
{
	int32	sep_pos = 0;

	groupname[0] = 0;
	commandname[0] = 0;
	
	// Separation chars allowed ":" and "."
	sep_pos = cliFindChar(arg, ':');
	if (sep_pos == -1)
		sep_pos = cliFindChar(arg, '.');

	if (sep_pos >= 0)
	{
		strcpy(groupname, arg);
		groupname[sep_pos] = 0;
		strcpy(commandname, &(arg[sep_pos+1]));
	}
	else
	{
		strcpy(commandname, arg);	
	}
}

// Returns the index of the character in src. returns -1 if not found.
int32 cliFindChar(char*	src, char findchar)
{
	int32	index = 0;
	BOOL	bFound = FALSE;

	while (*src)
	{
		if (*src == findchar)
		{
			bFound = TRUE;
			break;
		}
		index++;
		src++;
	}
	
	if (bFound == FALSE)
	{
		index = -1;
	}

	return index;
}


/*
	cliCallCommand
	
	This function will actually do a few things:
	
	
	1. If the tool has an Argument String, it will validate the contents of the
	   command line.  
	   
	2. It will call the tool as appropriate.  Commands which wish to handle an
	   optional parameter list, should not include an ArgumentPatternString.

	3. If the command returns an error, the error will be looked up in an error
	   list, and displayed. If it doesn't return an error, than it does nothing.

*/

HRESULT cliCallCommand(CLIDescriptor* tool, uint32 argc, char** argv)
{
	HRESULT		hResult = NO_ERROR;
	char*		argstring = 0;
	char*		toolname = 0;
	uint32		type = 0;
	uint32		value = 0;
	char		tempstring[kTempBufferSize*2];
	void*		arglist[6];
	char*		arg = NULL;
	uint32		len = 0;
	void*   	rawfunc = 0;
	uint32		i = 0;
	uint32		requiredParmLen;
	BOOL		bFound = FALSE;

    for (i = 0; i < 6; i++) arglist[i] = 0;

	kvGetValue(tool,kCLI_Name,(uint32*)&toolname);
	
	// This case is for those tools which use the argument pattern string to do baseline parsing for them.
	hResult = kvIsKey(tool, kCLI_ArgumentPatternString);

	if (hResult == NO_ERROR)
	{
		kvGetValue(tool, kCLI_ArgumentPatternString, (uint32*) &argstring);
		kvGetValue(tool, kCLI_Function, (uint32*) &rawfunc);
		
		// Calculates the minimum number of parameters required
		for (requiredParmLen = 0; requiredParmLen <  strlen(argstring); requiredParmLen++)
		{
			// If uppercase then rest of parameters is optional in argstring list
			if (argstring[requiredParmLen] >= 'A' && argstring[requiredParmLen] <= 'Z')
				break;
		}

		//	check to see that the appropriate number of arguments are passed in.
		if (argc-1 < requiredParmLen)
		{
			cliOutputLine("CLI ERROR: Too few arguments passed.");
			hResult = E_BAD_INPUT_PARAMETERS;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}
		
		if (argc-1 > strlen(argstring))
		{
			cliOutputLine("CLI ERROR: Too many arguments passed.");
			hResult = E_BAD_INPUT_PARAMETERS;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}
		
		// process all the arguments against the argument pattern string.
		for	(i = 0; i < argc-1; i++)
		{
			switch (argstring[i])
			{
				case 'i':
				case 'I':
					type = is_number(argv[i+1],strlen(argv[i+1]));
					
					if (type == 0) // Not a number so look it up as an constant.
					{
						hResult = cliLookUpConstant(tool, argv[i+1], (uint32*)&value);
						if (hResult != NO_ERROR)
						{
							sprintf(tempstring, "argument #%d needs to be a number or a constant.",i+1);
							cliOutputLine(tempstring);
							hResult = E_BAD_INPUT_PARAMETERS;
							sysLogError(hResult, __LINE__, moduleName);
							return hResult;
						}
					}
					
					if (type == 1)
					{
						value = (uint32) dec2int(argv[i+1], strlen(argv[i+1]));
					}
					
					if (type == 2)
					{
						value = hex2int(argv[i+1], strlen(argv[i+1]));
					}
					
					arglist[i] = (void*)value;
					break;
				case 'c': // Char arguments
				case 'C':
					arg = argv[i+1];
					len = strlen(arg);
					if (len != 1)
					{
						sprintf(tempstring, "argument #%d needs to be a char",i+1);
						cliOutputLine(tempstring);
						hResult = E_BAD_INPUT_PARAMETERS;
						sysLogError(hResult, __LINE__, moduleName);
						return hResult;
					}

					{
						char b0;
						sscanf(argv[i+1], "%c", &b0);
						value = (uint32) b0; //(b0-'0');
					}
					arglist[i] = (void*)value;
					break;
				case 's':	// deal with string arguments.
				case 'S':
					arg = argv[i+1];
					len = strlen(arg);
					formatRemoveAllInstancesOf(arg, '"', &len);
					arglist[i] = (void*)arg;
					break;
				case 'k':	// deal with Constants
				case 'K':
					type = is_number(argv[i+1], strlen(argv[i+1]));
					hResult = cliLookUpConstant(tool, argv[i+1], &value);
					if (hResult != NO_ERROR)
					{
						sprintf(tempstring, "argument #%d needs to be a constant.",i+1);
						cliOutputLine(tempstring);
						hResult = E_BAD_INPUT_PARAMETERS;
						sysLogError(hResult, __LINE__, moduleName);
						return hResult;
					}
					arglist[i] = (void*)value;
					break;
				case 'a':	// Hexadecimal address, must be a FULL 8 hexadecimal address.
				case 'A':
					type = is_number(argv[i+1],strlen(argv[i+1]));
					
					if (type != 2 || (strlen(argv[i+1]) < 10))
					{
						sprintf(tempstring, "argument #%d needs to be a full 32bit hexadecimal address.",i+1);
						cliOutputLine(tempstring);
						hResult = E_BAD_INPUT_PARAMETERS;
						sysLogError(hResult, __LINE__, moduleName);
						return hResult;
					}
					value = hex2int(argv[i+1], strlen(argv[i+1]));
					arglist[i] = (void*)value;
					break;
				case 'h':
				case 'H':
					type = is_number(argv[i+1],strlen(argv[i+1]));
					
					if (type != 2)
					{
						sprintf(tempstring, "argument #%d needs to be a hexadecimal value.",i+1);
						cliOutputLine(tempstring);
						hResult = E_BAD_INPUT_PARAMETERS;
						sysLogError(hResult, __LINE__, moduleName);
						return hResult;
					}
					
					value = hex2int(argv[i+1],strlen(argv[i+1]));
					
					arglist[i] = (void*)value;
					break; 
				case 'd':
				case 'D':
					type = is_number(argv[i+1], strlen(argv[i+1]));
					
					if (type != 1)
					{
						sprintf(tempstring, "argument #%d needs to be a decimal value.",i+1);
						cliOutputLine(tempstring);
						hResult = E_BAD_INPUT_PARAMETERS;
						sysLogError(hResult, __LINE__, moduleName);
						return hResult;
					}
					
					value = (uint32) dec2int(argv[i+1], strlen(argv[i+1]));
					arglist[i] = (void*)value;
					break;
				case 'b':
				case 'B':
					
					if (strlen(argv[i+1]) < 8)
					{
						sprintf(tempstring, "argument #%d needs to be a 8 bit binary value.",i+1);
						cliOutputLine(tempstring);
						hResult = E_BAD_INPUT_PARAMETERS;
						sysLogError(hResult, __LINE__, moduleName);
						return hResult;
					}
					{
						char b0, b1, b2, b3, b4, b5, b6, b7;
						sscanf(argv[i+1], "%c%c%c%c%c%c%c%c", &b7, &b6, &b5, &b4, &b3, &b2, &b1, &b0);
						value = (uint32) (((b7-'0')<<7)+((b6-'0')<<6)+((b5-'0')<<5)+((b4-'0')<<4)+((b3-'0')<<3)+((b2-'0')<<2)+((b1-'0')<<1)+(b0-'0'));
						
					}
					
					arglist[i] = (void*)value;
					break; 
				default:
				{
					cliOutputLine("CLI ERROR: ArgumentPatternString, may only contain 's','i','c','k','h' 'd' or 'b' characters.");
					hResult = E_BAD_INPUT_PARAMETERS;
					sysLogError(hResult, __LINE__, moduleName);
					return hResult;
				}	
			}	
		}
		
		// if we got here then the arguments were all valid, 
		// so call the the function with appropriate number of arguments.
		switch (strlen(argstring))
		{
			case 0:
				{
					CLIToolCallback_0ARG	func0 = (CLIToolCallback_0ARG) rawfunc;
					hResult = (func0)();
				}
				break;
			case 1:
				{
					CLIToolCallback_1ARG	func1 = (CLIToolCallback_1ARG) rawfunc;
					hResult = (func1)(arglist[0]);
				}
				break;
			case 2:
				{
					CLIToolCallback_2ARG	func2 = (CLIToolCallback_2ARG) rawfunc;
					hResult = (func2)(arglist[0],arglist[1]);
				}
				break;
			case 3:
				{
					CLIToolCallback_3ARG	func3 = (CLIToolCallback_3ARG) rawfunc;
					hResult = (func3)(arglist[0],arglist[1],arglist[2]);
				}
				break;
			case 4:
				{
					CLIToolCallback_4ARG	func4 = (CLIToolCallback_4ARG) rawfunc;
					hResult = (func4)(arglist[0],arglist[1],arglist[2],arglist[3]);
				}
				break;
			case 5:
				{
					CLIToolCallback_5ARG	func5 = (CLIToolCallback_5ARG) rawfunc;
					hResult = (func5)(arglist[0],arglist[1],arglist[2],arglist[3],arglist[4]);
				}
				break;
			case 6:
				{
					CLIToolCallback_6ARG	func6 = (CLIToolCallback_6ARG) rawfunc;
					hResult = (func6)(arglist[0],arglist[1],arglist[2],arglist[3],arglist[4],arglist[5]);
				}
				break;
		}
		bFound = TRUE;
	}
	else
	{
		CLIToolCallback	func = NULL;
		kvGetValue(tool, kCLI_Function, (uint32*) &func);
		if (func)
		{
			hResult = (func) (argc, argv);
			bFound = TRUE;
		}
	}
	
	if (bFound == TRUE && hResult != NO_ERROR)
	{
#ifdef _SYSDEBUG
		sprintf(tempstring, "CLI ERROR: The tool \"%s\" returned the error 0x%08x - %s", toolname, hResult, errorText(hResult));
#else //_SYSDEBUG
		sprintf(tempstring, "CLI ERROR: The tool \"%s\" returned the error 0x%08x", toolname, hResult);
#endif //_SYSDEBUG
		cliOutputLine(tempstring);
	}
	
	return hResult;
}
#endif //_CLI
