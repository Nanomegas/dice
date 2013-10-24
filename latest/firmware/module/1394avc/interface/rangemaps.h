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
		created 21/03/2003 KBJ/LM

*****************************************************************************/
#ifndef _RANGEMAPS_H
#define _RANGEMAPS_H

#include "TCTypes.h"

/*
	INSTRUCTIONS
	
	******* IMPORTANT ********
	only declare a range as a static value, so that namespace collisions dont happen.
		   
	One the major problems with AVC is the number of ranges that must be checked
	in order to determine if a value falls within a valid set.   Some of these
	tables are HUGE, and some are not.  The problem with writing code to deal
	with these range checks is the number of errors that can be introduced. Also
	it makes changing such code, tricky and sometimes dangerous.

	Thus the RANGEMAP, is a way of creating a map of values which are evaluated
	at runtime, against a particular value.

	For instance take the following map from AVC:
	
	static	RANGEMAP	 VALID_SUBUNITTYPES[]=
	{
			{ 1,	RMEVAL_INRANGE,	2},			// RESERVED VALUES
			{ 8,	RMEVAL_INRANGE,	0x1B},		// RESERVED VALUES
			{ 0x1D,	RMEVAL_ONLY,	0},			// A SINGLE RESERVED VALUE
			{ 0,	RMEVAL_END,		0}			// ALL MAPS MUST BE TERMINATED IN THIS FASHION!	
	};
	
	This table describes the INVALID values.  Or those which cannot
	be used because they are RESERVED.  As is the case with most 
	values.
	
	If you called rmInMap(5,&VALID_SUBUNITTYPES), the result would be FALSE.
	If you called rmInMap(1,&VALID_SUBUNITTYPES), the result would be TRUE.
	
*/	

/*
	EVALUATIONS
	
	These are used to populate the operand field of a RANGEITEM.
*/

#define	RMEVAL_END			  0x00		// id defining end of map
#define	RMEVAL_INRANGE		  0x01		// values are between and including the RANGEs in param1 and param2.  
#define RMEVAL_ONLY			  0x02		// values matched against the value of param1. param2, is ignored in this case, and should for clarity purposes, set to 0.

/*
	Used as an element for the evaluation list.
*/
typedef	 struct  
{
	uint32	param1;
	uint8	operand;
	uint32	param2;
} RANGEMAP;

/*********************************************
	FUNCTIONS
**********************************************/

BOOL rmInMap(uint32 value, RANGEMAP* map);

#endif //_RANGEMAPS_H

