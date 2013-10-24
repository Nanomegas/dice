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
	
	Purpose:	Abstract MIDI Layer
  
  Description:       
  
	Revisions:
		created 09/18/2006 zluo

****************************************************************************/
#include "TCTypes.h"

#include "aml.h"
#include "TCSemaphore.h"
#include "TCDebug.h"
#include "avsDefs.h"
#include "avs.h"

#include <cyg/hal/plf_io.h>
#include <cyg/hal/hal_platform_ints.h>
#include "TCTasking.h"
#include "TCInterrupt.h"

#include "avsMidi.h"

//***************************************************************************
//* avs mid dev code
//***************************************************************************
#define AVS_STREAM				6 
#define AVS_CHAN_PER_STREAM		8

static BOOL bInterruptEanbled = FALSE;

typedef struct {
	BOOL		enabled;
	uint32		aml_dev_id;
} avs_midi_drv_dev_t;

static avs_midi_drv_dev_t avsMidiDevTable[AVS_STREAM][AVS_CHAN_PER_STREAM]; 


static void AvsMidiEventHandler(void)
{
	uint32	MidiStatus;	
	uint32	bytes;
	uint32	midiport;
	uint32	midirx;
	uint32	data;

	//Clear Interrupt and Read Status
	avsRegRead(APBA_MIDI_APB_IF_STATUS, &MidiStatus);
	
	//ML: for diagnostic purposes
	bytes = ((MidiStatus & MIDI_TX_QUADS_IN_BUF_MASK) >> MIDI_TX_QUADS_IN_BUF_SHIFT);
	if (bytes > 10)
	{
		SYS_TRACE1(SYSDEBUG_TRACE_AML, bytes);
	}

	while (!(MidiStatus & MIDI_BUF_EMPTY_TX))
	{
		//Read Midi
		avsRegRead(APBA_MIDI_APB_IF_QUAD_OUT, &data);
		//Write to Midi Port
		bytes = ((data >> 24) & 0x03 );
		midirx = ((data >> 27) & 0x03 );
		midiport = ((data >> 29) & 0x07);

		//SYS_TRACE3(SYSDEBUG_TRACE_AML, midirx, avsMidiDevTable[midirx+2][midiport].enabled, data);
		if(avsMidiDevTable[midirx+2][midiport].enabled) {
			int i;
			//SYS_TRACE2(SYSDEBUG_TRACE_AML, bytes, avsMidiDevTable[midirx+2][midiport].enabled);
			for(i = 0; i < bytes; i++) {
				amlDrvIndRxByte(avsMidiDevTable[midirx+2][midiport].aml_dev_id, (data >> ((2-i) * 8)& 0xff));
			}
		}
		avsRegRead(APBA_MIDI_APB_IF_STATUS, &MidiStatus); //clean interrupt
	}
}

void avsMidiDev_init(uint32 dev_id, uint32 port, uint32* retHandle)
{
	if(port / 8 > 6) {
		SYS_DEBUG(SYSDEBUG_TRACE_AML, "wrong port number.\n");
		return;
	}

	avsMidiDevTable[port/8][port%8].aml_dev_id = dev_id;
	avsMidiDevTable[port/8][port%8].enabled = 0; //dont' enable it yet.

	*retHandle = port;
	if(!bInterruptEanbled) {
		bInterruptEanbled = TRUE;
		TCInterruptInstallHandler(CYGNUM_HAL_INTERRUPT_MIDI, NULL, &AvsMidiEventHandler);
		avsEnableMidiInterrupt(true,false,false);
	}
}

bool avsMidiDev_putChar(uint32 handle, uint8 data)
{
	uint32	port = handle;
	uint32	dummy;
	if(avsMidiDevTable[port/8][port%8].enabled) {
		//Format data for AVS always to Midi 0
		dummy = ((data << 16) |			//data
				0x01000000 |			//1 byte
				((port & 7) <<  29));	//with right channel
		//Write to AVS

		if(port & 0x8)
			avsRegWrite(APBA_MIDI_APB_IF_QUAD_IN1, dummy);
		else
			avsRegWrite(APBA_MIDI_APB_IF_QUAD_IN0, dummy);
		//avsWriteMidiData(((handle & 0x8)>>3), &dummy, 1);	
	}
	return true; //should make interrupt driven TX
}

void avsMidiDev_enable(uint32 port)
{
	avsMidiDevTable[port/8][port%8].enabled = 1; 
}

void avsMidiDev_disable(uint32 port)
{
	avsMidiDevTable[port/8][port%8].enabled = 0; 
};

midi_dev_cb_t avs_midi_dev_cbs = {
	&avsMidiDev_init,
	&avsMidiDev_putChar, 
	&avsMidiDev_enable, 
	&avsMidiDev_disable
};
