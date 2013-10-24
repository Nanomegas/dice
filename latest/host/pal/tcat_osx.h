//==============================================================================
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004 - 2012 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
//==============================================================================
//
//	TCAT osx++ wrapper
//	
//	These classes should make it easier to use the OSX API. The classes are
//	stack objects and take care about balancing opening/creating and closing/
//	releasing of resources. The classes provide typesafe type conversation.
//

#include <IOKit/IOKitLib.h>
#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/firewire/IOFireWireLib.h>
#ifdef TCAT_DICE_AVC
#include <IOKit/avc/IOFireWireAVCLib.h>
#include <IOKit/avc/IOFireWireAVCConsts.h>
#endif


//\cond
///////////////////////////////////////////////////////////////////////////////
//
//	CFNumber
//
///////////////////////////////////////////////////////////////////////////////
//
template<CFNumberType cf_type=kCFNumberSInt32Type,class type=SInt32>
class CFNumber
{
public:
	CFNumber(type number, CFAllocatorRef alocator=kCFAllocatorDefault)	{TCAT_EXCEPTION_THROW(!(cf_number=::CFNumberCreate(alocator,cf_type,&number)), tcat::dice::xptn_internal);}
	~CFNumber()															{::CFRelease(cf_number);}
	operator CFNumberRef()												{return cf_number;}
private:
	CFNumberRef cf_number;
};
//\endcond


//\cond
///////////////////////////////////////////////////////////////////////////////
//
//	PlugInInterface
//
///////////////////////////////////////////////////////////////////////////////
//
#ifdef TCAT_DICE_AVC
typedef IOFireWireAVCLibUnitInterface** IOFireWireAVCLibUnitInterfaceRef; // missing OSX type definition
#endif
class PlugInInterface
{
public:
	PlugInInterface(CFUUIDRef UUID,CFMutableDictionaryRef dictionary): // UUID:kIOFireWireAVCLibUnitTypeID or kIOFireWireLibTypeID
		m_interface( NULL)
	{
		SInt32 score;
		TCAT_EXCEPTION_THROW (
			::IOCreatePlugInInterfaceForService(
				::IOServiceGetMatchingService(kIOMasterPortDefault, dictionary), 
				UUID, kIOCFPlugInInterfaceID, &m_interface, &score), tcat::dice::xptn_internal);
	}
	~PlugInInterface()
	{
		if (m_interface)
		{
			::IODestroyPlugInInterface(m_interface);
			m_interface = NULL;
		}
	}
#ifdef TCAT_DICE_AVC
	operator IOFireWireAVCLibUnitInterfaceRef()
	{
		IOFireWireAVCLibUnitInterfaceRef result;
		TCAT_EXCEPTION_THROW (
			(**m_interface).QueryInterface(m_interface, CFUUIDGetUUIDBytes(kIOFireWireAVCLibUnitInterfaceID_v2), (void**)&result) &&
			(**m_interface).QueryInterface(m_interface, CFUUIDGetUUIDBytes(kIOFireWireAVCLibUnitInterfaceID), (void**)&result), 
			tcat::dice::xptn_internal);
		return result;
	}
#endif
	operator IOFireWireLibDeviceRef()
	{
		IOFireWireLibDeviceRef result;
		TCAT_EXCEPTION_THROW (
			(**m_interface).QueryInterface(m_interface, CFUUIDGetUUIDBytes(kIOFireWireDeviceInterfaceID_v8), (void**)&result), 
			tcat::dice::xptn_internal);
		return result;
	}
private:
	IOCFPlugInInterface** m_interface;
};
//\endcond

