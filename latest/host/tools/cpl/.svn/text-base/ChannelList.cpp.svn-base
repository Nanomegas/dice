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
#include "ChannelList.h"
#include "juce_TCATLookAndFeel.h"
#include "EventLogger.h"

// ListBox item
class RowComponent : 
		public Component,
		public DragAndDropTarget,
		public bus_listener
{
public:
    RowComponent (ChannelListComponent& owner_, int rowNumber_, 
					StringArray &m_wdm_names_, 
					StringArray &m_in_ch_names_, 
					StringArray &m_out_ch_names_, 
					StringArray &m_in_map_names_,
					StringArray &m_out_map_names_) :
        m_owner (owner_),
		m_row(rowNumber_),
		m_dragging (false),
		m_wdm_names (m_wdm_names_),
		m_in_ch_names (m_in_ch_names_),
		m_out_ch_names (m_out_ch_names_),
		m_in_map_names (m_in_map_names_),
		m_out_map_names (m_out_map_names_)
    {
		lookAndFeelChanged();
	}

    ~RowComponent()
    {
    }

	void lookAndFeelChanged()
	{
		repaint();
	}

	void mouseDown  (const MouseEvent &  e)
	{
		String srcDescription(String::empty);

		if (m_owner.m_owner.m_out)
		{
			srcDescription << m_out_map_names[m_row];
		}
		else
		{
			srcDescription << m_in_map_names[m_row];
		}

		if (srcDescription.compare(" -"))
		{
			m_owner.m_owner.findParentDragContainerFor(this)->startDragging(srcDescription, this);
		}
	}

	void paint(Graphics &g)
	{
		int h = getHeight();
		int w = getWidth();
		DrawablePath dp;
		Path p;
		int mapChX = 5;
		int mapChW = 160;
		int arrowX1 = 175;
		int arrowX2 = 195;
		int devChX = 200;

		Colour backgroundColour = findColour(TCATLookAndFeel::tcatListboxBackgroundColourId);
		Colour outlineColour = findColour(TCATLookAndFeel::tcatListboxOutlineColourId);

		ColourGradient cg (backgroundColour,
						  m_owner.m_owner.m_out ? (float)arrowX1 : (float)arrowX2, (float)10,
						  outlineColour,
						  m_owner.m_owner.m_out ? (float)arrowX2 : (float)arrowX1, (float)10, true);

		// draw an outline and arrow if the user's currently dragging something over it..
		if (m_dragging)
		{
			g.setColour (backgroundColour.contrasting());
			g.drawRect (0, 0, w, h, 2);
			if (m_owner.m_owner.m_out)
				p.addArrow(Line<float>((float)arrowX1, 10, (float)arrowX2, 10), 5, 10, 5);
			else
				p.addArrow(Line<float>((float)arrowX2, 10, (float)arrowX1, 10), 5, 10, 5);

			g.saveState();
			g.setGradientFill(cg);
			g.fillPath(p);
			g.restoreState();
		}

        g.setColour (backgroundColour.contrasting());
		g.setFont (h * 0.6f);

		if (m_owner.m_owner.m_out)
		{
			if (m_out_map_names[m_row].compare(" -"))
			{
				// currently mapped wdm channel name
				g.drawFittedText (m_out_map_names[m_row], mapChX, 0, mapChW, h, Justification::left, 1);
				
				// arrow
				p.addArrow(Line<float>((float)arrowX1, 10, (float)arrowX2, 10), 5, 10, 5);

				g.saveState();
				g.setGradientFill(cg);
				g.fillPath(p);
				g.restoreState();
			}

			if (m_out_ch_names[m_row].compare(String::empty))
			{
				// device channel number
				g.drawFittedText (String::formatted("%i", m_row+1), devChX, 0, 15, h, Justification::right, 1);
				// device channel name
				g.drawFittedText (m_out_ch_names[m_row], devChX+25, 0, w-200, h, Justification::left, 2, 0.5f);
			}
		}
		else
		{
			if (m_in_map_names[m_row].compare(" -"))
			{
				// currently mapped wdm channel name
				g.drawFittedText (m_in_map_names[m_row], mapChX, 0, mapChW, h, Justification::left, 1);

				// gradient arrow
				p.addArrow(Line<float>((float)arrowX2, 10, (float)arrowX1, 10), 5, 10, 5);

				g.saveState();
				g.setGradientFill(cg);
				g.fillPath(p);
				g.restoreState();
			}
			
			if (m_in_ch_names[m_row].compare(String::empty))
			{
				// device channel number
				g.drawFittedText (String::formatted("%i", m_row+1), devChX, 0, 15, h, Justification::right, 1);
				// device channel name
				g.drawFittedText (m_in_ch_names[m_row], devChX+25, 0, w-200, h, Justification::left, 1);
			}
		}
	}

	//==============================================================================
	bool isInterestedInDragSource (const SourceDetails& /*dragSourceDetails*/)
	{
		return isEnabled();
	}

	void itemDragEnter (const SourceDetails& /*dragSourceDetails*/)
	{
		m_dragging = true;
		repaint();
	}

	void itemDragMove (const SourceDetails& /*dragSourceDetails*/)
	{
	}

	void itemDragExit (const SourceDetails& /*dragSourceDetails*/)
	{
		m_dragging = false;
		repaint();
	}

	void itemDropped (const SourceDetails& dragSourceDetails)
	{
		bool bNewMap = false;	// map or remap happened

		String sourceDescription = dragSourceDetails.description.toString();

		// an example valid drag sourceDescription is:
		//  WDM 2 (Front Right)
		//	 where the number indicates the item's 1-based index
		int map_src_index = sourceDescription.substring(4,6).getIntValue() - 1;  // convert to zero-based

		if ((map_src_index < 0) || (map_src_index > 15))
		{
			m_dragging = false;
			repaint();
			return;
		}

		if (m_owner.m_owner.m_out)
		{
			// find any existing map, and change only if different
			int index = m_out_map_names.indexOf(sourceDescription);
			if (index != m_row)
			{
				// clear any existing map on a different channel
				if (index >= 0)
				{
					m_out_map_names.set(index, " -");
					bNewMap = true;
				}

				// set the new map
				m_out_map_names.set(m_row, sourceDescription);
				
				if (m_owner.m_bus)
				{
					try
					{
						m_owner.m_bus->speaker_map(true, (uint32)map_src_index, m_row);
						String eventStr = String::empty;
						eventStr << "mapped wdm out channel " << map_src_index
								<< " to aggregate dev out channel " << m_row << " (zero-based)"; 
						EventLogger::getInstance()->logMessage(eventStr);
					}
					catch (...)
					{
						EventLogger::getInstance()->logMessage("out ch map exception");
					}
					bNewMap = true;
				}
			}
		}
		else	
		{			
			// find any existing map, and change only if different
			int index = m_in_map_names.indexOf(sourceDescription);
			if (index != m_row)
			{
				// clear any existing map on a different channel
				if (index >= 0)
				{
					m_in_map_names.set(index, " -");
					bNewMap = true;
				}

				// set the new map
				m_in_map_names.set(m_row, sourceDescription);
				
				if (m_owner.m_bus)
				{
					try
					{
						m_owner.m_bus->speaker_map(false, (uint32)map_src_index, m_row);
						String eventStr = String::empty;
						eventStr << "mapped wdm in channel " << map_src_index
								<< " to aggregate dev in channel " << m_row << " (zero-based)"; 
						EventLogger::getInstance()->logMessage(eventStr);
					}
					catch (...)
					{
						EventLogger::getInstance()->logMessage("in ch map exception");
					}
					bNewMap = true;
				}
			}
		}

		m_dragging = false;
		repaint();
		if (bNewMap) m_owner.updateContent();
	}

	void mount(tcat::dice::reference<event_bus> bus)
	{
		// call super's mount() first
		bus_listener::mount(bus);
		// which gives us an m_bus reference
		if (m_bus)
		{
		}
	}
	
	juce_UseDebuggingNewOperator

private:
	ChannelListComponent&	m_owner;
	int						m_row;
    bool					m_dragging;
	StringArray&			m_wdm_names; 
	StringArray&			m_in_ch_names;
	StringArray&			m_out_ch_names;
	StringArray&			m_in_map_names;
	StringArray&			m_out_map_names;
};


ChannelListComponent::ChannelListComponent (WdmChMapComponent& owner_, 
					StringArray &m_wdm_names_, 
					StringArray &m_in_ch_names_, 
					StringArray &m_out_ch_names_, 
					StringArray &m_in_map_names_,
					StringArray &m_out_map_names_) :
		m_owner (owner_),
		m_num_rows (0),
		ListBox ("ch list", 0),
		m_wdm_names (m_wdm_names_),
		m_in_ch_names (m_in_ch_names_),
		m_out_ch_names (m_out_ch_names_),
		m_in_map_names (m_in_map_names_),
		m_out_map_names (m_out_map_names_)
{
	m_owner.addActionListener(this);	// channel list wants action messages from the owner

	setModel (this);
    setMultipleSelectionEnabled (false);
	setColour(ListBox::backgroundColourId, findColour(TCATLookAndFeel::tcatListboxBackgroundColourId));
	setColour(ListBox::outlineColourId, findColour(TCATLookAndFeel::tcatListboxOutlineColourId));
	lookAndFeelChanged();
}

ChannelListComponent::~ChannelListComponent()
{
}

int ChannelListComponent::getNumRows()
{
	if (m_owner.m_bus)
	{
		m_num_rows = m_owner.m_out ? m_out_ch_names.size() : m_in_ch_names.size();
		return m_num_rows;
	}
	return 0;
}

void ChannelListComponent::lookAndFeelChanged()
{
	repaint();
}

// updating custom ListBox component
Component * ChannelListComponent::refreshComponentForRow(int rowNumber, bool isRowSelected, Component * existingComponentToUpdate)
{
	RowComponent* rowBox = (RowComponent*) existingComponentToUpdate;

	// If an existing component is being passed-in for updating, we'll delete it and create a new one
	//  this works around a scrolling issue with the ListBox control
	if (rowBox)
	{
		delete rowBox;
	}
		
	if (rowNumber < m_num_rows)
	{
		rowBox = new RowComponent (*this, rowNumber, m_wdm_names, m_in_ch_names, m_out_ch_names, m_in_map_names, m_out_map_names);
		rowBox->setName(String::formatted("clist %d", rowNumber));
		return rowBox;
	}
	
	return 0;	// 0 indicates we use custom row components which paint themselves
}

void ChannelListComponent::paintListBoxItem (int rowNumber,
                        Graphics& g,
                        int width, int height,
                        bool rowIsSelected)
{
}

void ChannelListComponent::actionListenerCallback (const String &message)
{
	if (0 == message.compare("speaker config changed"))
	{
		updateContent();
	}
}

void ChannelListComponent::mount(tcat::dice::reference<event_bus> bus)
{
	// call super's mount() first
	bus_listener::mount(bus);
	// which gives us an m_bus reference
	if (m_bus)
	{
	}
}
