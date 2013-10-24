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

#ifndef _KEYVALUE_H
#define _KEYVALUE_H

#include "TCTypes.h"

/** @addtogroup KeyValue

	This utility provides Key-Value maps.

	Usually the "KEY" is a numeric identifier, independant from the
	position of the "KEY's" position in a list, which is in reference to
	a VALUE.
	
	This is way of making some object oriented type operations, without
	using C++ or some other object oriented language.

	<h2>
	Creating a Key-Value list
	</h2>
	
	Basically this is much like a RangeMap, you are not creating a structure,
	but instead an array of KEYVALUE pairs. 

	Thus in your "C" code your KeyValue pairs should look like this:
	
	@code
	static KEYVALUE   errormap[]=
	{
		{NO_ERROR						,   CLASS1394_SUCCESS              		},
		{E_LAL_GENERIC_FAILURE			,   CLASS1394_GENERIC_FAILURE      		},
		{E_LAL_INVALID_REQUEST			,   CLASS1394_INVALID_REQUEST           },
		{E_LAL_WWWUID_INVALID			,   CLASS1394_WWUID_INVALID        		},
		{E_LAL_WWWUID_NOTFOUND			,   CLASS1394_WWUID_NOTFOUND       		},
		{E_LAL_HANDLE_INVALID			,   CLASS1394_HANDLE_INVALID       		},
		{E_LAL_HANDLE_NOTOPEN			,   CLASS1394_HANDLE_NOTOPEN       		},
		{E_LAL_SPEEDMAP_ERROR			,   CLASS1394_SPEEDMAP_ERROR       		},
		
		{KV_TERMINATEKEY, KV_TERMINATEVALUE}
	};
	@endcode
	
	<strong>
	Notice at the end there is a "KV_TERMINATEKEY" and a "KV_TERMINATEVALUE" pair.
	</strong>
	
	This pair is extremely special, and MANDATORY, this lets the parsing functions
	know where, the end of the list is.    Failure to use these in your list, will
	cause many silly problems.

	@{
*/

/* MACROS */
#define		KV_LISTSIZE(x)		x+1

/* CONSTS */
//
//	Placing these at the end of your key:value list is MANDATORY!
//
#define		KV_TERMINATEKEY		0xFFFFFFFF
#define 	KV_TERMINATEVALUE	0xFFFFFFFF
#define		KV_KEYSPACE			0xFFFFFFFE		//Used to reserve space for new keys.

/* STRUCTS */

typedef struct
{
	uint32	key;
	uint32	value;
} KEYVALUE;


/**	This function takes a buffer of KEYVALUE types and initializes them
	so that the keys are set to KV_KEYSPACE, and then includes a terminator.
	
	Make sure the buffer you give to kvCreate is 1 entry larger than the number
	of keys you intend to have present, thus leaving room for the terminator.
	You can use the KV_LISTSIZE() macro to create a buffer of the appropriate size.
	
	Use this when you need to create a list structure out of an array.  You don't 
	need to call this with statically created lists.
	@param list		Address that will be update to point to the newly created list.
	@param size		The size of the list
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		kvCreate (KEYVALUE* list, uint32 size);


/**	Sets the value of the specified key
	@param list		The list to use.
	@param key		The key to use.
	@param value	The value ot update.
	@return			NO_ERROR (0) or an @ref ErrorCodes "error code".
*/
HRESULT		kvSetValue(KEYVALUE* list, uint32 key, uint32 value);

/** Check to see if the specified key exists in the specified list.
	@param list		The list to use.
	@param key		The key to check for.
	@return			NO_ERROR (0) indicates that the key was found.
*/
HRESULT		kvIsKey(KEYVALUE* list, uint32 key);


/** Obtain the value associated with the specified key.
	@param list		The list to use.
	@param key		The key to use.
	@param value	Address to place the value in.
	@return			If this functions returns NO_ERROR then the value was returned in the "value" parameter.	
*/
HRESULT		kvGetValue(KEYVALUE* list, uint32 key, uint32* value);

/** Check the value associated with a key.
	@param list			The list to use.
	@param key			The key to use.
	@param valuecompare	The value to check for.
	@return				NO_ERROR if the value passed in is the same as in the value of the specified key.
*/
HRESULT		kvIsValue(KEYVALUE* list, uint32 key, uint32 valuecompare);

/** Check to see if the value of a key is not zero and that the key does exist.
	@param list			The list to use.
	@param key			The key to use.
	@return				TRUE if the associated value is set to other than 0. It returns false otherwise. It also returns false on non-existant keys.
*/
BOOL	kvValueTrue(KEYVALUE* list, uint32 key);

/** Obtain the number of keys in a list
	@param list			The list to use.
	@return				The number of keys.
*/
uint32	kvCountKeys(KEYVALUE* list);

/** Sets the value of the key to KV_FREESPACE, thus allowing for
	the space occupied by the key to be recycled.
	@param list			The list to use.
	@param key			The key to use.
*/
void		kvFreeKey(KEYVALUE* list, uint32 key);

//! @}
#endif
