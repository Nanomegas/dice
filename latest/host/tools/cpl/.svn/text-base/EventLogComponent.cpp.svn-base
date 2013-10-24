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

#include "EventLogger.h"
#include "juce_TCATLookAndFeel.h"

// The Event Log window
//==============================================================================
juce_ImplementSingleton (EventLogWindow)

EventLogWindow::EventLogWindow()
    : DialogWindow ("Event Log window",
					Colours::white,
                    false)
{
    setContentOwned (elComp = new EventLogComponent(40), false);
	setBackgroundColour(findColour(TCATLookAndFeel::tcatPopupWindowBackgroundColourId));
	setResizable (true, true);
	setVisible(false);

	lookAndFeelChanged();
}

EventLogWindow::~EventLogWindow()
{
	clearSingletonInstance();
}

void EventLogWindow::closeButtonPressed()
{
	setVisible(false);
}

void EventLogWindow::showWindow(bool isVisible)
{
	centreWithSize(540, 590);
	toFront(true);
	setVisible(isVisible);
	elComp->updateText();
}

void EventLogWindow::lookAndFeelChanged()
{
	repaint();

	setBackgroundColour(findColour(TCATLookAndFeel::tcatPopupWindowBackgroundColourId));
}

//==============================================================================
EventLogComponent::EventLogComponent (int maxEntries) :
		m_max_entries (maxEntries),
		m_group (0),
		m_num_entries (0),
		m_text (String::empty)
{
	setOpaque(false);
	addAndMakeVisible (m_group = new GroupComponent (String::empty, "Log entries"));

	addAndMakeVisible (m_log_text = new TextEditor (String::empty));
    m_log_text->setFont (Font (12.0f, Font::plain));
	m_log_text->setCaretVisible(false);
	m_log_text->setReadOnly(false);
	m_log_text->setMultiLine(true);
#ifdef WIN32
	m_log_text->setFont(Font("Lucida Console", 10.0f, Font::plain));
#else
	m_log_text->setFont(Font("Monaco", 13.0f, Font::plain));
#endif
	// register for messages from the event logger
	EventLogger::getInstance()->addActionListener(this);

	lookAndFeelChanged();
}

EventLogComponent::~EventLogComponent()
{
	EventLogger::getInstance()->removeActionListener(this);
	EventLogger::getInstance()->clearSingletonInstance();
	deleteAllChildren();
}

void EventLogComponent::resized()
{
	m_group->setBounds (5, 10, getWidth()-10, getHeight()-14);
	m_log_text->setBounds (18, 30, getWidth()-36, getHeight()-45);
}

void EventLogComponent::lookAndFeelChanged()
{
	if (isVisible())
	{
		repaint();

		m_log_text->setTextToShowWhenEmpty("(no entries yet)", findColour(TCATLookAndFeel::tcatPopupWindowBackgroundColourId)); 
		Colour backgroundColour = findColour(TCATLookAndFeel::tcatListboxBackgroundColourId);
		m_log_text->setColour(TextEditor::textColourId, backgroundColour.contrasting()); //findColour(TextEditor::backgroundColourId).contrasting());
		m_group->setColour(GroupComponent::textColourId, 
							findColour(TCATLookAndFeel::tcatPopupWindowBackgroundColourId).contrasting());
	}
}


void EventLogComponent::textEditorTextChanged (TextEditor& editor)
{
}

/** Called when the user presses the return key. */
void EventLogComponent::textEditorReturnKeyPressed (TextEditor& editor)
{
}

/** Called when the user presses the escape key. */
void EventLogComponent::textEditorEscapeKeyPressed (TextEditor& editor)
{
}

/** Called when the text editor loses focus. */
void EventLogComponent::textEditorFocusLost (TextEditor& editor)
{
}
void EventLogComponent::updateText()
{
	if (EventLogWindow::getInstance()->isVisible())
	{
		m_log_text->setText(m_text, false);
	}
}

void EventLogComponent::actionListenerCallback(const String &message)
{
	m_text.append(message.fromFirstOccurrenceOf("[log event]", false, true), 94);
	m_text.append("\r", 1);
	if (m_num_entries++ > m_max_entries)
	{
		m_text = m_text.fromFirstOccurrenceOf("\r", false, true);
		m_num_entries--;
	}
	updateText();
}

