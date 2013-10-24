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

#pragma once

#include "JuceHeader.h"
#include "BusGeneralComponent.h"
#include "BusInfoComponent.h"
#include "BusOhciCheckComponent.h"
#ifdef WIN32
#include "BusWdmComponent.h"
#include "BusStatsComponent.h"
#endif // WIN32

class BusWindow : 
	public Component,
	public ChangeListener,
	public bus_listener
{
public:
	BusWindow ();
    ~BusWindow();

	void resized();
	void lookAndFeelChanged();
	void changeListenerCallback (ChangeBroadcaster* source);
	void mount(tcat::dice::reference<event_bus> bus);
	void unmount();

    juce_UseDebuggingNewOperator

private:
	friend class ContentComp;

	TabbedComponent *		m_tabs;
	BusGeneralComponent *	m_busgeneral;
	BusOhciCheckComponent * m_busohci;
	BusInfoComponent *		m_businfo;
#ifdef WIN32
	BusWdmComponent	*		m_buswdm;
	BusStatsComponent *		m_busstats;
#endif // WIN32
};

