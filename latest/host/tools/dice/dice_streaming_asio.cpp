///////////////////////////////////////////////////////////////////////////////
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
///////////////////////////////////////////////////////////////////////////////
//
//	see dice_streaming.h
//
#include "dice_streaming.h"
#include "../../interface/tcat_dice_win.h"
#include <mmsystem.h>
#include <ks.h>
#include <ksmedia.h>



using namespace tcat;

#define kTCAT_DICE_MIDI_BUSY_TIMEOUT	2000



///////////////////////////////////////////////////////////////////////////////
//
//	callback_midi
//
///////////////////////////////////////////////////////////////////////////////
//
void CALLBACK callback_midi(
	HMIDIIN hMidiIn,
	UINT wMsg,
	DWORD_PTR dwInstance,
	DWORD_PTR dwParam1,
	DWORD_PTR dwParam2)
{
	sdevice::os_midi*	midi = (sdevice::os_midi*)dwInstance;

	if (!midi)
		return;

#if 0
	if (midi->m_handle[false] == kTCAT_INVALID_HANDLE)
		std::cout << "m_handle[input] is null" << std::endl;
	if (midi->m_handle[true] == kTCAT_INVALID_HANDLE)
		std::cout << "m_handle[output] is null" << std::endl;

	if (wMsg == MIM_OPEN)
		std::cout << "MIM_OPEN" << std::endl;
	else if (wMsg == MIM_CLOSE)
		std::cout << "MIM_CLOSE" << std::endl;
	else if (wMsg == MIM_DATA)
		std::cout << "MIM_DATA" << std::endl;
	else if (wMsg == MIM_LONGDATA)
		std::cout << "MIM_LONGDATA" << std::endl;
	else if (wMsg == MIM_ERROR)
		std::cout << "MIM_ERROR" << std::endl;
	else if (wMsg == MIM_LONGERROR)
		std::cout << "MIM_LONGERROR" << std::endl;
#endif

	if (wMsg == MIM_DATA)
	{
		if (midi->m_started)
		{
			midi->process( uint32(dwParam1) );
		}
		else
		{
			if (s_trc)
				std::cout << "os_midi receive: m_started is false" << std::endl;
		}
	}
	else if (wMsg == MIM_LONGDATA)
	{
		std::cout << "MIM_LONGDATA (received sysex message)" << std::endl;
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::os_midi::send
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::os_midi::send(uint32 data)
{
	if (m_started)
	{
		if (m_handle[true])
			::midiOutShortMsg((HMIDIOUT)m_handle[true], data);
	}
	else
	{
		if (s_trc)
			std::cout << "os_midi send: m_started is false" << std::endl;
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::create
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::create(dice::bus_impl* bus, const dice::DEVICE_INFO& info) throw(tcat::exception::base)
{
	if (s_trc)
		std::clog << "    " << TCAT_FUNCTION << "[" << info.nickname << "]" << std::endl;
	super::create(bus,info);
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::create
//
///////////////////////////////////////////////////////////////////////////////
//
sdevice::~sdevice()
{
	if (s_trc)
		std::clog << "    " << TCAT_FUNCTION << "[" << nickname() << "]:" << std::endl;
	m_midi.clear();
}


///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::os_midi::name
//
///////////////////////////////////////////////////////////////////////////////
//
std::string sdevice::os_midi::name()
{
	std::string		name("N/A");		// default name

	if (s_midi)
	{
		GUID		driverGUID = kTCAT_DICE_GUID;
		uint64		guid64 = m_device.guid64();
		int			numDevices = midiOutGetNumDevs();

		for (int device = 0; device < numDevices; device++)
		{
			MIDIOUTCAPS2 moc;

			memset( &moc, 0, sizeof(moc) );
			if (!::midiOutGetDevCaps(device, (LPMIDIOUTCAPSW)&moc, sizeof moc))
			{
				if (    (driverGUID == moc.ManufacturerGuid)
					 && (memcmp(&moc.ProductGuid.Data4, &guid64, sizeof(guid64)) == 0)
					 && (moc.ProductGuid.Data1 == m_port_idx)
				   )
				{
					name = wstrtostr(moc.szPname);
					break;
				}
			}
		}
	}

	return name;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::os_midi::open
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::os_midi::open()
{
	if (s_trc)
		std::clog << "  (" << m_port_idx << ") os_midi::open" << std::endl;
	close();
	if (s_midi)
	{
		GUID		driverGUID = kTCAT_DICE_GUID;
		uint64		guid64 = m_device.guid64();

		for (int dir=0; dir<2; dir++)
		{
			uint32	time = pal::time();
			int		devices=dir?::midiOutGetNumDevs(): ::midiInGetNumDevs();

			do
			{
				for (int device=0; device<devices; device++)
				{
					if (dir)
					{
						MIDIOUTCAPS2 moc;

						memset( &moc, 0, sizeof(moc) );
						if (!::midiOutGetDevCaps(device, (LPMIDIOUTCAPSW)&moc, sizeof moc))
						{
							if (    (driverGUID == moc.ManufacturerGuid)
								 && (memcmp(&moc.ProductGuid.Data4, &guid64, sizeof(guid64)) == 0)
								 && (moc.ProductGuid.Data1 == m_port_idx)
							   )
							{
								TCAT_EXCEPTION_THROW(::midiOutOpen((LPHMIDIOUT)&m_handle[dir], device, NULL, NULL, CALLBACK_NULL), xptn_os_midi);
								break;
							}
						}
					}
					else
					{
						MIDIINCAPS2 mic;
						if (!::midiInGetDevCaps(device, (LPMIDIINCAPSW)&mic, sizeof mic))
						{
							if (    (driverGUID == mic.ManufacturerGuid)
								 && (memcmp(&mic.ProductGuid.Data4, &guid64, sizeof(guid64)) == 0)
								 && (mic.ProductGuid.Data1 == m_port_idx)
							   )
							{
								TCAT_EXCEPTION_THROW(::midiInOpen((LPHMIDIIN)&m_handle[dir], device, (DWORD_PTR)callback_midi, (DWORD_PTR)this, CALLBACK_FUNCTION), xptn_os_midi);
								break;
							}
						}
					}
				}
				if (m_handle[dir]!=kTCAT_INVALID_HANDLE)
					break;
				m_device.bus()->progress(100*(pal::time()-time)/kTCAT_DICE_MIDI_BUSY_TIMEOUT, sbus::kProgressOpenMidi);
				::Sleep(100);
			} while (pal::time() - time < kTCAT_DICE_MIDI_BUSY_TIMEOUT);
		}
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::os_midi::close
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::os_midi::close()
{
	if (m_handle[false]!=kTCAT_INVALID_HANDLE)
	{
		if (s_trc)
			std::clog << "  (" << m_port_idx << ") os_midi::close [input]" << std::endl;
		::midiInClose((HMIDIIN)m_handle[false]);
		m_handle[false]=kTCAT_INVALID_HANDLE;
	}
	if (m_handle[true]!=kTCAT_INVALID_HANDLE)
	{
		if (s_trc)
			std::clog << "  (" << m_port_idx << ") os_midi::close [output]" << std::endl;
		::midiOutClose((HMIDIOUT)m_handle[true]);
		m_handle[true]=kTCAT_INVALID_HANDLE;
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::os_midi::start
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::os_midi::start()
{
	if (m_handle[false])
	{
		TCAT_EXCEPTION_THROW (::midiInStart((HMIDIIN)m_handle[false]), xptn_os_midi);
		m_started = true;

		m_dataTwo = (::rand() & 0x7f);
		if (s_trc)
			std::clog << "  (" << m_port_idx << ") os_midi::start (0x" << std::hex << (int)m_dataTwo << ")" << std::dec << std::endl;
		send( (m_dataTwo << 16) | 0x7c94 );
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::os_midi::stop
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::os_midi::stop()
{
	if (m_started && m_handle[false])
	{
		m_started = false;
		if (s_trc)
			std::clog << "  (" << m_port_idx << ") os_midi::stop" << std::endl;
		::midiInStop((HMIDIIN)m_handle[false]);
	}
}



// this is real crap: since ASIO does not support OOP we need to remember our bus object.
static sbus* __static_asio_streaming_bus = NULL;



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::os_audio_initialize
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::os_audio_initialize()
{
	m_buffers=NULL;
	memset (&m_asio, 0, sizeof m_asio); 
	memset (&m_callbacks, 0, sizeof m_callbacks);
	__static_asio_streaming_bus = NULL;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::callback_asio_bufferswitch_timeinfo
//
///////////////////////////////////////////////////////////////////////////////
//
ASIOTime* sbus::callback_asio_bufferswitch_timeinfo (ASIOTime* time, long index, ASIOBool)
{
	if (__static_asio_streaming_bus)
	{
		long* in[kTCAT_DICE_MAX_CHANNELS];
		long* out[kTCAT_DICE_MAX_CHANNELS];
		long ins = __static_asio_streaming_bus->channels_in();
		long outs = __static_asio_streaming_bus->channels_out();
		ASIOBufferInfo* pBuffers = __static_asio_streaming_bus->m_buffers;
		int channel;
		for (channel=0; channel<ins; channel++)
			in[channel] = (long*) pBuffers++->buffers[index];
		for (channel=0; channel<outs; channel++)
			out[channel] = (long*) pBuffers++->buffers[index];
		// steinberg: what a funny way of ordering a 64bit integer...
		__static_asio_streaming_bus->process(in,out,time->timeInfo.samplePosition.hi*(1ll<<32)+time->timeInfo.samplePosition.lo);
	}
	return 0L;
}



///////////////////////////////////////////////////////////////////////////////
//
//	callback_asio_samplerate_changed
//
///////////////////////////////////////////////////////////////////////////////
//
void callback_asio_samplerate_changed(ASIOSampleRate sRate)
{
	std::cout << "asio_clock:" << std::dec << sRate << std::endl;
	// do whatever you need to do if the sample rate changed
	// usually this only happens during external sync.
	// Audio processing is not stopped by the driver, actual sample rate
	// might not have even changed, maybe only the sample rate status of an
	// AES/EBU or S/PDIF digital input at the audio device.
	// You might have to update time/sample related conversion routines, etc.
}



///////////////////////////////////////////////////////////////////////////////
//
//	callback_asio_messages
//
///////////////////////////////////////////////////////////////////////////////
//
long callback_asio_messages(long selector, long value, void* message, double* opt)
{
	// currently the parameters "value", "message" and "opt" are not used.
	if (s_log)
		std::clog << "asio_msg:" << std::dec << selector << std::endl;
	long ret = 0;
	switch(selector)
	{
		case kAsioSelectorSupported:
			if(value == kAsioResetRequest
			|| value == kAsioEngineVersion
			|| value == kAsioResyncRequest
			|| value == kAsioLatenciesChanged
			// the following three were added for ASIO 2.0, you don't necessarily have to support them
			|| value == kAsioSupportsTimeInfo
			|| value == kAsioSupportsTimeCode
			|| value == kAsioSupportsInputMonitor)
				ret = 1L;
			break;
		case kAsioResetRequest:
			// defer the task and perform the reset of the driver during the next "safe" situation
			// You cannot reset the driver right now, as this code is called from the driver.
			// Reset the driver is done by completely destruct is. I.e. ASIOStop(), ASIODisposeBuffers(), Destruction
			// Afterwards you initialize the driver again.
			if (__static_asio_streaming_bus)
				__static_asio_streaming_bus->os_audio_restart_request();
			ret = 1L;
			break;
		case kAsioResyncRequest:
			// This informs the application, that the driver encountered some non fatal data loss.
			// It is used for synchronization purposes of different media.
			// Added mainly to work around the Win16Mutex problems in Windows 95/98 with the
			// Windows Multimedia system, which could loose data because the Mutex was hold too long
			// by another thread.
			// However a driver can issue it in other situations, too.
			ret = 1L;
			break;
		case kAsioLatenciesChanged:
			// This will inform the host application that the drivers were latencies changed.
			// Beware, it this does not mean that the buffer sizes have changed!
			// You might need to update internal delay data.
			ret = 1L;
			break;
		case kAsioEngineVersion:
			// return the supported ASIO version of the host application
			// If a host applications does not implement this selector, ASIO 1.0 is assumed
			// by the driver
			ret = 2L;
			break;
		case kAsioSupportsTimeInfo:
			// informs the driver wether the asioCallbacks.bufferSwitchTimeInfo() callback
			// is supported.
			// For compatibility with ASIO 1.0 drivers the host application should always support
			// the "old" bufferSwitch method, too.
			ret = 1;
			break;
		case kAsioSupportsTimeCode:
			// informs the driver wether application is interested in time code info.
			// If an application does not need to know about time code, the driver has less work
			// to do.
			ret = 0;
			break;
	}
	return ret;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::os_audio_open
//
///////////////////////////////////////////////////////////////////////////////
//
bool loadAsioDriver(char *name);
extern AsioDrivers* asioDrivers;
void sbus::os_audio_open()
{
	if (!s_audio)
		return;
	int i;
	TCAT_EXCEPTION_THROW (!::loadAsioDriver(kTCAT_DICE_USR_ASIO_STR), xptn_os_audio);

	if (s_plugin)
		TCAT_EXCEPTION_THROW (!::ASIOFuture(dice::kAsioSetSlaveClient, NULL), xptn_os_audio);

	memset (&m_asio, 0, sizeof m_asio);
	TCAT_EXCEPTION_THROW (::ASIOInit (&m_asio)!=ASE_OK, xptn_os_audio);
	if (s_log)
		std::clog << 
			"ASIO:" << std::endl <<
			"  version:" << std::dec << m_asio.asioVersion << 
			", drv:" << m_asio.driverVersion << 
			", name:\"" << m_asio.name << 
			"\", open:\"" << std::dec << m_asio.errorMessage << "\"" <<
			std::endl;
	m_name = m_asio.name;

	TCAT_EXCEPTION_THROW (::ASIOGetChannels(&m_in, &m_out)!=ASE_OK, xptn_os_audio);
	if (s_log)
		std::clog << "  channels: in:" << m_in << ", out:" << m_out << std::endl;

	long min, max, pref, gran;	
	TCAT_EXCEPTION_THROW (::ASIOGetBufferSize(&min, &max, &pref, &gran)!=ASE_OK, xptn_os_audio);
	if (s_log)
		std::clog << "  I/O size: min:" << min << ", max:" << max << ", pref:" << pref << ", gran:" << gran << std::endl;

	ASIOSampleRate clock;
	TCAT_EXCEPTION_THROW (::ASIOGetSampleRate(&clock)!=ASE_OK, xptn_os_audio);
	if (s_log)
		std::clog << "  clock:" << clock << std::endl;

	// check wether the driver requires the ASIOOutputReady() optimization
	// (can be used by the driver to reduce output latency by one block)
	if (s_log)
		std::clog << "  latency reduction:" << (::ASIOOutputReady()==ASE_OK?"":"not ") << "supported." << std::endl;

	m_buffers = new ASIOBufferInfo[m_in+m_out];
	ASIOChannelInfo* channels = new ASIOChannelInfo[m_in+m_out];
	ASIOBufferInfo* pBuffers = m_buffers;
	ASIOChannelInfo* pChannels = channels;
	for (i=0; i<m_in; i++)
	{
		pBuffers->buffers[0] = pBuffers->buffers[1] = NULL;
		pBuffers->channelNum = pChannels->channel = i;
		pBuffers->isInput = pChannels->isInput = ASIOTrue;
		TCAT_EXCEPTION_THROW (::ASIOGetChannelInfo(pChannels)!=ASE_OK, xptn_os_audio);
		m_channel_names[true].push_back(std::string(pChannels->name));
		pChannels++;
		pBuffers++;
	}
	for (i=0; i<m_out; i++)
	{
		pBuffers->buffers[0] = pBuffers->buffers[1] = NULL;
		pBuffers->channelNum = pChannels->channel = i;
		pBuffers->isInput = pChannels->isInput = ASIOFalse;
		TCAT_EXCEPTION_THROW (::ASIOGetChannelInfo(pChannels)!=ASE_OK, xptn_os_audio);
		m_channel_names[false].push_back(std::string(pChannels->name));
		pChannels++;
		pBuffers++;
	}
	delete channels;
	m_callbacks.asioMessage = &callback_asio_messages;
	m_callbacks.bufferSwitch = NULL; // &callback_asio_bufferswitch;
	m_callbacks.bufferSwitchTimeInfo = &callback_asio_bufferswitch_timeinfo;
	m_callbacks.sampleRateDidChange = &callback_asio_samplerate_changed;
	TCAT_EXCEPTION_THROW (::ASIOCreateBuffers(m_buffers, m_in+m_out, pref, &m_callbacks)!=ASE_OK, xptn_os_audio);

	TCAT_EXCEPTION_THROW (::ASIOGetLatencies(&m_latency[false], &m_latency[true])!=ASE_OK, xptn_os_audio);
	if (s_log)
		std::clog << "  latency: in:" << m_latency[false] << ", out:" << m_latency[true] << std::endl;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::os_audio_close
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::os_audio_close()
{
	os_audio_stop();
	::ASIODisposeBuffers();
	if (m_buffers)
	{
		delete m_buffers;
		m_buffers = NULL;
	}
	::ASIOExit();
	asioDrivers = 0;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::os_audio_start
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::os_audio_start()
{
	dice::lock lock(this);
	__static_asio_streaming_bus = this;
	TCAT_EXCEPTION_THROW (::ASIOStart(), xptn_os_audio);
	for (iterator device=begin(); device!=end(); device++) 
		(*device)->midi_start(); 
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::os_audio_stop
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::os_audio_stop()
{
	dice::lock lock(this);
	for (iterator device=begin(); device!=end(); device++) 
		(*device)->midi_stop(); 
	::ASIOStop();
	__static_asio_streaming_bus = NULL;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::io_size
//
///////////////////////////////////////////////////////////////////////////////
//
//	changing the I/O size requires the OS audio drivers to be closed 
void sbus::io_size (uint32 nIoSize) throw(tcat::exception::base)
{
	os_audio_close();
	try
	{
		super::io_size(nIoSize);
	}
	catch(...)
	{
		ready_wait();
		os_audio_open();
		throw;		// re-throw the exception
	}
	ready_wait();
	os_audio_open();
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::update_io_size
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::update_io_size() 
{ 
	if (s_trc) 
		std::clog << "  " << TCAT_FUNCTION << ": " << std::dec << super::io_size() << std::endl; 
	super::update_io_size(); 
}



struct SPK
{
	uint32 id;
	std::string name;
} spk[] = {
	{KSAUDIO_SPEAKER_DIRECTOUT, "direct"}, 
	{KSAUDIO_SPEAKER_MONO, "mono"},
	{KSAUDIO_SPEAKER_STEREO, "stereo"}, 
	{KSAUDIO_SPEAKER_QUAD, "quad"}, 
	{KSAUDIO_SPEAKER_SURROUND, "surround"}, 
	{KSAUDIO_SPEAKER_5POINT1, "5.1"}, 
	{KSAUDIO_SPEAKER_7POINT1, "7.1"}, 
	{KSAUDIO_SPEAKER_5POINT1_SURROUND, "5.1 surround"}, 
	{KSAUDIO_SPEAKER_7POINT1_SURROUND, "7.1 surround"}, 
	{KSAUDIO_SPEAKER_7POINT1_WIDE, "7.1 wide"}};



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::cmd_speaker_config
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::cmd_speaker_config(std::string argv)
{
	if (!s_qut)
		std::cout << "out speaker configuration: ";

		uint32 cfg = speaker();
		uint32 idx;
		for (idx=0; idx<(sizeof spk/sizeof SPK); idx++)
			if (spk[idx].id == cfg)
				break;
			std::cout << "\"" <<
				spk[idx].name << "\" (" << 
				std::dec << idx << "/" << 
				std::hex << spk[idx].id << ")" << std::flush;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::cmd_speaker_map
//
///////////////////////////////////////////////////////////////////////////////
//
void sbus::cmd_speaker_map(std::string argv)
{
	std::string name[] = {
		"front left",
		"front right",
		"front center",
		"low frequency",
		"back left",
		"back right",
		"front left of center",
		"front right of center",
		"back center",
		"side left",
		"side right",
		"top center",
		"top front left",
		"top front center",
		"top front right",
		"top back left",
		"top back center",
		"top back right"};
	
	std::cout << "speaker mapping: ";

	uint32 chn=0;

	uint32 cfg = speaker();
	uint32 idx;
	for (idx=0; idx<(sizeof spk/sizeof SPK); idx++)
	{
		if (spk[idx].id == cfg)
			break;
	}
	std::cout << std::endl << "  out: " << 
		spk[idx].name << " (" << 
		std::dec << idx << "/" << 
		std::hex << spk[idx].id << "):";

	for (chn=0; chn<8; chn++)
	{
		int32 aggregate_ch = speaker_map(true,chn);	// mapped channel in the combined list of all devices
		std::string wdm_str("");
		std::string ch_str(channel_name(false,aggregate_ch));
		if (ch_str.empty())
		{
			ch_str.append("unmapped");
		}
		else
		{
			uint32 count = 0;
			for (uint32 pos=0; pos<32; pos++)	// go thru all bits of the speaker bitmap
			{
				// and find a name if any for this channel
				if ((spk[idx].id & (1 << pos)))
				{
					if (count == chn)
					{
						wdm_str.append("(");
						wdm_str.append(name[pos]);
						wdm_str.append(")");
						break;
					}
					count++;
				}
			}
		}
		std::cout << std::endl << "    " << 
			std::setiosflags(std::ios::left) << "WDM " << chn << ": " << std::setw(24) << wdm_str  
			<< std::resetiosflags(std::ios::left) << 
			"[" << std::dec << aggregate_ch << "] " << ch_str;
		wdm_str.clear();
	}

	std::cout << std::endl << "  in: ";
	for (chn=0; chn<8; chn++)
	{
		int32 aggregate_ch = speaker_map(true,chn);	// mapped channel in the combined list of all devices
		std::string ch_str(channel_name(true,aggregate_ch));
		if (ch_str.empty())
		{
			ch_str.append("unmapped");
		}
		std::cout << std::endl << "    " << 
			std::setiosflags(std::ios::left) << "WDM " << chn << ":"
			<< std::setw(26) << std::resetiosflags(std::ios::left) << 
			"[" << std::dec << aggregate_ch << "] " << ch_str;
	}
}
