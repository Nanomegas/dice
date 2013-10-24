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
#include "avs.h"
#include "avsCli.h"
#include "avsDefs.h"
#include "avsInt.h"
#include "avsMidi.h"
#ifdef _DICE2
#include "llcReg.h"
#endif //_DICE2
#ifdef _DICE2_WIN
#include "avsPC.h"
#endif //_DICE2_WIN

static char 	moduleName[] = __FILE__ ;

typedef struct tPARTITION
{
	BOOL		inUse;			// is this Media partition in use
	BOOL		isM6;			// is this Media M6 capable
	uint32		type;			// type of partition (e.g. ARX, ATX,...)
	uint32		base;			// base value
	uint32		limit;			// limit value
	uint32		baseOffset;		// Address of base partition register
	uint32		limitOffset;	// Address of limit partition register
	uint32		flushOffset;	// Address of flush partition register
} PARTITION;

// Media Partition FIFOs
static PARTITION partition[NUM_PARTITIONS] =
{
#if (NUM_PARTITIONS == 6)

#ifdef _SPECIAL_PARTITION_MEMORY
	{ FALSE, TRUE,  ATX, 0x000, 0x32f, APBA_PARTITION0_BASE, APBA_PARTITION0_LIMIT, APBA_PARTITION0_FLUSH},		// Partition 1 (M6)
	{ FALSE, TRUE,  ARX, 0x330, 0x7ff, APBA_PARTITION1_BASE, APBA_PARTITION1_LIMIT, APBA_PARTITION1_FLUSH},		// Partition 2 (M6)
	{ FALSE, TRUE,  ARX, 0x800, 0xcce, APBA_PARTITION2_BASE, APBA_PARTITION2_LIMIT, APBA_PARTITION2_FLUSH},		// Partition 3 (M6)
	{ FALSE, FALSE, ATX, 0xccf, 0xffe, APBA_PARTITION3_BASE, APBA_PARTITION3_LIMIT, APBA_PARTITION3_FLUSH},		// Partition 4
	{ FALSE, FALSE, ARX, 0xffe, 0xffe, APBA_PARTITION4_BASE, APBA_PARTITION4_LIMIT, APBA_PARTITION4_FLUSH},		// Partition 5
	{ FALSE, FALSE, ARX, 0xffe, 0xffe, APBA_PARTITION5_BASE, APBA_PARTITION5_LIMIT, APBA_PARTITION5_FLUSH},		// Partition 6
#else  //_SPECIAL_PARTITION_MEMORY, for full AVS usuage, 2 TX and 4 RX
	{ FALSE, TRUE,  ATX, 0x000, 0x32f, APBA_PARTITION0_BASE, APBA_PARTITION0_LIMIT, APBA_PARTITION0_FLUSH},		// Partition 1 (M6)
	{ FALSE, TRUE,  ARX, 0x330, 0x76f, APBA_PARTITION1_BASE, APBA_PARTITION1_LIMIT, APBA_PARTITION1_FLUSH},		// Partition 2 (M6)
	{ FALSE, TRUE,  ARX, 0x770, 0xbaf, APBA_PARTITION2_BASE, APBA_PARTITION2_LIMIT, APBA_PARTITION2_FLUSH},		// Partition 3 (M6)
	{ FALSE, FALSE, ATX, 0xbb0, 0xedf, APBA_PARTITION3_BASE, APBA_PARTITION3_LIMIT, APBA_PARTITION3_FLUSH},		// Partition 4
	{ FALSE, FALSE, ARX, 0xee0, 0xf6f, APBA_PARTITION4_BASE, APBA_PARTITION4_LIMIT, APBA_PARTITION4_FLUSH},		// Partition 5
	{ FALSE, FALSE, ARX, 0xf70, 0xfff, APBA_PARTITION5_BASE, APBA_PARTITION5_LIMIT, APBA_PARTITION5_FLUSH},		// Partition 6
#endif // _SPECIAL_PARTITION_MEMORY

#elif (NUM_PARTITIONS == 1)
#error "implementation for partitions is not correct avsGetFreePartition need to allow for type AXX"
	{ FALSE, TRUE,  AXX, 0x000, 0x7ff, APBA_PARTITION0_BASE, APBA_PARTITION0_LIMIT, APBA_PARTITION0_FLUSH},		// Partition 1 (M6)
#else
#error "implementation for partitions is not correct"
	{ FALSE, FALSE, AXX, 0x000, 0x000, APBA_PARTITION6_BASE, APBA_PARTITION6_LIMIT, APBA_PARTITION6_FLUSH},		// Partition 7
	{ FALSE, FALSE, AXX, 0x000, 0x000, APBA_PARTITION7_BASE, APBA_PARTITION7_LIMIT, APBA_PARTITION7_FLUSH},		// Partition 8
#endif // NUM_PARTITIONS
};

AVSPORT avsRxPort[NUM_AVSPORTS];
AVSPORT avsTxPort[NUM_AVSPORTS];


static void avsInitHardware (void)
{
	uint16		i;
	
	avsEnvConfigRegWrite(PDB_IRX_RUN | PDB_ITX_RUN | PDB_AVS_RUN);
	
	llcLinkRegWrite(ISO_TX_ENB_REG_1_DP, 0);
	llcLinkRegWrite(ISO_TX_ENB_REG_2_DP, 0);
	llcLinkRegWrite(ISO_RX_ENB_REG_1_DP, 0);
	llcLinkRegWrite(ISO_RX_ENB_REG_2_DP, 0);	
	
	// Configure MEDIA FIFO buffers 
	for (i = 0; i < NUM_PARTITIONS; i++)
	{
		avsRegWrite(partition[i].baseOffset, partition[i].base);
		avsRegWrite(partition[i].limitOffset, partition[i].limit);
		avsRegWrite(partition[i].flushOffset,  1);
	}

}

// Reset and configurte the AVS system
static void avsReset(void)
{
	uint16		i;
	uint8		j;

	// Enable AVS system rx and tx
	avsEnvConfigRegWrite(PDB_IRX_RUN | PDB_ITX_RUN | PDB_AVS_RUN);

	for (i = 0; i < NUM_PARTITIONS; i++)
	{
		partition[i].inUse = FALSE;
	}

	for (i = 0; i < NUM_AVSPORTS; i++)
	{
		// Setting default values for Rx ports
		avsRxPort[i].type = EMPTY_PORT;
		avsRxPort[i].partition = NO_PARTITION;
		avsRxPort[i].channel = 1;
		
		// Basic format for AM824 audio data
		avsRxPort[i].FMT = CIP1_FMT_AUDIO_MUSIC; 
		avsRxPort[i].FDF = 0x00;		
		avsRxPort[i].SFC = AVS_SFC_44_1_KHZ;

		avsRxPort[i].DBS = DBS_STEREO;
		// MIDI disabled
		avsRxPort[i].midi = 0;

		avsRxPort[i].QSelSlot[0] = 0x01;
		avsRxPort[i].QSelSlot[1] = 0x02;
		for (j = 2; j < QSEL_SLOTS; j++)
		{
			avsRxPort[i].QSelSlot[j] = 0;
		}

		avsRxPort[i].chiperCfg = 0;		
		avsRxPort[i].keyParity = 0;
		avsRxPort[i].EMI = 0;
		
		avsRxPort[i].sourceID = BROADCAST_NODE_ID;
		avsRxPort[i].state = AVS_STATE_DISABLED;

		avsRxPort[i].presentOffSmp = 0;
		avsRxPort[i].presentationAdj = 0;

		// Setting default values for Tx ports
		avsTxPort[i].type = EMPTY_PORT;
		avsTxPort[i].partition = NO_PARTITION;
		avsTxPort[i].channel = 1;
		avsTxPort[i].ATXSpeed = SPEED_200;

		// Basic format for AM824 audio data
		avsTxPort[i].FMT = CIP1_FMT_AUDIO_MUSIC; 
		avsTxPort[i].FDF = 0x00;
		avsTxPort[i].SFC = AVS_SFC_44_1_KHZ;
		avsTxPort[i].DBS = DBS_STEREO;

		// MIDI disabled
		avsTxPort[i].midi = 0;

        avsTxPort[i].state = AVS_STATE_DISABLED;
		avsTxPort[i].sourceID = BROADCAST_NODE_ID;

		avsTxPort[i].chiperCfg = 0;		
		avsTxPort[i].keyParity = 0;
		avsTxPort[i].EMI = 0;
		avsTxPort[i].presentOffSmp = ATX_TSTAMPSMP_DEFAULT;
		avsTxPort[i].presentationAdj = 0;

	}

	// Configurate Audio receive ports = 0-3
	avsRxPort[AVS_PLUG_ID1].type = AUDIO_RXDO_PORT;
	avsRxPort[AVS_PLUG_ID1].baseAddr = APBA_ARXDO1_BASE;
	avsRxPort[AVS_PLUG_ID2].type = AUDIO_RXDO_PORT;
	avsRxPort[AVS_PLUG_ID2].baseAddr = APBA_ARXDO2_BASE;
	avsRxPort[AVS_PLUG_ID3].type = AUDIO_RXDO_PORT;
	avsRxPort[AVS_PLUG_ID3].baseAddr = APBA_ARXDO3_BASE;
	avsRxPort[AVS_PLUG_ID4].type = AUDIO_RXDO_PORT;
	avsRxPort[AVS_PLUG_ID4].baseAddr = APBA_ARXDO4_BASE;

	// Configurate Video receive ports = 4
	avsRxPort[AVS_PLUG_ID5].type = VIDEO_RXDO_PORT;
	avsRxPort[AVS_PLUG_ID5].baseAddr = APBA_VRXDO_BASE;

	// Configurate Audio transmit ports = 0-1
	avsTxPort[AVS_PLUG_ID1].type = AUDIO_TXDI_PORT;
	avsTxPort[AVS_PLUG_ID1].baseAddr = APBA_ATXDI1_BASE;
	avsTxPort[AVS_PLUG_ID2].type = AUDIO_TXDI_PORT;
	avsTxPort[AVS_PLUG_ID2].baseAddr = APBA_ATXDI2_BASE;

	// Configurate Video transmit ports = 2
	avsTxPort[AVS_PLUG_ID3].type = VIDEO_TXDI_PORT;
	avsTxPort[AVS_PLUG_ID3].baseAddr = APBA_VTXDI_BASE;

	avsInitHardware();
	// Straight mapping for all MIDI plugs
	for (i = 0; i < NUM_AVS_TX; i++)
	{
		for (j = 0; j < 8; j++)
		{
			avsSetTxMidiMap(i, (uint8) j, (uint8) j);
		}
	}

	for (i = 0; i < NUM_AVS_RX; i++)
	{
		for (j = 0; j < 8; j++)
		{
			avsSetRxMidiMap(i, (uint8) j, (uint8) j);
		}
	}

	//Enable ITP block
	//avsRegWrite(ITP_CFG, ITP_ENABLE);
}

HRESULT	avsInitialize(void)
{
	HRESULT hResult;
	
	avsReset();
	
	hResult = avsIntInitialize();
	if (hResult != NO_ERROR) return hResult;

#ifdef _CLI_TOOL_AVS
	avsCliInstallTools();
	avsDTCPCliInstallTools();
#endif //_CLI_TOOL_AVS

	return hResult;
}

/*********************************************************
	Resume tasks for this module
*/
HRESULT avsResumeTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avs interrupt handler threads
	hResult = avsIntResumeTasks();
	if (hResult != NO_ERROR) return hResult;
	
	return hResult;
}

/*********************************************************
	Suspend tasks for this module
*/
HRESULT avsSuspendTasks(void)
{
	HRESULT hResult = NO_ERROR;

	// avs interrupt handler threads
	hResult = avsIntSuspendTasks();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}

HRESULT avsInitializeTasks(void)
{	
	HRESULT hResult = NO_ERROR;
	
	// avs interrupt handler threads
	hResult = avsIntInitializeTasks();
	if (hResult != NO_ERROR) return hResult;

	return hResult;
}



/********************************************************************************/
// Helper functions to implement simple calculations of presentation time offset
/********************************************************************************/
static uint32 ticksToCycleTime (uint32 ticks)
{
	//calculate a cycle timer number in the 7:13:12 format
	uint32 cycles;
	uint32 seconds;
	
	cycles = ticks / 3072;
	ticks -= cycles * 3072;
	seconds = cycles / 8000;
	cycles -= seconds * 8000;
	seconds &= 0x7f;
	
	return (seconds << 25) | (cycles << 12) | ticks;
}


static uint32 samplesToTicks (uint8 samples, uint8 adjust, uint32 rate)
{
	//calculate the number of tics for a given number of samples.
	//adjust is 0..99% fine adjustment
	//this will only work for numbers up to 176 samples
	
	return ((uint32)samples*24576000 + (uint32)adjust*245760)/rate;
}

static uint32 samplesToCycleTime (uint8 samples, uint8 adjust, uint8 SFC)
{
	//calculate the number of tics for a given number of samples.
	//adjust is 0..99% fine adjustment
	//this will only work for numbers up to 176 samples
	//NOTE: Samples are given as base rate samples
    uint32 rate;
    
    switch (SFC)
    {
		case AVS_SFC_32_KHZ:    rate = 32000; break;
		case AVS_SFC_44_1_KHZ:  rate = 44100; break;
		default:
		case AVS_SFC_48_KHZ:    rate = 48000; break;
		case AVS_SFC_88_2_KHZ:  rate = 44100; break;
		case AVS_SFC_96_KHZ:    rate = 48000; break;
		case AVS_SFC_176_4_KHZ: rate = 44100; break;
		case AVS_SFC_192_KHZ:   rate = 48000; break;
	}
	return ticksToCycleTime (samplesToTicks (samples, adjust, rate));
}

static uint32 samplesRxOffset (int8 samples, uint8 adjust, uint8 SFC)
{
	uint8 bNeg;
	uint32 ofs;
	
	bNeg = samples < 0;
	if (bNeg) samples = -samples;
	ofs = samplesToCycleTime (samples, adjust, SFC);
	ofs &= 0x7fff;
	if (bNeg) ofs |= 0x8000;
	
	return ofs;
}
	
	
/********************************************************************************/
// MEDIA FIFO Section
/********************************************************************************/
		

/***************************************************************************/
// Returns free partion that is M6 capable if TRUE
/***************************************************************************/
static HRESULT avsGetFreePartition(BOOL M6, uint8 type, uint8 *part) 
{
	HRESULT	hResult = NO_ERROR;
	uint32	i;

#if 1 //LM??? until M6 is correctly added as argument to function (in plug structure)
	UNUSED_ARG(M6);

	// Search for a free partion no matter if it is M6 capable
	for (i = 0; i < NUM_PARTITIONS; i++)
	{
		if ((partition[i].inUse == FALSE) &&
			(partition[i].type == type))
		{
			partition[i].inUse = TRUE;
			*part = (uint8) i;
			return hResult;
		}
	}
#else
	// Search for a free partion that is same type of M6 capable
	for (i = 0; i < NUM_PARTITIONS; i++)
	{
		if ((partition[i].inUse == FALSE) &&
			(partition[i].type == type) &&
			(partition[i].isM6 == M6))
		{
			partition[i].inUse = TRUE;
			*part = (uint8) i;
			return hResult;
		}
	}
	// Search for a free partion no matter if it is M6 capable
	for (i = 0; i < NUM_PARTITIONS; i++)
	{
		if ((partition[i].inUse == FALSE) &&
			(partition[i].type == type) &&
			(M6 == FALSE))
		{
			partition[i].inUse = TRUE;
			*part = (uint8) i;
			return hResult;
		}
	}
#endif

	hResult = E_AVS_NO_PARTIONS_FREE;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}




/********************************************************************************/
// LLC Isoc Stream Section
/********************************************************************************/


static void avsSetLLCIsoRxCh(uint32 channel, BOOL bEnable)
{
	uint32	isoRxChEnable;
	uint32  isoRxChEnbReg;
	uint32	isoRxChBit = 1;

	// Enable or disable LLC to receive packets on isoch. channel 0 - 63
	if (channel < 32)
	{
		isoRxChBit = isoRxChBit << channel;
		isoRxChEnbReg = ISO_RX_ENB_REG_1_DP; 	
	}
	else
	{
		isoRxChBit = isoRxChBit << (channel - 32);
		isoRxChEnbReg = ISO_RX_ENB_REG_2_DP; 	
	}

	llcLinkRegRead(isoRxChEnbReg, &isoRxChEnable);
	if (bEnable)
	{
		isoRxChEnable |= isoRxChBit;
	}
	else
	{
		isoRxChEnable &= ~isoRxChBit;
	}
	
	llcLinkRegWrite(isoRxChEnbReg, isoRxChEnable);
}

static void avsSetLLCIsoTxCh(uint32 channel, BOOL bEnable)
{
	uint32	isoTxChEnable;
	uint32  isoTxChEnbReg;
	uint32	isoTxChBit = 1;

	// Enable or disable LLC to receive packets on isoch. channel 0 - 63
	if (channel < 32)
	{
		isoTxChBit = isoTxChBit << channel;
		isoTxChEnbReg = ISO_TX_ENB_REG_1_DP; 	
	}
	else
	{
		isoTxChBit = isoTxChBit << (channel - 32);
		isoTxChEnbReg = ISO_TX_ENB_REG_2_DP; 	
	}

	llcLinkRegRead(isoTxChEnbReg, &isoTxChEnable);
	if (bEnable)
	{
		isoTxChEnable |= isoTxChBit;
	}
	else
	{
		isoTxChEnable &= ~isoTxChBit;
	}
	
	llcLinkRegWrite(isoTxChEnbReg, isoTxChEnable);
}


/****************************************************************************
* DESCRIPTION: AVS General helper functions

	
****************************************************************************/

// Returns CIP1 header format base on avsPort setting
static void	avsGetCIP1Format(AVSPORT *avsPort, uint32 *format)
{
	*format = 0x80000000; 
	*format |= ATX_FDF_MASK & ((avsPort->FDF | avsPort->SFC) << ATX_FDF_SHIFT);
	*format |= ATX_FMT_MASK & (avsPort->FMT << ATX_FMT_SHIFT);
}

// Update avsPort setting base on CIP1 header format
static HRESULT avsSetCIP1Format(AVSPORT *avsPort, uint32 format)
{
	HRESULT hResult = NO_ERROR;

	// Testing that FMT is correct
	if (avsPort->FMT != ((format & ATX_FMT_MASK) >> ATX_FMT_SHIFT))
	{
		hResult = E_AVS_INVALID_FORMAT;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// Extract FDF and SFC part from CIP1 header format
	avsPort->FDF = (uint8) ((format & ATX_FDF_MASK) >> ATX_FDF_SHIFT);
	avsPort->SFC = (uint8) (avsPort->FDF & ATX_SFC_MASK);
	avsPort->FDF &= ~ATX_SFC_MASK; // Clear SFC part of FDF
	
	if (avsPort->SFC > AVS_SFC_192_KHZ)
	{
		hResult = E_AVS_INVALID_FORMAT;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	return hResult;
}


/****************************************************************************
* DESCRIPTION: AVS Audio Transmit section

	
****************************************************************************/

// set or clear Cfg TXDI enable bit
static void avsTxEnable (uint32 plugID, uint8 bEnable)
{
	uint32 ATXDIBase;
	uint32 ATXCfg;
    
    ATXDIBase = avsTxPort[plugID].baseAddr;
    
    // Enable receiver
	avsRegRead(ATXDIBase + ATX_CFG, &ATXCfg);
	if (bEnable)
	{
	    ATXCfg |= ATX_TXDI_ENABLE;
	}
	else
	{
	    ATXCfg &= ~ATX_TXDI_ENABLE;
	}
	avsRegWrite(ATXDIBase + ATX_CFG, ATXCfg); 
}


// updates Tx CFG, preserve odd even bit setting for encryption engine and the TXDI ENABLE bit
// Depends on: DBS, midi, SFC
static void avsTxUpdateCFG (uint32 plugID)
{
	uint32 ATXDIBase;
	uint32 ATXCfg;
	uint32 DBS;

	ATXDIBase = avsTxPort[plugID].baseAddr;

	DBS = avsTxPort[plugID].DBS;
	if (avsTxPort[plugID].midi)	DBS++;

	// Setting transmitter enable, configuration, Media FIFO partition and DBS
	avsRegRead(ATXDIBase + ATX_CFG, &ATXCfg);
	ATXCfg &= (ATX_ODD_EVEN_INSERT & ATX_TXDI_ENABLE); // Preserve some settings
	ATXCfg |= ATX_CIP_TIMESTAMP; 
	ATXCfg |= ATX_GLB_DBS_MASK & (DBS << ATX_GLB_DBS_SHIFT);
	if (avsTxPort[plugID].SFC > AVS_SFC_48_KHZ)
	{
		// Enable SYT inteval 16 for high rate (96 kHz)
		ATXCfg |= ATX_SYT_INTERVAL_16;
		if (avsTxPort[plugID].SFC > AVS_SFC_96_KHZ)
		{
			// Enable "cheating" SYT interval 32 for high rate (192 kHz)
			ATXCfg |= ATX_SYT_INTERVAL_32;
		}
	}
	ATXCfg |= ATX_PARTITION_SEL_MASK & avsTxPort[plugID].partition;
	avsRegWrite(ATXDIBase + ATX_CFG, ATXCfg); 
}

// updates Tx TSTAMP
// Depends on: presentOffSmp, presentationAdj, SFC
static void avsTxUpdateTSTAMP (uint32 plugID)
{
	uint32		presntOffset;
	// Setting default timestamp adjust value
	presntOffset = samplesToCycleTime (avsTxPort[plugID].presentOffSmp, avsTxPort[plugID].presentationAdj, avsTxPort[plugID].SFC) & 0xffff;
	avsRegWrite(avsTxPort[plugID].baseAddr + ATX_TSTAMP, presntOffset); 
}


// updates Tx PHDR
// Depends on: ATXSpeed, channel
static void avsTxUpdatePHDR (uint32 plugID)
{
	uint32 ATXDIBase;
	uint32 ATXPhdr;


	ATXDIBase = avsTxPort[plugID].baseAddr;

	// Setting speed, channel and TCODE in isoch. packet header
	ATXPhdr = ATX_SPEED_MASK & (avsTxPort[plugID].ATXSpeed << ATX_SPEED_SHIFT);
	ATXPhdr |= ATX_TAG_MASK & (ISO_HDR_TAG_CIP << ATX_TAG_SHIFT);
	ATXPhdr |= ATX_CH_MASK & (avsTxPort[plugID].channel << ATX_CH_SHIFT);
	ATXPhdr |= ATX_TCODE_MASK & (TCODE_ISOBLOCK << ATX_TCODE_SHIFT);

	avsDTCPSetATXPhdr(&avsTxPort[plugID], &ATXPhdr);
	
	avsRegWrite(ATXDIBase + ATX_PHDR, ATXPhdr); 
}

// updates Tx CIP0
// Depends on: sourceID, DBS, midi, SFC 
static void avsTxUpdateCIP0 (uint32 plugID)
{
	uint32		ATXDIBase;
	uint32		ATXCIP0;
	uint32		DBS;

	// Setting SID and DBS in CIP0 header

	ATXDIBase = avsTxPort[plugID].baseAddr;

	ATXCIP0 = CIP0_ID;
	ATXCIP0 |= ATX_SID_MASK & (avsTxPort[plugID].sourceID << ATX_SID_SHIFT);

	// Calculating the data block size, it midi enabled 
	// then an extra quadlet is added
	DBS = avsTxPort[plugID].DBS;
	if (avsTxPort[plugID].midi)	DBS++;
/*
	if (avsTxPort[plugID].SFC > AVS_SFC_96_KHZ)
	{
		// "Cheating" SYT interval 32 setting CIP0 = DBS/2
		ATXCIP0 |= ATX_DBS_MASK & ((DBS/2) << ATX_DBS_SHIFT);
	}
	else
	{
		ATXCIP0 |= ATX_DBS_MASK & (DBS << ATX_DBS_SHIFT);
	}
	*/
	ATXCIP0 |= ATX_DBS_MASK & (DBS << ATX_DBS_SHIFT);

	avsRegWrite(ATXDIBase + ATX_CIP0, ATXCIP0); 

}

// updates Tx CIP1
// Depends on: FDF, SFC, FMT
static void avsTxUpdateCIP1 (uint32 plugID)
{
	uint32		ATXCIP1;
	
	avsGetCIP1Format(&avsTxPort[plugID], &ATXCIP1);
	avsRegWrite(avsTxPort[plugID].baseAddr + ATX_CIP1, ATXCIP1); 
}

// updates Tx ADI_CFG
// Depends on: mute, midi
static void avsTxUpdateADI_CFG (uint32 plugID)
{
	uint32		ADICfg = 0;

	if (!avsTxPort[plugID].mute) ADICfg |= ADI_UNMUTE;
	if (avsTxPort[plugID].midi) ADICfg |= ADI_MIDI_ENABLE;
	avsRegWrite(avsTxPort[plugID].baseAddr + ADI_CFG, ADICfg); 
}




static HRESULT	avsStartAudioTx(uint32 plugID)
{
	HRESULT		hResult = NO_ERROR;
	uint32		partIdx;
	uint32		nodeAddr;

	if (plugID >= NUM_AVS_TX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (avsTxPort[plugID].partition == NO_PARTITION)
	{
		// Get free partition of Media FIFO
		hResult = avsGetFreePartition(TRUE, avsTxPort[plugID].type, &avsTxPort[plugID].partition);
		if (hResult != NO_ERROR) return hResult;
	}


	// Flush Media FIFO partition used for this transmitter
	partIdx = avsTxPort[plugID].partition;
	avsRegWrite(partition[partIdx].flushOffset, 1);


	//set the correct source ID
	hResult = lalGetThisNodeAddr(&nodeAddr);
	if (hResult != NO_ERROR) return hResult;
	avsTxPort[plugID].sourceID = nodeAddr & MASK_NODEID;


	// Set Cipher configuration
	avsDTCPSetCipherCfg(&avsTxPort[plugID]);
	avsTxUpdateTSTAMP (plugID);
	avsTxUpdatePHDR   (plugID);


	// Setting SID and DBS in CIP0 header
	avsTxUpdateCIP0(plugID);
	avsTxUpdateCIP1 (plugID);
	
	avsTxUpdateADI_CFG (plugID);
	avsTxUpdateCFG (plugID);
	avsTxEnable (plugID, TRUE);
    avsTxPort[plugID].state = AVS_STATE_LOCKED;
	return hResult;
}


HRESULT	avsStartTx(uint32 plugID)
{
	HRESULT hResult = NO_ERROR;

	if (plugID >= NUM_AVS_TX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// Stop Tx if already started
	hResult = avsStopTx(plugID);
	if (hResult != NO_ERROR) return hResult;

#ifdef _DICE2_WIN
	avsPCStartTx(plugID);
#endif //_DICE2_WIN

	if (avsTxPort[plugID].type == AUDIO_TXDI_PORT)
	{
		hResult = avsStartAudioTx(plugID);
	}
	else if (avsTxPort[plugID].type == VIDEO_TXDI_PORT)
	{
		hResult = E_NOT_IMPLEMENTED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	else
	{
		hResult = E_AVS_INVALID_PLUGID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (hResult == NO_ERROR)
	{
		// Enable LLC to transmit packets on isoch. channel 0 - 63
		avsSetLLCIsoTxCh(avsTxPort[plugID].channel, TRUE);
	}

	return hResult;
}

HRESULT	avsStopTx(uint32 plugID)
{
	HRESULT	hResult = NO_ERROR;

	if (plugID >= NUM_AVS_TX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

#ifdef _DICE2_WIN
	avsPCStopTx(plugID);
#endif //_DICE2_WIN

	if (avsIsTxStarted(plugID) == FALSE) return hResult;

	if (avsTxPort[plugID].type == AUDIO_TXDI_PORT)
	{
		avsTxEnable (plugID, FALSE);        
		avsTxPort[plugID].state = AVS_STATE_DISABLED;

		avsDTCPSetTxEncryption(plugID, 0, 0);
	}
	else if (avsTxPort[plugID].type == VIDEO_TXDI_PORT)
	{
		hResult = E_NOT_IMPLEMENTED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	else
	{
		hResult = E_AVS_INVALID_PLUGID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// Disable LLC to transmit packets on isoch. channel 0 - 63
	avsSetLLCIsoTxCh(avsTxPort[plugID].channel, FALSE);
	return hResult;
}

BOOL avsIsTxStarted(uint32 plugID)
{
	if (plugID >= NUM_AVS_TX)
	{
		HRESULT	hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return FALSE;
	}

	return (avsTxPort[plugID].state != AVS_STATE_DISABLED);
}

HRESULT	avsSetTxCh(uint32 plugID, uint8 channel)
{
	HRESULT	hResult = NO_ERROR;

	if (plugID >= NUM_AVS_TX ||
		avsTxPort[plugID].type == EMPTY_PORT)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (channel >= 64)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	avsTxPort[plugID].channel = channel;
	
	// Restart to make effective
	if (avsIsTxStarted(plugID))	avsStartTx(plugID);
	return hResult;
}


HRESULT	avsGetTxCh(uint32 plugID, uint8 *channel)
{
	HRESULT	hResult = NO_ERROR;

	if (plugID >= NUM_AVS_TX ||
		avsTxPort[plugID].type == EMPTY_PORT)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	
	*channel = avsTxPort[plugID].channel;
	return hResult;
}


// Set format for transmitter (output  plugID), used by CMP
HRESULT	avsSetTxFormat(uint32 plugID, uint32 format)
{
	HRESULT hResult = E_AVS_INVALID_PLUGID;
	
	if (plugID >= NUM_AVS_TX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (avsTxPort[plugID].type == AUDIO_TXDI_PORT)
	{

		hResult = avsSetCIP1Format(&avsTxPort[plugID], format);
		if (hResult != NO_ERROR) return hResult;
		
		// Setting new CIP1 header format
		avsTxUpdateCIP1 (plugID);
		
		// Adjusting SYT interval as SFC might have changed
		avsTxUpdateCFG (plugID);
		avsTxUpdateTSTAMP (plugID);
		avsTxUpdateCIP0 (plugID);
		
		return NO_ERROR;
	}

	if (avsTxPort[plugID].type == VIDEO_TXDI_PORT)
	{
		hResult = E_NOT_IMPLEMENTED;
	}

	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// Get format for transmitter (output  plugID)
HRESULT	avsGetTxFormat(uint32 plugID, uint32 *format)
{
	HRESULT hResult = E_AVS_INVALID_PLUGID;

	if (plugID >= NUM_AVS_TX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if ((avsTxPort[plugID].type == AUDIO_TXDI_PORT) ||
		(avsTxPort[plugID].type == VIDEO_TXDI_PORT))
	{
		avsGetCIP1Format(&avsTxPort[plugID], format);
		return NO_ERROR;
	}
	
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}


HRESULT avsSetTxPresntOffset(uint32 plugID, uint8 samples, uint8 phase)
{
	HRESULT	hResult = NO_ERROR;

	if (plugID >= NUM_AVS_TX ||
		avsTxPort[plugID].type == EMPTY_PORT)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (samples >= 100 || phase >= 100)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	avsTxPort[plugID].presentOffSmp = samples;
	avsTxPort[plugID].presentationAdj = phase;
	// no need to restart
	avsTxUpdateCFG (plugID);
	// Restart to make effective
	//if (avsIsTxStarted(plugID))	avsStartTx(plugID);
	return hResult;
}

HRESULT avsSetTxSpeed(uint32 plugID, uint32 speed)
{
	HRESULT	hResult = NO_ERROR;

	if (plugID >= NUM_AVS_TX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if ((avsTxPort[plugID].type == AUDIO_TXDI_PORT)||
		(avsTxPort[plugID].type == VIDEO_TXDI_PORT))
	{
		avsTxPort[plugID].ATXSpeed = (BYTE) speed;
		return hResult;
		// Restart to make effective
		if (avsIsRxStarted(plugID))	avsStartRx(plugID);
	}

	hResult = E_AVS_INVALID_PLUGID;
	sysLogError(hResult, __LINE__, moduleName);

	return hResult;
}

HRESULT avsGetTxSpeed(uint32 plugID, uint32 *speed)
{
	HRESULT	hResult = NO_ERROR;

	if (plugID >= NUM_AVS_TX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if ((avsTxPort[plugID].type == AUDIO_TXDI_PORT)||
		(avsTxPort[plugID].type == VIDEO_TXDI_PORT))
	{
		*speed = avsTxPort[plugID].ATXSpeed;
		return hResult;
	}

	hResult = E_AVS_INVALID_PLUGID;
	sysLogError(hResult, __LINE__, moduleName);

	return hResult;
}

static const uint32 SYT_INTERVAL[] =
{8, 8, 8, 16, 16, 32, 32};

HRESULT	avsSetTxSampleRate(uint32 plugID, uint8 sfc)
{
	HRESULT hResult = NO_ERROR;
	
	if (plugID >= NUM_AVS_TX)
	{
		hResult = E_AVS_INVALID_PLUGID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if (sfc >= AVS_NUM_SFC)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	//oldSytInt = SYT_INTERVAL[avsTxPort[plugID].SFC];
	//newSytInt = SYT_INTERVAL[sfc];
    //if (oldSyt != newSyt)
    if (avsTxPort[plugID].SFC != sfc)
    {
		//need to restart receiver
		avsTxPort[plugID].SFC = sfc;
		if (avsIsTxStarted(plugID))	avsStartTx(plugID);
	}
	//else
	//{
	//	avsTxPort[plugID].SFC = sfc;
	//	avsTxUpdateTSTAMP(plugID);
	//	avsTxUpdateCIP1 (plugID);
	//}
	return hResult;
}

HRESULT	avsTxMute(uint32 plugID, BOOL muteon)
{
	HRESULT	hResult = NO_ERROR;

	if (plugID >= NUM_AVS_TX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (avsTxPort[plugID].type == AUDIO_TXDI_PORT)
	{
		avsTxPort[plugID].mute = muteon;
		avsTxUpdateADI_CFG (plugID);
		return hResult;
	}


	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

HRESULT	avsSetTxDBS(uint32 plugID, uint32 channels)
{
	HRESULT	hResult = NO_ERROR;
	
	if (plugID >= NUM_AVS_TX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if (channels > DBS_MAX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;	
	}
	if (avsTxPort[plugID].type == AUDIO_TXDI_PORT)
	{
		avsTxPort[plugID].DBS = (uint8) channels;
		// Restart to make effective
		if (avsIsTxStarted(plugID))	avsStartTx(plugID);
		return hResult;
	}
	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

HRESULT	avsGetTxDBS(uint32 plugID, uint32 * channels)
{
	HRESULT	hResult = NO_ERROR;
	
	if (plugID >= NUM_AVS_TX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if (avsTxPort[plugID].type == AUDIO_TXDI_PORT)
	{
		*channels = avsTxPort[plugID].DBS;
		return hResult;
	}
	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

// updating Tx plugs sourceID if changed af bus reset
HRESULT avsTxCheckSourceID(void)
{
	HRESULT		hResult = NO_ERROR;
	uint32		nodeAddr;
	uint32		sourceID;
	uint32		plugID;
	
	// Setting SID and DBS in CIP0 header
	hResult = lalGetThisNodeAddr(&nodeAddr);
	if (hResult != NO_ERROR) return hResult;
	
	sourceID = nodeAddr & MASK_NODEID;

	// check sourceID for all active transmitters
	for (plugID = 0; plugID < NUM_AVS_TX; plugID++)
	{
		if (avsTxPort[plugID].type == AUDIO_TXDI_PORT)
		{
			uint32 ATXDIBase;
			uint32 ATXCfg;
			ATXDIBase = avsTxPort[plugID].baseAddr;
			avsRegRead(ATXDIBase + ATX_CFG, &ATXCfg);

			if ((ATXCfg & ATX_TXDI_ENABLE) == ATX_TXDI_ENABLE)
			{
				if (avsTxPort[plugID].sourceID != sourceID)
				{
					SYS_DEBUG(SYSDEBUG_TRACE_AVS, "avsTxCheckSourceID: sourceID changed for plugID:%i (old:%i, new:%i) \n\r", plugID, avsTxPort[plugID].sourceID, sourceID);
					avsTxPort[plugID].sourceID = sourceID;
					avsTxUpdateCIP0(plugID);
				}
			}
		}
	}

	return hResult;
}

//only used by encryption sub system
HRESULT avsWriteATxPHDR(uint32 plugID)
{
	HRESULT hResult = NO_ERROR;

	if (plugID >= NUM_AVS_TX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
    avsTxUpdatePHDR (plugID);

	return hResult;
}


/****************************************************************************
* DESCRIPTION: AVS Audio Receiver section

	
****************************************************************************/

// set or clear Cfg0 RXDO enable bit
static void avsRxEnable (uint32 plugID, uint8 bEnable)
{
	uint32 ARXCfg0;
	// Enable receiver
	avsRegRead(avsRxPort[plugID].baseAddr + ARX_CFG0,  &ARXCfg0);
	if (bEnable)
	{
	    ARXCfg0 |= ARX_RXDO_ENABLE;
	}
	else
	{
	    ARXCfg0 &= ~ARX_RXDO_ENABLE;
	}
	avsRegWrite(avsRxPort[plugID].baseAddr + ARX_CFG0,  ARXCfg0);
}



// updates Cfg0, it maintains the RXDO enable bit
// Depends on: channel, SFC, 
static void avsRxUpdateCfg0 (uint32 plugID)
{
	uint32		ARXCfg0;
	uint32		ARXDOBase = avsRxPort[plugID].baseAddr;

	avsRegRead(ARXDOBase + ARX_CFG0, &ARXCfg0);	
	ARXCfg0 &= ARX_RXDO_ENABLE; //clear all other bits
	
	ARXCfg0 = ARX_FDF_NO_DATA;
	ARXCfg0 |= ARX_TAG_SPECIFIED_MASK & (ISO_HDR_TAG_CIP << ARX_TAG_SPECIFIED_SHIFT);
	ARXCfg0 |= ARX_PARTITION_SEL_MASK & (avsRxPort[plugID].partition << ARX_PARTITION_SEL_SHIFT);
	ARXCfg0 |= ARX_CHANNEL_ID_MASK & avsRxPort[plugID].channel;

	if (avsRxPort[plugID].SFC > AVS_SFC_96_KHZ)
	{
		// Enable "cheating" SYT interval 32 for high rate (192 kHz)
		ARXCfg0 |= ARX_SYT_INTERVAL_32;
	}
	avsDTCPSetARXCfg0(&ARXCfg0);
	avsRegWrite(ARXDOBase + ARX_CFG0, ARXCfg0);
	
}

// updates Cfg1, no bits are maintained
// Depends on: FMT, SFC, FDF, DBD, midi
static void avsRxUpdateCfg1 (uint32 plugID)
{
	uint32		ARXCfg1;
	uint32		ARXDOBase = avsRxPort[plugID].baseAddr;
	uint32		DBS;
	// Setting DBS and FDF values
	ARXCfg1 = ARX_FMT_SPECIFIED_MASK & (avsRxPort[plugID].FMT << ARX_FMT_SPECIFIED_SHIFT);
	if (avsRxPort[plugID].SFC > AVS_SFC_96_KHZ)
	{
		ARXCfg1 |= ARX_DBS_ENABLE; // Use the specified DBS field since CIP0 header contains (DBS/2)
		// Use the specified FDF field since CIP0 header contains SFC = 192, 176.4 kHz
		// The dice is only running in high rate - 88.2 or 96 kHz
		ARXCfg1 |= ARX_FDF_ENABLE; 
		if (avsRxPort[plugID].SFC == AVS_SFC_176_4_KHZ)
			ARXCfg1 |= ARX_FDF_SPECIFIED_MASK & (avsRxPort[plugID].FDF | AVS_SFC_88_2_KHZ);
		else
			ARXCfg1 |= ARX_FDF_SPECIFIED_MASK & (avsRxPort[plugID].FDF | AVS_SFC_96_KHZ);
	}
	else
	{
		ARXCfg1 |= ARX_FDF_SPECIFIED_MASK & (avsRxPort[plugID].FDF | avsRxPort[plugID].SFC);
		if (avsRxPort[plugID].bSpecifyDBS) ARXCfg1 |= ARX_DBS_ENABLE;
		if (avsRxPort[plugID].bSpecifyFDF) ARXCfg1 |= ARX_FDF_ENABLE;
			
	}
	DBS = avsRxPort[plugID].DBS;
	if (avsRxPort[plugID].midi)	DBS++;
	ARXCfg1 |= ARX_DBS_SPECIFIED_MASK & (DBS << ARX_DBS_SPECIFIED_SHIFT);
	avsRegWrite(ARXDOBase + ARX_CFG1, ARXCfg1);
}

// updates QSel's, no bits are maintained
// Depends on: QSelSlot, presentOffSmp, presentationAdj, SFC
static void avsRxUpdateQSels (uint32 plugID)
{
	uint32		ARXQSel[5];	
	uint32		ARXDOBase = avsRxPort[plugID].baseAddr;
	uint8		i;
	uint32      offset;
	
	for (i = 0; i < 4; i++) 
	{
		ARXQSel[i] = (avsRxPort[plugID].QSelSlot[i*4+0]) |
					 (avsRxPort[plugID].QSelSlot[i*4+1] << 8) |
					 (avsRxPort[plugID].QSelSlot[i*4+2] << 16) |
					 (avsRxPort[plugID].QSelSlot[i*4+3] << 24);
	}
	// Setting timestamp adjust value and MIDI
	offset = samplesRxOffset (avsRxPort[plugID].presentOffSmp, avsRxPort[plugID].presentationAdj, avsRxPort[plugID].SFC);
    
	ARXQSel[4] = offset << 16;
	ARXQSel[4] |= avsRxPort[plugID].QSelSlot[QSEL_SLOTS - 1];


	avsRegWrite(ARXDOBase + ARX_QSEL0, ARXQSel[0]);
	avsRegWrite(ARXDOBase + ARX_QSEL1, ARXQSel[1]);
	avsRegWrite(ARXDOBase + ARX_QSEL2, ARXQSel[2]);
	avsRegWrite(ARXDOBase + ARX_QSEL3, ARXQSel[3]);
	avsRegWrite(ARXDOBase + ARX_QSEL4, ARXQSel[4]);
}

// updates Ado, no bits are maintained
// Depends on: mute, midi
static void avsRxUpdateAdo (uint32 plugID)
{
	uint32		ADOCfg;
	uint32		ARXDOBase = avsRxPort[plugID].baseAddr;

	ADOCfg  = ADO_MUTE_AUTO;
	ADOCfg |= ADO_LOCK_CNT_DEFAULT;
	if (!avsRxPort[plugID].mute)	ADOCfg |= ADO_UNMUTE;
	if (avsRxPort[plugID].midi)	    ADOCfg |= ADO_MIDI_ENABLE;
	avsRegWrite(ARXDOBase + ADO_CFG, ADOCfg); 	
}

/***************************************************************************/
// Functions to implement the AVS Rx State Machine. This FSM handles the
// details of getting to a locked state. 

// ML, Nov 04, 2004
// The Stream Start and End events are not actually reliable in terms of 
// detecting whether the stream is running. Another method of detecting lock
// using a combination of error events and timers has been implemented below.
/***************************************************************************/

static BOOL avsRxToggleEnable = TRUE; //default toggle the ARX enable in case of error events
static uint8 avsRxFSPWaitErrorClrCnt = 1; //give the errors time to settle
static uint8 avsRxFSMConseqNoErrorCnt = 1; //see no errors for some time before going to LOCKED
static uint8 avsRxFSMDisableCnt = 1;  //disable ARX for some time


HRESULT avsSetStateMachineParameters (BOOL bToggle, uint8 errorClrCnt, uint8 noErrorCnt, uint8 disableCnt)
{
	avsRxToggleEnable = bToggle;
	avsRxFSPWaitErrorClrCnt = errorClrCnt;
	avsRxFSMConseqNoErrorCnt = noErrorCnt;
	avsRxFSMDisableCnt = disableCnt;

	return NO_ERROR;
}




// This function gets a combination of CIP0 and CIP1 which are not likely to be the same
// between packages.

static uint32 avsAudioRxSignature (uint32 plugID)
{
    uint32 CIP0, CIP1;
    
    avsRegRead(avsRxPort[plugID].baseAddr + ARX_CIP0, &CIP0);
    avsRegRead(avsRxPort[plugID].baseAddr + ARX_CIP1, &CIP1);
    
    return CIP0 + CIP1;
} 



// Returns TRUE if error and FALSE if no error    
static BOOL avsHandleRxCheckErrorEvent(uint32 plugID)
{
#if 1
	return (avsRxPort[plugID].errorEvent);
#else
	// checking local count against counted events done at interrupt level
#endif
}

static void avsHandleRxClearErrorEvent(uint32 plugID)
{
#if 1
	avsRxPort[plugID].errorEvent = 0;
#else
	// setting local count to counted events done at interrupt level
#endif
}

// Returns TRUE if error and FALSE if no error    
static BOOL avsHandleRxCheckSytEvent(uint32 plugID)
{
#if 1
	return (avsRxPort[plugID].sytEvent);
#else
	// checking local count against counted events done at interrupt level
#endif
}

static void avsHandleRxClearSytEvent(uint32 plugID)
{
#if 1
	avsRxPort[plugID].sytEvent = 0;
#else
	// setting local count to counted events done at interrupt level
#endif
}

static uint32 lastTxTstmp[NUM_AVS_TX];
static uint32 cntTxTstmpChg[NUM_AVS_TX];

static void avsReinitializeAll(void)
{
	int i;
	BOOL rxEnable[NUM_AVS_RX];
	BOOL txEnable[NUM_AVS_TX];

	SYS_DEBUG(SYSDEBUG_TRACE_SERIOUS, "STOCK AVS Tx, system re-init\n\r");
	//figure out who is enabled before we reset and stop them
	for (i=0; i<NUM_AVS_RX; i++)
	{
		rxEnable[i] = avsIsRxStarted(i);
		avsStopRx(i);
	}
	for (i=0; i<NUM_AVS_TX; i++) 
	{
		cntTxTstmpChg[i] = 0;
		txEnable[i] = avsIsTxStarted(i);
		avsStopTx(i);
	}
		
	avsEnvConfigRegWrite(0);
	
	avsInitHardware();
	
	//reinit all rx and tx
	for (i=0; i<NUM_AVS_RX; i++) 
	{
		if (rxEnable[i]) avsStartRx(i);
	}
	for (i=0; i<NUM_AVS_TX; i++) 
	{
		if (txEnable[i]) avsStartTx(i); 
	}
	//we should be good to go again	
}

void  avsTxPlugStateMachine (uint32 plugID, uint8 * pEvent)
{
	uint32 tstmp;
	*pEvent = 0;
	if (plugID >= NUM_AVS_TX)
	{
		sysLogError(E_BAD_INPUT_PARAMETERS, __LINE__, moduleName);
		return;
	}
	if (avsIsTxStarted(plugID))
	{
		avsRegRead(avsTxPort[plugID].baseAddr + ATX_CIP0,  &tstmp);
		if (lastTxTstmp[plugID]!=tstmp)
		{
			cntTxTstmpChg[plugID]=0;
			lastTxTstmp[plugID]=tstmp;
		}
		else
		{
			cntTxTstmpChg[plugID]++;
		}
		if (cntTxTstmpChg[plugID]>100) //~2 seconds
		{
			avsReinitializeAll();
		}
	}
}
    
// This function handles all changes of states for the Rx plugs, 
// except for initialization.
// the state variable can not be changed by other functions
static void avsRxSetState (uint32 plugID, AVS_STATE state)
{

	if (plugID >= NUM_AVS_RX)
	{
		sysLogError(E_BAD_INPUT_PARAMETERS, __LINE__, moduleName);
		return;
	}	
	if (avsRxPort[plugID].state == state) return;
    
    if (avsRxPort[plugID].type != AUDIO_RXDO_PORT) return;
    
    if (avsRxPort[plugID].state == AVS_STATE_DISABLED) 
		avsRxPort[plugID].eventMask |= EVT_AVS_ENABLE;
    
    if (avsRxPort[plugID].state == AVS_STATE_LOCKED) 
		avsRxPort[plugID].eventMask |= EVT_AVS_LOCK_LOST;
    
    switch (state)
    {
	    case AVS_STATE_DISABLED:
	        //Always disable ARX in DISABLED state
	        avsRxEnable (plugID, FALSE);
	        avsRxPort[plugID].eventMask |= EVT_AVS_DISABLE;
	        break;
	        
	    case AVS_STATE_WAIT_FLOW:
	        //Always enable the ARX in this state
	        avsRxEnable (plugID, TRUE);
	        avsRxPort[plugID].stateCount = 3;
	        avsRxPort[plugID].stateVal = avsAudioRxSignature (plugID);
	        break;
	        
	    case AVS_STATE_CLR_EVT:
	        //Always enable the ARX in this state
	        avsRxEnable (plugID, TRUE);
	        //Let's make sure the events get cleared for at least n counts
	        avsHandleRxClearErrorEvent(plugID);
	        avsHandleRxClearSytEvent(plugID);
	        avsRxPort[plugID].stateCount = avsRxFSPWaitErrorClrCnt;
	        break;
	    	        
        case AVS_STATE_CHK_EVT:
            // Let's make sure it stays error free for a couple of counts
	        avsRxPort[plugID].stateCount = avsRxFSMConseqNoErrorCnt;
	        break;
            
            
	    case AVS_STATE_TGL_RX:
	        //We seem to have a bad lock situation, toggle receiver off and on
	        avsRxEnable (plugID, FALSE);
            // Flush the FIFO
         	avsRegWrite(partition[avsRxPort[plugID].partition].flushOffset, 1);

            avsRxPort[plugID].stateCount = avsRxFSMDisableCnt;
            
			SYS_DEBUG(SYSDEBUG_TRACE_AVS, "avsEvent: ARX%i Enter AVS_STATE_TGL_RX\n\r", plugID);
	        break;
	        
	    case AVS_STATE_LOCKED:
	        //In this state we will look for events and changes of CIP1
	        avsRxPort[plugID].stateVal = avsAudioRxSignature (plugID);
	        avsRxPort[plugID].stateCount = 10;
	        avsRxPort[plugID].eventMask |= EVT_AVS_LOCK_ACQUIRED;
	        avsRxPort[plugID].rptCnt = 0;
					avsRxPort[plugID].slpCnt = 0;	        
	        break;
	}
	avsRxPort[plugID].state = state;
}


//This state machine should be driven every 20ms or slower.
void avsRxPlugStateMachine (uint32 plugID, uint8 * pEvent)
{
	avsRxPort[plugID].eventMask = 0;
	if (plugID >= NUM_AVS_RX)
	{
		sysLogError(E_BAD_INPUT_PARAMETERS, __LINE__, moduleName);
		*pEvent = avsRxPort[plugID].eventMask; 
		return;
	}
	switch (avsRxPort[plugID].state)
	{
	    case AVS_STATE_DISABLED:
	        // This state can only be left by a call to avsRxEnablePlug
	        break;
	        
	    case AVS_STATE_WAIT_FLOW:
	        // Wait to see at least count changes of CIP2
	        if (avsRxPort[plugID].stateCount == 0)
	        {
	            avsRxSetState (plugID, AVS_STATE_CLR_EVT);
	        }
	        else
	        {
	            uint32 sig;
	            sig = avsAudioRxSignature (plugID);
	            
	            if (avsRxPort[plugID].stateVal != sig) 
	            {
	                avsRxPort[plugID].stateVal = sig;
	                avsRxPort[plugID].stateCount--;
	            }
	        }
	        break;
	              
	    case AVS_STATE_CLR_EVT:
	        if (avsRxPort[plugID].stateCount == 0)
	        {
	            avsRxSetState (plugID, AVS_STATE_CHK_EVT);
            }
            avsHandleRxClearErrorEvent(plugID);
	        avsHandleRxClearSytEvent(plugID);
            avsRxPort[plugID].stateCount--;
            break;

	    case AVS_STATE_CHK_EVT:
	        if (avsHandleRxCheckErrorEvent(plugID))
	        {
	            if (avsRxToggleEnable)
	            {
					//avsRxEnable (plugID, FALSE);
	                //avsRxEnable (plugID, TRUE);
	                //avsRxSetState (plugID, AVS_STATE_CLR_EVT);
					avsRxSetState (plugID, AVS_STATE_TGL_RX);
				}
				else
				{
					avsRxSetState (plugID, AVS_STATE_CLR_EVT);
				}
	        }
	        else if (avsHandleRxCheckSytEvent(plugID))
	        {
				if (avsRxToggleEnable)
	            {
					avsRegWrite(partition[avsRxPort[plugID].partition].flushOffset, 1);
				}				
				avsRxSetState (plugID, AVS_STATE_CLR_EVT);				
	        }
	        else if (avsRxPort[plugID].stateCount == 0)
	        {
	            avsRxSetState (plugID, AVS_STATE_LOCKED);
	        }
	        else
	        {
	          avsRxPort[plugID].stateCount--;
	        }
	        break;
	        
	    case AVS_STATE_TGL_RX:
	        //The receiver was disabled before entering this state, we will reenable and go back into CLR_EVT
	        if (avsRxPort[plugID].stateCount == 0)
	        {
	            avsRxSetState (plugID, AVS_STATE_CLR_EVT);
	        } 
	        else
	        {
	          avsRxPort[plugID].stateCount--;
	        }
	        break;
	    
	    case AVS_STATE_LOCKED:
	        //Need to monitor if we stay locked, no error and CIP1 changing
	        if (avsHandleRxCheckErrorEvent(plugID) || avsHandleRxCheckSytEvent(plugID))
	        {
	            //we need to unlock and wait for no errors
	            avsRxSetState (plugID, AVS_STATE_CLR_EVT);
	        }
	        else if (avsRxPort[plugID].stateCount == 0)	        
	        {
	            //we have run out of patience, no change of CIP1
	            avsRxSetState (plugID, AVS_STATE_WAIT_FLOW);
	        }
	        else if ((avsRxPort[plugID].rptCnt > 100) && (avsRxPort[plugID].slpCnt > 100))
	        {
				//special case, only error is slip and repeat pairs, we need to toggle the rx on/off
				avsRxSetState (plugID, AVS_STATE_TGL_RX);
	        }
	        else
	        {
	            uint32 sig;
	            
	            avsRxPort[plugID].stateCount--;	            
	            sig = avsAudioRxSignature (plugID);
	            if (avsRxPort[plugID].stateVal != sig) 
	            {
	              //it changed so lets reset the count
	              avsRxPort[plugID].stateCount = 10;
	              avsRxPort[plugID].stateVal = sig;
	            }
	        }
	        break;
	}
	*pEvent = avsRxPort[plugID].eventMask; 
}	    

HRESULT avsHandleRxDbcFail(uint32 plugID)
{
	HRESULT	hResult = NO_ERROR;

	if (plugID >= NUM_AVS_RX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	avsRxPort[plugID].errorEvent=1;
	return hResult;
}

HRESULT avsHandleRxPktAbort(uint32 plugID)
{
	HRESULT	hResult = NO_ERROR;

	if (plugID >= NUM_AVS_RX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	avsRxPort[plugID].errorEvent=1;
	return hResult;
}

HRESULT avsHandleRxSytAgeOut(uint32 plugID)
{
	HRESULT	hResult = NO_ERROR;

	if (plugID >= NUM_AVS_RX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	avsRxPort[plugID].sytEvent=1;
	return hResult;
}

HRESULT		avsHandleRxRepeat(uint32 plugID)
{
	HRESULT	hResult = NO_ERROR;

	if (plugID >= NUM_AVS_RX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	avsRxPort[plugID].rptCnt++;
	return hResult;
}


HRESULT		avsHandleRxSlip(uint32 plugID)
{
	HRESULT	hResult = NO_ERROR;

	if (plugID >= NUM_AVS_RX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	avsRxPort[plugID].slpCnt++;
	return hResult;
}


/***************************************************************************/
// End of state machine functions
/***************************************************************************/



static HRESULT	avsStartAudioRx(uint32 plugID)
{
	HRESULT		hResult = NO_ERROR;
	uint32		partIdx;	

	if (plugID >= NUM_AVS_RX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (avsRxPort[plugID].partition == NO_PARTITION)
	{
		hResult = avsGetFreePartition(TRUE, avsRxPort[plugID].type, &avsRxPort[plugID].partition);
		if (hResult != NO_ERROR) return hResult;
	}

	// Flush Media FIFO partition used for this receiver
	partIdx = avsRxPort[plugID].partition;
	avsRegWrite(partition[partIdx].flushOffset, 1);

	// Set Cipher configuration
	avsDTCPSetCipherCfg(&avsRxPort[plugID]);

	avsRxUpdateQSels (plugID);
	
	avsRxUpdateCfg1 (plugID);

	avsRxUpdateAdo (plugID);
	
	avsRxUpdateCfg0 (plugID);

    //go wait for flow
    avsRxSetState (plugID, AVS_STATE_WAIT_FLOW);
	return hResult;
}

uint32 avsAudioRxGetReceivedSFC (uint32 plugID)
{
	uint32 CIP1;
	
	if (plugID >= NUM_AVS_RX)
	{
		sysLogError(E_BAD_INPUT_PARAMETERS, __LINE__, moduleName);
		return 0;
	}
	avsRegRead(avsRxPort[plugID].baseAddr + ARX_CIP1, &CIP1);

	return (uint32)((CIP1 & CIP1_SFC_MASK) >> CIP1_SFC_SHIFT);
}



HRESULT	avsStartRx(uint32 plugID)
{
	HRESULT hResult;

	if (plugID >= NUM_AVS_RX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// Stop Rx if already started
	hResult = avsStopRx(plugID);
	if (hResult != NO_ERROR) return hResult;

#ifdef _DICE2_WIN
	avsPCStartRx(plugID);
#endif //_DICE2_WIN

	if (avsRxPort[plugID].type == AUDIO_RXDO_PORT)
	{
		hResult = avsStartAudioRx(plugID);
	} 
	else if (avsRxPort[plugID].type == VIDEO_RXDO_PORT)
	{
		hResult = E_NOT_IMPLEMENTED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	} 
	else
	{
		hResult = E_AVS_INVALID_PLUGID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (hResult == NO_ERROR)
	{
		// Enable LLC to receive packets on isoch. channel 0 - 63
		avsSetLLCIsoRxCh(avsRxPort[plugID].channel, TRUE);
	}

	return hResult;
}

HRESULT	avsStopRx(uint32 plugID)
{
	HRESULT hResult = NO_ERROR;

	if (plugID >= NUM_AVS_RX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

#ifdef _DICE2_WIN
	avsPCStopRx(plugID);
#endif //_DICE2_WIN

	if (avsIsRxStarted(plugID) == FALSE) return hResult;

	if (avsRxPort[plugID].type == AUDIO_RXDO_PORT)
	{
        avsRxSetState (plugID, AVS_STATE_DISABLED);

		avsDTCPSetRxDecryption(plugID, 0);
	}
	else if (avsRxPort[plugID].type == VIDEO_RXDO_PORT)
	{
		hResult = E_NOT_IMPLEMENTED;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	else
	{
		hResult = E_AVS_INVALID_PLUGID;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	// Disable LLC to receive packets on isoch. channel 0 - 63
	avsSetLLCIsoRxCh(avsRxPort[plugID].channel, FALSE);
	return hResult;
}

BOOL avsIsRxStarted(uint32 plugID)
{
	if (plugID >= NUM_AVS_RX)
	{
		HRESULT	hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return FALSE;
	}

	return (avsRxPort[plugID].state != AVS_STATE_DISABLED);
}


HRESULT	avsSetRxCh(uint32 plugID, uint8 channel)
{
	HRESULT	hResult = NO_ERROR;
 
	if (plugID >= NUM_AVS_RX ||
		avsRxPort[plugID].type == EMPTY_PORT)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (channel >= 64)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	avsRxPort[plugID].channel = channel;
	// Restart to make effective
	if (avsIsRxStarted(plugID))	avsStartRx(plugID);
	return hResult;
}

HRESULT	avsGetRxCh(uint32 plugID, uint8 *channel)
{
	HRESULT	hResult = NO_ERROR;
 
	if (plugID >= NUM_AVS_RX ||
		avsRxPort[plugID].type == EMPTY_PORT)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	*channel = avsRxPort[plugID].channel;
	return hResult;
}

HRESULT	avsSetRxPresntAdjust(uint32 plugID, int8 samples, uint8 phase)
{
	HRESULT	hResult = NO_ERROR;

	if (plugID >= NUM_AVS_RX ||
		avsRxPort[plugID].type == EMPTY_PORT)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (samples >= 100 || samples <= -100 || phase >= 100)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	avsRxPort[plugID].presentOffSmp = samples;
	avsRxPort[plugID].presentationAdj = phase;
	avsRxUpdateQSels (plugID);
	// Restart to make effective
	//if (avsIsRxStarted(plugID))	avsStartRx(plugID);
	return hResult;
}

// Set format for receiver (input  plugID)
HRESULT	avsSetRxFormat(uint32 plugID, uint32 format)
{
	HRESULT hResult = E_AVS_INVALID_PLUGID;
	
	if (plugID >= NUM_AVS_RX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (avsRxPort[plugID].type == AUDIO_RXDO_PORT)
	{
		hResult = avsSetCIP1Format(&avsRxPort[plugID], format);
		if (hResult != NO_ERROR) return hResult;

		//Warning, if rate changes to from 192 receiver should be restarted
		avsRxUpdateCfg1 (plugID);
		avsRxUpdateCfg0 (plugID);
		avsRxUpdateQSels (plugID);
		
		return NO_ERROR;
	}

	if (avsRxPort[plugID].type == VIDEO_RXDO_PORT)
	{
		hResult = E_NOT_IMPLEMENTED;
	}

	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}


// Get format for receiver (input  plugID)
HRESULT	avsGetRxFormat(uint32 plugID, uint32 *format)
{
	HRESULT hResult = E_AVS_INVALID_PLUGID;

	if (plugID >= NUM_AVS_RX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if ((avsRxPort[plugID].type == AUDIO_RXDO_PORT) ||
		(avsRxPort[plugID].type == VIDEO_RXDO_PORT))
	{
		avsGetCIP1Format(&avsRxPort[plugID], format);
		return NO_ERROR;
	}
	
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

HRESULT	avsSetRxSampleRate(uint32 plugID, uint8 sfc)
{
	HRESULT hResult = NO_ERROR;

	if (plugID >= NUM_AVS_RX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if (sfc >= AVS_NUM_SFC)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

    if (((avsRxPort[plugID].SFC > AVS_SFC_96_KHZ) && (sfc <= AVS_SFC_96_KHZ)) || ((avsRxPort[plugID].SFC <= AVS_SFC_96_KHZ) && (sfc > AVS_SFC_96_KHZ)))
    {
		//need to restart receiver
		avsRxPort[plugID].SFC = sfc;
		if (avsIsRxStarted(plugID))	avsStartRx(plugID);
	}
	else
	{
		avsRxPort[plugID].SFC = sfc;
		avsRxUpdateCfg1 (plugID);
		avsRxUpdateQSels (plugID);		
	}
	return hResult;
}


HRESULT	avsRxMute(uint32 plugID, BOOL muteon)
{
	HRESULT	hResult = NO_ERROR;
	
	if (plugID >= NUM_AVS_RX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (avsRxPort[plugID].type == AUDIO_RXDO_PORT)
	{
		avsRxPort[plugID].mute = muteon;
		avsRxUpdateAdo (plugID);
		return hResult;
	}	
	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

HRESULT	avsSetRxDBS(uint32 plugID, uint32 channels)
{
	HRESULT	hResult = NO_ERROR;
	
	if (plugID >= NUM_AVS_RX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if (channels > DBS_MAX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;	
	}
	if (avsRxPort[plugID].type == AUDIO_RXDO_PORT)
	{
		avsRxPort[plugID].DBS = (uint8) channels;
		// Restart to make effective
		if (avsIsRxStarted(plugID))	avsStartRx(plugID);		
		return hResult;
	}	
	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

HRESULT	avsGetRxDBS(uint32 plugID, uint32 * channels)
{
	HRESULT	hResult = NO_ERROR;
	
	if (plugID >= NUM_AVS_RX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if (avsRxPort[plugID].type == AUDIO_RXDO_PORT)
	{
		*channels = avsRxPort[plugID].DBS;
		return hResult;
	}	
	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}


//note! not updated until next start
HRESULT	avsSetRxQSelSlot(uint32 plugID, uint8 slot, uint8 value)
{
	HRESULT	hResult = NO_ERROR;

	if (plugID >= NUM_AVS_RX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (avsRxPort[plugID].type == AUDIO_RXDO_PORT)
	{
		if (slot < QSEL_SLOTS)
		{
			avsRxPort[plugID].QSelSlot[slot] = value;
			return hResult;
		}
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

HRESULT	avsSetRxSepcifyFlags (uint32 plugID, uint8 bSpecifyDBS, uint8 bSpecifyFDF)
{
	HRESULT	hResult = NO_ERROR;
	
	if (plugID >= NUM_AVS_RX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}
	if (avsRxPort[plugID].type == AUDIO_RXDO_PORT)
	{
		avsRxPort[plugID].bSpecifyDBS = bSpecifyDBS;
		avsRxPort[plugID].bSpecifyFDF = bSpecifyFDF;
		if (avsIsRxStarted(plugID))	avsStartRx(plugID);		
		return hResult;
	}	
	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}


/************************************************************************/
// Returns TRUE if AVS rx is in lock
BOOL avsRxIsLocked(uint32 plugID)
{
	HRESULT	hResult = NO_ERROR;

	if (plugID >= NUM_AVS_RX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return FALSE;
	}

	return (avsRxPort[plugID].state == AVS_STATE_LOCKED);
}


/****************************************************************************
* DESCRIPTION: AVS Receiver Source ID handling
* These functions are used with CMP and AV/C
****************************************************************************/


static HRESULT avsRxGetCurrentSourceID(uint32 plugID, uint32 *pSourceID)
{
#ifdef _DICE2_WIN
	return avsPCRxGetCurrentSourceID(plugID, pSourceID);
#elif defined _DICE2
	HRESULT	hResult = NO_ERROR;

	if (plugID >= NUM_AVS_RX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return hResult;
	}

	if (avsRxPort[plugID].type == AUDIO_RXDO_PORT)
	{
		uint32 ARXCIP0;

		// Read source Id from CIP0 header
		avsRegRead(avsRxPort[plugID].baseAddr + ARX_CIP0, &ARXCIP0);
		*pSourceID = (ARXCIP0 & CIP0_SID_MASK) >> CIP0_SID_SHIFT;
		
		SYS_DEBUG(SYSDEBUG_TRACE_AVS, "avsRxGetCurrentSourceID: ARXCIP0:0x%08x (sourceID:%i) \n\r", ARXCIP0, *pSourceID);

		return hResult;
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
#endif //_DICE2
}

// Returns the source Id for the receiver if it is rxStarted
BOOL avsRxGetSourceID(uint32 plugID, uint32 *pSourceID)
{
	HRESULT	hResult = NO_ERROR;
	BOOL	bLocked = FALSE;

	if (plugID >= NUM_AVS_RX)
	{
		hResult = E_BAD_INPUT_PARAMETERS;
		sysLogError(hResult, __LINE__, moduleName);
		return FALSE;
	}

	bLocked = avsRxIsLocked(plugID);

	if (bLocked)
	{
		HRESULT hResult = NO_ERROR;

		hResult = avsRxGetCurrentSourceID(plugID, pSourceID);
		if (hResult != NO_ERROR) return FALSE;
	}

	return bLocked;
}



/************************************************************************/
HRESULT	avsBoardReset(void)
{
	uint32 i;
	uint32 ATXDIBase;
	uint32 ARXDOBase;
	
	// Reset AVS system rx and tx
	avsEnvConfigRegWrite(0);
	
	// Short delay
	for (i = 0; i < 10000; i++);

	// Restart AVS, clear structures 
	avsReset();

	// Clear all receive registers
	for (i = 0; i < NUM_AVS_RX; i++)
	{
		ARXDOBase = APBA_ARXDO1_BASE + APBA_ARXDO_OFFSET*i;
		avsRegWrite(ARXDOBase + ARX_CFG0,  0);
		avsRegWrite(ARXDOBase + ARX_CFG1,  0);
		avsRegWrite(ARXDOBase + ARX_QSEL0, 0);
		avsRegWrite(ARXDOBase + ARX_QSEL1, 0);
		avsRegWrite(ARXDOBase + ARX_QSEL2, 0);
		avsRegWrite(ARXDOBase + ARX_QSEL3, 0);
		avsRegWrite(ARXDOBase + ARX_QSEL4, 0);
		avsRegWrite(ARXDOBase + ADO_CFG,   0);
		avsRegWrite(ARXDOBase + ADO_MIDI,  0);
	}

	// Clear all transmit registers
	for (i = 0; i < NUM_AVS_TX; i++)
	{
		ATXDIBase = APBA_ATXDI1_BASE + APBA_ATXDI_OFFSET*i;
		avsRegWrite(ATXDIBase + ATX_CFG,	0);
		avsRegWrite(ATXDIBase + ATX_TSTAMP, 0);
		avsRegWrite(ATXDIBase + ATX_PHDR,   0);
		avsRegWrite(ATXDIBase + ATX_CIP0,   0);
		avsRegWrite(ATXDIBase + ATX_CIP1,   0);
		avsRegWrite(ATXDIBase + ADI_CFG,    0);
		avsRegWrite(ATXDIBase + ADI_MIDI,   0);
	}

	return NO_ERROR;
}



/************************************************************************/
// Flush media fifo for all active receivers

void avsRxFlush(void)
{
/* ML 17.03.2005 should not be done
	uint32 plugID;
	uint32 partIdx;
	uint32 ARXDOBase;
	uint32 ARXCfg0;

	// Flush media fifo for all active receivers
	for (plugID = 0; plugID < NUM_AVS_RX; plugID++)
	{
		ARXDOBase = avsRxPort[plugID].baseAddr;
		avsRegRead(ARXDOBase + ARX_CFG0,  &ARXCfg0);
		// Check receiver started
		if ((ARXCfg0 & ARX_RXDO_ENABLE) == ARX_RXDO_ENABLE)
		{
			// If started then stop and flush media fifo
			avsRegWrite(ARXDOBase + ARX_CFG0,  (ARXCfg0 & ~ARX_RXDO_ENABLE));
			// Flush Media FIFO partition used for this receiver
			partIdx = avsRxPort[plugID].partition;
			avsRegWrite(partition[partIdx].flushOffset, 1);
			// Restart AVS receiver
			avsRegWrite(ARXDOBase + ARX_CFG0,  ARXCfg0);
		}
	}
*/
}

/*		
HRESULT avsGetPlugIDFromPartition(BOOL bTx, uint32 partIdx, uint32 *plugID) 
{
	HRESULT		hResult = E_AVS_INVALID_PLUGID;
	uint16		i;
	AVSPORT		*avsPort;

	avsPort = (bTx) ? avsTxPort : avsRxPort;

	for (i = 0; i < NUM_AVSPORTS; i++)
	{
		if (avsPort[i].partition == partIdx)
		{
			*plugID = i;
			return NO_ERROR;
		}
	}

	return hResult;
}
*/

HRESULT avsGetPlugIDFromChannel(BOOL bTx, uint32 channel, BYTE *plugID)
{
	HRESULT		hResult = NO_ERROR;
	uint16		i;

	for (i = 0; i < NUM_AVSPORTS; i++)
	{

		if ((bTx == TRUE &&
			avsTxPort[i].channel == channel &&
			avsTxPort[i].type == AUDIO_TXDI_PORT &&
			avsTxPort[i].partition != NO_PARTITION &&
			avsIsTxStarted(i)) ||
			(bTx == FALSE &&
			avsRxPort[i].channel == channel &&
			avsRxPort[i].type == AUDIO_RXDO_PORT &&
			avsRxPort[i].partition != NO_PARTITION &&
			avsIsRxStarted(i)))
		{
			*plugID = (BYTE) i;
			return hResult;
		}
	}

	hResult = E_BAD_INPUT_PARAMETERS;
	sysLogError(hResult, __LINE__, moduleName);
	return hResult;
}

/****************************************************************************
* DESCRIPTION: AVS General section
****************************************************************************/


HRESULT	avsStart(uint32 plugID, BOOL bTx)
{
	if (bTx) return avsStartTx(plugID);
	else	 return avsStartRx(plugID);
}

HRESULT	avsStop(uint32 plugID, BOOL bTx)
{
	if (bTx) return avsStopTx(plugID);
	else	 return avsStopRx(plugID);
}

BOOL avsIsStarted(uint32 plugID, BOOL bTx)
{
	if (bTx) return avsIsTxStarted(plugID);
	else	 return avsIsRxStarted(plugID);
}

HRESULT	avsSetCh(uint32 plugID, BOOL bTx, uint8 channel)
{
	if (bTx) return avsSetTxCh(plugID, channel);
	else	 return avsSetRxCh(plugID, channel);
}

HRESULT	avsGetCh(uint32 plugID, BOOL bTx, uint8 *channel)
{
	if (bTx) return avsGetTxCh(plugID, channel);
	else	 return avsGetRxCh(plugID, channel);
}

HRESULT avsSetFormat(uint32 plugID, BOOL bTx, uint32 format)
{
	if (bTx) return avsSetTxFormat(plugID, format);
	else	 return avsSetRxFormat(plugID, format);
}
HRESULT	avsGetFormat(uint32 plugID, BOOL bTx, uint32 *format)
{
	if (bTx) return avsGetTxFormat(plugID, format);
	else	 return avsGetRxFormat(plugID, format);
}

HRESULT	avsSetSampleRate(uint32 plugID, BOOL bTx, uint8 sfc)
{
	if (bTx) return avsSetTxSampleRate(plugID, sfc);
	else	 return avsSetRxSampleRate(plugID, sfc);
}

HRESULT	avsMute (uint32 plugID, BOOL bTx, BOOL online)
{
	if (bTx) return avsTxMute(plugID, !online);
	else	 return avsRxMute(plugID, !online);
}

HRESULT	avsSetOnline(uint32 plugID, BOOL bTx, BOOL online)
{
	return avsMute (plugID, bTx, online);
}


HRESULT	avsSetDBS(uint32 plugID, BOOL bTx, uint32 channels)
{
	if (bTx) return avsSetTxDBS(plugID, channels);
	else	 return avsSetRxDBS(plugID, channels);
}

HRESULT	avsGetDBS(uint32 plugID, BOOL bTx, uint32 *channels)
{
	if (bTx) return avsGetTxDBS(plugID, channels);
	else	 return avsGetRxDBS(plugID, channels);
}
