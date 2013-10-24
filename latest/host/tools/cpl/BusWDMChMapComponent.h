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

#ifndef __WDM_CH_MAP_COMPONENT__
#define __WDM_CH_MAP_COMPONENT__

#include "JuceHeader.h"
#include "events.h"
#include "listener.h"

//==============================================================================
/**
    This component is a drag-and-drop interface for mapping input device source
    channels to speaker destinations
*/

class WdmChMapComponent : 
		public Component,
		public ButtonListener,
		public ActionBroadcaster,
		public DragAndDropContainer,
		public DragAndDropTarget,
		public AsyncUpdater,
		public bus_listener
{
public:
    //==============================================================================
    WdmChMapComponent ();
    ~WdmChMapComponent();

    //==============================================================================
	// fill m_wdm_names with the WDM channel names from ksmedia.h
	void initWdmNames();

	// refresh m_in_ch_names and m_out_ch_names with the current names
	void getChannelNames();

	// refresh m_in_map_names and m_out_map_names the current mapped WDM channel names 
	void getMappedNames();

    void resized();
	void lookAndFeelChanged();

    void buttonClicked (Button* buttonThatWasClicked);

	// bus listener callbacks use this to allow gui changes in the message thread
    void handleAsyncUpdate();

	void actionListenerCallback (const String &message);

	void updateContent();
	
	void mount(tcat::dice::reference<event_bus> bus);
	void unmount();
	void update_devicelist();
	void update_speaker();
	void update_speaker_map(bool out, tcat::uint32 speaker);

    //==============================================================================
    juce_UseDebuggingNewOperator

private:
	friend class SpeakerListComponent;
	friend class ChannelListComponent;
	friend class RowComponent;
	
    //==============================================================================
    bool					m_out;
	GroupComponent*			m_group;
	ToggleButton*			m_input;
	ToggleButton*			m_output;
	TextButton*				m_reset;
	bool					m_dragging;
    Label*					m_speaker_label;
    SpeakerListComponent*	m_speaker_table;
    Label*					m_dev_ch_label;
    Label*					m_wdm_ch_label;
    Label*					m_instr_label;
    ChannelListComponent*	m_device_channels;
	StringArray				m_wdm_names;			// channels from ksmedia.h
	StringArray				m_ch_names;				// channels in the Speaker Setup, with KS names where relevant
	StringArray				m_in_ch_names;			// aggregate list of input channels of all devices
	StringArray				m_out_ch_names;			// aggregate list of output channels of all devices
	StringArray				m_in_map_names;			// aggregate list of mapped input channels
	StringArray				m_out_map_names;		// aggregate list of mapped output channels
	uint32					m_speaker;				// bitmap of channels in the current Speaker Setup
	tcat::uint32			m_update;				// bitmap of pending updates

	// bus listener events, for notifying the message thread
	enum BUS_UPDATE_NOTIFICATION
	{
		updt_mount =			(1 << 0),
		updt_unmount =			(1 << 1),
		updt_devicelist =		(1 << 2),
		updt_speaker =			(1 << 3),
		updt_speaker_map =		(1 << 4)
	};

	bool isInterestedInDragSource (const SourceDetails& /*dragSourceDetails*/);
	void itemDragEnter (const SourceDetails& /*dragSourceDetails*/);
	void itemDragMove (const SourceDetails& /*dragSourceDetails*/);
	void itemDragExit (const SourceDetails& /*dragSourceDetails*/);
	void itemDropped (const SourceDetails& /*dragSourceDetails*/);
//==============================================================================
    // (prevent copy constructor and operator= being generated..)
    //WdmChMapComponent (const WdmChMapComponent&);
    //const WdmChMapComponent& operator= (const WdmChMapComponent&);
};


#endif   // __WDM_CH_MAP_COMPONENT__
