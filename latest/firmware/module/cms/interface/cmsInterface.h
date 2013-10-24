//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004-2007 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************
	
	Purpose:	DICE  Enhanced Application Protocol Interface
	
	Revisions:
		created 19/02/2007 ml
****************************************************************************/
/** @addtogroup CMS

	Exposes a interface for a Connection Mamagement System Protocol for configuring connections
	between devices in a network.

	This module can be used from the command prompt by the @ref cli_cms commands.

	The CMS module takes advantage of DAL and AVS. The main application 
	configures CMS and pass on information from DAL,
	@{
*/

#ifndef _CMS_INT_H_
#define _CMS_INT_H_

#include "TCTypes.h"

#define CMS_SPEC_VERSION 0x00000001

typedef struct
{
	uint32					capabilityOffSet;
	uint32					capabilitySize;
	uint32					globalParOffSet;
	uint32					globalParSize;
	uint32					txParOffSet;
	uint32					txParSize;
	uint32					rxParOffSet;
	uint32					rxParSize;
	uint32					rxRouteOffSet;
	uint32					rxRouteSize;
	uint32					rxCommOffSet;
	uint32					rxCommSize;
	uint32					generalStatOffSet;
	uint32					generalStatSize;
	uint32					txStatOffSet;
	uint32					txStatSize;
	uint32					rxStatOffSet;
	uint32					rxStatSize;
	uint32					xmlOffSet;
	uint32					xmlSize;
	uint32					appSpaceOffset;
	uint32					appSpaceSize;
	uint32					lastIsZero;
}CMS_OFFSET_STRUCT;


typedef struct
{
	uint32					specVersion;
	uint32					generalCaps;
	uint32					nbTx;
	uint32					nbRx;
	uint32					nbOutputs;
	uint32					nbComms;
	uint32					reserved;
} CMS_CAPABILITIES;


// Defines for generalCaps bit fields
#define CMS_CAP_MAXRATE_MSK			0x00000003
#define CMS_CAP_MAXRATE_SHFT		0x00000000
#define CMS_CAP_MAXRATE48K			0x00000000
#define CMS_CAP_MAXRATE96K			0x00000001
#define CMS_CAP_MAXRATE192K			0x00000002

#define CMS_NICKNAME_MAX	64

typedef struct
{
	char					nickname[CMS_NICKNAME_MAX];
	uint32					syncSource;
	uint32					rateMode;
	uint32					indicate;
	uint32					com0Setup;
	uint32					com1Setup;
} CMS_GLOBAL_PAR;

//comm setup defines
#define CMS_COMM_BAUD_MSK		0x00f
#define CMS_COMM_BAUD_SHFT		0
#define CMS_COMM_PARITY_MSK		0x030
#define CMS_COMM_PARITY_SHFT	4
#define CMS_COMM_BPC_MSK		0x0c0
#define CMS_COMM_BPC_SHFT		6

#define CMS_COMM_STPB_12		0x100
#define CMS_COMM_ENABLE			0x200

#define CMS_COMM_BAUD_1K2		0
#define CMS_COMM_BAUD_2K4		1
#define CMS_COMM_BAUD_4K8		2
#define CMS_COMM_BAUD_9K6		3
#define CMS_COMM_BAUD_19K2		4
#define CMS_COMM_BAUD_38K4		5
#define CMS_COMM_BAUD_57K6		6
#define CMS_COMM_BAUD_115K2		7
#define CMS_COMM_BAUD_MIDI		15

#define CMS_COMM_PARITY_NONE	0
#define CMS_COMM_PARITY_EVEN	1
#define CMS_COMM_PARITY_ODD		2

#define CMS_COMM_BPC_5			0
#define CMS_COMM_BPC_6			1
#define CMS_COMM_BPC_7			2
#define CMS_COMM_BPC_8			3

#define CMS_SYNC_SRC_INT32K		0
#define CMS_SYNC_SRC_INT44K1	1
#define CMS_SYNC_SRC_INT48K		2
#define CMS_SYNC_SRC_INT88K2	3
#define CMS_SYNC_SRC_INT96K		4
#define CMS_SYNC_SRC_INT176K4	5
#define CMS_SYNC_SRC_INT192K	6
#define CMS_SYNC_SRC_EXT		16
#define CMS_SYNC_SRC_SYT		32

#define CMS_RATE_32K_48K		0
#define CMS_RATE_88K2_96K		2
#define CMS_RATE_176K4_192K		3


#define CMS_ISOC_DISABLE 0xff


typedef struct
{
	uint32					isocCh;
	uint32					audioCfg;
	uint32					commCfg;
	uint32					reserved;
} CMS_TX_PAR;


typedef struct
{
	uint32					isocCh;
	uint32					audioChs;
	uint32					commChs;
	uint32					reserved;
} CMS_RX_PAR;

typedef struct
{
	uint32					lockInfo;
	uint32					aesLockInfo;
	uint32					adatLockInfo;
	uint32					reserved;
} CMS_GENERAL_STAT;

typedef struct
{
	uint32					audioChs;
	uint32					commChs;
	uint32					state;
	uint32					reserved;
} CMS_TX_STAT;

typedef struct
{
	uint32					state;
} CMS_RX_STAT;

#define CMS_MAX_FILE_NAME_LENGTH 256
typedef struct
{
	uint32					OUI;
	uint32					PID;
	char					xmlFile[CMS_MAX_FILE_NAME_LENGTH];
	uint32					reserved; //must be 0;
} CMS_XML;

	


#define CMS_BASE_START_HI	0xffff
#define CMS_BASE_START		0xe0000000




//
#endif  //_CMS_INT_H_
