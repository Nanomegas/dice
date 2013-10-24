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
  * DESCRIPTION: Command Line Interface - Serial IO 

	Purpose: 

		This file describes the Command Line Interpreter (cli) Serial IO
	
  Revisions:
		created 18/10/2002 lm
****************************************************************************/

#ifndef _CLISIO_H
#define _CLISIO_H

#ifdef _CLI

#include "TCTypes.h"
#include "TCDebug.h"
#include "cliCB.h"
#include "cliSIODefs.h"

#define		cliOutputLine(text)	os_printf("%s\n\r", text)
#define		cliOutputRaw		os_printf
#define		cliPrintf			os_printf

HRESULT		cliSIOInitialize (void);															// cliSIOInitialize - Initializes Serial IO module
																								
void 		cliSIOHandleNewCharIRQ(BYTE value);													// cliSIOHandleNewCharIRQ - Serial IO Interrupt Handler calls this function when new char has arrived
char* 		cliSIOWaitForLine(void);															// cliSIOWaitForCommandLine - Returns pointer to a new string when sent from Interrupt Handler

HRESULT		cliSIOWriteChar (SIO_CHANNEL channel, char txChar, uint32 msecs2Wait);				// cliSIOWriteChar - Write one character to the serial channel
HRESULT		cliSIOWriteString (SIO_CHANNEL channel, void *pStrBuf, uint32 msecs2Wait);			// cliSIOWriteString - Writes a null-terminated ascii string to the serial channel
HRESULT		cliSIOWriteBuf (SIO_CHANNEL channel, void *pTxBuf, int32 len, uint32 msecs2Wait);	// cliSIOWriteBuf - Writes multiple characters to the serial channel

void		cliSIOEchoOn(BOOL bEchoOn);															// cliSIOEchoOn - Turns on/off the echoing of characters
BOOL		cliSIOIsEchoOn(void);																// cliSIOIsEchoOn - Return the echoing of characters mode

#endif //_CLI
#endif //_CLISIO_H

