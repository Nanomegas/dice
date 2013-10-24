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

	Embedded Descriptor Services Server
	
	This file contains all the SERVER routines used in the EDS.  I just
	put them here, to keep files from growing too big.
	
  Revisions:
		created 06/05/2003 KBJ/LM
****************************************************************************/
#ifndef _EDS_SERVER_H
#define _EDS_SERVER_H
#ifdef _EDS

#include "TCTypes.h"
#include "avcUtils.h"
#include "drs.h"
#include "edstypes.h"
#include "eds.h"

#ifndef EDS_SERVER_CONNECTION_COUNT
#define	EDS_SERVER_CONNECTION_COUNT 10
#endif

#ifndef	EDS_SERVER_ATTEMPTCONNECTION_COUNT
#define EDS_SERVER_ATTEMPTCONNECTION_COUNT 4	// Number of connections which can be in the attempt to open mode.
#endif

#ifndef EDS_SERVER_SESSION_COUNT	
#define	EDS_SERVER_SESSION_COUNT	20
#endif

#ifndef EDS_MAX_DESCRIPTOR_DATASIZE
// #define	EDS_MAX_DESCRIPTOR_DATASIZE 176		//Maximum number of bytes to read and write.
#define	EDS_MAX_DESCRIPTOR_DATASIZE 174		//Maximum number of bytes to read and write.
#endif

//#define	EDS_CONNECTION_SWEEPER_SLEEP	100	   // Number of ms between connection sweeper cycles.
//#define	EDS_CONNECTION_TIMEOUT			10*60  // Number of connection sweeper executions before a connection is timed out.
#define	EDS_CONNECTION_SWEEPER_SLEEP	5	   // Number of ms between connection sweeper cycles.
#define	EDS_CONNECTION_TIMEOUT			10*6000  // Number of connection sweeper executions before a connection is timed out.

/*
	HANDLER FUNCTIONS
	
	These functions are used to do the work on different descriptor
	requests, from incoming packets.
*/
extern uint32 edsConnectionLockSemID;			
extern uint32 EdsShowParsedData;
extern char   EdsTempRead[512];

extern EDS_SERVER_CONNECTION EdsConnectionList[EDS_SERVER_CONNECTION_COUNT];

HRESULT	edsServerInitialize(void);

HRESULT	edsServerRegisterSession(EDS_CLIENT_SESSION* session);

HRESULT	edsGetBufferTypeSize(EDS_DESCRIPTOR_REF descriptor, 
							 EDS_BUFFER_TYPE  buffertype, 
							 uint32* size);

HRESULT	edsGetDescriptorSize(EDS_DESCRIPTOR_REF		descriptor, 
							 BOOL include_children, uint32*	size);

#endif //_EDS
#endif //_EDS_SERVER_H
