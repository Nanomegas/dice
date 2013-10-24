//===========================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd..
//
// (C) COPYRIGHT 2005 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
/****************************************************************************

 DESCRIPTION:

	Implementation of AV/C Stream Format Information commands (Ver. 1.1)

  Revisions:
		created 12/20/2005 BK

****************************************************************************/

#ifndef _AVCCMDSTREAMFORMAT_H
#define _AVCCMDSTREAMFORMAT_H

#include "TCTypes.h"
#include "packetBlock.h"
#include "datastream.h"
#include "avcUtils.h"

#define STREAM_FMT_SUBFUNCTION_INPUT				0x00
#define STREAM_FMT_SUBFUNCTION_OUTPUT				0x01

#define STREAM_FMT_EXT_SUBFUNCTION_SINGLE			0xC0
#define STREAM_FMT_EXT_SUBFUNCTION_LIST				0xC1

#define STREAM_FMT_IN_STATUS_CONFIGURED				0x00
#define STREAM_FMT_IN_STATUS_NOT_YET_CONFIGURED		0x01
#define STREAM_FMT_IN_STATUS_READY					0x02
#define STREAM_FMT_IN_STATUS_NOT_CONFIGURED			0x03
#define STREAM_FMT_IN_STATUS_NOT_SUPPORTED			0x04
#define STREAM_FMT_IN_STATUS_NO_INFO				0xFF

#define STREAM_FMT_OUT_STATUS_SUPPORTED				0x00
#define STREAM_FMT_OUT_STATUS_NOT_SUPPORTED			0x04
#define STREAM_FMT_OUT_STATUS_NO_INFO				0xFF

#define STREAM_FMT_ROOT_AM							0x90	// Audio and Music
#define STREAM_FMT_ROOT_INVALID						0xFF

#define STREAM_FMT_LEVEL1_AM_SINGLE					0x00
#define STREAM_FMT_LEVEL1_AM_24BIT_PACK				0x01
#define STREAM_FMT_LEVEL1_AM_32BIT_FLOAT			0x02
#define STREAM_FMT_LEVEL1_AM_COMPOUND				0x40	// used only with EXTENDED_STREAM_FORMAT_INFORMATION command
#define STREAM_FMT_LEVEL1_AM_DONT_CARE				0xFF

// 
#define STREAM_FMT_LEVEL2_IEC60958_3				0x00	// SPDIF transparent
#define STREAM_FMT_LEVEL2_IEC61937_3				0x00	// AC3 compressed
#define STREAM_FMT_LEVEL2_MBLA_RAW					0x06	// uncompressed linear PCM
#define STREAM_FMT_LEVEL2_MBLA_DVD					0x07	// DVD-Audio
#define STREAM_FMT_LEVEL2_1BIT_PLAIN_RAW			0x08
#define STREAM_FMT_LEVEL2_1BIT_PLAIN_SACD			0x09
#define STREAM_FMT_LEVEL2_1BIT_ENCODED_RAW			0x0A
#define STREAM_FMT_LEVEL2_1BIT_ENCODED_SACD			0x0B
#define STREAM_FMT_LEVEL2_MBLA_HIGH_PRECISION		0x0C
#define STREAM_FMT_LEVEL2_MIDI						0x0D
#define STREAM_FMT_LEVEL2_SAMPLE_COUNT				0x0F
#define STREAM_FMT_LEVEL2_ANCILLIARY				0x10
#define STREAM_FMT_LEVEL2_SYNC_STREAM				0x40	// stereo raw audio, does not carry audio data
#define STREAM_FMT_LEVEL2_DONT_CARE					0xFF

// mbla
#define STREAM_FMT_SAMPLING_FREQ_22050				0x00
#define STREAM_FMT_SAMPLING_FREQ_24000				0x01
#define STREAM_FMT_SAMPLING_FREQ_32000				0x02
#define STREAM_FMT_SAMPLING_FREQ_44100				0x03
#define STREAM_FMT_SAMPLING_FREQ_48000				0x04
#define STREAM_FMT_SAMPLING_FREQ_96000				0x05
#define STREAM_FMT_SAMPLING_FREQ_176400				0x06
#define STREAM_FMT_SAMPLING_FREQ_192000				0x07
#define STREAM_FMT_SAMPLING_FREQ_88200				0x0A
#define STREAM_FMT_SAMPLING_FREQ_DONT_CARE			0xFF

#define STREAM_FMT_PLUG_DIRECTION_INPUT				0x00
#define STREAM_FMT_PLUG_DIRECTION_OUTPUT			0x01

#define STREAM_FMT_PLUG_MODE_UNIT					0x00
#define STREAM_FMT_PLUG_MODE_SUBUNIT				0x01
#define STREAM_FMT_PLUG_MODE_FB						0x02

#define STREAM_FMT_PLUG_TYPE_PCR					0x00
#define STREAM_FMT_PLUG_TYPE_EXTERNAL				0x01
#define STREAM_FMT_PLUG_TYPE_ASYNC					0x02

#define STREAM_FMT_PLUG_RATE_CTRL_SUPPORTED			0x00
#define STREAM_FMT_PLUG_RATE_CTRL_DONT_CARE			0x01
#define STREAM_FMT_PLUG_RATE_CTRL_NOT_SUPPORTED		0x02

#define STREAM_FMT_RATE_IS_SYNC_SOURCE				(0x01 << 2)

#define STREAM_FMT_PCR_ANY							0x7f

typedef struct _AVC_PLUG_ADDRESS_UNIT
{
	BYTE	plug_type;
	BYTE	plug_id;
	BYTE	rsvd;
} AVC_PLUG_ADDRESS_UNIT;

typedef struct _AVC_PLUG_ADDRESS_SUBUNIT
{
	BYTE	plug_id;
	BYTE	rsvd1;
	BYTE	rsvd2;
} AVC_PLUG_ADDRESS_SUBUNIT;

typedef struct _AVC_PLUG_ADDRESS_FB
{
	BYTE	fb_type;
	BYTE	fb_id;
	BYTE	plug_id;
} AVC_PLUG_ADDRESS_FB;

typedef union _AVC_PLUG_ADDR_UNION
{
	AVC_PLUG_ADDRESS_UNIT		unit;
	AVC_PLUG_ADDRESS_SUBUNIT	su;
	AVC_PLUG_ADDRESS_FB			fb;
} AVC_PLUG_ADDR_UNION;

typedef struct _AVC_STREAM_FORMAT_INFO
{
	BYTE				root;					// format_heirarchy_root
	BYTE				L1;						// format_heirarchy_level1
	BYTE				L2;						// format_heirarchy_level2
	BYTE				attribute;				// sampling freq and rate bit
} AVC_STREAM_FORMAT_INFO;

typedef struct _AVC_PLUG_STREAM_FORMAT
{
	BYTE						plug_id;		// unit and subunit plug
	BYTE						status;			// support status
	BYTE						num_formats;	// corresponds to # of clusters
	AVC_STREAM_FORMAT_INFO		formats[8];
} AVC_PLUG_STREAM_FORMAT;

typedef struct _AVC_STREAM_FORMAT_EXT_INFO
{
	BYTE				num_channels;			// number of channels in the cluster
	BYTE				format;					// AM824 stream format
} AVC_STREAM_FORMAT_EXT_INFO;

typedef struct _AVC_PLUG_EXT_STREAM_FORMAT
{
	AVC_PLUG_ADDR_UNION			u;				// address_data, mode-dependent
	BYTE						status; 
	BYTE						fmt_root;
	BYTE						fmt_L1;
	BYTE						num_rates;
	BYTE						rates[9];
	BYTE						sync;
	BYTE						num_fmts;
	AVC_STREAM_FORMAT_EXT_INFO	formats[8];
} AVC_PLUG_EXT_STREAM_FORMAT;

HRESULT	avcCmdStreamFmtSupport(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream, BYTE unit);
HRESULT	avcCmdStreamFmtExtInfo(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream);

#endif //_AVCCMDSTREAMFORMAT_H

