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

		CLI Built In Tools
	
  Revisions:
		created 05/01/2002 lm
****************************************************************************/

static char 		moduleName[] = __FILE__ ;

#ifdef _CLI

#include <stdlib.h>

#include "TCTasking.h"
#include "cli.h"
#include "cliBuiltIn.h"
#include "cliCB.h"
#include "cliSIO.h"
#include "cliSystem.h"
#include "sysCli.h"
#include "listManager.h"
#include "format.h"

static CLI_CALLBACK cliBuiltInToolSplashCB;
static CLI_CALLBACK cliBuiltInToolResetCB;

extern LM_CONTEXT*	cliEnvironmentList;

HRESULT	cliBuiltInToolSetSplashCB(CLI_CALLBACK splashCB)
{
	HRESULT	hResult = NO_ERROR;

	cliBuiltInToolSplashCB = splashCB;

	return hResult;
}

HRESULT	cliBuiltInTool_SPLASH(void)
{
	HRESULT	hResult = NO_ERROR;

	cliOutputLine("");
	cliOutputLine("**********************************************************");
	cliOutputLine("* TC Command Line Interface System                       *");
	cliOutputLine("* Copyright 2003-2011 by TC Applied Technologies Ltd.    *");
	cliOutputLine("**********************************************************");

	if (cliBuiltInToolSplashCB != NULL)
	{
		hResult = (cliBuiltInToolSplashCB)();
	}
	
	return hResult;
}

HRESULT	cliBuiltInTool_ENV(uint32 argc, char** argv)
{
	HRESULT			hResult = NO_ERROR;
	uint32			counter=0;
	CLIDescriptor**	elem=0;
	uint32			index=0;
	char			tempstring[80];

	argc = argc;
	argv = argv;
		
	cliOutputLine("Environment Variables:");
	cliOutputLine("------------------------------------------------------------");
		
	while (lmGetNthElement(cliEnvironmentList, (void **) &elem, counter++, &index) == NO_ERROR)
	{
		char*						varname = 0;
		uint32						value = 0;
		uint32*						pValue = 0;
		uint32						displaytype = 0;
		uint32						displaysize = 32;
		char*						variablefields = NULL;
		char						spacer[40];
		CLIVariableChangedCallback	varcallback = 0;
			
		kvGetValue(*elem, kCLI_Name, (uint32*)&varname);
		kvGetValue(*elem, kCLI_Variable,(uint32*)&pValue);
		kvGetValue(*elem, kCLI_VariableDisplayType, &displaytype); 
		kvGetValue(*elem, kCLI_VariableSize, &displaysize);
		kvGetValue(*elem, kCLI_Function, (uint32*)&varcallback);
		kvGetValue(*elem, kCLI_VariableBitFields, (uint32*)&variablefields);
				
		memset(spacer,' ',ARRAYSIZE(spacer));
		spacer[24-strlen(varname)]=0;
		
		if (varcallback)
		{
			//	For virtual variable, it will have a varcallback, that we can ask for the value of the variable.
			hResult = (varcallback)(kCLI_VariableCallbackOperation_GET,varname,0L,&value);
			if (hResult != NO_ERROR)
			{
				sprintf(tempstring,"%s%s= N/A",varname,spacer);
				cliOutputLine(tempstring);
				hResult = NO_ERROR;
				continue;
			}	
		}
		else
		{
			//	For normal variables which just point to a place in memory.
			if (pValue)
			{	
				value=*pValue;
			}	
		}
		
		if (varname)
		{
			if (kCLI_DISPLAYTYPE_HEX==displaytype)
			{
				switch (displaysize)
				{
					case 32:
						sprintf(tempstring,"%s%s= 0x%08X",varname,spacer,value);
						break;
					
					case 24:
						sprintf(tempstring,"%s%s= 0x%06X",varname,spacer,value);	
						break;
					
					case 16:
						sprintf(tempstring,"%s%s= 0x%04X",varname,spacer,value);	
						break;
					
					case 8:
						sprintf(tempstring,"%s%s= 0x%02X",varname,spacer,value);	
						break;
				}
				cliOutputRaw(tempstring);
			}
			
			if (kCLI_DISPLAYTYPE_DECIMAL==displaytype)
			{
				sprintf(tempstring,"%s%s= %d",varname,spacer,value);
				cliOutputRaw(tempstring);	
			}
			
			if (kCLI_DISPLAYTYPE_BOOLEAN==displaytype)
			{
				sprintf(tempstring,"%s%s= ",varname,spacer);
				if (value)
				{
					strcat(tempstring,"true");	
				}
				else
				{
					strcat(tempstring,"false");
				}	
				cliOutputRaw(tempstring);
			}
			
			if (kCLI_DISPLAYTYPE_BINARY==displaytype)
			{
				uint32	i = 0;
				
				sprintf(tempstring,"%s%s= ",varname,spacer);
				for (i = displaysize; i > 0; i--)
				{
					uint32	mask = (uint32) (1 << (i-1));
					if ((value) & mask)
					{
						strcat(tempstring,"1");	
					}
					else
					{
						strcat(tempstring,"0");
					}
				}
				cliOutputRaw(tempstring);	
			}
			
			// Check to see if we should display fields in the variables.
			if (variablefields)
			{
				cliPrintVariableFields(value,variablefields);
			}
			cliOutputLine(" ");
		}
		
		lmReleaseElement(cliEnvironmentList,index);		
	}

	return hResult;
}

static HRESULT cliBuiltInToolGetNumber(uint32 index, char** argv, CLIDescriptor* variable, uint32* value)
{
	HRESULT		hResult = NO_ERROR;

	if (!is_number(argv[index],strlen(argv[index])))
	{
		if (variable)
		{
			hResult = cliLookUpConstant(variable, argv[index], value);
		}
		else
		{
			hResult = E_CLI_BADARGUMENTS;
		}
	}

	if (hResult != NO_ERROR)
	{
		cliPrintf("SET ERROR: argument #%i must be a number\n\r", index+1);
		return hResult;						
	}

	if (is_number(argv[index],strlen(argv[index]))==2)
	{
		*value = hex2int(argv[index],strlen(argv[index]));
	}
	else
	{
		*value = (uint32) dec2int(argv[index],strlen(argv[index]));
	}

	return hResult;
}

static HRESULT cliBuiltInToolGetAddressHexValue(char* addressarg, uint32 *pAddress, uint32 *pSize)
{
	HRESULT			hResult = NO_ERROR;

	//	Check for the address argument as a hex value.
	//  This handles parsing the address of forms:
	//  0xHHHHHHHH
	//  0xHHHHHHHH:S

	if (pSize)
	{
		*pSize = 32;
	}

	if (addressarg[0] == '0' && addressarg[1] == 'x')
	{
		char* p = addressarg;

		//	special case, this is either a hex value, or a hex value with a size.
		while (*p)
		{
			switch (*p)
			{
				case ':':
					*p = 0;
					p++;
					if (pSize)
					{
						*pSize = (uint32) dec2int(p, strlen(p));
						*pSize = *pSize * 8;
					}
					else
					{
						hResult = E_CLI_INVALIDARGUMENTS;
						sysLogError(hResult, __LINE__, moduleName);
						return hResult;
					}
					break;
				
				default:
					p++;
					break;	
			}	
		}
		
		*pAddress = hex2int(addressarg, strlen(addressarg));
	}
	else
	{
		hResult = E_FAIL;
	}

	return hResult;
}

static HRESULT cliBuiltInToolGetAddress(char* addressarg, uint32 *pAddress, uint32 *pSize, CLIVariableChangedCallback *pVarcallback, uint32 *pDisplaytype)
{
	HRESULT			hResult = NO_ERROR;
	CLIDescriptor*	variable = 0;
	
	// get address and size of the variable.

	if (cliBuiltInToolGetAddressHexValue(addressarg, pAddress, pSize) == NO_ERROR)
	{
		// the address argument is a hex value.
		*pVarcallback = NULL;
		if (pDisplaytype)
		{
			*pDisplaytype = kCLI_DISPLAYTYPE_HEX;
		}
	}
	else	
	{
		// didn't find a direct address, assume that the passed in argument is the name of a variable
		uint32	temp = 0;
		
		variable = cliLookupDescriptorByName("",addressarg);
		
		if (!variable)
		{
			hResult = E_CLI_BADDESCRIPTORNAME;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}
		
		//	Check to make sure that this is variable and not a tool.
		kvGetValue(variable, kCLI_Type, (uint32*)&temp);
		if (temp == kCLI_TYPE_TOOL)
		{
			hResult = E_CLI_BADDESCRIPTORTYPE;
			sysLogError(hResult, __LINE__, moduleName);
			return hResult;
		}
		
		//	Get information about the variable.
		kvGetValue(variable, kCLI_VariableSize, pSize);
		kvGetValue(variable, kCLI_Function, (uint32*) pVarcallback);
		kvGetValue(variable, kCLI_Variable, pAddress);
		if (pDisplaytype)
		{
			kvGetValue(variable, kCLI_VariableDisplayType, (uint32*) pDisplaytype);
		}
	}

	return hResult;
}

static HRESULT cliBuiltInToolGetValue(char* addressarg, uint32 address, uint32 size, CLIVariableChangedCallback varcallback, uint32 *pValue)
{
	HRESULT		hResult = NO_ERROR;

	if (varcallback)
	{
		// if there is a callback associated with the variable, call it
		hResult = (varcallback) (kCLI_VariableCallbackOperation_GET, addressarg, 0L, pValue);
		if (hResult != NO_ERROR) return hResult;
	}
	else
	{
		// Otherwise just get the value directly
		// Assign the pointer to the correctly sized variable pointer type.
		// This way we don't have to worry about handling byte ordering, when changing the data.
		switch (size)
		{
			case 32:	*pValue = *((uint32 *) address);	break;
			case 16:	*pValue = *((uint16 *) address);	break;
			case 8:		*pValue = *((uint8 *) address);		break;
			default:
				hResult = E_NOT_IMPLEMENTED;
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
		}
	}

	return hResult;
}

static HRESULT cliBuiltInToolSetValue(char* addressarg, uint32 address, uint32 size, CLIVariableChangedCallback varcallback, uint32 oldvalue, uint32 newvalue)
{
	HRESULT		hResult = NO_ERROR;

	if (varcallback)
	{
		// if there is a callback associated with the variable, call it
		hResult = (varcallback) (kCLI_VariableCallbackOperation_SET, addressarg, oldvalue, &newvalue);
		if (hResult != NO_ERROR) return hResult; // Returning here, means that the variable's value will not be changed.
	}
	else if (address)
	{
		// Otherwise just set the value directly
		// Assign the pointer to the correctly sized variable pointer type.
		// This way we don't have to worry about handling byte ordering, when changing the data.
		switch (size)
		{
			case 32:	*((uint32 *) address) = newvalue;			break;
			case 16:	*((uint16 *) address) = (uint16) newvalue;	break;
			case 8:		*((uint8 *) address) = (uint8) newvalue;	break;
			default:
				hResult = E_NOT_IMPLEMENTED;
				sysLogError(hResult, __LINE__, moduleName);
				return hResult;
		}
	}
	else
	{
		hResult = E_FAIL;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

HRESULT	cliBuiltInTool_GET(uint32 argc, char** argv)
{
	HRESULT						hResult = NO_ERROR;
	char*						addressarg = 0;
	uint32						size = 0;
	char						tempstring[kTempBufferSize];
	uint32						displaytype = kCLI_DISPLAYTYPE_HEX;
	uint32						oldvalue = 0;
	uint32						newvalue = 0;
	CLIVariableChangedCallback	varcallback = 0;
	CLIDescriptor*	 			variable = 0;
	uint32						address = 0;

	//	Make sure that the minimum number of arguments exists
	if (argc < 2)
	{
		hResult = E_CLI_TOOFEWARGUMENTS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	if (argc > 4)
	{
		hResult = E_CLI_TOOMANYARGUMENTS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	addressarg=argv[1];
	
	// get address and size of the variable.
	hResult = cliBuiltInToolGetAddress(addressarg, &address, &size, &varcallback, &displaytype);
	if (hResult != NO_ERROR) return hResult;

	//	Get the old value
	hResult = cliBuiltInToolGetValue(addressarg, address, size, varcallback, &oldvalue);
	if (hResult != NO_ERROR) return hResult;

	//	figure out which METHOD the user is intending to use.

	//	DIRECT BIT ACCESS METHOD:
	//	This method handles the set tool form of:
	//	get <variable|address> bit X 

	if (argc>=4 && strcmp(argv[2],"bit")==0)
	{
		uint32	bitnumber = 0;
		uint32	mask = 1;
		
		// Figure out which bit to get.
		hResult = cliBuiltInToolGetNumber(3, argv, variable, &bitnumber);
		if (hResult != NO_ERROR) return hResult;

		if (bitnumber > size)
		{
			cliOutputLine("GET ERROR: the bit value was out of range.");
			return hResult;							
		}
		
		//	Check for an alternative display type.
		displaytype = kCLI_DISPLAYTYPE_BOOLEAN;
		
		// create the mask we require.
		mask = mask << (bitnumber-1);
		newvalue = oldvalue & mask;
	}

	//	SIMPLE METHOD
	//	This function handles the simplest method of get:
	//	get <variable|address> [binary|hex|decimal|boolean]

	if (argc >= 2 && argc <= 3)
	{
		//	Check for an alternative display type.
		if (argc == 3)
		{
			if (strcmp("binary",argv[2])==0)
			{
				displaytype=kCLI_DISPLAYTYPE_BINARY;	
			}
			if (strcmp("boolean",argv[2])==0)
			{
				displaytype=kCLI_DISPLAYTYPE_BOOLEAN;	
			}
			if (strcmp("decimal",argv[2])==0)
			{
				displaytype=kCLI_DISPLAYTYPE_DECIMAL;	
			}
			if (strcmp("hex",argv[2])==0)
			{
				displaytype=kCLI_DISPLAYTYPE_HEX;	
			}
		}

		newvalue = oldvalue;
	}
	
	//	Again use the appropriate pointer, which matches the specified size
	//	of the variable.  This gets rid of the problems with having to 
	//	worry about byte ordering.
	if (kCLI_DISPLAYTYPE_HEX==displaytype)
	{
		switch (size)
		{
			case 32:
				sprintf(tempstring,"%s= 0x%08X",addressarg,newvalue);
				break;
			
			case 24:
				sprintf(tempstring,"%s= 0x%06X",addressarg,newvalue);	
				break;
			
			case 16:
				sprintf(tempstring,"%s= 0x%04X",addressarg,newvalue);	
				break;
			
			case 8:
				sprintf(tempstring,"%s= 0x%02X",addressarg,newvalue);	
				break;
		}
	}
	
	if (kCLI_DISPLAYTYPE_DECIMAL==displaytype)
	{
		sprintf(tempstring,"%s= %d",addressarg,newvalue);
	}
	
	if (kCLI_DISPLAYTYPE_BOOLEAN==displaytype)
	{
		sprintf(tempstring,"%s= ",addressarg);
		if (newvalue)
		{
			strcat(tempstring,"true");	
		}
		else
		{
			strcat(tempstring,"false");
		}
	}
	
	if (kCLI_DISPLAYTYPE_BINARY==displaytype)
	{
		uint32	i = 0;
		
		sprintf(tempstring,"%s= ",addressarg);
		for (i = size; i > 0; i--)
		{
			uint32	mask = (uint32) (1 << (i-1));
			if ((newvalue) & mask)
			{
				strcat(tempstring,"1");	
			}
			else
			{
				strcat(tempstring,"0");
			}
		}
	}
		
	cliOutputLine(tempstring);

	return hResult;
}

HRESULT	cliBuiltInTool_SET(uint32 argc, char** argv)
{
	HRESULT						hResult = NO_ERROR;
	char*						addressarg = 0;
	uint32						address = 0;
	uint32						size = 32;
	uint32						oldvalue = 0;
	uint32						newvalue = 0;
	CLIVariableChangedCallback	varcallback = 0;
	CLIDescriptor*	 			variable = 0;
	uint32						index = 0;
	BOOL						bNewValueSet = FALSE;

	//	Make sure that the minimum number of arguments exists

	if (argc < 3)
	{
		hResult = E_CLI_TOOFEWARGUMENTS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	if (argc > 6)
	{
		hResult = E_CLI_TOOMANYARGUMENTS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	index++;	// index = 1
	addressarg = argv[index];
	
	// get address and size of the variable.
	hResult = cliBuiltInToolGetAddress(addressarg, &address, &size, &varcallback, NULL);
	if (hResult != NO_ERROR) return hResult;
	
	// get the old value
	hResult = cliBuiltInToolGetValue(addressarg, address, size, varcallback, &oldvalue);
	if (hResult != NO_ERROR) return hResult;
	
	//	figure out which METHOD the user is intending to use.
	
	//	DIRECT BIT ACCESS METHOD:
	//	This method handles the set tool form of:
	//	set <variable|address> bit X <off|on>

	index++;	// index = 2

	if (index < argc &&
		strcmp(argv[index],"bit")==0)
	{
		uint32	bitnumber=0;
		uint32	mask=1;

		index++;	// index = 3

		// the next two arguments should be <number> <on|off>
		hResult = cliBuiltInToolGetNumber(index, argv, variable, &bitnumber);
		if (hResult != NO_ERROR) return hResult;
		
		if (bitnumber > size)
		{
			cliOutputLine("SET ERROR: the bit value was out of range.");
			return hResult;							
		}
		
		// create the mask we require.
		mask = mask << (bitnumber-1);
		mask = ~mask;
		
		newvalue = oldvalue & mask;
		bNewValueSet = TRUE;
		
		// If the last argument is valid then turn on the bit.
		
		index++;	// index = 4

		if (strcmp(argv[index],"on")!=0 && 
			strcmp(argv[index],"off") !=0)
		{
			cliOutputLine("SET ERROR: Argument #5 must be either 'on'|'off'.");
			return hResult;
		}
		
		if (strcmp(argv[index],"on")==0)
		{
			mask = 1;
			mask = mask << (bitnumber-1);
			newvalue = newvalue | mask;	
		}
	}

	//	AND'd method
	//  This handles the set of form:
	//  set <variable|address> and N

	//	index = 2
	if (index < argc &&
		strcmp(argv[index],"and")==0)
	{
		uint32	value = 0;
		
		index++;	// index = 3

		hResult = cliBuiltInToolGetNumber(index, argv, variable, &value);
		if (hResult != NO_ERROR) return hResult;

		newvalue = oldvalue & value;
		bNewValueSet = TRUE;
				
		index++;	// index = 4
	}
	
	//	OR'd method
	//  This handles the set of form:
	//  set <variable|address> or N

	//	index = 2 or 4
	if (index < argc &&
		strcmp(argv[index],"or")==0)
	{
		uint32	value = 0;
		
		index++;	// index = 3 or 5

		hResult = cliBuiltInToolGetNumber(index, argv, variable, &value);
		if (hResult != NO_ERROR) return hResult;

		if (bNewValueSet == TRUE)
		{
			oldvalue = newvalue;
		}
		newvalue = oldvalue | value;
		bNewValueSet = TRUE;
				
		index++;	// index = 4 or 6
	}
	
	//	SIMPLE METHOD
	//	This function handles the simplest method of set:
	//	set <variable|address> <newvalue>

	if (index < argc &&
		index == 2)
	{
		uint32	value = 0;
		
		hResult = cliBuiltInToolGetNumber(index, argv, variable, &value);
		if (hResult != NO_ERROR) return hResult;

		newvalue = value;
		bNewValueSet = TRUE;
	}
	
	hResult = cliBuiltInToolSetValue(addressarg, address, size, varcallback, oldvalue, newvalue);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT	cliBuiltInTool_SETM(uint32 argc, char** argv)
{
	HRESULT			hResult = NO_ERROR;
	uint32			data[64];
	uint32			index;

	//	Make sure that the minimum number of arguments exists

	if ((argc - 1) % 2 != 0)
	{
		hResult = E_CLI_INVALIDARGUMENTS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if ((argc - 1) > 64)
	{
		hResult = E_CLI_TOOMANYARGUMENTS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	for (index = 0; index < argc - 1; index+=2)
	{
		hResult = cliBuiltInToolGetAddressHexValue(argv[1+index], &data[index], NULL);
		if (hResult != NO_ERROR) return hResult;
		hResult = cliBuiltInToolGetNumber(1+index+1, argv, 0, &data[index+1]);
		if (hResult != NO_ERROR) return hResult;
	}

	for (index = 0; index < argc - 1; index+=2)
	{
		*((uint32 *) data[index]) = data[index+1];
	}

	return hResult;
}

HRESULT	cliBuiltInTool_GMS(uint32 argc, char** argv) // Get-Modify-Set
{
	HRESULT			hResult = NO_ERROR;
	char			tempStr[kTempBufferSize];
	char*			arglist[kMaxArgElements];
	uint32			argcount = 0;
	char*			addressarg = 0;
	CLIDescriptor*	variable = 0;
	uint32			clearmask = 0;
	uint32			setmask = 0;

	// Make sure that the correct number of arguments exists
	// convert to "set <addr> and <and-mask> or <or-mask>

	if (argc < 4)
	{
		hResult = E_CLI_TOOFEWARGUMENTS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if (argc > 4)
	{
		hResult = E_CLI_TOOMANYARGUMENTS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	addressarg = argv[1];

	hResult = cliBuiltInToolGetNumber(2, argv, variable, &clearmask);
	if (hResult != NO_ERROR) return hResult;
	clearmask = ~clearmask;

	hResult = cliBuiltInToolGetNumber(3, argv, variable, &setmask);
	if (hResult != NO_ERROR) return hResult;

	sprintf(tempStr, "set %s and 0x%08x or 0x%08x", addressarg, clearmask, setmask);

	hResult = cliStrToArgs(tempStr,arglist,&argcount);
	if (hResult != NO_ERROR) return hResult;

	hResult = cliBuiltInTool_SET(argcount,arglist);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT	cliBuiltInTool_DUMP(void* address, uint32 unitsize, uint32 numberofitems, uint32 itemsperline)
{
	HRESULT		hResult = NO_ERROR;
	uint32		addr = (uint32) address;
	uint32		numberofbytes = numberofitems * unitsize;
	uint32		bytesperline = itemsperline * unitsize;
	uint32		counter = 0;
	char		tempdata[kTempBufferSize];
	char		fmtdata[20];
			
	if (unitsize != 1 &&
		unitsize != 2 &&
		unitsize != 4)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (itemsperline == 0) bytesperline = 16;

	while (numberofbytes)
	{
		address = (void *) addr;
		counter = 0;
		tempdata[0] = 0;

		if (numberofbytes >= bytesperline)
		{
			counter = bytesperline;	
			numberofbytes -= bytesperline;
		}
		else
		{
			counter = numberofbytes; 
			numberofbytes = 0;	
		}

		while (counter)
		{
			switch (unitsize)
			{
				case 1:	sprintf(fmtdata,"%02X ", (uint8) (*((uint8 *) addr))); break;
				case 2:	sprintf(fmtdata,"%04X ", (uint16) (*((uint16 *) addr))); break;
				case 4: sprintf(fmtdata,"%08X ", (uint32) (*((uint32 *) addr))); break;
			}
			strcat(tempdata, fmtdata);
			counter -= unitsize;
			addr += unitsize; 
		}
		cliPrintf("0x%08X: %s\n\r", address, tempdata);
	}

	return hResult;
}

HRESULT	cliBuiltInTool_EXIT(void)
{
	exit(0);

#ifdef _WINMP
#else //_WINMP
	return E_FAIL;
#endif //_WINMP
}

HRESULT	cliBuiltInToolSetResetCB(CLI_CALLBACK resetCB)
{
	HRESULT	hResult = NO_ERROR;

	cliBuiltInToolResetCB = resetCB;

	return hResult;
}

HRESULT	cliBuiltInTool_RESET(void)
{
	HRESULT	hResult = NO_ERROR;

	if (cliBuiltInToolResetCB != NULL)
	{
		hResult = (cliBuiltInToolResetCB)();
	}

	hResult = E_FAIL;

	return hResult;
}

HRESULT	cliBuiltInTool_ECHO(BOOL bEchoOn)
{
	HRESULT	hResult = NO_ERROR;

	cliSIOEchoOn(bEchoOn);

	return hResult;
}

HRESULT	cliBuiltInTool_PROMPT(BOOL bPromptOn)
{
	HRESULT	hResult = NO_ERROR;

	cliSystemPromptOn(bPromptOn);

	return hResult;
}

// BUILT IN TOOLS		
// These are the tools which are always available in the CLI

// ENV TOOL
static  char 	ENV_UsageString[]=\
				"env: (Built-In)\n\r"\
				"use: env [<variablename>]\n\r"\
				"------------------------------------------------------------\n\r"\
				"This is a very simple command, which displays \n\r"\
				"all the environment variables, and their values"; 
static	CLIDescriptor	ENV_Descriptor[]=
{
	{kCLI_Name,				(uint32)"env"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cliBuiltInTool_ENV},
	{kCLI_UsageString, 		(uint32)ENV_UsageString},
	{kCLI_ModuleName,		(uint32)"built-in"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// GET TOOL
static  char	GET_UsageString[]=\
				"get: (Built-In)\n\r"\
				"use: \n\r"\
				"Simple Method:\n\r"\
				"    get <variablename|address[:<sizeinbytes>]> [hex|binary|decimal|boolean]\n\r"\
				" example:\n\r"\
				"    get FPGARegister1 hex\n\r"\
				"    get FPGARegister1 binary\n\r"\
  				"Direct Bit Access:\n\r"\
			    "    get <variablename|address[:<sizeinbytes>]> bit <integer>\n\r"\
			    " example:\n\r"\
			    "    get FPGARegister1 bit 2\n\r";
static	CLIDescriptor	GET_Descriptor[]=
{
	{kCLI_Name,				(uint32)"get"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cliBuiltInTool_GET},
	{kCLI_UsageString, 		(uint32)GET_UsageString},
	{kCLI_ModuleName,		(uint32)"built-in"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// SET TOOL
static  char	SET_UsageString[]=\
				"set: (Built-In)\n\r"\
				"use: \n\r"\
				"Simple Method:\n\r"\
				"    set <variablename|address[:<sizeinbytes>]> <integer>\n\r"\
				" example:\n\r"\
				"    set FPGARegister1 0x81\n\r"\
				"    set 0xFF00FF00    0x81 (assumed as 32 bits.)\n\r"\
				"    set 0xFF00FF00:4\n\r"\
				"OR|AND'd Methods:\n\r"\
			    "    set <variablename|address[:<sizeinbytes>]> <or|and> <integer>\n\r"\
				" can be combined into one statement (always 'and' before 'or'):\n\r"\
			    "    set <variablename|address[:<sizeinbytes>]> and <i1> or <i2>\n\r"\
			    " This method does a common OR|AND operation. Using this command\n\r"\
			    " same as doing in C: '|=' '&='\n\r"\
				" example:\n\r"\
				"    set FPGARegister1 or 0x80\n\r"\
				"    set 0xFF00FF00:4 and 0x0000FF8F\n\r"\
				"    set 0xFF00FF00 and 0xFFFF0000 or 0x0000FF8F\n\r"\
  				"Direct Bit Access:\n\r"\
			    "    set <variablename|address[:<sizeinbytes>]> bit <integer> <on|off>\n\r"\
			    " example:\n\r"\
			    "    set FPGARegister1 bit 2 on\n\r";
static	CLIDescriptor	SET_Descriptor[]=
{
	{kCLI_Name,				(uint32)"set"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cliBuiltInTool_SET},
	{kCLI_UsageString, 		(uint32)SET_UsageString},
	{kCLI_ModuleName,		(uint32)"built-in"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// SETM TOOL
static  char	SETM_UsageString[]=\
				"setm: (Built-In)\n\r"\
				"use: \n\r"\
				"    setm <addr1> <data1> <addr2> <data2> ... <addrN> <dataN>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set multiple address-value pairs in one CLI operation";
static	CLIDescriptor	SETM_Descriptor[]=
{
	{kCLI_Name,				(uint32)"setm"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cliBuiltInTool_SETM},
	{kCLI_UsageString, 		(uint32)SETM_UsageString},
	{kCLI_ModuleName,		(uint32)"built-in"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// GMS TOOL
static  char	GMS_UsageString[]=\
				"gms: (Built-In)\n\r"\
				"use: \n\r"\
				"    gms <variablename|address[:<sizeinbytes>]> <clearmask> <setmask>\n\r"\
				" identical to:\n\r"\
			    "    set <variablename|address[:<sizeinbytes>]> and <and-mask> or <or-mask>\n\r"\
				" where and-mask: bitwise-inverse of clearmask\n\r"\
				"    and or-mask: setmask\n\r"\
			    " modifies value speficified by variablename|address\n\r"\
			    "    each bit set in clearmask clears bit in value\n\r"\
			    "    each bit set in setmask sets bit in value\n\r"\
				"------------------------------------------------------------\n\r"\
				"Get-Modify-Set tool to modify a value";
static	CLIDescriptor	GMS_Descriptor[]=
{
	{kCLI_Name,				(uint32)"gms"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cliBuiltInTool_GMS},
	{kCLI_UsageString, 		(uint32)GMS_UsageString},
	{kCLI_ModuleName,		(uint32)"built-in"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// Splash Screen Tool
static  char	SPLASH_UsageString[]=\
				"splash: (Built-In)\n\r"\
				"use: splash\n\r"\
				"------------------------------------------------------------\n\r"\
				"display startup screen";
static	CLIDescriptor	SPLASH_Descriptor[]=
{
	{kCLI_Name,				(uint32)"splash"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cliBuiltInTool_SPLASH},
	{kCLI_UsageString, 		(uint32)SPLASH_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"built-in"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// Dump Tool
static  char	DUMP_UsageString[]=\
				"dump: (Built-In)\n\r"\
				"use: dump <address> <unitsize> <unitcount> <items per line> \n\r"\
				"<unitsize>: 1,2,4 \n\r"\
				"------------------------------------------------------------\n\r"\
				"display raw memory in a formatted fashion";
static	CLIDescriptor	DUMP_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dump"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cliBuiltInTool_DUMP},
	{kCLI_UsageString, 		(uint32)DUMP_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"akii"},
	{kCLI_ModuleName,		(uint32)"built-in"},
	{kCLI_ConstantsString,	(uint32)"1=1,2=2,4=4"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char 	EXIT_UsageString[]=\
				"exit: \n\r"\
				"use: exit\n\r"\
				"------------------------------------------------------------\n\r"\
				"exit the application";
static	CLIDescriptor	EXIT_Descriptor[]=
{
	{kCLI_Name,				(uint32)"exit"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cliBuiltInTool_EXIT},
	{kCLI_UsageString, 		(uint32)EXIT_UsageString},
	{kCLI_ArgumentPatternString,	(uint32)""},
	{kCLI_ConstantsString,  (uint32)""},
	{kCLI_ModuleName,		(uint32)"built-in"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	RESET_UsageString[]=\
				"reset: (Built-In)\n\r"\
				"use: reset\n\r"\
				"------------------------------------------------------------\n\r"\
				"resets device if implemented";
static	CLIDescriptor	RESET_Descriptor[]=
{
	{kCLI_Name,				(uint32)"reset"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cliBuiltInTool_RESET},
	{kCLI_UsageString, 		(uint32)RESET_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"built-in"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	ECHO_UsageString[]=\
				"echo: (Built-In)\n\r"\
				"use: echo <on|off>\n\r"\
				"------------------------------------------------------------\n\r"\
				"turns on/off the echoing of characters";
static	CLIDescriptor	ECHO_Descriptor[]=
{
	{kCLI_Name,				(uint32)"echo"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cliBuiltInTool_ECHO},
	{kCLI_UsageString, 		(uint32)ECHO_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"built-in"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	PROMPT_UsageString[]=\
				"prompt: (Built-In)\n\r"\
				"use: prompt <on|off>\n\r"\
				"------------------------------------------------------------\n\r"\
				"turns on/off the printing of the prompt character";
static	CLIDescriptor	PROMPT_Descriptor[]=
{
	{kCLI_Name,				(uint32)"prompt"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&cliBuiltInTool_PROMPT},
	{kCLI_UsageString, 		(uint32)PROMPT_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"built-in"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


void cliBuiltInInstallTools(void)
{
	HRESULT			hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(ENV_Descriptor);
	hResult += cliInstallCLIDescriptor(GET_Descriptor);
	hResult += cliInstallCLIDescriptor(SET_Descriptor);
	hResult += cliInstallCLIDescriptor(SETM_Descriptor);
	hResult += cliInstallCLIDescriptor(GMS_Descriptor);
	hResult += cliInstallCLIDescriptor(DUMP_Descriptor);
	hResult += cliInstallCLIDescriptor(SPLASH_Descriptor);
	hResult += cliInstallCLIDescriptor(EXIT_Descriptor);
	hResult += cliInstallCLIDescriptor(RESET_Descriptor);
	hResult += cliInstallCLIDescriptor(ECHO_Descriptor);
	hResult += cliInstallCLIDescriptor(PROMPT_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing built-in cli tools");
	}
}

#endif //_CLI
