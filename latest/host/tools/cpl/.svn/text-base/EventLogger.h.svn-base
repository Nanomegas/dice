///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2008 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
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

#include "JuceHeader.h"
#include "EventLogComponent.h"

//==============================================================================
/**
    A simple Trampoline pattern implemenation of a Logger that collects log 
	messages and broadcasts them for any registered listeners.
*/
class EventLogger : 
    public DeletedAtShutdown,
	public ActionBroadcaster
{
public:
    //==============================================================================
	EventLogger();
    ~EventLogger();

    //==============================================================================
    juce_DeclareSingleton (EventLogger, true);

	//==============================================================================
    void logMessage (const String& message);

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
	EventLogger (const EventLogger&);
	const EventLogger& operator= (const EventLogger&);

};

