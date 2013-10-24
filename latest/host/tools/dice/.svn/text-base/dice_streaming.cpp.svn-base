///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004 - 2010 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////
//
//	see dice_streaming.h
//
#include <math.h>
#include <fstream>
#include "dice_streaming.h"
#include "TDL_Interface.h"

#ifdef WIN32
#include "fwi.h"
#include "DPCChk.h"
extern dpcChk dpc_chk; 

std::string wstrtostr(const std::wstring &wstr)
{
    // Convert a Unicode string to an ASCII string
    std::string strTo;
    char *szTo = new char[wstr.length() + 1];
    szTo[wstr.size()] = '\0';
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szTo, (int)wstr.length(), NULL, NULL);
    strTo = szTo;
    delete[] szTo;
    return strTo;
}
#endif // WIN32


#define kDriverLog_File_ID		1101

#define	kMaxErrorsToLog			64		// max amount of any one type of streaming error to log

bool tcat::s_log = false;
bool tcat::s_trc = false;
bool tcat::s_audio = true;
bool tcat::s_midi = true;
bool tcat::s_qut = false;
bool tcat::s_plugin = false;
volatile bool tcat::s_exit = false;


using namespace tcat;


///////////////////////////////////////////////////////////////////////////////
//
//	sdevice
//
///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::reset_counters
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::reset_counters()							
{ 
	memset (&m_counters, 0, sizeof m_counters);
	for (std::deque<os_midi>::iterator i=m_midi.begin(); i<m_midi.end(); i++)
	{
		(*i).m_counter.reset();
		(*i).m_errors = 0;
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::update
//
///////////////////////////////////////////////////////////////////////////////
//
bool sdevice::update()	throw(tcat::exception::base)
{
	bool bReturn = super::update();
	if (bReturn)
	{
		if (s_trc)
			std::clog << "    " << TCAT_FUNCTION << "[" << nickname() << 
				"]" << // : notification:" << std::hex << m_notification << 
				", ex_status:" << m_cache[0].clock.ex_status << std::endl;
		if (s_log)
		{
			// following should never happen:
			if (m_cache[0].info.guid64 != m_cache[1].info.guid64)
				std::clog << "    + guid:" << m_cache[1].info.guid64<< "->" << m_cache[0].info.guid64 << std::endl;
			std::clog << std::endl;
		}
	}
	return bReturn;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::update_rx
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::update_rx() throw(tcat::exception::base)
{ 
	dice::lock lock(this); 
	super::update_rx(); 
	if (s_trc) 
		std::clog << "    " << TCAT_FUNCTION << "[" << nickname() << "]: " << std::dec << channels(false).size() << " audio channels, " << midi_ports(false) << " MIDI ports" << std::endl;
	update_midi();
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::update_tx
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::update_tx() throw(tcat::exception::base)		
{ 
	dice::lock lock(this); 
	super::update_tx(); 
	if (s_trc) 
		std::clog << "    " << TCAT_FUNCTION << "[" << nickname() << "]: " << std::dec << channels(true).size() << " audio channels, " << midi_ports(true) << " MIDI ports" << std::endl;
	update_midi();
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::update_midi
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::update_midi() throw(tcat::exception::base)		
{ 
	if (s_midi)
	{
		m_midi.clear();
		for (uint32 port=0; port<std::min<uint32>(midi_ports(false),midi_ports(true)); port++)
			m_midi.push_back(os_midi(*this,port));
		for (std::deque<os_midi>::iterator i=m_midi.begin(); i<m_midi.end(); i++)
			(*i).open();
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::os_midi::process
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::os_midi::process (uint32 data)
{
	uint64	current = pal::time64();
	uint8	statusByte = (uint8)(data & 0xff);
	uint8	dataByteOne = (uint8)((data >> 8) & 0xff);
	uint8	dataByteTwo = (uint8)((data >> 16) & 0xff);

	if (m_counter.first_occurence == 0)
	{
		if ((statusByte == 0x94) && (dataByteOne == 0x7c))
		{
			TDL_REALTIME_ENTRY( kTDLFlag_Verbose | kTDLFlag_MIDI, "DiceTool: start MIDI received  (port %d)", m_port_idx, 0, 0, 0 );
			if (s_trc)
				std::clog << "  (" << m_port_idx << ") start MIDI received " << std::endl;
			m_counter.current = current;
			m_counter.first_occurence = current;
			m_counter.changes = 0;

			m_dataOne = 0;
			m_dataTwo = dataByteTwo;

			for (int n = 0; n < 6; n++)
			{
				send( (m_dataTwo << 16) | (m_dataOne << 8) | (0x80 + (m_port_idx & 0xf)) );
				m_dataOne = ((m_dataOne + 1) & 0x7f);
			}

			m_dataOneExpected = 0;
		}
		else
		{
			TDL_REALTIME_ENTRY( kTDLFlag_Error, "DiceTool: unexpected MIDI received  (port %d, status 0x%02x, dataOne 0x%02x, dataTwo 0x%02x)",
								m_port_idx, statusByte, dataByteOne, dataByteTwo );
		}
	}
	else
	{
		if (statusByte == (0x80 + (m_port_idx & 0xf)))
		{
			uint32 latency = (uint32)((current - m_counter.current) & 0xffffffff);

			if (m_counter.changes == 0)
			{
				if (s_trc)
					std::clog << "  (" << m_port_idx << ") first MIDI received:  0x" << std::hex << (int)statusByte << " 0x" << (int)dataByteOne << " 0x" << (int)dataByteTwo << std::dec << std::endl;
			}

			if ((dataByteOne != m_dataOneExpected) || (dataByteTwo != m_dataTwo))
			{
				// send/receive error
				if (m_errors < kMaxErrorsToLog)
				{
					if (dataByteOne != m_dataOneExpected)
					{
						TDL_REALTIME_ENTRY( kTDLFlag_Error, "DiceTool: MIDI receive error  (port %d, status 0x%02x, dataOne 0x%02x, dataOneExpected 0x%02x)",
											m_port_idx, statusByte, dataByteOne, m_dataOneExpected );
					}
					else
					{
						TDL_REALTIME_ENTRY( kTDLFlag_Error, "DiceTool: MIDI receive error  (port %d, status 0x%02x, dataTwo 0x%02x, dataTwoExpected 0x%02x)",
											m_port_idx, statusByte, dataByteTwo, m_dataTwo );
					}
				}

				m_errors++;
				m_dataOneExpected = ((dataByteOne + 1) & 0x7f);
			}
			else
			{
				m_dataOneExpected = ((m_dataOneExpected + 1) & 0x7f);
				m_counter.changes += 3;		// 3 bytes per MIDI message sent
			}

			m_counter.current = current;
			m_counter.min = std::min<uint32>(latency,m_counter.min);
			m_counter.max = std::max<uint32>(latency,m_counter.max);

			send( (m_dataTwo << 16) | (m_dataOne << 8) | (0x80 + (m_port_idx & 0xf)) );	
			m_dataOne = ((m_dataOne + 1) & 0x7f);
		}
		else
		{
			if (m_errors < kMaxErrorsToLog)
			{
				TDL_REALTIME_ENTRY( kTDLFlag_Error, "DiceTool: MIDI receive error  (port %d, status 0x%02x, statusExpected 0x%02x)",
									m_port_idx, statusByte, (0x80 + (m_port_idx & 0xf)), 0 );
			}
			// send/receive error
			m_errors++;
		}
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::midi_start
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::midi_start()
{
	if (s_midi)
		for (std::deque<os_midi>::iterator i=m_midi.begin(); i<m_midi.end(); i++)
			(*i).start();
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::midi_stop
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::midi_stop()
{
	if (s_midi)
		for (std::deque<os_midi>::iterator i=m_midi.begin(); i<m_midi.end(); i++)
			(*i).stop();
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::cmd_info
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::cmd_info()
{
	std::cout << nickname() << ": (" << type() << ", guid:" << std::hex << guid64() << ")" << std::endl;
	std::cout << "  ";
	for (int dir=0; dir<2; dir++)
		std::cout << "midi " << (dir?"out":"in") << ":" << midi_ports(dir?true:false) << " plug(s), ";
	std::cout << "midi loopback I/O:" << std::endl;
	for (std::deque<os_midi>::iterator midi=m_midi.begin(); midi!=m_midi.end(); midi++)
		std::cout << "    " << (*midi).m_name << std::endl;
#ifdef TCAT_DEBUG
	dice::SPACE space;
	dice::IO<kTCAT_DICE_TX> tx = _dbg_tx();
	dice::IO<kTCAT_DICE_RX> rx = _dbg_rx();
	#ifdef __BIG_ENDIAN__
	pal::swap32 (tx.isoc[0].szNames, sizeof (tx.isoc[0].szNames) >> 2);	// the ppc needs to swap the string...
	pal::swap32 (tx.isoc[1].szNames, sizeof (tx.isoc[1].szNames) >> 2);	// the ppc needs to swap the string...
	pal::swap32 (rx.isoc[0].szNames, sizeof (rx.isoc[0].szNames) >> 2);	// the ppc needs to swap the string...
	pal::swap32 (rx.isoc[1].szNames, sizeof (rx.isoc[1].szNames) >> 2);	// the ppc needs to swap the string...
	#endif
	space = _dbg_space(dice::kSpaceTx);
	out(space.offset);
	out(space.size32);
	out(tx.isoc_channels);
	out(tx.size32);
	out(tx.size32*4);
	out(sizeof tx.isoc[0]);
	out(tx.isoc[0].tx.sequences.audio);
	out(tx.isoc[0].tx.sequences.midi);
	out(tx.isoc[0].tx.speed);
	out(tx.isoc[0].names);
	out(tx.isoc[0].ac3_caps);
	out(tx.isoc[0].ac3_enable);
	std::cout << std::endl;					
	out(tx.isoc[1].tx.sequences.audio);
	out(tx.isoc[1].tx.sequences.midi);
	out(tx.isoc[1].tx.speed);
	out(tx.isoc[1].names);
	out(tx.isoc[1].ac3_caps);
	out(tx.isoc[1].ac3_enable);
	std::cout << std::endl;					
	space = _dbg_space(dice::kSpaceRx);
	out(space.offset);
	out(space.size32);
	out(rx.isoc_channels);
	out(rx.size32);
	out(rx.size32*4);
	out(sizeof rx.isoc[0]);
	out(rx.isoc[0].rx.first_sequence);
	out(rx.isoc[0].rx.sequences.audio);
	out(rx.isoc[0].rx.sequences.midi);
	out(rx.isoc[0].names);
	out(rx.isoc[0].ac3_caps);
	out(rx.isoc[0].ac3_enable);
	std::cout << std::endl;					
	out(rx.isoc[1].rx.first_sequence);
	out(rx.isoc[1].rx.sequences.audio);
	out(rx.isoc[1].rx.sequences.midi);
	out(rx.isoc[1].names);
	out(rx.isoc[1].ac3_caps);
	out(rx.isoc[1].ac3_enable);
	std::cout << std::endl;					
#endif //TCAT_DEBUG
	unsigned int routings = firmware_router(0xce000000)/0x100+1;
	size_t ins = channels(true).size();
	size_t outs = channels(false).size();
	std::cout << "  sample rate: " << std::dec << clock() << std::endl;
	std::cout << "  " << std::setiosflags(std::ios::right) << std::dec << 
		std::setw(2) << ins << " inputs:             |isoc|avs|" << 
		std::setw(2) <<	outs << " outputs:            |isoc]avs|" << firmware_router(0xce000000)/0x100+1 << " routings:" << 
		std::resetiosflags(std::ios::right) << std::endl;
	for (unsigned int chnl=0; chnl<std::max<size_t>(std::max<size_t>(ins,outs),firmware_router(0xce000000)/0x100+1); chnl++)
	{
		if (chnl<ins)
			std::cout << 
				std::setiosflags(std::ios::right) << "  " << std::dec << std::setw(2) << chnl+1 << std::resetiosflags(std::ios::right) << ":" <<
				std::setiosflags(std::ios::left) << std::setw(20) << channels(true)[chnl] << std::resetiosflags(std::ios::left) << "|" << 
				std::setiosflags(std::ios::right) << std::setw(4) << channels(true)[chnl].isoc() << std::resetiosflags(std::ios::right) << "|" <<
				std::setiosflags(std::ios::right) << std::setw(3) << channels(true)[chnl].avs() << std::resetiosflags(std::ios::right) << "|";
		else
			std::cout << "                         |    |   |";
		if (chnl<outs)
			std::cout << 
				std::setiosflags(std::ios::right) << std::dec << std::setw(2) << chnl+1 << std::resetiosflags(std::ios::right) << ":" <<
				std::setiosflags(std::ios::left) << std::setw(20) << channels(false)[chnl] << std::resetiosflags(std::ios::left) << "|" <<
				std::setiosflags(std::ios::right) << std::setw(4) << channels(false)[chnl].isoc() << std::resetiosflags(std::ios::right) << "|" <<
				std::setiosflags(std::ios::right) << std::setw(3) << channels(false)[chnl].avs() << std::resetiosflags(std::ios::right) << "|";
		else
			std::cout << "                       |    |   |";
		if (chnl<routings)
			std::cout << std::hex << std::setiosflags(std::ios::right) << 0xce000400+chnl*4 << ": " << std::setw(4) << (firmware_router(0xce000400+chnl*4) & 0xffff) << std::resetiosflags(std::ios::right);
		std::cout << std::endl;
	}

	dice::CLOCK_SYNC_CAPS caps = clock_sync_caps();
	const char * const clock_names[] = { "32kHz" , "44.1kHz", "48kHz", "88kHz", "96kHz", "176kHz", "192kHz", "Unknown"};
	uint16	clockcaps = (uint16)((*(uint32*) &caps) & 0xffff);
	std::cout << std::endl << "  clock caps bitmap: " << std::hex << std::setfill('0') << std::setw(4) << clockcaps << std::setfill(' ') << "  (";
	for (int clock_cap = dice::kClock192; clock_cap >= 0; clock_cap--)
	{
		std::cout << ((clockcaps & (1 << clock_cap)) ? std::string(clock_names[clock_cap]) + ", " : "");
	}
	std::cout << ")" << std::endl;
	uint16	synccaps = (uint16)((*(uint32*) &caps) >> 16);
	std::cout << "  sync  caps bitmap: " << std::hex << std::setfill('0') << std::setw(4) << synccaps << std::setfill(' ') << "  (";
	for (int sync_cap = dice::kSyncInternal; sync_cap >= 0; sync_cap--)
	{
		std::cout << ((synccaps & (1 << sync_cap)) ? sync_names()[sync_cap] + ", " : "");
	}
	std::cout << ")" << std::endl;

	std::cout << std::endl << "  source|unlocks|slips:" << std::endl;
	for (int sync=0; sync<(int)dice::kExStatusItems; sync++)
	{
		if (counters().status[sync].unlocks || counters().status[sync].slips)
		{
			std::cout << "  " << 
				std::resetiosflags(std::ios::right) << std::setiosflags(std::ios::left) << 
				std::setw(6) << sync_names()[sync] << "|" << 
				std::resetiosflags(std::ios::left) << std::setiosflags(std::ios::right) <<
				std::setw(7) << counters().status[sync].unlocks << "|" << 
				std::setw(7) << counters().status[sync].slips << 
				std::endl;
		}
	}
	std::cout << std::dec << std::endl << "  AVS device error registers:" << std::endl;

	dice::AVS avs;
	firmware_avs(avs);
	out0(avs[dice::kApbaInt0VtxStreamStart]);
	out0(avs[dice::kApbaInt0VtxStreamEnd]);
	out0(avs[dice::kApbaInt0VdoStreamStart]);
	out0(avs[dice::kApbaInt0VdoStreamEnd]);
	out0(avs[dice::kApbaInt0Atx2StreamStart]);
	out0(avs[dice::kApbaInt0Atx2StreamEnd]);
	out0(avs[dice::kApbaInt0Atx1StreamStart]);
	out0(avs[dice::kApbaInt0Atx1StreamEnd]);
	out0(avs[dice::kApbaInt0Ado4StreamStart]);
	out0(avs[dice::kApbaInt0Ado4StreamEnd]);
	out0(avs[dice::kApbaInt0Ado4Locked]);
	out0(avs[dice::kApbaInt0Ado3StreamStart]);
	out0(avs[dice::kApbaInt0Ado3StreamEnd]);
	out0(avs[dice::kApbaInt0Ado3Locked]);
	out0(avs[dice::kApbaInt0Ado2StreamStart]);
	out0(avs[dice::kApbaInt0Ado2StreamEnd]);
	out0(avs[dice::kApbaInt0Ado2Locked]);
	out0(avs[dice::kApbaInt0Ado1StreamStart]);
	out0(avs[dice::kApbaInt0Ado1StreamEnd]);
	out0(avs[dice::kApbaInt0Ado1Locked]);
	out0(avs[dice::kApbaInt0Ado2KeyReq]);
	out0(avs[dice::kApbaInt0Ado1KeyReq]);
	out0(avs[dice::kApbaInt0Ado0KeyReq]);
	out0(avs[dice::kApbaInt0VtxPktAbort]);
	out0(avs[dice::kApbaInt0VtxFrameAgeout]);
	out0(avs[dice::kApbaInt0VtxBoundryError]);
	out0(avs[dice::kApbaInt0VrxStatusError]);
	out0(avs[dice::kApbaInt0VrxPktAbort]);
	out0(avs[dice::kApbaInt0VrxLongPkt]);
	out0(avs[dice::kApbaInt0VrxDbcFail]);
	out0(avs[dice::kApbaInt0VrxCipFail]);
	out0(avs[dice::kApbaInt0VrxCfgFail]);

//	out0(avs[dice::kApbaInt1Reserved3]);
//	out0(avs[dice::kApbaInt1Reserved2]);
//	out0(avs[dice::kApbaInt1Reserved1]);
	out0(avs[dice::kApbaInt1VdoAgeout]);
	out0(avs[dice::kApbaInt1Ado4SytAgeout]);
	out0(avs[dice::kApbaInt1Ado4Slip]);
	out0(avs[dice::kApbaInt1Ado4Repeat]);
	out0(avs[dice::kApbaInt1Ado3SytAgeout]);
	out0(avs[dice::kApbaInt1Ado3Slip]);
	out0(avs[dice::kApbaInt1Ado3Repeat]);
	out0(avs[dice::kApbaInt1Ado2SytAgeout]);
	out0(avs[dice::kApbaInt1Ado2Slip]);
	out0(avs[dice::kApbaInt1Ado2Repeat]);
	out0(avs[dice::kApbaInt1Ado1SytAgeout]);
	out0(avs[dice::kApbaInt1Ado1Slip]);
	out0(avs[dice::kApbaInt1Ado1Repeat]);
	out0(avs[dice::kApbaInt1VtxdiSytUnderflow]);
	out0(avs[dice::kApbaInt1VtxdiSytOverflow]);
	out0(avs[dice::kApbaInt1VrxdoSytUnderflow]);
	out0(avs[dice::kApbaInt1VrxdoSytOverflow]);
	out0(avs[dice::kApbaInt1AtxDI2SytUnderflow]);
	out0(avs[dice::kApbaInt1AtxDI2SytOverflow]);
	out0(avs[dice::kApbaInt1AtxDI1SytUnderflow]);
	out0(avs[dice::kApbaInt1AtxDI1SytOverflow]);
	out0(avs[dice::kApbaInt1Arxdo4SytUnderflow]);
	out0(avs[dice::kApbaInt1Arxdo4SytOverflow]);
	out0(avs[dice::kApbaInt1Arxdo3SytUnderflow]);
	out0(avs[dice::kApbaInt1Arxdo3SytOverflow]);
	out0(avs[dice::kApbaInt1Arxdo2SytUnderflow]);
	out0(avs[dice::kApbaInt1Arxdo2SytOverflow]);
	out0(avs[dice::kApbaInt1Arxdo1SytUnderflow]);
	out0(avs[dice::kApbaInt1Arxdo1SytOverflow]);

//	out0(avs[dice::kApbaInt2Reserved4]);
	out0(avs[dice::kApbaInt2Atx2PktAbort]);
	out0(avs[dice::kApbaInt2Atx2FrameAgeout]);
	out0(avs[dice::kApbaInt2Atx2BoundryError]);
	out0(avs[dice::kApbaInt2Atx1PktAbort]);
	out0(avs[dice::kApbaInt2Atx1FrameAgeout]);
	out0(avs[dice::kApbaInt2Atx1BoundryError]);
	out0(avs[dice::kApbaInt2Arx4StatusError]);
	out0(avs[dice::kApbaInt2Arx4PktAbort]);
	out0(avs[dice::kApbaInt2Arx4LongPkt]);
	out0(avs[dice::kApbaInt2Arx4DbcFail]);
	out0(avs[dice::kApbaInt2Arx4CipFail]);
	out0(avs[dice::kApbaInt2Arx4CfgFail]);
	out0(avs[dice::kApbaInt2Arx3StatusError]);
	out0(avs[dice::kApbaInt2Arx3PktAbort]);
	out0(avs[dice::kApbaInt2Arx3LongPkt]);
	out0(avs[dice::kApbaInt2Arx3DbcFail]);
	out0(avs[dice::kApbaInt2Arx3CipFail]);
	out0(avs[dice::kApbaInt2Arx3CfgFail]);
	out0(avs[dice::kApbaInt2Arx2StatusError]);
	out0(avs[dice::kApbaInt2Arx2PktAbort]);
	out0(avs[dice::kApbaInt2Arx2LongPkt]);
	out0(avs[dice::kApbaInt2Arx2DbcFail]);
	out0(avs[dice::kApbaInt2Arx2CipFail]);
	out0(avs[dice::kApbaInt2Arx2CfgFail]);
	out0(avs[dice::kApbaInt2Arx1StatusError]);
	out0(avs[dice::kApbaInt2Arx1PktAbort]);
	out0(avs[dice::kApbaInt2Arx1LongPkt]);
	out0(avs[dice::kApbaInt2Arx1DbcFail]);
	out0(avs[dice::kApbaInt2Arx1CipFail]);
	out0(avs[dice::kApbaInt2Arx1CfgFail]);
	out0(avs[dice::kApbaInt2ItpEpTooBig]);

	std::cout << std::endl << "  firmware information:" << std::hex << std::endl;
	dice::FL_GET_APP_INFO_RETURN info = firmware_info();
	out(info.uiBaseSDKVersion);
	out(info.uiApplicationVersion);
	out(info.uiVendorID);
	out(info.uiProductID);
	out(info.BuildTime);
	out(info.BuildDate);
	out(info.uiBoardSerialNumber);
	std::cout << std::endl;

	dice::FL_GET_IMAGE_DESC_RETURN fw;
	uint32 sector=0;
	std::cout << "  sector          |length" << std::endl;
	while (firmware_info(sector++,fw))
	{
		std::cout << 
			"  " << std::setiosflags(std::ios::left) << std::resetiosflags(std::ios::right) << std::setw(16) << fw.name << std::dec <<
			"|" << std::setiosflags(std::ios::right) << std::hex << std::setw(6) << fw.size << std::resetiosflags(std::ios::right) <<
		//	"|" << 	", serial:" << std::dec << fw.uiBoardSerialNumber <<
		//	", config:" << std::hex << fw.uiConfigurationFlags <<
			std::endl;
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	tcat::sdevice::firmware_router
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::firmware_router (uint32 address, uint32 data) throw(tcat::exception::base)
{
	dice::FL_TEST_ACTION_PARAM fl;
	fl.cmdID=FL_TEST_CMD_POKE;
	fl.lvalue0=address;
	fl.lvalue1=data;

	firmware_package (dice::FL_OP_TEST_ACTION, 0x2000, &fl, sizeof(fl)/sizeof(uint32));
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::firmware_router
//
///////////////////////////////////////////////////////////////////////////////
//
uint32 sdevice::firmware_router (uint32 address) const throw(tcat::exception::base)
{
	dice::FL_TEST_ACTION_PARAM fl;
	fl.cmdID=FL_TEST_CMD_PEEK;
	fl.lvalue0=address;

	firmware_package (dice::FL_OP_TEST_ACTION, 0x2000, &fl, sizeof(fl)/sizeof(uint32));
	ohci_read (kTCAT_DICE_FW_OFFSET_FWL + TCAT_OFFSET(dice::FL_1394MEMORY,data.parameters), &fl.lvalue1);
	return fl.lvalue1;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::firmware_avs
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::firmware_avs(dice::AVS& avs) const throw(tcat::exception::base)
{
	dice::FL_TEST_ACTION_PARAM fl;
	fl.cmdID=FL_TEST_CMD_GET_AVS_CNT;

	try
	{
		firmware_package (dice::FL_OP_TEST_ACTION, 0x2000, &fl, sizeof(fl)/sizeof(uint32));
		ohci_read (kTCAT_DICE_FW_OFFSET_FWL + TCAT_OFFSET(dice::FL_1394MEMORY,data.rtn.testRtn), avs, sizeof(avs)/sizeof(uint32));
	}
	catch (tcat::exception::base xptn)
	{
		std::cout << "Unable to retrieve AVS stats from firmware" << std::endl;
		memset( &avs, 0, sizeof(dice::AVS) );
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::firmware_phase
//
///////////////////////////////////////////////////////////////////////////////
//
int sdevice::firmware_phase(uint32 avs) const throw(tcat::exception::base)
{
	int phase = 0;
	dice::FL_TEST_ACTION_PARAM fl;

	fl.cmdID = FL_TEST_CMD_GET_AVS_PHASE;
	fl.lvalue0 = avs;
	try
	{
		firmware_package (dice::FL_OP_TEST_ACTION, 0x2000, &fl, sizeof(fl)/sizeof(uint32));
		ohci_read (kTCAT_DICE_FW_OFFSET_FWL + TCAT_OFFSET(dice::FL_1394MEMORY,data.rtn.testRtn.data[1]), (uint32*)&phase);
	}
	catch (tcat::exception::base xptn)
	{
		std::cout << "Unable to retrieve AVS phase from firmware" << std::endl;
	}

	return phase;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::cmd_clear
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::cmd_clear()
{
	if (!s_qut)
		std::cout << nickname() << ": clear AVS receivers and driver notification lock/slip error counter" << std::endl;

	reset_counters();
	dice::FL_TEST_ACTION_PARAM fl;
	fl.cmdID=FL_TEST_CMD_CLR_AVS_CNT;
	try
	{
		firmware_package (dice::FL_OP_TEST_ACTION, 0x2000, &fl, sizeof(fl)/sizeof(uint32));
	}
	catch (tcat::exception::base xptn)
	{
		std::cout << "Unable to clear AVS stats in firmware" << std::endl;
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::cmd_mymode
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::cmd_mymode(std::string argv)
{
	dice::FL_TEST_ACTION_PARAM fl;
	fl.cmdID=FL_TEST_CMD_SET_MODE;
	if (argv.size())
	{
		fl.lvalue0 = from_string<uint32>(argv);
		if (!s_qut)
			std::cout << nickname() << ": set to mode #" << fl.lvalue0 << std::endl;
		firmware_package (dice::FL_OP_TEST_ACTION, 0x2000, &fl, sizeof(fl)/sizeof(uint32));
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::cmd_nickname
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::cmd_nickname(std::string argv)
{
	if (!s_qut)
		std::cout << "nickname:\"" << nickname() << "\"";
	#ifndef TCAT_DICE_AVC
	if (argv.size())
	{
		nickname(argv);
		TCAT_EXCEPTION_TRY(bus()->ready_wait(),dice::xptn_device_timeout,dice::xptn_device_timeout);
		if (!s_qut)
			std::cout << " set to:\"" << nickname() << "\"" << std::endl;
	}
	else
	#endif
		std::cout << std::endl;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::cmd_loopback
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::cmd_loopback(std::string argv)
{

	int width = 
#ifndef TCAT_DICE_AVC
		bus()->clock()>=tcat::dice::kClock176 ? 2:
#endif//TCAT_DICE_AVC
		1;
	size_t chnls = std::min<size_t>(channels(true).size(), channels(false).size());
	unsigned int chnl;
	if (argv.size())
	{
		chnl = from_string<int>(argv);
		if (!s_qut)
			std::cout << nickname() << ": creating loopback on chnl #" << chnl << std::endl;
		firmware_router (0xce000400+chnl*4,0xb0b0+chnl*0x101);
		TCAT_EXCEPTION_THROW (firmware_router(0xce000400+chnl*4)!=0xb0b0+chnl*0x101, xptn_feature);
	}
	else
	{
		if (!s_qut)
			std::cout << nickname() << ": creating " << std::dec << chnls << " loopbacks of " << width << " AVS sequence(s):";
		uint32 xcross = dynamic_cast<sbus*>((dice::bus_impl*)bus())->xcross();
		for (chnl=0; chnl<chnls; chnl++)
		{
			uint32 xchnl = xcross?TCAT_DICE_CHANNEL_CROSS(chnl,xcross):chnl;
			uint32 avs_in = channels(false)[xchnl].avs();
			uint32 avs_out = channels(true)[chnl].avs();
			for (int squ=0;squ<width;squ++) // sequences per audio channel (1=lo/mid 2=hi)
			{
				uint32 routing = 0xb0b0 + 
					avs_out * 0x1000 + ((chnl % sequence(true,avs_out).audio)*width+squ) * 0x100 + 
					avs_in *  0x10 +    (xchnl % sequence(false,avs_in).audio)*width+squ;

				firmware_router (0xce000400+(chnl*width+squ)*4, routing);
				TCAT_EXCEPTION_THROW (firmware_router(0xce000400+(chnl*width+squ)*4)!=routing, xptn_feature);
				if (s_log)
					std::clog << 
						std::resetiosflags(std::ios::left) << std::setiosflags(std::ios::right) << 
						std::dec << std::setw(2) << chnl+1 << "[" <<
						avs_out << "] :" <<
						std::resetiosflags(std::ios::right) << std::setiosflags(std::ios::left) << 
						std::setw(20) << 
						channels(false).at(chnl) <<
						" ->" <<
						std::resetiosflags(std::ios::left) << std::setiosflags(std::ios::right) << 
						std::setw(2) << xchnl+1 << "[" << 
						avs_in << "] :" <<
						std::resetiosflags(std::ios::right) << std::setiosflags(std::ios::left) << 
						std::setw(20) << channels(true).at(xchnl) << " " <<
						std::hex << routing << std::endl;
				else if (!s_qut)
					std::cout << "." << std::flush;
			}
		}
		if (!s_qut)
			std::cout << std::endl;
	}
	firmware_router(0xce000000,(uint32)(chnls*width-1) * 0x100 | 1);
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::cmd_firmware_upload
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::cmd_firmware_upload(std::string argv)
{
	if (argv.size())
	{
		std::size_t idx = argv.find(",");
		std::string sector;
		std::string filename;
		if (idx!=std::string::npos)
		{
			sector = argv.substr(0,idx);
			filename = argv.substr(idx+1);
		}
		else
		{
			sector = "dice\0";
			filename = argv;
		}
		if (sector=="dice")
		{
			// check firmware binary file
			try
			{
				dice::FL_GET_APP_INFO_RETURN image = firmware_info();
				dice::FL_GET_APP_INFO_RETURN file = firmware_info(filename);
				// binary valid: check compatibility and version
				if (file.uiVendorID != image.uiVendorID ||
					file.uiProductID != image.uiProductID ||
					file.uiBaseSDKVersion < image.uiBaseSDKVersion ||
					file.uiApplicationVersion < image.uiApplicationVersion)
				{
					std::string in;
					std::cout << 
						nickname() << ": are you sure to upload firmware binary [" << filename << 
						", vendor:" << std::hex << file.uiVendorID << 
						", product:" << file.uiProductID << 
						", SDK:" << std::dec << VERSION(file.uiBaseSDKVersion) << 
						", app:" << VERSION(file.uiApplicationVersion) << 
						"] to Sector [" << sector << 
						", SDK:" << VERSION(image.uiBaseSDKVersion) << 
						", app:" << VERSION(image.uiApplicationVersion) << 
						"] (Y/N)" << std::endl;
					std::cin >> in;
					if (in[0]!='y' && in[0]!='Y')
						return;
				}
				std::cout << nickname() << 
					": upload firmware [" << filename << 
					", vendor:" << std::hex << file.uiVendorID << 
					", product:" << file.uiProductID << 
					", SDK:" << std::dec << VERSION(file.uiBaseSDKVersion) << 
					", app:" << VERSION(file.uiApplicationVersion) << 
					"] to Sector [" << sector << "]" << std::endl;
			}
			catch (tcat::exception::base Exception)
			{
				eout(Exception);
				return;
			}
		}
		else
			std::cout << nickname() << ": upload binary [" << filename << "] to Sector [" << sector << "]" << std::endl;
		
		uint32 filesize;
		if (std::string(kTCAT_DICE_DRV_STR)=="TCNear")
			filesize = firmware_upload(filename, sector, true);
		else
		{
			filesize = firmware_upload(filename, sector);
			if (!(firmware_capabilities() & FL_CAP_AUTOERASE))
				try
				{
					firmware_delete (sector);
				}
				catch (...)
				{
					// there is no choice: continue and try to create new sector...
				}
		}
		firmware_create(sector,filesize);
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::cmd_firmware_download
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::cmd_firmware_download(std::string argv)
{
	if (argv.size())
	{
		std::size_t idx = argv.find(",");
		std::string sector;
		std::string filename;
		if (idx!=std::string::npos)
		{
			sector = argv.substr(0,idx);
			filename = argv.substr(idx+1);
		}
		else
		{
			sector = "";
			filename = argv;
		}
		unsigned int size = firmware_download(filename);
		std::cout << std::dec << size << " bytes downloaded from " << nickname() << " into " << filename << std::endl;
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::cmd_firmware_delete
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::cmd_firmware_delete(std::string argv)
{
	if (argv.size())
	{
		dice::FL_GET_IMAGE_DESC_RETURN fw;
		uint32 sector_idx=0;
		// find firmware sector to get current firmware description
		while (firmware_info(sector_idx++,fw))
			if (argv==fw.name)
				break;
		if (argv!=fw.name) return;
		std::cout << 
			nickname() << ": are you sure to delete firmware sector #" << sector_idx << 
			"[" << argv << 
			", version:" << fw.uiVersionHigh << "." << fw.uiVersionLow << 
			", legnth:" << fw.length <<
			", size:" << fw.size <<
			"] (Y/N)" << std::endl;
		std::string in;
		std::cin >> in;
		if (in[0]!='y' && in[0]!='Y')
			return;
		firmware_delete(argv);
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::cmd_dump
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::cmd_dump(std::string argv)
{
	std::cout << nickname() << ": dump:";
	if (argv.size())
	{
		uint32 data[0x1000];
		size_t separation = argv.find(",");
		int size;
		if (separation != std::string::npos)
		{
			size = from_string<int>(argv.substr(separation+1));
			argv = argv.substr(0, separation);
		}
		else
			size = 16;
		int addr = from_string<int>(argv);
			
		std::cout << std::setiosflags(std::ios::right) << std::resetiosflags(std::ios::left) << std::hex << addr << ":" << size << ":" << std::endl;
		ohci_read(addr, data, size);
		int offset = 0;
		while (size>0)
		{
			std::cout << std::hex << std::setw(8) << (addr | 0xe0000000) << ":   ";
			std::string ascii("");
			for (int i=0; (i<8) && (size>0); i++)
			{
				uint32 data32 = data[offset++];
				std::cout << std::setw(8) << data32 << " ";
				for (int idx=4; idx; idx--)
				{
					char chr = (char)(data32 & 0xff);
					ascii += (chr>=32 && chr<127)?chr:'.';
					data32 >>= 8;
				}
				size--;
				addr+=4;
			}
			std::cout << " [" << ascii << "]" << std::endl;
		}
	}
	else
		std::cout << std::endl;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::cmd_write
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::cmd_write(std::string argv)
{
	std::cout << nickname() << ": write:";
	if (argv.size())
	{
		uint32 data[0x1000];
		size_t separation = argv.find(",");
		if (separation == std::string::npos)
		{
			std::cout << " expects ADDR,FILENAME" << std::endl;
			return;
		}
		int addr = from_string<int>(argv.substr(0, separation));
		std::ifstream file(argv.substr(separation+1).c_str(), std::ios::in | std::ios::app);
	
		int offset = 0;
		std::string in;
		while (!file.eof())
		{
			file >> in;
			data[offset++] = from_string<unsigned int>(in);
		}	
		ohci_write(addr, data, offset);
		std::cout << std::hex << addr << ":" << offset;
	}
	std::cout << std::endl;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::cmd_phase
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::cmd_phase()
{
	std::cout << nickname() << ": phase alignment of";
	dice::FL_TEST_ACTION_PARAM fl;
	int32 data[2];
	fl.cmdID = FL_TEST_CMD_GET_AVS_PHASE;
	for (unsigned int avs=0; avs<isoc_streams(false); avs++)
	{
		fl.lvalue0 = avs;
		firmware_package (dice::FL_OP_TEST_ACTION, 0x2000, &fl, sizeof(fl)/sizeof(uint32));
		ohci_read (kTCAT_DICE_FW_OFFSET_FWL + TCAT_OFFSET(dice::FL_1394MEMORY,data.rtn.testRtn), (uint32*)data, sizeof(data)/sizeof(uint32));
		std::cout << " avs[" << avs << "]:" << data[0]/10. << "us, " << data[1] << "%";
	}
	std::cout << std::endl;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::cmd_bnc
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::cmd_bnc(std::string argv)
{
	std::cout << nickname() << ": WCKL BNC trigger: ";
	dice::FL_TEST_ACTION_PARAM fl;
	switch (tolower((argv.c_str()[0])))
	{
	case 'e':		
		std::cout << "enable" << std::endl;
		fl.cmdID = FL_TEST_CMD_ENABLE_BNC_SYNC;
		break;
	case 'p':
		std::cout << "pulse" << std::endl;
		fl.cmdID = FL_TEST_CMD_PULSE_BNC_SYNC;
		break;
	default:
		std::cout << argv << " unknown" << std::endl;
		return;
	}
	firmware_package (dice::FL_OP_TEST_ACTION, 0x2000, &fl, sizeof(fl)/sizeof(uint32));
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::cmd_busy
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::cmd_busy(std::string argv)
{
	dice::FL_TEST_ACTION_PARAM fl;
	fl.cmdID = FL_TEST_CMD_EMUL_SLOW_CMD;
	fl.lvalue0 = from_string<uint32>(argv);
	std::cout << nickname() << ": busy for " << fl.lvalue0 << "ms" << std::endl;
	firmware_package (dice::FL_OP_TEST_ACTION, 0x2000, &fl, sizeof(fl)/sizeof(uint32));
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::firmware_progress
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::firmware_progress(uint32 percent, dice::FL_OP_ENUM operation) const
{
	const std::string desc[] = 
	{
		"Retrieving Image Description",	// FL_OP_GET_IMAGE_DESC,	///< parameters: imageId  return: imageDesc
		"Deleting Flash Image",			// FL_OP_DELETE_IMAGE,		///< parameters: name  return: none
		"Programing Flash Image",		// FL_OP_CREATE_IMAGE,		///< parameters: execAddr, entryAddr, name  return: none
		"Uploading Binary",				// FL_OP_UPLOAD,			///< parameters: index, length, data, return none
		"Checking Data",				// FL_OP_UPLOAD_STAT,		///< parameters: length return checksum
		"Reset Device",					// FL_OP_RESET_IMAGE,		///< parameters: none
		"",								// FL_OP_BUTTON,			///< ...
		"",								// FL_OP_GET_FLASH_INFO,	///< ...
		"Firmware Download",			// FL_OP_READ_MEMORY,		///< ...
		"",								// FL_OP_NOOP,				///< ...
		"",								// FL_OP_GET_RUNNING_IMAGE_VINFO	///< parameters: none  return: vendorInfo
	};
	std::cout << nickname() << ": " << std::dec << percent << "% " << desc[operation] << std::flush << "               \r";
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus
//
///////////////////////////////////////////////////////////////////////////////
//
//	sbus::create
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::create(dice::system_impl* system, const dice::bus_impl::ID id)		throw(tcat::exception::base)
{
	if (s_trc)
		std::clog << "  " << TCAT_FUNCTION << "[" << std::hex << id << "]" << std::endl;
	super::create(system,id);
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::~sbus
//
///////////////////////////////////////////////////////////////////////////////
//
sbus::~sbus() 
{
	if (s_trc)
		std::clog << "  " << TCAT_FUNCTION << "[" << std::hex << super::id() << "] chnls: " << m_channel_names[0].size() << "," << m_channel_names[1].size() << std::endl;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::reset_counters
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::reset_counters()							
{
	for (int ctr=0; ctr<kProcessCounters; ctr++)
		m_process_counter[ctr].reset();
	for (int channel=0;channel<kTCAT_DICE_MAX_CHANNELS;channel++)
		for (int ctr=0; ctr<kChannelCounters; ctr++)
			m_counters[channel].counters[ctr].reset();
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::open
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::open() throw(tcat::exception::base)
{ 
	if (s_trc)
		std::clog << "  " << TCAT_FUNCTION << "[" << std::hex << super::id() << "]" << std::endl;
	dice::lock lock(this); 
	super::open(); 
	os_audio_open(); 
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::close
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::close() throw(tcat::exception::base)
{ 
	if (s_trc)
		std::clog << "  " << TCAT_FUNCTION << "[" << std::hex << super::id() << "]" << std::endl;
	os_audio_close(); 
	super::close(); 
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::update
//
///////////////////////////////////////////////////////////////////////////////
//
bool sbus::update() throw(tcat::exception::base)
{
	bool bReturn = super::update();
	if (bReturn)
	{
		if (s_trc)
			std::clog << "  " << TCAT_FUNCTION << "[" << std::hex << super::id() << "]:" << std::endl;
		if (s_log)
		{
#ifndef TCAT_DICE_AVC
			if (m_cache[0].config.clock != m_cache[1].config.clock)		
				std::cout << "  + clk:" << super::clock_name(m_cache[1].config.clock) << "->" << super::clock_name(m_cache[0].config.clock) << std::endl;
			if (m_cache[0].config.sync != m_cache[1].config.sync)		
				std::cout << "  + sync:" << super::sync_name(m_cache[1].config.sync) << "->" << super::sync_name(m_cache[0].config.sync) << std::endl;
			if (m_cache[0].config.master_guid64 != m_cache[1].config.master_guid64)	
				std::cout << "  + master:" << std::hex << m_cache[1].config.master_guid64 << "->" << m_cache[0].config.master_guid64 << std::endl;
			if (m_cache[0].op_mode != m_cache[1].op_mode)	
				std::cout << "  + op_mode:" << std::hex << m_cache[1].op_mode << "->" << m_cache[0].op_mode << std::endl;
#endif//TCAT_DICE_AVC
			#ifdef WIN32
			if (m_cache[0].config.io_size != m_cache[1].config.io_size)	
				std::cout << "  + io_size:" << std::hex << m_cache[1].config.io_size << "->" << m_cache[0].config.io_size << std::endl;
			for (uint32 idx=0;idx<2;idx++)
			{
	 			if (m_cache[0].speaker[idx].cfg != m_cache[1].speaker[idx].cfg)	
					std::cout << "  + " << (idx?"out":"in") << " speaker_config:" << std::hex << m_cache[1].speaker[idx].cfg << "->" << m_cache[0].speaker[idx].cfg << std::endl;
				for (int chn=0; chn<kTCAT_DICE_SPEAKER_MAPS; chn++)
				{
					if (m_cache[0].speaker[idx].map[chn] != m_cache[1].speaker[idx].map[chn])
						std::cout << "  + " << (idx?"out":"in") << 
						" speaker_map[" << chn << "]: " << 
						m_cache[0].speaker[idx].map[chn] << ":" << 
						(m_cache[1].speaker[idx].map[chn] < (int)m_cache[1].channels[idx].size() ? m_cache[1].channels[idx][m_cache[1].speaker[idx].map[chn]]:std::string("")) << " -> " << 
						m_cache[0].speaker[idx].map[chn] << ":" << 
						m_cache[0].channels[idx][m_cache[0].speaker[idx].map[chn]] << std::endl;
				}
			}
			#endif
		}
	}
	return bReturn;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::update_devicelist
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::update_devicelist() throw(tcat::exception::base)
{
	super::update_devicelist();
	if (s_trc)
		std::clog << "  " << TCAT_FUNCTION << "[" << std::hex << super::id() << "]:" << super::size() << " device(s)" << std::endl;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::progress
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::progress(uint32 percent, uint32 ctrl) const 
{
	static struct OP
	{
		uint32 ctrl;
		std::string txt;
	} op[] = 
	{
		{ super::kProgressOpen, "opening " kTCAT_DICE_USR_STR },
		{ kProgressOpenMidi, "opening MIDI" },
	};
	int idx;
	std::string txt = "operation";
	for (idx=0; idx<(int)(sizeof(op)/sizeof(OP)); idx++)
		if (op[idx].ctrl == ctrl)
		{
			txt=op[idx].txt;
			break;
		}
	std::clog << txt << ": " << percent << std::dec << "%" << "\r" << std::flush;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::process
//
///////////////////////////////////////////////////////////////////////////////
//
//	platform independent processing function.
//	sends out a unique sync signal on each channel and compares it with the
//	returned signal. latency, dropout and slip measurement is performed.
void sbus::process (int32* in[], int32* out[], uint64 sample_position)
{
	uint32 sample;
	int32 chnl;
	int32* psync;
	int32 sign;

	uint64 current = pal::time64wrap();	
	uint32 timer = (uint32)(uint64)(current - m_process_counter[kProcessCounterDriver].current);
	if (m_process_counter[kProcessCounterDriver].current)
	{
		m_process_counter[kProcessCounterDriver].min = std::min<uint32>(m_process_counter[kProcessCounterDriver].min,timer);
		m_process_counter[kProcessCounterDriver].max = std::max<uint32>(m_process_counter[kProcessCounterDriver].max,timer);
	}
	m_process_counter[kProcessCounterDriver].current = current;
	m_process_counter[kProcessCounterDriver].changes++;

	if (m_process_counter[kProcessCounterSync].current == 0)
	{
		// just latch the starting position the first time through
		m_process_counter[kProcessCounterSync].current = sample_position;
	}
	else
	{
		m_process_counter[kProcessCounterSync].current+=io_size();
		if (sample_position != m_process_counter[kProcessCounterSync].current)
		{
			if (m_process_counter[kProcessCounterSync].changes < kMaxErrorsToLog)
			{
				TDL_REALTIME_ENTRY( kTDLFlag_Error, "DiceTool: sample position mismatch  (expected %d, received %d)",
									m_process_counter[kProcessCounterSync].current, sample_position, 0, 0 );
			}
			m_process_counter[kProcessCounterSync].min = std::min<uint32>(m_process_counter[kProcessCounterDriver].min, (uint32)(uint64)(sample_position - m_process_counter[kProcessCounterSync].current));
			m_process_counter[kProcessCounterSync].max = std::max<uint32>(m_process_counter[kProcessCounterDriver].max, (uint32)(uint64)(sample_position - m_process_counter[kProcessCounterSync].current));
			m_process_counter[kProcessCounterSync].current = sample_position;
			if (!m_process_counter[kProcessCounterSync].changes++)
			{
				m_process_counter[kProcessCounterSync].first_occurence = m_process_counter[kProcessCounterSync].current;
			}
		}
	}

	// sending out the sync signal
	// 0x87654321
	//   cc			chnl information (255-chnl)
	//     sync		sync information (sawtooth)
	//         tt	24bit trash (not transported)
	//
	for (chnl=0; chnl<m_out; chnl++)
	{
		if (m_channel_mask & (uint64(1)<<chnl))
		{
			psync=out[chnl];
			sign = (127-chnl)<<24;
			// generating sync signal for each chnl
			for (sample=0; sample<io_size(); sample++)
				*psync++ = ((((uint32)sample_position+sample)&0xffff)<<8) | sign;
		}
		else
		{
			memcpy(out[chnl],in[chnl],io_size()*sizeof(int32));
		}
	}

	// synchronizing
	for (chnl=0; chnl<m_in; chnl++)
	{
		if (m_channel_mask & (uint64(1)<<chnl))
		{
			counter& latency=m_counters[chnl].counters[kChannelCounterLatency];
			counter& slip=m_counters[chnl].counters[kChannelCounterSlips];
			counter& dropouts=m_counters[chnl].counters[kChannelCounterDropouts];

			sign = 127 - chnl;
			if (m_xcross)
				psync=in[TCAT_DICE_CHANNEL_CROSS(chnl,m_xcross)];
			else
				psync=in[chnl];

			for (sample=0; sample<io_size(); sample++)
			{
				int32 signal = *psync++;
				if ((signal>>24)==sign)
				{
					// this is a valid signal
					uint32 latency_current = ((uint32)sample_position+sample - (signal>>8)) & 0xffff;
					if (latency_current != latency.current)
					{
						// latency changed: this is a slip
						if (latency.first_occurence)
						{
							// its not slipping from the initial gap
							if (slip.changes < kMaxErrorsToLog)
							{
								TDL_REALTIME_ENTRY( kTDLFlag_Error, "DiceTool: slip - latency change detected  (chnl %d, position %d, expected %d, measured %d)",
													chnl, (sample_position + sample), latency.current, latency_current );
							}
							slip.changes++;
							slip.current = std::abs(int(latency_current-latency.current));
							slip.min = std::min<uint32>((uint32)slip.current,slip.min);
							slip.max = std::max<uint32>((uint32)slip.current,slip.max);
							if (!slip.first_occurence)
								slip.first_occurence=(m_process_counter[kProcessCounterDriver].changes-1)*io_size()+sample-dropouts.current;
							m_counters[chnl].errors += slip.current;
						}
						latency.current = latency_current;
						latency.min = std::min<uint32>(latency_current,latency.min);
						latency.max = std::max<uint32>(latency_current,latency.max);
					}
					if (dropouts.current)
					{
						// its recovering from a drop out
						if (latency.first_occurence)
						{
							// its not the initial gap
							dropouts.min = std::min<uint32>((uint32)dropouts.current,dropouts.min);
							dropouts.max = std::max<uint32>((uint32)dropouts.current,dropouts.max);
							dropouts.changes++;
							if (!dropouts.first_occurence)
								dropouts.first_occurence=(m_process_counter[kProcessCounterDriver].changes-1)*io_size()+sample;
							m_counters[chnl].errors += dropouts.current;
						}
						else
						{
							latency.first_occurence = dropouts.current;
						}
						dropouts.current = 0;
					}
				}
				else
				{
					if (latency.first_occurence && (dropouts.current < kMaxErrorsToLog))
					{
						TDL_REALTIME_ENTRY( kTDLFlag_Error, "DiceTool: dropout - chnl bits mismatch  (chnl %d, position %d, data 0x%08x, expected chnl 0x%02x)",
											chnl, (sample_position + sample), signal, sign );
					}
					dropouts.current++;		// this is a drop out
				}
			}
		}
		else
		{
			memset(out[chnl],0,io_size()*sizeof(int32));
		}
	}

	if (m_load)
		pal::delay(m_load);

	timer = (uint32)(uint64)(pal::time64wrap() - current);
	m_process_counter[kProcessCounterUser].min = std::min<uint32>(m_process_counter[kProcessCounterUser].min,timer);
	m_process_counter[kProcessCounterUser].max = std::max<uint32>(m_process_counter[kProcessCounterUser].max,timer);
}



#ifndef TCAT_DICE_AVC
///////////////////////////////////////////////////////////////////////////////
//
//	sbus::clock
//
///////////////////////////////////////////////////////////////////////////////
//
//	changing the clock requires the OS audio drivers to be closed 
void sbus::clock (dice::CLOCK clock) throw(tcat::exception::base)
{
	os_audio_close();
	super::clock(clock);
	TCAT_EXCEPTION_TRY(ready_wait(),dice::xptn_device_timeout,dice::xptn_device_timeout);
	os_audio_open();
}
#endif//TCAT_DICE_AVC



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::os_audio_check
//
///////////////////////////////////////////////////////////////////////////////
//
bool sbus::os_audio_check()
{
	if (m_restart_request)
	{
		m_restart_request = false;
		os_audio_stop();
		os_audio_close();
		os_audio_open();
		os_audio_start();
		return true;
	}
	return false;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::cmd_info
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::cmd_info()
{
	if (s_trc)
		std::clog << TCAT_FUNCTION << std::endl;
	uint32 version = driver_version();
	std::cout << "  bus " << id() << std::endl; 
	std::cout << "    drv version: " << std::dec << VERSION(version) << std::endl;
	std::cout << "      client count: " << clients() << std::endl;
	std::cout << "    streaming drv information:" << std::endl;
	std::cout << "      audio driver: " << name() << std::endl;
	#ifndef TCAT_DICE_AVC
	std::cout << "      i/o size: " << std::dec << io_size() << std::endl;
	std::cout << "      clock: " << clock_name(clock()) << ": " << sync_name(sync()) << std::endl;
	#endif//TCAT_DICE_AVC

	std::cout << "      latency in: " << 
		latency(false) << ", out: " << 
		latency(true) << ", in+out: " << 
		latency(false)+latency(true) << std::endl;
	std::cout << "      " << std::setiosflags(std::ios::right) << std::dec << std::setw(2) << 
		channels_in() << " inputs:                         |" << 
		channels_out() << " outputs" << std::resetiosflags(std::ios::right) << std::endl;
	unsigned int chnl;
	for (chnl=0; chnl<std::max<uint32>(channels_in(),channels_out()); chnl++)
	{
		if (chnl<channels_in())
			std::cout << 
				std::setiosflags(std::ios::right) << "      " << std::setw(2) << chnl << std::resetiosflags(std::ios::right) << ":" <<
				std::setiosflags(std::ios::left) << std::setw(32) << channel_name(true,chnl) << "|";
		else
			std::cout << "                                    |";
		if (chnl<channels_out())
			std::cout << 
				std::setiosflags(std::ios::right) << std::setw(2) << chnl << std::resetiosflags(std::ios::right) << ":" <<
				channel_name(false,chnl);
		std::cout << std::endl;
	}
	std::cout << std::dec << std::endl;
	#ifndef TCAT_DICE_AVC
	dice::ioctl::BUS_ERROR error = errors();
	out0(error.error[dice::ioctl::BUS_ERROR::kBusErrorWatchdogTimer]);
	out0(error.error[dice::ioctl::BUS_ERROR::kBusErrorMissedCallbacks]);
	out0(error.error[dice::ioctl::BUS_ERROR::kBusErrorDbcFail]);
	out0(error.error[dice::ioctl::BUS_ERROR::kBusErrorSytFail]);
	out0(error.error[dice::ioctl::BUS_ERROR::kBusErrorUsermodeDelays]);
	#endif//TCAT_DICE_AVC
	{
		dice::lock lock(this);
		std::cout << "      number of devices: " << (uint32) size() << std::endl;
//		for (const_iterator device=begin(); device!=end(); device++)
//			(*device)->info();
	}
}



#ifndef TCAT_DICE_AVC
///////////////////////////////////////////////////////////////////////////////
//
//	tcat::sbus::cmd_sync
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::cmd_sync(std::string argv)
{
	if (!s_qut)
		std::cout << "sync source: " << sync_name(sync());
	if (argv.size())
	{
		int snc;
		for (snc=0; snc<dice::kSyncItems; snc++)
			if (argv==sync_name((dice::SYNC)snc))
				break;
		if (snc<dice::kSyncItems)
		{
			sync((dice::SYNC)snc);
			TCAT_EXCEPTION_TRY(ready_wait(),dice::xptn_device_timeout,dice::xptn_device_timeout);
			if (!s_qut)
				std::cout << " set to:" << sync_name(sync()) << std::endl;
		}
		else
		{
			std::cout << std::endl << "sync source \"" << argv << "\" does not exist." << std::endl;
			std::cout << "use one of this sync sources:" << std::endl;\
			for (snc=0; snc<dice::kSyncItems; snc++)
				std::cout << "  " << sync_name((dice::SYNC)snc) << std::endl;
		}
	}
	else if (!s_qut)
		std::cout << std::endl;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::cmd_clock
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::cmd_clock(std::string argv)
{
	if (!s_qut)
		std::cout << "clock: " << clock_name(clock());
	if (argv.size())
	{
		switch (from_string<int>(argv))
		{
		case 32:
		case 32000:
			clock(dice::kClock32);
			break;
		case 44:
		case 441:
		case 44100:
			clock(dice::kClock44);
			break;
		case 48:
		case 48000:
			clock(dice::kClock48);
			break;
		case 88:
		case 882:
		case 88000:
			clock(dice::kClock88);
			break;
		case 96:
		case 96000:
			clock(dice::kClock96);
			break;
		case 176:
		case 176000:
			clock(dice::kClock176);
			break;
		case 192:
		case 192000:
			clock(dice::kClock192);
			break;
		default:
			std::cout << "; \"" << argv << "\" not supported." << std::endl;
			return;
		}
		TCAT_EXCEPTION_TRY(ready_wait(),dice::xptn_device_timeout,dice::xptn_device_timeout);
		if (!s_qut)
			std::cout << " set to " << clock_name(clock()) << std::endl;
	}
	else if (!s_qut)
		std::cout << std::endl;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::cmd_iosize
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::cmd_iosize(std::string argv)
{
	bool		belowMinimum = false;

	if (!s_qut)
	{
#ifdef WIN32
		std::cout << "io size: " << io_size() << "  (driver minimum: " << min_io_size() << " maximum: " << max_io_size() << ")";
#else
		std::cout << "io size: " << io_size();
#endif
	}

	if (argv.size())
	{
		try
		{
			io_size(from_string<uint32>(argv));
		}
		catch(...)
		{
			if (!s_qut)
				std::cout << std::endl;

			std::cout << "Attempted to set buffer size to " << from_string<uint32>(argv);
#ifdef WIN32
			std::cout << " which is below the current driver minimum (" << min_io_size() << ")" << std::endl;
#endif
			belowMinimum = true;
		}
		TCAT_EXCEPTION_TRY(ready_wait(),dice::xptn_device_timeout,dice::xptn_device_timeout);
		if (!s_qut && !belowMinimum)
			std::cout << " set to:" << io_size() << std::endl;
	}
	else
		std::cout << std::endl;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::cmd_master
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::cmd_master(std::string argv)
{
	uint64 master_guid = master();
	if (!s_qut)
		std::cout << "sync master:\"" << 
			find(master_guid)->nickname() << 
			"\" (GUID:" << std::hex << master_guid << ")";

	if (argv.size())
	{
		sdevice_ref device;
		{
			dice::lock lock(this);
			device = find(argv);	// try nickname
			if (!device)
				device = find(from_string<uint64>(argv)); // try GUID
			if (!device)
			{
				unsigned int master_idx = from_string<unsigned int>(argv); // try index
				if (master_idx<size())
					device = at(master_idx);
			}
		}
		if (device)
		{
			master(device->guid64());
			TCAT_EXCEPTION_TRY(ready_wait(),dice::xptn_device_timeout,dice::xptn_device_timeout);
			if (!s_qut)
				std::cout << " set to:\"" << 
					device->nickname() << 
					"\" (GUID:" << device->guid64() << ")" << std::endl;
			return;
		}
		else
		{
			std::cout << std::endl << "sync master source \"" << argv << "\" does not exist." << std::endl;
		}
	}
	else
		std::cout << std::endl;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::cmd_op_mode
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::cmd_op_mode(std::string argv)
{
	if (!s_qut)
		std::cout << "op_mode: " << op_mode();
	if (argv.size())
	{
		op_mode(dice::OP_MODE(from_string<int>(argv)));
		TCAT_EXCEPTION_TRY(ready_wait(),dice::xptn_device_timeout,dice::xptn_device_timeout);
		if (!s_qut)
			std::cout << " set to " << op_mode() << std::endl;
	}
	else
		std::cout << std::endl;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::cmd_add_latency
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::cmd_add_latency(std::string argv)
{
	if (argv.size())
	{
		size_t separation = argv.find(",");
		if (separation == std::string::npos)
		{
			std::cout << " expects IN,OUT" << std::endl;
			return;
		}

		uint32	inputSamples = from_string<uint32>(argv.substr(0, separation));
		uint32	outputSamples = from_string<uint32>(argv.substr(separation+1));

		add_reported_audio_latency( inputSamples, outputSamples );
		if (!s_qut)
			std::cout << "cmd_add_latency: in " << inputSamples << ", out " << outputSamples;
	}
	std::cout << std::endl;
}



#endif//TCAT_DICE_AVC



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::cmd_channel_mask
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::cmd_channel_mask(std::string argv)
{
	if (!s_qut)
		std::cout << "channel mask:" << std::hex << channel_mask();
	if (argv.size())
	{
		channel_mask(from_string<uint64>(argv));
		if (!s_qut)
			std::cout << " set to:" << channel_mask();						
	}
	std::cout << std::endl;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::qi
//
///////////////////////////////////////////////////////////////////////////////
//
double sbus::qi()
{
	int chnls = std::min<uint32>(channels_in(),channels_out());
	double qi;

	if (process_counter(kProcessCounterDriver).min == 0xffffffff)
	{
		qi=10.;
	}
	else if (chnls)
	{
		qi = 0.;	
		int chnl = chnls;
		while (chnl--)
			qi += channel_errors(chnl);
		if (qi>0.)
		{
			qi = qi/((double)process_counter(kProcessCounterDriver).changes*(double)io_size()*(double)chnls);
			if (qi>1.) qi=1.;
			qi = 10.+log10 (qi);
			if (qi<1.) qi=1.;
		}
	}
	else
	{
		qi=10.;
	}

	return qi;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::ma
//
///////////////////////////////////////////////////////////////////////////////
//
int sbus::ma()
{
	unsigned int ma_min = 0x7fffffff;
	unsigned int ma_max = 0;
	int chnls = std::min<uint32>(channels_in(),channels_out());
	int chnl = chnls;
	while (chnl--)
	{
		ma_min = TCAT_MIN (ma_min, (uint32)channel_counter(chnl,sbus::kChannelCounterLatency).current);
		ma_max = TCAT_MAX (ma_max, (uint32)channel_counter(chnl,sbus::kChannelCounterLatency).current);
	}
	return ma_max - ma_min;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::log_streaming_head
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::log_streaming_head()
{
	unsigned int chnls = std::min<uint32>(channels_in(), channels_out());
	unsigned int chnl;
	if (!s_qut)
	{
		dice::lock lock(this);
#ifdef WIN32
		// set win console width (does not work from inside a script!)
		CONSOLE_SCREEN_BUFFER_INFO info;
		::GetConsoleScreenBufferInfo(::GetStdHandle(STD_OUTPUT_HANDLE),&info);
		if (info.dwMaximumWindowSize.X < (SHORT)std::min<uint32>(channels_in(), channels_out())*11 + 56)
			info.dwMaximumWindowSize.X = (SHORT)std::min<uint32>(channels_in(), channels_out())*11 + 56;
		::SetConsoleScreenBufferSize(::GetStdHandle(STD_OUTPUT_HANDLE),info.dwMaximumWindowSize);
#endif //WIN32
		// output channel headline
		if (s_audio)
		{
			std::clog << "i/o:"; 
			logf0_(io_size(),4,9999);
			#ifndef TCAT_DICE_AVC
			std::clog << "clk:" << std::setw(7) << clock_name(clock());
			#endif//TCAT_DICE_AVC		
			std::clog <<  "      outputs  ";
			for (chnl=0; chnl<std::min<uint32>(channels_in(), channels_out()); chnl++)
				if (channel_mask()&(uint64(1)<<chnl))
					std::clog << "|" << std::dec << std::setw(2) << chnl+1 << ":" << std::setw(7) <<
					channel_name(false,chnl).substr(std::max<int>((int)channel_name(false,chnl).size()-7,0));
		}
		else
			std::clog << "                            ";
		if (s_midi)
			for (sbus::iterator device=begin(); device!=end(); device++)
				for (std::deque<sdevice::os_midi>::iterator midi=(*device)->m_midi.begin(); midi!=(*device)->m_midi.end(); midi++)
					std::clog << "|MIDI";
		std::clog << std::endl;

		// input channel headline
		std::clog << "time";
		if (s_audio)
		{
			std::clog << "|                     inputs   ";
			for (chnl=0; chnl<chnls; chnl++)
				if (channel_mask()&(uint64(1)<<chnl))
				{
					int xchnl;
					if (xcross())
						xchnl=TCAT_DICE_CHANNEL_CROSS(chnl,xcross());
					else
						xchnl=chnl;
					std::clog << "|" << std::dec << std::setw(2) << xchnl+1 << ":" << std::setw(7) <<
					channel_name(true,xchnl).substr(std::max<int>((int)channel_name(true,xchnl).size()-7,0));
				}
		}
		else
			std::clog << "|                       ";
		if (s_midi)
		{
			chnl=0;
			for (sbus::iterator device=begin(); device!=end(); device++)
			{
				chnl++;
				for (std::deque<sdevice::os_midi>::iterator midi=(*device)->m_midi.begin(); midi!=(*device)->m_midi.end(); midi++)
					std::clog << "| " << std::setw(1) << chnl << ":" << (*midi).m_port_idx+1;
			}
		}
		std::clog << std::endl;

		// description headline
		std::clog << "left|sl ul rs|wd mc db sy um";
		if (s_audio)
		{
			std::clog << "| clock";
			for (chnl=0; chnl<chnls; chnl++)
				if (channel_mask()&(uint64(1)<<chnl))
					std::clog << "|sl dr ltcy";
		}
		if (s_midi)
			for (sbus::iterator device=begin(); device!=end(); device++)
				for (std::deque<sdevice::os_midi>::iterator midi=(*device)->m_midi.begin(); midi!=(*device)->m_midi.end(); midi++)
					std::clog << "| Bps";
		std::clog << std::endl;
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::log_streaming
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::log_streaming(uint32 time_elapsed, uint32 time_left, int32 dpc)
{
	int chnl;
	int chnls = std::min<uint32>(channels_in(), channels_out());
	dice::lock lock(this);
	if (!s_qut)
	{
		// count down
		std::clog << 
			std::resetiosflags(std::ios::left) << std::setiosflags(std::ios::right) << 
			std::setw(4) << std::min<uint32>((time_left)/1000,9999) << "|";

		// driver reported slips and unlocks
		int slips = 0;
		int unlocks = 0;
		logf0_(slips,2,99)
		logf0_(unlocks,2,99)
		logf0(os_audio_restarts(),2,99)
		#ifndef TCAT_DICE_AVC
		dice::ioctl::BUS_ERROR err = errors();
		for (int i=0; i<dice::ioctl::BUS_ERROR::kBusErrorItems; i++)			
			if (i<(dice::ioctl::BUS_ERROR::kBusErrorItems-1))
				logf0_(err.error[i],2,99)
			else
				logf0(err.error[i],2,99)
		#endif//TCAT_DICE_AVC

		// measure clock rate
		if (s_audio)
			if (time_elapsed)
				std::clog << std::setw(6) << uint64(process_counter(kProcessCounterDriver).changes)*uint64(io_size()*1000)/time_elapsed;
			else
				std::clog << "      ";

		// per channel slips, drops and latency
		for (chnl=0; chnl<chnls; chnl++)
			if (channel_mask()&(uint64(1)<<chnl))
			{
				std::clog << "|";
				logf0_ (channel_counter(chnl,sbus::kChannelCounterSlips).changes,2,99);
				logf0_ (channel_counter(chnl,sbus::kChannelCounterDropouts).changes,2,99);

				if (channel_counter(chnl,sbus::kChannelCounterDropouts).current || !process_counter(kProcessCounterDriver).changes)
					std::clog << "----";
				else
					std::clog << std::dec << std::setw(4) << std::min<int32>((uint32)channel_counter(chnl,sbus::kChannelCounterLatency).current,9999);
			}
		{
		for (sbus::iterator device=begin(); device!=end(); device++)
			for (std::deque<sdevice::os_midi>::iterator midi=(*device)->m_midi.begin(); midi!=(*device)->m_midi.end(); midi++)
			{
				std::clog << "|" << std::dec;
				if (time_elapsed && (*midi).m_counter.changes)
					std::clog << std::setw(4) << std::min<int32>((*midi).m_counter.changes*1000/time_elapsed,9999);
				else
					std::clog << "----";
			}
		}
		#ifdef WIN32
		if (dpc)
			std::clog << std::dec << dpc;
		else
			std::clog << "n/a";
		#endif
	}
	else
	{
		// count down
		std::clog << "time:" << time_left/1000 << " io:" << io_size() 
		#ifndef TCAT_DICE_AVC
		<< " cl:" << clock_name(clock())
		#endif//TCAT_DICE_AVC
		; 

		chnl=0;
		if (process_counter(kProcessCounterDriver).changes)
		{
			int limit=3;
			while (chnl<chnls)
			{
				unsigned int first_channel = chnl;

				// group results
				for (;chnl<chnls && 
					channel_counter(chnl,sbus::kChannelCounterLatency).current==	channel_counter(first_channel,sbus::kChannelCounterLatency).current &&
					channel_counter(chnl,sbus::kChannelCounterDropouts).changes==	channel_counter(first_channel,sbus::kChannelCounterDropouts).changes &&
					channel_counter(chnl,sbus::kChannelCounterSlips).changes==		channel_counter(first_channel,sbus::kChannelCounterSlips).changes &&
					((channel_mask()>>chnl)&1)==((channel_mask()>>first_channel)&1);
					chnl++);
				
				if (channel_mask()&(uint64(1)<<first_channel))
				{
					if (!limit--)
					{
						std::clog << " ... ";
						break;
					}
					// outs
					std::clog << " | ch:" << first_channel+1;
					if ((int)first_channel!=chnl-1)
						std::clog << "-" << chnl;

					if (channel_counter(chnl,sbus::kChannelCounterDropouts).current)
						std::clog << " ----";
					else
						std::clog << " lt:" << channel_counter(first_channel,sbus::kChannelCounterLatency).current;
					std::clog << " dr:" << channel_counter(first_channel,sbus::kChannelCounterDropouts).changes;
					std::clog << " sl:" << channel_counter(first_channel,sbus::kChannelCounterSlips).changes;
				}
			}
		}
		else
			std::clog << " not streaming!";
		
		uint32 min=0xfffffff;	// min midi package count
		int idx=0;	// device idx
		int grp=0;	// count grouping of midi ports
		for (sbus::iterator device=begin(); device!=end(); device++, idx++)
			for (std::deque<sdevice::os_midi>::iterator midi=(*device)->m_midi.begin(); midi!=(*device)->m_midi.end(); midi++)
			{
				if (min==0xfffffff)
					min=(*midi).m_counter.changes;
				else if ((min != 0) ^ ((*midi).m_counter.changes != 0))
				{
					if (grp)
						std::clog << "[" << grp << "]:>";
					if (min && time_elapsed)
						std::clog << min*1000/time_elapsed;
					else
						std::clog << "not streaming!";
						min=(*midi).m_counter.changes;
					grp=0;
				}
				else
				{
					min=std::min<uint32>(min,(*midi).m_counter.changes);
					grp++;
				}
				if (!grp)
					std::clog << " | midi" << idx+1 << "." << (*midi).m_port_idx+1 << ":";
			}
		if (min!=0xfffffff)
		{
			if (grp)
				std::clog << "[" << grp << "]:>";
			if (min && time_elapsed)
				std::clog << min*1000/time_elapsed;
			else
				std::clog << "not streaming!";
		}
			
		#ifdef WIN32
		std::clog << "                       ";
		#endif//WIN32
		#ifdef __MACH__
		std::clog << (char)27 << "[0K";
		#endif//__MACH__
	}
	std::clog << "\r";
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::log_streaming_synopsis
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::log_streaming_synopsis(uint32 duration, int32 dpc)
{
	dice::lock lock(this);
	uint32 chnl;
	uint32 chnls=std::min<uint32>(channels_in(),channels_out());

	if (!s_qut) std::cout << std::endl;
	std::cout << std::dec <<
		process_counter(kProcessCounterDriver).changes << " packets at " <<
		io_size() << " samples (" <<
		channels_in() << " in, " <<
		channels_out() << " out) streamed for " <<
		(duration)/1000 << "sec. at ~" <<
		uint64(process_counter(kProcessCounterDriver).changes)*uint64(io_size()*1000)/duration <<
		"Hz, SQI:" << qi() <<
		", MA:" << ma() <<
		", drops:" << m_process_counter[kProcessCounterSync].changes <<
		", drv: min:" << process_counter(kProcessCounterDriver).min <<	"us(" << (io_size()?(process_counter(kProcessCounterDriver).min*clock_rate())/(10000*io_size()):0) <<
		"%)-max:" << process_counter(kProcessCounterDriver).max <<		"us(" << (io_size()?(process_counter(kProcessCounterDriver).max*clock_rate())/(10000*io_size()):0) <<
		"%), proc: min:" << process_counter(kProcessCounterUser).min <<	"us(" << (io_size()?(process_counter(kProcessCounterUser).min*clock_rate())/(10000*io_size()):0) <<
		"%)-max:" << process_counter(kProcessCounterUser).max <<		"us(" << (io_size()?(process_counter(kProcessCounterUser).max*clock_rate())/(10000*io_size()):0) << "%)";

	uint32 min=0xfffffff;	// min midi package count
	int idx=0;	// device idx
	int grp=0;	// count grouping of midi ports
	for (sbus::iterator device=begin(); device!=end(); device++, idx++)
		for (std::deque<sdevice::os_midi>::iterator midi=(*device)->m_midi.begin(); midi!=(*device)->m_midi.end(); midi++)
		{
			if (min==0xfffffff)
				min=(*midi).m_counter.changes;
			else if ((min != 0) ^ ((*midi).m_counter.changes != 0))
			{
				if (grp)
					std::cout << "[" << grp << "]:>";
				if (min && duration)
					std::cout << min*1000/duration;
				else
					std::cout << "not streaming!";
					min=(*midi).m_counter.changes;
				grp=0;
			}
			else
			{
				min=std::min<uint32>(min,(*midi).m_counter.changes);
				grp++;
			}
			if (!grp)
				std::cout << ", midi" << idx+1 << "." << (*midi).m_port_idx+1 << ":";
		}
	if (min!=0xfffffff)
	{
		if (grp)
			std::cout << "[" << grp << "]:>";
		if (min && duration)
			std::cout << min*1000/duration;
		else
			std::cout << "not streaming!";
	}

	std::cout <<
		#ifdef WIN32
		"                       " << std::endl;
		#endif//WIN32
		#ifdef __MACH__
		(char)27 << "[0K" << std::endl; 
		#endif//__MACH__

	if (!s_qut & size())
	{
		// synopsis headline
		std::cout << "       |     | init|           latency           |      slips             |    drop-outs           |  total | drv |tx|    arx    |" << std::endl;
		std::cout << "  outs | ins | gap | last| min | max | expc| diff| amnt| min | max | first| amnt| min | max | first| errors |ul sl|BN|SA DB PA SO|" << std::endl;
		dice::AVS avs[kTCAT_DICE_DEVICES];
		int i=0;
		for (const_iterator device=begin(); device!=end(); device++)
			(*device)->firmware_avs(avs[i++]);
		chnl=0;
		while (chnl<chnls)
		{
			unsigned int first_channel = chnl;
			int xchnl = (xcross() ? TCAT_DICE_CHANNEL_CROSS(first_channel,xcross()):first_channel);

			// group results
			for (;(chnl<std::min<uint32>(channels_in(),channels_out()) && 
				channel_counter(chnl,sbus::kChannelCounterLatency)==	channel_counter(first_channel,sbus::kChannelCounterLatency) &&
				channel_counter(chnl,sbus::kChannelCounterDropouts)==	channel_counter(first_channel,sbus::kChannelCounterDropouts) &&
				channel_counter(chnl,sbus::kChannelCounterSlips)==		channel_counter(first_channel,sbus::kChannelCounterSlips) &&
				channel_errors(chnl)==									channel_errors(first_channel) &&
				((channel_mask()>>chnl)&1)==							((channel_mask()>>first_channel)&1));
				chnl++);
			
			// outs
			std::cout << "  " << std::setw(2) << first_channel+1;
			if (first_channel!=chnl-1)
				std::cout << "-" << std::setw(2) << chnl << "|";
			else
				std::cout << "   |";

			// ins
			std::cout << std::setw(2) << xchnl+1;
			if (first_channel!=chnl-1)
				std::cout << "-" << std::setw(2) << (xcross() ? TCAT_DICE_CHANNEL_CROSS(chnl-1,xcross())+1:chnl) << "|";
			else
				std::cout << "   |";

			if (channel_mask()&(uint64(1)<<first_channel))
			{
				counter& cnt = channel_counter(first_channel,sbus::kChannelCounterLatency);
				outf0((uint32)cnt.first_occurence,5,99999);
				outf0((uint32)cnt.current,5,99999);
				outf0(cnt.current!=cnt.min?cnt.min:0,5,99999);
				outf0(cnt.current!=cnt.max?cnt.max:0,5,99999);

				outf0(latency(true)+latency(false),5,99999);
				outf0(abs(int32(channel_counter(first_channel,sbus::kChannelCounterLatency).current-latency(true)-latency(false))),5,99999);
				outf0(channel_counter(first_channel,sbus::kChannelCounterSlips).changes,5,99999);
				if (channel_counter(first_channel,sbus::kChannelCounterSlips).changes)
					std::cout <<
						std::setw(5) << std::min<uint32>(channel_counter(first_channel,sbus::kChannelCounterSlips).min,99999) << "|" <<
						std::setw(5) << std::min<uint32>(channel_counter(first_channel,sbus::kChannelCounterSlips).max,99999) << "|" <<
						std::setw(6) << std::min<uint64>(channel_counter(first_channel,sbus::kChannelCounterSlips).first_occurence,999999) << "|";
				else
					std::cout << "     |     |      |";

				outf0(channel_counter(first_channel,sbus::kChannelCounterDropouts).changes,5,99999);
				if (channel_counter(first_channel,sbus::kChannelCounterDropouts).changes)
					std::cout <<
						std::setw(5) << std::min<uint32>(channel_counter(first_channel,sbus::kChannelCounterDropouts).min,	99999) << "|" <<
						std::setw(5) << std::min<uint32>(channel_counter(first_channel,sbus::kChannelCounterDropouts).max, 99999) << "|" <<
						std::setw(6) << std::min<uint64>(channel_counter(first_channel,sbus::kChannelCounterDropouts).first_occurence, 999999) << "|";
				else
					std::cout << "     |     |      |";
				outf0((uint32)(channel_errors(first_channel) & 0xffffffff),8,99999999);

				sdevice_ref device;
				int rx = 0;
				int tx = 0;
				{
					dice::lock lock(this);
					size_t dev_chnl = 0;
					for (iterator i=begin(); i!=end(); i++)
					{
						if (dev_chnl+(*i)->channels(false).size()>first_channel)
						{
							device=*i;
							rx = device->channels(false)[first_channel-dev_chnl].avs();	// device rx - host output
							tx = device->channels(true)[xchnl-dev_chnl].avs();			// device tx - host input
							break;
						}
						else
							dev_chnl += (*i)->channels(false).size();
					}
				}
				
				outf0_(device->counters().status[(uint32)dice::kExStatusArx0 + rx].unlocks,2,99);
				outf0(device->counters().status[(uint32)dice::kExStatusArx0 + rx].slips,2,99);

				outf0(avs[0][dice::kApbaInt2Atx1BoundryError + tx*3],2,99);
				outf0_(avs[0][dice::kApbaInt1Ado1SytAgeout + rx*3],2,99);
				outf0_(avs[0][dice::kApbaInt2Arx1DbcFail + rx*6],2,99);
				outf0_(avs[0][dice::kApbaInt2Arx1PktAbort + rx*6],2,99);
				outf0(avs[0][dice::kApbaInt1Arxdo1SytOverflow + rx*2],2,99);
			}
			else
				std::cout << " n/a.|     |     |     |     |     |     |     |     |      |     |     |     |      |        |";
			std::cout << std::endl;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	sbus::log_file
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::log_file(std::string filename, uint32 duration, int32 dpc)
{
	int		i;

	dice::lock lock(this);
	// creating spreadsheet output
	std::ofstream file(filename.c_str(), std::ios::out | std::ios::app);
	if (!file)
	{
		std::cerr << filename << ": creation error" << std::endl;
		return;
	}
	file.seekp(0,std::ios::end);
	if (!file.tellp())
	{
		file << 
			"workstation:  " << workstation() << std::endl <<
			"OS:  " << environment() << std::endl <<
			"CPU:  " << hardware() << std::endl;

	#ifdef WIN32
		int index = 0;
		int numFound = 0;
		int nOSMajor = -1;
		int nOSMinor = -1;
		int nOSServicePack = -1;

		extern fwi g_fwi;

		// OS version numbers
		if (0 == g_fwi.getOSInfo(&nOSMajor, &nOSMinor, &nOSServicePack))
		{
			// driver file versions
			TCHAR verStr[32];
			bool bIsWin7 = ( (6 == nOSMajor) && (1 == nOSMinor) );

			file << "1394 driver file versions:" << std::endl;
			if (bIsWin7)
			{ 
				file << "  Microsoft 1394ohci.sys ";
				if ( 0 != g_fwi.getDriverFileVersion( TEXT("1394ohci.sys"), verStr, sizeof(verStr)) )
				{
					file << "[" << wstrtostr( verStr ) << "]" << std::endl;
				}
				else
				{
					file << "[N/A]" << std::endl;
				}
			}

			if ( 0 != g_fwi.getDriverFileVersion( TEXT("ohci1394.sys"), verStr, sizeof(verStr)) )
			{
				file << "  Microsoft ohci1394.sys [" << wstrtostr( verStr ) << "]";
				if (bIsWin7)
				{
					file << " (legacy)";
				}
				file << std::endl;
			}

			if ( 0 != g_fwi.getDriverFileVersion( TEXT("1394bus.sys"), verStr, sizeof(verStr)) )
			{
				file << "  Microsoft 1394bus.sys [" << wstrtostr( verStr ) << "]";
				if (bIsWin7)
				{
					file << " (legacy)";
				}
				file << std::endl;
			}
		}

		// ohci controllers
		{
			fwi::OHCI1394_DEV_INFO devInfo = { 0 };
			long lResult = g_fwi.get1394OHCIControllerInfo( index++, fwi::kUseBuiltIn, fwi::kActiveAdaptersOnly, &devInfo );

			if (lResult == FWI_NO_ERROR)
			{
				std::string		vendorNameStr( wstrtostr( devInfo.vendorName ) );
				std::string		chipsetStr( wstrtostr( devInfo.chipset ) );

				// replace any commas with spaces
				for (std::string::iterator it = vendorNameStr.begin(); it < vendorNameStr.end(); ++it)
					if (*it == ',')  *it = ' ';
				for (std::string::iterator it = chipsetStr.begin(); it < chipsetStr.end(); ++it)
					if (*it == ',')  *it = ' ';

				file << "OHCI " << ++numFound << ":  (" <<
					wstrtostr( devInfo.vendorId ) << ":" << wstrtostr( devInfo.deviceId ) << ") " <<
					vendorNameStr << " : " << chipsetStr <<
					std::endl;
			}
			else if (lResult == FWI_FILTERED)
			{

			}
			else
			{

			}
		}
	#endif // WIN32

		file <<
			"dice tool version:  " << kTCAT_DICE_VERSION_EXTENDED_STR << std::endl <<
			"driver version   :  " << VERSION(driver_version()) << std::endl;
		#ifndef TCAT_DICE_AVC
		file << "sync:  " << sync_name(sync()) << std::endl;
		#endif//TCAT_DICE_AVC

		i = 0;
		for (sbus::iterator device = begin(); device != end(); device++, i++)
		{
			tcat::uint64 guid64 = (*device)->guid64();
			tcat::dice::FL_GET_APP_INFO_RETURN info = (*device)->firmware_info();

			file << "Device " << i+1 << ":" << std::endl <<
				"    description:  " << (*device)->type() << "  (serial#: " <<
				((guid64 >> kTCAT_DICE_GUID64_PRODUCT_SERIAL_SHIFT) & kTCAT_DICE_GUID64_PRODUCT_SERIAL_MASK) << ")" << std::endl <<
				"    firmware version:  " << std::dec << VERSION(info.uiBaseSDKVersion) <<
				std::endl;

			for (std::deque<sdevice::os_midi>::iterator midi = (*device)->m_midi.begin(); midi != (*device)->m_midi.end(); midi++)
			{
				file << "    MIDI " << i+1 << "." << (*midi).m_port_idx+1 << " name:  " << (*midi).m_name << std::endl;
			}
		}

		file <<			
			"op mode,outs,ins,clock,I/O,sec,qi,MA,phase%,drv CB drops,"
			#ifdef __MACH__
			"CA overloads,"
			#endif
			"CPU:expct intvl us,CPU:drv min us(%),max us(%),delayed CB us,CPU:dsp min us(%),max us(%),"
			"out,avs/out,in,avs/in,"
			"init gap,min latency,max latency,last latency,exp latency,diff,"
			"drops,first drop,min drop,max drop,"
			"slips,first slip,min slip size,max slip size,slips+drops,streaming restarts,"
			"RX unlocks,RX slips,Watchdog Timer,Missed Callbacks,DbcFail,SytFail,Usermode Delays,"
			"ATX BOUNDARY,ADO SYT AGEOUT,ARX DBC FAIL,ARX PKT ABORT,ARXDO SYT OF"
			#ifdef WIN32
			",DPC"
			#endif
			;

		i = 0;
		for (sbus::iterator device=begin(); device!=end(); device++, i++)
		{
			for (std::deque<sdevice::os_midi>::iterator midi=(*device)->m_midi.begin(); midi!=(*device)->m_midi.end(); midi++)
			{
				file << ",MIDI " << i+1 << "." << (*midi).m_port_idx+1;
				file << ",MIDI " << i+1 << "." << (*midi).m_port_idx+1 << " drops";
			}
		}

		file << std::endl << std::flush;
	}

	if (!size())
	{
		file << "no devices" << std::endl << std::flush;
		return;
	}

	int master_phase[4] = {0};
	unsigned int master_isoc_input_streams = 0;
	{
		sdevice_ref master_device;
		dice::lock lock(this);

		#ifndef TCAT_DICE_AVC
		master_device = find(master());
		#else//TCAT_DICE_AVC
		master_device = at(0);	// TODO: find streaming device
		#endif//TCAT_DICE_AVC

		if (!master_device) 
		{
			file << "no master" << std::endl;
			return;
		}
		master_isoc_input_streams = master_device->isoc_streams(false);
		if (master_isoc_input_streams > 4)
			master_isoc_input_streams = 4;
		for (unsigned int avs = 0; avs < master_isoc_input_streams; avs++)
			master_phase[avs] = master_device->firmware_phase(avs);
	}

	unsigned int chnl=0;
	while (chnl < std::min<uint32>(channels_in(),channels_out()))
	{
		// group results
		unsigned int first_channel = chnl;
		for (;(chnl < std::min<uint32>(channels_in(),channels_out()) && 
			channel_counter(chnl,sbus::kChannelCounterLatency) ==	channel_counter(first_channel,sbus::kChannelCounterLatency) &&
			channel_counter(chnl,sbus::kChannelCounterDropouts) ==	channel_counter(first_channel,sbus::kChannelCounterDropouts) &&
			channel_counter(chnl,sbus::kChannelCounterSlips) ==		channel_counter(first_channel,sbus::kChannelCounterSlips) &&
			channel_errors(chnl) ==									channel_errors(first_channel) &&
			((channel_mask()>>chnl)&1) ==							((channel_mask()>>first_channel)&1));
			chnl++);
			
		int xchnl = xcross() ? TCAT_DICE_CHANNEL_CROSS(first_channel,xcross()):first_channel;

		sdevice_ref		device;
		unsigned int	dev_chnl_offset = 0;
		{
			dice::lock lock(this);
			size_t dev_max_chnl = 0;
			for (iterator i=begin(); i!=end(); i++)
			{
				dev_chnl_offset = (unsigned int)dev_max_chnl;
				dev_max_chnl += (*i)->channels(false).size();
				if (dev_max_chnl > first_channel)
				{
					device=*i;
					break;
				}
			}
		}
		dice::AVS avs;
		device->firmware_avs(avs);
		if (!first_channel)
		{
			file << 
				op_mode() << "," <<
				channels_out() << "," <<
				channels_in() << "," <<
				#ifndef TCAT_DICE_AVC
				clock_name(clock()) << "," <<
				#else//TCAT_DICE_AVC
				"," <<
				#endif//TCAT_DICE_AVC
				io_size() << "," <<
				(duration)/1000 << "," <<
				qi() << "," <<
				ma() << ",";

			file << "\"";
			for (unsigned int n = 0; n < master_isoc_input_streams; n++)
			{
				if (n != 0)
					file << ",";
				file << master_phase[n];
			}
			file << "\",";

			if (m_process_counter[kProcessCounterSync].changes)
				file << m_process_counter[kProcessCounterSync].changes << ",";
			else
				file << ",";

			#ifdef __MACH__
			if (overloads())
				file << overloads() << ",";
			else
				file << ",";
			#endif

			// expected callback interval in microseconds
			file << (io_size()*1000000)/clock_rate() << ",";

			if (process_counter(kProcessCounterDriver).min != 0xffffffff)
				file <<
					process_counter(kProcessCounterDriver).min <<	"(" << (process_counter(kProcessCounterDriver).min*clock_rate()) / (10000*io_size()) << ")," <<
					process_counter(kProcessCounterDriver).max <<	"(" << (process_counter(kProcessCounterDriver).max*clock_rate()) / (10000*io_size()) << ")," <<
					process_counter(kProcessCounterDriver).max-(io_size()*1000000)/clock_rate() << ",";
			else
				file << ",,,";

			if (process_counter(kProcessCounterUser).min != 0xffffffff)
				file <<
					process_counter(kProcessCounterUser).min <<		"(" << (process_counter(kProcessCounterUser).min*clock_rate()) / (10000*io_size()) << ")," <<
					process_counter(kProcessCounterUser).max <<		"(" << (process_counter(kProcessCounterUser).max*clock_rate()) / (10000*io_size()) << "),";
			else
				file << ",,";
		}
		else
		{
			file << ",,,,,,,,,,,,,,,,";
			#ifdef __MACH__
			file << ",";
			#endif
		}

		int rx = device->channels(false)[first_channel - dev_chnl_offset].avs();	// device rx - host output
		int tx = device->channels(true)[xchnl - dev_chnl_offset].avs();				// device tx - host input

		// outs
		file << first_channel+1;
		if (first_channel!=chnl-1)
			file << "~" << chnl;
		file << "," <<
//			channel_name(false,first_channel) << "," <<
			rx << ",";

		// ins
		file << xchnl+1;
		if (first_channel!=chnl-1)
			file << "~" << (xcross() ? TCAT_DICE_CHANNEL_CROSS(chnl-1,xcross())+1:chnl);
		file << "," <<
//			channel_name(true,xchnl) << "," <<
			tx << ",";

		if (channel_counter(first_channel,sbus::kChannelCounterLatency).first_occurence)
			file <<
				channel_counter(first_channel,sbus::kChannelCounterLatency).first_occurence << "," <<
				channel_counter(first_channel,sbus::kChannelCounterLatency).min << "," <<
				channel_counter(first_channel,sbus::kChannelCounterLatency).max << "," << 
				channel_counter(first_channel,sbus::kChannelCounterLatency).current << ",";
		else
			file << ",,,,";

		uint32		expLatency = latency(true) + latency(false);
		float		expLatencyMs = (float(expLatency) / float(clock_rate())) * 1000.0f;

		file << expLatency << " (" << std::fixed << std::setprecision(2) << expLatencyMs << "ms)," << std::dec;

		if (channel_counter(first_channel,sbus::kChannelCounterLatency).first_occurence)
			file << abs(int32(channel_counter(first_channel,sbus::kChannelCounterLatency).current - expLatency)) << ",";
		else
			file << ",";

		if (channel_counter(first_channel,sbus::kChannelCounterDropouts).changes)
			file <<
				channel_counter(first_channel,sbus::kChannelCounterDropouts).changes << "," <<
				channel_counter(first_channel,sbus::kChannelCounterDropouts).first_occurence << "," <<
				channel_counter(first_channel,sbus::kChannelCounterDropouts).min << "," <<
				channel_counter(first_channel,sbus::kChannelCounterDropouts).max << ",";
		else
			file << ",,,,";
		if (channel_counter(first_channel,sbus::kChannelCounterSlips).changes)
			file <<
				channel_counter(first_channel,sbus::kChannelCounterSlips).changes << "," <<
				channel_counter(first_channel,sbus::kChannelCounterSlips).first_occurence << "," <<
				channel_counter(first_channel,sbus::kChannelCounterSlips).min << "," <<
				channel_counter(first_channel,sbus::kChannelCounterSlips).max << ",";
		else
			file << ",,,,";
		file0(channel_errors(first_channel));
		file0(os_audio_restarts());
		file0(device->counters().status[(uint32)dice::kExStatusArx0 + rx].unlocks);
		file0(device->counters().status[(uint32)dice::kExStatusArx0 + rx].slips);

		#ifndef TCAT_DICE_AVC
		dice::ioctl::BUS_ERROR err = errors();
		for (int i=0; i<dice::ioctl::BUS_ERROR::kBusErrorItems; i++)
		{
			file0(err.error[i]);
		}
		file0(avs[dice::kApbaInt2Atx1BoundryError + tx*3]);
		file0(avs[dice::kApbaInt1Ado1SytAgeout + rx*3]);
		file0(avs[dice::kApbaInt2Arx1DbcFail + rx*6]);
		file0(avs[dice::kApbaInt2Arx1PktAbort + rx*6]);
		file0(avs[dice::kApbaInt1Arxdo1SytOverflow + rx*2]);
		#endif//TCAT_DICE_AVC


		#ifdef WIN32
		file0(dpc);
		#endif

		for (sbus::iterator device=begin(); device!=end(); device++)
			for (std::deque<sdevice::os_midi>::iterator midi=(*device)->m_midi.begin(); midi!=(*device)->m_midi.end(); midi++)
			{
				if ((*midi).m_counter.changes && duration)
					file << (*midi).m_counter.changes*1000/duration << "Bps min:" << (*midi).m_counter.min/1000 << " max:" << (*midi).m_counter.max/1000;
				else
					file << "not streaming";
				file << ",";

				if ((*midi).m_errors)
					file << (*midi).m_errors;   // << " (" << (*midi).m_counter.changes << ")";
				file << ",";
			}

		file << std::endl << std::flush;
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::cmd_streaming
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::cmd_streaming(std::string argv)
{
	int32 dpc = 0;
	unsigned int time_start = 0;
	std::string filename("");
	std::size_t delimiter = argv.find(",");
	unsigned int time = from_string<unsigned int>(argv.substr(0,delimiter));

	m_load = 0;

	if (delimiter!=std::string::npos)
	{
		argv = argv.substr(delimiter+1);
		delimiter = argv.find(",");
		filename = argv.substr(0,delimiter);
		if (delimiter!=std::string::npos)
		{
			argv = argv.substr(delimiter+1);
			m_load = (from_string<unsigned int>(argv)*10*io_size())/clock_rate();
		}
	}

	if (time && size())
	{
		log_streaming_head();
		ready_wait();
		reset_counters();
		unsigned int chnl;
		time_start = pal::time();
		unsigned int time_end = time*1000 + time_start;

		os_audio_start();
		while (time_end>(time=pal::time()) && size() && !s_exit)
		{
			#ifdef WIN32
			MSG msg;
			while(::PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
			}
			dpc_chk.getDPCPeak(dpc);
			#endif//WIN32
			log_streaming(time-time_start, time_end-time, dpc);
			os_audio_check();
			pal::delay(50);
		}
		os_audio_stop();

		// correcting values for total failures
		for (chnl=0; chnl<std::min<uint32>(channels_in(),channels_out()); chnl++)
		{
			if (channel_mask() & (uint64(1)<<chnl))
			{
				counter& dropouts=channel_counter(chnl,sbus::kChannelCounterDropouts);
				if (dropouts.current)
				{
					dropouts.min = std::min<uint32>((uint32)dropouts.current,dropouts.min);
					dropouts.max = std::max<uint32>((uint32)dropouts.current,dropouts.max);
					dropouts.changes++;
					if (!dropouts.first_occurence)
						dropouts.first_occurence = process_counter(kProcessCounterDriver).changes*io_size() - dropouts.current;
					channel_errors(chnl) += dropouts.current;
					dropouts.current = 0;
				}
			}
		}

		// report error conditions
		{
			int streamingResult = (int)qi();
			if (streamingResult != 0)
			{
				TDL_ENTRY( kTDLFlag_Error, "Dice tool streaming test result is non-zero (%d)", streamingResult, 0, 0, 0 );
			}

			int microalignment = ma();
			if (microalignment != 0)
			{
				TDL_ENTRY( kTDLFlag_Error, "Dice tool micro-alignment is non-zero (%d)", microalignment, 0, 0, 0 );
			}

#if 0
			unsigned int chnl = 0;
			while (chnl < std::min<uint32>(channels_in(),channels_out()))
			{
				if (channel_counter(chnl,sbus::kChannelCounterLatency).first_occurence)
				{
					uint32	expLatency = latency(true) + latency(false);
					int32	diff = abs(int32(channel_counter(chnl,sbus::kChannelCounterLatency).current - expLatency));

					if (diff != 0)
					{
						TDL_ENTRY( kTDLFlag_Error, "Mismatch between reported and measured latency (diff %d)", diff, 0, 0, 0 );
						break;
					}
				}
			}
#endif
		}

		log_streaming_synopsis (time-time_start, dpc);
	}
	else if (!size() && !s_qut)
	{
		std::cerr << "no devices" << std::endl;
	}

	if (filename.size())
	{
		log_file (filename, time-time_start, dpc);
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::ssystem
//
///////////////////////////////////////////////////////////////////////////////
//
//	tcat::ssystem::create
//
///////////////////////////////////////////////////////////////////////////////
//
void ssystem::create() throw(tcat::exception::base)
{
	if (s_trc)
		std::clog << TCAT_FUNCTION << std::endl;
	uint32 time_open = pal::time();
	super::create();
	m_time_open = pal::time() - time_open;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::ssystem::~ssystem
//
///////////////////////////////////////////////////////////////////////////////
//
ssystem::~ssystem()
{ 
	if (s_trc)
		std::clog << TCAT_FUNCTION << ": " << super::size() << " bus(ses)" << std::endl;	
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::ssystem::update_buslist
//
///////////////////////////////////////////////////////////////////////////////
//
void ssystem::update_buslist() throw(tcat::exception::base)
{
	dice::lock lock(this);
	super::update_buslist();
	if (s_trc)
		std::clog << TCAT_FUNCTION << ": " << super::size() << " bus(ses)" << std::endl;
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::ssystem::fetch_buslist
//
///////////////////////////////////////////////////////////////////////////////
//
bool ssystem::fetch_buslist() throw(tcat::exception::base)
{
	if (s_trc)
		std::clog << TCAT_FUNCTION << ": " << super::size() << " bus(ses)" << std::endl;
	return super::fetch_buslist();
}



///////////////////////////////////////////////////////////////////////////////
//
//	tcat::ssystem::cmd_info
//
///////////////////////////////////////////////////////////////////////////////
//
void ssystem::cmd_info()
{
	if (s_trc)
		std::clog << TCAT_FUNCTION << std::endl;
	{
		dice::lock lock(this);
		std::cout << "  system opening time: " << time_open() << "ms for " << super::size() << " bus(ses):" << std::endl;
	
		for (const_iterator bus=begin(); bus!=end(); bus++)
			std::cout << "    " << std::hex << (*bus)->id() << ": " << (*bus)->name() << std::endl;
	}
}
