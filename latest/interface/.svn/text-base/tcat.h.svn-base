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
//
//	TCAT common interface types & defines
//

#pragma once


#if (__cplusplus)
namespace tcat
{
#endif //(__cplusplus)

typedef unsigned	char		uint8;		///< unsigned 8 bit integer
typedef				short		int16;		///< signed 16 bit integer
typedef	unsigned	short		uint16;		///< unsigned 16 bit integer
#ifdef __MACH__
typedef				int			int32;		///< signed 32 bit integer
typedef	unsigned	int			uint32;		///< unsigned 32 bit integer
#else
typedef				long		int32;		///< signed 32 bit integer
typedef	unsigned	long		uint32;		///< unsigned 32 bit integer
#endif
#ifdef _WIN32
typedef				void*		handle;		///< Windows: HANDLE
#endif
#ifdef __MACH__
typedef	unsigned	int			handle;		///< Mac OS X: io_connect_t
#endif
#if defined(_WIN32) && (_MSC_VER < 1310)
// older versions of Visual Studio and the DDK PREFAST compiler require '__int64'
typedef				__int64		int64;		///< signed 64 bit integer
typedef	unsigned	__int64		uint64;		///< unsigned 64 bit integer
#else
typedef				long long	int64;		///< signed 64 bit integer
typedef	unsigned	long long	uint64;		///< unsigned 64 bit integer
#endif


#define kTCAT_INVALID_HANDLE		((handle)-1)
#define TCAT_OFFSET(type,member)	((uint32)(uint64)&(((type*)NULL)->member))
#define TCAT_MAX(a,b)				(((a)>(b))?(a):(b))
#define TCAT_MIN(a,b)				(((a)<(b))?(a):(b))


#ifdef _WIN32
#define TCAT_FUNCTION __FUNCTION__
#endif
#ifdef __MACH__
#define TCAT_FUNCTION __PRETTY_FUNCTION__
#endif


#ifdef _DEBUG
#define TCAT_DEBUG _DEBUG
#endif


#if (__cplusplus)
}
#endif //(__cplusplus)
