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
 	Purpose:	Implementing Interrupt Platform for cygwin
	
	Revisions:
		created 16/09/2004 lm

****************************************************************************/

#include "TCInterrupt.h"
#include "TCTasking.h"
#include <cyg/kernel/kapi.h>
#include <cyg/io/devtab.h>
#include <cyg/hal/plf_io.h>
#include <cyg/hal/hal_intr.h>
#include "targetBoard.h"

/**********************************************************************

 	General

***********************************************************************/
static CYG_INTERRUPT_STATE interrupts;
static BOOL bInterruptDisabled = FALSE;

HRESULT TCInterruptGlobalDisable(void)
{
	HRESULT hResult = NO_ERROR;
	
	if (bInterruptDisabled)
	{
		hResult = E_NOT_INITIALIZED;
		return hResult;
	}
	
	bInterruptDisabled = TRUE;

	HAL_DISABLE_INTERRUPTS(interrupts);

	return hResult;
}

HRESULT TCInterruptGlobalRestore(void)
{
	HRESULT hResult = NO_ERROR;

	if (bInterruptDisabled == FALSE)
	{
		hResult = E_NOT_INITIALIZED;
		return hResult;
	}
	
	bInterruptDisabled = FALSE;

	HAL_RESTORE_INTERRUPTS(interrupts);

	return hResult;
}

HRESULT TCInterruptGlobalEnable(void)
{
	HRESULT hResult = NO_ERROR;

	HAL_ENABLE_INTERRUPTS();

	return hResult;
}

/**********************************************************************

 	FIQ

***********************************************************************/
#ifdef _FIQ
extern TC_INTERRUPT_HANDLER	__fiq_handler;

static void fake_fiq_handler(void) __attribute__  ((section (".sram.fake_fiq_handler")));
int 	fake_fiqcnt					__attribute__  ((section (".sram.fake_fiq_data"))) = 0;

static void fake_fiq_handler(void)
{
	fake_fiqcnt++;
//	*((volatile uint32*) GPIO_A_DR) ^= 0x80;
	*((volatile uint32*)0xce160100) = 1; //clear arm-audio fiq
}
#endif //_FIQ

HRESULT TCInterruptCreateFIQ(TC_INTERRUPT_HANDLER handler)
{
	HRESULT		hResult = NO_ERROR;

#ifdef _FIQ
	//Make sure to disable interrupt and fiq 
	hResult = TCInterruptGlobalDisable();
	if (hResult != NO_ERROR) return hResult;

	if (handler == NULL)
	{
		__fiq_handler = &fake_fiq_handler;
	}
	else
	{
		__fiq_handler = handler;
	}

	hResult = TCInterruptGlobalRestore();
	if (hResult != NO_ERROR) return hResult;

	*((volatile uint32 *) ICTL_FIQ_ENABLE) = 0x1;				//enable fiq0

#else //_FIQ
	hResult = E_NOT_IMPLEMENTED;
#endif //_FIQ

	return hResult;
}

HRESULT TCInterruptDestroyFIQ(void)
{
	HRESULT		hResult = NO_ERROR;

#ifdef _FIQ
	*((volatile uint32 *) ICTL_FIQ_ENABLE) = 0x0;				//disable fiq0

#else //_FIQ
	hResult = E_NOT_IMPLEMENTED;
#endif //_FIQ

	return hResult;
}

typedef struct {
	cyg_interrupt interrupt;
	cyg_handle_t  interrupt_handle;
	TC_INTERRUPT_HANDLER isrHandler;
	TC_INTERRUPT_HANDLER dsrHandler;
} tc_interrupt_t;

static tc_interrupt_t interrupt_table[CYGNUM_HAL_ISR_COUNT];

static cyg_uint32 tc_interrupt_isr(cyg_vector_t vector, cyg_addrword_t data)
{

	cyg_drv_interrupt_mask(vector);
	cyg_drv_interrupt_acknowledge(vector);
	if(interrupt_table[vector].isrHandler) {
		(*interrupt_table[vector].isrHandler)();
	}
	return CYG_ISR_CALL_DSR;
}


static void tc_interrupt_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
	if(interrupt_table[vector].dsrHandler) {
		(*interrupt_table[vector].dsrHandler)();
	}
	cyg_drv_interrupt_unmask(vector);
}

HRESULT	 TCInterruptInstallHandler(uint32 vector, TC_INTERRUPT_HANDLER isr, TC_INTERRUPT_HANDLER dsr) 
{
	interrupt_table[vector].isrHandler = isr;
	interrupt_table[vector].dsrHandler = dsr;
	
	cyg_drv_interrupt_create(vector,
                             99,
							 (cyg_addrword_t)0,
                             tc_interrupt_isr,
                             tc_interrupt_dsr,
                             &interrupt_table[vector].interrupt_handle,
                             &interrupt_table[vector].interrupt);
    cyg_drv_interrupt_attach(interrupt_table[vector].interrupt_handle);
    cyg_drv_interrupt_unmask(vector);
	return NO_ERROR;
}

HRESULT	 TCInterruptInstallEcosHandler(uint32 vector, TC_ECOS_ISR_HANDLER isr, TC_ECOS_DSR_HANDLER dsr)
{
	cyg_drv_interrupt_create(vector,
                             99,
							 (cyg_addrword_t)0,
                             (cyg_ISR_t *)isr,
                             (cyg_DSR_t *)dsr,
                             &interrupt_table[vector].interrupt_handle,
                             &interrupt_table[vector].interrupt);
    cyg_drv_interrupt_attach(interrupt_table[vector].interrupt_handle);
    cyg_drv_interrupt_unmask(vector);
	return NO_ERROR;
}

//We really don't uninstall interrupts, just make the handler to NULL.
HRESULT			TCInterruptUninstallHandler(uint32 vector)
{
	interrupt_table[vector].isrHandler = NULL;
	interrupt_table[vector].dsrHandler = NULL;
	return NO_ERROR;
}
