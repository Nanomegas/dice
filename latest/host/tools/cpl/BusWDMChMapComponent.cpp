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
#include "SpeakerList.h"
#include "BusWDMChMapComponent.h"
#include "juce_TCATLookAndFeel.h"
#include "EventLogger.h"

//==============================================================================
WdmChMapComponent::WdmChMapComponent () :
		m_out (false),
		m_group (0),
		m_input (0),
		m_output (0),
		m_dragging (0),
		m_speaker_label (0),
		m_speaker_table (0),
		m_dev_ch_label (0),
		m_device_channels (0),
		m_speaker(0),
		m_update (0)
{
	setName("WDM Ch Mapper");

	initWdmNames();
	getChannelNames();
	getMappedNames();

    addAndMakeVisible (m_group = new GroupComponent ("channelsGroup", "In Channels"));

    addAndMakeVisible (m_input = new ToggleButton ("input"));
    m_input->setButtonText ("Input");
    m_input->addListener (this);
	m_input->setRadioGroupId(1234);
	
    addAndMakeVisible (m_output = new ToggleButton ("output"));
    m_output->setButtonText ("Output");
    m_output->addListener (this);
	m_output->setRadioGroupId(1234);

    addAndMakeVisible (m_reset = new TextButton ("reset"));
    m_reset->setButtonText ("Reset");
    m_reset->addListener (this);

	addAndMakeVisible (m_speaker_label = new Label ("wdmChannelsLabel", "WDM Channels"));
    m_speaker_label->setFont (Font (15.0000f, Font::plain));
    m_speaker_label->setJustificationType (Justification::centredBottom);
    m_speaker_label->setEditable (false, false, false);

    addAndMakeVisible (m_speaker_table = new SpeakerListComponent (*this, 
																		m_wdm_names,
																		m_in_ch_names, 
																		m_out_ch_names, 
																		m_in_map_names,
																		m_out_map_names, 
																		false));
    m_speaker_table->setOutlineThickness(1);
	
    addAndMakeVisible (m_wdm_ch_label = new Label ("wdmChLabel", "Mapped WDM Channels"));
    m_wdm_ch_label->setFont (Font (15.0000f, Font::plain));
    m_wdm_ch_label->setJustificationType (Justification::topLeft);
    m_wdm_ch_label->setEditable (false, false, false);

    addAndMakeVisible (m_dev_ch_label = new Label ("devChLabel", "Device Channels"));
    m_dev_ch_label->setFont (Font (15.0000f, Font::plain));
    m_dev_ch_label->setJustificationType (Justification::topLeft);
    m_dev_ch_label->setEditable (false, false, false);

    addAndMakeVisible (m_instr_label = new Label ("instLabel", "Drag WDM channels \nto Device channels to map them.\n\nDrag maps out of the \nDevice channels list\nto unmap them."));
    m_instr_label->setFont (Font (15.0000f, Font::plain));
    m_instr_label->setJustificationType (Justification::centred);
    m_instr_label->setEditable (false, false, false);

    addAndMakeVisible (m_device_channels = new ChannelListComponent (*this,  
																		m_wdm_names, 
																		m_in_ch_names, 
																		m_out_ch_names, 
																		m_in_map_names,
																		m_out_map_names));
    m_device_channels->setOutlineThickness(1);

	if (m_out)
		m_input->setToggleState(true, true);
	else
		m_output->setToggleState(true, true);

	lookAndFeelChanged();
}

WdmChMapComponent::~WdmChMapComponent()
{
	deleteAllChildren();
}

//==============================================================================
void WdmChMapComponent::resized()
{
	m_group->setBounds (8, 5, getWidth() - 16, getHeight() - 13);

	m_input->setBounds (35, 35, 65, 24);
	m_output->setBounds (110, 35, 75, 24);

	m_reset->setBounds(60, 90, 90, 24);

	m_speaker_label->setBounds (22, 130, 170, 22);
	m_speaker_table->setBounds (22, 155, 170, 162);
    m_instr_label->setBounds (22, m_speaker_table->getBottom()+10, m_speaker_table->getWidth(), 120);

	m_device_channels->setBounds (200, 51, getWidth() - 221, getHeight() - 71);
    m_wdm_ch_label->setBounds (210, 31, 150, 22);
	m_dev_ch_label->setBounds (420, 31, 150, 22);
}

void WdmChMapComponent::lookAndFeelChanged()
{
	Colour backgroundColour = findColour(TCATLookAndFeel::tcatPopupWindowBackgroundColourId);
	Colour labelTextColour(findColour(TCATLookAndFeel::tcatPopupWindowBackgroundColourId).contrasting());

	repaint();

	m_group->setColour(GroupComponent::textColourId, labelTextColour);
	m_speaker_label->setColour(Label::textColourId, labelTextColour);
	m_dev_ch_label->setColour(Label::textColourId, labelTextColour);
	m_wdm_ch_label->setColour(Label::textColourId, labelTextColour);
	m_instr_label->setColour(Label::textColourId, labelTextColour);
}

void WdmChMapComponent::buttonClicked (Button* buttonThatWasClicked)
{
    if (buttonThatWasClicked == m_input)
    {
		// configure controls and labels for input
		m_out = false;
		m_group->setText("Map Speaker In Channels");
		m_speaker_table->deselectAllRows();
		m_reset->setTooltip("Map all WDM inputs to the first device input channels.");
		updateContent();
    }
    else if (buttonThatWasClicked == m_output)
    {
		// configure controls and labels for output
		m_out = true;
		m_group->setText("Map Speaker Out Channels");
		m_speaker_table->deselectAllRows();
		m_reset->setTooltip("Map all WDM outputs to the first device output channels.");
		updateContent();
	}
    else if (buttonThatWasClicked == m_reset)
    {
		if (m_bus)
		{
			for (int i=0; i<kTCAT_DICE_MAX_WDM_OUT_CHANNELS; i++)
			{
				try
				{
					m_bus->speaker_map(m_out ? 1 : 0, i, i);
					m_bus->ready_wait();
				}
				catch(...)
				{
				}
				String eventStr = String::empty;
				eventStr << "mapped wdm " << (m_out ? "out" : "in") << " channel " << i
						<< " to aggregate dev " << (m_out ? "out" : "in") << " channel " << i; 
				EventLogger::getInstance()->logMessage(eventStr);
			}
		}
		updateContent();
	}
}

//==============================================================================
bool WdmChMapComponent::isInterestedInDragSource (const SourceDetails& /*dragSourceDetails*/)
{
	return true;
}

void WdmChMapComponent::itemDragEnter (const SourceDetails& /*dragSourceDetails*/)
{
	m_dragging = true;
}

void WdmChMapComponent::itemDragMove (const SourceDetails& /*dragSourceDetails*/)
{
}

void WdmChMapComponent::itemDragExit (const SourceDetails& /*dragSourceDetails*/)
{
	m_dragging = false;
}

void WdmChMapComponent::itemDropped (const SourceDetails& dragSourceDetails)
{
	// an example valid drag sourceDescription is:
	//  WDM 2 (Front Right)
	//	 where the number indicates the item's 1-based index
	int map_src_index = dragSourceDetails.description.toString().substring(4,6).getIntValue() - 1;  // convert to zero-based

	if ((map_src_index < 0) || (map_src_index > 15))
	{
		m_dragging = false;
		return;
	}

	// a drop here means we're unmapping a channel
	if (m_bus)
	{
		Component *sourceComp = dragSourceDetails.sourceComponent;
		String sourceName = sourceComp->getName();
		if (sourceName.contains("clist"))
		{
			String rowName = sourceName.fromFirstOccurrenceOf("clist ", false, false);

			try
			{
				m_bus->speaker_map(m_out, (uint32)map_src_index, (uint32)-1);
				m_bus->ready_wait();

				int row_num = rowName.getIntValue();
				if (m_out)
				{
					m_out_map_names.set(row_num, " -");
				}
				else
				{
					m_out_map_names.set(row_num, " -");
				}

				String eventStr = String::empty;
				eventStr << "unmapped wdm " << (m_out ? "out" : "in") << " ch " << map_src_index << ", dev ch " << row_num << " (zero-based)";
				EventLogger::getInstance()->logMessage(eventStr);

				update_speaker_map(0,0);	// update the channel list asynchronously
			}
			catch (...)
			{
				EventLogger::getInstance()->logMessage("ch unmap exception");
			}
		}
	}	
	m_dragging = false;
}

void WdmChMapComponent::handleAsyncUpdate()
{
	if (m_update & updt_unmount)
	{
		// bus is no longer available, disable gui components
		m_update = 0;
		setEnabled(false);
	}
	else if (m_bus)
	{
		// this object has just been attached, enable and populate component values
		if (m_update & updt_mount)
		{
			setEnabled(true);

			updateContent();

			m_update &= ~updt_mount;
		}

		// the speaker setup has changed
		if (m_update & updt_speaker)
		{
			updateContent();

			m_update &= ~updt_speaker;
		}
		
		// the speaker map has changed
		if (m_update & updt_speaker_map)
		{
			updateContent();

			m_update &= ~updt_speaker_map;
		}
	}
}

void WdmChMapComponent::updateContent()
{
	getChannelNames();
	getMappedNames();

	m_speaker_table->updateList();
	m_device_channels->updateContent();
}

void WdmChMapComponent::initWdmNames()
{
	m_wdm_names.add ("Front Left");
	m_wdm_names.add ("Front Right");
	m_wdm_names.add ("Front Center");
	m_wdm_names.add ("Low Frequency");
	m_wdm_names.add ("Back Left");
	m_wdm_names.add ("Back Right");
	m_wdm_names.add ("Front Left of Center");
	m_wdm_names.add ("Front Right of Center");
	m_wdm_names.add ("Back Center");
	m_wdm_names.add ("Side Left");
	m_wdm_names.add ("Side Right");
	m_wdm_names.add ("Top Center");
	m_wdm_names.add ("Top Front Left");
	m_wdm_names.add ("Top Front Center");
	m_wdm_names.add ("Top Front Right");
	m_wdm_names.add ("Top Back Left");
	m_wdm_names.add ("Top Back Center");
	m_wdm_names.add ("Top Back Right");
}

void WdmChMapComponent::getChannelNames()
{
	tcat::dice::bus_impl::channel_list channels;
	if (m_bus)
	{
		channels = m_bus->channels(true);
		m_out_ch_names.clear();
		for (int i=0; i<(int)channels.size(); i++)
		{
			m_out_ch_names.add(channels.at(i).c_str());
		}

		channels = m_bus->channels(false);
		m_in_ch_names.clear();
		for (int i=0; i<(int)channels.size(); i++)
		{
			m_in_ch_names.add(channels.at(i).c_str());
		}
	}
}

void WdmChMapComponent::getMappedNames()
{
	int devChMappedTo = 0;		// index into the list of aggregated channels of all bus devices
	uint32 speakerBits = 0;		// bitmap bits correspond to speaker positions, as defined in ksmedia.h
	int numDevChs = 0;

	tcat::dice::bus_impl::channel_list channels;	// deque containing device names for all aggregated channels of all bus devices

	if (m_bus)
	{
		if (m_out)
		{
			// output channels	
			channels = m_bus->channels(true);
			numDevChs = (int)channels.size();

			m_out_map_names.clear();
			// using set() to initially reserve the array, modify therafter
			for (int i=0; i<(int)numDevChs; i++)
			{
				m_out_map_names.set(i, " -");	// unmapped
			}
			
			// get the speaker positions present in the current speaker setup
			speakerBits = m_bus->speaker();

			// build the wdm name
			for (int chn=0; chn<kTCAT_DICE_MAX_WDM_IN_CHANNELS; chn++)
			{
				String nameStr;
				nameStr << "WDM " << (chn+1); // show 1-based numbering

				// add a wdm speaker name if present in speaker map
				int count = 0;
				for (int pos=0; pos<32; pos++)
				{
					if (speakerBits & (1<<pos))
					{
						if (count == chn)
						{
							nameStr << " (" << m_wdm_names[pos] << ")";
							break;
						}
						count++;
					}
				}
				devChMappedTo = m_bus->speaker_map(true, chn);
				if (devChMappedTo>=0)
				{
					m_out_map_names.set(devChMappedTo, nameStr);
				}
			}
		}
		else
		{
			// input channels	
			channels = m_bus->channels(false);
			numDevChs = (int)channels.size();

			m_in_map_names.clear();
			// using set() to initially reserve the array, modify therafter
			for (int i=0; i<(int)numDevChs; i++)
			{
				m_in_map_names.set(i, " -");		// unmapped
			}

			for (int i=0; i<kTCAT_DICE_MAX_WDM_IN_CHANNELS; i++)
			{
				devChMappedTo = m_bus->speaker_map(false, i);
				if ((devChMappedTo >= 0) && (devChMappedTo < numDevChs))
				{
					String chStr(String::empty);
					chStr << "WDM " << (i+1);	// convert to 1-based numbering
					m_in_map_names.set(devChMappedTo, chStr);
				}
			}
		}
	}
}

// we've been attached to a bus
void WdmChMapComponent::mount(tcat::dice::reference<event_bus> bus)
{
	// call super's mount() first
	bus_listener::mount(bus);
	// which gives us an m_bus reference
	if (m_bus)
	{
		m_update |= updt_mount;	// notification for gui thread
		m_bus->attach(m_speaker_table);
		m_bus->attach(m_device_channels);
		triggerAsyncUpdate();
	}
}

void WdmChMapComponent::unmount()
{
	bus_listener::unmount();
	m_update |= updt_unmount;
	triggerAsyncUpdate();
}

// bus changed, devices added or removed
void WdmChMapComponent::update_devicelist()
{
	m_update |= updt_devicelist;
	triggerAsyncUpdate();
}

// speaker setup changed by another app
void WdmChMapComponent::update_speaker()
{
	if (m_bus)
	{
		m_speaker = m_bus->speaker();
		m_update |= updt_speaker;
		triggerAsyncUpdate();
		sendActionMessage("speaker config changed");
	}
}

// a channel was (re)mapped by another app
void WdmChMapComponent::update_speaker_map(bool out, tcat::uint32 speaker)
{
	m_update |= updt_speaker_map;
	triggerAsyncUpdate();
}
