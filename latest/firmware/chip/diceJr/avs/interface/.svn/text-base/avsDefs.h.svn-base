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
 * DESCRIPTION: Defines for the AVSystem API (AVS)


	Revisions:
		created 09/19/2002 lm

****************************************************************************/

#ifndef _AVSDEFS_H
#define _AVSDEFS_H

#include "TCTypes.h"

// IEC61883-6 CIP header defines.
// -------------------------------------------------------------------------------------
// Isochronous packet header
#define ISO_HDR_SY_MASK				0x00000001
#define ISO_HDR_ODDEVEN_MASK		0x00000002
#define ISO_HDR_ODDEVEN_SHIFT		1
#define ISO_HDR_EMI_MASK			0x0000000c
#define ISO_HDR_EMI_SHIFT			2
#define ISO_HDR_TCODE_MASK			0x000000f0
#define ISO_HDR_TCODE_SHIFT			4
#define ISO_HDR_CH_MASK				0x00003f00
#define ISO_HDR_CH_SHIFT			8
#define ISO_HDR_TAG_MASK			0x0000c000
#define ISO_HDR_TAG_SHIFT			14
#define ISO_HDR_TAG_CIP				0x1		// Value of TAG in isoc. header to identify CIP header
#define ISO_HDR_LENGTH_MASK			0xffff0000
#define ISO_HDR_LENGTH_SHIFT		16

// CIP0 quadlet format
#define CIP0_DBC_MASK				0x000000ff
#define CIP0_SPH_BIT				BIT10
#define CIP0_QPC_MASK				0x00003800
#define CIP0_QPC_SHIFT				11
#define CIP0_FN_MASK				0x0000c000
#define CIP0_FN_SHIFT				14
#define CIP0_DBS_MASK				0x00ff0000
#define CIP0_DBS_SHIFT				16
#define CIP0_SID_MASK				0x3f000000
#define CIP0_SID_SHIFT				24
#define CIP0_ID						0x00000000

// CIP1 quadlet format
#define CIP1_SYT_MASK				0x0000ffff
#define CIP1_FDF_MASK				0x00ff0000
#define CIP1_FDF_SHIFT				16
#define CIP1_SFC_MASK				0x00070000
#define CIP1_SFC_SHIFT				16
#define CIP1_FMT_MASK				0x3f000000
#define CIP1_FMT_SHIFT				24
#define CIP1_ID						0x80000000

#define CIP1_FMT_AUDIO_MUSIC		0x10	// Value of FMT in CIP header for audio music data

// PDB INT Enable Register
// -------------------------------------------------------------------------------------
#define PDB_AVS_NEWTX				BIT3  // Enable new cycle done method which minimizes tx buffer delay
#define PDB_IRX_RUN					BIT2  // Enable AVS isochronous receive interface
#define PDB_ITX_RUN					BIT1  // Enable AVS isochronous transmit interface
#define PDB_AVS_RUN					BIT0  // Enable AVS module

// Audio/Video base addresses
// -------------------------------------------------------------------------------------
#define APBA_ARXDO1_BASE			0x000	// Audio receive port 1
#define APBA_ARXDO2_BASE			0x030	// Audio receive port 2
#define APBA_ATXDI1_BASE			0x0c0	// Audio transmit port 1
#define APBA_ATXDI2_BASE			0x0dc	// Audio transmit port 1

#define APBA_ARXDO_OFFSET			0x30	// Address offset between ARXDO
#define APBA_ATXDI_OFFSET			0x1c	// Address offset between ATXDI

// Offset for APBA_ADRXDO._BASE registers
// -------------------------------------------------------------------------------------
#define ARX_CFG0					0x00	
#define ARX_CFG1					0x04
#define ARX_QSEL0					0x08
#define ARX_QSEL1					0x0c
#define ARX_QSEL2					0x10
#define ARX_QSEL3					0x14
#define ARX_QSEL4					0x18
#define ARX_PHDR					0x1c	// Read only copy of last isoch. header
#define ARX_CIP0					0x20	// Read only copy of last CIP0 header
#define ARX_CIP1					0x24	// Read only copy of last CIP1 header

#define ADO_CFG						0x28
#define ADO_MIDI					0x2c

// ARX_CFG0 Audio receive configuration 0 RXDO port reg.
#define ARX_RXDO_ENABLE				  BIT31		// Enables operation of the RXDO block
#define ARX_EMI_CHECK_ENABLE		BIT30		// Enable checking if EMI bits change mid-stream (If enable the interrupt IRQ_ARX1_CFG_FAIL)
#define ARX_FMT_CHECK_ENABLE		BIT29		// Enable checking if specified fields
#define ARX_FDF_CHECK_ENABLE		BIT28		// is equal to detected in CIP header
#define ARX_DBS_CHECK_ENABLE		BIT27		// If enable the interrupt IRQ_ARX1_CFG_FAIL
#define ARX_FN_CHECK_ENABLE			BIT26		// is generated
#define ARX_QPC_CHECK_ENABLE		BIT25
#define ARX_SPH_CHECK_ENABLE		BIT24
#define ARX_TAG_CHECK_ENABLE		BIT23
#define ARX_SYT_INTERVAL_32			BIT22		// SYT interval of 32 "cheating" mode enable
#define ARX_FDF_NO_DATA 			  BIT21		// Enables check of the FDF field to identify NO_DATA packet.
#define ARX_SPH_ENABLE				  BIT20		// Use the specified SPH field else uses SPH in CIP header
#define ARX_SPH_SPECIFIED			  BIT19		// Specified SPH to be used if ARX_SPH_ENABLE
#define ARX_TAG_ENABLE				  BIT18		// Use the specifed TAG field else uses TAG in packet header                                    
#define ARX_TAG_SPECIFIED_MASK	0x00030000	// Specified TAG to be used if ARX_TAG_ENABLE
#define ARX_TAG_SPECIFIED_SHIFT	16
#define ARX_SID_CHECK_ENABLE		BIT15		// Enable compare of Source ID (SID) from isoch stream agains
#define ARX_SID_MASK				    0x00007e00	// the SID value given in ARX_SID_MASK, interrupt IRQ_ARX1_CFG_FAIL
#define ARX_SID_SHIFT				    9
#define ARX_PARTITION_SEL_MASK	0x000001c0	// Media FIFO partition this RXDO block shall use (1-3 supports M6)
#define ARX_PARTITION_SEL_SHIFT	6
#define ARX_CHANNEL_ID_MASK			0x0000003f	// Tell the ARX what channel ID it shall take its isoch data from

// ARX_CFG1 Audio receive configuration 1 RXDO port reg.
#define ARX_DBS_ENABLE				  BIT31		// Use the specifed DBS field else uses DBS in CIP header
#define ARX_FN_ENABLE				    BIT30		// Use the specifed FN field else uses FN in CIP header
#define ARX_QPC_ENABLE				  BIT29		// Use the specifed QPC field else uses QPC in CIP header
#define ARX_DBS_SPECIFIED_MASK	0x1fe00000	// Specified DBS to be used if ARX_DBS_ENABLE
#define ARX_DBS_SPECIFIED_SHIFT	21
#define ARX_FN_SPECIFIED_MASK		0x00180000	// Specified FN to be used if ARX_FN_ENABLE
#define ARX_FN_SPECIFIED_SHIFT	19
#define ARX_QPC_SPECIFIED_MASK	0x00070000	// Specified QPC to be used if ARX_QPC_ENABLE
#define ARX_QPC_SPECIFIED_SHIFT	16
#define ARX_FMT_ENABLE				  BIT15		// Use the specifed FMT field else uses FMT in CIP header
#define ARX_FDF_ENABLE				  BIT14		// Use the specifed FDF field else uses FDF in CIP header
#define ARX_FMT_SPECIFIED_MASK	0x00003f00	// Specified FMT to be used if ARX_FMT_ENABLE
#define ARX_FMT_SPECIFIED_SHIFT	8
#define ARX_FDF_SPECIFIED_MASK	0x000000ff	// Specified FDF to be used if ARX_FDT_ENABLE

// ARX_QSEL4 Filter selection for slot in DB to allow through to the Media FIFO
#define ARX_TIME_ADJUST_MASK		 0xffff0000	// Allows skew on presentation time.	
#define ARX_TIME_ADJUST_SHIFT		 16
#define ARX_TIME_ADJUST_DEFAULT  0x00000000
#define ARX_TIME_ADJUST_NOT_CYCM 0x03130000  // Ajust value when not cycle master
 
// ADO_CFG Audio data output configuration reg.
#define ADO_UNMUTE					    BIT31		// All data output from the ADO will be muted
#define ADO_MIDI_ENABLE				    BIT30		// Tell the ADO whether the last quadlet in every DB is MIDI
#define ADO_MUTE_AUTO				    BIT29		// Causes an auto mute of the ADO data stream when no lock
#define ADO_LOCKED                      BIT28
#define ADO_CFG_DUAL_WIRE				BIT27
#define ADO_ENCR_RESET_COUNT_ADBB		BIT25		// Reset Encryption Frame Count on Audio Data Block Boundaries (used in ARX).
#define ADO_ENCR_RESET_COUNT_AFB		BIT24		// Reset Encryption Frame Count on Audio Frame Boundaries (used in ARX).
#define ADO_ENCR_FRAME_SIZE_MASK		0x00ff0000	// Encryption Frame Size, in quadlets (used in ARX).
#define ADO_ENCR_FRAME_SIZE_SHIFT		16
#define ADO_LOCK_CNT_MASK			    0x0000ffff  // Sets the number of samples that must pass without slipping before locked
#define ADO_LOCK_CNT_DEFAULT			0xffff	    // Number of samples before accept lock

// ADO_MIDI/ADI_MIDI Audio output/input midi mapper
#define ADIO_MIDI_ENABLE			    BIT3

// Offset for APBA_ADTXDI._BASE registers
// -------------------------------------------------------------------------------------
#define ATX_CFG						0x00
#define ATX_TSTAMP					0x04
#define ATX_PHDR					0x08
#define ATX_CIP0					0x0c
#define ATX_CIP1					0x10

#define ADI_CFG						0x14
#define ADI_MIDI					0x18

// ATX_CFG Audio transmit configuration TXDI port reg.
#define ATX_TXDI_ENABLE				BIT31		// Enables operation of the TXDI block
#define ATX_NO_DATA					BIT30		// Causes the ATX to send NO_DATA packets when insufficient data (Else empty packets)
#define ATX_FMT_NO_DATA				BIT29		// FMT field of NO_DATA packets to be 0x3f
#define ATX_FDF_NO_DATA				BIT28		// FDF field of NO_DATA packets to be 0xff
#define ATX_CIP_TIMESTAMP			BIT24		// Tells ATX to put timestamps in the CIP header (audio streams)
#define ATX_SYS_MODE_MASK			(BIT21 | BIT20) 		// Sys mode sets the syt interval, 0:8, 1:16, 2:32
#define ATX_SYS_MODE_SHIFT    20
#define ATX_ODD_EVEN_INSERT			BIT19		// Enable insertion of Encryption Odd/Even bit into isoch PHDR
#define ATX_GLB_DBS_MASK			0x000001f0	// Data block size that drives all counters of the ATX
#define ATX_GLB_DBS_SHIFT			4
#define ATX_PARTITION_SEL_MASK		0x00000007  // Media FIFO partition				

//#define ATX_TSTAMP_DEFAULT		0x00003000	// 3 isoch. cycle of 125 us
//#define ATX_TSTAMP_DEFAULT		0x00004600	// 3 isoch. cycle of 125 us
//#define ATX_TSTAMP_DEFAULT		0x00005000	// 3 isoch. cycle of 125 us

#define ATX_TSTAMPSMP_DEFAULT		30          // 30 samples    


#define ATX_TSTAMP_DEFAULT			0x00005000	// 5 isoch. cycle of 125 us 

#define ATX_TSTAMP_32KHZ			0x00004617	// Ajusting clock to be in phase at 32 kHz
#define ATX_TSTAMP_44KHZ			0x000039a0	// Ajusting clock to be in phase at 44 kHz
#define ATX_TSTAMP_48KHZ			0x000035ea	// Ajusting clock to be in phase at 48 kHz
#define ATX_TSTAMP_88KHZ			0x0000368b	// Ajusting clock to be in phase at 88 kHz
#define ATX_TSTAMP_96KHZ			0x000033E7	// Ajusting clock to be in phase at 96 kHz

// ATX_PHDR Audio transmit isoch. header
#define ATX_SPEED_MASK				0x00070000	// Tx speed field. (S100, S200, S400)
#define ATX_SPEED_SHIFT				16
#define ATX_TAG_MASK				0x0000c000	// Isoch. hdr TAG
#define ATX_TAG_SHIFT				14
#define ATX_CH_MASK					0x00003f00	// Isoch. hdr Channel
#define ATX_CH_SHIFT				8
#define ATX_TCODE_MASK				0x000000f0	// Isoch. hdr TCODE
#define ATX_TCODE_SHIFT				4
#define ATX_EMI_MASK				0x0000000c	// Isoch. hdr EMI
#define ATX_EMI_SHIFT				2
#define ATX_ODDEVEN_BIT				BIT1		// Isoch. hdr ODD/EVEN
#define ATX_SY_MASK					0x00000001	// Isoch. hdr SY

// ATX_CIP0 Audio transmit CIP header part 0
#define ATX_SID_MASK				0x3f000000	// Source ID of CIP0 header
#define ATX_SID_SHIFT				24
#define ATX_DBS_MASK				0x00ff0000	// Data Block Size
#define ATX_DBS_SHIFT				16
#define ATX_FN_MASK					0x0000c000	// FN field of CIP0 quadlet	
#define ATX_FN_SHIFT				14
#define ATX_QPC_MASK				0x00003800	// QPC field of CIP0 quadlet
#define ATX_QPC_SHIFT				11
#define ATX_SPH_BIT					BIT10		
#define ATX_DBC_MASK				0x000000ff	// Filled by ATX when sending CIP0 quadlet			

// ATX_CIP1	Audio transmit CIP header part 1
#define ATX_FMT_MASK				0x3f000000	// FMT field of CIP1 quadlet
#define ATX_FMT_SHIFT				24
#define ATX_FDF_MASK				0x00ff0000	// FDF field of CIP1 quadlet
#define ATX_FDF_SHIFT				16
#define ATX_SFC_MASK				0x07		// SFC part of FDF field
#define ATX_SYT_MASK				0x0000ffff  // SYT timestamp filled by ATX when sending CIP1 quadlet

// ADI_CFG Audio data input configuration
#define ADI_UNMUTE					BIT31		// When low all output data from ADI will be muted
#define ADI_MIDI_ENABLE				BIT30		// Tell the ADI that the last quadlet in every DB should be filled with MIDI data.
#define ADI_CFG_DW           BIT29
#define ADI_ODD_EVEN_TAG			BIT10		// Raw Odd/Even Bit for Encryption Tag Bits (changes take effect on EF boundary)
#define ADI_ENCR_RESET_COUNT_ADBB	BIT9		// Reset Encryption Frame Count on Audio Data Block Boundaries
#define ADI_ENCR_RESET_COUNT_AFB	BIT8		// Reset Encryption Frame Count on Audio Frame Boundaries
#define ADI_ENCR_FRAME_SIZE_MASK	0x000000ff	// Encryption Frame Size, in quadlets


// AVS INT_CTLR APB addresses 
// -------------------------------------------------------------------------------------
#define APBA_INT0_STATUS			0x144
#define APBA_INT0_MASK				0x148
#define APBA_INT1_STATUS			0x14c
#define APBA_INT1_MASK				0x150


 
// AVS Media FIFO APB addresses
// -------------------------------------------------------------------------------------
#define APBA_PARTITION0_BASE		0x184
#define APBA_PARTITION0_LIMIT		0x188
#define APBA_PARTITION0_FLUSH		0x18c
#define APBA_PARTITION1_BASE		0x190
#define APBA_PARTITION1_LIMIT		0x194
#define APBA_PARTITION1_FLUSH		0x198
#define APBA_PARTITION2_BASE		0x19c
#define APBA_PARTITION2_LIMIT		0x1a0
#define APBA_PARTITION2_FLUSH		0x1a4
#define APBA_PARTITION3_BASE		0x1a8
#define APBA_PARTITION3_LIMIT		0x1ac
#define APBA_PARTITION3_FLUSH		0x1b0
#define APBA_PARTITION4_BASE		0x1b4
#define APBA_PARTITION4_LIMIT		0x1b8
#define APBA_PARTITION4_FLUSH		0x1bc

// APB<->MIDI interface APB addresses
// -------------------------------------------------------------------------------------
#define APBA_MIDI_APB_IF_STATUS		0x1e4
#define APBA_MIDI_APB_IF_CONTROL	0x1e8
#define APBA_MIDI_APB_IF_QUAD_OUT	0x1ec
#define APBA_MIDI_APB_IF_QUAD_IN0	0x1f0
#define APBA_MIDI_APB_IF_QUAD_IN1	0x1f4

// Size in quadlets of MIDI in/out buffers
#define APBA_MIDI_QUAD_OUT_SIZE		16
#define APBA_MIDI_QUAD_IN_SIZE		4

// Bits for APBA_MIDI_APB_IF_STATUS
#define MIDI_TX_QUADS_IN_BUF_MASK 	0x000007c0
#define MIDI_TX_QUADS_IN_BUF_SHIFT 	6
#define MIDI_BUF_FULL_TX			BIT5
#define MIDI_BUF_EMPTY_TX			BIT4
#define MIDI_BUF_FULL_RX1			BIT3
#define MIDI_BUF_EMPTY_RX1			BIT2
#define MIDI_BUF_FULL_RX0			BIT1
#define MIDI_BUF_EMPTY_RX0			BIT0

// Bits for APBA_MIDI_APB_IF_CONTROL
#define MIDI_TX_IRQ_EN				BIT2
#define MIDI_RX1_IRQ_EN				BIT1
#define MIDI_RX0_IRQ_EN				BIT0

// Bits for MIDI data quadlet exchanged with AVS tx and rx
#define MIDI_QUAD_PORT_MASK			0xe0000000
#define MIDI_QUAD_PORT_SHIFT		29
#define MIDI_QUAD_SOURCE_MASK		0x18000000	// In Midi read data only
#define MIDI_QUAD_SOURCE_SHIFT		27
#define MIDI_QUAD_COUNTER_MASK		0x03000000
#define MIDI_QUAD_COUNTER_SHIFT		24
#define MIDI_QUAD_DATA_MASK			0x00ffffff


// Media FIFO partition status
#define APBA_MFIFO_STATUS			0x1fc
#define PARTITION_UNDERFLOW			BIT0
#define PARTITION_OVERFLOW			BIT1

#define AVS_SFC_32_KHZ				0x00
#define AVS_SFC_44_1_KHZ			0x01
#define AVS_SFC_48_KHZ				0x02
#define AVS_SFC_88_2_KHZ			0x03
#define AVS_SFC_96_KHZ				0x04
#define AVS_SFC_176_4_KHZ			0x05
#define AVS_SFC_192_KHZ				0x06
#define AVS_NUM_SFC					  0x07

// Data block size, number of channels per. sample
#define DBS_STEREO					2		// Stereo Left and Rigth channel
#define DBS_SURROUND				6		// 5.1 setup
#define DBS_MAX						16		// Max. 16 audio channels in one block for transmitter

#define QSEL_SLOTS					17		// Number of Slots in QSels

// Protected variables only to be used by other avs modules
											          //  Rx port type,   Tx port type		
#define AVS_PLUG_ID1				0		//  Audio			      Audio
#define AVS_PLUG_ID2				1		//	Audio			      Audio
#define NUM_AVSPORTS				2		// There is a 1:1 relation between ports and plugID

typedef enum
{
	EMPTY_PORT,						// Empty port not in use
	AUDIO_RXDO_PORT,				// AVS isoc. audio receive port
	AUDIO_TXDI_PORT,				// AVS isoc. audio transmit port
	AXX,							// audio receive or transmit
	ARX = AUDIO_RXDO_PORT,
	ATX = AUDIO_TXDI_PORT,
} AVS_PORT_TYPE;


// Audio and Video Ports
typedef struct tPORT
{
	AVS_PORT_TYPE	type;					// type of port	
	uint32			baseAddr;				// AVS register base address
	uint8			channel;				// Isoc. channel number
	uint32			sourceID;				// 1394 node id
	uint8			mute;					// Mute/Unmute
	uint8			midi;					// Midi enable/disable
	uint8			DBS;					// Quadlets per. data block
	uint8			QSelSlot[QSEL_SLOTS];	// Slot indexes for QSels
	uint8			FMT;					// Value of FMT part of CIP1
	uint8			FDF;					// Value of FDF part of CIP1 witout (SFC)
	uint8			SFC;					// Sample rate part of FDF in CIP1
	uint8			SYT32;					// Enabling of SYT interval 32, in 96 kHz mode, simulates 192 kHz
	uint8			partition;				// Partition used in Media Fifo
	uint8			ATXSpeed;				// Isoch. transmission speed (SPEED_100, SPEED_200 or SPEED_400)
	int8			presentOffSmp;			// The presentation time offset in samples (This can be negative in case of Rx)
	uint8			presentationAdj;		// Presentation time fine adjust in percent of a sample.
	uint8			bSpecifyDBS;
	uint8			bSpecifyFDF;
	uint8			bDualWire;
	uint8			bEnabled;
	uint8			lastValidSFC;			// For Rx only, this is the last valid SFC read.
											// In some cases the SFC of an empty packet (Mac) is not valid (0xff)
											// we will just use the last in that case.
} AVSPORT;

#endif //_AVSDEFS_H
