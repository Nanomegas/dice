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
****************************************************************************/

#ifndef _ENV_H
#define _ENV_H

#include "TCTypes.h"

// Initialize the environment system, envname should be the name of a flash image
HRESULT envInitialize	(const char * envname);

// Get an environment string value and convert it to an integer value, works with hex and decimal
HRESULT envGetEenvironmentLong	(const char * varname, uint32 * pvalue);

// Same as envGetEenvironmentLong but will return defval if not found
HRESULT envGetEenvironmentLongDefault	(const char * varname, uint32 * pvalue, uint32 defval);

// Get an environment string value
HRESULT envGetEenvironmentStr	(const char * varname, char * value, int maxlen);



#endif //_ENV_H
