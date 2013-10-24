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
 * DESCRIPTION: Command Line Interface - Serial IO Defines

	Purpose: 

		Defines for the Command Line Interpreter (cli) Serial IO
	
  Revisions:
		created 18/10/2002 lm
****************************************************************************/

#ifndef _CLISIODEFS_H
#define _CLISIODEFS_H

#include "TCTypes.h"

#define KEY_KEYCODES				0x5b	// Keycodes
#define KEY_LEFT_ARROW				0x44	// left arrow
#define KEY_RIGHT_ARROW				0x43	// right arrow
#define KEY_UP_ARROW				0x41	// up arrow
#define KEY_DOWN_ARROW				0x42	// down arrow
#define KEY_HOME					0x42	// home
#define KEY_END						0x42	// home
#define KEY_PAGE_UP					0x42	// page up
#define KEY_PAGE_DOWN				0x42	// page down
								
#define CHAR_BELL					0x07	// Bell Character
#define CHAR_BS						0x08	// backspace
#define CHAR_DEL					0x7f	// forward delete
#define CHAR_ESC					0x1b	// escape
#define CHAR_CR 					0x0d	// return/enter
#define CHAR_LF						0x0a	// linefeed

// Many functions take a 'msecs2wait' parameter, define a very large period (about 10 days)
#define SIO_WAIT_FOREVER 			((uint32)-1)

/*************************************************************

	Channels

*****************************

	The following is a list of all the possible channels that might be
	provided on a platform.  There is no guarantee that any specific
	channel will be supported on a given implementation.

****************************/

// logical channels, zero-based, no direct relationship to physical ports

typedef	uint32	SIO_CHANNEL;

#define	SIO_CHAN_HOST 			0		// host communications
#define	SIO_CHAN_DEBUG			1		// debug output (and possibly input)
#define	SIO_CHAN_MONITOR		2		// monitor program I/O

#define SIO_NUM_CHANNELS		3

#endif //_CLISIODEFS_H

