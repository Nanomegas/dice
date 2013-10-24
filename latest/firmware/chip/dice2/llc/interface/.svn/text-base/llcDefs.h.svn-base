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
	
	Purpose:	Defines for Link access for the Samsung 1394 LLC
	
	Revisions:
		created 02/12/2002 kbj


****************************************************************************/

#ifndef _LLCDEFS_H_
#define _LLCDEFS_H_

/****************************************************************************/

#include "coreDefs.h"

// Macro to convert register to memory pointer
#define mPtr(x)						((volatile int *)(LLCBASE+x))
// Macro to convert register to 32 bits array index
#define mIdx(x)						(x/4)



#define LLC_BIT31					0x00000001
#define LLC_BIT30					0x00000002
#define LLC_BIT29					0x00000004
#define LLC_BIT28					0x00000008
#define LLC_BIT27					0x00000010
#define LLC_BIT26					0x00000020
#define LLC_BIT25					0x00000040
#define LLC_BIT24					0x00000080
#define LLC_BIT23					0x00000100
#define LLC_BIT22					0x00000200
#define LLC_BIT21					0x00000400
#define LLC_BIT20					0x00000800
#define LLC_BIT19					0x00001000
#define LLC_BIT18					0x00002000
#define LLC_BIT17					0x00004000
#define LLC_BIT16					0x00008000
#define LLC_BIT15					0x00010000
#define LLC_BIT14					0x00020000
#define LLC_BIT13					0x00040000
#define LLC_BIT12					0x00080000
#define LLC_BIT11					0x00100000
#define LLC_BIT10					0x00200000
#define LLC_BIT9 					0x00400000
#define LLC_BIT8 					0x00800000
#define LLC_BIT7 					0x01000000
#define LLC_BIT6 					0x02000000
#define LLC_BIT5 					0x04000000
#define LLC_BIT4 					0x08000000
#define LLC_BIT3 					0x10000000
#define LLC_BIT2 					0x20000000
#define LLC_BIT1 					0x40000000
#define LLC_BIT0 					0x80000000

//=============================================================================
// Samsung LLC register definitions taken from manual Annex D

#define VERSION_REG_DP				0x00		// Version control reg.
#define ND_ID_REG_DP				0x04		// Node ID reg.
#define LNK_CTRL_REG_DP				0x08		// Link control reg. 
#define LCSR_REG_DP					0x0c		// Link control and status
#define CY_TMR_REG_DP				0x10		// Cycle timer reg.
#define ATFIFO_STAT_REG_DP			0x14		// AT fifo status reg.
#define ITFIFO_STAT_REG_DP			0x18		// IT fifo status reg.
#define ARFIFO_STAT_REG_DP			0x1c		// AR fifo status reg.
#define IRFIFO_STAT_REG_DP			0x20		// IR fifo status reg.
#define ISO_RX_ENB_REG_1_DP			0x24		// Isoch channel enable reg. 1
#define ISO_RX_ENB_REG_2_DP			0x28		// Isoch channel enable reg. 2
#define ISO_TX_STAT_REG_DP			0x2c		// IT status reg.
#define ASY_TX_STAT_REG_DP			0x30		// AT status reg.
#define PHY_CTRL_REG_DP				0x44		// Phy control reg.
#define INTERRUPT_REG_DP			0x48		// Interrupt reg.
#define INTERRUPT_REG_SET_DP		0x48		// Interrupt set reg.
#define INTERRUPT_REG_CLEAR_DP		0x4c		// Interrupt clear reg.
#define INTR_MASK_REG_SET_DP		0x50		// Interrupt enable reg.
#define INTR_MASK_REG_CLEAR_DP		0x54		// Interrupt disable reg.
#define DIAG_REG_DP					0x58		// Diagnostics status reg.
#define BUS_STAT_REG_DP				0x5c		// Bus status reg.
#define ASY_TX_FIFO_SPACE_REG_DP	0x60		// AT FIFO space reg.
#define ASY_RX_FIFO_QLETS_REG_DP	0x64		// AR FIFO quadlets reg.
#define ISO_TX_FIFO_SPACE_REG_DP	0x68		// IT FIFO space reg.
#define ISO_RX_FIFO_QLETS_REG_DP	0x6c		// IR FIFO quadlets reg.
#define ISO_DATA_PATH_REG_DP		0x70		// Data path reg.
#define ASY_TX_FIRST_REG_DP			0x74		// AT first quadlet reg.
#define ASY_CONTINUE_REG_DP			0x78		// AT Continue reg.
#define ASY_CONTINUE_UPDATE_REG_DP	0x7c		// AT Continue Update reg.
#define ASY_TX_FIFO_DEPTH_REG_DP	0x80		// AT FIFO Depth reg.
#define ASY_RX_FIFO_REG_DP			0x84		// AR FIFO reg. 
#define ASY_RX_FIFO_DEPTH_REG_DP	0x88		// AR FIFO Depth reg.
#define ISO_TX_FIRST_REG_DP			0x8c		// IT frst quadlet reg.
#define ISO_CONTINUE_REG_DP			0x90		// IT Continue reg.
#define ISO_CONTINUE_UPDATE_REG_DP	0x94		// IT Contiunue Update reg.
#define ISO_TX_FIFO_DEPTH_REG_DP	0x98		// IT FIFO Depth reg.
#define ISO_RX_FIFO_REG_DP			0x9c		// IR FIFO reg.
#define ISO_RX_FIFO_DEPTH_REG_DP	0xa0		// IR FIFO Depth reg.
#define HST_ACC_ERR_REG_DP			0xa4		// Host Access Error reg.
#define RET_CT_REG_DP				0xa8		// Retry Count reg.
#define DIG_FSM_STAT_REG			0xac		// Get teh status of the internal FSMs
#define ISO_TX_ENB_REG_1_DP			0xb0		// Isoch. Channel Tx enable reg. 1
#define ISO_TX_ENB_REG_2_DP			0xb4		// Isoch. Channel Tx enable reg. 2
#define ISO_HDR_REG_DP				0xb8		// Isoch. Header Quadlet reg.
#define LPS_REG_DP					0xbc		// Link Power Status reg.
#define PING_REG_DP					0xc0		// Ping reg.
#define ISO_EXPC_CHAN_REG1			0xc4		// Iso expected channel reg. 1
#define ISO_EXPC_CHAN_REG2			0xc8		// Iso expected channel reg. 2
#define DUP_EXPC_STAT_REG			0xcc		// Duplicate, expected channel status reg.
#define ASYN_RX_ENB_REG_1_DP		0xd0		// Asy received channel reg. 1
#define ASYN_RX_ENB_REG_2_DP		0xd4		// Asy received channel reg. 2


//=============================================================================
// 1394 Samsung registers definition of register structures
//		Asynchronous and Control Registers only (No ISOC specified)

//----------------------------------------------------------------------------
// VERSION_REG_DP Version and revicion control reg.
#define LLC_VERSION_NUM_MASK	0xFFFF0000	// Version number
#define LLC_VERSION_NUM_SHIFT	16
#define LLC_REV_NUM_MASK		0x0000FFFF	// Revision number

//----------------------------------------------------------------------------
// ND_ID_REG_DP Node Identification and Status reg.
#define LLC_VALID_ND_ID			LLC_BIT0	// Set if valid node id 
#define LLC_ROOT				LLC_BIT1	// Set if root
#define LLC_CBL_PWR_ST			LLC_BIT4	// Set if cable power status OK
#define LLC_BUS_ADDR_MASK		0x0000FFC0	// Bus address (Default 0x3FF)
#define LLC_BUS_ADDR_SHIFT		6
#define LLC_ND_ADDR_MASK		0x0000003F	// Physical node number (Default 0x3F)

//----------------------------------------------------------------------------
// LNK_CTRL_REG_DP Link Control reg.
#define LLC_CY_SOURCE			LLC_BIT8	// External 8 Khz clock 
#define LLC_CMSTR_EN			LLC_BIT9	// When root and set cycle start packets will be send
#define LLC_CY_TIMER_EN			LLC_BIT10	// Set by LINK if cycle master is enabled
#define LLC_RCV_PHY_PKT			LLC_BIT21	// Accept incoming PHY packets
#define LLC_SELF_ID_RXD			LLC_BIT22	// When set self_id packets are received

//----------------------------------------------------------------------------
// LCSR_REG_DP Link Control and Status information Reg.
#define LLC_BUSY_IND				LLC_BIT4	// Set then link will send the busy ack.
#define LLC_FIFO_CONFIG_EN			LLC_BIT5	// Set then possible to modify FIFO partition
#define LLC_MAX_LINK_SPEED_MASK		0x000E0000  // Maximum speed is S100, S200 or S400
#define LLC_MAX_LINK_SPEED_SHIFT	17
#define LLC_ACCLM_ENABLED			LLC_BIT16	// Enable LLC accelration (Enable in PHY)
#define LLC_SOFT_RST				LLC_BIT17	// Set then core will be reset
#define LLC_STORE_BAD_ASY_HDR		LLC_BIT18	// Set then bad header and xfer will be stored
#define LLC_AUTO_CY_MAST			LLC_BIT27	// If set this node will be cycle master 
#define LLC_HOST_SLEEP_MODE			LLC_BIT28	// If set then LINK generates intr on new packets
#define LLC_MAN_MODE				LLC_BIT31	// If set then write ND_ID_REG_DP allowed

//----------------------------------------------------------------------------
// CY_TMR_REG_DP Cycle timer reg.
#define LLC_SEC_CNT_MASK		0xFE000000	// Cycle Seconds
#define LLC_SEC_CNT_SHIFT		25			// bit shift value for seconds
#define LLC_CY_CNT_MASK			0x01FFF000	// Cycle Count
#define LLC_CY_OFFSET_SHIFT		12			// bit shift value for count
#define LLC_CY_OFFSET_MASK		0x00000FFF	// Cycle Offset
 
//----------------------------------------------------------------------------
// ATFIFO_STAT_REG_DP Asynchronous transmit FIFO status reg.
#define LLC_ATFIFO_FULL			LLC_BIT0
#define LLC_ATFIFO_MT			LLC_BIT1
#define LLC_ATFIFO_4QD_AV		LLC_BIT2
#define LLC_ATFIFO_1QD_AV		LLC_BIT3
#define LLC_ATFIFO_4QS_AV		LLC_BIT4
#define LLC_ATFIFO_1QS_AV		LLC_BIT5
#define LLC_ATFIFO_CLR			LLC_BIT6
#define LLC_MASK_ATFIFO_FULL	LLC_BIT20
#define LLC_MASK_ATFIFO_MT		LLC_BIT21
#define LLC_MASK_ATFIFO_4QD_AV	LLC_BIT22
#define LLC_MASK_ATFIFO_1QD_AV	LLC_BIT23
#define LLC_MASK_ATFIFO_4QS_AV	LLC_BIT24
#define LLC_MASK_ATFIFO_1QS_AV	LLC_BIT25


//----------------------------------------------------------------------------
// ARFIFO_STAT_REG_DP Asynchronous receive FIFO status reg.
#define LLC_ARFIFO_FULL			LLC_BIT0
#define LLC_ARFIFO_MT			LLC_BIT1
#define LLC_ARFIFO_4QD_AV		LLC_BIT2
#define LLC_ARFIFO_1QD_AV		LLC_BIT3
#define LLC_ARFIFO_4QS_AV		LLC_BIT4
#define LLC_ARFIFO_1QS_AV		LLC_BIT5
#define LLC_ARFIFO_CLR			LLC_BIT6

#define LLC_ARFIFO_HEAD  		LLC_BIT8
#define LLC_ARFIFO_TAIL  		LLC_BIT9

#define LLC_MASK_ARFIFO_FULL	LLC_BIT20
#define LLC_MASK_ARFIFO_MT		LLC_BIT21
#define LLC_MASK_ARFIFO_4QD_AV	LLC_BIT22
#define LLC_MASK_ARFIFO_1QD_AV	LLC_BIT23
#define LLC_MASK_ARFIFO_4QS_AV	LLC_BIT24
#define LLC_MASK_ARFIFO_1QS_AV	LLC_BIT25

//----------------------------------------------------------------------------
// ASY_TX_STAT_REG_DP Asynchronous Transmission Status Reg.
#define LLC_ASY_TX_CMPLT		LLC_BIT0	// Asyn transmission complete (Ack received)
#define LLC_ASY_TX_STATUS_MASK	(LLC_BIT1|LLC_BIT2)
#define LLC_ASY_TX_ST_IDLE		0			// Asyn tx. idle
#define LLC_ASY_TX_ST_ARB		LLC_BIT1	// Asyn tx. arbitrating
#define LLC_ASY_TX_ST_TRANS		LLC_BIT2	// Asyn tx. transmitting
#define LLC_ASY_TX_ST_RETRY		(LLC_BIT1|LLC_BIT2) // Asyn tx. retry pending
#define LLC_RETRY_ABORT			LLC_BIT3	// If set then Abort retry state
#define LLC_RETRY_CODE_MASK 	(LLC_BIT4|LLC_BIT5)
#define LLC_RETRY_CODE_NORMAL 	0
#define LLC_RETRY_CODE_X		LLC_BIT4
#define LLC_ASY_TX_EVENT_MASK	0x001F0000	// See ASY_TX_EVENT codes
#define LLC_ASY_TX_EVENT_SHIFT	16	

// LLC_ASY_TX_EVENT see also defined IEEE 1394a Acknowledge Codes (0x00 - 0x0F)

#define LLC_EVENT_INIT			0x00	// Termination of async packet with no ack (e.g. bcast, phy, async stream)
#define LLC_EVT_TCODE_ERR		0x10	// Bad tcode found in the asy_tx_fifo
#define LLC_ACK_MISSING_ERR		0x13	// A sub action gab was detected before ack
#define LLC_EVT_UNDERRUN		0x14	// Host did not supply data as specified in the pay load	
#define LLC_EVT_PAR_ERROR		0x1B	// Received ack with parity error
#define LLC_EVT_RETRY_FAIL		0x1C	// Retry counter has rolled over
#define LLC_EVT_ABORT_RTRY		0x1E	// Received abort command from the host and term. retry
#define LLC_EVT_FLUSHED			0x1F	// Termination due to bus reset
		
//----------------------------------------------------------------------------
// PHY_CTRL_REG_DP Phy Control Reg.
#define LLC_RD_DONE				LLC_BIT0	// Set when a transfer is received from PHY
#define LLC_RD_ADDR_MASK		0x0F000000	// Address of the register most recently rcv. from PHY	
#define LLC_RD_ADDR_SHIFT		24
#define LLC_RD_DATA_MASK		0x00FF0000	// Register content most recently rcv. from PHY
#define LLC_RD_DATA_SHIFT		16
#define LLC_RD_REG				LLC_BIT16	// Set to initiate a read request to the PHY
#define LLC_WR_REG				LLC_BIT17	// Set to initiate a write request to the PHY
#define LLC_REG_ADDR_MASK		0x0000F00	// Address to the PHY reg. to be written or read
#define LLC_REG_ADDR_SHIFT		8
#define LLC_WR_DATA_MASK		0x00000FF	// Contents of register to be written into PHY 

//----------------------------------------------------------------------------
#define LLC_INT_ITEMS			32

// INTERRUPT_REG_DP
// INTERRUPT_REG_SET_DP
// INTERRUPT_REG_CLEAR_DP
// INTR_MASK_REG_SET_DP
// INTR_MASK_REG_CLEAR_DP
#define LLC_ALL_INTR_MASK			0xFFFFFFFF



#define LLCID_INTR_OUT					31		// ORing of all interrupts - if set then intr. pending
#define LLCID_PHY_INTR					30		// Status change in PHY - read register in PHY to read status
#define LLCID_PHY_REG_RXD				29		// PHY received a register data. Read data from PHY control reg.
#define LLCID_BUS_RST_RXD				28		// PHY received a reset status from the bus.
#define LLCID_ARB_RST_GAB				27		// Arb reset gan observed
#define LLCID_BUS_INIT_OVR			26		// Bus initialization process is over
#define LLCID_PING_INTR					25		// Ping response receive or ping error
#define LLCID_ASY_TX_CMPL				24		// Tx has completed the current asyn packet. Ready for next.
#define LLCID_ISO_TX_CMPL				23		// Tx has completed the one iso stream packet. Ready for next.
#define LLCID_ASY_RX_PKT_AVL		22		// Proper asyn packet without error is received (In AR fifo)
#define LLCID_RST_CMND_RX				21		// Reset command is received - appl reset 
#define LLCID_ATFIFO_STAT				20		// Asyn TX FIFO status changed
#define LLCID_ITFIFO_STAT				19		// Iso TX FIFO status changed
#define LLCID_ARFIFO_STAT				18		// Asyn RX FIFO status changed
#define LLCID_IRFIFO_STAT				17		// Iso RX FIFO status chagned
#define LLCID_DUB_EXPC_OCC_INTR	16		// Status changed due to bits in the DUB_EXPC reg.
#define LLCID_ISO_RX_PKT_AVL		15		// Isochronous receive packet arrived
#define LLCID_TX_BUSY_ACK				14		// Link sending a busy acknowledge
#define LLCID_HEADER_ERR				13		// Header error observed in the Asyn rx packet
#define LLCID_TCODE_ERR					12		// tcode error observed in the Asyn rx packet
#define LLCID_ASY_DATA_ERR			11		// Data CRC/length error in the Asyn rx packet
#define LLCID_INC_SEC_COUNT			10		// Cycle count reaches 7999 (1 sec.)
#define LLCID_CY_START					9			// Cycle start send or received
#define LLCID_CY_DONE						8			// Isochronous cycle is completed
#define LLCID_CY_TOO_LONG				7			// Iso cycle has exceeded the limit
#define LLCID_CY_LOST						6			// Count rolls over twice without a cycle start
#define LLCID_ISO_TX_ERR				5			// Error in iso transmission
#define LLCID_ISO_RX_ERR				4			// Mismatch in the data length of the iso rx
#define LLCID_HOST_ACSS_ERR			3			// Error access to non avaiable FIFO or during retry
#define LLCID_ISO_HDR_RCVD			2			// Isoch stream packet header received
#define LLCID_NO_BUS_GRANT			1			// Could not request bus - lost sync. with PHY or speed mismatch
#define LLCID_APP_GEN_INTR			0			// Interrupt generated by the application layer

#define LLC_INT_BIT(x) ((uint32)1<<(x))


#define LLC_INTR_OUT				LLC_INT_BIT(LLCID_INTR_OUT		)
#define LLC_PHY_INTR				LLC_INT_BIT(LLCID_PHY_INTR		)
#define LLC_PHY_REG_RXD				LLC_INT_BIT(LLCID_PHY_REG_RXD	)
#define LLC_BUS_RST_RXD				LLC_INT_BIT(LLCID_BUS_RST_RXD	)
#define LLC_ARB_RST_GAB				LLC_INT_BIT(LLCID_ARB_RST_GAB	)
#define LLC_BUS_INIT_OVR			LLC_INT_BIT(LLCID_BUS_INIT_OVR	)
#define LLC_PING_INTR				LLC_INT_BIT(LLCID_PING_INTR		)
#define LLC_ASY_TX_CMPL				LLC_INT_BIT(LLCID_ASY_TX_CMPL	)
#define LLC_ISO_TX_CMPL				LLC_INT_BIT(LLCID_ISO_TX_CMPL	)
#define LLC_ASY_RX_PKT_AVL			LLC_INT_BIT(LLCID_ASY_RX_PKT_AVL)
#define LLC_RST_CMND_RX				LLC_INT_BIT(LLCID_RST_CMND_RX	)
#define LLC_ATFIFO_STAT				LLC_INT_BIT(LLCID_ATFIFO_STAT	)
#define LLC_ITFIFO_STAT				LLC_INT_BIT(LLCID_ITFIFO_STAT	)
#define LLC_ARFIFO_STAT				LLC_INT_BIT(LLCID_ARFIFO_STAT	)
#define LLC_IRFIFO_STAT				LLC_INT_BIT(LLCID_IRFIFO_STAT	)
#define LLC_DUB_EXPC_OCC_INTR		LLC_INT_BIT(LLCID_DUB_EXPC_OCC_INTR	)
#define LLC_ISO_RX_PKT_AVL			LLC_INT_BIT(LLCID_ISO_RX_PKT_AVL)
#define LLC_TX_BUSY_ACK				LLC_INT_BIT(LLCID_TX_BUSY_ACK	)
#define LLC_HEADER_ERR				LLC_INT_BIT(LLCID_HEADER_ERR	)
#define LLC_TCODE_ERR				LLC_INT_BIT(LLCID_TCODE_ERR		)
#define LLC_ASY_DATA_ERR			LLC_INT_BIT(LLCID_ASY_DATA_ERR	)
#define LLC_INC_SEC_COUNT			LLC_INT_BIT(LLCID_INC_SEC_COUNT	)
#define LLC_CY_START				LLC_INT_BIT(LLCID_CY_START		)
#define LLC_CY_DONE					LLC_INT_BIT(LLCID_CY_DONE		)
#define LLC_CY_TOO_LONG				LLC_INT_BIT(LLCID_CY_TOO_LONG	)
#define LLC_CY_LOST					LLC_INT_BIT(LLCID_CY_LOST		)
#define LLC_ISO_TX_ERR				LLC_INT_BIT(LLCID_ISO_TX_ERR	)
#define LLC_ISO_RX_ERR				LLC_INT_BIT(LLCID_ISO_RX_ERR	)
#define LLC_HOST_ACSS_ERR			LLC_INT_BIT(LLCID_HOST_ACSS_ERR	)
#define LLC_ISO_HDR_RCVD			LLC_INT_BIT(LLCID_ISO_HDR_RCVD	)
#define LLC_NO_BUS_GRANT			LLC_INT_BIT(LLCID_NO_BUS_GRANT	)
#define LLC_APP_GEN_INTR			LLC_INT_BIT(LLCID_APP_GEN_INTR	)




//----------------------------------------------------------------------------
// BUS_STAT_REG_DP Bus Status Reg.
#define LLC_ND_ID_OVR					LLC_BIT0	// Node identification is over
#define LLC_ROOT_ID_ERR				LLC_BIT1	// There is a root with L bit = 0.
#define LLC_ND_ID_ERR					LLC_BIT2	// Node id is having error
#define LLC_TIME_OUT					LLC_BIT3	// Config timout (loop in cable)
#define LLC_ROOT_ID_OVR				LLC_BIT4	// Root identification is over
#define LLC_SELF_ID_INCR_ERR	LLC_BIT5	// Counts are missing in the self_id count
#define LLC_SEL_ID_PAR_ERR		LLC_BIT6	// Parity error in the self_id packtes
#define LLC_NO_ERR						LLC_BIT7	// All self_ids received successfully
#define LLC_RESET_COUNT_MASK	0x00F00000	// Bus reset count value
#define LLC_RESET_COUNT_SHIFT	20			
#define LLC_SELFID_RX_FAIL		LLC_BIT12	// Self_id packet not received AR FIFO is full
#define LLC_NO_IRM_NODE				LLC_BIT13	// No isochronous resource manager capable
#define LLC_IRM_ID_MASK				0x0003F000	// Isochronous resource manager ID
#define LLC_IRM_ID_SHIFT			12
#define LLC_ROOT_ID_MASK			0x00000FC0	// Root ID
#define LLC_ROOT_ID_SHIFT			6
#define LLC_ND_ID_MAKS				0x0000003F	// Node ID

//----------------------------------------------------------------------------
// ASY_TX_FIFO_SPACE_REG_DP
// ISO_TX_FIFO_SPACE_REG_DP
#define LLC_TX_SPC_AVAIL_MASK	0x000003FF	// Number of quadlet space left in the tx FIFO

//----------------------------------------------------------------------------
// ASY_RX_FIFO_QLETS_REG_DP
// ISO_RX_FIFO_QLETS_REG_DP
#define LLC_RX_QLT_AVAIL_MASK	0x000003FF	// Number of data quadlets available in rx FIFO

//----------------------------------------------------------------------------
// ASY_TX_FIFO_DEPTH_REG_DP
// ASY_RX_FIFO_DEPTH_REG_DP
// ISO_TX_FIFO_DEPTH_REG_DP
// ISO_RX_FIFO_DEPTH_REG_DP
#define LLC_FIFO_DEPTH_MASK		0x000003FF	// Size of the rx or tx FIFO depth in quadlets
#define LLC_MAX_DEPTH			0x200		// Maximum FIF0 depth is 512 quadlets
#ifndef LLC_ASY_RX_DEPTH                    // This can be defined in the Make.params
#define LLC_ASY_RX_DEPTH		(0x100) //256 quads = 1024 bytes
#endif
#define LLC_ASY_TX_DEPTH		(LLC_MAX_DEPTH-LLC_ASY_RX_DEPTH) //0x100 = 1024 bytes
#define LLC_ASY_DEPTH			(LLC_ASY_RX_DEPTH)

//----------------------------------------------------------------------------
// HST_ACC_ERR_REG_DP Host Access Error Reg.
#define LLC_ASY_TX_FIFO_VIOL	LLC_BIT0	// Write access violation to the asy_tx_fifo
#define LLC_FIFO_ALLOC_ERR		LLC_BIT1	// FIFO allocation error (Non available FIFO depths)
#define LLC_FIFO_ALLOC_ACC_ERR	LLC_BIT2	// FIFO depth access without setting the enable bit in LCSR
#define LLC_FIFO_ACC_ERR		LLC_BIT3	// Try to read or write into FIFO with zero depth
#define LLC_PHY_ACC_ERR			LLC_BIT4	// Error in the access to the PHY due to missing write/read cmd
#define LLC_REG_ACC_ERR			LLC_BIT5	// Try to access a non existing or non permitted register

//----------------------------------------------------------------------------
// RET_CT_REG_DP Retry Count and Retry Time Reg.
#define LLC_RETRY_COUNT_MASK	0xE0000000	// Number ot times the LINK has to retry the asy packet
#define LLC_RETRY_COUNT_SHIFT	29
#define LLC_RETRY_TIME_MASK		0x1E000000	// Number of arbitration reset gab the LINK should retry
#define LLC_RETRY_TIME_SHIFT	25

//----------------------------------------------------------------------------
// LPS_REG_DP Link Power Status Reg. and Power Managment
#define LLC_HOST_LN2P_RESET		LLC_BIT0	// Reset link_to_phy interface
#define LLC_EN_LINK_ON_INTR		LLC_BIT1	// Enables generation of link on interrupt 
#define LLC_CLR_LINK_ON_INTR	LLC_BIT2	// Clear link on interrupt
#define LLC_EN_L2P_IF_ON_LNKON	LLC_BIT3	// Set bit before going into disable mode

// Xfer Status and time stamp
#define LLC_SPEED_MASK			0x00E00000
#define LLC_SPEED_SHIFT			21
#define LLC_ACK_MASK			0x001F0000
#define LLC_ACK_SHIFT			16
#define LLC_BUS_RESET_CNT_MASK  0x0F000000
#define LLC_BUS_RESET_CNT_SHIFT 24
#define LLC_TIME_STAMP_MASK		0x0000FFFF

#define LLC_TRAILER_PACKET		0xC0000000
#define LLC_TRAILER_MASK		0xF0000000

#define LLC_SELFID_PACKET		0x80000000
#define LLC_SELFID_MASK			0xC0000000
														// Define IEEE 1394 Async Transaction Codes
#define TCODE_PHY_PACKET		TCODE_LLC_SPECIFIC		// internally used by link designs

/* Define LLC CSR Bus Manager Resources - csrSel values */
typedef enum
{
	LLC_BUS_MANAGER_ID			= 0x00,
	LLC_BANDWIDTH_AVAILABLE		= 0x01,
	LLC_CHANNELS_AVAILABLE_HI	= 0x02,
	LLC_CHANNELS_AVAILABLE_LO	= 0x03
} LLC_CSR_BM_RSRC;

/* Define LLC CSR Architecture Space */
typedef enum
{
	LLC_ARCH_STATE_CLEAR		= 0x00,
	LLC_ARCH_STATE_SET			= 0x01,
	LLC_ARCH_NODEIDS			= 0x02,
	LLC_ARCH_RESET_START		= 0x03,
	LLC_ARCH_SPLIT_TO_HI		= 0x04,
	LLC_ARCH_SPLIT_TO_LO		= 0x05,
} LLC_CSR_ARCH;

/* Define LLC CSR Operation Types */
typedef enum _LLC_CSR_TYPE
{
	LLC_CSR_UNDEF				= 0x00,
	LLC_CSR_READ				= 0x01,
	LLC_CSR_WRITE				= 0x02,
	LLC_CSR_LOCK				= 0x03
} LLC_CSR_TYPE;

/****************************************************************************/

#endif //_LLCDEFS_H_



