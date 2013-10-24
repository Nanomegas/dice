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

#include "JuceHeader.h"
#include "Listener.h"
#include "BusWindow.h"
#include "BusWdmChMapComponent.h"
#include "juce_TCATLookAndFeel.h"
#include "appDefaults.h"
#include "fwi.h"

// The WDM Channel Mapper dialog
//==============================================================================
class WdmChMapWindow;

static WdmChMapWindow* chMapWindow = 0;
//==============================================================================
class WdmChMapWindow : public DialogWindow
{
    WdmChMapComponent*	chMapperComp;
	
public:
    WdmChMapWindow(event_bus_ref& busref)
        : DialogWindow ("WDM Channel Mapper",
						Colours::white,
                        false)
    {
        setContentOwned (chMapperComp = new WdmChMapComponent(), false);
		setBackgroundColour(findColour(TCATLookAndFeel::tcatPopupWindowBackgroundColourId));
        if (busref) busref->attach(chMapperComp);
        chMapperComp->updateContent();
        setResizable (true, true);
    }

    ~WdmChMapWindow()
    {
        chMapWindow = 0;
    }

	void lookAndFeelChanged()
	{
		repaint();

		setBackgroundColour(findColour(TCATLookAndFeel::tcatPopupWindowBackgroundColourId));
	}

	void updateContent()
	{
		chMapperComp->updateContent();
	}
	
    void closeButtonPressed()
    {
        delete this;
    }
 };

// The WDM tab
BusWdmComponent::BusWdmComponent () :
		m_outspkconfig (0),
		m_set_ch_maps (0),
		m_constrainer (0),
		m_num_clients (0),
		m_outspeaker ( 0xDEADDEAD ),
		m_allow_wdm_sr_changes (false),
		m_allow_wdm_sr_changes_settable (true),
		m_update (0)
{
	setName ("Bus WDM");

    addAndMakeVisible (m_outspkconfig = new Label(String::empty, String::empty));
    m_outspkconfig->setJustificationType (Justification::centred);
	m_outspkconfig->setTooltip("The speaker setup is set in Windows 'Sounds and Audio Devices Properties'");

    addAndMakeVisible (m_outspklabel = new Label ("OutSpkConfigLabel",
		"Speaker Setup:"));
    m_outspklabel->setFont (Font (15.0000f, Font::plain));
    m_outspklabel->setJustificationType (Justification::centredRight);
    m_outspklabel->setEditable (false, false, false);

    addAndMakeVisible (m_set_ch_maps = new TextButton ("ChMapsButton"));
    m_set_ch_maps->setButtonText ("Set WDM Channel Maps");
    m_set_ch_maps->addListener (this);
    
    m_constrainer = new ComponentBoundsConstrainer();
    m_constrainer->setSizeLimits(600, 500, 800, 810);

	lookAndFeelChanged();
}
	
BusWdmComponent::~BusWdmComponent() 
{
    deleteAllChildren();
    if (chMapWindow) delete chMapWindow;
    if (m_constrainer) delete m_constrainer;
}

void BusWdmComponent::resized()
{
	int offset = 0;

	m_outspkconfig->setBounds (210, 35+offset, 130, 24);
	m_outspklabel->setBounds (30, 35+offset, 170, 24);
	m_set_ch_maps->setBounds (430, 35+offset, 160, 24);
}

void BusWdmComponent::broughtToFront()
{
	lookAndFeelChanged();
}

void BusWdmComponent::lookAndFeelChanged()
{
	Colour backgroundColour = findColour(TCATLookAndFeel::tcatGlobalTabAreaColourId);

	repaint();

	m_outspkconfig->setColour (Label::backgroundColourId, backgroundColour);
	m_outspkconfig->setColour(Label::outlineColourId, findColour(TextEditor::backgroundColourId));
	m_outspkconfig->setColour (Label::textColourId, backgroundColour.contrasting());

	m_outspklabel->setColour (Label::backgroundColourId, backgroundColour);
	m_outspklabel->setColour (Label::textColourId, backgroundColour.contrasting());
}

void BusWdmComponent::buttonClicked (Button* buttonThatWasClicked)
{
	if (buttonThatWasClicked == m_set_ch_maps)
    {
		if (m_bus)
		{
			// bring up modeless window
			if (chMapWindow == 0)
			{
				chMapWindow = new WdmChMapWindow(m_bus);
				chMapWindow->centreAroundComponent (this, 600, 500);
				chMapWindow->setConstrainer(m_constrainer);
				chMapWindow->setVisible (true);
				chMapWindow->toFront(true);				
			}
			else
			{
				chMapWindow->toFront(true);				
			}
		}
	}
}

void BusWdmComponent::actionListenerCallback(const String &message)
{
	if ( (0 == message.compare("nickname changed")) || (0 == message.compare("update channels")) )
	{
		if (chMapWindow)
		{
			chMapWindow->updateContent();
		}
	}
}

void BusWdmComponent::updateContent()
{
	update_speaker();
}

void BusWdmComponent::clearContent()
{
	m_outspkconfig->setText(String::empty, false);
}


// Async callback used for GUI updates
// Since the PAL uses locks and the message thread (this GUI thread) uses locks,
//  we use this asynchronous mechanism to avoid deadlocks.
//  The PAL callbacks have cached everything needed before triggering updates here
void BusWdmComponent::handleAsyncUpdate()
{
	if (m_update & updt_unmount)
	{
		// bus is no longer available, disable gui components
		m_update = 0;
		clearContent();
	}
	else if (m_bus)
	{
		// this object has just been attached, enable and populate component values
		if (m_update & updt_mount)
		{
			m_update &= ~updt_mount;
			updateContent();
		}
				
		// the speaker setup has changed
		if (m_update & updt_speaker)
		{
			String outStr(String::empty);

			m_update &= ~updt_speaker;

			switch (m_outspeaker)
			{
			case 0:
				outStr="None (Direct)";
				break;
			case 4:
				outStr="Mono";
				break;
			case 3:
				outStr="Stereo";
				break;
			case 51:
				outStr="Quad";
				break;
			case 263:
				outStr="Surround";
				break;
			case 63:
				outStr="5.1";
				break;
			case 255:
				outStr="7.1 Wide";
				break;
			case 1551:
				outStr="5.1 Home";
				break;
			case 1599:
				outStr="7.1 Home";
				break;
			default:
				break;
			}

			m_outspkconfig->setText(outStr, false);
		}
	}
}

void BusWdmComponent::mount(tcat::dice::reference<event_bus> bus)
{
	// call super's mount() first
	bus_listener::mount(bus);
	// which gives us an m_bus reference
	if (m_bus)
	{
		m_update |= updt_mount;	// notification for gui thread
		triggerAsyncUpdate();		// notify the gui thread
	}
}

void BusWdmComponent::unmount()
{
	// call super's unmount first
	bus_listener::unmount();
	m_update |= updt_unmount;
	triggerAsyncUpdate();
}

void BusWdmComponent::update_speaker()
{
	if (m_bus)
	{
		m_outspeaker = m_bus->speaker();

		m_update |= updt_speaker;
		triggerAsyncUpdate();
	}
}
