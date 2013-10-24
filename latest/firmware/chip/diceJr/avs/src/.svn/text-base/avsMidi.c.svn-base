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
* DESCRIPTION: AV System interface implementation.

	Revisions:
		created 06/06/2003 kbj

****************************************************************************/

#include "TCTasking.h"
#include "TCTypes.h"
#include "1394.h"
#include "lal.h"
#include "avsDefs.h"
#include "avs.h"
#include "avsMidi.h"

static char 	moduleName[] = __FILE__ ;

/************************************************************************/
HRESULT	avsSetMidi(uint32 plugID, BOOL bTx, BOOL enable)
{
	HRESULT hResult = NO_ERROR;

	if (bTx)
	{
		if (avsTxPort[plugID].type == AUDIO_TXDI_PORT)
		{
			avsTxPort[plugID].midi = (uint8)enable;
			return hResult;
		}
	}
	else
	{
		if (avsRxPort[plugID].type == AUDIO_RXDO_PORT)
		{
			avsRxPort[plugID].midi = (uint8)enable;
			return hResult;
		}
	}

	hResult = E_AVS_INVALID_PLUGID;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

HRESULT avsSetTxMidiMap(uint32 plugID, uint8 midiPlug, uint8 midiPort)
{
	HRESULT hResult = NO_ERROR;
	uint32 ATXDIBase;
	uint32 ADIMidi;
	uint32 shift;

	if ((avsTxPort[plugID].type == AUDIO_TXDI_PORT) &&
		(midiPlug < MAX_MIDI_PLUG_PORT))
	{
		ATXDIBase = avsTxPort[plugID].baseAddr;
		shift = midiPlug*4;

		avsRegRead(ATXDIBase + ADI_MIDI, &ADIMidi);
		ADIMidi &= ~(0xf << shift);
		if (midiPort < MAX_MIDI_PLUG_PORT)
		{
			ADIMidi |= (ADIO_MIDI_ENABLE | midiPort) << shift;
		}
		avsRegWrite(ATXDIBase + ADI_MIDI, ADIMidi); 
		return hResult;
	}
	else
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
}

HRESULT avsSetRxMidiMap(uint32 plugID, uint8 midiPlug, uint8 midiPort)
{
	HRESULT hResult = NO_ERROR;
	uint32 ARXDOBase;
	uint32 ADOMidi;
	uint32 shift;

	if ((avsRxPort[plugID].type == AUDIO_RXDO_PORT) &&
		(midiPlug < MAX_MIDI_PLUG_PORT))
	{
		ARXDOBase = avsRxPort[plugID].baseAddr;
		shift = midiPlug*4;

		avsRegRead(ARXDOBase + ADO_MIDI, &ADOMidi);
		ADOMidi &= ~(0xf << shift);
		if (midiPort < MAX_MIDI_PLUG_PORT)
		{
			ADOMidi |= (ADIO_MIDI_ENABLE | midiPort) << shift;
		}
		avsRegWrite(ARXDOBase + ADO_MIDI, ADOMidi); 
		return hResult;
	}
	else
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
}

HRESULT avsReadMidiData(uint32 *data, uint32 size, uint32 *read)
{
	HRESULT hResult = NO_ERROR;
	uint32 midiStatus;
	uint32 quads;
	uint32 i;
	
	avsRegRead(APBA_MIDI_APB_IF_STATUS, &midiStatus);
	
	quads = ((midiStatus & MIDI_TX_QUADS_IN_BUF_MASK) >> MIDI_TX_QUADS_IN_BUF_SHIFT);

	for (i = 0; (i < size) && (i < quads); i++)
	{
		avsRegRead(APBA_MIDI_APB_IF_QUAD_OUT, &data[i]);
	}

	*read = i;
	return hResult;
}

HRESULT avsWriteMidiData(uint32 plugID, uint32 *data, uint32 size)
{
	HRESULT hResult = NO_ERROR;
	uint32 midiStatus;
	uint32 i;
	
	avsRegRead(APBA_MIDI_APB_IF_STATUS, &midiStatus);

	if (plugID == 0)
	{
		if (midiStatus & MIDI_BUF_EMPTY_RX0) // Midi Rx0 buffer is empty
		{
			for (i = 0; i < APBA_MIDI_QUAD_IN_SIZE && i < size; i++)
			{
				avsRegWrite(APBA_MIDI_APB_IF_QUAD_IN0, data[i]);
			}
			return hResult;
		}
	}

	if (plugID == 1)
	{
		if (midiStatus & MIDI_BUF_EMPTY_RX1) // Midi Rx1 buffer is empty
		{
			for (i = 0; i < APBA_MIDI_QUAD_IN_SIZE && i < size; i++)
			{
				avsRegWrite(APBA_MIDI_APB_IF_QUAD_IN1, data[i]);
			}
			return hResult;
		}
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;

}

void avsEnableMidiInterrupt(BOOL txEnable, BOOL rx0Enable, BOOL rx1Enable)
{
	uint32 midiControl = 0;

	if (txEnable) midiControl |= MIDI_TX_IRQ_EN;
	if (rx0Enable) midiControl |= MIDI_RX0_IRQ_EN;	
	if (rx1Enable) midiControl |= MIDI_RX1_IRQ_EN;	

	avsRegWrite(APBA_MIDI_APB_IF_CONTROL, midiControl);
}
