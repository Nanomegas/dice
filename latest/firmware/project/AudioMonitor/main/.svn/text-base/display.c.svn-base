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

// We need to define how we present ourselves to the PC and Mac driver. We will
// need to specify our Isco channel layout, names for ASIO and CoreAudio etc.
// Starting and stopping the 1394 Isoc streams will be handled by the DiceDriver
// module.
//
// This file uses myModes to select between a number of configurations of the board
// The current mode is stored in the EEPROM unless MYAPP_MODE0_ONLY is defined in 
// which case the myMode0.c file defines the mode.

// task (thread) ID for this module									


uint32						displayThreadID = 0;				// clock controller thread
const char*					displayThreadName = "displayThread";

static struct led_display_data {
	uint8	LED_value;
	uint8	digit0, digit1;
} display_data = { 0xFF, 0xFF, 0xFF}; 


int dispEnabled=1;

static uint8  display_values[AM_DISPLAY_VAL_MAX] = {
	0x44, 0xcf, 0xa4, 0x86, 0x0f, 0x16, 0x14, 0xc7, 0x04, 0x06, 0x05, 0x1c, 0x74, 0x8c, 0x34, 0x35,
	0xff,0x25,0xbf
};

void amDisplaySetDigit0(AM_DISPLAY_VAL_t v)
{
	display_data.digit0 = v % AM_DISPLAY_VAL_MAX;
}
void amDisplaySetDigit1(AM_DISPLAY_VAL_t v)
{
	display_data.digit1 = v % AM_DISPLAY_VAL_MAX;
}

void amDisplaySetLed(uint8 index)
{
	index --;
	display_data.LED_value &= ~(1<<(index %AM_MAX_LED_INDEX));
}

void amDisplayClearLed(uint8 index)
{
	index --;
	display_data.LED_value |= 1<<(index %AM_MAX_LED_INDEX);
}

void amDisplayLed(uint8 index, uint8 on)
{
  if (on)
	amDisplaySetLed(index);
  else
	amDisplayClearLed(index);
}

static void displayThread(void *dummy)
{
	uint8 row=0;

	TCTaskWait(20);
#ifdef _DISPLAY_TEST
	uint32 counter  = 0;
#endif //_DISPLAY_TEST

	uint16 data[3];
	DO_FOREVER
	{	
#ifdef _DISPLAY_TEST
		uint32  num;
		counter++; 
		if(counter % 100 == 0) {
			num = counter / 100;
			amDisplaySetDigit0(num & 0xF);
			amDisplaySetDigit1((num>>4) & 0xF);
			(num & 0x1) ? amDisplaySetLed(5) : amDisplayClearLed(5);
			(num & 0x2) ? amDisplaySetLed(2) : amDisplayClearLed(2);
			(num & 0x4) ? amDisplaySetLed(1) : amDisplayClearLed(1);
			(num & 0x8) ? amDisplaySetLed(4) : amDisplayClearLed(4);
			(num & 0x10) ? amDisplaySetLed(3) : amDisplayClearLed(3);
		};
#endif //_DISPLAY_TEST
		TCTaskWait(10);
		if (dispEnabled)
		{
			//*((volatile uint32 *) SPI_CTRL) = (uint32)0x45;


			data[0] = (display_data.LED_value << 11) |  //LED
									(0x1 << 8)|						//first digit
									display_values[display_data.digit0];
			data[1] = (display_data.LED_value << 11) |  //LED
									(0x2 << 8) |					//2nd digit
									display_values[display_data.digit1];

			if (!row)
			{
				targetWriteDisplaySPIWord(data[0]);
				targetWriteDisplaySPIWord(data[1]);
				targetWriteDisplaySPIWord(data[0]);
			}
			else
			{
				targetWriteDisplaySPIWord(data[1]);
				targetWriteDisplaySPIWord(data[0]);
				targetWriteDisplaySPIWord(data[1]);
			}
			row = !row;
			//targetWriteSPIWord(data[2]);
			//*((volatile uint32 *) SPI_CTRL) = (uint32)0x75;
		}
		
	}
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


static void gpio_dsr(void)
{
	uint32	dummy, status;

	//Read Status
	status = *((uint32*)(GPIO_INT_STATUS));

	//Get the external data, 
	dummy = *((uint32*)(GPIO_EXT_A));
	
	if (status & 0x100) 
	{
	  value = (dummy & 0x80) ? value+1 : value-1;
   	  //amDisplaySetDigit1((value>>4) & 0x0f);
	  //amDisplaySetDigit0(value & 0x0f);
	}
	/*
	if (status & 0x80) 
	{
	  value = (dummy & 0x100) ? value-1 : value+1;
	}
	*/
    
	if (status & 0x40) keyPshed = 1;
	
	
	

	//SYS_TRACE2(SYSDEBUG_TRACE_TESTS, status, dummy);
	*((uint32*)(GPIO_A_EOI)) = 0x1C0;  //clearn interrupt
}




HRESULT gpioGrayEnableInitialize(void)
{

	TCInterruptInstallHandler(CYGNUM_HAL_INTERRUPT_GPIO, &gpio_dsr, NULL);
	//enable GPIO interrupts, GPIO6,7,8(0 based) are for gray encoder
	*((volatile uint32 *) GPIO_INT_LEVEL) = 0x1C0;  //edge driven
	*((volatile uint32 *) GPIO_INT_POL) = 0x1C0;	//rising edge
	*((volatile uint32 *) GPIO_INT_EN) = 0x1C0;		//eanble interrupt, mask are by default unmasked
	return NO_ERROR;
}

		
HRESULT displayInitializeTasks(void)
{	
	HRESULT hResult = NO_ERROR;
	
	hResult = TCTaskCreate(&displayThreadID, displayThreadName, displayThread, TASK_PRIORITY_DEFAULT, TASK_STACKSIZE_DEFAULT);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT displayResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;


	hResult = TCTaskResume(displayThreadID);
	if (hResult != NO_ERROR) return hResult;
	
	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT displaySuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	hResult = TCTaskSuspend(displayThreadID);
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}


/*********************************************************
	Initializations for this module
*/

HRESULT	displayInitialize(void)
{
	HRESULT hResult = NO_ERROR;	

	hResult = displayResumeTasks();
	if (hResult != NO_ERROR) return hResult;

	//zluo, I put gpioGray stuff here TEMPORORARILY.
	gpioGrayEnableInitialize();
	return hResult;
}




