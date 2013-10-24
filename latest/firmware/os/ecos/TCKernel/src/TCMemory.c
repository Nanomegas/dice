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
 	Purpose:	Implementing Memory Platform Generic layer
				for the PowerCore platform

	Revisions:
		created 11/12/2002 lm

****************************************************************************/

#include "TCMemory.h"

void memcpy_uint32 (uint32* pDst, const uint32* pSrc, uint32 size)
{
	uint32 i;
	
	for (i = 0; i < size; i++)
	{
		*pDst++ = *pSrc++;
	}
}
