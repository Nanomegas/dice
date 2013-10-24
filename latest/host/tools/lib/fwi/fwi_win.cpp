///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2009 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "fwi.h"
#include "DevsXmlFile.h"
#include "ohci1394db.h"

#include <intrin.h>

#pragma comment(lib, "User32.lib")
#pragma comment(lib, "Version.lib")

typedef void (WINAPI *PGNSI)(LPSYSTEM_INFO);


fwi::fwi()
{
	CoInitialize(NULL);
	devsXMLInitialize();
}

fwi::~fwi()
{
	devsXMLUninitialize();
	CoUninitialize();
}



static DWORD countSetBitsInProcessorMask( ULONG_PTR bitMask )
{
	DWORD		LSHIFT = sizeof(ULONG_PTR) * 8 - 1;
	DWORD		bitSetCount = 0;
	ULONG_PTR	bitTest = (ULONG_PTR)1 << LSHIFT;    

	for (DWORD i = 0; i <= LSHIFT; ++i)
	{
		bitSetCount += ((bitMask & bitTest) ? 1 : 0);
		bitTest /= 2;
	}

	return bitSetCount;
}



typedef BOOL (WINAPI *LPFN_GLPI)( PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD );


long fwi::getHardwareInfo(TCHAR * pszHW, size_t nSize)
{
	TCHAR		cpubuf[96];
	TCHAR		membuf[32];

	memset(cpubuf, 0, sizeof(cpubuf));
	memset(membuf, 0, sizeof(membuf));

	if (!pszHW || (nSize < (sizeof(cpubuf) + sizeof(membuf))))
	{
		return -1;
	}

	// null-terminate the string since we concatenate strings to it below
	*pszHW = 0;

	// processor info
	{
		LPFN_GLPI	glpi;

		glpi = (LPFN_GLPI) GetProcAddress( GetModuleHandle(TEXT("kernel32")), "GetLogicalProcessorInformation" );
		if (!glpi)
		{
			// GetLogicalProcessorInformation is unsupported on this OS, use GetSystemInfo() instead
			SYSTEM_INFO sysInfo;
			::GetSystemInfo( &sysInfo );
			StringCchPrintf( cpubuf, 96, TEXT("  Number of CPUs: %d\n"), sysInfo.dwNumberOfProcessors);
			StringCchCopy( pszHW, nSize, cpubuf );
		}
		else
		{
			PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
			DWORD		returnLength = 0;
			BOOL		done = FALSE;

			while (!done)
			{
				if (glpi( buffer, &returnLength ) == FALSE) 
				{
					if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) 
					{
						if (buffer) 
							free( buffer );

						buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION) malloc( returnLength );
						if (!buffer)
						{
							// allocation failure
							done = TRUE;
						}
					}
					else
					{
						// error returned
						done = TRUE;
					}
				}
				else
				{
					done = TRUE;
				}
			}

			if (buffer)
			{
				PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = buffer;
				DWORD processorPackageCount = 0;
				DWORD processorCoreCount = 0;
				DWORD logicalProcessorCount = 0;
				DWORD processorL1CacheCount = 0;
				DWORD processorL1CacheSize = 0;
				DWORD processorL2CacheCount = 0;
				DWORD processorL2CacheSize = 0;
				DWORD processorL3CacheCount = 0;
				DWORD processorL3CacheSize = 0;
				DWORD byteOffset = 0;

				while (byteOffset + sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION) <= returnLength) 
				{
					switch (ptr->Relationship) 
					{
						case RelationNumaNode:
							// Non-NUMA systems report a single record of this type.
							break;

						case RelationProcessorCore:
							processorCoreCount++;

							// A hyperthreaded core supplies more than one logical processor.
							logicalProcessorCount += countSetBitsInProcessorMask( ptr->ProcessorMask );
							break;

						case RelationCache:
							// Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache.
							{
								PCACHE_DESCRIPTOR	Cache = &ptr->Cache;

								if (Cache->Level == 1)
								{
									processorL1CacheCount++;
									if (!processorL1CacheSize) { processorL1CacheSize = Cache->Size; }
								}
								else if (Cache->Level == 2)
								{
									processorL2CacheCount++;
									if (!processorL2CacheSize) { processorL2CacheSize = Cache->Size; }
								}
								else if (Cache->Level == 3)
								{
									processorL3CacheCount++;
									if (!processorL3CacheSize) { processorL3CacheSize = Cache->Size; }
								}
							}
							break;

						case RelationProcessorPackage:
							// Logical processors share a physical package.
							processorPackageCount++;
							break;

						default:
							// unsupported LOGICAL_PROCESSOR_RELATIONSHIP value
							break;
					}

					byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
					ptr++;
				}

				StringCchPrintf( cpubuf, 96, TEXT("  CPU%s:\n"), (processorPackageCount ? TEXT("(s)") : TEXT("")) );
				StringCchCat( pszHW, nSize, cpubuf );

				StringCchCat( pszHW, nSize, TEXT("    Processor:  ") );
				{
					HKEY		procZeroKey;
					BOOL		procNameFound = false;

					if (RegOpenKeyEx( HKEY_LOCAL_MACHINE, TEXT("HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"), 0, KEY_READ, &procZeroKey ) == ERROR_SUCCESS)
					{
						TCHAR		processorNameStr[256];
						DWORD		dataSizeBytes = sizeof(processorNameStr);

						if (RegQueryValueEx( procZeroKey, TEXT("ProcessorNameString"), NULL, NULL, (BYTE *)processorNameStr, &dataSizeBytes) == ERROR_SUCCESS)
						{
							procNameFound = true;
							StringCchCat( pszHW, nSize, processorNameStr );
						}
						RegCloseKey( procZeroKey );
					}

					if (!procNameFound)
					{
						StringCchCat( pszHW, nSize, TEXT("Unknown") );
					}
				}
				StringCchCat( pszHW, nSize, TEXT("\n") );

				StringCchPrintf( cpubuf, 96, TEXT("    Physical:  %d\n"), processorPackageCount );
				StringCchCat( pszHW, nSize, cpubuf );

				StringCchPrintf( cpubuf, 96, TEXT("    Cores:     %d\n"), processorCoreCount );
				StringCchCat( pszHW, nSize, cpubuf );

				StringCchPrintf( cpubuf, 96, TEXT("    Logical:   %d\n"), logicalProcessorCount );
				StringCchCat( pszHW, nSize, cpubuf );

				if (processorL1CacheCount)
				{
					StringCchPrintf( cpubuf, 96, TEXT("    L1 Cache:  %d KB   (%d)\n"), (processorL1CacheSize / 1024), processorL1CacheCount );
					StringCchCat( pszHW, nSize, cpubuf );
				}

				if (processorL2CacheCount)
				{
					StringCchPrintf( cpubuf, 96, TEXT("    L2 Cache:  %d KB  (%d)\n"), (processorL2CacheSize / 1024), processorL2CacheCount );
					StringCchCat( pszHW, nSize, cpubuf );
				}

				if (processorL3CacheCount)
				{
					StringCchPrintf( cpubuf, 96, TEXT("    L3 Cache: %d KB  (%d)\n"), (processorL3CacheSize / 1024), processorL3CacheCount );
					StringCchCat( pszHW, nSize, cpubuf );
				}

				free( buffer );
			}
		}
	}

	// installed RAM
	MEMORYSTATUSEX memStatus;
	ZeroMemory(&memStatus, sizeof(memStatus));
	memStatus.dwLength = sizeof(memStatus);
	::GlobalMemoryStatusEx(&memStatus);

	StringCchPrintf( membuf, 32, TEXT("  Installed RAM: %d MB\n"), (memStatus.ullTotalPhys / (DWORDLONG)(1024 * 1024)) );
	StringCchCat(pszHW, nSize, membuf);

	return 0;
}

long fwi::getOSInfo(int * nOSMajor, int * nOSMinor, int * nOSServicePack)
{
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if ( !(GetVersionEx ((OSVERSIONINFO *) &osvi)) )
		return 1;

	if ( VER_PLATFORM_WIN32_NT==osvi.dwPlatformId && osvi.dwMajorVersion > 4 )
	{
		*nOSMajor = osvi.dwMajorVersion;
		*nOSMinor = osvi.dwMinorVersion;
		*nOSServicePack = osvi.wServicePackMajor;
		return 0;
	}
	return 1;
}

bool fwi::getOSDisplayString (TCHAR * pszOS, size_t nSize)
{
	OSVERSIONINFOEX osvi;
	SYSTEM_INFO si;
	PGNSI pGNSI;
	BOOL bOsVersionInfoEx;

	ZeroMemory(&si, sizeof(SYSTEM_INFO));
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if ( !(bOsVersionInfoEx = GetVersionEx ((OSVERSIONINFO *) &osvi)) )
		return FALSE;

	// Call GetNativeSystemInfo if supported or GetSystemInfo otherwise.
	pGNSI = (PGNSI) GetProcAddress( GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
	if (NULL != pGNSI)
	{
		pGNSI(&si);
	}
	else
	{
		GetSystemInfo(&si);
	}

	if ( VER_PLATFORM_WIN32_NT==osvi.dwPlatformId && osvi.dwMajorVersion > 4 )
	{
		StringCchCopy(pszOS, nSize, TEXT("Microsoft "));

		// Test for the specific product.

		if ((osvi.dwMajorVersion > 6) || ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion > 2 ))
		{
			StringCchCat(pszOS, nSize, TEXT("Windows Version unknown " ));
		}
		else if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 2 )
		{
			if ( osvi.wProductType == VER_NT_WORKSTATION )
			{
				StringCchCat(pszOS, nSize, TEXT("Windows 8"));
			}
			else
			{
				StringCchCat(pszOS, nSize, TEXT("Unknown Windows Product Type" ));
			}

			if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
			{
				StringCchCat(pszOS, nSize, TEXT( " 64-bit" ));
			}
			else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL )
			{
				StringCchCat(pszOS, nSize, TEXT(" 32-bit"));
			}
		}
		else if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 1 )
		{
			if ( osvi.wProductType == VER_NT_WORKSTATION )
			{
				StringCchCat(pszOS, nSize, TEXT("Windows 7"));
			}
			else
			{
				StringCchCat(pszOS, nSize, TEXT("Windows Server 2008 R2" ));
			}

			if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
			{
				StringCchCat(pszOS, nSize, TEXT( " 64-bit" ));
			}
			else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL )
			{
				StringCchCat(pszOS, nSize, TEXT(" 32-bit"));
			}
		}
		else if ( osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0 )
		{
			if ( osvi.wProductType == VER_NT_WORKSTATION )
			{
				StringCchCat(pszOS, nSize, TEXT("Windows Vista"));
			}
			else
			{
				StringCchCat(pszOS, nSize, TEXT("Windows Server 2008" ));
			}

			if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
			{
				StringCchCat(pszOS, nSize, TEXT( " 64-bit" ));
			}
			else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL )
			{
				StringCchCat(pszOS, nSize, TEXT(" 32-bit"));
			}
		}
		else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 2 )
		{
			if ( GetSystemMetrics(SM_SERVERR2) )
			{
				StringCchCat(pszOS, nSize, TEXT( "Windows Server 2003 R2, "));
			}
			else if ( osvi.wSuiteMask==VER_SUITE_STORAGE_SERVER )
			{
				StringCchCat(pszOS, nSize, TEXT( "Windows Storage Server 2003"));
			}
			else if ( osvi.wProductType == VER_NT_WORKSTATION && si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64)
			{
				StringCchCat(pszOS, nSize, TEXT( "Windows XP Professional x64 Edition"));
			}
			else
			{
				StringCchCat(pszOS, nSize, TEXT("Windows Server 2003, "));
			}

			// Test for the server type.
			if ( osvi.wProductType != VER_NT_WORKSTATION )
			{
				if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_IA64 )
				{
					if ( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					{
						StringCchCat(pszOS, nSize, TEXT( "Datacenter Edition for Itanium-based Systems" ));
					}
					else if ( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					{
						StringCchCat(pszOS, nSize, TEXT( "Enterprise Edition for Itanium-based Systems" ));
					}
				}

				else if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
				{
					if ( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					{
						StringCchCat(pszOS, nSize, TEXT( "Datacenter x64 Edition" ));
					}
					else if ( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					{
						StringCchCat(pszOS, nSize, TEXT( "Enterprise x64 Edition" ));
					}
					else 
					{
						StringCchCat(pszOS, nSize, TEXT( "Standard x64 Edition" ));
					}
				}

				else
				{
					if ( osvi.wSuiteMask & VER_SUITE_COMPUTE_SERVER )
					{
						StringCchCat(pszOS, nSize, TEXT( "Compute Cluster Edition" ));
					}
					else if ( osvi.wSuiteMask & VER_SUITE_DATACENTER )
					{
						StringCchCat(pszOS, nSize, TEXT( "Datacenter Edition" ));
					}
					else if ( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
					{
						StringCchCat(pszOS, nSize, TEXT( "Enterprise Edition" ));
					}
					else if ( osvi.wSuiteMask & VER_SUITE_BLADE )
					{
						StringCchCat(pszOS, nSize, TEXT( "Web Edition" ));
					}
					else 
					{
						StringCchCat(pszOS, nSize, TEXT( "Standard Edition" ));
					}
				}
			}
		}
		else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 1 )
		{
			StringCchCat(pszOS, nSize, TEXT("Windows XP"));

			if ( si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64 )
			{
				StringCchCat(pszOS, nSize, TEXT( " 64-bit" ));
			}
			else if (si.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL )
			{
				StringCchCat(pszOS, nSize, TEXT(" 32-bit"));
			}

			if ( osvi.wSuiteMask & VER_SUITE_PERSONAL )
			{
				StringCchCat(pszOS, nSize, TEXT( " Home Edition" ));
			}
			else 
			{
				StringCchCat(pszOS, nSize, TEXT( " Professional" ));
			}
		}
		else if ( osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0 )
		{
			StringCchCat(pszOS, nSize, TEXT("Windows 2000"));

			if ( osvi.wProductType == VER_NT_WORKSTATION )
			{
				StringCchCat(pszOS, nSize, TEXT( " Professional" ));
			}
			else 
			{
				if ( osvi.wSuiteMask & VER_SUITE_DATACENTER )
				{
					StringCchCat(pszOS, nSize, TEXT( " Datacenter Server" ));
				}
				else if ( osvi.wSuiteMask & VER_SUITE_ENTERPRISE )
				{
					StringCchCat(pszOS, nSize, TEXT( " Advanced Server" ));
				}
				else 
				{
					StringCchCat(pszOS, nSize, TEXT( " Server" ));
				}
			}
		}

		// Include service pack (if any)
		if ( _tcslen(osvi.szCSDVersion) > 0 )
		{
			StringCchCat(pszOS, nSize, TEXT(" ") );
			StringCchCat(pszOS, nSize, osvi.szCSDVersion);
		}

		// Include build number
		{
			TCHAR spbuf[80];
			memset(spbuf, 0, sizeof(spbuf));
			StringCchPrintf( spbuf, 80, TEXT(" [%d.%d.%d]"), osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber);
			StringCchCat(pszOS, nSize, spbuf);
		}

		return TRUE; 
	}
	else
	{  
		return FALSE;
	}
}

// since it applies only to windows xp, just use registry lookup of known key
bool fwi::isXP1394SP2HotFixInstalled ()
{
	HKEY regKey=NULL;
	bool bResult=FALSE;

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\App Management\\ARPCache\\KB885222"),
		0,KEY_QUERY_VALUE,&regKey))
	{
		bResult = true;
		RegCloseKey(regKey);
	}
	return bResult;
}

bool fwi::isXP1394SP3HotFixInstalled ()
{
	HKEY regKey=NULL;
	bool bResult=FALSE;

	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		TEXT("SOFTWARE\\Microsoft\\Updates\\Windows XP\\SP4\\KB955408"),
		0,KEY_QUERY_VALUE,&regKey))
	{
		bResult = true;
		RegCloseKey(regKey);
	}
	return bResult;
}

long fwi::get1394OHCIControllerInfo( int index, 
								fwi::FWI_INFO_METHOD findMethod, 
								fwi::FWI_INFO_FILTER findFilter, 
								fwi::OHCI1394_DEV_INFO *devInfo )
{
	HKEY pciKey=NULL;
	DWORD pciIdx=0;
	DWORD retCode;
	DWORD keyLen=256;
	TCHAR pciName[256];

	TCHAR pData[256] = { 0 };
	bool bActive = false;

	union U_SIDSPEED
	{
		char pSidSpeed[4];
		int sidSpeed;
	} uSidSpeed;

	bool bFound = false;
	int foundIndex = 0;

	// os version
	OSVERSIONINFOEX osvi;
	ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));

	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	if ( !(GetVersionEx ((OSVERSIONINFO *) &osvi)) )
		return 1;

	// initialize devInfo
	memset( devInfo, 0, sizeof(fwi::OHCI1394_DEV_INFO) );
	wcsncpy_s( devInfo->vendorName, VENDOR_SIZE, TEXT("Unknown"), VENDOR_SIZE );
	wcsncpy_s( devInfo->chipset, CHIPSET_SIZE, TEXT("Unknown"), CHIPSET_SIZE );
	wcsncpy_s( devInfo->maxRx, NUM_STR_SIZE, TEXT("Unknown"), NUM_STR_SIZE );
	wcsncpy_s( devInfo->maxTx, NUM_STR_SIZE, TEXT("Unknown"), NUM_STR_SIZE );
	devInfo->support = FW_INTERFACES_SUPPORT_LEVEL_UNKNOWN;

	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,TEXT("SYSTEM\\CurrentControlSet\\Enum\\PCI"),0,KEY_READ,&pciKey)!=ERROR_SUCCESS)
	{
//		_tprintf(TEXT("Unable to open registry key, probably access error\n"));
		return FWI_ACCESS_ERROR;
	}
	//enumerate the outer PCI keys
	while (1)
	{
		keyLen=256;
		retCode = RegEnumKeyEx(pciKey,pciIdx++,pciName,&keyLen,NULL,NULL,NULL,NULL);
		if (retCode!=ERROR_SUCCESS) break;
		//we got a PCI key, lets open it and enumerate the instances
		HKEY instKey=NULL;
		DWORD instIdx=0;
		TCHAR instName[256];
		if (RegOpenKeyEx(pciKey,pciName,0,KEY_READ,&instKey)!=ERROR_SUCCESS) break;
		//and enumerate that
		while (1)
		{
			keyLen=256;
			retCode = RegEnumKeyEx(instKey,instIdx++,instName,&keyLen,NULL,NULL,NULL,NULL);
			if (retCode!=ERROR_SUCCESS) break;
			//need to open the key to get information

			HKEY devKey;
			DWORD type;
			TCHAR keyToMatch[32];
			TCHAR classNameToMatch[32];

			if (    (osvi.dwMajorVersion < 6)
			     || (osvi.dwMajorVersion == 6 && osvi.dwMinorVersion == 0)
			   )
			{
				// Windows Vista and earlier
				StringCchCopy(keyToMatch, 6, TEXT("Class"));
				StringCchCopy(classNameToMatch, 5, TEXT("1394"));
			}
			else
			{
				// Windows 7 and later
				StringCchCopy(keyToMatch, 8, TEXT("Service"));
				StringCchCopy(classNameToMatch, 9, TEXT("1394ohci"));
			}

			if (RegOpenKeyEx(instKey,instName,0,KEY_READ,&devKey)==ERROR_SUCCESS)
			{
				//lets get the class
				TCHAR className[256];
				keyLen = 256;
				retCode = RegQueryValueEx(devKey,keyToMatch,NULL,&type,(BYTE *)className,&keyLen);

				if ((retCode==ERROR_SUCCESS) && (_tcscmp((TCHAR*)className,classNameToMatch)==0))
				{
					if (foundIndex++ == index)
					{
						//lets get the hardware ID
						retCode = RegQueryValueEx(devKey,TEXT("HardwareID"),NULL,&type,NULL,&keyLen);
						if (retCode==ERROR_SUCCESS)
						{
							retCode = RegQueryValueEx(devKey,TEXT("HardwareID"),NULL,&type,(BYTE *)pData,&keyLen);
							if (retCode==ERROR_SUCCESS)
							{
								//lets try to get some more info, we need to know if it is running
								bActive=false;
								HKEY otherKey;
								if (RegOpenKeyEx(devKey,TEXT("Control"),0,KEY_READ,&otherKey)==ERROR_SUCCESS)
								{
									//okay, we have a control key.
									//does it have an active service?
									if (RegQueryValueEx(otherKey,TEXT("AllocConfig"),NULL,&type,NULL,&keyLen)==ERROR_SUCCESS)
									{
										bActive=true;
									}
									RegCloseKey(otherKey);								
								}
								devInfo->sidSpeed = -1;
								if (RegOpenKeyEx(devKey,TEXT("Device Parameters"),0,KEY_READ,&otherKey)==ERROR_SUCCESS)
								{
									//okay, we have a Device Parameters key.
									//does it have SidSpeed?
									if (RegQueryValueEx(otherKey,TEXT("SidSpeed"),NULL,&type,NULL,&keyLen)==ERROR_SUCCESS)
									{
										if ((type==REG_DWORD) && (keyLen==sizeof(DWORD)))
										{											
											RegQueryValueEx(otherKey,TEXT("SidSpeed"),NULL,&type,(LPBYTE)uSidSpeed.pSidSpeed,&keyLen);
											devInfo->sidSpeed = uSidSpeed.sidSpeed;
										}
									}
									RegCloseKey(otherKey);								
								}
							}
						}
						RegCloseKey(devKey);
						bFound = true;
						break;
					}
				}
			}
		}
		RegCloseKey(instKey);
		if (bFound) break;
	}
	RegCloseKey(pciKey);

	// now we can parse the registry string that corresponds to the index
	if (bFound)
	{
		if ((fwi::kActiveAdaptersOnly == findFilter) && !bActive)
		{
			return FWI_FILTERED;
		}

		TCHAR * token,*vendor,*dev,*rev, *subsys;
		TCHAR * next_pos;
		unsigned int	subsysInt;

		//parse the string	
		token = _tcstok_s(pData,TEXT("\\"), &next_pos); //This first token is always "PCI" should check
		if (!token) goto formatProblem;
		token = _tcstok_s(NULL,TEXT("_"), &next_pos); //this is the VEN_ part of the vendor ID
		if (!token) goto formatProblem;
		vendor = _tcstok_s(NULL,TEXT("&"), &next_pos); //got the vendor ID
		if (!vendor) goto formatProblem;
		token = _tcstok_s(NULL,TEXT("_"), &next_pos); //this is the DEV_ part of the device ID
		if (!token) goto formatProblem;
		dev = _tcstok_s(NULL,TEXT("&"), &next_pos); //this is the device ID
		if (!dev) goto formatProblem;
		token = _tcstok_s(NULL,TEXT("_"), &next_pos); //this is the SUBSYS_ part of the subsystem
		if (!token) goto formatProblem;
		subsys = _tcstok_s(NULL,TEXT("&"), &next_pos); //this is the subsystem
		if (!subsys) goto formatProblem;
		token = _tcstok_s(NULL,TEXT("_"), &next_pos); //this is the REV_ part of the revision
		if (!token) goto formatProblem;
		rev = _tcstok_s(NULL,TEXT("&"), &next_pos); //this is the revision
		if (!rev) goto formatProblem;

		wcsncpy_s(devInfo->vendorId, vendor, NUM_STR_SIZE);
		wcsncpy_s(devInfo->deviceId, dev, NUM_STR_SIZE);
		wcsncpy_s(devInfo->revision, rev, NUM_STR_SIZE);
		swscanf_s(subsys, TEXT("%08x"), &subsysInt);
		devInfo->subsysVendorId = (subsysInt & 0xffff);
		devInfo->subsysDeviceId = ((subsysInt >> 16) & 0xffff);
		devInfo->bActive = bActive;

		// get info from the online devices file?
		if (fwi::kLookOnline == findMethod)
		{
			return findControllerInfoXML(devInfo);
		}

		// otherwise get info from the compiled struct
		int i = 0;
		while (vendors[i].vendorId)
		{
			if (0 == (_tcsicmp(vendors[i].vendorId, devInfo->vendorId)))
			{
				// even if we don't find the matching chipset, we can provide the manufacturer
				wcsncpy_s(devInfo->vendorName, VENDOR_SIZE, vendors[i].vendorName, VENDOR_SIZE);

				if (0 == (_tcsicmp(vendors[i].deviceId, devInfo->deviceId)))
				{
					devInfo->bFound = TRUE;
					break;
				}
			}
			i++;
		}

		if (devInfo->bFound)
		{
			wcsncpy_s(devInfo->vendorName, VENDOR_SIZE, vendors[i].vendorName, VENDOR_SIZE);
			wcsncpy_s(devInfo->chipset, CHIPSET_SIZE, vendors[i].chipset, CHIPSET_SIZE);
			wcsncpy_s(devInfo->notes, NOTES_SIZE, vendors[i].notes, NOTES_SIZE);
			wcsncpy_s(devInfo->maxRx, NUM_STR_SIZE, vendors[i].maxRx, NUM_STR_SIZE);
			wcsncpy_s(devInfo->maxTx, NUM_STR_SIZE, vendors[i].maxTx, NUM_STR_SIZE);
			devInfo->maxspeed = vendors[i].maxspeed;
			devInfo->support = vendors[i].support;
		}
	}
	else
	{
		return FWI_ERROR_END_OF_FILE;
	}

	devInfo->bValid = TRUE;

	return FWI_NO_ERROR;

formatProblem:
	devInfo->bValid = FALSE;
	return FWI_ERROR_BAD_FILE_FORMAT;
}

// get version info for files in <system_dir>\drivers
long fwi::getDriverFileVersion( TCHAR * filename, TCHAR * pszVer, size_t nSize )
{
	DWORD	zero  = 0;
	DWORD	dwLen = 0;
	LPVOID	buffer = 0;
	TCHAR driverPath[1024];
 
	if (nSize < 32)
	{
		return 0;
	}

	GetSystemDirectory((LPTSTR)driverPath, sizeof(driverPath));

	StringCchCat( (LPTSTR)driverPath, sizeof(driverPath), TEXT("\\drivers\\") );
	StringCchCat( (LPTSTR)driverPath, sizeof(driverPath), filename );

	dwLen = ::GetFileVersionInfoSize( (LPTSTR)driverPath, &zero );

	if( dwLen > 0 )
	{
		LPVOID version[ 16384 ];

		if( 0 != ::GetFileVersionInfo( (LPTSTR) driverPath, 0, dwLen, version  ) )
		{
			UINT len = (UINT)nSize/2;   

			if( 0 != ::VerQueryValue( version, (LPTSTR) TEXT("\\"), &buffer, &len ) )	// get root info
			{
				VS_FIXEDFILEINFO * pffInfo = (VS_FIXEDFILEINFO *)buffer;
				DWORD dwVerMajor = HIWORD(pffInfo->dwFileVersionMS);
				DWORD dwVerMinor = LOWORD(pffInfo->dwFileVersionMS);
				DWORD dwBuild = HIWORD(pffInfo->dwFileVersionLS);
				DWORD dwQFE = LOWORD(pffInfo->dwFileVersionLS);
				StringCchPrintf( pszVer, nSize/2, TEXT("%d.%d.%d.%d"), dwVerMajor, dwVerMinor, dwBuild, dwQFE);
				return 1;
			}
		}
	}
	return 0;
}