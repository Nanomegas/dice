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
	
	Purpose:	DICE  Driver Interface. This file defines the data structures
				used between the DiceDriver firmware and the PC or Mac driver.
	Revisions:
		created 01/03/2005 ml
****************************************************************************/

#ifndef _DICE_DRIVER_INT_H_
#define	_DICE_DRIVER_INT_H_

#include "TCTypes.h"
#include "diceDriverExtStatus.h"
	
// driver notification bit definitions
#define	DD_NOTIFY_RX_CFG_CHG_BIT	0x00000001
#define	DD_NOTIFY_TX_CFG_CHG_BIT	0x00000002
#define	DD_NOTIFY_DUP_ISOC_BIT		0x00000004
#define	DD_NOTIFY_BW_ERR_BIT		0x00000008
#define	DD_NOTIFY_LOCK_CHG_BIT		0x00000010
#define	DD_NOTIFY_CLOCK_ACP			0x00000020
#define	DD_INTERFACE_CHG_BIT		0x00000040

// DD_NOTIFY_RESERVED(1-4) are reserved bits for future system wide use. 
//
// When DD_NOTIFY_MESSAGE is set DD_NOTIFY_USER4 through DD_NOTIFY_USER11 are defined as an 8 bit message
// so you can have 256 seperate messages (like gray encoder movements).
//
#define	DD_NOTIFY_RESERVED1		0x00010000
#define	DD_NOTIFY_RESERVED2		0x00020000
#define	DD_NOTIFY_RESERVED3		0x00040000
#define	DD_NOTIFY_RESERVED4		0x00080000
#define	DD_NOTIFY_MESSAGE		0x00100000
#define	DD_NOTIFY_USER1			0x00200000
#define	DD_NOTIFY_USER2			0x00400000
#define	DD_NOTIFY_USER3			0x00800000
#define	DD_NOTIFY_USER4			0x01000000
#define	DD_NOTIFY_USER5			0x02000000
#define	DD_NOTIFY_USER6			0x04000000
#define	DD_NOTIFY_USER7			0x08000000
#define	DD_NOTIFY_USER8			0x10000000
#define DD_NOTIFY_USER9			0x20000000
#define	DD_NOTIFY_USER10		0x40000000
#define	DD_NOTIFY_USER11		0x80000000

#define DD_NOTIFY_ALL_USER		0xFFFF0000
#define DD_NOTIFY_ALL			0xFFFFFFFF

// clock source defines
#define	DD_CLK_SRC_AES1		0x0000	
#define	DD_CLK_SRC_AES2		0x0001	
#define	DD_CLK_SRC_AES3		0x0002
#define	DD_CLK_SRC_AES4		0x0003
#define	DD_CLK_SRC_AES_ANY	0x0004
#define	DD_CLK_SRC_ADAT		0x0005
#define	DD_CLK_SRC_TDIF		0x0006	//only on DICE II
#define	DD_CLK_SRC_ADATAUX	0x0006	//only on DICE JR/Mini
#define	DD_CLK_SRC_WC		0x0007
#define	DD_CLK_SRC_ARX1		0x0008
#define	DD_CLK_SRC_ARX2		0x0009
#define	DD_CLK_SRC_ARX3		0x000A	//only on DICE II
#define	DD_CLK_SRC_ARX4		0x000B	//only on DICE II
#define	DD_CLK_SRC_INTERNAL	0x000C

// nominal rate defines
#define	DD_RATE_32K			0x0000	//be careful changing these defines
#define	DD_RATE_441K		0x0001	//they are asumed to be alligned with
#define	DD_RATE_48K			0x0002	//DAL_NOMINAL_RATE_ENUM
#define	DD_RATE_882K		0x0003
#define	DD_RATE_96K			0x0004
#define	DD_RATE_1764K		0x0005
#define	DD_RATE_192K		0x0006
#define	DD_RATE_ANY_LOW		0x0007
#define	DD_RATE_ANY_MID		0x0008
#define	DD_RATE_ANY_HIGH	0x0009
#define	DD_RATE_NONE 		0x000a


#define	DD_STATUS_SRC_LOCKED		0x00000001
#define	DD_STATUS_RATE_CONFLICT		0x00000002
#define DD_STATUS_NOM_RATE_MASK		0x0000ff00
#define DD_STATUS_NOM_RATE_SHIFT	8

#define NO_ENABLER_ADDRESS_HIGH		0xFFFF0000
#define NO_ENABLER_ADDRESS_LOW		0x00000000


#define DD_SIZE_OF_NICK_NAME	64
#define DD_SIZE_OF_NAMES		256
#define DD_SIZE_OF_CLK_SRC_NAME	256


#define DD_NUM_ISOC_TRANSMITTER	2
#define DD_NUM_ISOC_RECEIVER	4

#define DICE_DRIVER_BASE_START_HI	0xffff
#define DICE_DRIVER_BASE_START		0xe0000000
#define DICE_DRIVER_BASE_END_HI		0xffff
#define DICE_DRIVER_BASE_END		(DICE_DRIVER_BASE_START + sizeof(DICE_DRIVER_STRUCT))

#define DICE_DRIVER_SPEC_NO_A	1
#define DICE_DRIVER_SPEC_NO_B	0
#define DICE_DRIVER_SPEC_NO_C	12
#define DICE_DRIVER_SPEC_NO_D	0

#define DICE_DRIVER_SPECIFICATION_VERSION_NUMBER (	( DICE_DRIVER_SPEC_NO_A <<24 ) |\
													( DICE_DRIVER_SPEC_NO_B <<16 ) |\
													( DICE_DRIVER_SPEC_NO_C <<8  ) |\
													( DICE_DRIVER_SPEC_NO_D      ) )
#define DD_CP_RATE_32			0x00000001
#define DD_CP_RATE_44_1			0x00000002
#define DD_CP_RATE_48			0x00000004
#define DD_CP_RATE_88_2			0x00000008
#define DD_CP_RATE_96			0x00000010
#define DD_CP_RATE_176_4		0x00000020
#define DD_CP_RATE_192			0x00000040
#define DD_CP_SOURCE_AES1		0x00010000
#define DD_CP_SOURCE_AES2		0x00020000
#define DD_CP_SOURCE_AES3		0x00040000
#define DD_CP_SOURCE_AES4		0x00080000
#define DD_CP_SOURCE_AES_ANY	0x00100000
#define DD_CP_SOURCE_ADAT		0x00200000
#define DD_CP_SOURCE_TDIF		0x00400000	//only on DICE II
#define DD_CP_SOURCE_ADATAUX	0x00400000	//only on DICE JR/Mini
#define DD_CP_SOURCE_WC			0x00800000
#define DD_CP_SOURCE_ARX1		0x01000000
#define DD_CP_SOURCE_ARX2		0x02000000
#define DD_CP_SOURCE_ARX3		0x04000000	//only on DICE II
#define DD_CP_SOURCE_ARX4		0x08000000	//only on DICE II
#define DD_CP_SOURCE_INT		0x10000000


typedef enum
{
	kHighRatesUseDualWire			= 0x00000001,
	kSupportEmptyPacketsWithPayload	= 0x00000002,
	kSupportsEAP					= 0x00000004,
} DD_CAPABILITIES;


typedef struct
{
	uint32	High;
	uint32	Low;
}DD_OWNER;

typedef struct
{
	DD_OWNER	Owner;
	uint32		Notification;
	char		NickName[DD_SIZE_OF_NICK_NAME];
	uint32 		ClockSelect;
	uint32		Enable;
	uint32		Status;
	uint32		ExtendedStatus;
	uint32		SampleRate;
	uint32		Version;
	uint32		ClockCapabilities;
	char		ClockSourceNames[DD_SIZE_OF_CLK_SRC_NAME];

	// added in 1.0.11.0
	uint32		Capabilities;
	uint32		MaxFwSpeed;
	uint32		TxHwLatency;
	uint32		RxHwLatency;
	uint32		SortID;

}DD_GLOBAL_PARAMETERS;

typedef struct
{
	uint32	ChannelNumber;
	uint32	NumAudioSequences;
	uint32	NumMidiSequences;
	uint32	Speed;
	char    Names[DD_SIZE_OF_NAMES];
	uint32	AC3Capabilities;
	uint32	AC3Enable;
}DD_ISOC_TRANSMITTER;

typedef struct
{
	uint32	NumIsocTransmitter;
	uint32	IsocTransmitterSize;
	DD_ISOC_TRANSMITTER	IsocTransmitter[DD_NUM_ISOC_TRANSMITTER];
}DD_TX_PARAMETERS;

typedef struct
{
	uint32	ChannelNumber;
	uint32	FirstSequenceNumber;
	uint32	NumAudioSequences;
	uint32	NumMidiSequences;
	char	Names[DD_SIZE_OF_NAMES];
	uint32	AC3Capabilities;
	uint32	AC3Enable;
}DD_ISOC_RECEIVER;

typedef struct
{
	uint32	NumIsocReceiver;
	uint32	IsocReceiverSize;
	DD_ISOC_RECEIVER	IsocReceiver[DD_NUM_ISOC_RECEIVER];
}DD_RX_PARAMETERS;

typedef struct
{
	uint32 src;
	uint32 locked;
	uint32 nomRate;
	uint32 adatUser;
}DD_EXT_SYNC_INFO;

typedef struct
{
	uint32					GlobalParametersOffSet;
	uint32					GlobalParametersSize;
	uint32					TxParametersOffSet;
	uint32					TxParametersSize;
	uint32					RxParametersOffSet;
	uint32					RxParametersSize;
	uint32					Dummy1ParametersOffSet;
	uint32					Dummy1ParametersSize;
	uint32					Dummy2UpdateSpaceOffSet;
	uint32					Dummy2UpdateSpaceSize;
	DD_GLOBAL_PARAMETERS	GlobalParameters;
	DD_TX_PARAMETERS		TxParameters;	
	DD_RX_PARAMETERS		RxParameters;
	DD_EXT_SYNC_INFO		ExtSyncInfo;
}DICE_DRIVER_STRUCT;



#endif//_DICE_DRIVER_INT_H_
