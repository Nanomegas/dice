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
 	Purpose:	Implementing Interrupt Platform Generic layer
	
	Revisions:
		created 16/09/2004 lm

****************************************************************************/

#ifndef _TC_INTERRUPT_H_
#define _TC_INTERRUPT_H_

#include "TCTypes.h"

typedef void (*TC_INTERRUPT_HANDLER)(void);


HRESULT 		TCInterruptGlobalDisable(void);		// Disable any enabled IRQ and FIQ interrupts
HRESULT 		TCInterruptGlobalRestore(void);		// Restore interrupts disabled by TCInterruptGlobalDisable()
HRESULT 		TCInterruptGlobalEnable(void);		// Enable all IRQ and FIQ interrupts

HRESULT 		TCInterruptCreateFIQ(TC_INTERRUPT_HANDLER handler);
HRESULT 		TCInterruptDestroyFIQ(void);

HRESULT			TCInterruptInstallHandler(uint32 vector, TC_INTERRUPT_HANDLER isr, TC_INTERRUPT_HANDLER dsr);
HRESULT			TCInterruptUninstallHandler(uint32 vector);


typedef uint32 (*TC_ECOS_ISR_HANDLER)(uint32 vector, void * data);
typedef void   (*TC_ECOS_DSR_HANDLER)(uint32 vector, uint32 count, void * data);

HRESULT	 TCInterruptInstallEcosHandler(uint32 vector, TC_ECOS_ISR_HANDLER isr, TC_ECOS_DSR_HANDLER dsr);

#endif //_TC_INTERRUPT_H_
