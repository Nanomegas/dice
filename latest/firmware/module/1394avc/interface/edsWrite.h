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

	Embedded Descriptor Services Server, Write operations
	
	This file contains the Write Server routines used in the EDS.  

	
  Revisions:
		created 08/08/2003 KBJ
****************************************************************************/
#ifndef _EDS_WRITE_H
#define _EDS_WRITE_H
#ifdef _EDS

HRESULT	edsHandleWriteDescriptor(EDS_SERVER_CONNECTION*	connection);
HRESULT	edsHandleWriteCallback(EDS_DESCRIPTOR_REF ref, EDS_BUFFER_TYPE buffertype);

#endif //_EDS
#endif //_EDS_WRITE_H
