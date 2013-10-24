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
	These functions allow for checking for RANGEs which fall within specific
	ranges, and assigning these to a map. 
	
	Specifically this was made for AVC, because the number of different fields
	which can have different valid ranges, is infinite, and in AVC, anything
	which is infinite, is used in infinite variations. 

  Revisions:
		created 20/03/2003 KBJ/LM

*****************************************************************************/

static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "rangemaps.h"

BOOL rmInMap(uint32 value, RANGEMAP* map)
{
	HRESULT		hResult = NO_ERROR;
	uint32		count = 0;	
	uint32		higher = 0;
	uint32		lower = 0;
	BOOL		bFound = FALSE;
	
	if (map == NULL)
	{
		hResult = E_NULL_PTR;
		sysLogError(hResult, __LINE__, moduleName);
		return bFound;
	}
	
	// As long as we have an operand and no result, we need to continue checking.
	// All maps will end with 0,0,0, so operand will be false.
	
	DO_FOREVER
	{
		switch(map[count].operand)
		{
			case RMEVAL_INRANGE:
				if (map[count].param1 > map[count].param2)
				{
					higher = map[count].param1;
					lower = map[count].param2;
				}
				else
				{
					higher = map[count].param2;
					lower = map[count].param1;
				}
			
				bFound = (value >= lower && value <= higher);
				break;
		 
			case RMEVAL_ONLY:
				bFound = (value == map[count].param1);
				break;
			
			case RMEVAL_END:
				return bFound;
		}

		if (bFound == TRUE) break;

		count++;
	}
	
	return bFound;
}
