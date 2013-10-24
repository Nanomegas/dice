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

	Embedded Descriptor Services (EDS)
	
	This file is the public interface for the EDS. Primarily it describes
	the the interface for the client. There are a few interfaces that are
	used for controlling the EDS server as well.
	
	EDS Client:
		The Client API allows for the construction of descriptors, and for
		construction of the descriptor trees. (Tree are not an actual object
		but rather the description of how the descriptors are arranged 
		relative to one another.)  
		
	EDS Server:
		The Server is used to handle AV/C commands which enable another device
		to query and change the contents of descriptors on this system. 
		
		The server also manages descriptor connections from other devices, and
		makes sure that descriptor reads, occur in a mode which is 
		consistent with the originating node of the read.
		
		Primarily the EDS Server only shares data with the EDS Client interface.

  Revisions:
		created 06/05/2003 KBJ/LM

****************************************************************************/
#ifndef	_EDSCLI_H
#define _EDSCLI_H
#ifdef _EDS
#ifdef _CLI
#include "cliToolDefs.h"

void		edsCliPrintResponse(AVC_HEADER *pHeaderInfo, PB *packetBlock);

#ifdef _CLI_TOOL_EDS

HRESULT		edsCmdCliOpenDesc(char cctype, uint8 subfunction);
HRESULT		edsCmdCliReadDesc(char cctype, uint16 address, uint16 data_length);
HRESULT		edsCmdCliWriteDesc(char cctype, uint16 address, uint16 data_length);
HRESULT		edsCmdCliReadInfoBlock(char cctype, uint16 address, uint16 data_length);

void		edsCliInstallTools(void);

#endif //_CLI_TOOL_EDS
#endif //_CLI
#endif //_EDS
#endif //_EDSCLI_H
