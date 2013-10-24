//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2005 TC Applied Technologies  Ltd.(ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
	
	Purpose:	Workaround for mixer bug at 176.4 and 192kHz

	Description:
						This module contains the basic FIQ routines to implement the two
						workarounds for the mixer hardware errata.
						1) if _MIXFIX_PERFECT4 defined
							Takes 8 mixer outputs and combine them into 4 perfect mixed channels.
							In order to use this route:
								Mix ch 0-3  to APB ch 0-3
								Mix ch 8-11 to APB ch 4-7
							The reconstructed channels will be available on:
								APB Ch 0-3
							NOTE:
								This fix requires the routing of Mix Ch 8-11 to be placed after
								position 20 in the router!!!!!
						
						2) if _MIXFIX_PERFECT4 NOT defined
							Takes 8 mixer outputs and combine them into 8 downsampled mixed channels.
							In order to use this route:
								Mix ch 0-7  to APB ch 0-7
							The reconstructed channels will be available on:
								APB Ch 0-7
							
						This module will automatically create a simple bypass in the APB algo. when
						not in HIGH rate mode. It is however recomended to not route through the APB
						when running at lower rate modes and to disable the FIQ routine.
						The FIQ routine can be enabled/disabled with mixfixEnable.

	Revisions:
		created 17/02/2008 ml

****************************************************************************/
#ifndef _MIXFIX_H_
#define	_MIXFIX_H_

#ifdef _MIXFIX_FIQ
extern HRESULT mixfixEnable (BOOL bEnable);
extern HRESULT mixfixInitialize (void);
#endif //_MIXFIX_FIQ


#endif//_MIXFIX_H_
