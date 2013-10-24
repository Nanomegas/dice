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

		opcode									ctype			target
												C,S,N			U,S
		0x2F	STREAM FORMAT SUPPORT			-,O,-			+,-
		0xBF	EXTENDED STREAM FORMAT INFO		O,O,O			+,+

		ctype:
		C:CONTROL, S:STATUS, N:NOTIFY
		M:Mandatory, R:Recommended, O:Optional, -:Not Defined

		target
		U:UNIT, S:SUBUNIT

  Revisions:
		created 12/20/2005 BK

****************************************************************************/

//static char 		moduleName[] = __FILE__ ;

#include "TCTasking.h"
#include "avcCmdConnection.h"
#include "avcUnitCli.h"
#include "avcUnitUtils.h"
#include "avcUtils.h"
#include "avcUnitChannelUsage.h"
#include "avcUnitConnect.h"
#include "avcUnitConnectAV.h"
#include "avcUnitSignalFormat.h"				// inputPlugSignalFormat, outputPlugSignalFormat
#include "avcDefs.h"
#include "avc.h"
#include "targetPlugs.h"
#include "avcCmdStreamFormat.h"
#include "avcDriver.h"
#include "cmp.h"
#include "cmpUtils.h"
#include "cli.h"
#include "targetBoard.h"

// This function is deprecated, not to be used.
HRESULT avcCmdStreamFmtSupport(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream, BYTE unit)
{
	UNUSED_ARG(pHeader);
	UNUSED_ARG(packetBlock);
	UNUSED_ARG(pStream);
	UNUSED_ARG(unit);

	SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "Got a request using deprecated opcode 0x2F, rejecting");

	return E_PKT_AVC_REJECTED;
}

HRESULT avcCmdStreamFmtExtInfo(AVC_HEADER *pHeader, PB *packetBlock, pDataStream pStream)
{
	HRESULT		hResult = NO_ERROR;
	HRESULT		hResultError = E_PKT_AVC_NOT_IMPLEMENTED;
	BYTE		i = 0;
	BYTE		subfunction = 0;
	uint32		payloadSize = 0;
	PB*			respPacket = NULL;
	pDataStream	pStreamResp = NULL;
	uint32		subunit_type = 0;
	BYTE		plug_addr_data[3] = { 0 };
	BYTE		plug_dir = 0;
	BYTE		plug_addr_mode = 0;
	BYTE		plug_type = 0;
	BYTE		plug_ID = 0;
	BYTE		support_status = 0;
	BYTE		index = 0;
	BYTE		fmt_header_size = 0;
	DAL_NOMINAL_RATE_ENUM dal_rate = 0;
	DAL_NOMINAL_RATE_ENUM new_dal_rate = 0;
	BYTE		sampling_freq = 0;
	BYTE		ext_fmt_rate = 0;
	BYTE		num_fmts_reported = 0;

	AVC_PLUG_EXT_STREAM_FORMAT* plugExtInfo = NULL;

	hResultError = E_PKT_AVC_REJECTED;

	SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "\n\rExt Strm Fmt Info \n\r");

	// subunit_type is 0x1f if it's a unit command
	subunit_type = pHeader->addrSubunit.subunit_type;

	// Get current payload size, we'll create a new (larger) packet if we have a response
	hResult = pbGetPayloadSize (packetBlock, &payloadSize) ;
	if (hResult != NO_ERROR) return hResultError;

	switch (pHeader->ctype)
	{
		case AVC_CTYPE_STATUS:	// O : M:Mandatory, R:Recommended, O:Optional, -:Not Defined

			SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "status, ");

			hResult = dsRead8Bits(pStream, &subfunction);
			if (hResult != NO_ERROR) return hResultError;

			// plug direction
			hResult = dsRead8Bits(pStream, &plug_dir);
			if (hResult != NO_ERROR) return hResultError;

			// plug address mode
			hResult = dsRead8Bits(pStream, &plug_addr_mode);
			if (hResult != NO_ERROR) return hResultError;

			// plug address data
			for (i = 0; i < 3; i++)
			{
				hResult = dsRead8Bits(pStream, &plug_addr_data[i]);
				if (hResult != NO_ERROR) return hResultError;
			}

			// support_status
			hResult = dsRead8Bits(pStream, &support_status);	
			if (hResult != NO_ERROR) return hResultError;

			if (STREAM_FMT_EXT_SUBFUNCTION_SINGLE == subfunction)
			{
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "single, ");
				fmt_header_size = 3;
			}
			else if (STREAM_FMT_EXT_SUBFUNCTION_LIST == subfunction)
			{
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "list, ");

				// querying an index into our sample rates array
				hResult = dsRead8Bits(pStream, &index);
				if (hResult != NO_ERROR) return hResultError;

				fmt_header_size = 4;
			}

			// get plug_address info depending on plug_addr_mode
			if (STREAM_FMT_PLUG_MODE_UNIT == plug_addr_mode)
			{
				// data: plug_type, Plug_ID, 0xff
				plug_type = plug_addr_data[0];
				plug_ID = plug_addr_data[1];
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "unit, type %d, id %d\n\r", plug_type,  plug_ID);
			}
			else if (STREAM_FMT_PLUG_MODE_SUBUNIT == plug_addr_mode)
			{
				// data: plug_ID, 0xff, 0xff
				plug_ID = plug_addr_data[0];
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "subunit, id %d\n\r", plug_ID);
			}

			// get info for the requested plug
			hResult = targetGetExtPlugFmtInfo(plug_addr_mode, plug_type, plug_dir, plug_ID, subunit_type, &plugExtInfo);
			if (NO_ERROR != hResult) return hResultError;	// invalid request
			
			SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "unit/su 0x%02x, plug 0x%02x, dir %d\n\r", subunit_type, plug_ID, plug_dir, plugExtInfo->formats[0].num_channels, plugExtInfo->formats[0].format);

			// if it's a list command, reject when we have no more sample rates to report
			if (STREAM_FMT_EXT_SUBFUNCTION_LIST == subfunction)
			{
				if (index >= plugExtInfo->num_rates)
				{
					SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "sr index %d, rejected (ok)\n\r", index);
					return E_PKT_AVC_REJECTED;
//					return E_PKT_AVC_NOT_IMPLEMENTED;
				}
			}

			// at this point we have valid plug info to return, make a larger response packet and send it off

			payloadSize += (fmt_header_size + (plugExtInfo->num_fmts * 2)); // set up payload size for response packet

			// Create a new (larger) packet for the response
			hResult = avcCreateResponsePacket(packetBlock, &respPacket, TRUE, payloadSize);
			if (hResult != NO_ERROR) return hResultError;

			// write the requested information into the packet
			hResult = pbGetApplicationDatastream (respPacket, &pStreamResp);
			if (hResult != NO_ERROR)
			{
				avcDestroyResponsePacket(respPacket);
				return hResult;
			}

			hResult = dsSwitchMode(pStreamResp, dsMODE_WRITE);
			dsSetPosition(pStreamResp, 9);	// point to status byte

			// support_status
			hResult = dsWrite8Bits(pStreamResp, plugExtInfo->status);

			// format_information
			if (STREAM_FMT_EXT_SUBFUNCTION_LIST == subfunction)
			{
				// index
				hResult = dsWrite8Bits(pStreamResp, index);
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "sr index %d\n\r", index);

				hResult = dsWrite8Bits(pStreamResp, plugExtInfo->fmt_root);
				hResult = dsWrite8Bits(pStreamResp, plugExtInfo->fmt_L1);

				if (STREAM_FMT_LEVEL1_AM_COMPOUND == plugExtInfo->fmt_L1)
				{
					hResult = dsWrite8Bits(pStreamResp, plugExtInfo->rates[index]);
					hResult = dsWrite8Bits(pStreamResp, plugExtInfo->sync);

					num_fmts_reported = plugExtInfo->num_fmts;
					hResult = dsWrite8Bits(pStreamResp, num_fmts_reported);
					for (i=0; i<num_fmts_reported; i++)
					{
						SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "   fmt# %d, #ch %d, fmt %x\n\r", i, plugExtInfo->formats[i].num_channels, plugExtInfo->formats[i].format);
						hResult = dsWrite8Bits(pStreamResp, plugExtInfo->formats[i].num_channels);
						hResult = dsWrite8Bits(pStreamResp, plugExtInfo->formats[i].format);
					}
				}
				else if (STREAM_FMT_LEVEL1_AM_SINGLE == plugExtInfo->fmt_L1)
				{
					hResult = dsWrite8Bits(pStreamResp, 0xff);
					hResult = dsWrite8Bits(pStreamResp, (plugExtInfo->rates[index]<<4) + 1);
					hResult = dsWrite8Bits(pStreamResp, 0xff);
				}
			}
			else
			{
				hResult = avcDriverGetRate(&dal_rate);
				hResult = dal2ExtFmtRate(dal_rate, &ext_fmt_rate);

				hResult = dsWrite8Bits(pStreamResp, plugExtInfo->fmt_root);
				hResult = dsWrite8Bits(pStreamResp, plugExtInfo->fmt_L1);

				if (STREAM_FMT_LEVEL1_AM_COMPOUND == plugExtInfo->fmt_L1)
				{
					// use current sample rate of unit
					hResult = dsWrite8Bits(pStreamResp, ext_fmt_rate);

					hResult = dsWrite8Bits(pStreamResp, plugExtInfo->sync);
					num_fmts_reported = plugExtInfo->num_fmts;
					hResult = dsWrite8Bits(pStreamResp, num_fmts_reported);
					for (i=0; i<num_fmts_reported; i++)
					{
						SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "   fmt# %d, #ch %d, fmt %x\n\r", i, plugExtInfo->formats[i].num_channels, plugExtInfo->formats[i].format);
						hResult = dsWrite8Bits(pStreamResp, plugExtInfo->formats[i].num_channels);
						hResult = dsWrite8Bits(pStreamResp, plugExtInfo->formats[i].format);
					}
				}
				else if (STREAM_FMT_LEVEL1_AM_SINGLE == plugExtInfo->fmt_L1)
				{
					hResult = dsWrite8Bits(pStreamResp, 0xff);
					hResult = dsWrite8Bits(pStreamResp, (ext_fmt_rate<<4) + STREAM_FMT_PLUG_RATE_CTRL_NOT_SUPPORTED);
					hResult = dsWrite8Bits(pStreamResp, 0xff);
				}
			}

			avcSendResponsePacket(AVC_RESPONSE_STABLE, respPacket);

			hResult = NO_ERROR; //avcHandler shouldn't send a response too
			break;

		case AVC_CTYPE_CONTROL: // - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
			// host is trying to change sample rate
			SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "control, ");

			hResult = dsRead8Bits(pStream, &subfunction);
			if (hResult != NO_ERROR) return hResultError;

			if (STREAM_FMT_EXT_SUBFUNCTION_SINGLE == subfunction)
			{
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "single, ");

				// plug direction
				hResult = dsRead8Bits(pStream, &plug_dir);
				if (hResult != NO_ERROR) return hResultError;

				// plug address mode
				hResult = dsRead8Bits(pStream, &plug_addr_mode);
				if (hResult != NO_ERROR) return hResultError;

				// plug address data
				for (i = 0; i < 3; i++)
				{
					hResult = dsRead8Bits(pStream, &plug_addr_data[i]);
					if (hResult != NO_ERROR) return hResultError;
				}

				// support_status
				hResult = dsRead8Bits(pStream, &support_status);	
				if (hResult != NO_ERROR) return hResultError;

				// get plug_address info depending on plug_addr_mode
				if (STREAM_FMT_PLUG_MODE_UNIT == plug_addr_mode)
				{
					// data: plug_type, Plug_ID, 0xff
					plug_type = plug_addr_data[0];
					plug_ID = plug_addr_data[1];
					SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "unit, type %d, id %d\n\r", plug_type,  plug_ID);
				}
				else if (STREAM_FMT_PLUG_MODE_SUBUNIT == plug_addr_mode)
				{
					// data: plug_ID, 0xff, 0xff
					plug_ID = plug_addr_data[0];
					SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "subunit, id %d\n\r", plug_ID);
				}

				// point to sampling freq byte and read
				hResult = dsFastForward(pStream, 2);	
				hResult = dsRead8Bits(pStream, &sampling_freq);	
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "  new ext fmt rate %d\n\r", sampling_freq);
				if (hResult != NO_ERROR) return hResultError;

				// rates do not match DAL/SFC values, use conversions

				hResult = avcDriverGetRate(&dal_rate);
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "  cur dal rate %d\n\r", dal_rate);

				hResult = extFmt2DalRate(sampling_freq, &new_dal_rate);
				SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "  new dal rate %d\n\r", new_dal_rate);

				if (dal_rate != new_dal_rate)
				{
					uint32 OldFormat = 0;
					uint32 NewFormat = (new_dal_rate << 16) | 0x1000ffff;

					SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "    sr update: old %d, new %d \n\r", dal_rate, new_dal_rate);

					// change in and out formats
					hResult = cmpGetiPCRFormat(0, &OldFormat);
					if ((NO_ERROR == hResult) && (OldFormat != NewFormat))
					{
						SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "      updating PCR's, OldFormat %x, NewFormat %x\n\r", OldFormat, NewFormat);
						hResult = cmpSetiPCRFormat(0, NewFormat);
						hResult = cmpSetoPCRFormat(0, NewFormat);
//						hResult = cmpSetiPCRFormat(1, NewFormat);
//						hResult = cmpSetoPCRFormat(1, NewFormat);
					}
					SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "      setting dal rate %d\n\r", new_dal_rate);
					avcDriverSetRate(new_dal_rate);
				}
				return E_PKT_AVC_ACCEPTED;
			}
			hResult = hResultError;
			break;

		case AVC_CTYPE_NOTIFY:	// - : M:Mandatory, R:Recommended, O:Optional, -:Not Defined
			hResult = hResultError;
			break;

		case AVC_CTYPE_SPECIFIC_INQUIRY:
			SYS_DEBUG(SYSDEBUG_TRACE_AVC_MUSIC, "specific inq\n\r");
			hResult = E_PKT_AVC_IMPLEMENTED;
			break;

		case AVC_CTYPE_GENERAL_INQUIRY:
			// don't even bother parsing
			hResult = E_PKT_AVC_IMPLEMENTED;
			break;
		
		default:
			hResult = hResultError;
			break;
	}

	return hResult;
}
