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

#ifndef __SPEAKER_LIST__
#define __SPEAKER_LIST__

#include "JuceHeader.h"
#include "events.h"
#include "listener.h"
#include "BusWDMChMapComponent.h"

class SpeakerListComponent :
		public ListBox,
		public ListBoxModel,
		public ActionListener,
		public ActionBroadcaster,
		public bus_listener
{
public:
    //==============================================================================
    SpeakerListComponent (WdmChMapComponent& owner_, 
							StringArray &m_wdm_names_, 
							StringArray &m_in_ch_names_, 
							StringArray &m_out_ch_names_, 
							StringArray &m_in_map_names_,
							StringArray &m_out_map_names_, 
							bool out_);

    ~SpeakerListComponent();

	int getNumRows();
    void paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected);
	void updateList();
	void actionListenerCallback (const String &message);

	var getDragSourceDescription (const SparseSet<int>& selectedRows)
	{
		String desc;
		// source description is the selected row item string
		desc << m_list_names[selectedRows[0]];

		return desc.trim();
	}
	
	void update_speaker();
	
	juce_UseDebuggingNewOperator
	
private:
	WdmChMapComponent&	m_owner;
	int					m_num_rows;
	int					m_ch_bits;
	StringArray&		m_wdm_names;
	StringArray&		m_in_ch_names;
	StringArray&		m_out_ch_names;
	StringArray&		m_in_map_names;
	StringArray&		m_out_map_names;
	StringArray			m_list_names;
};

#endif   // __SPEAKER_LIST__
