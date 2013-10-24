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

#include "tcat.h"
#include "tcat_dice.h"
#include "events.h"
#include "listener.h"
#include <map>

class DPCGraph :
	public Component
{
public:
	DPCGraph();
    ~DPCGraph();
	void paint(Graphics &g);
	void resized();
	void updateValue(uint32 value);
	void clear();
	void enableGraph(bool bEnable);

private:
	uint32					m_curVal;
	Array<uint32>			m_values;
	int						m_xVal;
	bool					m_clear;
	bool					m_enabled;
};

//==============================================================================
// this component contains the subcomponents that display DPC information
//
// 
class BusStatsComponent : 
	public Component,
	public Timer,
	public ButtonListener,
	public ComboBoxListener,
	public AsyncUpdater,
	public bus_listener
{
public:
	BusStatsComponent();
    ~BusStatsComponent();

	// gui callbacks
    void resized();
	void paint(Graphics &g);
	void broughtToFront();
    void lookAndFeelChanged();
	void comboBoxChanged(ComboBox* comboBoxThatHasChanged);
	void buttonClicked (Button* buttonThatWasClicked);
	void timerCallback();
	void handleAsyncUpdate();
	void updateInfo();
	void enableStats(bool bShouldBeEnabled);

	// bus listener callbacks, used in the PAL thread
	void mount(tcat::dice::reference<event_bus> bus);
	void unmount();

	// bus listener events, for notifying the gui message thread
	enum BUS_UPDATE_NOTIFICATION
	{
		updt_mount =			(1 << 0),
		updt_unmount =			(1 << 1)
	};

private:
	ScopedPointer<TextButton>			m_clear;

    ScopedPointer<Label>				m_update_rate_l;
	ScopedPointer<ComboBox>				m_update_rate;

	ScopedPointer<DPCGraph>				m_graph;

	uint32								m_maxVal;
	ScopedPointer<Label>				m_cur_val_l;
    ScopedPointer<Label>				m_maxVal_l;
    ScopedPointer<Label>				m_safe_mode_l;

    OwnedArray<Label>					m_count_l;
	OwnedArray<Label>					m_count;
	tcat::dice::ioctl::BUS_DPC_STATS	m_stats;
	uint32								m_values[22];
	tcat::uint32						m_update;				// bitmap of pending updates
};
