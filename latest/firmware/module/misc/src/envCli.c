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
  * DESCRIPTION: Environment variable Debug Command Line Interface


	Revisions:
		created 04/01/2005 ml

****************************************************************************/

#ifdef _CLI
#ifdef _FIS
#include "cliToolDefs.h"
#include "TCTasking.h"
#include "sysDebug.h"
#include "envCli.h"
#include "env.h"



static HRESULT envInit (const char * name)
{
	if (envInitialize (name) != NO_ERROR)
	{
		cliPrintf("Error loading environment\n\r");
	}
	return NO_ERROR;
}

static HRESULT envGetStr (const char * varname)
{
	char buffer[64];
	HRESULT res;
	
	res = envGetEenvironmentStr (varname, buffer, 63);
	if (res != NO_ERROR)
	{
		cliPrintf("Environment string not found\n\r");
	}
	else
	{
		cliPrintf("%s=%s\n\r",varname,buffer);
	}
	return res;
}


static HRESULT envGetLong (const char * varname)
{
	uint32 val;
	HRESULT res;
	
	res = envGetEenvironmentLong (varname, &val);
	if (res != NO_ERROR)
	{
		cliPrintf("Environment string not found\n\r");
	}
	else
	{
		cliPrintf("%s=%x\n\r",varname,val);
	}
	return res;
}

extern int getNextVar (char ** env, char ** name, int * lname, char ** val, int * lval) ;
extern char* getEnvString(void);

static HRESULT envGetAll (void)
{
	char *	env = getEnvString();
	char bufName[64];
	char bufVal[64];
	char *	name;
	char *	val;
	int     lname;
	int		lval;
	
	if (!env) return 0;
	
	while (getNextVar (&env, &name, &lname, &val, &lval))
	{
		if (lname > 63) break;
		if (lval > 63) break;

		strncpy (bufName, name, lname);
		bufName[lname] = 0;
		strncpy (bufVal, val, lval);
		bufVal[lval] = 0;
		cliPrintf("%s=%s\n\r",bufName, bufVal);
	}

	return NO_ERROR;
}


/***********************************************************************/
static  char 	envInit_UsageString[]=\
				"envcfg:init: \n\r"\
				"use: envcfg:init <envname>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Initialize environment config";
static	CLIDescriptor	envInit_Descriptor[]=
{
	{kCLI_Name,				(uint32)"init"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&envInit},
	{kCLI_UsageString, 		(uint32)envInit_UsageString},
	{kCLI_ArgumentPatternString,	(uint32)"s"},
	{kCLI_ConstantsString,  (uint32)""},
	{kCLI_ModuleName,		(uint32)"Environment Config system"},
	{kCLI_GroupName,		(uint32)"envcfg"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/***********************************************************************/

static  char 	envGetStr_UsageString[]=\
				"envcfg:getstr: \n\r"\
				"use: envcfg:getstr <varname>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Get environment config string";
static	CLIDescriptor	envGetstr_Descriptor[]=
{
	{kCLI_Name,				(uint32)"getstr"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&envGetStr},
	{kCLI_UsageString, 		(uint32)envGetStr_UsageString},
	{kCLI_ArgumentPatternString,	(uint32)"s"},
	{kCLI_ConstantsString,  (uint32)""},
	{kCLI_ModuleName,		(uint32)"Environment Config system"},
	{kCLI_GroupName,		(uint32)"envcfg"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/***********************************************************************/

static  char 	envGetNum_UsageString[]=\
				"envcfg:getlong: \n\r"\
				"use: envcfg:getlong <varname>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Get environment config long integer";
static	CLIDescriptor	envGetnum_Descriptor[]=
{
	{kCLI_Name,				(uint32)"getlong"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&envGetLong},
	{kCLI_UsageString, 		(uint32)envGetNum_UsageString},
	{kCLI_ArgumentPatternString,	(uint32)"s"},
	{kCLI_ConstantsString,  (uint32)""},
	{kCLI_ModuleName,		(uint32)"Environment Config system"},
	{kCLI_GroupName,		(uint32)"envcfg"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

/***********************************************************************/

static  char 	envGetAll_UsageString[]=\
				"envcfg:getall: \n\r"\
				"use: envcfg:getall\n\r"\
				"------------------------------------------------------------\n\r"\
				"Get environment config values";
static	CLIDescriptor	envGetall_Descriptor[]=
{
	{kCLI_Name,				(uint32)"getall"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&envGetAll},
	{kCLI_UsageString, 		(uint32)envGetAll_UsageString},
	{kCLI_ArgumentPatternString,	(uint32)""},
	{kCLI_ConstantsString,  (uint32)""},
	{kCLI_ModuleName,		(uint32)"Environment Config system"},
	{kCLI_GroupName,		(uint32)"envcfg"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


/***********************************************************************/


HRESULT envCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(envInit_Descriptor);
	hResult += cliInstallCLIDescriptor(envGetstr_Descriptor);
	hResult += cliInstallCLIDescriptor(envGetnum_Descriptor);
	hResult += cliInstallCLIDescriptor(envGetall_Descriptor);

	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing sys cli tools");
	}
	return NO_ERROR;
}


#endif //_FIS
#endif //_CLI
