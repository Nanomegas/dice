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
	
	Purpose:	DICE - Initialization and main service thread
	
	Revisions:
		created 26/06/2003 kbj

****************************************************************************/

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "dice.h"
#include "diceCli.h"

HRESULT diceInitialize(void)
{
	// Initialization of dice modules
	hpllInitialize();
	clkInitialize();

	aesInitialize();
	adatInitialize();
	tdifInitialize();
	dsaiInitialize();
	i2sInitialize();

	// Installation of dice CLI tools
#ifdef _CLI_TOOL_DICE
	diceCliInstallTools();
#endif //_CLI_TOOL_DICE

	return NO_ERROR;
}
