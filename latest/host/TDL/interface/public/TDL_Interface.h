///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2009 - 2011 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#include "tcat.h"


namespace tcat
{


enum
{
	kTDLFlag_DisableAll		= 0x00000000,
	kTDLFlag_EnableAll		= 0xffffffff,

	kTDLFlag_StreamIndexMask	= 0x0000000f,
	kTDLFlag_IsTransmitStream	= 1 << 4,			// host computer --> device
	kTDLFlag_Streaming			= 1 << 5,

	kTDLFlag_Error				= 1 << 6,
	kTDLFlag_Warning			= 1 << 7,
	kTDLFlag_Info				= 1 << 8,
	kTDLFlag_Verbose			= 1 << 9,

	kTDLFlag_Configuration		= 1 << 10,
	kTDLFlag_StreamSetup		= 1 << 11,
	kTDLFlag_Audio				= 1 << 12,			// ASIO on Windows, CoreAudio on Mac OS X
	kTDLFlag_WDM				= 1 << 13,
	kTDLFlag_MIDI				= 1 << 14,
	kTDLFlag_Ioctl				= 1 << 15,
	kTDLFlag_PAL				= 1 << 16,

	kTDLFlag_Marker				= 1 << 30,
	kTDLFlag_StopLogging		= 1 << 31
};

#define kTDLFlags_Stream( streamType, streamIndex )		\
	(kTDLFlag_Streaming | (((streamType) == 1) ? kTDLFlag_IsTransmitStream : 0) | ((streamIndex) & kTDLFlag_StreamIndexMask))



int  TDL_OpenConnection();
int  TDL_CloseConnection();
void TDL_StdMsg( uint32 flags, uint32 id, int64 dataOne, int64 dataTwo, int64 dataThree, int64 dataFour );
void TDL_RltMsg( uint32 flags, uint32 id, int64 dataOne, int64 dataTwo, int64 dataThree, int64 dataFour );


extern volatile uint32	* volatile g_TDL_Std_Log_Flags;
extern volatile uint32	* volatile g_TDL_Rlt_Log_Flags;


#define TDL_ENABLED( flags32 )				((((uint32)flags32) & *g_TDL_Std_Log_Flags) == ((uint32)flags32))
#define TDL_REALTIME_ENABLED( flags32 )		((((uint32)flags32) & *g_TDL_Rlt_Log_Flags) == ((uint32)flags32))


#define TDL_ENTRY( flags32, message, dataOne64, dataTwo64, dataThree64, dataFour64 )	\
																						\
	if ((((uint32)flags32) & *g_TDL_Std_Log_Flags) == ((uint32)flags32))				\
	{																					\
		TDL_StdMsg( (uint32)flags32, ((kDriverLog_File_ID << 13) | __LINE__ ), (int64)dataOne64, (int64)dataTwo64, (int64)dataThree64, (int64)dataFour64 );	\
	}


#define TDL_REALTIME_ENTRY( flags32, message, dataOne64, dataTwo64, dataThree64, dataFour64 )	\
																						\
	if ((((uint32)flags32) & *g_TDL_Rlt_Log_Flags) == ((uint32)flags32))				\
	{																					\
		TDL_RltMsg( (uint32)flags32, ((kDriverLog_File_ID << 13) | __LINE__ ), (int64)dataOne64, (int64)dataTwo64, (int64)dataThree64, (int64)dataFour64 );	\
	}


} // namespace tcat
