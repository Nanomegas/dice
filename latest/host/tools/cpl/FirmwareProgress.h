///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2007 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef __FIRMWARE_PROGRESS__
#define __FIRMWARE_PROGRESS__

//==============================================================================
/*
    This is a progress thread component that is used for the various firmware
    management operations.
    
    Create an instance of this object and attach a device to it. Any progress
    events will then arrive in the callback. The progress window does not have
    a cancel button by default, since the calls to the pal firmware functions 
    do all of the necessary timeouts.
    
    example for reading flash:
    
		FWProgressBackgroundThread * fwThread = new FWProgressBackgroundThread( tcat::dice::FL_OP_READ_MEMORY, 
																				localHostFileNameStr, 
																				String::empty, 
																				0,  
																				m_device);
		m_device->attach(fwThread);
		fwThread->runThread();
		delete fwThread;

*/

#include "JuceHeader.h"
#include "events.h"
#include "listener.h"
#include "tcat_dice_firmware_loader.h"

class FWProgressBackgroundThread  : 
	public ThreadWithProgressWindow, 
	public ActionBroadcaster,
	public device_listener
{
public:
    FWProgressBackgroundThread(tcat::dice::FL_OP_ENUM operation_, String file_, String sector_, tcat::uint32 size_, event_device_ref device_, bool bRequiresEntryPoint = false);
    ~FWProgressBackgroundThread();
    
    void run();    
	virtual void firmware_progress(tcat::uint32 percent, tcat::dice::FL_OP_ENUM operation) const;
		
    juce_UseDebuggingNewOperator

private:
	tcat::dice::FL_OP_ENUM	m_operation;			// operation type
	String					m_file;					// local file name
	String					m_sector;				// flash file name
	tcat::uint32			m_size;					// local file size
	event_device_ref		m_device_ref;			// device ref
	bool					m_requires_entrypoint;	// used with create, true if upload is an application file
};

#endif //__FIRMWARE_PROGRESS__