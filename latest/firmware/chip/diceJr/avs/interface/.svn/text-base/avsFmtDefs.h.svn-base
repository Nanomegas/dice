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
	
	Purpose:	AVS Format Receiver and Transmitter register definition
				for format processing of the AVS system
	
	Revisions:
		created 12/09/2003 kbj


****************************************************************************/

#ifndef __avsFmtDefs_h
#define __avsFmtDefs_h

#define AVS_CHANNELS			16

/****************** AVS format receiver memory map ********************/

#define FMT_ARXDO1_BASE			0x200	// Format receiver 1 base address
#define FMT_ARXDO2_BASE			0x230	// Format receiver 2 base address
#define FMT_ARXDO3_BASE			0x260	// Format receiver 3 base address
#define FMT_ARXDO4_BASE			0x290	// Format receiver 4 base address

#define FMT_RXDO_SETUP			0x000	// Receiver format setup
#define FMT_RXDO_LABEL			0x004	// Receiver label bytes
#define FMT_RXDO_LABEL1			0x008	// Receiver label 1 bytes
#define FMT_RXDO_LABEL2			0x00c	// Receiver label 2 bytes
#define FMT_RXDO_LABEL3			0x010	// Receiver label 3 bytes
#define FMT_RXDO_CS_ST			0x014	// Channel status
#define FMT_RXDO_CS_ST1			0x018	// Channel status 1
#define FMT_RXDO_CS_ST2			0x01c	// Channel status 2
#define FMT_RXDO_CS_ST3			0x020	// Channel status 3
#define FMT_RXDO_CS_ST4			0x024	// Channel status 4
#define FMT_RXDO_CS_ST5			0x028	// Channel status 5

// FMT_RXDO_SETUP Receiver format setup
#define RXDO_CSCH_MASK			0x000f	// Selects the channel to sink channel status
#define RXDO_BLKSYNC_MASK		0x00f0	// Selects the channel to create output block sync from
#define RXDO_BLKSYNC_SHIFT		4
#define RXDO_UDCH_MASK			0x0f00	// Selects the channel to take user data from
#define RXDO_UDCH_SHIFT			8
#define RXDO_BLKCS_MASK			0xf000	// Selects the channel as block sync for channel status collection
#define RXDO_BLKCS_SHIFT		12

// FMT_RXDO_LABEL Receiver label bytes
#define RXDO_LABEL_MASK			0xff	// Label bytes receiver x	
#define RXDO_LABEL_SHIFT		8

/****************** AVS format transmitter memory map ********************/

#define FMT_ATXDI1_BASE			0x2c0	// Format transmitter 1 base address
#define FMT_ATXDI2_BASE			0x340	// Format transmitter 2 base address

#define FMT_TXDI_CFG0			0x000	// Channel label configuration
#define FMT_TXDI_CFG1			0x004	// Block sync configuration
#define FMT_TXDI_CFG2			0x008	// Channel status configuration
#define FMT_TXDI_CFG3			0x00c	// PAC enable configuration
#define FMT_TXDI_CFG4			0x010	// Configuration of V(Validity) bit replacement
#define FMT_TXDI_CFG5			0x014	// User Bit selection  ch. 1-8
#define FMT_TXDI_CFG6			0x018	// User Bit selection  ch. 9-16
#define FMT_TXDI_CH_ST			0x01c	// Channel status to be send
#define FMT_TXDI_CH_ST1			0x020	// Channel status 1 to be send
#define FMT_TXDI_CH_ST2			0x024	// Channel status 2 to be send
#define FMT_TXDI_CH_ST3			0x028	// Channel status 3 to be send
#define FMT_TXDI_CH_ST4			0x02c	// Channel status 4 to be send
#define FMT_TXDI_CH_ST5			0x030	// Channel status 5 to be send
#define FMT_TXDI_CH_CS			0x034	// Channel specific status bits in IEC 60958 mode
										// Label bits in mask mode

// FMT_TXDI_CFG0 channel label configuration
#define TXDI_LABEL_MASK			0x3
#define TXDI_LABEL_SHIFT		2

// FMT_TXDI_CFG1 Block sync configuration
#define TXDI_BLOCK_SYNC_SLAVE	BIT31	// Block Sync Master(0)/Slave(1) Mode 
#define TXDI_MASTER_SYNC_INPUT	BIT30	// Master Sync Select. Free running (0), Sync to input (1)
#define TXDI_CS_ON_SLAVE_CH		BIT29	// Collect Channel Status on Slave Channel Enable. 
										// (Channel that block sync is slave to)
										
#define TXDI_BS_SLAVE_CH_SELECT_MASK	0x0f000000
#define TXDI_BS_SLAVE_CH_SELECT_SHIFT	24	// Block Sync Slave Channel Select
#define TXDI_BS_PRESET_VALUE_MASK		0x00ff0000
#define TXDI_BS_PRESET_VALUE_SHIFT		16	// Block Sync Preset Value
#define TXDI_AUTO_CH_STATUS_CRC_MASK	0x0000ffff // Auto channel status CRC disable bits

// FMT_TXDI_CFG2 Channel status configuration
#define TXDI_CS_MASK			0x3		// Channel x CS configuration
#define TXDI_CS_SHIFT			2

// FMT_TXDI_CFG3 PAC enable configuration
#define TXDI_PAC_SB_CH_MASK		0xffff0000 // PAC SB Enable bit for channels 16 to 1
#define TXDI_PAC_SB_CH_SHIFT	16
#define TXDI_PAC_SF_CH_MASK		0x0000ffff // PAC SF Enable bit for channels 16 to 1

// FMT_TXDI_CFG4 Configuration of V bit replacement
#define TXDI_V_REPLACE_CH_MASK	0xffff0000 // V Bit Replacement Enable bit for channels 16 to 1
#define TXDI_V_REPLACE_CH_SHIFT	16
#define TXDI_V_MASK				0x0000ffff // V Bits for Channels 16 to 1, used when enabled

// FMT_TXDI_CFG5 User Bit selection  ch. 1-8
// FMT_TXDI_CFG6 User Bit selection  ch. 9-16
#define TXDI_USER_BIT_MASK		0xf		// User Bit Selection for channels
#define TXDI_USER_BIT_SHIFT		4

// Position of label in FMT_TXDI_CH_CS area
#define TXDI_CH_LABEL_MASK		0xff000000
#define TXDI_CH_LABEL_SHIFT		24


#endif
