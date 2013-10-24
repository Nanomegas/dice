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
//	TCAT command line tool
//
#include <IOKit/IOKitLib.h>
#include "dice_streaming.h"
#include "dice.h"

#include <stdio.h>              /* standard input/output routines.    */
#include <unistd.h>             /* access(), R_OK etc.                */



using namespace tcat;



///////////////////////////////////////////////////////////////////////////////
//
//	workstation
//
///////////////////////////////////////////////////////////////////////////////
//
std::string	workstation()
{
	char hostname[256];
	gethostname(hostname,sizeof hostname);
  	return hostname;
}



///////////////////////////////////////////////////////////////////////////////
//
//	hardware
//
///////////////////////////////////////////////////////////////////////////////
//
std::string hardware()
{
	std::string ret = "";
	try
	{
#ifdef __i386__
		mach_port_t master_port;
		__CFDictionary*	properties;
		io_registry_entry_t reg;
		io_iterator_t i;
		TCAT_EXCEPTION_THROW (::IOMasterPort (MACH_PORT_NULL, &master_port), dice::xptn_resource);
		// retrieve hardware name
		TCAT_EXCEPTION_THROW (!(reg=::IORegistryEntryFromPath (master_port, kIODeviceTreePlane ":/")), dice::xptn_resource);
		TCAT_EXCEPTION_THROW (::IORegistryEntryCreateCFProperties(reg, &properties, kCFAllocatorDefault, kNilOptions), dice::xptn_resource);
		ret = (char*)::CFDataGetBytePtr((CFDataRef)::CFDictionaryGetValue(properties, CFSTR("product-name")));
		ret.replace(ret.find(","),1,".");
		::IOObjectRelease(reg);
		// retrieve CPU count & clock frequency
		int cpu_count=0;
		std::string clk;
		TCAT_EXCEPTION_THROW (!(reg=::IORegistryEntryFromPath (master_port, kIODeviceTreePlane ":/cpus")), dice::xptn_resource);
		TCAT_EXCEPTION_THROW (::IORegistryEntryGetChildIterator(reg, kIODeviceTreePlane, &i), dice::xptn_resource);
		::IOObjectRelease(reg);
		while ((reg = IOIteratorNext(i)))
		{
			if (!cpu_count++)
				if (!::IORegistryEntryCreateCFProperties(reg, &properties, kCFAllocatorDefault, kNilOptions))
					clk = to_string(*(unsigned int*)::CFDataGetBytePtr((CFDataRef)::CFDictionaryGetValue(properties, CFSTR("clock-frequency")))/1000000000) + "GHz";
			::IOObjectRelease(reg);
		}
		ret += ": " + to_string(cpu_count) + "x" + clk + ", ";
#else
/* doesn't seem to be available anymore:
		long int mach;
		TCAT_EXCEPTION_THROW (::Gestalt(gestaltMachineType, &mach), dice::xptn_resource);
		switch (mach)
		{
		default:		ret += "machine" + to_string<long int>(mach);
		}
*/
#endif

#if 0
		// no longer available as of the Mac OS X SDK 10.6

		SInt32 cpu;
		TCAT_EXCEPTION_THROW (::Gestalt(gestaltCountOfCPUs, &cpu), dice::xptn_resource);
		ret += to_string(cpu) + "x";
		TCAT_EXCEPTION_THROW (::Gestalt(gestaltNativeCPUfamily, &cpu), dice::xptn_resource);
		switch (cpu)
		{
		// G3:
		case gestaltCPU750:			ret+="G3"; break;
		case gestaltCPU750FX:		ret+="G3 750FX"; break;
		// G4:
		case gestaltCPUG4:			ret+="G4"; break;
		case gestaltCPUG47450:		ret+="G4 7450"; break;
		case gestaltCPUApollo:		ret+="G4 7455"; break;
		case gestaltCPUG47447:		ret+="G4 7447"; break;
		// G5:
		case gestaltCPU970:			ret+="G5 970"; break;
		case gestaltCPU970FX:		ret+="G5 970FX"; break;
		// intel:
		case gestaltCPU486:			ret+="486"; break;
		case gestaltCPUPentium:		ret+="Pentium"; break;
		case gestaltCPUPentiumPro:	ret+="PentiumPro"; break;
		case gestaltCPUPentiumII:	ret+="PentiumII"; break;
		case gestaltCPUPentium4:	ret+="Pentium4"; break;
		case gestaltCPUX86:			ret+="x86"; break;
		default:					ret+="cpu" + to_string(cpu); break;
		}

		SInt32 ram;
		TCAT_EXCEPTION_THROW (::Gestalt(gestaltPhysicalRAMSizeInMegabytes, &ram), dice::xptn_resource);
		ret += ", (" + to_string<uint32>(ram) + "mb ram)";
#endif
	}
	catch(...)
	{
	}
	return ret;
}



///////////////////////////////////////////////////////////////////////////////
//
//	environment
//
///////////////////////////////////////////////////////////////////////////////
//
std::string environment()
{
	std::string ret="";
	try
	{
#if 0
		// no longer available as of the Mac OS X SDK 10.6

		SInt32 major, minor, bugfix;

		TCAT_EXCEPTION_THROW (::Gestalt(gestaltSystemVersionMajor, &major), dice::xptn_resource);
		TCAT_EXCEPTION_THROW (::Gestalt(gestaltSystemVersionMinor, &minor), dice::xptn_resource);
		TCAT_EXCEPTION_THROW (::Gestalt(gestaltSystemVersionBugFix, &bugfix), dice::xptn_resource);
		ret = "Mac OS X " + to_string<uint32>(major) + "." + to_string<uint32>(minor) + "." + to_string<uint32>(bugfix);
#else
		ret = "Mac OS X";
#endif
	}
	catch(...)
	{
	}
	return ret;
}



///////////////////////////////////////////////////////////////////////////////
//
//	environment
//
///////////////////////////////////////////////////////////////////////////////
//
void os_audio_info()
{
	AudioDeviceID device_list[64];
	AudioObjectPropertyAddress address;
	CFStringRef cf_name;
	char c_name[256];
	UInt32 size;

	// retrieve number of coreaudio devices
	address.mSelector = kAudioHardwarePropertyDevices;
	address.mScope = kAudioObjectPropertyScopeGlobal;
	address.mElement = kAudioObjectPropertyElementMaster;
	TCAT_EXCEPTION_THROW(::AudioObjectGetPropertyDataSize( kAudioObjectSystemObject, &address, 0, NULL, &size), xptn_os_audio);
	std::cout << "  coreaudio: (" << size/sizeof(AudioDeviceID) << " devices):" << std::endl;

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
		std::cout << (std::string(c_name)==kTCAT_DICE_USR_AUDIO_STR ?"  * ":"    ") << device_list[device] << ":" << c_name << std::endl;
	}

	// midi
	MIDIEndpointRef endpoint;
	devices=::MIDIGetNumberOfSources();
	std::cout << "  coremidi/in: (" << devices << " devices):" << std::endl;
	for (int device=0; device<devices; device++)
	{
		endpoint = ::MIDIGetSource (device);
		TCAT_EXCEPTION_THROW(::MIDIObjectGetStringProperty (endpoint, kMIDIPropertyName, &cf_name), xptn_os_midi);
		TCAT_EXCEPTION_THROW(!::CFStringGetCString(cf_name, c_name, sizeof(c_name), kCFStringEncodingUTF8), xptn_os_midi);
		::CFRelease(cf_name);
		std::cout << (std::string(c_name)==kTCAT_DICE_USR_MIDI_STR ?"  * ":"    ") << c_name << std::endl;
	}
	devices=::MIDIGetNumberOfDestinations();
	std::cout << "  coremidi/out: (" << devices << " devices):" << std::endl;
	for (int device=0; device<devices; device++)
	{
		endpoint = ::MIDIGetDestination (device);
		TCAT_EXCEPTION_THROW(::MIDIObjectGetStringProperty (endpoint, kMIDIPropertyName, &cf_name), xptn_os_midi);
		TCAT_EXCEPTION_THROW(!::CFStringGetCString(cf_name, c_name, 1024, kCFStringEncodingUTF8), xptn_os_midi);
		::CFRelease(cf_name);
		std::cout << (std::string(c_name)==kTCAT_DICE_USR_MIDI_STR ?"  * ":"    ") << c_name << std::endl;
	}
}