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
#include "SpeakerList.h"
#include "BusWDMChMapComponent.h"
#include "juce_TCATLookAndFeel.h"

SpeakerListComponent::SpeakerListComponent (WdmChMapComponent& owner_, 
											StringArray &m_wdm_names_, 
											StringArray &m_in_ch_names_, 
											StringArray &m_out_ch_names_, 
											StringArray &m_in_map_names_,
											StringArray &m_out_map_names_, 
											bool out_) :
												m_owner (owner_),
												m_wdm_names (m_wdm_names_),
												m_in_ch_names (m_in_ch_names_),
												m_out_ch_names (m_out_ch_names_),
												m_in_map_names (m_in_map_names_),
												m_out_map_names (m_out_map_names_),
												ListBox (" ", 0),
												m_num_rows (0),
												m_ch_bits (0)
{
	setRowHeight(20);
	setModel (this);
    setMultipleSelectionEnabled (false);
	setColour(ListBox::backgroundColourId, findColour(TCATLookAndFeel::tcatListboxBackgroundColourId));
	setColour(ListBox::outlineColourId, findColour(TCATLookAndFeel::tcatListboxOutlineColourId));
	
    // the speaker list wants action messages from the owner
	m_owner.addActionListener(this);
}

SpeakerListComponent::~SpeakerListComponent()
{
}

int SpeakerListComponent::getNumRows()
{
	return m_num_rows;
}

void SpeakerListComponent::paintListBoxItem (int rowNumber,
                        Graphics& g,
                        int width, int height,
                        bool rowIsSelected)
{
	Colour backgroundColour = findColour(TCATLookAndFeel::tcatListboxBackgroundColourId);

	if (rowIsSelected)
	{
		g.fillAll (backgroundColour.darker());
	}

	if (isEnabled())
	{
		g.setColour (backgroundColour.contrasting());
	}
	else
	{
		g.setColour (backgroundColour.contrasting().withAlpha(0.4f));
	}
	g.setFont (height * 0.6f);

	g.drawText (m_list_names[rowNumber],
				5, 0, width, height,
				Justification::centredLeft, true);
}

void SpeakerListComponent::updateList()
{
	if (m_bus)
	{
		String nameStr(String::empty);
		
		m_list_names.clear();

		if (m_owner.m_out)
		{
			m_ch_bits = m_bus->speaker();
			m_num_rows = 0;
			for (int i=0; i<(int)m_wdm_names.size(); i++)
			{
				if ((m_ch_bits>>i) & 1)
				{
					nameStr = String::empty;
					m_num_rows++;
					nameStr << "WDM " << m_num_rows << " (";
					nameStr << m_wdm_names[i];
					nameStr << (")");
					m_list_names.add(nameStr);
				}
			}
			while (m_num_rows++ < kTCAT_DICE_MAX_WDM_OUT_CHANNELS)
			{
				nameStr = String::empty;
				nameStr << "WDM " << m_num_rows;
				m_list_names.add(nameStr);
			}
			m_num_rows = kTCAT_DICE_MAX_WDM_OUT_CHANNELS;
		}
		else
		{
			m_num_rows = 0;
			while (m_num_rows++ < kTCAT_DICE_MAX_WDM_IN_CHANNELS)
			{
				nameStr = String::empty;
				nameStr << "WDM " << m_num_rows;
				m_list_names.add(nameStr);
			}
			m_num_rows = kTCAT_DICE_MAX_WDM_IN_CHANNELS;
		}
	}
	updateContent();
	repaint();
}

// speaker config was changed from this app
void SpeakerListComponent::actionListenerCallback (const String &message)
{
	if (0 == message.compare("speaker config changed"))
	{
		updateList();
	}
}

// out speaker config was changed by another app
void SpeakerListComponent::update_speaker()
{
	sendActionMessage("speaker config changed");
}
