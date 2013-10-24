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
#include "DeviceWindow.h"
#include "juce_TCATLookAndFeel.h"
#include "appDefaults.h"
#ifdef WIN32
#include "BusStatsComponent.h"
#endif // WIN32

// the bus window consists of a header and a tabbed control
//  for Mac, there is only the one "Bus" tab
BusWindow::BusWindow ()
#ifdef WIN32
 :	m_buswdm (NULL),
// deprecated	m_busdpc (NULL),
	m_busohci (NULL),
	m_busstats (NULL)
#endif // WIN32
{
	setName ("Bus Window");
	Colour tabColour(findColour(TCATLookAndFeel::tcatGlobalTabAreaColourId));

	// create and place controls. 
	//	the controls will be "live" after a bus listener is attached. 
	//	see mount() below
	m_tabs = new TabbedComponent(TabbedButtonBar::TabsAtTop);
	addAndMakeVisible (m_tabs);
	m_tabs->setBoundsInset(BorderSize<int> (0));
	m_tabs->getTabbedButtonBar().addChangeListener(this);
	m_tabs->setTabBarDepth(35);

	m_busgeneral = new BusGeneralComponent();
	m_tabs->addTab ("Bus", tabColour, m_busgeneral, true);

#ifdef WIN32
	m_buswdm = new BusWdmComponent();
	m_tabs->addTab ("WDM", tabColour, m_buswdm, true);
	m_buswdm->setBounds (0, 28, getWidth(), 130);

	if (appDefaults->getAppSettings()->getBoolValue("showDPC"))
	{
		m_busstats = new BusStatsComponent();
		m_tabs->addTab ("DPC", tabColour, m_busstats, true);
	}
#endif // WIN32

	if (appDefaults->getAppSettings()->getBoolValue("showSystem"))
	{
		m_busohci = new BusOhciCheckComponent();
		m_tabs->addTab ("System", tabColour, m_busohci, true);
	}
    
	m_businfo = new BusInfoComponent();
	m_tabs->addTab ("Info", tabColour, m_businfo, true);

	lookAndFeelChanged();
}

BusWindow::~BusWindow() 
{
	m_tabs->getTabbedButtonBar().removeAllChangeListeners();
    deleteAllChildren();
}

void BusWindow::lookAndFeelChanged()
{
	Colour tabColour(findColour(TCATLookAndFeel::tcatGlobalTabAreaColourId));

	repaint();

	for (int i=0; i<m_tabs->getNumTabs(); i++)
	{
		m_tabs->setTabBackgroundColour(i, tabColour);
	}
	m_tabs->setColour(TabbedComponent::outlineColourId, tabColour);

	repaint();
}

void BusWindow::changeListenerCallback (ChangeBroadcaster* source)
{
}

void BusWindow::resized()
{
	m_tabs->setBounds (2, 31, getWidth()-4, 129);
}

// callback for bus arrival
void BusWindow::mount(tcat::dice::reference<event_bus> bus)
{
	// call super's mount() first
	bus_listener::mount(bus);
	// which gives us an m_bus reference
	if (m_bus)
	{
		// attach the bus listener views
		m_bus->attach(m_busgeneral);
		m_bus->attach(m_businfo);
#ifdef WIN32
		if (m_busstats)
		{
			m_bus->attach(m_busstats);
		}
		m_bus->attach(m_buswdm);
#endif
	}
}

void BusWindow::unmount()
{
	// call super's unmount first
	bus_listener::unmount();
}
