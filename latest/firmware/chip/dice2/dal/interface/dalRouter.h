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
//===========================================================================
// $$
//===========================================================================
/****************************************************************************
	
	Purpose:	DICE Abstraction Layer router API definition
				The exported API should only be used by the "dal" sub-system	

	Revisions:
		created 24/02/2005 ml

****************************************************************************/

#ifndef _DALROUTER_H
#define _DALROUTER_H

#include "dal.h"

/** @addtogroup DAL
	dalRouting - Dice Abstraction Layer Router!
	@{
*/
 
/**  Initialize the internal data structure used to hold the routing information.
//  this function is called by the general 'dal' init routine.
//
//  @return NO_ERROR                        - no errors occured
*/ 
HRESULT dalRouterInitialize (void);


/** Create a router configuration based on the mode and in/out membership info
//
//  This routine builds the router configuration based on the outputs allocated
//  it will store the input membership for validation of further routing calls.
//  All source entries will be set to muted.
//
//  @return NO_ERROR                        - no errors occured
//
*/
HRESULT dalRouterCreateConfiguration (DAL_INTERFACE_ID_ENUM routerId, DAL_RATE_MODE_ENUM rateMode, uint32 inputDevices, uint32 outputDevices);


/**
	Add Route
*/
HRESULT dalRouterAddRoute (DAL_INTERFACE_ID_ENUM routerId, SRC_DESCRIPTION src, DEST_DESCRIPTION dst);

/// @}


#endif // _DALROUTER_H
