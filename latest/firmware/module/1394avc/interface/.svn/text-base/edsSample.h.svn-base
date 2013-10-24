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
  FILE PURPOSE:  

	Embedded Descriptor Example for creating and testing a eds Client.

	Creates a descriptor three with two root lists and four entries

	Subunint -+--> Root list ID1 -> Entry OBJID1, 
			  |					    Entry OBJID2, 
			  |					    Entry OBJID3
			  +--> Root list ID2 -> Entry OBJID4 (Callback)
			 					    Entry OBJID5 --> List ID3 -> Entry OBJID7
									Entry OBJID6 [--> List ID1 -> ...] Reassing

  Revisions:
		created 16/09/2003 KBJ

****************************************************************************/
#ifndef _EDS_SAMPLE_H
#define _EDS_SAMPLE_H
#ifdef _EDS

// Create and initialisation of descriptors
HRESULT edsSampleCreate(void);

// Reassign list 1 to list 2
HRESULT edsSampleReassignList1ToList2(void);

// Release sample descriptor three
HRESULT edsSampleReleaseAll(void);

extern EDS_SUBUNITIDENTIFIER siData;

#endif //_EDS
#endif //_EDS_SAMPLE_H
