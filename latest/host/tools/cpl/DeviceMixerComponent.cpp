///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2010 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
//
// Unauthorized use, duplication or distribution of this software is  
// strictly prohibited by law.
//
// The entire notice above must be reproduced on all authorised copies and
// copies may only be made to the extent permitted by a licensing agreement
// from TC Applied Technologies Ltd.
//
///////////////////////////////////////////////////////////////////////////////

#include "DeviceMixerComponent.h"
#include "juce_TCATLookAndFeel.h"
#include "ChannelStrip.h"
#include "tcat_dice.h"
#include "dice_eap_device.h"
#include "Blender.h"


// dBu, -100db or less is 0 gain, 0db is unity gain, +6dB is typical max
// these values are for display in the slider textbox
// coefficient calculations use dBV = pow(10, (dBu/20));
float dBuMap [MIXSLIDER_NUM_TICKS+1] = {
	-100.00f, -80.70f, -68.60f, -61.50f, -56.50f, -52.70f, -49.50f, -46.80f,
	 -44.50f, -42.50f, -40.60f, -38.97f, -37.45f, -36.06f, -34.78f, -33.58f,
	 -32.46f, -31.40f, -30.41f, -29.47f, -28.58f, -27.73f, -26.92f, -26.15f,
	 -25.41f, -24.70f, -24.02f, -23.37f, -22.74f, -22.13f, -21.54f, -20.97f,
	 -20.42f, -19.88f, -19.36f, -18.86f, -18.37f, -17.89f, -17.43f, -16.98f,
	 -16.54f, -16.11f, -15.69f, -15.28f, -14.88f, -14.49f, -14.11f, -13.74f,
	 -13.37f, -13.01f, -12.66f, -12.32f, -11.98f, -11.65f, -11.33f, -11.01f,
	 -10.69f, -10.39f, -10.08f,  -9.79f,  -9.50f,  -9.21f,  -8.93f,  -8.65f,
	  -8.37f,  -8.10f,  -7.84f,  -7.58f,  -7.32f,  -7.07f,  -6.82f,  -6.57f,
	  -6.33f,  -6.09f,  -5.80f,  -5.51f,  -5.22f,  -4.94f,  -4.67f,  -4.41f,
	  -4.16f,  -3.91f,  -3.68f,  -3.44f,  -3.22f,  -2.99f,  -2.78f,  -2.57f,
	  -2.36f,  -2.16f,  -1.97f,  -1.78f,  -1.59f,  -1.40f,  -1.23f,  -1.05f,
	  -0.88f,  -0.71f,  -0.54f,  -0.38f,  -0.22f,   0.00f,   0.19f,   0.50f,
	   0.80f,   1.08f,   1.36f,   1.63f,   1.89f,   2.15f,   2.39f,   2.63f,
	   2.87f,   3.10f,   3.32f,   3.53f,   3.74f,   3.95f,   4.15f,   4.35f,
	   4.54f,   4.73f,   4.91f,   5.09f,   5.27f,   5.44f,   5.61f,   5.78f,
	   5.94f,   6.00f , 6.00000001f };

#ifdef WIN32
// suppress warning about default initialize of array in constructor
//  default init is correct but recently supported by MS, so MS warns that we're using it as new behavior
#pragma warning( disable : 4351 )
#endif

// this component contains the subcomponents that display mixer-related items
DeviceMixerComponent::DeviceMixerComponent(DeviceWindow * _owner, const String & name, int _mixer_id) :
	m_owner (_owner),
	m_guid (0),
	m_mixer_id (_mixer_id),
	m_channel (),
	m_num_channels (0),
	m_ch_peak_id (),
	m_master (0),
	m_master_peak_ids(),
	m_ch_peak_mult (),
	m_direct_out (0),
	m_user_dirout (false),
	m_forced_dirout (false),
	m_reset (0),
	m_mix_ch_name (),
	m_channel_spacing (),
	m_channel_offset (),
	m_group_label_name (),
	m_group_label_x_offset (),
	m_action_msg (String::empty),
	m_description (String::empty),
	m_panel_width (700)
{
	setName (name);

	jassert(m_owner->m_blender);  // Blender observer must exist
	
	// message fragment for direct-out state
	m_action_msg = (String::formatted("dirout:%02d+00=", m_mixer_id));

	m_master = new MasterChannelStrip("Output", m_mixer_id);
	addAndMakeVisible(m_master);
	m_master->setMeterEnabled(true);      
	m_master->setPeakHoldEnabled(true);
// bkbk	m_master->setClipThreshold(0, 0); // use saturation for clip

	// blender component wants action messages from the channel strip
	m_master->addActionListener(m_owner->m_blender);
	m_master->setBounds(400, 20, 65, 300);

	addAndMakeVisible (m_direct_out = new ToggleButton (String::empty));
    m_direct_out->addListener (this);

	addAndMakeVisible (m_reset = new TextButton ("reset", "Reset all settings for this mixer tab"));
    m_reset->addListener (this);

	// blender component wants action messages from this mixer
	addActionListener(m_owner->m_blender);
	// mixer component wants change messages from the blender
	m_owner->m_blender->addChangeListener(this);

	setEnabled(false);
}

DeviceMixerComponent::~DeviceMixerComponent()
{
	for (int ch_id=0; ch_id<m_num_channels; ch_id++)
	{
		m_channel[ch_id]->removeActionListener(m_owner->m_blender);
	}
	m_master->removeActionListener(m_owner->m_blender);
	removeActionListener(m_owner->m_blender);

	if (m_owner->m_blender)
	{	
		MessageManagerLock mmLock;
		m_owner->m_blender->removeChangeListener(this);
	}
	
	deleteAllChildren();
}

void DeviceMixerComponent::addChannel(int ch_id, const String & name)
{
	if (MIX_MAX_CHANNELS <= ch_id)
	{
		DBG("Error: max channel id exceeded.");
		return;
	}
	m_channel[ch_id] = new MixChannelStrip(String::formatted("%d", ch_id), m_mixer_id, ch_id);
	addAndMakeVisible(m_channel[ch_id]);
	m_channel[ch_id]->setMeterEnabled(true);
	m_channel[ch_id]->setPeakHoldEnabled(true);
	m_channel[ch_id]->setName(name);

	// blender component wants action messages from the channel strip
	m_channel[ch_id]->addActionListener(m_owner->m_blender);

	m_num_channels++;
}

void DeviceMixerComponent::setChannelPos(int ch_id, int x_offset)
{
	if (ch_id < m_num_channels)
	{
		m_channel[ch_id]->setXOffset(x_offset);
	}
}

void DeviceMixerComponent::setMasterPos(int x_offset)
{
	m_master->setXOffset(x_offset);
	m_master->setBounds(m_master->getXOffset(), 20, 65, 300);
	m_panel_width = m_master->getX() + m_master->getWidth() + 230;
}

void DeviceMixerComponent::setDescription (String & description)
{
	m_description = description;
	repaint();
}

int DeviceMixerComponent::getMixerWidth()
{
	return m_panel_width;
}

void DeviceMixerComponent::addGroupLabel(int x_offset, const String & text)
{
	m_group_label_x_offset[m_group_label_name.size()] = x_offset;
	m_group_label_name.add(text);
}

void DeviceMixerComponent::forceDirectOut(bool bIsDirectOut)
{
	m_forced_dirout = bIsDirectOut;

	updateContent();
}	


void DeviceMixerComponent::paint (Graphics& g)
{
	g.setColour(findColour(TCATLookAndFeel::tcatDeviceTabAreaColourId).contrasting());
	g.setFont(Font(18.0f, Font::bold));
	g.drawFittedText(m_description, 10, 0, getWidth()-20, 20, Justification::centred, 1, 1.0f);
	g.setFont(Font(16.0f, Font::bold));
	g.drawFittedText("Direct Out", m_master->getX() + 22, m_master->getBottom()+10, m_master->getWidth() -20, 24, Justification::centred, 2, 1.0f);
	for (int i=0; i<m_group_label_name.size(); i++)
	{
		g.drawFittedText(m_group_label_name[i], m_group_label_x_offset[i],
			getHeight()-20, 100, 20, Justification::centred, 1, 1.0f);
	}
}

void DeviceMixerComponent::buttonClicked (Button* button)
{
	m_user_dirout = button->getToggleState();

	if (button == m_direct_out)
    {
		// signal the blender of state change
		sendActionMessage(m_action_msg + (m_user_dirout ? "1" : "0"));
	}
	else if (button == m_reset)
    {
		// signal the blender of reset
		sendActionMessage("reset:" + String::formatted("%d", m_mixer_id));
	}

	updateContent();
}

void DeviceMixerComponent::enablementChanged()
{
	bool bEnabled = isEnabled();

	for (int i=0; i<m_num_channels; i++)
	{
		if (m_channel[i])
		{
			m_channel[i]->setMeterEnabled(bEnabled);
		}
	}
	m_master->setMeterEnabled(bEnabled);

	if (bEnabled)
	{
		startTimer(MIX_METER_UPD_MS);
	}
	else
	{
		stopTimer();
	}
}

void DeviceMixerComponent::handleAsyncUpdate()
{
	updateContent();
}

void DeviceMixerComponent::changeListenerCallback (ChangeBroadcaster* source)
{
	Blender * bl = dynamic_cast<Blender *>(source);
	change_msg msg;

	bl->getChangeMessage(&msg);

	int mixer_id = msg.mixer_id;

	if (m_mixer_id != mixer_id) return;

	String nameStr(String::empty);
	int ch_id = msg.ch_id;
	int i = 0;

	switch (msg.type)
	{
	case kGain:
		m_channel[ch_id]->setGain((int)msg.int_val1);
		break;

	case kPan:
		m_channel[ch_id]->setPan((int)msg.int_val1);
		break;

	case kMutes:
		for (i=0; i<m_num_channels; i++)
		{
			bool mute = (0 != (msg.int_val1 & (1<<i)) );
			m_channel[i]->setMute(mute);
		}
		break;

	case kSolos:
		for (i=0; i<m_num_channels; i++)
		{
			bool solo = (0 != (msg.int_val1 & (1<<i)) );
			m_channel[i]->setSolo(solo);
		}
		break;

	case kPeakId:
		m_ch_peak_id[ch_id] = msg.int_val1;
		break;

	case kPeakMult:
		m_ch_peak_mult[ch_id] = msg.float_val;
		break;

	case kChName:
		m_ch_peak_mult[ch_id] = msg.float_val;
		break;

	case kMasterGain:
		m_master->setGain((int)msg.int_val1);
		break;

	case kMasterMute:
		m_master->setMute(0 != msg.int_val1);
		break;

	case kMasterPeakIds:
		m_master_peak_ids[0] = msg.int_val1;
		m_master_peak_ids[1] = msg.int_val2;
		break;

	case kMasterName:
		m_master->setName(msg.str);
		break;

	case kDirectOut:
		m_user_dirout = (0 != msg.int_val1);
		updateContent();
		break;

	case kMixerText:
		if (m_device)
		{
			String descriptionStr(m_device->nickname().c_str());
			descriptionStr = descriptionStr + " - " + msg.str;
			setDescription(descriptionStr);
		}
		break;

	default:
		break;
	}
}

void DeviceMixerComponent::resized()
{
	for (int i=0; i<m_num_channels; i++)
	{
		m_channel[i]->setBounds(m_channel[i]->getXOffset(), 20, 55, 360);
	}
	if (m_master)
	{
		m_master->setBounds(m_master->getXOffset(), 20, 65, 300);
		m_direct_out->setBounds(m_master->getX(), m_master->getBottom()+10, m_master->getWidth(), 24);
		m_reset->setBounds(m_direct_out->getX()+2, m_direct_out->getBottom()+15, m_direct_out->getWidth()-4, 24);
	}
}

//==============================================================================
void DeviceMixerComponent::updateContent (void)
{
	if (m_device)
	{
		if (m_device->isEapSupported())
		{
			setEnabled(true);
		}
		else
		{
			setEnabled(false);
			setVisible(false);
		}

		bool bDirout = (m_user_dirout || m_forced_dirout);

		m_master->setDirectOut(bDirout);
		m_master->setEnabled(!bDirout);
		for (int i=0; i<m_num_channels; i++)
		{
			m_channel[i]->setEnabled(!bDirout);
		}

		m_direct_out->setToggleState(bDirout, false);
		m_direct_out->setEnabled(!m_forced_dirout);
		m_reset->setEnabled(!bDirout);
	}
	else
	{
		setEnabled(false);
	}
}

void DeviceMixerComponent::timerCallback()
{	
	if (!isVisible()) return;

	if (m_device)
	{
		if (m_device->isEapSupported())
		{
			tcat::uint32 peaks[64] = { 0 };
			m_device->getPeak(0,peaks,34);

			for (int i=0; i<m_num_channels; i++)
			{
				int peak_post_fader = (int)((float)peaks[m_ch_peak_id[i]] * m_ch_peak_mult[i]);
 				m_channel[i]->setPeak(peak_post_fader);
			}
			m_master->setPeaks(peaks[m_master_peak_ids[0]], peaks[m_master_peak_ids[1]]); 

/* bkbk
			uint32 sat_bits = m_device->getSatBits();
			uint32 l = (sat_bits & (1<<(2*m_mixer_id)));
			uint32 r = (sat_bits & (1<<((2*m_mixer_id)+1)));
			bool sat_l = l ? true : false;
			bool sat_r = r ? true : false;
			if (sat_l || sat_r)
			{
				m_master->setSaturated(sat_l, sat_r);
			}
*/
		}
	}
}

void DeviceMixerComponent::mount(tcat::dice::reference<event_device> device)
{
	// call super's mount() first
	device_listener::mount(device);
	// which gives us an m_device reference

	triggerAsyncUpdate();
}

void DeviceMixerComponent::unmount()
{
	device_listener::unmount();

	stopTimer();
	triggerAsyncUpdate();
}
