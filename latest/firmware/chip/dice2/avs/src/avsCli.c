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
* DESCRIPTION: AV System interface implementation CLI

	Revisions:
		created 09/17/2002 lm

****************************************************************************/

#ifdef _CLI
#include "cliToolDefs.h"
#ifdef _CLI_TOOL_AVS
#include "TCTasking.h"
#include "avsDefs.h"
#include "avsFmtDefs.h"
#include "avs.h"
#include "avsInt.h"
#include "avsCli.h"
#include "avsMidi.h"
#include "avsFmt.h"
#include "cli.h"
#ifdef _DICE2
#include "llcReg.h"
#endif //_DICE2

//static char 		moduleName[] = __FILE__ ;

enum
{
	AVS_EVENT_CLEAR = 0,
	AVS_EVENT_DUMP,
	AVS_EVENT_DUMP_IFSET,
	AVS_EVENT_DUMP_IFSET_AND_CLEAR
};

// Display all AVS registers
static HRESULT avsCliEvent(uint32 operation)
{
	uint32	i;
	uint32	j;

	cliPrintf("avs event mode: %s\n\r", avsIntGetPollEnable() ? "polling" : "interrupt");

	switch (operation) // dump
	{
		case AVS_EVENT_DUMP:
		case AVS_EVENT_DUMP_IFSET:
			for (i = 0; i < NUM_AVS_EVENTS; i++)
			{
				cliPrintf("AVS%i interrupt event counters:\n\r", i);
				for (j = 0; j < avsIntEventState[i].numEventItems; j++)
				{
					if (operation == AVS_EVENT_DUMP ||
						avsIntEventState[i].avsIntCounter[j])
					{
						cliPrintf("%8i %s\n\r", avsIntEventState[i].avsIntCounter[j], (char *)((int)avsIntEventState[i].avsIntTexts + AVS_EVENT_TEXT_SIZE * j));
					}
				}
			}
			break;

		case AVS_EVENT_DUMP_IFSET_AND_CLEAR:
			for (i = 0; i < NUM_AVS_EVENTS; i++)
			{
				cliPrintf("AVS%i interrupt event counters:\n\r", i);
				for (j = 0; j < avsIntEventState[i].numEventItems; j++)
				{
					if (avsIntEventState[i].avsIntCounter[j])
					{
						cliPrintf("%8i %s\n\r", avsIntEventState[i].avsIntCounter[j], (char *)((int)avsIntEventState[i].avsIntTexts + AVS_EVENT_TEXT_SIZE * j));
					}
				}
			}

		case AVS_EVENT_CLEAR:
			avsIntClearCounters();
			break;
	}

	return NO_ERROR;
}

void avsDumpEventCounters(uint32*data)
{
	int i, j, counter = 0; 
	for (i = 0; i < NUM_AVS_EVENTS; i++)
	{
		for (j = 0; j < avsIntEventState[i].numEventItems; j++)
		{
			data[counter++] = avsIntEventState[i].avsIntCounter[j];
		}
	}
}

// Display AVS registers
static HRESULT avsCliDumpPlug(uint32 plugID, BOOL bTx, BOOL bAll)
{
	uint32	avsReg;
	uint32	base;
	uint32  i,part;

	if (bTx)
	{
		switch (plugID)
		{
			case 1:
				base = APBA_ATXDI1_BASE;
				break;
			case 2:
				base = APBA_ATXDI2_BASE;
				break;
			default:
				return E_BAD_INPUT_PARAMETERS;
		}

		cliPrintf("AVS%i Tx registers:\n\r", plugID);
		avsRegRead(base + ATX_CFG, &avsReg);
		part = avsReg & 0x07;
		cliPrintf("\tATX_CFG   : 0x%08x", avsReg);
		avsRegRead(base + ATX_CIP1, &avsReg);
		cliPrintf("\tATX_CIP1  : 0x%08x\n\r", avsReg);
		avsRegRead(base + ATX_TSTAMP, &avsReg);
		cliPrintf("\tATX_TSTAMP: 0x%08x", avsReg);
		avsRegRead(base + ADI_CFG, &avsReg);
		cliPrintf("\tADI_CFG   : 0x%08x\n\r", avsReg);
		avsRegRead(base + ATX_PHDR, &avsReg);
		cliPrintf("\tATX_PHDR  : 0x%08x", avsReg);
		avsRegRead(base + ADI_MIDI, &avsReg);
		cliPrintf("\tADI_MIDI  : 0x%08x\n\r", avsReg);
		avsRegRead(base + ATX_CIP0, &avsReg);
		cliPrintf("\tATX_CIP0  : 0x%08x\n\r", avsReg);
	}
	else
	{
		switch (plugID)
		{
			case 1:
				base = APBA_ARXDO1_BASE;
				break;
			case 2:
				base = APBA_ARXDO2_BASE;
				break;
			case 3:
				base = APBA_ARXDO3_BASE;
				break;
			case 4:
				base = APBA_ARXDO4_BASE;
				break;
			default:
				return E_BAD_INPUT_PARAMETERS;
		}

		cliPrintf("AVS%i Rx registers:\n\r", plugID);
		avsRegRead(base + ARX_CFG0, &avsReg);
		part = (avsReg >> 6) & 0x07;
		cliPrintf("\tARX_CFG0  : 0x%08x", avsReg);
		avsRegRead(base + ARX_QSEL4, &avsReg);
		cliPrintf("\tARX_QSEL4 : 0x%08x\n\r", avsReg);
		avsRegRead(base + ARX_CFG1, &avsReg);
		cliPrintf("\tARX_CFG1  : 0x%08x", avsReg);
		avsRegRead(base + ARX_PHDR, &avsReg);
		cliPrintf("\tARX_PHDR  : 0x%08x\n\r", avsReg);
		avsRegRead(base + ARX_QSEL0, &avsReg);
		cliPrintf("\tARX_QSEL0 : 0x%08x", avsReg);
		avsRegRead(base + ARX_CIP0, &avsReg);
		cliPrintf("\tARX_CIP0  : 0x%08x\n\r", avsReg);
		avsRegRead(base + ARX_QSEL1, &avsReg);
		cliPrintf("\tARX_QSEL1 : 0x%08x", avsReg);
		avsRegRead(base + ARX_CIP1, &avsReg);
		cliPrintf("\tARX_CIP1  : 0x%08x\n\r", avsReg);
		avsRegRead(base + ARX_QSEL2, &avsReg);
		cliPrintf("\tARX_QSEL2 : 0x%08x", avsReg);
		avsRegRead(base + ADO_CFG, &avsReg);
		cliPrintf("\tADO_CFG   : 0x%08x\n\r", avsReg);
		avsRegRead(base + ARX_QSEL3, &avsReg);
		cliPrintf("\tARX_QSEL3 : 0x%08x", avsReg);
		avsRegRead(base + ADO_MIDI, &avsReg);
		cliPrintf("\tADO_MIDI  : 0x%08x\n\r", avsReg);
	}
	
	if (bAll)
	{
		cliPrintf("AVS partition:\n\r");
		for (i = 0; i < NUM_PARTITIONS; i++)
		{
			avsRegRead(APBA_PARTITION0_BASE+i*(APBA_PARTITION1_BASE-APBA_PARTITION0_BASE), &avsReg);
			cliPrintf("\tP%i_BASE   : 0x%08x", i, avsReg);
			avsRegRead(APBA_PARTITION0_LIMIT+i*(APBA_PARTITION1_BASE-APBA_PARTITION0_BASE), &avsReg);			
			cliPrintf("\tP%i_LIMIT  : 0x%08x", i, avsReg);
			if (i == part) 
				cliPrintf("*\r\n");
			else
				cliPrintf("\r\n");			
		}

		cliPrintf("AVS interrupt0:\n\r");
		avsRegRead(APBA_INT0_STATUS, &avsReg);
		cliPrintf("\tI0_STATUS : 0x%08x", avsReg);
		avsRegRead(APBA_INT0_MASK, &avsReg);
		cliPrintf("\tI0_MASK   : 0x%08x\n\r", avsReg);

		cliPrintf("AVS midi:\n\r");
		avsRegRead(APBA_MIDI_APB_IF_STATUS, &avsReg);
		cliPrintf("\tSTATUS    : 0x%08x", avsReg);
		avsRegRead(APBA_MIDI_APB_IF_CONTROL, &avsReg);
		cliPrintf("\tCONTROL   : 0x%08x\n\r", avsReg);

		cliPrintf("AVS ITP:\n\r");
		avsRegRead(ITP_CFG, &avsReg);
		cliPrintf("\tITP_CFG   : 0x%08x\n\r", avsReg);
		
		cliPrintf("AVS STATE: ");
        switch (bTx ? avsTxPort[plugID-1].state : avsRxPort[plugID-1].state)
        {
            case 	AVS_STATE_DISABLED:    cliPrintf("DISABLED\n\r");break;
	        case 	AVS_STATE_WAIT_FLOW:   cliPrintf("WAIT_FLOW\n\r");break;
	        case 	AVS_STATE_CLR_EVT:     cliPrintf("CLR_EVT\n\r");break;
	        case 	AVS_STATE_CHK_EVT:     cliPrintf("CHK_EVT\n\r");break;
	        case 	AVS_STATE_TGL_RX:      cliPrintf("TGL_RX\n\r");break;
	        case 	AVS_STATE_LOCKED:      cliPrintf("LOCKED\n\r");break;
            default:                       cliPrintf("ILLEGAL\n\r");break;
        }
	}

	return NO_ERROR;
}

// Display all AVS format registers
static HRESULT avsCliDumpFmt(uint32 plugID, BOOL bTx, BOOL bAll)
{
	uint32	avsReg;
	uint32	base;
	uint32	i;
	uint32	avsRegAddr;

	if (bTx)
	{
		switch (plugID)
		{
			case 1:
				base = APBA_ATXDI1_BASE;
				break;
			case 2:
				base = APBA_ATXDI2_BASE;
				break;
			default:
				return E_BAD_INPUT_PARAMETERS;
		}

		cliPrintf("AVS%i Tx format registers:\n\r", plugID);
		avsRegRead(base + FMT_TXDI_CFG0, &avsReg);
		cliPrintf("\tFMT_TXDI_CFG0   : 0x%08x", avsReg);
		avsRegRead(base + FMT_TXDI_CFG1, &avsReg);
		cliPrintf("\tFMT_TXDI_CFG1   : 0x%08x\n\r", avsReg);
		avsRegRead(base + FMT_TXDI_CFG2, &avsReg);
		cliPrintf("\tFMT_TXDI_CFG2   : 0x%08x", avsReg);
		avsRegRead(base + FMT_TXDI_CFG3, &avsReg);
		cliPrintf("\tFMT_TXDI_CFG3   : 0x%08x\n\r", avsReg);
		avsRegRead(base + FMT_TXDI_CFG4, &avsReg);
		cliPrintf("\tFMT_TXDI_CFG4   : 0x%08x", avsReg);
		avsRegRead(base + FMT_TXDI_CFG5, &avsReg);
		cliPrintf("\tFMT_TXDI_CFG5   : 0x%08x\n\r", avsReg);
		avsRegRead(base + FMT_TXDI_CFG6, &avsReg);
		cliPrintf("\tFMT_TXDI_CFG6   : 0x%08x\n\r", avsReg);
		avsRegRead(base + FMT_TXDI_CH_ST, &avsReg);
		cliPrintf("\tFMT_TXDI_CH_ST0 : 0x%08x", avsReg);
		avsRegRead(base + FMT_TXDI_CH_ST1, &avsReg);
		cliPrintf("\tFMT_TXDI_CH_ST1 : 0x%08x\n\r", avsReg);
		avsRegRead(base + FMT_TXDI_CH_ST2, &avsReg);
		cliPrintf("\tFMT_TXDI_CH_ST2 : 0x%08x", avsReg);
		avsRegRead(base + FMT_TXDI_CH_ST3, &avsReg);
		cliPrintf("\tFMT_TXDI_CH_ST3 : 0x%08x\n\r", avsReg);
		avsRegRead(base + FMT_TXDI_CH_ST4, &avsReg);
		cliPrintf("\tFMT_TXDI_CH_ST4 : 0x%08x", avsReg);
		avsRegRead(base + FMT_TXDI_CH_ST5, &avsReg);
		cliPrintf("\tFMT_TXDO_CH_ST5 : 0x%08x\n\r", avsReg);

		if (bAll)
		{
			avsRegAddr = base + FMT_TXDI_CH_CS;
			cliPrintf("AVS%i Tx specific channel status FMT_TXDI_CH_CS :", plugID);
			for (i = 0; i < AVS_CHANNELS; i++)
			{
				if (i%4 == 0) cliPrintf("\n\r\t"); // New line
				avsRegRead(avsRegAddr, &avsReg);
				cliPrintf("0x%08x ", avsReg);
				avsRegAddr += 4;
			}
			cliPrintf("\n\r");
		}
	}
	else
	{
		switch (plugID)
		{
			case 1:
				base = FMT_ARXDO1_BASE;
				break;
			case 2:
				base = FMT_ARXDO2_BASE;
				break;
			case 3:
				base = FMT_ARXDO3_BASE;
				break;
			case 4:
				base = FMT_ARXDO4_BASE;
				break;
			default:
				return E_BAD_INPUT_PARAMETERS;
		}

		cliPrintf("AVS%i Rx format registers:\n\r", plugID);
		avsRegRead(base + FMT_RXDO_SETUP, &avsReg);
		cliPrintf("\tFMT_RXDO_SETUP  : 0x%08x\n\r", avsReg);
		avsRegRead(base + FMT_RXDO_LABEL, &avsReg);
		cliPrintf("\tFMT_RXDO_LABEL0 : 0x%08x", avsReg);
		avsRegRead(base + FMT_RXDO_LABEL1, &avsReg);
		cliPrintf("\tFMT_RXDO_LABEL1 : 0x%08x\n\r", avsReg);
		avsRegRead(base + FMT_RXDO_LABEL2, &avsReg);
		cliPrintf("\tFMT_RXDO_LABEL2 : 0x%08x", avsReg);
		avsRegRead(base + FMT_RXDO_LABEL3, &avsReg);
		cliPrintf("\tFMT_RXDO_LABEL3 : 0x%08x\n\r", avsReg);
		avsRegRead(base + FMT_RXDO_CS_ST, &avsReg);
		cliPrintf("\tFMT_RXDO_CS_ST0 : 0x%08x", avsReg);
		avsRegRead(base + FMT_RXDO_CS_ST1, &avsReg);
		cliPrintf("\tFMT_RXDO_CS_ST1 : 0x%08x\n\r", avsReg);
		avsRegRead(base + FMT_RXDO_CS_ST2, &avsReg);
		cliPrintf("\tFMT_RXDO_CS_ST2 : 0x%08x", avsReg);
		avsRegRead(base + FMT_RXDO_CS_ST3, &avsReg);
		cliPrintf("\tFMT_RXDO_CS_ST3 : 0x%08x\n\r", avsReg);
		avsRegRead(base + FMT_RXDO_CS_ST4, &avsReg);
		cliPrintf("\tFMT_RXDO_CS_ST4 : 0x%08x", avsReg);
		avsRegRead(base + FMT_RXDO_CS_ST5, &avsReg);
		cliPrintf("\tFMT_RXDO_CS_ST5 : 0x%08x\n\r", avsReg);
	}

	return NO_ERROR;
}

static HRESULT avsCliIT(uint32 plugID, BOOL bTx, BOOL bOn)
{
	HRESULT		hResult = NO_ERROR;

	if (bOn)
	{
		AVSPORT *pPort = NULL;
		hResult = avsStart(plugID-1, bTx);
		if (hResult != NO_ERROR) return hResult;

		pPort = (bTx) ? &(avsTxPort[plugID-1]) : &(avsRxPort[plugID-1]);
		sysDebugPrintf("avs Isoch %s Test for plugID: %i ch:%i,SFC:%i,DBS:%i\n\r", (bTx) ? "Tx" : "Rx", plugID, pPort->channel, pPort->SFC, pPort->DBS);
	}
	else
	{
		hResult = avsStop(plugID-1, bTx);
		if (hResult != NO_ERROR) return hResult;
	}

	return hResult;
}


static HRESULT avsCliITC(uint32 plugID, BOOL bTx, uint8 channel)
{
	HRESULT		hResult = NO_ERROR;

	hResult = avsSetCh(plugID-1, bTx, channel);
	if (hResult != NO_ERROR) return hResult;

	sysDebugPrintf("Isoch %s channel for plugID: %i is now: %i\n\r", (bTx) ? "Tx" : "Rx", plugID, channel);
	sysDebugPrintf("Restart %s to make changes take effect\n\r", (bTx) ? "Tx" : "Rx");

	return hResult;
}

static HRESULT avsCliTxPre (uint32 plugID, uint8 samples, uint8 phase)
{
	HRESULT		hResult = NO_ERROR;

	hResult = avsSetTxPresntOffset(plugID-1, samples, phase);
	if (hResult != NO_ERROR) return hResult;

	sysDebugPrintf("Restart Tx to make changes take effect\n\r");

	return hResult;
}

static HRESULT avsCliRxPre (uint32 plugID, int8 samples, uint8 phase)
{
	HRESULT		hResult = NO_ERROR;

	hResult = avsSetRxPresntAdjust(plugID-1, samples, phase);
	if (hResult != NO_ERROR) return hResult;

	sysDebugPrintf("Restart Rx to make changes take effect\n\r");

	return hResult;
}


static HRESULT avsCliITS(uint32 plugID, BOOL bTx, uint8 sampleRate)
{
	HRESULT		hResult = NO_ERROR;

	hResult = avsSetSampleRate(plugID-1, bTx, sampleRate);
	if (hResult != NO_ERROR) return hResult;

	sysDebugPrintf("Isoch %s channel SFC for plugID: %i is now: %i\n\r", (bTx) ? "Tx" : "Rx", plugID, sampleRate);
	sysDebugPrintf("Restart %s to make changes take effect\n\r", (bTx) ? "Tx" : "Rx");

	return hResult;
}

static HRESULT avsCliDBSX(uint32 plugID, BOOL bTx, uint32 channels, uint32 bMidi)
{
	HRESULT		hResult = NO_ERROR;
	uint8		slot;
	uint8		index;

	avsSetMidi(plugID-1, bTx, bMidi);
#if 1
	// Set default rx slot indexes for a QSel for a given plug
	if (bTx == FALSE)
	{
		for (slot = 0; slot < QSEL_SLOTS; slot++)
		{
			index = 0;
			if ((slot < channels) ||							// audio slots
				(slot == channels && avsRxPort[plugID-1].midi))	// MIDI slot
			{
				index = slot + 1;
			}
			hResult = avsSetRxQSelSlot(plugID-1, slot, index);
			if (hResult != NO_ERROR) return hResult;
		}
	}
#endif

	hResult = avsSetDBS(plugID-1, bTx, channels);
	if (hResult != NO_ERROR) return hResult;

	sysDebugPrintf("Isoch %s number of audio channels for plugID: %i is now: %i\n\r", (bTx) ? "Tx" : "Rx", plugID, channels);

	return hResult;
}

static HRESULT avsCliDBS(uint32 plugID, BOOL bTx, uint32 channels)
{
	return avsCliDBSX(plugID, bTx, channels, 0);
}
static HRESULT avsCliQSel(uint32 plugID, char *slotStr)
{
	HRESULT		hResult = NO_ERROR;
	uint32		value[QSEL_SLOTS];
	uint8		slot;
	uint32		res = 0;

	// Set slot indexes for a QSel for a given plug

	res = sscanf(slotStr, "%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x",  
					&value[0], &value[1], &value[2], &value[3], 
					&value[4], &value[5], &value[6], &value[7], 
					&value[8], &value[9], &value[10], &value[11], 
					&value[12], &value[13], &value[14], &value[15], 
					&value[16]);

	if (res != QSEL_SLOTS) return E_BAD_INPUT_PARAMETERS;

	for (slot = 0; slot < QSEL_SLOTS; slot++)
	{
		hResult = avsSetRxQSelSlot(plugID-1, slot, (uint8) value[slot]);
		if (hResult != NO_ERROR) return hResult;
	}

	sysDebugPrintf("QSels for plugID: %i is (%i):\n\r", plugID, res);
	for (slot = 0; slot < QSEL_SLOTS; slot++)
	{
		sysDebugPrintf("%02x ", value[slot]);
	}
	sysDebugPrintf("\n\r");

	return hResult;
}

static HRESULT avsCliMute(uint32 plugID, BOOL bTx, BOOL muteon)
{
	HRESULT		hResult = NO_ERROR;

	hResult = avsSetOnline(plugID-1, bTx, !muteon);
	if (hResult != NO_ERROR) return hResult;

	sysDebugPrintf("Isoch %s mute state for plugID: %i is now: %s\n\r", (bTx) ? "Tx" : "Rx", plugID, (muteon) ? "MUTED" : "UN-MUTED");

	return hResult;
}

static HRESULT avsCliSpeed(uint32 plugID, uint32 speed)
{
	HRESULT		hResult = NO_ERROR;

	hResult = avsSetTxSpeed(plugID-1, speed);
	if (hResult != NO_ERROR) return hResult;

	sysDebugPrintf("Isoch TX speed for plugID: %i is now: %i\n\r", plugID, speed);
	sysDebugPrintf("Restart TX to make changes take effect\n\r");

	return hResult;
}

static char *avsSpeedNames[3] =
{
	"S100",
	"S200",
	"S400"
};

static HRESULT avsCliGetSpeed(uint32 plugID)
{
	HRESULT		hResult = NO_ERROR;
	uint32		speed = 0;

	hResult = avsGetTxSpeed(plugID, &speed);

	sysDebugPrintf("Isoch TX speed for plugID: %i is: %s (%i)\n\r", plugID, speed < 3 ? avsSpeedNames[speed] : "unknown", speed);

	return hResult;
}

static HRESULT avsCliFormat(uint32 plugID, BOOL bTx, uint32 format, uint8 channel)
{
	HRESULT		hResult = NO_ERROR;

	hResult = avsFmtConfig(plugID-1, bTx, channel, (AVS_FORMATS)format);

	sysDebugPrintf("Isoch %s format for plugID: %i on channel: %i is now: %i\n\r", (bTx) ? "Tx" : "Rx", plugID, channel, format);

	return hResult;
}

static HRESULT avsCliITP(BOOL bOn)
{
	HRESULT		hResult = NO_ERROR;
	uint32		itpCfg;
	
	if (bOn)
	{
		itpCfg = ITP_ENABLE | ITP_CLEAR_OFFSET;
		avsRegWrite(ITP_CFG, itpCfg);
	}
	else
	{
		itpCfg = ITP_CLEAR_OFFSET;
		avsRegWrite(ITP_CFG, itpCfg);
	}

	return hResult;
}

/****************************** Midi test functions *******************/

static uint32 midiTxData[32] =
{				
	0x03111001,
	0x23222001,
	0x43333001,
	0x63444001,
	0x83555001,
	0xa3666001,
	0xc3777001, 
	0xe3888001,
	0x03111002,
	0x23222002,
	0x43333002,
	0x63444002,
	0x83555002,
	0xa3666002,
	0xc3777002,
	0xe3888002,
	0x03111003,
	0x23222003,
	0x43333003,
	0x63444003,
	0x83555003,
	0xa3666003,
	0xc3777003,
	0xe3888003,
	0x03111004,
	0x23222004,
	0x43333004,
	0x63444004,
	0x83555004,
	0xa3666004,
	0xc3777004,
	0xe3888004
};

static uint32 midiRxData[APBA_MIDI_QUAD_OUT_SIZE];

static HRESULT avsCliMidi(BOOL bTx, uint32 plugIDTx, BOOL bRx, uint32 plugIDRx, uint32 loops)
{
	HRESULT	hResult = NO_ERROR;
	uint32	read;
	uint32	i;
	uint32	j;
	uint32	cnt;
	uint32	waittime = 1;
	uint32	DBS = 0;

	if (bTx)
	{
		// Enables all TX midiPlugs
		for (i = 0; i < 8; i++)
		{
			hResult = avsSetTxMidiMap(plugIDTx-1, (uint8) i, (uint8) i);
			if (hResult != NO_ERROR) return hResult;
		}

		// Enable midi on transmitter
		hResult = avsSetMidi(plugIDTx-1, TRUE, TRUE);
		if (hResult != NO_ERROR) return hResult;
		// Start isoch. transmitter
		hResult = avsCliIT(plugIDTx, TRUE, TRUE);
		if (hResult != NO_ERROR) return hResult;

		waittime = 50;
	}

	if (bRx)
	{
		// Enables all RX midiPlugs
		for (i = 0; i < 8; i++)
		{
			hResult = avsSetRxMidiMap(plugIDRx-1, (uint8) i, (uint8) i);
			if (hResult != NO_ERROR) return hResult;
		}

		// Enable midi on receiver
		hResult = avsSetMidi(plugIDRx-1, FALSE, TRUE);
		if (hResult != NO_ERROR) return hResult;

		// Set MIDI slot index for QSel
		hResult = avsGetDBS(plugIDRx-1, FALSE, &DBS);
		if (hResult != NO_ERROR) return hResult;

		hResult = avsSetRxQSelSlot(plugIDRx-1, DBS, DBS + 1);
		if (hResult != NO_ERROR) return hResult;

		// Start isoch. receiving
		hResult = avsCliIT(plugIDRx, FALSE, TRUE);
		if (hResult != NO_ERROR) return hResult;
	}

	cnt = 0;
	for (i = 0; i < loops; i++)
	{
		if (bTx)
		{
			hResult = avsWriteMidiData(plugIDTx-1, &midiTxData[(i%8)*4], APBA_MIDI_QUAD_IN_SIZE);
			if (hResult != NO_ERROR) return hResult;
		}
		if (bRx)
		{
			hResult = avsReadMidiData(midiRxData, APBA_MIDI_QUAD_IN_SIZE, &read);
			if (hResult != NO_ERROR) return hResult;
			if (read > 0)
			{
				for (j = 0; j < read; j++)
				{
					sysDebugPrintf("0x%08x ", midiRxData[j]);
					if (++cnt%4 == 0) sysDebugPrintf("\r\n");
				}
			}
		}

		TCTaskWait(waittime); // Wait X ms between each receiving aprox. midi speed
	}

	if (bTx)
	{
		// Stop isoch. transmitter
		hResult = avsCliIT(plugIDTx, TRUE, FALSE);
		if (hResult != NO_ERROR) return hResult;

		// Disable midi on transmitter
		hResult = avsSetMidi(plugIDTx-1, TRUE, FALSE);
		if (hResult != NO_ERROR) return hResult;
	}
	if (bRx)
	{
		// Stop isoch. receiving
		hResult = avsCliIT(plugIDRx, FALSE, FALSE);
		if (hResult != NO_ERROR) return hResult;

		// Disable midi on receiver
		hResult = avsSetMidi(plugIDRx-1, FALSE, FALSE);
		if (hResult != NO_ERROR) return hResult;

		// Clear MIDI slot index for QSel
		hResult = avsSetRxQSelSlot(plugIDRx-1, DBS, 0);
		if (hResult != NO_ERROR) return hResult;
	}

	sysDebugPrintf("Midi testing completed.\n\r");

	return hResult;
}

static HRESULT avsCliSpecify (uint32 plugIDRx, uint32 bSpecifyDBS, uint32 bSpecifyFDF)
{
	return avsSetRxSepcifyFlags (plugIDRx-1, (uint8)bSpecifyDBS, (uint8)bSpecifyFDF);
}

static HRESULT avsCliFSM(BOOL bToggle, uint32 errorClrCnt, uint32 noErrorCnt, uint32 disableCnt)
{
	return avsSetStateMachineParameters (bToggle, (uint8)errorClrCnt, (uint8)noErrorCnt, (uint8)disableCnt);
}

/*********************** Installed AVS CLI functions *****************************/

static  char	avsCliEvent_UsageString[]=\
				"event: \n\r"\
				"use: event <operation>\n\r"\
				"<operations>: dump, dumpifset, clear\n\r"\
				"   dumpifset: dump non-zero counter events\n\r"\
				"------------------------------------------------------------\n\r"\
				"Displays the AVS interrupt events or clear event counters";

static	CLIDescriptor	avsCliEvent_Descriptor[]=
{
	{kCLI_Name,				(uint32)"event"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsCliEvent},
	{kCLI_UsageString, 		(uint32)avsCliEvent_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ConstantsString,	(uint32)"clear=0,dump=1,dumpifset=2,df=3"},
	{kCLI_ModuleName,		(uint32)"AVS events dump and clear"},
	{kCLI_GroupName,		(uint32)"avs"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	avsCliDumpPlug_UsageString[]=\
				"dump: \n\r"\
				"use: dump <plugID> <rx|tx> <all>\n\r"\
				"<plugID>: rx: 1-4, tx: 1-2\n\r"\
				"<all>: 0,1 dump all additional general avs registers\n\r"\
				"------------------------------------------------------------\n\r"\
				"Dump AVS registers";

static	CLIDescriptor	avsCliDumpPlug_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dump"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsCliDumpPlug},
	{kCLI_UsageString, 		(uint32)avsCliDumpPlug_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iii"},
	{kCLI_ConstantsString,	(uint32)"rx=0,tx=1,all=1"},
	{kCLI_ModuleName,		(uint32)"Dump AVS registers"},
	{kCLI_GroupName,		(uint32)"avs"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	avsCliDumpFmt_UsageString[]=\
				"dumpFmt: \n\r"\
				"use: dumpFmt <plugID> <rx|tx> <all>\n\r"\
				"<plugID>: rx: 1-4, tx: 1-2\n\r"\
				"<all>: 0,1 dump all fmt registers (use:1)\n\r"\
				"------------------------------------------------------------\n\r"\
				"Dump AVS format registers receiver Id 1-4 and transmitter Id 1-2";

static	CLIDescriptor	avsCliDumpFmt_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dumpFmt"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsCliDumpFmt},
	{kCLI_UsageString, 		(uint32)avsCliDumpFmt_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iii"},
	{kCLI_ConstantsString,	(uint32)"rx=0,tx=1,all=1"},
	{kCLI_ModuleName,		(uint32)"Dump AVS format registers"},
	{kCLI_GroupName,		(uint32)"avs"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

//	AVS Reset Tool
static  char	avsCliReset_UsageString[]=\
				"reset: \n\r"\
				"use: reset\n\r"\
				"------------------------------------------------------------\n\r"\
				"Causes a reset of all AVS registers";

static	CLIDescriptor	avsCliReset_Descriptor[]=
{
	{kCLI_Name,				(uint32)"reset"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsBoardReset},
	{kCLI_UsageString, 		(uint32)avsCliReset_UsageString},
	{kCLI_ArgumentPatternString, (uint32)""},
	{kCLI_ModuleName,		(uint32)"Reset of AVS registers"},
	{kCLI_GroupName,		(uint32)"avs"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// Iso Test Tool
static  char	avsCliIT_UsageString[]=\
				"it: \n\r"\
				"use: it <plugID> <rx|tx> <start|stop>\n\r"\
				"<plugID>: rx: 1-4, tx: 1-2\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set iso tx|rx test state";

static	CLIDescriptor	avsCliIT_Descriptor[]=
{
	{kCLI_Name,				(uint32)"it"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsCliIT},
	{kCLI_UsageString, 		(uint32)avsCliIT_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iii"},
	{kCLI_ConstantsString,	(uint32)"rx=0,tx=1,stop=0,start=1"},
	{kCLI_ModuleName,		(uint32)"iso test state"},
	{kCLI_GroupName,		(uint32)"avs"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// Iso Test Channel setting tool
static  char	avsCliITC_UsageString[]=\
				"itc: \n\r"\
				"use: itc <plugID> <rx|tx> <ch>\n\r"\
				"<plugID>: rx: 1-4, tx: 1-2\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set iso tx|rx channels";

static	CLIDescriptor	avsCliITC_Descriptor[]=
{
	{kCLI_Name,				(uint32)"itc"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsCliITC},
	{kCLI_UsageString, 		(uint32)avsCliITC_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iii"},
	{kCLI_ConstantsString,	(uint32)"rx=0,tx=1"},
	{kCLI_ModuleName,		(uint32)"iso test channels"},
	{kCLI_GroupName,		(uint32)"avs"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// Iso Test Sampe Rate setting tool
static  char	avsCliITS_UsageString[]=\
				"its: \n\r"\
				"use: its <plugID> <rx|tx> <samplerate>\n\r"\
				"<plugID>: rx: 1-4, tx: 1-2\n\r"\
				"<samplerate>: 32k 44k 48k 88k 96k 176k 192k\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set iso tx|rx channel sample rates";

static	CLIDescriptor	avsCliITS_Descriptor[]=
{
	{kCLI_Name,				(uint32)"its"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsCliITS},
	{kCLI_UsageString, 		(uint32)avsCliITS_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iii"},
	{kCLI_ConstantsString,	(uint32)"rx=0,tx=1,32k=0,44k=1,48k=2,88k=3,96k=4,176k=5,192k=6"},
	{kCLI_ModuleName,		(uint32)"iso test sample rate"},
	{kCLI_GroupName,		(uint32)"avs"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// Iso Test Tx presentation time setting tool
static  char	avsCliTXP_UsageString[]=\
				"txp: \n\r"\
				"use: txp <plugID> <samples> <phase>\n\r"\
				"<plugID>: 1-2\n\r"\
				"<samples>: 0-100\n\r"\
				"<phase>: 0-100\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set iso tx presentation offset";

static	CLIDescriptor	avsCliTXP_Descriptor[]=
{
	{kCLI_Name,				(uint32)"txp"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsCliTxPre},
	{kCLI_UsageString, 		(uint32)avsCliTXP_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iii"},
	{kCLI_ConstantsString,	(uint32)"default=0"},
	{kCLI_ModuleName,		(uint32)"iso test presentation time"},
	{kCLI_GroupName,		(uint32)"avs"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// Iso Test Rx presentation adjust setting tool
static  char	avsCliRXA_UsageString[]=\
				"rxa: \n\r"\
				"use: rxa <plugID> <samples> <phase>\n\r"\
				"<plugID>: 1-2\n\r"\
				"<samples>: -100-100\n\r"\
				"<phase>: 0-100\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set iso rx presentation adjust";

static	CLIDescriptor	avsCliRXA_Descriptor[]=
{
	{kCLI_Name,				(uint32)"rxa"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsCliRxPre},
	{kCLI_UsageString, 		(uint32)avsCliRXA_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iii"},
	{kCLI_ConstantsString,	(uint32)"default=30"},
	{kCLI_ModuleName,		(uint32)"iso test presentation time"},
	{kCLI_GroupName,		(uint32)"avs"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static  char	avsCliDBSX_UsageString[]=\
				"dbsx: \n\r"\
				"use: dbsx <plugID> <tx|rx> <channels> <bMIDI>\n\r"\
				"<plugID>: rx: 1-4, tx: 1-2\n\r"\
				"<channels>: max 16 for tx\n\r"\
				"<bMIDI>: <on|off>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set number of audio channels and MIDI to be streamed for tx|rx";

static	CLIDescriptor	avsCliDBSX_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dbsx"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsCliDBSX},
	{kCLI_UsageString, 		(uint32)avsCliDBSX_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iiik"},
	{kCLI_ConstantsString,	(uint32)"rx=0,tx=1,off=0,on=1"},
	{kCLI_ModuleName,		(uint32)"Set DBS size for plug"},
	{kCLI_GroupName,		(uint32)"avs"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	avsCliDBS_UsageString[]=\
				"dbs: \n\r"\
				"use: dbs <plugID> <tx|rx> <channels>\n\r"\
				"<plugID>: rx: 1-4, tx: 1-2\n\r"\
				"<channels>: max 16 for tx \n\r"\
				"------------------------------------------------------------\n\r"\
				"Set number of audio channels to be streamed for tx|rx";

static	CLIDescriptor	avsCliDBS_Descriptor[]=
{
	{kCLI_Name,				(uint32)"dbs"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsCliDBS},
	{kCLI_UsageString, 		(uint32)avsCliDBS_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iii"},
	{kCLI_ConstantsString,	(uint32)"rx=0,tx=1"},
	{kCLI_ModuleName,		(uint32)"Set DBS size for plug"},
	{kCLI_GroupName,		(uint32)"avs"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


static  char	avsCliQSel_UsageString[]=\
				"qsel: \n\r"\
				"use: qsel <plugID> <\"qselstr\">\n\r"\
				"<plugID>: rx: 1-4\n\r"\
				"<\"qselstr\">: \"xx xx xx xx xx .. xx\"\n\r"\
				"<xx>: byte hexvalue of index value - 17 items\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set QSel slot index values for rx plug";

static	CLIDescriptor	avsCliQSel_Descriptor[]=
{
	{kCLI_Name,				(uint32)"qsel"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsCliQSel},
	{kCLI_UsageString, 		(uint32)avsCliQSel_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"is"},
	{kCLI_ModuleName,		(uint32)"Set QSel slot index values for rx plug"},
	{kCLI_GroupName,		(uint32)"avs"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	avsCliMute_UsageString[]=\
				"mute: \n\r"\
				"use: mute <plugID> <rx|tx> <on|off>\n\r"\
				"<plugID>: rx: 1-4, tx: 1-2\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set mute on or off for tx|rx";

static	CLIDescriptor	avsCliMute_Descriptor[]=
{
	{kCLI_Name,				(uint32)"mute"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsCliMute},
	{kCLI_UsageString, 		(uint32)avsCliMute_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iii"},
	{kCLI_ConstantsString,	(uint32)"rx=0,tx=1,off=0,on=1"},
	{kCLI_ModuleName,		(uint32)"mute/unmute receiver"},
	{kCLI_GroupName,		(uint32)"avs"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	avsCliSpeed_UsageString[]=\
				"speed: \n\r"\
				"use: speed <plugID> <speed>\n\r"\
				"<plugID>: tx: 1-2\n\r"\
				"<speed>: s100,s200,s400\n\r"\
				"------------------------------------------------------------\n\r"\
				"set iso tx speed";

static	CLIDescriptor	avsCliSpeed_Descriptor[]=
{
	{kCLI_Name,				(uint32)"speed"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsCliSpeed},
	{kCLI_UsageString, 		(uint32)avsCliSpeed_UsageString},
	{kCLI_ConstantsString,	(uint32)"s100=0,s200=1,s400=2"},
	{kCLI_ArgumentPatternString, (uint32)"ii"},
	{kCLI_ModuleName,		(uint32)"set iso tx speed"},
	{kCLI_GroupName,		(uint32)"avs"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	avsCliGetSpeed_UsageString[]=\
				"getspeed: \n\r"\
				"use: getspeed\n\r"\
				"use: getspeed <plugID>\n\r"\
				"<plugID>: tx: 1-2\n\r"\
				"------------------------------------------------------------\n\r"\
				"get iso tx speed";

static	CLIDescriptor	avsCliGetSpeed_Descriptor[]=
{
	{kCLI_Name,				(uint32)"getspeed"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsCliGetSpeed},
	{kCLI_UsageString, 		(uint32)avsCliGetSpeed_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ModuleName,		(uint32)"get iso tx speed"},
	{kCLI_GroupName,		(uint32)"avs"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	avsCliFormat_UsageString[]=\
				"format: \n\r"\
				"use: format <plugID> <tx|rx> <format> <channel>\n\r"\
				"<plugID>: rx: 1-4, tx: 1-2\n\r"\
				"<format>: trans, aesMst, aesBSClk, aesSlv, label\n\r"\
				"<channel>: 0-7 (block sync input(tx) or output(rx) channel)\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set AVS format for transmitter or receiver";

static	CLIDescriptor	avsCliFormat_Descriptor[]=
{
	{kCLI_Name,				(uint32)"format"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsCliFormat},
	{kCLI_UsageString, 		(uint32)avsCliFormat_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"ikkI"},
	{kCLI_ConstantsString,	(uint32)"tx=1,rx=0,trans=0,aesMst=1,aesBSClk=2,aesSlv=3,label=5"},
	{kCLI_ModuleName,		(uint32)"Set AVS format configuration"},
	{kCLI_GroupName,		(uint32)"avs"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

// Iso ITP module on or off
static  char	avsCliITP_UsageString[]=\
				"itp: \n\r"\
				"use: itp <on|off>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set ITP module on or off";

static	CLIDescriptor	avsCliITP_Descriptor[]=
{
	{kCLI_Name,				(uint32)"itp"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsCliITP},
	{kCLI_UsageString, 		(uint32)avsCliITP_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"i"},
	{kCLI_ConstantsString,	(uint32)"off=0,on=1"},
	{kCLI_ModuleName,		(uint32)"Set AVS ITP on or off"},
	{kCLI_GroupName,		(uint32)"avs"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	avsCliMIDI_UsageString[]=\
				"midi: \n\r"\
				"use: midi <tx:on|off> <tx:plugID> <rx:on|off> <rx:plugID> <count>\n\r"\
				"<plugID>: rx: 1-4, tx: 1-2\n\r"\
				"midi tx test use: midi on <plugid> off 0 <count>\n\r"\
				"midi rx test use: midi off 0 on <plugid> <count>\n\r"\
				"midi rx+tx test use: midi on <plugid> on <plugid> <count>\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set iso tx|rx MIDI test - count in times of 10ms";

static	CLIDescriptor	avsCliMIDI_Descriptor[]=
{
	{kCLI_Name,				(uint32)"midi"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsCliMidi},
	{kCLI_UsageString, 		(uint32)avsCliMIDI_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iiiii"},
	{kCLI_ConstantsString,	(uint32)"off=0,on=1"},
	{kCLI_ModuleName,		(uint32)"MIDI testing"},
	{kCLI_GroupName,		(uint32)"avs"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};

static  char	avsCliFSM_UsageString[]=\
				"fsm: \n\r"\
				"use: fsm <toggle:on|off> <errClr:cnt> <noErr:cnt> <disable:cnt>\n\r"\
				"<cnt>: 1-255\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set various FSM parameters, counts are in times of 10ms";

static	CLIDescriptor	avsCliFSM_Descriptor[]=
{
	{kCLI_Name,				(uint32)"fsm"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsCliFSM},
	{kCLI_UsageString, 		(uint32)avsCliFSM_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iiii"},
	{kCLI_ConstantsString,	(uint32)"off=0,on=1"},
	{kCLI_ModuleName,		(uint32)"AVS Debug"},
	{kCLI_GroupName,		(uint32)"avs"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};



static  char	avsCliSpecify_UsageString[]=\
				"specify: \n\r"\
				"use: specify <rxID> <DBS> <FMT>\n\r"\
				"<rxID>: 1-4\n\r"\
				"<DBS>: yes/no\n\r"\
				"<FMT>: yes/no\n\r"\
				"------------------------------------------------------------\n\r"\
				"Set receiver specification parameters";

static	CLIDescriptor	avsCliSpecify_Descriptor[]=
{
	{kCLI_Name,				(uint32)"specify"},
	{kCLI_Type, 			kCLI_TYPE_TOOL},
	{kCLI_Function, 		(uint32)&avsCliSpecify},
	{kCLI_UsageString, 		(uint32)avsCliSpecify_UsageString},
	{kCLI_ArgumentPatternString, (uint32)"iii"},
	{kCLI_ConstantsString,	(uint32)"no=0,yes=1"},
	{kCLI_ModuleName,		(uint32)"AVS Debug"},
	{kCLI_GroupName,		(uint32)"avs"},
	{KV_TERMINATEKEY,KV_TERMINATEVALUE}
};


void avsCliInstallTools(void)
{
	HRESULT		hResult = NO_ERROR;

	hResult += cliInstallCLIDescriptor(avsCliEvent_Descriptor);
	hResult += cliInstallCLIDescriptor(avsCliDumpPlug_Descriptor);
	hResult += cliInstallCLIDescriptor(avsCliDumpFmt_Descriptor);

	hResult += cliInstallCLIDescriptor(avsCliReset_Descriptor);

	hResult += cliInstallCLIDescriptor(avsCliIT_Descriptor);
	hResult += cliInstallCLIDescriptor(avsCliITC_Descriptor);
	hResult += cliInstallCLIDescriptor(avsCliITS_Descriptor);
	hResult += cliInstallCLIDescriptor(avsCliDBS_Descriptor);
	hResult += cliInstallCLIDescriptor(avsCliDBSX_Descriptor);
	hResult += cliInstallCLIDescriptor(avsCliQSel_Descriptor);
	
	hResult += cliInstallCLIDescriptor(avsCliMute_Descriptor);
	hResult += cliInstallCLIDescriptor(avsCliSpeed_Descriptor);
	hResult += cliInstallCLIDescriptor(avsCliGetSpeed_Descriptor);
	hResult += cliInstallCLIDescriptor(avsCliFormat_Descriptor);

	hResult += cliInstallCLIDescriptor(avsCliITP_Descriptor);
	
	hResult += cliInstallCLIDescriptor(avsCliMIDI_Descriptor);
	hResult += cliInstallCLIDescriptor(avsCliTXP_Descriptor);
	hResult += cliInstallCLIDescriptor(avsCliRXA_Descriptor);
	hResult += cliInstallCLIDescriptor(avsCliFSM_Descriptor);
	hResult += cliInstallCLIDescriptor(avsCliSpecify_Descriptor);
	
	if (hResult != NO_ERROR)
	{
		cliOutputLine("error installing avs cli tools");
	}
}

#endif //_CLI_TOOL_AVS
#endif //_CLI
#ifdef _WINMP
#pragma warning( disable : 4206 )
#endif //_WINMP
