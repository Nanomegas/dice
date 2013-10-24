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

#ifndef __CHANNEL_LIST__
#define __CHANNEL_LIST__

#include "JuceHeader.h"
#include "events.h"
#include "listener.h"
#include "BusWDMChMapComponent.h"

class WdmChMapComponent;
class ChannelListComponent:
		public ListBox,
		public ListBoxModel,
		public ActionListener,
		public bus_listener
{
public:
    //==============================================================================
    ChannelListComponent (WdmChMapComponent& owner_,
					StringArray &m_wdm_names_, 
					StringArray &m_in_ch_names_, 
					StringArray &m_out_ch_names_, 
					StringArray &m_in_map_names_,
					StringArray &m_out_map_names_);
    ~ChannelListComponent();

    int getNumRows();
    void paintListBoxItem (int rowNumber, Graphics& g, int width, int height, bool rowIsSelected);
	Component * refreshComponentForRow(int rowNumber, bool isRowSelected, Component * existingComponentToUpdate);    

	void lookAndFeelChanged();
	void actionListenerCallback (const String &message);
	void mount(tcat::dice::reference<event_bus> bus);
	
	juce_UseDebuggingNewOperator
	
private:
	friend class RowComponent;
	
	WdmChMapComponent&	m_owner;
	int					m_num_rows;
	StringArray&		m_wdm_names;
	StringArray&		m_in_ch_names;
	StringArray&		m_out_ch_names;
	StringArray&		m_in_map_names;
	StringArray&		m_out_map_names;
};

#endif   // __CHANNEL_LIST__
