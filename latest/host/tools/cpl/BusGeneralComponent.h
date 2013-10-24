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
#include "tcat.h"
#include "events.h"
#include "listener.h"
#include <map>

class BusGeneralComponent;

//==============================================================================
// this component listens for device locked notifications from the bus clock master
class LockStatusComponent :
	public Component,
	public SettableTooltipClient,
	public device_listener,
	public ActionBroadcaster,
	public ActionListener
{
public:
	LockStatusComponent();
	~LockStatusComponent();

	// gui callbacks
	void resized();
	void paint (Graphics& g);
	void lookAndFeelChanged();

	// associates this component with the current master
	void attachToMaster(event_bus_ref& busref);
	
	// device listener callbacks
	void mount(tcat::dice::reference<event_device> device);
	void unmount();
	void update_locked();

	// call this to allow gui changes in the message thread
	//  we're not using AsyncUpdater here because the label component
	//  already privately inherits it.
	void actionListenerCallback(const String &message);

	juce_UseDebuggingNewOperator

private:
	bool				m_locked;
	Drawable			* m_warning_icon;
	Drawable			* m_unlock_icon;
};


//==============================================================================
// this component contains the subcomponents that manage general bus settings
//
//
#define num_buff_sizes	12

class BusGeneralComponent : 
	public Component,
	public ComboBoxListener,
	public ActionBroadcaster,
	public ActionListener,
	public bus_listener,
	private AsyncUpdater
{
public:
	BusGeneralComponent();
    ~BusGeneralComponent();

	// gui callbacks
	void comboBoxChanged (ComboBox* comboBoxThatHasChanged);    
    void resized();
	void paint (Graphics& g);
	void broughtToFront();
	void lookAndFeelChanged();


	// content updates
	void updateContent();
	void clearContent();

	// used for updates between components
	void actionListenerCallback(const String &message);

	// bus listener call this to allow gui changes in the message thread
    void handleAsyncUpdate();
    
	// bus listener callbacks, used in the PAL thread
	void mount(tcat::dice::reference<event_bus> bus);
	void unmount();
	void update_rate_readonly();
	void update_sync_readonly();
	void update_master_readonly();
	void update_io_size_readonly();
	void update_op_mode_readonly();
	void update_nickname_readonly();
    void update_clock();
    void update_clock_caps();
    void update_sync();
	void update_sync_names();
	void update_sync_caps();
	void update_devicelist();
	void update_master();
	void update_op_mode();
#ifdef WIN32
	void update_io_size();
	void update_min_io_size();
	void update_max_io_size();
#endif

	// bus listener events, for notifying the message thread
	enum BUS_UPDATE_NOTIFICATION
	{
		updt_mount =			(1 << 0),
		updt_unmount =			(1 << 1),
		updt_clock =			(1 << 2),
		updt_clock_caps =		(1 << 3),
		updt_sync =				(1 << 4),
		updt_syncnames =		(1 << 5),
		updt_synccaps =			(1 << 6),
		updt_devicelist =		(1 << 7),
		updt_devices =			(1 << 8),
		updt_iosize =			(1 << 9),
		updt_master =			(1 << 10),
		updt_opmode = 			(1 << 11),
		updt_wdm =				(1 << 12),
		updt_speaker =			(1 << 13),
		updt_speakermap =		(1 << 14),
		updt_progress =			(1 << 15),
		updt_min_iosize =		(1 << 16),
		updt_max_iosize =		(1 << 17),
		updt_rate_readonly =	(1 << 18),
		updt_sync_readonly =	(1 << 19),
		updt_master_readonly =	(1 << 20),
		updt_io_size_readonly =	(1 << 21),
		updt_op_mode_readonly =	(1 << 22),
		updt_nickname_readonly =(1 << 23)
	};

private:
    ComboBox * m_rate_cb;				// sample rates combobox
	Label * m_rate_l;
    ComboBox * m_sync_cb;				// sync names combobox
	Label * m_sync_l;
    ComboBox * m_buffer_cb;				// buffer size combobox
	Label * m_buffer_l;
    ComboBox * m_master_cb;				// current bus clock master combobox
	Label * m_master_l;
	LockStatusComponent * m_locked_l;	// lock state of current bus clock master
    ComboBox* m_opmode_cb;				// operation mode combobox
	Label * m_opmode_l;

	Drawable * m_clock_icon;

	tcat::dice::CLOCK m_rate;			// nominal sample rate
	tcat::dice::CLOCK_CAPS m_clockcaps;	// available nominal sample rates
	tcat::dice::SYNC m_sync;			// current sync source
	tcat::dice::SYNC_CAPS m_synccaps;	// available sync sources
	tcat::uint32 m_iosize;				// buffer size
	tcat::uint32 m_min_iosize;			// minimum buffer size
	tcat::uint32 m_max_iosize;			// maximum buffer size
	tcat::uint64 m_master;				// guid of current master
	bool m_locked;						// bus clock lock state
	String m_master_nickname;			// nickname of current master	
	uint32	m_num_clients;				// number of open ASIO and WDM client handles
	tcat::uint32 m_mode;				// op mode
	tcat::uint32 m_update;				// bitmap of pending updates
	bool m_rate_hide_if_single;			// hide sample rate controls if there is only one choice
	bool m_sync_hide_if_single;			// hide sync source controls if there is only one choice

	StringPairArray	m_syncnames_a;		// names of available sync sources
	StringPairArray m_clocknames_a;		// names of available sample rates
	std::deque<tcat::uint64> m_wwuids;	// unique id's of available bus devices
	std::deque<std::string> m_names;	// nicknames of available bus devices

	static const int buff_sizes[num_buff_sizes];
};
