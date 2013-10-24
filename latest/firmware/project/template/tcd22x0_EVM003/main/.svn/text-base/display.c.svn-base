//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
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
//===========================================================================
// $Id: display.c,v 1.7 2007/02/14 19:19:48 zluo Exp $
//===========================================================================
/****************************************************************************
	
	Purpose:	myApp.c - Example of a user application using the DiceDriver
	            and dal interfaces.
	            This example creates a simple break-out-box with 8 channels
	            of AES I/O and 8 channels of ADAT I/O
	
	Revisions:
		created 19/03/2005 ml
		

****************************************************************************/

#include "TCTypes.h"
#include "ErrorCodes.h"
#include "TCTasking.h"
#include "coreDefs.h" //for GPIO definition for DIP switch
#include "display.h"
#include "targetBoard.h"



uint32						displayThreadID = 0;				// clock controller thread
const char*					displayThreadName = "displayThread";
uint32            ledMutex;

static struct led_display_data {
	uint16	LED_value;
	uint16	digit01;
} display_data = {LED_ALL_LED_MASK | LED_MASK(LED_PHY_NRST), 0xFFFF}; 



static uint8  display_values[AM_DISPLAY_VAL_MAX] = {
	0xC0,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,
	0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E,
	0xFF,0x8C,0xBF
};

static void updateDigits (uint16 w)
{
	if (w!=display_data.digit01)
	{
		display_data.digit01 = w;
		targetWriteDisplaySPIWord(display_data.digit01);
	}
}

static void changeLED (uint8 index, uint8 bSet)
{
	TCSemaphoreWait (ledMutex);
	uint16 w = display_data.LED_value;
	if (bSet)
		w |= ((uint16)1)<<index;
	else
		w &= ~(((uint16)1)<<index);	
	if (w!=display_data.LED_value)
	{
		display_data.LED_value = w;
		targetWriteLEDSPIWord(display_data.LED_value);
		//sysDebugPrintf("LED word=0x%04X\r\n",display_data.LED_value);
	}
	TCSemaphoreSignal (ledMutex);
}



void amDisplaySetDigit1(AM_DISPLAY_VAL_t v)
{
	uint16 w = (display_data.digit01 & 0xFF00) | display_values[v % AM_DISPLAY_VAL_MAX];
	updateDigits (w);
}

void amDisplaySetDigit0(AM_DISPLAY_VAL_t v)
{
	uint16 w = (display_data.digit01 & 0x00FF) | (((uint16)display_values[v % AM_DISPLAY_VAL_MAX])<<8);
	updateDigits (w);
}

void amDisplaySetLed(uint8 index)
{
	changeLED (index, 0);
}

void amDisplayClearLed(uint8 index)
{
	changeLED (index, 1);
}

void amDisplayLed(uint8 index, uint8 on)
{
  changeLED (index, !on);
}


/////////////////////////////////////////////////////////////////////////////////////////////
// Put GPIO driven gray encoder interrupt handling here, should be changed later. 
/////////////////////////////////////////////////////////////////////////////////////////////
#include <cyg/hal/hal_platform_ints.h>
#include "TCInterrupt.h"

static int value;
static int lastValue;
static int keyPshed;


int displayEncoderDelta (void)
{
	int delta;
	
	//note wrap around is not handled, would happen after 2^31 clicks!!!
	delta = value - lastValue;
	lastValue = value;
	

  	return delta;	

}

bool displayClicked (void)
{
	bool clicked;
	clicked = keyPshed != 0;
	keyPshed = 0;
	
  	return clicked;	
}

#define ENC_A_BIT 8
#define ENC_B_BIT 7
#define ENC_SW_BIT 6

#define ENC_MASK(x) (((uint16)1)<<(x))
#define ENC_ALL_MASK (ENC_MASK(ENC_A_BIT) | ENC_MASK(ENC_SW_BIT))

static void gpio_dsr(void)
{
	uint32	dummy, status;

	//Read Status
	status = *((uint32*)(GPIO_INT_STATUS));

	//Get the external data, 
	dummy = *((uint32*)(GPIO_EXT_A));
	
	if (status & ENC_MASK(ENC_A_BIT)) 
	{
	  value = (dummy & ENC_MASK(ENC_B_BIT)) ? value+1 : value-1;
  }  
	if (status & ENC_MASK(ENC_SW_BIT)) keyPshed = 1;
	
	
	

	//SYS_TRACE2(SYSDEBUG_TRACE_TESTS, status, dummy);
	*((uint32*)(GPIO_A_EOI)) = ENC_ALL_MASK;  //clearn interrupt
}




HRESULT gpioGrayEnableInitialize(void)
{

	TCInterruptInstallHandler(CYGNUM_HAL_INTERRUPT_GPIO, &gpio_dsr, NULL);
	//enable GPIO interrupts, GPIO6,7,8(0 based) are for gray encoder
	*((volatile uint32 *) GPIO_INT_LEVEL) = ENC_ALL_MASK;  //edge driven
	*((volatile uint32 *) GPIO_INT_POL) = ENC_ALL_MASK;	//rising edge
	*((volatile uint32 *) GPIO_INT_EN) = ENC_ALL_MASK;		//eanble interrupt, mask are by default unmasked
	return NO_ERROR;
}

		
HRESULT displayInitializeTasks(void)
{	
	HRESULT hResult = NO_ERROR;
	
	//Create a mutex for LED updates
	TCSemaphoreOpen (&ledMutex,1);
	
	//hResult = TCTaskCreate(&displayThreadID, displayThreadName, displayThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	//if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT displayResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;


	//hResult = TCTaskResume(displayThreadID);
	//if (hResult != NO_ERROR) return hResult;
	
	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT displaySuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	//hResult = TCTaskSuspend(displayThreadID);
	//if (hResult != NO_ERROR) return hResult;

	return hResult;
}


/*********************************************************
	Initializations for this module
*/

HRESULT	displayInitialize(void)
{
	HRESULT hResult = NO_ERROR;	

	//hResult = displayResumeTasks();
	//if (hResult != NO_ERROR) return hResult;

	//zluo, I put gpioGray stuff here TEMPORORARILY.
	gpioGrayEnableInitialize();
	return hResult;
}




