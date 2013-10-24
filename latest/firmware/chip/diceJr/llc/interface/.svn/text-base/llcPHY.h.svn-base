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
	Purpose:	Defines for Phy Layer Chip (LLC)	

	Revisions:
		created 04/29/2002 kbj

****************************************************************************/

#ifndef _LLCPHY_H_
#define _LLCPHY_H_


/* Define PHY chip register addresses */

#define	PHY_ID_ADR				0x00		/* Physical ID, R, CPS					*/
#define	PHY_GC_ADR				0x01		/* RHB, IBR, Gap_Count					*/
#define	PHY_NP_ADR				0x02		/* Extended, Num_Ports					*/
#define PHY_SPEED				0x03		/* PHY_Speed, Delay						*/
#define PHY_LC_PWR				0x04        /* Link active, Contender, Pwr_Class 	*/
#define PHY_BUS_STATUS			0x05		/* RPIE,ISBR,CTOI,CPSI,STOI,PEI,EAA,EMC	*/
#define PHY_RSVD				0x06		/* reserved								*/
#define PHY_PAGE_SELECT			0x07		/* Page_Select, Port_Select				*/

/* Page_Select is 0 */
#define PHY_P0_PORT_STATUS_0	0x08		/* AStat, Bstat, Ch, Con, Bias, Dis		*/
#define PHY_P0_PORT_STATUS_1	0x09		/* Peer_speed, PIE, Fault,				*/

/* Page_Select is 1 */
#define PHY_P1_VENDOR_ID_COMPL	0x08		/* Compliance Level (01, 1394a)			*/
#define PHY_P1_VENDOR_ID_RSVD	0x09		/* reserved								*/
#define PHY_P1_VENDOR_ID_0		0x0a		/* Vendor_ID 0, msb						*/
#define PHY_P1_VENDOR_ID_1		0x0b		/* Vendor_ID 1							*/
#define PHY_P1_VENDOR_ID_2		0x0c		/* Vendor_ID 2, lsb						*/
#define PHY_P1_PRODUCT_ID_0		0x0d		/* Product_ID 0, msb					*/
#define PHY_P1_PRODUCT_ID_1		0x0e		/* Product_ID 1							*/
#define PHY_P1_PRODUCT_ID_2		0x0f		/* Product_ID 2, lsb					*/

/* Page Select is 7 */
#define PHY_P7_NPA_LINK_SPEED	0x08		/* Null Packet Actions Flag, Link_Speed	*/


/* Define Phy address 0000b register masks */
#define PHY_ID_PHYSICALID		0xfC		/* node physical id						*/
#define PHY_ID_ROOT		        0x02		/* root bit								*/
#define PHY_ID_CPS				0x01		/* cable power status					*/

/* Define Phy address 0001b register masks */
#define PHY_GC_RHB				0x80		/* root holdoff bit						*/
#define PHY_GC_IBR				0x40		/* initiatebus reset					*/
#define PHY_GC_GAP_COUNT		0x3f		/* gap count							*/

/* Define Phy address 0010b register masks */
#define PHY_NP_ADR_EXTENDED		0xe0		/* 111 means the ext reg set present	*/
#define PHY_NP_ADDR_NUM_PORTS	0x0f

/* define Phy address 0011b register masks */
#define PHY_SPEED_SPEED			0xe0		/* 010b for TI Phy indicating S400		*/
#define PHY_SPEED_DELAY			0x0f		/* 000b for TI Phy indicating 144ns		*/

/* define Phy address 0100b register masks */
#define PHY_LC_PWR_L			0x80		/* Link-active status					*/
#define PHY_LC_PWR_C			0x40		/* Contender status						*/
#define PHY_LC_PWR_JITTER		0x38		/* PHY repeater jitter, 000b 20ns		*/ 
#define PHY_LC_PWR_PWR_CLASS	0x07		/* see Power Class Codes table below	*/

/* define Phy address 0101b register masks */
#define PHY_BUS_STATUS_RPIE		0x80		/* resuming port interrupt enable		*/
#define PHY_BUS_STATUS_ISBR		0x40		/* initiate short bus arbitrated reset	*/
#define PHY_BUS_STATUS_CTOI		0x20		/* configuration timeout interrupt		*/
#define PHY_BUS_STATUS_CPSI		0x10		/* cable power status interrupt			*/
#define PHY_BUS_STATUS_STOI		0x08		/* state timeout interrupt				*/
#define PHY_BUS_STATUS_PEI		0x04		/* port event interrupt					*/
#define PHY_BUS_STATUS_EAA		0x02		/* enable accelerated arbitration		*/
#define PHY_BUS_STATUS_EMC		0x01		/* enable multispeed concat'd packets	*/

/* define Phy address 0110b register masks */
#define PHY_PAGE_SELECT_PAGE	0xe0		/* page select							*/
#define PHY_PAGE_SELECT_PORT	0x0f		/* port select							*/

/* define Phy address 1000b Page 0 register fields */
#define PHY_P0_PS1_ASTAT		0xc0		/* TPA Line state						*/
#define PHY_P0_PS1_BSTAT		0x30		/* TPB Line state						*/
#define PHY_P0_PS1_CH			0x08		/* child/parent status					*/
#define PHY_P0_PS1_CON			0x04		/* debounced port connection status		*/
#define PHY_P0_PS1_BIAS			0x02		/* debounced incoming cable bias status	*/
#define PHY_P0_PS1_DIS			0x01		/* port disabled control				*/

/* define Phy address 1001b Page 0 register fields */
#define PHY_P0_PS2_PEER_SPEED	0xe0		/* port peer speed						*/
#define PHY_P0_PS2_PIE			0x10		/* port event interrupt enable			*/
#define PHY_P0_PS2_FAULT		0x04		/* resume or suspend fault on sel'd port*/

/* Define Phy address 1000b Page 7 register fields */
#define PHY_P7_NPA				0x80		/* null-packet actions flag				*/
#define PHY_P7_LS				0x03		/* link speed 10b for S400              */


/* Define Power Class Codes */
#define PHY_PWR_NOSINK_NOREPEAT	0x00		/* does not need or repeat power		*/
#define PHY_PWR_SELF_SRC15W		0x01		/* self-powered, provides 15W min to bus*/
#define PHY_PWR_SELF_SRC30W		0x02		/* self-powered, provides 30W min to bus*/
#define PHY_PWR_SELF_SRC45W		0x03		/* self-powered, provides 45W min to bus*/
#define PHY_PWR_POSS_BUS_SINK3W	0x04		/* may be bus powered, using up to 3W	*/
#define PHY_PWR_BUS_SINK3W		0x05		/* is bus powered, using up to 3W, +0WL	*/
#define PHY_PWR_BUS_SINK6W		0x06		/* is bus powered, using up to 3W, +3WL	*/
#define PHY_PWR_BUS_SINK10W		0x07		/* is bus powered, using up to 3W, +7WL	*/

#endif	/* _OHCIPHY_H_ */
