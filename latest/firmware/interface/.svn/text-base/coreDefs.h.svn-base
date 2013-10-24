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
/*********************************************************************
	
	Purpose:	ARM core register definition for 
				GPIO, Timer, Remap, Memory Ctl, 
				Interrupt, Watchdog
				Uart, I2C and Power Manager
	
	Revisions:
		created 18/08/2003 kbj


***********************************************************************/
#ifndef _COREDEFS_H
#define _COREDEFS_H

#if defined _DICE2_ASIC && defined ECOS
#elif defined _DICEJR && defined ECOS
#elif defined _DICE2_ASIC && defined GHS
#elif defined _DICE2_FPGA && defined ECOS
#elif defined _DICE2_FPGA && defined GHS
#elif defined _DICE2_WIN && defined WIN32
#else
#error "missing case"
#endif

#ifdef ECOS
#include <cyg/hal/plf_io.h>
#elif defined GHS
#error "ghs defines not included"
#else //ECOS,...
#error "missing case"
#endif //ECOS,...

#endif //_COREDEFS_H
