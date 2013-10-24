//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd..
//
// (C) COPYRIGHT 2005 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
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

		Implement AV/C Music sub-unit

		This module is the 'main' for the music subunit.  Currently, this subunit
		executes av/c commands while running on the av/c command handler's thread.

		Basically, it registers a callback with the avc command handler.  When the
		callback is invoked, it executes the command (send command to host), sends
		response and returns to caller.  It does not need to block anywhere, and
		therefore does not need to be its own thread.
 
		PROBLEMS, ASSUMPTIONS and LIMITATIONS
			- this module handles a single instance, subunit id #0
			- no validation of function block type/id

  Revisions:
		created 11/21/2006 BK

***************************************************************************/
#ifndef	_AVCMUSIC_H
#define _AVCMUSIC_H
#if defined (_AVC) && defined (_EDS) && defined (_DRD)

HRESULT		avcMusicInitialize (void);

#endif //_AVC &&  _EDS && _DRD
#endif //_AVCMUSIC_H
