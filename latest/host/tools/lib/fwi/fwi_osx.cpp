///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2012 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#include "fwi.h"
#include "DevsXmlFile.h"
#include "ohci1394db.h"
#include <stdio.h>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <sys/sysctl.h>
#include <CoreServices/CoreServices.h>

// registry plane and name for search
static const char	regPlane[] = kIOServicePlane;
static const char	regName[] = "AppleFWOHCI\0";

static char			infoBuff[OHCITOOL_IOREG_INFO_BUFF_SIZE];
static fwi::OHCI1394_DEV_INFO	current1394Adapter;


fwi::fwi()
{
    getLocalAdapters();
}

fwi::~fwi()
{
	devsXMLUninitialize();
    local1394Adapters.clear();
}

long fwi::getLocalAdapters()
{
	local1394Adapters.clear();

	memset(&infoBuff, 0, sizeof(infoBuff));
	num_matched = 0;
    io_registry_entry_t stackOfObjects[64];
	
	io_registry_entry_t rootEntry = IORegistryGetRootEntry(kIOMasterPortDefault);
	if (rootEntry == MACH_PORT_NULL)
    {
        return -1;
    }

	search(rootEntry, 0, stackOfObjects); // write list of devs into infoBuff

	return local1394Adapters.size();
}

long fwi::getHardwareInfo(TCHAR * pszHW, size_t nSize)
{
    size_t size = 0;
    int mib[4];
    mib[0] = CTL_HW;

    mib[1] = HW_MODEL;
    sysctl(mib, 2, NULL, &size, NULL, 0);
    char *model = new char[size];
    sysctl(mib, 2, model, &size, NULL, 0);
    
    mib[1] = HW_MACHINE;
    sysctl(mib, 2, NULL, &size, NULL, 0);
    char *machine = new char[size];
    sysctl(mib, 2, machine, &size, NULL, 0);
    
    mib[1] = HW_NCPU;
    int num_cpu = 0;
    size = sizeof(int); 
    sysctl(mib, 2, &num_cpu, &size, NULL, 0);        
    if( num_cpu < 1 )
    {
        num_cpu = 1;
    }
    
    uint64_t ramSize = 0;
    size = sizeof(uint64_t);
    mib[1] = HW_MEMSIZE;
    sysctl(mib, 2, &ramSize, &size, NULL, 0);
        
    std::ostringstream hwInfo;
    
    hwInfo << "\nModel: " << model;
    hwInfo << "\nMachine: " << machine;
    hwInfo << "\nNumber of CPU's: " << num_cpu;
    hwInfo << "\nMemory size: " << ramSize/(1024*1024) << " MB\n";
    
    std::string hw = hwInfo.str();
    strcpy ((char *)pszHW, hw.c_str());

	return 0;

/*
	FILE *readPipe;
	
	int f = fcntl(0, F_GETFL, 0);
	f |= O_NONBLOCK;
	fcntl(0, F_SETFL, f);
	
	readPipe = popen("system_profiler -detailLevel mini", "r");
	dup2(0, fileno(stdout));

	size_t bytesReturned = 0;
	size_t bytesTotal = 0;
    
	fflush(stdout);	
	do
	{
		char readBuffer[128];
		bytesReturned = fread(readBuffer, 1, 127, readPipe);
		readBuffer[bytesReturned] = '\0';
		
        printf("%s", readBuffer);

        bytesTotal += bytesReturned;
        if (bytesTotal >= nSize)
        {
            return -1;
        }
		strncat((char *)pszHW, (char *)readBuffer, bytesReturned);
	} while(bytesReturned > 0);
	pclose(readPipe);
*/
    return 0;
}

long fwi::getOSInfo(int * nOSMajor, int * nOSMinor, int * nOSServicePack)
{
    SInt32 majorVersion = 0;
    SInt32 minorVersion = 0;
    SInt32 bugfixVersion = 0;
    
    Gestalt(gestaltSystemVersionMajor, &majorVersion);
    Gestalt(gestaltSystemVersionMinor, &minorVersion);
    Gestalt(gestaltSystemVersionBugFix, &bugfixVersion);

    *nOSMajor = (int)majorVersion;
    *nOSMinor = (int)minorVersion;
    *nOSServicePack = (int)bugfixVersion;

    return 0;
}

bool fwi::getOSDisplayString (TCHAR * pszOS, size_t nSize)
{
    int osMajor = 0;
    int osMinor = 0;
    int osBugfix = 0;
    
    getOSInfo(&osMajor, &osMinor, &osBugfix);
    snprintf((char *)pszOS, nSize, "Mac OS X %d.%d.%d", osMajor, osMinor, osBugfix);

    return true;
}

long fwi::get1394OHCIControllerInfo( int index, 
								fwi::FWI_INFO_METHOD findMethod, 
								fwi::FWI_INFO_FILTER findFilter, 
								fwi::OHCI1394_DEV_INFO *devInfo )
{
	if (index >= (int)local1394Adapters.size())
	{
		return FWI_ERROR_END_OF_FILE;
	}
	
	// initialize devInfo
	memset( devInfo, 0, sizeof(fwi::OHCI1394_DEV_INFO) );
	strncpy ( (char *)devInfo->vendorId, local1394Adapters[index].vendorId, NUM_STR_SIZE );
	strncpy ( (char *)devInfo->deviceId, local1394Adapters[index].deviceId, NUM_STR_SIZE );
	strncpy ( (char *)devInfo->revision, local1394Adapters[index].revision, NUM_STR_SIZE );
	
	strncpy ( (char *)devInfo->vendorName, TEXT("UNKNOWN"), VENDOR_SIZE );
	strncpy( (char *)devInfo->chipset, TEXT("UNKNOWN"), CHIPSET_SIZE );
	strncpy( (char *)devInfo->maxRx, TEXT("UNKNOWN"), NUM_STR_SIZE );
	strncpy( (char *)devInfo->maxTx, TEXT("UNKNOWN"), NUM_STR_SIZE );
	devInfo->support = FW_INTERFACES_SUPPORT_LEVEL_UNKNOWN;
	
	// get info from the online devices file?
	if (fwi::kLookOnline == findMethod)
	{
		return findControllerInfoXML(devInfo);
	}

	// otherwise get info from the compiled struct
	int i = 0;
	while (vendors[i].vendorId)
	{
		if (0 == (strncmp(vendors[i].vendorId, devInfo->vendorId, NUM_STR_SIZE)))
		{
			// even if we don't find the matching chipset, we can provide the manufacturer
			strncpy(devInfo->vendorName, vendors[i].vendorName, VENDOR_SIZE);
			if (0 == (strncmp(vendors[i].deviceId, devInfo->deviceId, NUM_STR_SIZE)))
			{
				devInfo->bFound = TRUE;
				break;
			}
		}
		i++;
	}

	if (devInfo->bFound)
	{
		strncpy(devInfo->vendorName, vendors[i].vendorName, VENDOR_SIZE);
		strncpy(devInfo->chipset, vendors[i].chipset, CHIPSET_SIZE);
		strncpy(devInfo->notes, vendors[i].notes, NOTES_SIZE);
		strncpy(devInfo->maxRx, vendors[i].maxRx, NUM_STR_SIZE);
		strncpy(devInfo->maxTx, vendors[i].maxTx, NUM_STR_SIZE);
		devInfo->maxspeed = vendors[i].maxspeed;
		devInfo->support = vendors[i].support;
	}

	devInfo->bValid = TRUE;

	return FWI_NO_ERROR;
}

// ----------------------------------------------------------------------------
// private functions

void buffPrintf(const char *format, ...)
{
	char buff_str[OHCITOOL_IOREG_INFO_BUFF_SIZE] = { 0 };
	int cur_len = strlen(infoBuff);
	va_list args;
	va_start(args, format);
	vsnprintf(buff_str, sizeof(buff_str), format, args);
	strncat(infoBuff, buff_str, OHCITOOL_IOREG_INFO_BUFF_SIZE-cur_len-1);
}


void printIDProperty(const void * key, const void * value, void * context)
{
	__CFString * CFKey = (__CFString *)key;
	
    // Print out the useful id properties.
	if (   (CFStringCompare( CFKey, CFSTR("vendor-id"), 0) == 0 )
		|| (CFStringCompare( CFKey, CFSTR("device-id"), 0) == 0 )
		|| (CFStringCompare( CFKey, CFSTR("revision-id"), 0) == 0 )
		|| (CFStringCompare( CFKey, CFSTR("subsystem-id"), 0) == 0 )
		|| (CFStringCompare( CFKey, CFSTR("subsystem-vendor-id"), 0 ) == 0) )
	{
		const char * keyStr = CFStringGetCStringPtr(CFKey, kCFStringEncodingMacRoman);		
		if (keyStr)
		{
			buffPrintf("    %s:", keyStr);
		}
        
		buffPrintf(" ");
		CFTypeID type = CFGetTypeID(value);		
		if (type == CFDataGetTypeID())
		{
			const UInt8 * bytes  = CFDataGetBytePtr((CFDataRef)value);
			CFIndex       length = CFDataGetLength((CFDataRef)value);
			CFIndex		  index;
            
			if (length == 4)
			{
				buffPrintf("0x%x", *((const UInt32*)bytes));
				if (CFStringCompare( CFKey, CFSTR("vendor-id"), 0) == 0 )
				{
					snprintf(current1394Adapter.vendorId, NUM_STR_SIZE, "%lx", *((const unsigned long*)bytes));
				}
				else if (CFStringCompare( CFKey, CFSTR("device-id"), 0) == 0 )
				{
					snprintf(current1394Adapter.deviceId, NUM_STR_SIZE, "%lx", *((const unsigned long*)bytes));
				}
				else if (CFStringCompare( CFKey, CFSTR("revision-id"), 0) == 0 )
				{
					snprintf(current1394Adapter.revision, NUM_STR_SIZE, "%lx", *((const unsigned long*)bytes));
				}
				else if (CFStringCompare( CFKey, CFSTR("subsystem-id"), 0) == 0 )
				{
					current1394Adapter.subsysDeviceId = *((const unsigned long*)bytes);
				}
				else if (CFStringCompare( CFKey, CFSTR("subsystem-vendor-id"), 0 ) == 0)
				{
					current1394Adapter.subsysVendorId = *((const unsigned long*)bytes);
				}
			}
			else
			{
				for (index = 0; index < length; index++) 
				{
					buffPrintf("%02x", bytes[index]);
					if (CFStringCompare( CFKey, CFSTR("vendor-id"), 0) == 0 )
					{
						snprintf(current1394Adapter.vendorId, NUM_STR_SIZE, "%02x", bytes[index]);
					}
					else if (CFStringCompare( CFKey, CFSTR("device-id"), 0) == 0 )
					{
						snprintf(current1394Adapter.deviceId, NUM_STR_SIZE, "%02x", bytes[index]);
					}
					else if (CFStringCompare( CFKey, CFSTR("revision-id"), 0) == 0 )
					{
						snprintf(current1394Adapter.revision, NUM_STR_SIZE, "%02x", bytes[index]);
					}
					else if (CFStringCompare( CFKey, CFSTR("subsystem-id"), 0) == 0 )
					{
						current1394Adapter.subsysDeviceId = *((const unsigned long*)bytes);
					}
					else if (CFStringCompare( CFKey, CFSTR("subsystem-vendor-id"), 0 ) == 0)
					{
						current1394Adapter.subsysVendorId = *((const unsigned long*)bytes);
					}
				}
			}
		}
		
		buffPrintf("\n");
	}
}

void printCapProperty(const void * key, const void * value, void * context)
{
	__CFString * CFKey = (__CFString *)key;
	
	// Print out the useful capabilites properties.
	if (   (CFStringCompare( CFKey, CFSTR("IsochReceiveContextCount"), 0) == 0 )
		|| (CFStringCompare( CFKey, CFSTR("IsochTransmitContextCount"), 0) == 0 ) ) 
    {
		const char * keyStr = CFStringGetCStringPtr(CFKey, kCFStringEncodingMacRoman);		
		if (keyStr)
		{
			buffPrintf("    %s:", keyStr);
		}
		
		buffPrintf(" ");
		CFTypeID type = CFGetTypeID(value);		
		if (type == CFNumberGetTypeID())
		{
			long long number;
			
			if (CFNumberGetValue((CFNumberRef)value, kCFNumberLongLongType, &number))
			{
				buffPrintf("0x%qx", (unsigned long long)number);
				if (CFStringCompare( CFKey, CFSTR("IsochReceiveContextCount"), 0) == 0 )
				{
					snprintf(current1394Adapter.maxRx, NUM_STR_SIZE, "%qx", (unsigned long long)number);
				}
				else if (CFStringCompare( CFKey, CFSTR("IsochTransmitContextCount"), 0) == 0 )
				{
					snprintf(current1394Adapter.maxTx, NUM_STR_SIZE, "%qx", (unsigned long long)number);
				}
			}
		}
		
		buffPrintf("\n");
    }
}

void fwi::scan( io_registry_entry_t service,
               Boolean             serviceHasMoreSiblings,
               UInt32              serviceDepth)
{
    io_iterator_t       children    = 0; // (needs release)
    kern_return_t       status      = KERN_SUCCESS;

    // Obtain the service's children.
    status = IORegistryEntryGetChildIterator(service, regPlane, &children);
	if (KERN_SUCCESS == status)
	{	
        io_registry_entry_t childUpNext; // (don't release)

		childUpNext = IOIteratorNext(children);

		// Traverse child entries
		while (childUpNext)
		{
            io_registry_entry_t child; // (needs release)

			child       = childUpNext;
			childUpNext = IOIteratorNext(children);

			scan(child,
				 (childUpNext) ? TRUE : FALSE,
				 serviceDepth + 1);

			IOObjectRelease(child);
		}

		IOObjectRelease(children); children = 0;
	}
}

void fwi::search( io_registry_entry_t service,
                 UInt32              serviceDepth,
                 io_registry_entry_t stackOfObjects[] )
{
    io_registry_entry_t parent		= 0; // (needs release)
    kern_return_t       status      = KERN_SUCCESS;

    // Determine whether the service matches the name
	if (IOObjectConformsTo(service, regName))
    {
		// the ID properties we are looking for are in the parent
		status = IORegistryEntryGetParentEntry(service, regPlane, &parent);
		if (KERN_SUCCESS == status)
		{
            CFMutableDictionaryRef properties = 0; // (needs release)

			status = IORegistryEntryCreateCFProperties( parent,
													   &properties,
													   kCFAllocatorDefault,
													   kNilOptions );
			if (KERN_SUCCESS == status)
			{
				if (CFGetTypeID(properties) == CFDictionaryGetTypeID())
				{
					if (num_matched)
						buffPrintf("\n\n");
					num_matched++;
					buffPrintf("  [OHCI %d]\n", num_matched);
                    
                    memset(&current1394Adapter, 0, sizeof (OHCI1394_DEV_INFO));

					// Print out the service's parent properties (id's).	
					::CFDictionaryApplyFunction(properties, printIDProperty, NULL);
					IOObjectRelease(parent); parent = 0;
				}
				
				CFRelease(properties);
			}
			
			// the isoch capabilities properties are in this entry
			status = IORegistryEntryCreateCFProperties( service,
													   &properties,
													   kCFAllocatorDefault,
													   kNilOptions );
			if (KERN_SUCCESS == status)
			{
				if (CFGetTypeID(properties) == CFDictionaryGetTypeID())
				{
					// Print out the service's properties (capabilities).	
					::CFDictionaryApplyFunction(properties, printCapProperty, NULL);
					IOObjectRelease(parent); parent = 0;
				}
				
				CFRelease(properties);
			}
		}
	
		local1394Adapters.push_back(current1394Adapter);
		
		scan(service, FALSE, serviceDepth);
    }
    else
    {
        io_iterator_t       children    = 0; // (needs release)

		stackOfObjects[serviceDepth] = service;

		// Obtain the service's children.		
        status = IORegistryEntryGetChildIterator(service, regPlane, &children);
		if (KERN_SUCCESS == status)
		{
            io_registry_entry_t childUpNext; // (don't release)

			childUpNext = IOIteratorNext(children);
			
			// Traverse over the children of this service.		
			while (childUpNext)
			{
                io_registry_entry_t child; // (needs release)

				child       = childUpNext;
				childUpNext = IOIteratorNext(children);
				
				search(child,
					   serviceDepth + 1,
					   stackOfObjects);
				
				IOObjectRelease(child);
			}
			
			IOObjectRelease(children); children = 0;
		}
    }
}

