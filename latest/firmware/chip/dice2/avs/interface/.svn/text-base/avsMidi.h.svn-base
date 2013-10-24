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
//===========================================================================
/****************************************************************************
 * DESCRIPTION: Define the AVSystem API (AVS)

	Exposes an Isochronous Interface configuration API for stream
	of midi data.

	Revisions:
		created 07/28/2003 kbj

****************************************************************************/

#ifndef _AVSMIDI_H
#define _AVSMIDI_H

// Enable or disable midi for a given plug, must be called before avsStart
// Remember to set the midi plug->port mapper before starting
HRESULT	avsSetMidi(uint32 plugID, BOOL bTx, BOOL enable);

#define MAX_MIDI_PLUG_PORT	8

// midiPlug must be 0-7 and is the index of data block in the isoch. data stream
// for where to transmit or receive the midi quadlets
// midiPort is a logical number 0-7 that is stamped in the MSB data byte of 
// the midi quadlet and used to map between plug and port
// If midiPort > 7 then the mapping is disabled

// Set the midiPlug to midiPort mapping for a given AVS audio transmitter 
HRESULT avsSetTxMidiMap(uint32 plugID, uint8 midiPlug, uint8 midiPort);

// Set the midiPlug to midiPort mapping for a given AVS audio receiver 
HRESULT avsSetRxMidiMap(uint32 plugID, uint8 midiPlug, uint8 midiPort);

// Read data from AVS midi receiver, max size is APBA_MIDI_QUAD_OUT_SIZE (16 quads)
HRESULT avsReadMidiData(uint32 *data, uint32 size, uint32 *read);

// Write midi data to AVS midi transmitter, max size is APBA_MIDI_QUAD_IN_SIZE (4 quads)
HRESULT avsWriteMidiData(uint32 plugID, uint32 *data, uint32 size);

void avsEnableMidiInterrupt(BOOL txEnable, BOOL rx0Enable, BOOL rx1Enable);

#endif
