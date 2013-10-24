///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004 - 2009 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
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
#include "tcat_dice_pal.h"
#include "dice_streaming.h"
#include "fwi.h"
#include <windows.h>
#include <asiodrivers.h>

fwi g_fwi;


///////////////////////////////////////////////////////////////////////////////
//
//	workstation
//
///////////////////////////////////////////////////////////////////////////////
//
std::string	workstation()
{
	WCHAR tmp[0x100];
	DWORD size = sizeof tmp;
	memset (tmp, 0, size);
	::GetComputerName (tmp, &size);

	return wstrtostr(tmp);
}



///////////////////////////////////////////////////////////////////////////////
//
//	hardware
//
///////////////////////////////////////////////////////////////////////////////
//

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);

static void getSystemInfo( SYSTEM_INFO *sysInfo )
{
	PGNSI	pGNSI;

	if (!sysInfo)
		return;

	memset( sysInfo, 0, sizeof(SYSTEM_INFO) );

	pGNSI = (PGNSI) GetProcAddress( GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo" );
	if (pGNSI)
	{
		pGNSI( sysInfo );
	}
	else
	{
		::GetSystemInfo( sysInfo );
	}
}



std::string hardware()
{
	static const char * const arch[] = {"INTEL","MIPS","ALPHA","PPC","SHX","ARM","IA64","ALPHA64","MSIL","AMD64","IA32 on 64"};
	SYSTEM_INFO sys;

	getSystemInfo( &sys );

	return tcat::to_string(sys.dwNumberOfProcessors) + " x " + arch[sys.wProcessorArchitecture] + " step:" + tcat::to_string(sys.wProcessorLevel) + " rev:" + tcat::to_string(sys.wProcessorRevision);
}



///////////////////////////////////////////////////////////////////////////////
//
//	environment
//
///////////////////////////////////////////////////////////////////////////////
//
std::string environment()
{
	TCHAR osString[256];

	g_fwi.getOSDisplayString( osString, sizeof(osString) );

	return std::string( wstrtostr( osString ) );
}



///////////////////////////////////////////////////////////////////////////////
//
//	os_audio_info
//
///////////////////////////////////////////////////////////////////////////////
//
void os_audio_info()
{
	std::cout << "  asio:" << std::endl;
	if (AsioDrivers* asio = new AsioDrivers())
		for(long i = 0; i < asio->asioGetNumDev(); i++)
		{
			char tmp[0x100];
			asio->asioGetDriverName(i, tmp, sizeof tmp);
			std::cout << (std::string(tmp).find(kTCAT_DICE_USR_ASIO_STR)!=std::string::npos ?"  * ":"    ") <<
				i << ":" << tmp << std::endl;
		}
	std::cout << "  MM midi.in:" << std::endl;
	for (unsigned int i=0; i<::midiInGetNumDevs(); i++)
	{
		MIDIINCAPS2 io;
		LPOLESTR	guidStr;

		memset( &io, 0, sizeof(io) );
		TCAT_EXCEPTION_THROW (::midiInGetDevCaps(i, (LPMIDIINCAPS)&io, sizeof io), tcat::xptn_os_midi);
		std::string namestr = wstrtostr(io.szPname);
		std::cout << (namestr.find(kTCAT_DICE_USR_MIDI_STR)!=std::string::npos ?"  * ":"    ") <<
			i << ":" << io.szPname << 
			" [ver:" << std::hex << io.vDriverVersion << 
			", mid:" << io.wMid << 
			", pid:" << io.wPid << 
			"]" << std::endl;
		if (::StringFromCLSID( io.ManufacturerGuid, &guidStr ) == S_OK)
		{
			std::wcout << "      [mguid: " << guidStr << "]" << std::endl;
			CoTaskMemFree( guidStr );
		}
		if (::StringFromCLSID( io.ProductGuid, &guidStr ) == S_OK)
		{
			std::wcout << "      [pguid: " << guidStr << "]" << std::endl;
			CoTaskMemFree( guidStr );
		}
	}
	std::cout << "  MM midi.out:" << std::endl;
	for (unsigned int i=0; i<::midiOutGetNumDevs(); i++)
	{
		MIDIOUTCAPS2 io;
		LPOLESTR	guidStr;

		memset( &io, 0, sizeof(io) );
		TCAT_EXCEPTION_THROW (::midiOutGetDevCaps(i, (LPMIDIOUTCAPS)&io, sizeof io), tcat::xptn_os_midi);
		std::string namestr = wstrtostr(io.szPname);
		std::cout << (namestr.find(kTCAT_DICE_USR_MIDI_STR)!=std::string::npos ?"  * ":"    ") <<
			i << ":" << io.szPname << 
			" [ver:" << std::hex << io.vDriverVersion << 
			", mid:" << io.wMid << 
			", pid:" << io.wPid << 
			", channel_mask:" << io.wChannelMask << 
			", notes:" << io.wNotes << 
			", technology:" << io.wTechnology << 
			", voices:" << io.wVoices << 
			"]" << std::endl;
		if (::StringFromCLSID( io.ManufacturerGuid, &guidStr ) == S_OK)
		{
			std::wcout << "      [mguid: " << guidStr << "]" << std::endl;
			CoTaskMemFree( guidStr );
		}
		if (::StringFromCLSID( io.ProductGuid, &guidStr ) == S_OK)
		{
			std::wcout << "      [pguid: " << guidStr << "]" << std::endl;
			CoTaskMemFree( guidStr );
		}
	}
	std::cout << "  MM wave.in:" << std::endl;
	for (unsigned int i=0; i<::waveInGetNumDevs(); i++)
	{
		WAVEINCAPS io;
		TCAT_EXCEPTION_THROW (::waveInGetDevCaps(i, &io, sizeof io), tcat::xptn_os_audio);
		std::string namestr = wstrtostr(io.szPname);
		std::cout << (namestr.find(kTCAT_DICE_USR_AUDIO_STR)!=std::string::npos ?"  * ":"    ") <<
			i << ":" << io.szPname << 
			" [ver:" << std::hex << io.vDriverVersion << 
			", mid:" << io.wMid << 
			", pid:" << io.wPid << 
			", formats:" << io.dwFormats << 
			", channels:" << io.wChannels << 
			"]" << std::endl;
	}
	std::cout << "  MM wave.out:" << std::endl;
	for (unsigned int i=0; i<::waveInGetNumDevs(); i++)
	{
		WAVEOUTCAPS io;
		TCAT_EXCEPTION_THROW (::waveOutGetDevCaps(i, &io, sizeof io), tcat::xptn_os_audio);
		std::string namestr = wstrtostr(io.szPname);
		std::cout << (namestr.find(kTCAT_DICE_USR_AUDIO_STR)!=std::string::npos ?"  * ":"    ") <<
			i << ":" << io.szPname << 
			" [ver:" << std::hex << io.vDriverVersion << 
			", mid:" << io.wMid << 
			", pid:" << io.wPid << 
			", formats:" << io.dwFormats << 
			", channels:" << io.wChannels << 
			", support:" << io.dwSupport << 
			"]" << std::endl;
	}
}

///////////////////////////////////////////////////////////////////////////////
//
//	ohci adapter info
//
///////////////////////////////////////////////////////////////////////////////
//
int	ohci_info(bool bVerbose=false, bool bShowInactive=false, bool bSearchOnline=false)
{
	char tmp[0x100];
	DWORD size = sizeof tmp;
	memset (tmp, 0, size);
	long lResult = 0;
	int index = 0;
	int numFound = 0;
	int nOSMajor = -1;
//	int nOSMinor = -1;
//	int nOSServicePack = -1;
	fwi::OHCI1394_DEV_INFO devInfo = { 0 };

	while ((lResult==FWI_NO_ERROR) || (lResult==FWI_FILTERED))
	{
		lResult = g_fwi.get1394OHCIControllerInfo(index++, 
				bSearchOnline ? fwi::kLookOnline : fwi::kUseBuiltIn, 
				bShowInactive ? fwi::kAllAdapters :  fwi::kActiveAdaptersOnly,
				&devInfo );

		switch (lResult)
		{
		case FWI_ERROR_FILE_NOT_FOUND:
			{
				_tprintf(TEXT("\n\n*** Could not get online data.\nThe online source may not be available, or your computer may not be connected to the Internet."));
				break;
			}
		case FWI_ERROR_BAD_FILE_FORMAT:
			{
				_tprintf(TEXT("\n\n*** The online data source is not available."));
				break;
			}
		case FWI_ACCESS_ERROR:
			{
				_tprintf(TEXT("\n\n*** Could not access the Registry. Make sure to run this program with Administrator privileges."));
				break;
			}
		case FWI_FILTERED:
			break;
		  
		case FWI_NO_ERROR:
			{
				if (devInfo.bValid)
				{
					++numFound;
					if (!bVerbose)
					{
						_tprintf(TEXT("#%d (%s:%s) %s : %s\n"), numFound, devInfo.vendorId, devInfo.deviceId, devInfo.vendorName, devInfo.chipset);
					}
					else
					{
						_tprintf(TEXT("#%d\n"), numFound);
						_tprintf(TEXT("  Vendor : (%s) %s\n"), devInfo.vendorId, devInfo.vendorName);
						_tprintf(TEXT("  Chipset: (%s) %s\n"), devInfo.deviceId, devInfo.chipset);
						_tprintf(TEXT("  Revision: %s\n"), devInfo.revision);

						if (nOSMajor == 5)
						{
							switch (devInfo.sidSpeed)
							{
							case 0: _tprintf(TEXT("  **** Warning speed=S100. 1394 devices will have limited channel count.\n"));break;
							case 1: _tprintf(TEXT("  **** Warning speed=S200. 1394 devices will have limited channel count.\n"));break;
							case 2: _tprintf(TEXT("  Speed: S400 (ok).\n"));break;
							case 3: _tprintf(TEXT("  Speed: S400/S800 (ok).\n"));break;
							default: _tprintf(TEXT("  **** Warning SidSpeed registry entry is missing or illegal.\n    1394 devices may not work, or will have limited channel count.\n"));break;
							}
						}

						if (bShowInactive)
						{
							_tprintf(TEXT("  Status : %s\n"), devInfo.bActive ? TEXT("Active") : TEXT("No longer present, or disabled."));
						}

						_tprintf(TEXT("  Details:\n"));
						_tprintf(TEXT("    Subsysten VendorId: %04x\n"), devInfo.subsysVendorId);
						_tprintf(TEXT("    Subsystem DeviceId: %04x\n"), devInfo.subsysDeviceId);

						if (devInfo.bFound)
						{
							_tprintf(TEXT("    Max # isoch Rx contexts: %s\n"), devInfo.maxRx);
							_tprintf(TEXT("    Max # isoch Tx contexts: %s\n"), devInfo.maxTx);
						}

						_tprintf(TEXT("  Support: "));
						if (devInfo.bFound)
						{
							switch (devInfo.support)
							{
							case FW_INTERFACES_SUPPORT_LEVEL_NONE:
								_tprintf(TEXT("Not compatible with DICE drivers.\n"));
								break;
							case FW_INTERFACES_SUPPORT_LEVEL_FULL: 
								_tprintf(TEXT("Compatible, no known issues.\n"));break;
							case FW_INTERFACES_SUPPORT_LEVEL_LIMITED:
								_tprintf(TEXT("Limited support.\n"));
								break;
							case FW_INTERFACES_SUPPORT_LEVEL_UNKNOWN:	// case fall-through
							default: 
								_tprintf(TEXT("This controller has not been qualified yet.\n"));break;
							}

							if (_tcsnlen( devInfo.notes, NOTES_SIZE ) != 0)
							{
								_tprintf(TEXT("  Notes: %s\n"), devInfo.notes);
							}
						}
						else
						{
							_tprintf(TEXT("No support data available.\n"));
						}

						_tprintf(TEXT("\n"));
					}
				}
				break;
			}
		default:
			break;
		}

	}

	return 0;
}
