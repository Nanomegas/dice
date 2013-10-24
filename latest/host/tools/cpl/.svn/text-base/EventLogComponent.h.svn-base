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

#ifndef __EVENT_LOG_COMPONENT__
#define __EVENT_LOG_COMPONENT__

#include "JuceHeader.h"

class EventLogComponent;

class EventLogWindow : 
    public DeletedAtShutdown,
	public DialogWindow
{
public:
    EventLogWindow();
    ~EventLogWindow();

    juce_DeclareSingleton (EventLogWindow, true);

	EventLogComponent * elComp;

	void showWindow(bool isVisible);
	void closeButtonPressed();
	void lookAndFeelChanged();
 };


//==============================================================================
/**
    This component is a view that shows EventLogger Output messages.
*/

class EventLogComponent : 
		public Component,
		public ActionListener
{
public:
    //==============================================================================
    EventLogComponent (int maxEntries);
    ~EventLogComponent();

    //==============================================================================

    void resized();
	void lookAndFeelChanged();

	// TextEditorListener callbacks
	void textEditorTextChanged (TextEditor& editor);
	void textEditorReturnKeyPressed (TextEditor& editor);
	void textEditorEscapeKeyPressed (TextEditor& editor);
    void textEditorFocusLost (TextEditor& editor);

	void updateText();

	// register for action messages in the ctor
    void actionListenerCallback(const String &message);

    //==============================================================================
    juce_UseDebuggingNewOperator

private:	
    //==============================================================================
    int				m_max_entries;
	GroupComponent*	m_group;
	TextEditor *	m_log_text;
	int				m_num_entries;
	String			m_text;
    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    //EventLogComponent (const WdmChMapComponent&);
    //const EventLogComponent& operator= (const WdmChMapComponent&);
};


#endif   // __EVENT_LOG_COMPONENT__
