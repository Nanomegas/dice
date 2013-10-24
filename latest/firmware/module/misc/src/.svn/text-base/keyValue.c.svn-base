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
 * DESCRIPTION: KeyValue Maps

	Purpose: 

		This file defines functions for creating/accessing and deleting
		keyvalue maps.  Keyvalue maps are extremely useful for allowing
		access to values, which are going to be shared.

  Revisions:
		created 05/15/2002 lm
****************************************************************************/

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "keyvalue.h"

/*
	kvCreate
*/
HRESULT	kvCreate (KEYVALUE* list, uint32 size)
{
	HRESULT		hResult = NO_ERROR;
	uint32		counter=0;
	
	if (list == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (size == 0)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	for (counter=0; counter < size - 1; counter++)
	{
		(list[counter]).key = KV_KEYSPACE;
		(list[counter]).value = 0;
	} 
	
	(list[counter]).key = KV_TERMINATEKEY;
	(list[counter]).value = KV_TERMINATEVALUE;
	
	return hResult;
}

/*

	kvSetValue
	
	This function sets value, which matches the key specified in the function.

*/
HRESULT	kvSetValue(KEYVALUE* list, uint32 key, uint32 value)
{
	HRESULT		hResult = NO_ERROR;
	uint32		curkey, curvalue;
	KEYVALUE*	plist = list;
	BOOL		bFound = FALSE;
	
	if (list == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	curkey = list->key;
	curvalue = list->value;
	
	while (curkey != KV_TERMINATEKEY && curvalue != KV_TERMINATEVALUE)
	{
		if (curkey == key)
		{
			plist->value = value;
			bFound = TRUE;
			break;
		}
	
		plist++;
		curkey = plist->key;
		curvalue = plist->value;
	}
	
	if (bFound == FALSE)
	{
		plist = list;
		curkey = plist->key;
		while (curkey != KV_TERMINATEKEY)
		{
			if (curkey == KV_KEYSPACE)
			{
				plist->key = key;
				plist->value = value;
				bFound = TRUE;
				break;
			}
			plist++;
			curkey = plist->key;
		} 
	}
	
	if (bFound == FALSE)
	{
		hResult = E_KV_BADKEYVALUE;
		//sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

/*	
	kvIsKey

	This function checks to see if the specified key exists in the specified list.
	
	A return value of NO_ERROR means the key was found. 
*/
HRESULT	kvIsKey(KEYVALUE* list, uint32 key)
{
	HRESULT		hResult = NO_ERROR;
	uint32		curkey, curvalue;
	BOOL		bFound = FALSE;
	
	if (list == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	curkey = list->key;
	curvalue = list->value;
	
	while (curkey != KV_TERMINATEKEY &&
		   curvalue != KV_TERMINATEVALUE)
	{
		if (curkey == key)
		{
			bFound = TRUE;
			break;
		}
	
		list++;
		curkey = list->key;
		curvalue = list->value;
	}
	
	if (bFound == FALSE)
	{
		hResult = E_KV_BADKEYVALUE;
//		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

/*
	kvGetValue
	
	This function returns the value associated with the specified key.
	
	If this functions returns NO_ERROR then the value was returned in the "value" 
	parameter.
*/

HRESULT	kvGetValue(KEYVALUE* list, uint32 key, uint32* value)
{
	HRESULT		hResult = NO_ERROR;
	uint32		curkey, curvalue;
	BOOL		bFound = FALSE;
	
	if (list == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	curkey = list->key;
	curvalue = list->value;
	
	while (curkey != KV_TERMINATEKEY && curvalue != KV_TERMINATEVALUE)
	{
		if (curkey == key)
		{
			if (value)
			{
				*value = curvalue;	
			}
			bFound = TRUE;
			break;
		}
	
		list++;
		curkey = list->key;
		curvalue = list->value;
	}
	
	if (bFound == FALSE)
	{
		hResult = E_KV_BADKEYVALUE;
		//sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}

HRESULT	kvIsValue(KEYVALUE* list, uint32 key, uint32 valuecompare)
{
	HRESULT		hResult = NO_ERROR;
	uint32		curkey, curvalue;
	BOOL		bFound = FALSE;
	
	if (list == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	curkey = list->key;
	curvalue = list->value;
	
	while (curkey != KV_TERMINATEKEY && curvalue != KV_TERMINATEVALUE)
	{
		if (curkey == key)
		{
			if (valuecompare == curvalue)
			{
				bFound = TRUE;
				break;
			}
		}
	
		list++;
		curkey = list->key;
		curvalue = list->value;
	}
	
	if (bFound == FALSE)
	{
		hResult = E_KV_BADKEYVALUE;
		//sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;	
}

/*
	kvValueTrue
	
	This function returns true if the associated value is set to other than 0.
	It returns false otherwise. It also returns false on non-existant keys.
*/

BOOL kvValueTrue(KEYVALUE* list, uint32 key)
{
	HRESULT		hResult = NO_ERROR;
	uint32		value = 0;
	
	hResult = kvGetValue(list, key, &value);
	if (hResult != NO_ERROR) return FALSE;

	return (value > 0);
}	

/*	
	CountKeys
	
	Count the number of used keys in the keyvalue map.
*/
uint32 kvCountKeys(KEYVALUE* list)
{
	HRESULT		hResult = NO_ERROR;
	uint32		curkey, curvalue;
	uint32		counter = 0;
		
	if (list == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return 0;
	}

	DO_FOREVER
	{
		curkey = list->key;
		curvalue = list->value;
		
		if (curkey == KV_TERMINATEKEY || curvalue == KV_TERMINATEVALUE) break;

		if (curkey != KV_KEYSPACE)
		{
			counter++;
		}

		list++;
	}

	return counter;
}

/*
	kvFreeKey
	
	This function turns a KEY into free key.
*/

void kvFreeKey(KEYVALUE* list, uint32 key)
{
	HRESULT		hResult = NO_ERROR;
	uint32		curkey, curvalue;
	
	if (list == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return;
	}

	hResult = kvIsKey(list, key);
	if (hResult != NO_ERROR) return;

	DO_FOREVER
	{
		curkey = list->key;
		curvalue = list->value;
		
		if (curkey == KV_TERMINATEKEY || curvalue == KV_TERMINATEVALUE) break;

		if (curkey == key)
		{
			list->key = KV_KEYSPACE;
			break;
		}

		list++;
	}
}	
