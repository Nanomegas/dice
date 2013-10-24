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
* DESCRIPTION:	This is a set of functions to access environment settings
				defined in a flash config structure.
				Please do not confuse this with the CLI environment variables!
				The purpose of this module is to provide configurations for the
				firmware. An example would be the WWUID.
				
				The variables are stored as one long string in a flash image.
				The format of the string is: 
				<varname>=<varvalue>
				<varname>=<varvalue>
				.
				.

	Revisions:
		created 04/01/2005 ml
*****************************************************************************/
#ifdef _FIS
#include "stdlib.h"
#include "env.h"
#include "fis.h"
#include "format.h"
#include "string.h"

#ifdef _CLI
#include "cli.h"
#endif

static FIS_IMAGE envImage; //the image containing the environment
static char *   envString = 0; //0 indicates that no image is found
static char *	envEnd    = 0; 

char* getEnvString(void)
{
	return envString;
}

HRESULT envInitialize (const char * envname)
{
    long	index = FIS_INIT_GET_IMAGE;
    HRESULT	res;
	//look for the image with the correct name
    res = fis_get_next_image(&envImage, &index);    
    while (res == NO_ERROR)
    {
		if (strncmp (envname,envImage.name,16) == 0)
		{
			envString = (char *)envImage.flash_base;
			envEnd = envString + envImage.data_length;
			//cliPrintf("Length %i\n\r",envImage.data_length);
			return NO_ERROR;		
		}
		res = fis_get_next_image(&envImage, &index);
	}
	return res;
}

int getNextVar (char ** env, char ** name, int * lname, char ** val, int * lval) 
{
	char * str = *env;
	*lname = 0;
	*name = str;
	while (str < envEnd)
	{
		if (*str == '=') break;
		str++;
		(*lname)++;
	}
	if (str >= envEnd) return 0; //we have got to the end
	str++;	
	*val = str;
    *lval = 0;
    while (str < envEnd)
	{
		if ((*str == 13) || (*str == 10)) break;
		str++;
		(*lval)++;
	}	
	if ((str >= envEnd) && (!*lval)) return 0; //we have got to the end
	str++; //skip the seperator
	if ((*str == 13) || (*str == 10)) str++;
	*env = str;
	return 1;
}
	
	

static int findEnvString (const char * varname, char ** value, int * vlen)
{
	char *	env = envString;
	char *	name;
	int     lname;
	int		lsrc = strlen (varname);
	
	if (!env) return 0;
	
	while (getNextVar (&env, &name, &lname, value, vlen))
	{
		
		//cliPrintf("Found <%.*s>,<%.*s>\n\r",lname,name,*vlen,*value);
		//check if name match
		if ((lsrc == lname)&& (strncmp (varname,name,lsrc) == 0)) return 1;
	}
	return 0;
}
		

HRESULT envGetEenvironmentLong (const char * varname, uint32 * pvalue)
{
	char * val;
	int vlen;
	uint8 type;
	
	if (findEnvString (varname, &val, &vlen))
	{
		type = is_number (val, vlen);
		if (type == 1)
		{
			*pvalue = (uint32) dec2int(val, vlen);
			return NO_ERROR;
		}
		else	if (type == 2)
		{
			*pvalue = (uint32) hex2int(val, vlen);
			return NO_ERROR;
		}
		else
		{
			return E_GEN_ILLEGAL_PARAM;
		}
	}
	return E_GEN_NOMATCH;
}

HRESULT envGetEenvironmentLongDefault (const char * varname, uint32 * pvalue, uint32 defval)
{
	HRESULT res = envGetEenvironmentLong (varname, pvalue);
	
	if (res != NO_ERROR) *pvalue = defval;
	
	return res;
}
	
	
HRESULT envGetEenvironmentStr (const char * varname, char * value, int maxlen)
{
	char * val;
	int vlen;
	
	if (!findEnvString (varname, &val, &vlen)) return E_GEN_NOMATCH;
	
	if (vlen > maxlen) return E_GEN_ILLEGAL_PARAM;
	
	strncpy (value, val,vlen);
	value[vlen] = 0;
	return NO_ERROR;
}

#endif //_FIS
