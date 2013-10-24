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

#include "EventLogger.h"

//==============================================================================
juce_ImplementSingleton (EventLogger)

EventLogger::EventLogger ()
{
}

EventLogger::~EventLogger()
{
	clearSingletonInstance();
}

//==============================================================================
void EventLogger::logMessage (const String& message)
{
	if (String::empty != message)
    {
		String eventLog;
		eventLog << "[log event]" << Time::getCurrentTime().toString (false, true) << " : " << message;
		sendActionMessage(eventLog);
	}
}