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
//	TCAT command line tool
//
#ifdef WIN32
#include <windows.h>
#include <asiodrivers.h>
#include "DPCChk.h"
#endif
#ifdef __MACH__
#include <IOKit/IOKitLib.h>
#include "tcat.h"
#include "tcat_dice.h"
#include "tcat_dice_mac.h"
#endif

#include <fstream>
#include "tcat_dice_version.h"
#include "dice_streaming.h"
#include "TDL_Interface.h"


#define kDriverLog_File_ID		1100


using namespace tcat;



///////////////////////////////////////////////////////////////////////////////
//
//	help
//
///////////////////////////////////////////////////////////////////////////////
//
void help()
{
	static bool shown = false;
	if (!shown)
	{
		std::cout << "dice" <<
		#ifdef _AMD64_
			"\64"
		#endif //_AMD64_
			" - command line util - ver. " << kTCAT_DICE_VERSION_EXTENDED_STR << std::endl << 
			"  " kTCAT_DICE_USR_COPYRIGHT_STR " (built " << __DATE__ << " " << __TIME__ << ")" << std::endl <<
			"  customized for " kTCAT_DICE_USR_STR " (" kTCAT_DICE_USR_VENDOR_STR ")" <<
			std::endl << std::endl;
		std::cout << "dice [optionPARAMETER] [optionPARAMETER] [..] - no space between option and PARAMETER!" << std::endl << std::endl;
		std::cout << "system operation:" << std::endl;
		std::cout << "  sg[SECONDS]               system loop with notification display" << std::endl;
		std::cout << "  si                        system information" << std::endl;
		std::cout << "bus operation:" << std::endl;
		std::cout << "  #b[BUS_NUMBER]            select bus #" << std::endl;
		std::cout << "  bb[IOBUFFERSIZE]          set/display I/O buffer size" << std::endl;
		std::cout << "  bc[CLOCK]                 set/display clock" << std::endl;
		std::cout << "  bd[NICKNAME|IDX|GUID64]   set/display sync master device" << std::endl;
		std::cout << "  be                        clear driver notification lock/slip error counter" << std::endl;
		std::cout << "  bg[SECONDS]               system loop with notification display [holding a bus ref]" << std::endl;
		std::cout << "  bi                        bus information" << std::endl;
		std::cout << "  bl[IN,OUT]                add additional amount of reported audio latency" << std::endl;
		std::cout << "  bm[MASK]                  set/display streaming test channel mask" << std::endl;
		std::cout << "  bo[OP_MODE]               set/display op mode" << std::endl;
		std::cout << "  btSECONDS[,[LOGFILE]][,%] streaming test for SECONDS, add % CPU load and output result to LOGFILE" << std::endl;
		std::cout << "  bt                        prints the streaming test help screen" << std::endl;
		std::cout << "  by[SYNC_SOURCE]           set/display sync source" << std::endl;
		std::cout << "  bx[AVS_SIZE]              set mode to cross out audio channels to different isocs receivers" << std::endl;
		std::cout << "                            (AVS_SIZE normally 8 and to be followed by dl)" << std::endl;
		std::cout << "  b#                        display number of open audio driver clients" << std::endl;
		#ifdef WIN32
		std::cout << "bus speaker operation:" << std::endl;
		std::cout << "  bsc                       display out speaker config" << std::endl;
		std::cout << "  bsm                       display speaker mapping" << std::endl;
		#endif//WIN32
		std::cout << "device operation:" << std::endl;
		std::cout << "  #d[DEVICE_NUMBER]         select device #" << std::endl;
		std::cout << "  dbMS                      hold the device busy for MS" << std::endl;
		std::cout << "  ddOFFSET[,WORDS]          dump firewire address space" << std::endl;
		std::cout << "  de                        clear AVS receivers error counter" << std::endl;
		std::cout << "  dg[SECONDS][,READADR]     system loop with notification display [holding a bus and a device ref]" << std::endl;
		std::cout << "  di                        device information" << std::endl;
		std::cout << "  dl[CHANNEL]               set loopback for all channels or for one CHANNEL" << std::endl;	
		std::cout << "  dm[MODE]                  set mymode" << std::endl;
		std::cout << "  dn[NICKNAME]              set/display nickname" << std::endl;
	//	std::cout << "  um[CHANNEL]               muting all channels or one CHANNEL" << std::endl;
		std::cout << "  dr                        firmware reset" << std::endl;
		std::cout << "  dt[ENABLE|PULSE]          enable/disable WCLK BNC trigger when AVS errors or pulse a trigger" << std::endl;
		std::cout << "  dwOFFSET,FILENAME         write ASCII data file to firwire address space" << std::endl;
		std::cout << "  dp                        display the relative phase between PLL and AVS receiver(s)" << std::endl;
		std::cout << "device firmware operation:" << std::endl;
		std::cout << "  dfu[SECTORNAME,]FILENAME  upload firmware to device" << std::endl;
		std::cout << "  dfxSECTORNAME             delete firmware sector" << std::endl;
		std::cout << "  dfd[SECTORNAME,]FILENAME  download firmware sector SECTORNAME from device;" << std::endl;
		std::cout << "                            don't give a SECTORNAME to download the entire flash" << std::endl;
		std::cout << "general:" << std::endl;
		std::cout << "  +|-[t|l|m|a|q|p]          en/disable [tracing|logging|MIDI|audio|quiet|plugin mode]" << std::endl;
		std::cout << "  i                         general environment information" << std::endl;
		#ifdef WIN32
		std::cout << "  lp                        get DPC latency" << std::endl;	
		std::cout << "  li                        install DPC latency chk" << std::endl;	
		std::cout << "  lu                        uninstall DPC latency chk" << std::endl;	
		std::cout << "  o[vas]                    show OHCI 1394 host controller info [verbose|show all devices|search online]" << std::endl;
		#endif//WIN32
		shown = true;
	}
}



///////////////////////////////////////////////////////////////////////////////
//
//	help_streaming
//
///////////////////////////////////////////////////////////////////////////////
//
void help_streaming()
{
	std::cout << "dtSECONDS[,LOGFILE] streaming test for SECONDS output result to LOGFILE" << std::endl;
	std::cout << "  a synopsis is printed out after the streaming test having the following meaning:" << std::endl;
	std::cout << "  init gap: an initial gap is expected after starting a stream test. the size in samples is reported here." << std::endl;
	std::cout << "  latency:" << std::endl;
	std::cout << "  - last: the last reported latency of this channel" << std::endl;
	std::cout << "  - min: if there were slips during the test the minimal measured latency of that channel is reported" << std::endl;
	std::cout << "  - max: if there were slips during the test the maximal measured latency of that channel is reported" << std::endl;
	std::cout << "  - expc: the in+out latency reported by the driver" << std::endl;
	std::cout << "  - diff: the difference of the expected latency (expc) and last reported latetency" << std::endl;
	std::cout << "  slips:" << std::endl;
	std::cout << "  - amnt: the amount of slips" << std::endl;
	std::cout << "  - min: if there were slips during the test the minimal measured slip amount of that channel is reported" << std::endl;
	std::cout << "  - max: if there were slips during the test the maximal measured slip amount of that channel is reported" << std::endl;
	std::cout << "  - first: if there were slips during the test the sample offset of the first occurrence is reported" << std::endl;
	std::cout << "  dropouts:" << std::endl;
	std::cout << "  - amnt: the amount of dropouts; a dropout is considered to be one or more sample in size" << std::endl;
	std::cout << "  - min: if there were dropouts during the test the minimal measured dropout size of that channel is reported" << std::endl;
	std::cout << "  - max: if there were dropouts during the test the maximal measured dropout size of that channel is reported" << std::endl;
	std::cout << "  - first: if there were dropouts during the test the sample offset of the first occurrence is reported" << std::endl;
	std::cout << "  total errors: - number of invalid samples per channel (slips will be counted by slip size)" << std::endl;
	std::cout << "  drv: (reported by driver via its EXTENDED STATUS" << std::endl;
	std::cout << "  - ul: counted unlocks for the channels of that AVS stream" << std::endl;
	std::cout << "  - sl: counted slips for the channels of that AVS stream" << std::endl;
	std::cout << "  tx: (device AVS transmitter errors:" << std::endl;
	std::cout << "  - BN: ATX_BOUNDARY" << std::endl;
	std::cout << "  arx: (device AVS receiver errors:" << std::endl;
	std::cout << "  - SA: ADO*_SYT_AGEOUT: this ADO had to age-out a stale frame of data" << std::endl;
	std::cout << "  - DB: ARX*_DBC_FAIL: This ARX module detected a discontinuity in the DBC" << std::endl;
	std::cout << "  - PA: ARX*_PKT_ABORT: This ARX was forced to abort an isoc packet" << std::endl;
	std::cout << "  - SO: ARXDO*_SYT_OVERFLOW: The timestamp fifo of this ARXDO has overflowed" << std::endl;
	std::cout << "  midi latency:" << std::endl;
	std::cout << "  - pckts: number of packets received" << std::endl;
	std::cout << "  - min: minimal measured latency (in samples)" << std::endl;
	std::cout << "  - max: minimal measured latency (in samples)" << std::endl;
	std::cout << "  streaming quality index (SQI):" << std::endl;
	std::cout << "    0: no error (SQI is discontinued between 0 and 1)" << std::endl;
	std::cout << "    1: 0.0000001% or less data faulty" << std::endl;
	std::cout << "    2: 0.000001% data faulty" << std::endl;
	std::cout << "    3: 0.00001% data faulty" << std::endl;
	std::cout << "    4: 0.0001% data faulty" << std::endl;
	std::cout << "    5: 0.001% data faulty" << std::endl;
	std::cout << "    6: 0.01% data faulty" << std::endl;
	std::cout << "    7: 0.1% data faulty" << std::endl;
	std::cout << "    8: 1% data faulty" << std::endl;
	std::cout << "    9: 10% data faulty" << std::endl;
	std::cout << "    10:100% data faulty - no data transmitted" << std::endl;
	std::cout << "  micro alignment (MA):" << std::endl;
	std::cout << "    shows latency variation across all in/out channels. (expected value:0)" << std::endl;
	std::cout << "    micro alignment errors values might also be introduced by different hardware latencies across some of the channels" << std::endl;
	std::cout << "  OS audio callback precision (drv):" << std::endl;
	std::cout << "    min: displays the minimal maesured time between processing requests in us and % (expected value: 100% for accurate timing)" << std::endl;
	std::cout << "    max: displays the maximal maesured time between processing requests in us and % (expected value: 100% for accurate timing)" << std::endl;
	std::cout << "  CPU processing consumption (proc):" << std::endl;
	std::cout << "    min: displays the minimal maesured time consumed by this streaming test in us and % (expected value: low as possible)" << std::endl;
	std::cout << "    max: displays the maximal maesured time consumed by this streaming test in us and % (expected value: low as possible)" << std::endl;
	std::cout << "    for test purpose time can be added to the time spent in the processing callback (see bt)" << std::endl;
	std::cout << "  MIDI streaming (midi):" << std::endl;
	std::cout << "    midi[DEVICE].[PORT]: MIDI messages sent and received per second (theoretical max: 3125bps=1041mps)" << std::endl;	
}



///////////////////////////////////////////////////////////////////////////////
//
//	info
//
///////////////////////////////////////////////////////////////////////////////
//
void info()
{
	std::cout << "system information:" << std::endl;
	std::cout << "  workstation:  " << workstation() << std::endl;
	std::cout << "  OS :  " << environment() << std::endl;
	std::cout << "  CPU:  " << hardware() << std::endl;
	os_audio_info();
}



///////////////////////////////////////////////////////////////////////////////
//
//	log_monitor
//
///////////////////////////////////////////////////////////////////////////////
//
//	the system tree monitor shows dynamically changes to the system tree
//
void log_monitor(std::string argv, sdevice_ref device)
{
	uint32 timeout = 0;
	uint32 addr = 0;
	uint32 data = 0;
	uint32 cnt = 0;
	if (argv.size())
	{
		std::size_t delimiter = argv.find(",");
		if (delimiter)
			timeout = pal::time() + from_string<uint32>(argv.substr(0,delimiter))*1000;
		if (delimiter != std::string::npos)
			addr = from_string<uint32>(argv.substr(delimiter+1));
	}
	
	if (!timeout && (s_log  || s_trc))
		std::clog << "system log monitor [logging:" << (s_log?"en":"dis") << "abled, trace:" << (s_trc?"en":"dis") << "abled]" << std::endl;	
	while ((!timeout || timeout>pal::time()) && !s_exit)
	{
		#ifdef WIN32
		MSG msg;
		while(::PeekMessage(&msg,NULL,NULL,NULL,PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		#endif
		if (addr && device)
		{
			uint32 tmp;
			try
			{
				device->ohci_read(addr,&tmp);
//				if (!s_log && !s_trc)
				{
					std::clog << std::hex << "\r" << addr << ": " << tmp;
					if (data != tmp)
					{
						if (cnt)
						{
							std::clog << std::endl;
							cnt = 0;
						}
						data=tmp;
					}
					else
					{
						cnt++;
						std::clog << ":" << std::dec << cnt;
					}
				}
			}
			catch (tcat::exception::base Exception)
			{
				eout(Exception);
			}
		}
		else
		{
			if (timeout && !s_log && !s_trc)
				std::clog << "wait: " << std::fixed << std::setprecision(1) << ((float)(timeout-pal::time()))/1000 << " sec.   \r";
			pal::delay(50);
		}
	}
}



#ifdef WIN32
dpcChk dpc_chk; 
#endif
#ifdef __MACH__
void ctrl_c(int)
{
	std::cout << "ctrl_c" << std::endl << std::flush;
    s_exit = true;
}
#endif






///////////////////////////////////////////////////////////////////////////////
//
//	main
//
///////////////////////////////////////////////////////////////////////////////
//
int main(int argc, char* argv[])
{
#ifdef TCAT_DEBUG
//	std::string exceptions[] = {"*",""};
//	tcat::exception::base::enable (exceptions);
#endif //TCAT_DEBUG
	int ret_val = 0;
	ssystem_ref system;
	unsigned int bus_idx = 0;
	unsigned int device_idx = 0;

	TDL_OpenConnection();
	TDL_ENTRY( kTDLFlag_Info, "TDL_OpenConnection  (DICE tool, version 0x%08X)", kTCAT_DICE_VERSION32, 0, 0, 0 );

	::srand( pal::time() );

#if 0
	std::cout << "----------------------------------------" << std::endl;
	std::cout << "VENDOR_VERSION_STR:  " << sizeof(dice::ioctl::VENDOR_VERSION_STR) << std::endl;
	std::cout << "BUS_CONFIG:  " << sizeof(dice::ioctl::BUS_CONFIG) << std::endl;
	std::cout << "BUS_NOTIFICATION:  " << sizeof(dice::ioctl::BUS_NOTIFICATION) << std::endl;
	std::cout << "DEVICE_CLOCK:  " << sizeof(dice::ioctl::DEVICE_CLOCK) << std::endl;
	std::cout << "DEVICE_NICKNAME:  " << sizeof(dice::ioctl::DEVICE_NICKNAME) << std::endl;
	std::cout << "DEVICE_NOTIFICATIONS:  " << sizeof(dice::DEVICE_NOTIFICATIONS) << std::endl;
#ifdef __MACH__
	std::cout << "DEVICE_NICKNAME_TABLE:  " << sizeof(dice::ioctl::DEVICE_NICKNAME_TABLE) << std::endl;
	std::cout << "DEVICE_MIDI_INFO:  " << sizeof(dice::ioctl::DEVICE_MIDI_INFO) << std::endl;
	std::cout << "DEVICE_MIDI_NOTIFICATION:  " << sizeof(dice::ioctl::DEVICE_MIDI_NOTIFICATION) << std::endl;
	std::cout << "DEVICE_SET_STATE:  " << sizeof(dice::ioctl::DEVICE_SET_STATE) << std::endl;
	std::cout << "DEVICE_DISABLED_TABLE:  " << sizeof(dice::ioctl::DEVICE_DISABLED_TABLE) << std::endl;
#endif
	std::cout << "BUS_ERROR:  " << sizeof(dice::ioctl::BUS_ERROR) << std::endl;
	std::cout << "BUS_STREAMING_PARAMS:  " << sizeof(dice::ioctl::BUS_STREAMING_PARAMS) << std::endl;
	std::cout << "OHCI<1>:  " << sizeof(dice::ioctl::OHCI<1>) << std::endl;
	std::cout << "ASYNC_OHCI<1>:  " << sizeof(dice::ioctl::ASYNC_OHCI<1>) << std::endl;
	std::cout << "----------------------------------------" << std::endl;
#endif

	try
	{
		#ifdef __MACH__
//		::signal (SIGINT, ctrl_c);
		#endif//__MACH__
		#ifdef WIN32
		bool bVerbose = false;
		bool bShowAll = false;
		bool bSearchOnline = false;
		int res;
		dpc_chk.open();
		dpc_chk.setChkTimerDefault(true);
		
		//ML: Need to wait for clear of dummy peak;
		int32 dummy;
		pal::delay(100);
		dpc_chk.getDPCPeak(dummy);
		#endif//WIN32

		if (argc==1)
			help();

		// param parsing
		while (--argc)
		{
			sbus_ref bus;
			sdevice_ref device;
			char cmd = (char)(tolower(*(*++argv)++));
			switch (cmd)
			{
			case 's':
			case 'b':
			case 'd':
				if (!system)
					try
					{
						system = ssystem::static_create<tcat::ssystem>();
					}
					catch (exception::base xptn)
					{
						eout(xptn);
						ret_val = -1;
						continue;
					}
				if (cmd=='s') break;
				{
					// lock system and obtain the bus
					dice::lock lock(system);
					if (system->size()>bus_idx)
						bus = system->at(bus_idx);
					else
						bus = NULL;
				}
				if (!bus) 
				{
					std::cout << "bus #" << bus_idx << " not found on " kTCAT_DICE_DRV_STR << std::endl;
					ret_val = -3;
					continue;
				}
				if (cmd=='b') break;
				{
					// lock the bus and obtain the device
					dice::lock lock(bus);
					if (bus->size()>device_idx)
						device = bus->at(device_idx);
					else
						device = NULL;
				}
				if (!device)
				{
					std::cout << "device #" << device_idx << " not found on " << bus->name() << std::endl;
					ret_val = -4;
					continue;
				}
				break;
			default:
				break;
			}

			switch (cmd)
			{
			case '+':	// enable features
				while (strlen(*argv))
					switch (tolower(*(*argv)++))
					{
					case 'a':	s_audio =	true; break;
					case 'l':	s_log =		true; break;
					case 'm':	s_midi =	true; break;
					case 't':	s_trc =		true; break;
					case 'p':	s_plugin =	true; break;
					case 'q':	s_qut =		true; break;
					default:	help(); break;
					}
				break;
			case '-':
				while (strlen(*argv))
					switch (tolower(*(*argv)++))
					{
					case 'a':	s_audio =	false; break;
					case 'l':	s_log =		false; break;
					case 'm':	s_midi =	false; break;
					case 't':	s_trc =		false; break;
					case 'p':	s_plugin =	false; break;
					case 'q':	s_qut =		false; break;
					default:	help(); break;
					}
				break;
			case '#':	// select
				switch (tolower(*(*argv)++))
				{
				case 'b':	bus_idx = from_string<unsigned int>(*argv); break;
				case 'd':	device_idx = from_string<unsigned int>(*argv); break;
				default:	help(); break;
				}
				break;
			case 'i':	info(); break;
			#ifdef WIN32
			case 'o':
				while (strlen(*argv))
				{
					switch (tolower(*(*argv)++))
					{
					case 'v':	bVerbose = true; break;
					case 'a':	bShowAll = true; break;
					case 's':	bSearchOnline = true; break;
					default:	help(); break;
					}
				}
				ohci_info(bVerbose, bShowAll, bSearchOnline);
				break;

			case 'l':
				switch (tolower(*(*argv)++))
				{
				case 'p':
					{	
						int32 dpc=0;
						if ((res=dpc_chk.getDPCPeak(dpc))==DPC_NO_ERROR) 
						{
							if (!s_qut) std::cout << "get and clear DPC latency peak: " << dpc << std::endl;
						}
						else
						{
							if (!s_qut) std::cout << "get and clear DPC latency peak: N/A" <<  std::endl;
						}
					}
					break;

				case 'i':
					res = dpc_chk.install( NULL, 0 );
					if (res != DPC_NO_ERROR)
					{
						if (!s_qut) std::cout << "Error Installing DPC check: " << res << std::endl;
					}
					break;
				case 'u':
					res = dpc_chk.remove ();
					if (res != DPC_NO_ERROR)
					{
						if (!s_qut) std::cout << "Error Removing DPC check: " << res << std::endl;
					}
					break;
				}
				break;
			#endif
			case 's':	// system
				switch (tolower(*(*argv)++))
				{
				case 'i':	system->cmd_info(); break;
				case 'g':	log_monitor(*argv,device); break;
				default:	help(); break;
				}
				break;
			case 'b':	// bus
				switch (tolower(*(*argv)++))
				{
				case '#':	std::cout << "open audio driver clients: " << bus->clients() << std::endl; break;
				#ifndef TCAT_DICE_AVC
				case 'b':	bus->cmd_iosize (*argv); break;
				case 'c':	bus->cmd_clock (*argv); break;
				case 'd':	bus->cmd_master (*argv); break;
				case 'l':	bus->cmd_add_latency (*argv); break;
				case 'm':	bus->cmd_channel_mask (*argv); break;
				case 'o':	bus->cmd_op_mode (*argv); break;
				case 'e':	if (!s_qut) std::cout << "clear driver streaming error counter" << std::endl; bus->error_clear(); break;
				case 'y':	bus->cmd_sync (*argv); break;
				#endif//TCAT_DICE_AVC
				case 'g':	log_monitor(*argv,device); break;
				case 'i':	bus->cmd_info(); break;
				case 't':	if(std::string("")!=*argv) bus->cmd_streaming (*argv); else help_streaming(); break;
				case 'x':	bus->xcross(from_string<int>(*argv)); break;
				#ifdef WIN32
				case 's':	// speaker
					switch (tolower(*(*argv)++))
					{				
					case 'c':	bus->cmd_speaker_config (*argv); break;
					case 'm':	bus->cmd_speaker_map (*argv); break;
					default: help(); break;
					}
					break;
				#endif//WIN32
				default: help(); break;
				}
				break;
			case 'd':	// device
				switch (tolower(*(*argv)++))
				{
				case 'b':	device->cmd_busy (*argv); break;
				case 'd':	device->cmd_dump (*argv); break;
				case 'e':	device->cmd_clear(); break;
				case 'f':
					switch (tolower(*(*argv)++))
					{
					case 'd':	device->cmd_firmware_download (*argv); break;
					case 'u':	device->cmd_firmware_upload (*argv); break;
					case 'x':	device->cmd_firmware_delete (*argv); break;
					default: help(); break;
					}
					break;
				case 'g':	log_monitor(*argv,device); break;
				case 'i':	device->cmd_info(); break;
				case 'l':	device->cmd_loopback (*argv); break;
				case 'm':	device->cmd_mymode (*argv); break;
				case 'n':	device->cmd_nickname (*argv); break;
				case 'p':	device->cmd_phase(); break;
				case 'r':	if (!s_qut) std::cout << "reseting " << device->nickname() << std::endl; device->firmware_reset(); break;
				case 't':	device->cmd_bnc (*argv); break;
				case 'w':	device->cmd_write (*argv); break;
/*				case 'm': // mute
					if (device)
					{
						uint32 chnl;
						if (strlen(*argv))
						{
							sscanf (*argv,"%d",&chnl);
							std::cout << "muting chnl #" << chnl << std::endl;
							device->firmware_router(0xce000400+chnl*4,0xf0b0+chnl);
							TCAT_EXCEPTION_THROW (device->firmware_router(0xce000400+chnl*4)!=0xf0b0+chnl, xptn_feature);
						}
						else
						{
							std::cout << "muting:";
							for (chnl=0; chnl<32; chnl++)
							{
								device->firmware_router(0xce000400+chnl*4,0xf0b0+chnl);
								TCAT_EXCEPTION_THROW (device->firmware_router(0xce000400+chnl*4)!=0xf0b0+chnl, xptn_feature);
								std::cout << "." << std::flush;
							}
							std::cout << std::endl;
						}
						device->firmware_router(0xce000000,0x1f01);
//						TCAT_EXCEPTION_THROW (device->firmware_router(0xce000000)!=0x1f01, xptn_feature);
					}
					break;
*/
				default: help(); break;
				}
				break;
			default: help(); break;
			}
		}
	}
	catch (tcat::exception::base xptn)
	{
		eout(xptn);
	}

	if (system)
	{
		system->close();
	}

	TDL_ENTRY( kTDLFlag_Info, "TDL_CloseConnection  (DICE tool)", 0, 0, 0, 0 );
	TDL_CloseConnection();

	return ret_val;
}
