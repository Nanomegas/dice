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
//	see streaming.h
//
#include "dice_streaming.h"
#include "TDL_Interface.h"


#define kDriverLog_File_ID		1102


using namespace tcat;



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::os_midi::name
//
///////////////////////////////////////////////////////////////////////////////
//
std::string sdevice::os_midi::name()
{
	std::string		name( kTCAT_DICE_USR_MIDI_STR );

	if (s_midi)
	{
		int devices = ::MIDIGetNumberOfDevices();

		for (int device = 0; device < devices; device++)
		{
			MIDIDeviceRef	deviceRef;
			CFStringRef		cf_guidStr;

			TCAT_EXCEPTION_THROW(!(deviceRef = ::MIDIGetDevice( device )), xptn_os_midi);

			if (::MIDIObjectGetStringProperty( deviceRef, CFSTR("DiceGUID"), &cf_guidStr ) == noErr)
			{
				uint64		guid64 = 0;
				const char *guidStrPtr;
				int			items = 0;

				guidStrPtr = ::CFStringGetCStringPtr( cf_guidStr, kCFStringEncodingMacRoman );
				items = ::sscanf( guidStrPtr, "%llx", &guid64 );
				if (items == 1)
				{
					if (guid64 == m_device.guid64())
					{
						ItemCount		entities = ::MIDIDeviceGetNumberOfEntities( deviceRef );
						CFStringRef		cf_deviceName;

						if (::MIDIObjectGetStringProperty( deviceRef, kMIDIPropertyName, &cf_deviceName ) == noErr)
						{
							if (m_port_idx < entities)
							{
								MIDIEntityRef	entityRef = ::MIDIDeviceGetEntity( deviceRef, m_port_idx );
								CFStringRef		cf_name;

								if (::MIDIObjectGetStringProperty( entityRef, kMIDIPropertyName, &cf_name ) == noErr)
								{
									name = CFStringGetCStringPtr( cf_deviceName, kCFStringEncodingMacRoman );
									name += " ";
									name += CFStringGetCStringPtr( cf_name, kCFStringEncodingMacRoman );
									::CFRelease( cf_name );
								}
							}
							::CFRelease( cf_deviceName );
						}
						break;
					}
				}
				::CFRelease( cf_guidStr );
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
//	open OS (coreaudio) interface
//
void sdevice::os_midi::open()
{
	close();
	if (s_midi)
	{
		for (int dir=0; dir<2; dir++)
		{
			MIDIEndpointRef endpoint;
			uint32 portIndex = 0;
			int devices = dir ? ::MIDIGetNumberOfDestinations() : ::MIDIGetNumberOfSources();

			for (int device=0; device<devices; device++)
			{
				CFStringRef cf_guidStr;

				TCAT_EXCEPTION_THROW(!(endpoint = dir ? ::MIDIGetDestination(device) : ::MIDIGetSource(device)), xptn_os_midi);

				if (::MIDIObjectGetStringProperty(endpoint, CFSTR("DiceGUID"), &cf_guidStr) == noErr)
				{
					uint64		guid64 = 0;
					const char *guidStrPtr;
					int			items = 0;

					guidStrPtr = ::CFStringGetCStringPtr( cf_guidStr, kCFStringEncodingMacRoman );
					items = ::sscanf( guidStrPtr, "%llx", &guid64 );
					if (items == 1)
					{
						if (guid64 == m_device.guid64())
						{
							if (portIndex++ == m_port_idx)
							{
								m_endpoint[dir] = endpoint;
								if (!dir)
								{
									TCAT_EXCEPTION_THROW(::MIDIInputPortCreate(m_device.m_midi_client, CFSTR("dice midi in-port"), &static_callback, this, &m_port), xptn_os_midi);
									TCAT_EXCEPTION_THROW(::MIDIPortConnectSource(m_port, m_endpoint[dir], this), xptn_os_midi);
								}
								break;
							}
						}
					}
					::CFRelease(cf_guidStr);
				}
			}
		}
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::os_midi::close
//
///////////////////////////////////////////////////////////////////////////////
//
//	closing OS (coreaudio) interface
//
void sdevice::os_midi::close()
{
	if (m_port)
	{
		TCAT_EXCEPTION_THROW(::MIDIPortDisconnectSource(m_port,m_endpoint[false]), xptn_os_midi);
		TCAT_EXCEPTION_THROW(::MIDIPortDispose(m_port), xptn_os_midi);
		m_port=NULL;
	}
	m_endpoint[false]=NULL;
	m_endpoint[true]=NULL;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::os_midi::start
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::os_midi::start()
{
	if (m_port)
	{
		m_started = true;

		m_dataTwo = ((::rand() >> 14) & 0x7f);
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
	if (m_started)
	{
		if (s_trc)
			std::clog << "  (" << m_port_idx << ") os_midi::stop" << std::endl;
		::MIDIFlushOutput( m_endpoint[true] );

		m_started = false;
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::os_midi::static_callback
//
///////////////////////////////////////////////////////////////////////////////
//
void sdevice::os_midi::static_callback(const MIDIPacketList *pktlist, void *readProcRefCon, void *srcConnRefCon)
{
	sdevice::os_midi*		midi = (sdevice::os_midi*)readProcRefCon;

	if (!midi)
		return;

	if (midi->m_started)
	{
		uint32 data;
		#ifdef __BIG_ENDIAN__
		pal::swap32 (&data, (uint32*)pktlist->packet->data, 1);
		#else
		data=*(uint32*)pktlist->packet->data;
		#endif//__BIG_ENDIAN__
		midi->process(data);
	}
	else
	{
		if (s_trc)
			std::cout << "os_midi receive: m_started is false" << std::endl;
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::os_midi::send
//
///////////////////////////////////////////////////////////////////////////////
//
//	send midi to OS interface
//
void sdevice::os_midi::send(uint32 data)
{
	if (m_started)
	{
		MIDIPacketList pkt;

		pkt.numPackets=1;
		pkt.packet->timeStamp=0;
		pkt.packet->length=3;
		#ifdef __BIG_ENDIAN__
		pal::swap32 ((uint32*)pkt.packet->data, &data, 1);
		#else
		*(uint32*) pkt.packet->data = data;	
		#endif//__BIG_ENDIAN__

		TCAT_EXCEPTION_THROW(::MIDISend(m_device.m_midi_port, m_endpoint[true], &pkt), xptn_os_midi);
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
	super::create(bus,info);
	// create a midi client and an output port
	if (s_midi)
	{
		TCAT_EXCEPTION_THROW(::MIDIClientCreate(CFSTR("dice midi client"), NULL, NULL, &m_midi_client), xptn_os_midi);
		TCAT_EXCEPTION_THROW(::MIDIOutputPortCreate(m_midi_client, CFSTR("dice midi out-port"), &m_midi_port), xptn_os_midi);
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice::~sdevice
//
///////////////////////////////////////////////////////////////////////////////
//
sdevice::~sdevice()
{
	try
	{
		m_midi.clear(); 
		if (m_midi_port)
		{
			TCAT_EXCEPTION_THROW(::MIDIPortDispose(m_midi_port), xptn_os_midi);
			m_midi_port=NULL;
		}
		if (m_midi_client)
		{
			TCAT_EXCEPTION_THROW(::MIDIClientDispose(m_midi_client), xptn_os_midi);
			m_midi_client=NULL;
		}
	}
	catch(tcat::exception::base xptn)
	{
		exception(xptn);
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	callback_coreaudio
//
///////////////////////////////////////////////////////////////////////////////
//
//	this callback abstracts between the coreaudio (channel interleaved floating)
//	format and sbus->process()
//
#define max_32bit (1<<31)
OSStatus callback_coreaudio (
	AudioDeviceID, 
	const AudioTimeStamp*, 
	const AudioBufferList*buffer_in,
	const AudioTimeStamp*time_in,
	AudioBufferList*buffer_out,
	const AudioTimeStamp*time_out,
	void* client)
{
	sbus* bus = (sbus*)client;

	static int32 _in[kTCAT_DICE_MAX_CHANNELS][8192];
	static int32 _out[kTCAT_DICE_MAX_CHANNELS][8192];

	int32* in[kTCAT_DICE_MAX_CHANNELS];
	int32** pin = in;
	int32* out[kTCAT_DICE_MAX_CHANNELS];
	int32** pout = out;
	
	int32* plng;
	float* pflt;
	
	const AudioBuffer* buffer;
	
	uint32 io_size = bus->io_size();
	uint32 chnls;
	UInt32 strms;
	UInt32 chnlOffset;

	if (time_in->mSampleTime < 0.0)
	{
		TDL_REALTIME_ENTRY( kTDLFlag_Verbose | kTDLFlag_Audio, "DiceTool: callback_coreaudio skipping negative  (time_in->mSampleTime %d)",
							time_in->mSampleTime, 0, 0, 0 );
		return 0;
	}

	TDL_REALTIME_ENTRY( kTDLFlag_Verbose | kTDLFlag_Audio, "DiceTool: callback_coreaudio  (time_in->mSampleTime %d, time_out->mSampleTime %d)",
						time_in->mSampleTime, time_out->mSampleTime, 0, 0 );

	// de-interlace and convert in-channel floats to int32
	chnlOffset = 0;
	strms = buffer_in->mNumberBuffers;
	for (UInt32 strm=0; strm<strms; strm++)
	{
		buffer=&buffer_in->mBuffers[strm];
		chnls = buffer->mNumberChannels;
		for (UInt32 chnl=0; chnl<chnls; chnl++)
		{
			plng = _in[chnl + chnlOffset];
			*pin++ = plng;
			
			pflt = (float*) buffer->mData + chnl;
			for (uint32 smpl=io_size; smpl; --smpl)
			{
				*plng++ = *pflt * float(max_32bit);
				pflt+=chnls;
			}
		}
		chnlOffset += chnls;
	}

	// provide the de-interlaced channels
	chnlOffset = 0;
	strms = buffer_out->mNumberBuffers;
	for (UInt32 strm=0; strm<strms; strm++)
	{
		chnls = buffer_out->mBuffers[strm].mNumberChannels;
		for (UInt32 chnl=0; chnl<chnls; chnl++)
		{
			*pout++ = _out[chnl + chnlOffset];
		}
		chnlOffset += chnls;
	}

	bus->process( in, out, time_in->mSampleTime );
	
	// interlace and convert out-channel int32s to float
	chnlOffset = 0;
	for (UInt32 strm=0; strm<strms; strm++)
	{
		buffer=&buffer_out->mBuffers[strm];
		uint32 chnls = buffer->mNumberChannels;
		for (UInt32 chnl=0; chnl<chnls; chnl++)
		{
			pflt = (float*) buffer->mData + chnl;
			plng = _out[chnl + chnlOffset];
			for (uint32 smpl=io_size; smpl; --smpl)
			{
				*pflt = *plng++ / float(max_32bit);
				pflt+=chnls;
			}
		}
		chnlOffset += chnls;
	}

	return 0;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::os_audio_initialize
//
///////////////////////////////////////////////////////////////////////////////
//
//	open OS (coreaudio) interface
//
void sbus::os_audio_initialize()
{
	m_audio_id = 0;
	m_audio_io_proc_id = 0;
}



static OSStatus overloadListenerProc( AudioDeviceID inDevice, UInt32 numAddresses, const AudioObjectPropertyAddress addresses[], void* inClientData )
{
	for (UInt32 n = 0; n < numAddresses; n++)
	{
		const AudioObjectPropertyAddress *addr = &addresses[n];

		if (addr->mSelector == kAudioDeviceProcessorOverload )
		{
			if (addr->mScope == kAudioDevicePropertyScopeInput)
			{
				TDL_REALTIME_ENTRY( kTDLFlag_Error, "DiceTool: overload notification input channel %d)", addr->mElement, 0, 0, 0 );
			}
			else
			{
				TDL_REALTIME_ENTRY( kTDLFlag_Error, "DiceTool: overload notification output channel %d)", addr->mElement, 0, 0, 0 );
			}

			sbus* bus = (sbus*)inClientData;

			if (bus)
			{
				bus->increment_overload_count();
			}
		}
	}

	return noErr;
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::os_audio_open
//
///////////////////////////////////////////////////////////////////////////////
//
//	open OS (coreaudio) interface
//
void sbus::os_audio_open()
{
	if (!s_audio)
		return;
	try
	{
		AudioDeviceID device_list[0x10];
		AudioObjectPropertyAddress address;
		CFStringRef cf_name;
		char c_name[0x100];
		UInt32 size;

		// retrieve number of coreaudio devices
		address.mSelector = kAudioHardwarePropertyDevices;
		address.mScope = kAudioObjectPropertyScopeGlobal;
		address.mElement = kAudioObjectPropertyElementMaster;
		TCAT_EXCEPTION_THROW(::AudioObjectGetPropertyDataSize( kAudioObjectSystemObject, &address, 0, NULL, &size), xptn_os_audio);
		
		if (s_log)
			std::clog << "  found " << size/sizeof(AudioDeviceID) << " coreaudio devices:" << std::endl;
		// get a device list
		TCAT_EXCEPTION_THROW(::AudioObjectGetPropertyData( kAudioObjectSystemObject, &address, 0, NULL, &size, &device_list), xptn_os_audio);
		int devices=size/sizeof(AudioDeviceID);
		for (int device=0; device<devices; device++)
		{
			// retrieve the name and ID of each device
			address.mSelector = kAudioDevicePropertyDeviceName;
			address.mScope = kAudioObjectPropertyScopeGlobal;
			address.mElement = kAudioObjectPropertyElementMaster;
			size = sizeof(c_name);
			TCAT_EXCEPTION_THROW(::AudioObjectGetPropertyData( device_list[device], &address, 0, NULL, &size, &c_name ), xptn_os_audio);
			if (s_log)
				std::clog << (std::string(c_name)==kTCAT_DICE_USR_AUDIO_STR ?"  * ":"    ") << device_list[device] << ":" << c_name << std::endl;
			if (std::string(c_name)==kTCAT_DICE_USR_AUDIO_STR)
			{
				m_audio_id = device_list[device];
				m_name = c_name;
				break;
			}
		}
		// check for a valid ID
		TCAT_EXCEPTION_THROW(!m_audio_id, xptn_os_audio);
		
		// obtain stream and channel configuration for in- and output
		m_in=0;
		m_out=0;
		update_io_size();
		for (int input=0; input<=1; input++)
		{
			AudioBufferList streams[TCAT_MAX(kTCAT_DICE_TX,kTCAT_DICE_RX)*kTCAT_DICE_DEVICES];
			// retrieve the size of the stream configuration
			address.mSelector = kAudioDevicePropertyStreamConfiguration;
			address.mScope = input ? kAudioDevicePropertyScopeInput : kAudioDevicePropertyScopeOutput;
			address.mElement = kAudioObjectPropertyElementMaster;
			TCAT_EXCEPTION_THROW(::AudioObjectGetPropertyDataSize( m_audio_id, &address, 0, NULL, &size ), xptn_os_audio);
			// retrieve the stream configuration
			TCAT_EXCEPTION_THROW(size>sizeof streams, xptn_os_audio);
			TCAT_EXCEPTION_THROW(::AudioObjectGetPropertyData( m_audio_id, &address, 0, NULL, &size, &streams ), xptn_os_audio);
			if (s_log)
				std::clog << "      " << streams->mNumberBuffers << " " << (input?"input":"output") << " stream(s):" << std::endl;
			// obtain each streams channels
			for (int strm=0; strm<(int)streams->mNumberBuffers; strm++)
			{
				if (input)
					m_in += streams->mBuffers[strm].mNumberChannels;
				else
					m_out += streams->mBuffers[strm].mNumberChannels;
				if (s_log)
					std::clog << "      #" << strm << ": " << streams->mBuffers[strm].mNumberChannels << " channels" << std::endl;
			}
			for (int chnl=0; chnl<(input?m_in:m_out); chnl++)
			{
				address.mSelector = kAudioDevicePropertyChannelNameCFString;
				address.mElement = chnl+1;
				size = sizeof(cf_name);
				TCAT_EXCEPTION_THROW(::AudioObjectGetPropertyData(m_audio_id, &address, 0, NULL, &size, &cf_name), xptn_os_audio);
				TCAT_EXCEPTION_THROW(!::CFStringGetCString(cf_name, c_name, sizeof(c_name), kCFStringEncodingUTF8), xptn_os_midi);
				::CFRelease(cf_name);
				m_channel_names[input].push_back(std::string(c_name));
			}
		}

		AudioStreamRangedDescription streams[10];
		address.mSelector = kAudioStreamPropertyAvailableVirtualFormats;
		address.mScope = kAudioDevicePropertyScopeOutput;
		address.mElement = kAudioObjectPropertyElementMaster;
		TCAT_EXCEPTION_THROW(::AudioObjectGetPropertyDataSize( m_audio_id, &address, 0, NULL, &size), xptn_os_audio);
		TCAT_EXCEPTION_THROW(::AudioObjectGetPropertyData( m_audio_id, &address, 0, NULL, &size, &streams), xptn_os_audio);

		// listen for overload notifications
		address.mSelector = kAudioDeviceProcessorOverload;
		address.mElement = kAudioPropertyWildcardChannel;
		for (int input=0; input<=1; input++)
		{
			address.mScope = input ? kAudioDevicePropertyScopeInput : kAudioDevicePropertyScopeOutput;
			TCAT_EXCEPTION_THROW(::AudioObjectAddPropertyListener( m_audio_id, &address, overloadListenerProc, this ), xptn_os_audio);
		}

		// install callback_coreaudio
		TCAT_EXCEPTION_THROW(::AudioDeviceCreateIOProcID( m_audio_id, callback_coreaudio, this, &m_audio_io_proc_id ), xptn_os_audio);
	}
	catch(...)
	{
		os_audio_close();
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::os_audio_close
//
///////////////////////////////////////////////////////////////////////////////
//
//	closing OS (coreaudio) interface
//
void sbus::os_audio_close()
{
	if (m_audio_id)
	{
		AudioObjectPropertyAddress address;

		address.mSelector = kAudioDeviceProcessorOverload;
		address.mElement = kAudioPropertyWildcardChannel;
		for (int input=0; input<=1; input++)
		{
			address.mScope = input ? kAudioDevicePropertyScopeInput : kAudioDevicePropertyScopeOutput;
			TCAT_EXCEPTION_THROW(::AudioObjectRemovePropertyListener( m_audio_id, &address, overloadListenerProc, this ), xptn_os_audio);
		}

		::AudioDeviceDestroyIOProcID( m_audio_id, m_audio_io_proc_id );
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::os_audio_start
//
///////////////////////////////////////////////////////////////////////////////
//
//	starting OS (coreaudio) streaming
//
void sbus::os_audio_start()
{
	if (m_audio_id)
	{
		OSStatus	status = ::AudioDeviceStart( m_audio_id, m_audio_io_proc_id );
		if (status != 0)
			std::clog << "AudioDeviceStart() returned " << std::hex << "0x" << status << std::endl;

		TCAT_EXCEPTION_THROW(status, xptn_os_audio);
	}
	if (s_midi)
	{
		for (iterator device=begin(); device != end(); device++)
		{
			(*device)->midi_start();
		}
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	sbus::os_audio_stop
//
///////////////////////////////////////////////////////////////////////////////
//
//	stopping OS (coreaudio) streaming
//
void sbus::os_audio_stop()
{
	if (s_midi)
	{
		for (iterator device=begin(); device != end(); device++)
		{
			(*device)->midi_stop();
		}
	}
	if (m_audio_id)
	{
		::AudioDeviceStop( m_audio_id, m_audio_io_proc_id );
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	sbus::io_size
//
///////////////////////////////////////////////////////////////////////////////
//
//	changing the I/O size requires the OS audio drivers to be closed 
void sbus::io_size (uint32 io_size) throw(tcat::exception::base)
{
	if (m_audio_id)
	{
		AudioObjectPropertyAddress address;

		address.mSelector = kAudioDevicePropertyBufferFrameSize;
		address.mScope = kAudioDevicePropertyScopeOutput;
		address.mElement = kAudioObjectPropertyElementMaster;
		TCAT_EXCEPTION_THROW(::AudioObjectSetPropertyData( m_audio_id, &address, 0, NULL, sizeof(m_io_size), &io_size), xptn_os_audio);

		update_io_size(); // hack: should be registered from CoreAudio client
	}
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
		std::clog << "  " << TCAT_FUNCTION << std::endl; 
	UInt32 io_size[2];
	for (int input=0; input<=1; input++)
	{
		AudioObjectPropertyAddress address;
		UInt32 size;
		UInt32 safety_offset;

		address.mSelector = kAudioDevicePropertyBufferFrameSize;
		address.mScope = input ? kAudioDevicePropertyScopeInput : kAudioDevicePropertyScopeOutput;
		address.mElement = kAudioObjectPropertyElementMaster;

		// retrieve the latency
		address.mSelector = kAudioDevicePropertyLatency;
		size = sizeof(m_latency[input]);
		::AudioObjectGetPropertyData( m_audio_id, &address, 0, NULL, &size, &m_latency[input]);

		address.mSelector = kAudioDevicePropertyBufferFrameSize;
		size = sizeof(io_size[input]);
		::AudioObjectGetPropertyData( m_audio_id, &address, 0, NULL, &size, &io_size[input]);

		address.mSelector = kAudioDevicePropertySafetyOffset;
		size = sizeof(safety_offset);
		::AudioObjectGetPropertyData( m_audio_id, &address, 0, NULL, &size, &safety_offset);

		if (s_log)
			std::clog << "      " << (input?"input":"output") << " latency:" << m_latency[input] << " + I/O size:" << io_size[input] << " + safety offset:" << safety_offset << std::endl;

		m_latency[input] += io_size[input] + safety_offset; 
	}
	m_io_size = io_size[false]; // assuming in==out;
}

