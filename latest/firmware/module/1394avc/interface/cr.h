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

		Implementation of a global callback registry.
		
		This allows for a single point of failure for allocating, retrieving 
		of callbacks.  Also it gives us another method for looking into a 
		AVC device and determining the state of the device from a command line.


  Revisions:
		created 21/03/2003 KBJ/LM

****************************************************************************/
#ifndef _CR_H
#define _CR_H

#include "TCTypes.h"
#include "keyValue.h"

/** @addtogroup CallbackRegistry
	This modules provides a global callback registry.

	This module can be used from the command prompt by the @ref cli_ds commands.

	This allows for a single point of failure for allocating, retrieving 
	of callbacks.  Also it gives us another method for looking into a 
	AVC device and determining the state of the device from a command line.
	@{
*/

// defines
// Keys that must/should be in every callback descriptor

#define	 CALLBACK_DESCRIPTOR_TYPE		0x0FFFFFFF		// REQUIRED
#define	 CALLBACK_DESCRIPTOR_NEXT		0x0FFFFFFE		// REQUIRED
#define  CALLBACK_DESCRIPTOR_NAME		0x0FFFFFFD		// OPTIONAL

typedef	KEYVALUE CALLBACK_DESCRIPTOR;

enum
{
	CR_FIND_CALLBACK_TYPE = 0,
	CR_FIND_CALLBACK_FIRST,
	CR_FIND_CALLBACK_THIS,
	CR_FIND_CALLBACK_PREV,
	CR_FIND_CALLBACK_NEXT
};

HRESULT		crInitialize(void);

/** Add a callback to the callback registry
	AV/C subunits should use this function to register with the AV/C handler.  See the /ref AVC documentation
	for details.
	@param callback	The callback descriptor to install.
*/

HRESULT		crAddCallback(CALLBACK_DESCRIPTOR *callback);
HRESULT		crGetCallback(uint32 type, uint32 index, CALLBACK_DESCRIPTOR **callback);
HRESULT		crRemoveCallback(CALLBACK_DESCRIPTOR *callback);

BOOL		crFindCallback(uint32 mode, uint32 type, uint32 index, CALLBACK_DESCRIPTOR *findcallback, CALLBACK_DESCRIPTOR **callback);

#endif	//_CR_H

