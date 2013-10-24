//==============================================================================
//####TCAT-SOURCE-HEADER-BEGIN####
//
// This confidential and proprietary source may be used only as authorized
// by a licensing agreement from TC Applied Technologies.
//
// Copyright (c) 2004-2013 TC Applied Technologies
//                         a division of TC Group Americas Ltd.
//                         All rights reserved.
//
// Unauthorized use, duplication or distribution of this source is strictly
// prohibited by law.
//
// The entire notice above must be reproduced on all authorized copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies.
//
//####TCAT-SOURCE-HEADER-END####
//==============================================================================

#pragma once


namespace tcat
{
namespace dice
{

//\cond

///////////////////////////////////////////////////////////////////////////////
//
//	structures interfacing host software and firewire memory space
//
///////////////////////////////////////////////////////////////////////////////
//
//	SPACES
//
///////////////////////////////////////////////////////////////////////////////
///
/// device firewire memory spaces
///
enum SPACES
{
	kSpaceGlobal,
	kSpaceTx,
	kSpaceRx,

	kSpaces
};



///////////////////////////////////////////////////////////////////////////////
//
//	SPACE
//
///////////////////////////////////////////////////////////////////////////////
///
/// device firewire memory space definition
///
struct SPACE
{
	uint32 offset;	///< address space offset for this memory space definition
	uint32 size32;	///< address space size in quadlets for this memory space definition
};

//\endcond


///////////////////////////////////////////////////////////////////////////////
//
//	SEQUENCES
//
///////////////////////////////////////////////////////////////////////////////
///
/// ISOC audio/midi sequence definition
///
struct SEQUENCES
{
	uint32 audio;	///< number of embedded audio channels
	uint32 midi;	///< number of embedded midi channels
};



//\cond

///////////////////////////////////////////////////////////////////////////////
//
//	IO
//
///////////////////////////////////////////////////////////////////////////////
///
/// unified device I/O configuration template
///
template<int channels> struct IO
{
	uint32	isoc_channels;					///< number of isoc channels
	uint32	size32;							///< structure size in quadlets
	struct CHANNEL							///  ISOC layout structure
	{
		uint32	isoc_channel;				///< associated isoc channel number
		union
		{
			struct
			{
				SEQUENCES	sequences;		///< ISOC audio/midi sequence definition
				uint32		speed;			///< firewire speed
			} tx;
			struct
			{
				uint32		first_sequence;	///< start sequence number
				SEQUENCES	sequences;		///< ISOC audio/midi sequence definition
			} rx;
		};
		char    names[256];					///< big endian channel names separated by /
		uint32	ac3_caps;					///< AC3 capability channelbit map
		uint32	ac3_enable;					///< AC3 enabled
	} isoc[channels];						///< Isoc channel properties
};



///////////////////////////////////////////////////////////////////////////////
//
//	DD_GLOBAL
//
///////////////////////////////////////////////////////////////////////////////
///
/// device global firewire space
///
struct DD_GLOBAL
{
	uint64				owner;			///< device owner GUID - which network device has an active connection to that device
	DEVICE_NOTIFICATION	notification;	///< notification bit map - to inform driver about device state changes
	NICKNAME			nickname;		///< the device nickname
	CLOCK 				clock;			///< the current clock (sample rate) setting enum
	uint32				enable;			///< device enabled status
	STATUS				status;			///< device lock status
	EX_STATUS			ex_status;		///< device extended status bitmap
	uint32				sample_rate;	///< messured sample rate
	uint32				version;		///< interface version
	CLOCK_SYNC_CAPS		clock_sync_caps;///< combined bit map indicating supported clocks and sync sources
	SYNCNAME			sync_names;		///< device sync source names char array
};



///////////////////////////////////////////////////////////////////////////////
//
//	DD_GLOBAL
//
///////////////////////////////////////////////////////////////////////////////
///
///	Debug interface AVS error register offsets
///
enum APBA
{
	//\cond
	// Int INT0 bits
	kApbaInt0VrxCfgFail,
	kApbaInt0VrxCipFail,
	kApbaInt0VrxDbcFail,
	kApbaInt0VrxLongPkt,
	kApbaInt0VrxPktAbort,
	kApbaInt0VrxStatusError,
	kApbaInt0VtxBoundryError,
	kApbaInt0VtxFrameAgeout,
	kApbaInt0VtxPktAbort,
	kApbaInt0Ado0KeyReq,
	kApbaInt0Ado1KeyReq,
	kApbaInt0Ado2KeyReq,
	kApbaInt0Ado1Locked,
	kApbaInt0Ado1StreamEnd,
	kApbaInt0Ado1StreamStart,
	kApbaInt0Ado2Locked,
	kApbaInt0Ado2StreamEnd,
	kApbaInt0Ado2StreamStart,
	kApbaInt0Ado3Locked,
	kApbaInt0Ado3StreamEnd,
	kApbaInt0Ado3StreamStart,
	kApbaInt0Ado4Locked,
	kApbaInt0Ado4StreamEnd,
	kApbaInt0Ado4StreamStart,
	kApbaInt0Atx1StreamEnd,
	kApbaInt0Atx1StreamStart,
	kApbaInt0Atx2StreamEnd,
	kApbaInt0Atx2StreamStart,
	kApbaInt0VdoStreamEnd,
	kApbaInt0VdoStreamStart,
	kApbaInt0VtxStreamEnd,
	kApbaInt0VtxStreamStart,

	// Int INT1 bits
	kApbaInt1Arxdo1SytOverflow,
	kApbaInt1Arxdo1SytUnderflow,
	kApbaInt1Arxdo2SytOverflow,
	kApbaInt1Arxdo2SytUnderflow,
	kApbaInt1Arxdo3SytOverflow,
	kApbaInt1Arxdo3SytUnderflow,
	kApbaInt1Arxdo4SytOverflow,
	kApbaInt1Arxdo4SytUnderflow,
	kApbaInt1AtxDI1SytOverflow,
	kApbaInt1AtxDI1SytUnderflow,
	kApbaInt1AtxDI2SytOverflow,
	kApbaInt1AtxDI2SytUnderflow,
	kApbaInt1VrxdoSytOverflow,
	kApbaInt1VrxdoSytUnderflow,
	kApbaInt1VtxdiSytOverflow,
	kApbaInt1VtxdiSytUnderflow,
	kApbaInt1Ado1Repeat,
	kApbaInt1Ado1Slip,
	kApbaInt1Ado1SytAgeout,
	kApbaInt1Ado2Repeat,
	kApbaInt1Ado2Slip,
	kApbaInt1Ado2SytAgeout,
	kApbaInt1Ado3Repeat,
	kApbaInt1Ado3Slip,
	kApbaInt1Ado3SytAgeout,
	kApbaInt1Ado4Repeat,
	kApbaInt1Ado4Slip,
	kApbaInt1Ado4SytAgeout,
	kApbaInt1VdoAgeout,
//	kApbaInt1Reserved1,
//	kApbaInt1Reserved2,
//	kApbaInt1Reserved3,

	// Int INT2 bits
	kApbaInt2ItpEpTooBig,
	kApbaInt2Arx1CfgFail,
	kApbaInt2Arx1CipFail,
	kApbaInt2Arx1DbcFail,
	kApbaInt2Arx1LongPkt,
	kApbaInt2Arx1PktAbort,
	kApbaInt2Arx1StatusError,
	kApbaInt2Arx2CfgFail,
	kApbaInt2Arx2CipFail,
	kApbaInt2Arx2DbcFail,
	kApbaInt2Arx2LongPkt,
	kApbaInt2Arx2PktAbort,
	kApbaInt2Arx2StatusError,
	kApbaInt2Arx3CfgFail,
	kApbaInt2Arx3CipFail,
	kApbaInt2Arx3DbcFail,
	kApbaInt2Arx3LongPkt,
	kApbaInt2Arx3PktAbort,
	kApbaInt2Arx3StatusError,
	kApbaInt2Arx4CfgFail,
	kApbaInt2Arx4CipFail,
	kApbaInt2Arx4DbcFail,
	kApbaInt2Arx4LongPkt,
	kApbaInt2Arx4PktAbort,
	kApbaInt2Arx4StatusError,
	kApbaInt2Atx1BoundryError,
	kApbaInt2Atx1FrameAgeout,
	kApbaInt2Atx1PktAbort,
	kApbaInt2Atx2BoundryError,
	kApbaInt2Atx2FrameAgeout,
	kApbaInt2Atx2PktAbort,
//	kApbaInt2Reserved4,

	kApbaItems
	//\endcond
};

typedef uint32 AVS[kApbaItems];		///< AVS error registers


///////////////////////////////////////////////////////////////////////////////
//
///	Firmware Information structures
//
///////////////////////////////////////////////////////////////////////////////
//
/// These structures are to be found in the firmware binary in little endian 
///	format. Older firmwares will use the tcat::dice::VERSION struct which is 
/// now depriciated due to the string format of certain members which could not 
///	be statically initialized.
/// tcat::pal will automatically distinguish between the old and new by 
///	looking for two different MAGIC numbers.
typedef struct
{
	char	MagicNum[36];			///< == kTCAT_DICE_VERSION_NEW_MAGIC_STRING defined in tcat_dice_macros.h
	uint32	uiBaseSDKVersion;		///< The full version/revision of the SDK this build was based on
	uint32	uiApplicationVersion;	///< The full version/revision of the Application
	uint32	uiVendorID;				///< The Vendor ID
	uint32	uiProductID;			///< The product ID
	char	BuildTime[64];			///< Build time
	char	BuildDate[64];			///< Build date
} NEW_VERSION;

/// deprecated: use NEW_VERSION
typedef struct
{
	char MagicNum[36];				///< == kTCAT_DICE_VERSION_MAGIC_STRING defined in tcat_dice_macros.h
	char Major[5];					///< 4 digits followed by a space
	char Minor[5];					///< 4 digits followed by a space
	char VendorID[7];				///< 6 digit 24-bit OUI (as defined in targetVendorDefs.h) followed by a space
	char VProductID[4];				///< 3 digit (10 bits) Product ID (as defined in targetVendorDefs.h) followed by a space
	char VMajor[5];					///< 4 digits followed by a space
	char VMinor[5];					///< 4 digits followed by a space
} VERSION;

//\endcond

} // namespace dice
} // namespace tcat
