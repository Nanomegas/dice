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
 * DESCRIPTION: Node CSR support

	Purpose:	

		This file exposes the ConfigROM API and defines CSR fields per
		the ISO/IEC 13213:1994 [ANSI/IEEE Std 1212, 1994 Edition] document.

	Revisions:
		created 04/11/2002 lm
****************************************************************************/

#ifndef _CSR_H
#define _CSR_H

#include "TCTypes.h"

#define CSR_ARCH_SPACE_START			0xF0000000
#define CSR_ARCH_SPACE_END				0xF00001FF
										
#define CSR_ARCH_STATE_CLEAR			0xF0000000
#define CSR_ARCH_STATE_SET				0xF0000004
#define CSR_ARCH_NODEIDS				0xF0000008
#define CSR_ARCH_RESET_START			0xF000000C
										
#define CSR_ARCH_SPLIT_HI				0xF0000018
#define CSR_ARCH_SPLIT_LO				0xF000001C
										
#define CSR_SB_SPACE_START				0xF0000200
#define CSR_SB_SPACE_END				0xF00003FF

#define CSR_SB_CYCLE_TIME				0xF0000200
#define CSR_SB_BUS_TIME					0xF0000204
#define CSR_SB_BUSY_TIME_OUT			0xF0000210

#define CSR_SB_FAIR_BUDGET				0xF0000218	//LM???
#define CSR_SB_BM_ID					0xF000021C
#define CSR_SB_BW_AVAIL					0xF0000220
#define CSR_SB_CH_AVAIL_HI				0xF0000224
#define CSR_SB_CH_AVAIL_LO				0xF0000228
#define CSR_SB_BROADCAST_CH				0xF0000234
#define CSR_NETWORK_CHANNELS			CSR_SB_BROADCAST_CH
										
#define CSR_ROM_SPACE_START				0xF0000400
#define CSR_ROM_BUS_INFO_1ST			0xF0000404		// bus identifier '1394'
#define CSR_ROM_BUS_INFO_2ND			0xF0000408		// bus capabilities
#define CSR_ROM_BUS_INFO_3RD			0xF000040C		// vendor_id and chip_id_hi
#define CSR_ROM_BUS_INFO_4TH			0xF0000410		// node_unique_id_lo
#define CSR_ROM_ROOT_DIR				0xF0000414
#define CSR_ROM_SPACE_END				0xF00007FF
#define CSR_ROM_MAX_SIZE				(CSR_ROM_SPACE_END-CSR_ROM_SPACE_START)

#define CSR_TOPOLOGY_MAP_START			0xF0001000
#define CSR_TOPOLOGY_MAP_END			0xF00013FC
										
#define CSR_SPEED_MAP_START				0xF0002000
#define CSR_SPEED_MAP_END				0xF0002FFC
										
/*==============================================================================
			BUS INFO BLOCK
*/

#define BUSINFO_IRMC					0x80000000		// 1=iso_resource_manager capable
#define BUSINFO_CMC						0x40000000		// 1=cycle_master capable
#define BUSINFO_ISC						0x20000000		// 1=support isochronous traffic
#define BUSINFO_BMC						0x10000000		// 1=bus_manager capable
#define BUSINFO_PMC						0x08000000		// 1=power_manager capable - 1394a supplement
#define BUSINFO_RESERVED1				0x04000000		// reserved
#define BUSINFO_RESERVED2				0x02000000		// reserved
#define BUSINFO_RESERVED3				0x01000000		// reserved
										
#define BUSINFO_NOCMC_ACC				0x00FF0000		// cycle master clock accuracy. if cmc=0
#define BUSINFO_CMC_ACC_32PPM			0x00200000		// cycle master clock accuracy(0-100)
#define BUSINFO_CMC_ACC_100PPM			0x06400000		// cycle master clock accuracy(0-100)
#define BUSINFO_CMC_ACC_0PPM			0x00000000		// cycle master clock accuracy(0-100)										
										
#define BUSINFO_MAXREC_NONE				0x00000000		// max receive size(async-packet) unspecified
#define BUSINFO_MAXREC					0x0000F000		// max receive size(async-packet),0001-1010 is 4-2048
#define BUSINFO_MAXREC4					0x00001000		// max size = 4 bytes
#define BUSINFO_MAXREC8					0x00002000		// max size = 8 bytes
#define BUSINFO_MAXREC16				0x00003000		// max size = 16 bytes
#define BUSINFO_MAXREC32				0x00004000		// max size = 32 bytes
#define BUSINFO_MAXREC64				0x00005000		// max size = 64 bytes
#define BUSINFO_MAXREC128				0x00006000		// max size = 128 bytes
#define BUSINFO_MAXREC256				0x00007000		// max size = 256 bytes
#define BUSINFO_MAXREC512				0x00008000		// max size = 512 bytes
#define BUSINFO_MAXREC1024				0x00009000		// max size = 1024 bytes
#define BUSINFO_MAXREC2048				0x0000A000		// max size = 2048 bytes
#define BUSINFO_MAXREC4096				0x0000B000		// max size = 4096 bytes - 1394a supplement
#define BUSINFO_MAXREC8192				0x0000C000		// max size = 8192 bytes - 1394a supplement
#define BUSINFO_MAXREC16384				0x0000D000		// max size = 16384 bytes - 1394a supplement
#define BUSINFO_MAXREC_RES1				0x0000E000		// max size reserved
#define BUSINFO_MAXREC_RES2				0x0000F000		// max size reserved
										
#define BUSINFO_MAX_ROM_4				0x00000000		// Config ROM quadlet
#define BUSINFO_MAX_ROM_64				0x00000100		// Config ROM quadlet and block reads up 64 bytes
#define BUSINFO_MAX_ROM_1024			0x00000200		// Config ROM quadlet and block reads up 1024 bytes
										
#define BUSINFO_GEN_INIT				0x00000010		// generation initializes to 1 (1394a-2000)
#define BUSINFO_GEN_MIN					0x00000020		// generation min value is 2 if modifiable (1394a-2000)
#define BUSINFO_GEN_MAX					0x000000F0		// generation max value is F (then wrap to min) (1394a-2000)

#define BUSINFO_MAX_SPD_100				0x00000000
#define BUSINFO_MAX_SPD_200				0x00000001
#define BUSINFO_MAX_SPD_400				0x00000002
#define BUSINFO_MAX_SPD_800				0x00000003
#define BUSINFO_MAX_SPD_1600			0x00000004
#define BUSINFO_MAX_SPD_3200			0x00000005

/*==============================================================================
				CONFIG ROM DIRECTORIES  (P1212 draft 1.0, 61883-1)

	First quadlet:
		Bits 16-31:	directory length, number of following quadlets
		Bits  0-15: directory crc
	Subsequent quadlets (directory entries)
		Bits 30-31:	dir entry type
		Bits 24-29: dir entry key
		Bits  0-23: dir entry value
*/
#define ROM_DIR_LEN(firstQuad)			(((firstQuad)>>16)&0xFFFF)
#define ROM_DIR_CRC(firstQuad)			((firstQuad)&0xFFFF)

// Rom Directory Key TYPES (in bits 30-31)
#define RDK_TYPE_MASK					(uint32) (BIT30 | BIT31)
#define	RDK_TYPE_IMMEDIATE				(uint32) (0 << 30)
#define RDK_TYPE_CSR_OFFSET				(uint32) (1 << 30)
#define	RDK_TYPE_LEAF					(uint32) (2 << 30)
#define	RDK_TYPE_DIRECTORY				(uint32) (3 << 30)

// Rom Directory Key IDs (in bits 24-29), names taken from p1212, 7.7
#define RDK_ID_DESCRIPTOR				(0x01 << 24)
#define RDK_ID_BUS_DEPENDENT_INFO		(0x02 << 24)
#define RDK_ID_VENDOR					(0x03 << 24)
#define RDK_ID_HARDWARE_VERSIOn			(0x04 << 24)
#define RDK_ID_MODULE					(0x07 << 24)
#define RDK_ID_NODE_CAPS				(0x0C << 24)
#define RDK_ID_EUI64					(0x0D << 24)
#define RDK_ID_UNIT						(0x11 << 24)
#define RDK_ID_SPECIFIER_ID				(0x12 << 24)
#define RDK_ID_VERSION					(0x13 << 24)
#define RDK_ID_DEPENDENT_INFO			(0x14 << 24)
#define RDK_ID_UNIT_LOCATION			(0x15 << 24)
#define RDK_ID_MODEL					(0x17 << 24)
#define RDK_ID_INSTANCE					(0x18 << 24)
#define RDK_ID_KEYWORD					(0x19 << 24)
#define RDK_ID_FEATURE					(0x1A << 24)
#define RDK_ID_EXTENDED_ROM				(0x1B << 24)
#define RDK_ID_EXTENDED_KEY_SPEC_ID		(0x1C << 24)
#define RDK_ID_EXTENDED_KEY				(0x1D << 24)
#define RDK_ID_EXTENDED_DATA			(0x1E << 24)
#define RDK_ID_MODIFIABLE_DESCRIPTOR	(0x1F << 24)
#define RDK_ID_DIRECTORY_ID				(0x20 << 24)

#define RDK_ID_HAL_VENDOR				(0x38 << 24)
#define RDK_ID_HAL_MODEL				(0x39 << 24)
#define RDK_ID_NC_VENDOR				(0x3A << 24)
#define RDK_ID_NC_MODEL					(0x3B << 24)

// Havi Key ID's, all defined within the context of a Havi Unit Directory (HAVi spec 1.0 dated Oct 31, 2000)
#define HAVI_DEVICE_PROFILE_KEY			(0x38 << 24)
#define HAVI_DCM_KEY					(0x39 << 24)
#define HAVI_DCM_REFERENCE_KEY			(0x3A << 24)
#define HAVI_DCM_PROFILE_KEY			(0x3B << 24)
#define HAVI_DEVICE_ICON_BITMAP_KEY		(0x3C << 24)
#define HAVI_MESSAGE_VERSION_KEY		(0x3D << 24)
										
// define Version ID's
#define UNIT_SPEC_ID_61883				0x0000A02D
#define UNIT_SPEC_ID_YAMAHA				0x0000A0DE
#define UNIT_SPEC_ID_TC					0x00000166

#define UNIT_SW_VERSION_AVC				0x00010001
#define UNIT_SW_VERSION_HAVI			0x00010008
#define UNIT_SW_VERSION_VENDOR			0x00004000
#define UNIT_SW_VERSION_MLAN			0x00FFFFFF

#define	UNIT_ID_MODEL					0x00000001

// define Node_Capabilities bits
#define NODECAP_SPT						0x00008000		// SPLIT_TIMEOUT.reg is implemented. 1394required
#define NODECAP_MS						0x00004000		// message-passing.reg is implemented. optional
#define NODECAP_INT						0x00002000		// INTERRUPT_TARGET/MASK.reg implemented. optional
#define NODECAP_EXT						0x00001000		// ARGUMENT.reg is implemented. optional
#define NODECAP_BAS						0x00000800		// TEST_START/STATUS.reg is implemented. optional
#define NODECAP_PRV						0x00000400		// implements the private space. 1394required
#define NODECAP_64						0x00000200		// uses 64-bit addressing. 1394required
#define NODECAP_FIX						0x00000100		// uses the fix addressing. 1394required
#define NODECAP_LOST					0x00000080		// STATE_BITS.lost bit is implemented. 1394required,see 8.3.2.1
#define NODECAP_DREQ					0x00000040		// STATE_BITS.dreq bit is implemented. 1394optional
#define NODECAP_ELOG					0x00000010		// STATE_BITS.elog bit is implemented. 1394optional
#define NODECAP_ATN						0x00000008		// STATE_BITS.atn bit is implemented. 1394nouse
#define NODECAP_OFF						0x00000004		// STATE_BITS.off bit is implemented. 1394nouse
#define NODECAP_DEAD					0x00000002		// supports the dead state. optional
#define NODECAP_INIT					0x00000001		// supports the intializing state. optional

#define NODE_MIN_CAPS					(NODECAP_SPT | NODECAP_PRV | NODECAP_64 | NODECAP_FIX | NODECAP_LOST | NODECAP_DREQ)

uint32		csrGetConfigROMAddress(void);												// return base address of Configuration ROM
uint32		csrGetConfigROMQuadletSize(void);										// return number of quadlet values in Configuration ROM
QUADLET		csrReadConfigROMQuadlet(uint32 offset);									// read quadlet value in Configuration ROM based on 1394 offset
HRESULT		csrBuildConfigROM(uint32 chip_id_low, uint32 chip_id_hi);				// generate the Configuration ROM based on chip_id
HRESULT		csrUpdateConfigROM(uint32 *data, uint32 size);							// complete update the Configuration ROM 
BOOL 		csrIsCMCCapable(void);													// returns true if cycle_master capable

#endif
