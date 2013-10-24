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
//	dice pal streaming classes using the dice pal classes
//
#pragma once
#include "dice.h"
#include "tcat_dice_pal_system.h"
#include <iostream>
#include <iomanip>

#ifdef WIN32
#include "asiosys.h"
#include "asio.h"
#include "asiodrivers.h"
std::string wstrtostr(const std::wstring &wstr);
#endif
#ifdef __MACH__
#include "CoreAudio/CoreAudio.h"
#include "CoreMIDI/CoreMIDI.h"
#endif


namespace tcat
{

#define kTCAT_DICE_MAX_CHANNELS		256

TCAT_EXCEPTION_DECLARE (xptn_os_audio, dice::xptn_internal, "OS audio exception");
TCAT_EXCEPTION_DECLARE (xptn_os_midi, dice::xptn_internal, "OS midi exception");
TCAT_EXCEPTION_DECLARE (xptn_feature, tcat::exception::base, "Feature not implemented");



#define TCAT_DICE_CHANNEL_CROSS(chnl,squ) (((chnl)%((squ)*2)) | (((((chnl)+(squ))/((squ)*2))&1)*2*(squ)))
#define TCAT_DICE_ROUTER_CROSS(chnl,squ) (((chnl)%((squ)*2)) | (((((chnl)+(squ))/((squ)*2))&1)*16))

#define out(x)					{std::cout << "  " << #x << ": " << x << std::endl;}
#define out0(x)					{if (x) std::cout << "  " << #x << ": " << x << std::endl;}
#define outf0(x,w,m)			{std::cout << std::setw(w); if (x) std::cout << std::min<uint32>((x),(m)); else std::cout << " "; std::cout << "|";}
#define outf0_(x,w,m)			{std::cout << std::setw(w); if (x) std::cout << std::min<uint32>((x),(m)); else std::cout << " "; std::cout << " ";}
#define logf0(x,w,m)			{std::clog << std::setw(w); if (x) std::clog << std::min<uint32>((x),(m)); else std::clog << " "; std::clog << "|";}
#define logf0_(x,w,m)			{std::clog << std::setw(w); if (x) std::clog << std::min<uint32>((x),(m)); else std::clog << " "; std::clog << " ";}
#define file0(x)				{if (x) file << x; file << ",";}
#define eout(exception)			{ std::cerr << std::endl << (exception).usr_msg() << std::endl; if (s_trc) std::cerr << "  " << TCAT_FUNCTION << ":" << (exception).name() << std::endl << "  " << (exception).dbg_msg() << std::endl << "  " << (exception).filename() << std::endl << "  line:" << (exception).line() << std::endl << std::flush; }
#define VERSION(v)				TCAT_VERSION32_MAJOR(v)<<"."<<TCAT_VERSION32_MINOR(v)<<"."<<TCAT_VERSION32_SUB(v)<<"."<<TCAT_VERSION32_BUILD(v)<<((v)&kTCAT_DICE_VERSION_MASK_BUILDSERVER?"":" local")<<((v)&kTCAT_DICE_VERSION_MASK_BETA?" beta":"")<<((v)&kTCAT_DICE_VERSION_MASK_INSTRUMENTED?" instrumented":"")

#define list_arguments(array)	{for(int __list_idx=0; __list_idx<sizeof(array)/sizeof(array[0]); __list_idx++) {std::cout << array[__list_idx]; if (__list_idx+1<sizeof(array)/sizeof(array[0])) std::cout << ", ";}}



extern bool s_log;
extern bool s_trc;
extern bool s_audio;
extern bool s_midi;
extern bool s_qut;
extern bool s_plugin;
volatile extern bool s_exit;



class counter
{
public:
	counter()		{ reset(); }
	void reset()	{ min=0xffffffff; current=first_occurence=max=changes=0; }
	bool operator==(const counter& test) const { return (test.min==min) && (test.max==max) && (test.current==current) && (test.changes==changes) && (test.first_occurence==first_occurence); }
	uint32	min;
	uint32	max;
	uint64	current;
	uint32	changes;
	uint64	first_occurence;
};



///////////////////////////////////////////////////////////////////////////////
//
//	sdevice
//
///////////////////////////////////////////////////////////////////////////////
//
class sdevice: public dice::device
{
friend	class dice::reference<sdevice>;
typedef	dice::device super;
public:
#define	trc	{ if (s_trc) std::clog << "    " << TCAT_FUNCTION << "[" << nickname() << "]" << std::endl; }
	struct COUNTER
	{
		struct
		{
			uint32 slips;
			uint32 unlocks;
		} status[dice::kExStatusItems];	///< slip and unlock counter for each sync option
	};
									sdevice()
										#ifdef __MACH__
										:
										m_midi_client(NULL),
										m_midi_port(NULL)
										#endif //__MACH__
																				{ trc; reset_counters(); }
	virtual			void			create				(dice::bus_impl* bus, const dice::DEVICE_INFO& info)throw(tcat::exception::base);
	virtual			bool			update()																throw(tcat::exception::base);
	virtual			void			mount()										{ trc; super::mount(); }
	virtual			void			unmount()									{ trc; super::unmount(); }
	virtual	const	std::string		name_midi(uint32 port)				const	{ return m_midi.at(port).m_name; }
	virtual			const counter&	midi_counter(uint32 port)			const	{ return m_midi.at(port).m_counter; }
					void			reset_counters();	
					void			midi_start();
					void			midi_stop();
					void			update_midi() throw(tcat::exception::base);

	inline			COUNTER&		counters()									{ return m_counters; }
					void			cmd_info();
	virtual			void			exception			(const tcat::exception::base& exception)		{ eout(exception); }
					void			firmware_router		(uint32 address, uint32 data)						throw(tcat::exception::base);
					uint32			firmware_router		(uint32 address)							const	throw(tcat::exception::base);
					void			firmware_avs		(dice::AVS& avs)							const	throw(tcat::exception::base);
					void			firmware_progress	(uint32 percent, dice::FL_OP_ENUM operation)const;
					int				firmware_phase		(uint32 avs)								const	throw(tcat::exception::base);
					
					void			cmd_mymode			(std::string argv);
					void			cmd_clear();
					void			cmd_nickname		(std::string argv);
					void			cmd_loopback		(std::string argv);
					void			cmd_firmware_upload	(std::string argv);
					void			cmd_firmware_download(std::string argv);
					void			cmd_firmware_delete	(std::string argv);
					void			cmd_dump			(std::string argv);
					void			cmd_write			(std::string argv);
					void			cmd_bnc				(std::string argv);
					void			cmd_busy			(std::string argv);
					void			cmd_phase();
protected:
	virtual							~sdevice();
	virtual			void			update_rx()				throw(tcat::exception::base);
	virtual			void			update_tx()				throw(tcat::exception::base);
	virtual			void			update_clock()								{ trc;	if (s_log) { std::clog << "    + clk: "; if (m_cache[0].clock.status & dice::kStatusLocked) std::clog << std::dec << clock() << "Hz"; else std::clog << "invalid"; std::clog << std::endl; } }
	virtual			void			update_ex_status()							
	{
		trc; 	
		if (s_log) 
		{
			std::string lock_names[] = {"Aes0",	"Aes1", "Aes2", "Aes3", "Adat", "Tdif", "Arx0", "Arx1", "Arx2", "Arx3"}; 
			for (int bit=0; bit<(int)dice::kExStatusItems; bit++)
			{
				int flag = 1<<bit;
				if ((m_cache[0].clock.ex_status & flag) != (m_cache[1].clock.ex_status & flag))
				{
					m_counters.status[bit].unlocks++;
					if (s_log)
						std::clog << "    + " << lock_names[bit] << ":" << (m_cache[0].clock.ex_status & flag?"":"not ") << "locked" << std::endl;
				}
			}
		}
	}
	virtual			void			update_locked()								{ trc; if (s_log) std::clog << "    + " << ((m_cache[0].clock.status & dice::kStatusLocked) ? "":"not ") << "locked" << std::endl; } 
	virtual			void			update_tx_locked()							{ trc; }
	virtual			void			update_nickname()							{ trc; if (s_log) std::clog << "    + " << (*(uint32*)m_cache[1].info.nickname==0xffffffff?"":m_cache[1].info.nickname) << "->" << nickname() << std::endl; }
	virtual			void			update_idx()								{ trc; if (s_log) std::clog << "    + idx:" << m_cache[1].info.idx << "->" << idx() << std::endl; }
	virtual			void			update_state()								{ trc; if (s_log) std::clog << "    + state:" << m_cache[1].info.state << "->" << state() << std::endl; }
	virtual         void            update_user(tcat::dice::DEVICE_NOTIFICATION notification) { trc; if (s_log) std::clog << "    + user: 0x" << std::hex << notification << std::dec << std::endl; }

					COUNTER					m_counters;				///< Unlock, slip error counters
					#ifdef __MACH__
					MIDIClientRef			m_midi_client;
					MIDIPortRef				m_midi_port;	// MIDI-out port
					#endif //__MACH__
public:
					class os_midi
					{
					public:
						os_midi(sdevice &device, uint32 port): 
							m_device( device ), 
							m_port_idx( port ),
							m_started( false ),
							#ifdef __MACH__
							m_port( NULL ), 
							#endif //__MACH__
							m_name( name() ),
							m_errors( 0 ),
							m_dataOne( 0 ),
							m_dataTwo( 0 ),
							m_dataOneExpected( 0 )
						{ 
							#ifdef __MACH__
							m_endpoint[false]=NULL;
							m_endpoint[true]=NULL;
							#endif //__MACH__ 
							#ifdef WIN32
							m_handle[false]=kTCAT_INVALID_HANDLE;
							m_handle[true]=kTCAT_INVALID_HANDLE;
							#endif //WIN32
						}
						~os_midi()
						{
							try
							{
								stop();
								close();
							}
							catch(tcat::exception::base xptn)
							{
								m_device.exception(xptn);
							}
						}

						std::string name();
						void	open();
						void	close();
						void	start();
						void	stop();
						void	process		(uint32 data);
						void	send		(uint32 data);	// send midi data
						#ifdef __MACH__
						static void static_callback(const MIDIPacketList *pktlist, void *readProcRefCon, void *srcConnRefCon);
						#endif

						sdevice&			m_device;
						uint32				m_port_idx;
						bool				m_started;
						#ifdef __MACH__
						MIDIEndpointRef		m_endpoint[2];
						MIDIPortRef			m_port;			// MIDI-in port
						#endif //__MACH__
						#ifdef WIN32
						handle				m_handle[2];
						#endif //WIN32
						std::string			m_name;
						counter				m_counter;
						uint64				m_errors;
						uint8				m_dataOne;
						uint8				m_dataTwo;
						uint8				m_dataOneExpected;
					};
					std::deque<os_midi>		m_midi;
};
typedef dice::reference<sdevice> sdevice_ref;



///////////////////////////////////////////////////////////////////////////////
//
//	sbus
//
///////////////////////////////////////////////////////////////////////////////
//
class sbus: public dice::bus<sdevice>
{
	typedef	dice::bus<sdevice> super;
public:
									sbus():
										#ifdef __MACH__
										m_io_size(0),
										m_overloads(0),
										#endif //__MACH__
										m_restart_request(false),
										m_restarts(0),
										m_name(), 
										m_in(0),
										m_out(0),
										m_channel_mask(0xffffffffffffffffull), 
										m_xcross(0),
										m_load(0)
									{
										memset(m_latency,0,sizeof m_latency);
										os_audio_initialize(); 
									}
									enum CHANNEL_COUNTER
									{
										kChannelCounterDropouts,
										kChannelCounterLatency,
										kChannelCounterSlips,
										kChannelCounters
									};
									enum PROCESS_COUNTER
									{
										kProcessCounterUser,
										kProcessCounterDriver,
										kProcessCounterSync,
										kProcessCounters
									};
									enum 
									{
										kProgressOpenMidi = super::kProgressOpen+1,
									};

	virtual			void			create(dice::system_impl* system, const dice::bus_impl::ID id)	throw(tcat::exception::base);
	virtual							~sbus();
	virtual			void			mount()																						{ if (s_trc) std::clog << "  " << TCAT_FUNCTION << "[" << id() << "]" << std::endl; super::mount(); }
	virtual			void			unmount()																					{ if (s_trc) std::clog << "  " << TCAT_FUNCTION << "[" << id() << "]" << std::endl; super::unmount(); }
	virtual	const	std::string		name()												const									{ return m_name; }

			const	uint32			channels_in()										const									{ return m_in; }
			const	uint32			channels_out()										const									{ return m_out; }
			const	std::string		channel_name		(bool input, uint32 channel)	const									{ if ((int32)channel >= 0)
																																	{
																																		return m_channel_names[input][channel];
																																	}
																																	else
																																	{
																																		return ""; // for unmapped channels: channel = -1
																																	}
																																}
			const	uint64			channel_mask()										const									{ return m_channel_mask; }
			const	void			channel_mask		(uint64 channel_mask)													{ m_channel_mask=channel_mask; }

	#ifndef TCAT_DICE_AVC
	virtual	const	dice::CLOCK&	clock()												const									{ return super::clock(); } 
	virtual			void			clock				(dice::CLOCK clock)						throw(tcat::exception::base);
	#endif//TCAT_DICE_AVC
			const	uint32			clock_rate()										const									{ uint32 clock_rate[] = {32000,44100,48000,88000,96000,176000,192000}; return clock_rate[clock()]; }
	#ifdef WIN32
					void			cmd_speaker_config	(std::string argv);
					void			cmd_speaker_map		(std::string argv);
	virtual	const	uint32			io_size()											const									{ return super::io_size(); }
	#endif//WIN32
	#ifdef __MACH__
	virtual	const	uint32			io_size()											const									{ return m_io_size; }
	virtual	const	uint32			overloads()											const									{ return m_overloads; }
	virtual	const	void			increment_overload_count()																	{ ++m_overloads; }
	#endif//__MACH__
	virtual			void			io_size				(uint32 io_size)						throw(tcat::exception::base);

					void			os_audio_start();
					void			os_audio_stop();
					bool			os_audio_check();
					void			os_audio_restart_request()																	{ m_restarts++; m_restart_request=true; }
					uint32			os_audio_restarts()									const									{ return m_restarts; }

					uint32			latency(bool out)									const									{ return m_latency[out]; }
					counter&		process_counter		(PROCESS_COUNTER ctr)													{ return m_process_counter[ctr]; }

					counter&		channel_counter		(uint32 channel, CHANNEL_COUNTER ctr)									{ return m_counters[channel].counters[ctr]; }
					uint64&			channel_errors		(uint32 channel)														{ return m_counters[channel].errors; }
					void			reset_counters();	
					
					void			process				(int32* in[], int32* out[], uint64 sample_position);

					uint32			xcross()											const									{ return m_xcross; }
					void			xcross(uint32 xcross)																		{ m_xcross = xcross; }
					void			cmd_info();
	#ifndef TCAT_DICE_AVC
					void			cmd_op_mode			(std::string argv);
					void			cmd_sync			(std::string argv);
					void			cmd_clock			(std::string argv);
					void			cmd_iosize			(std::string argv);
					void			cmd_master			(std::string argv);
					void			cmd_add_latency		(std::string argv);
	#endif//TCAT_DICE_AVC
					void			cmd_channel_mask	(std::string argv);
					void			cmd_streaming		(std::string argv);
	virtual			bool			update()													throw(tcat::exception::base);
	virtual			void			open()														throw(tcat::exception::base);
	virtual			void			close()														throw(tcat::exception::base);
	virtual			void			update_devicelist()											throw(tcat::exception::base);
	virtual			void			update_devices()											throw(tcat::exception::base)		{ if (s_trc) std::clog << "  " << TCAT_FUNCTION << std::endl; super::update_devices(); }

//	virtual			void			ready_wait()												throw(tcat::exception::base)		{ if (s_trc) std::clog << "  " << TCAT_FUNCTION << "[" << id() << "]" << std::endl; super::ready_wait(); }
//	virtual			void			ready_set()													throw(tcat::exception::base)		{ if (s_trc) std::clog << "  " << TCAT_FUNCTION << "[" << id() << "]" << std::endl; super::ready_set(); }
//	virtual			void			ready_reset()												throw(tcat::exception::base)		{ if (s_trc) std::clog << "  " << TCAT_FUNCTION << "[" << id() << "]" << std::endl; super::ready_reset(); }
protected:
					double			qi();
					int				ma();
					void			log_streaming_head();
					void			log_streaming		(uint32 time_elapsed, uint32 time_left, int32 dpc);
					void			log_streaming_synopsis(uint32 duration, int32 dpc);
					void			log_file			(std::string filename, uint32 duration, int32 dpc);
	virtual			void			update_clients()				{ if (s_trc) std::clog << "  " << TCAT_FUNCTION << ": " << super::clients() << std::endl; super::update_clients(); }
	#ifndef TCAT_DICE_AVC
	virtual			void			update_clock()					{ if (s_trc) std::clog << "  " << TCAT_FUNCTION << ": " << clock_name(super::clock()) << std::endl; super::update_clock(); }
	virtual			void			update_sync()					{ if (s_trc) std::clog << "  " << TCAT_FUNCTION << ": " << sync_name(super::sync()) << std::endl; super::update_sync(); }
	virtual			void			update_sync_caps()				{ if (s_trc) std::clog << "  " << TCAT_FUNCTION << ": " << std::endl; super::update_sync_caps(); }
	virtual			void			update_sync_names()				{ if (s_trc){std::clog << "  " << TCAT_FUNCTION << ": "; for (int idx=0; idx<dice::kSyncItems; idx++) std::clog << sync_name(dice::SYNC(idx)) << ", "; std::clog << std::endl;} super::update_sync_names(); }
	virtual			void			update_op_mode()				{ if (s_trc) std::clog << "  " << TCAT_FUNCTION << ": " << std::hex << super::op_mode() << std::endl; super::update_op_mode(); }
	virtual			void			update_master()					{ if (s_trc) std::clog << "  " << TCAT_FUNCTION << ": " << std::hex << super::master() << std::endl; super::update_master(); }
	#endif//TCAT_DICE_AVC
	#ifdef WIN32
	virtual			void			update_speaker()				{ if (s_trc) std::clog << "  " << TCAT_FUNCTION << ": " << std::endl; super::update_speaker(); }
	virtual			void			update_speaker_map	(bool out, uint32 speaker)	{ if (s_trc) std::clog << "  " << TCAT_FUNCTION << ": " << std::endl; super::update_speaker_map(out,speaker); }
	#endif//WIN32
	virtual			void			update_io_size();
	virtual			void			progress(uint32 percent, uint32 ctrl)				const;
	virtual			void			exception			(const tcat::exception::base& exception)								{ eout(exception); }

					void			os_audio_initialize();
					void			os_audio_open();
					void			os_audio_close();

					#ifdef WIN32
	static			ASIOTime*		callback_asio_bufferswitch_timeinfo (ASIOTime *timeInfo, long index, ASIOBool processNow);

					ASIOBufferInfo*			m_buffers;
					ASIOCallbacks			m_callbacks;	// dice asio doesn't make a copy of it; so keep the buffer alive!
					ASIODriverInfo			m_asio;
					#endif //WIN32
					#ifdef __MACH__
					AudioDeviceID			m_audio_id;
					AudioDeviceIOProcID		m_audio_io_proc_id;
					uint32					m_io_size;
					uint64					m_overloads;
					#endif //__MACH__
					bool					m_restart_request;
					uint32					m_restarts;
					std::string				m_name;
					int32					m_in;
					int32					m_out;
					uint64					m_channel_mask;
					#ifdef __MACH__
					uint32					m_latency[2];
					#else
					long					m_latency[2];
					#endif
					uint32					m_xcross;
					struct
					{
						counter				counters[kChannelCounters];
						uint64				errors;
					} m_counters[kTCAT_DICE_MAX_CHANNELS];
					counter					m_process_counter[kProcessCounters];
					std::deque<std::string>	m_channel_names[2];
					uint32					m_load;
};
typedef dice::reference<sbus> sbus_ref;



///////////////////////////////////////////////////////////////////////////////
//
//	ssystem
//
///////////////////////////////////////////////////////////////////////////////
//
//	streaming system class
//	this is only used to overwrite the exception handler.
//
class ssystem: public dice::system<sbus>
{
	typedef dice::reference<ssystem>	self_ref;
	typedef dice::system<sbus>			super;
public:
	virtual	void	create()			throw(tcat::exception::base);
	virtual			~ssystem();
	virtual	void	update_buslist()	throw(tcat::exception::base);
	virtual	bool	fetch_buslist()		throw(tcat::exception::base);
			void	cmd_info();
	inline	uint32	time_open()		const									{ return m_time_open; }
	virtual void	exception	(const tcat::exception::base& exception)	{ eout(exception); }
private:
	uint32	m_time_open;
};
typedef dice::reference<ssystem> ssystem_ref;



} // namespace tcat
