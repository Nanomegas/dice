///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2009-2012 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#pragma once

#if defined (_WIN32)
#include <tchar.h>
#else
#define TCHAR const char
#define TEXT(arg) arg
#include <cstddef>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#endif

#include <vector>

#define NUM_STR_SIZE	9		// string used to contain a 4-digit number, or "Unknown"
#define CHIPSET_SIZE	64
#define VENDOR_SIZE		80
#define NOTES_SIZE		256

#define FW_INTERFACES_SUPPORT_LEVEL_NONE						0			// verified not to work with our drivers
#define FW_INTERFACES_SUPPORT_LEVEL_FULL						1			// verified to fully work with our drivers
#define FW_INTERFACES_SUPPORT_LEVEL_LIMITED						2			// verifed to work with limited support
#define FW_INTERFACES_SUPPORT_LEVEL_UNKNOWN						0xFFFF		// not tested

#define FW_INTERFACES_MAX_SPEED_S100		100
#define FW_INTERFACES_MAX_SPEED_S200		200
#define FW_INTERFACES_MAX_SPEED_S400		400
#define FW_INTERFACES_MAX_SPEED_S800		800

#define FWI_NO_ERROR				0L
#define FWI_ACCESS_ERROR			1L			// registry access denied
#define FWI_ERROR_FILE_NOT_FOUND	2L			// xml file not found or computer not online
#define FWI_ERROR_BAD_FILE_FORMAT	3L			// bad xml format
#define FWI_ERROR_END_OF_FILE		4L			// searched to end of file without a match
#define FWI_FILTERED                5L

typedef struct
{
	TCHAR * vendorId;				// from registry, used for matching in database
	TCHAR * deviceId;				// from registry, used for matching in database
	TCHAR * maxTx;					// from database
	TCHAR * maxRx;					// from database
	TCHAR * vendorName;				// from database
	TCHAR * chipset;				// from database
	int  maxspeed;					// from database
	TCHAR * notes;					// from database
	int support;					// from database
} VENDOR_INFO;

// dictionary print helpers
void buffPrintf(const char *format, ...);
void printIDProperty(const void * key, const void * value, void * context);
void printCapProperty(const void * key, const void * value, void * context);

class fwi
{
public:
	fwi();
	~fwi();

#if defined (WIN32)
	typedef struct
	{
		bool	bValid;						// struct has been processed
		bool	bFound;						// device was found in the known devices data table
		bool	bActive;					// device is present and enabled, from registry
		TCHAR	vendorId[NUM_STR_SIZE];		// pci vendor id, from registry
		TCHAR	deviceId[NUM_STR_SIZE];		// ohci1394 device id, from registry
		TCHAR	revision[NUM_STR_SIZE];		// device revision, from registry
		TCHAR	vendorName[VENDOR_SIZE];	// vendor company name
		TCHAR	chipset[CHIPSET_SIZE];		// chipset string
		TCHAR	notes[NOTES_SIZE];			// any notes about the card (explain greylisted and blacklisted cards)
		int		support;					// DICE driver support level
		
		int		subsysVendorId;			    // subsystem device vendor id, from registry
		int		subsysDeviceId;				// subsystem device id, from registry
		TCHAR	maxTx[NUM_STR_SIZE];		// the chipset's max isochronous transmit contexts capability
		TCHAR	maxRx[NUM_STR_SIZE];		// the chipset's max isochronous receive contexts capability
		int		sidSpeed;					// max speed, from registry, windows xp only
		int		maxspeed;					// maximum 1394 speed capabillity
	} OHCI1394_DEV_INFO;
#else	
	typedef struct
	{
		bool	bValid;						// struct has been processed
		bool	bFound;						// device was found in the known devices data table
		bool	bActive;					// device is present and enabled, from registry
		char	vendorId[NUM_STR_SIZE];		// pci vendor id, from registry
		char	deviceId[NUM_STR_SIZE];		// ohci1394 device id, from registry
		char	revision[NUM_STR_SIZE];		// device revision, from registry
		char	vendorName[VENDOR_SIZE];	// vendor company name
		char	chipset[CHIPSET_SIZE];		// chipset string
		char	notes[NOTES_SIZE];			// any notes about the card (explain greylisted and blacklisted cards)
		int		support;					// DICE driver support level
		
		int		subsysVendorId;			    // subsystem device vendor id, from registry
		int		subsysDeviceId;				// subsystem device id, from registry
		char	maxTx[NUM_STR_SIZE];		// the chipset's max isochronous transmit contexts capability
		char	maxRx[NUM_STR_SIZE];		// the chipset's max isochronous receive contexts capability
		int		sidSpeed;					// max speed, from registry, windows xp only
		int		maxspeed;					// maximum 1394 speed capabillity
	} OHCI1394_DEV_INFO;
#endif // WIN32
	
	enum FWI_INFO_METHOD
	{
		kLookOnline,						// search xml file maintained online at TCAT
		kUseBuiltIn							// search the vendors[] struct
	};

	enum FWI_INFO_DETAIL
	{
		kNormal,							// show basic information
		kDetailed							// show additional information that is often not neede by users
	};

	enum FWI_INFO_FILTER
	{
		kActiveAdaptersOnly,				// filter results to return info about installed and enabled controllers only
		kAllAdapters						// return results for all adapters that are found in the registry
	};

    std::vector<OHCI1394_DEV_INFO>	local1394Adapters;
	
	long getLocalAdapters();													// get 1394 OHCI adapter list from registry
	long getHardwareInfo(TCHAR * pszHW, size_t nSize);							// get info such as installed RAM and number of CPU's
	long getOSInfo( int * nOSMajor, int * nOSMinor, int * nOSServicePack );		// get major/minor/sp of running OS
	bool getOSDisplayString( TCHAR * pszOS, size_t nSize );						// get OS info as string

#if defined (_WIN32)
	bool isXP1394SP2HotFixInstalled();											// check for installed SP2 1394 hotfix
	bool isXP1394SP3HotFixInstalled();											// check for installed SP3 1394 hotfix
	long getDriverFileVersion( TCHAR * filename, TCHAR * pszVer, size_t nSize );
#endif // WIN32

	// OHCI Controller information
	///////////////////////////////////////////////////////////////////////////////
	// look for support information for device
	//
	// Parameters
	//   index						call with increasing value until FWI_ERROR_END_OF_FILE is returned.
	//   findMethod					search compiled vendors[] struct, or online xml file
	//   findFilter					search all OHCI1394 registry entries, or only entries for active controllers
	//   devInfo					must have vendorId and DeviceID fields set for a possible match
	// Returns
	//  FWI_ERROR_FILE_NOT_FOUND	could not find the file online, or the computer
	//                              is not connected to the Internet
	//  FWI_ERROR_BAD_FILE_FORMAT   the xml file is not correctly formatted
	//  FWI_ACCESS_ERROR			registry access error
	//  FWI_NO_ERROR                success, look in devInfo to see results
	//
	long get1394OHCIControllerInfo( int index, fwi::FWI_INFO_METHOD findMethod, fwi::FWI_INFO_FILTER findFilter, fwi::OHCI1394_DEV_INFO *devInfo );

#if defined (__MACH__)
private:
    int num_matched;

    // collect results in static buffer
    #define	OHCITOOL_IOREG_INFO_BUFF_SIZE	2048

    int NSLog(CFStringRef format, ...);
    
    // registry search
    void scan(io_registry_entry_t service,
                          Boolean             serviceHasMoreSiblings,
                          UInt32              serviceDepth);
    void search(io_registry_entry_t service,
                            UInt32              serviceDepth,
                            io_registry_entry_t stackOfObjects[]);
#endif // __MACH__
};

