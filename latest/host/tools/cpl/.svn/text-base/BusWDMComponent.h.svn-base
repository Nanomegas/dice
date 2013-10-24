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

#ifndef __BUS_WDM_COMPONENT__
#define __BUS_WDM_COMPONENT__

#include "events.h"
#include "listener.h"


class BusWdmComponent : 
	public Component,
	public ButtonListener,
	public ActionListener,
	public bus_listener,
	private AsyncUpdater
{
public:
	BusWdmComponent();
    ~BusWdmComponent();

    void resized();
	void broughtToFront();
	void lookAndFeelChanged();

	void buttonClicked (Button* button);

	void handleAsyncUpdate();

	void actionListenerCallback(const String &message);
    void updateContent();
	void clearContent();
	
	void mount(tcat::dice::reference<event_bus> bus);
	void unmount();
	void update_speaker();
	
    juce_UseDebuggingNewOperator

private:
    Label* m_outspkconfig;
    Label* m_outspklabel;
    TextButton* m_set_ch_maps;

	ComponentBoundsConstrainer * m_constrainer;
	uint32 m_num_clients;				// number of driver handles open by audio clients
	tcat::uint32 m_outspeaker;			// bitmap of channels present in the current speaker setup
	tcat::uint32 m_update;				// bitmap of pending updates
	bool m_allow_wdm_sr_changes_settable;
	bool m_allow_wdm_sr_changes;

	// bus listener events, for notifying the message thread
	enum BUS_UPDATE_NOTIFICATION
	{
		updt_mount =					(1 << 0),
		updt_unmount =					(1 << 1),
		updt_speaker =					(1 << 2),
	};
};

#endif   // __BUS_WDM_COMPONENT__
