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

#include "ChannelStrip.h"
#include "juce_TCATLookAndFeel.h"

//==============================================================================

extern float dBuMap[];
MixSlider::MixSlider (const String& name) :
	Slider (name)
{
	setRange(0, MIXSLIDER_NUM_TICKS, 1);
	setSliderStyle(Slider::LinearVertical);
	setTextBoxStyle(Slider::TextBoxBelow, true, 30, 16);
	lookAndFeelChanged();
}

MixSlider::~MixSlider ()
{
}

String MixSlider::getTextFromValue  ( double value ) 
{
	int v =	(int)jlimit((double)0, (double)(MIXSLIDER_NUM_TICKS -1), value);
	if (0 == v)
	{
		return String (CharPointer_UTF8 ("-""\xe2\x88\x9e")); // minus infinity
	}
	else
	{
		return String (dBuMap[v], 1);
	}
}

void MixSlider::labelTextChanged(Label* label)
{
	float val = (float)jlimit((float)dBuMap[0], (float)(dBuMap[MIXSLIDER_NUM_TICKS-1]), label->getText().getFloatValue());
	setdBValue(val);
}

bool MixSlider::keyPressed  (const KeyPress & key)
{
	if ( (key.isKeyCode (KeyPress::leftKey)) ||  (key.isKeyCode (KeyPress::downKey)) )
	{
		setValue(getValue() - 1);
		return true;
	}
	else if ( (key.isKeyCode (KeyPress::rightKey)) ||  (key.isKeyCode (KeyPress::upKey)) )
	{
		setValue(getValue() + 1);
		return true;
	}
	return false;
}

void MixSlider::setdBValue(float value)
{
	for (int i = 0; i<MIXSLIDER_NUM_TICKS; i++)
	{
		if (value<=dBuMap[i])
		{
			setValue(i);
			return;
		}
	}
}

void MixSlider::lookAndFeelChanged()
{
}

//==============================================================================

MixChannelStrip::MixChannelStrip(const String& name, int mixer_id, int channel_id) :
	m_mixer_id (mixer_id),
	m_channel_id (channel_id),
	m_x_offset (0),
	m_pan (0),
	m_gain (0),
	m_meter (0),
	m_mute (0),
	m_solo (0),
	m_peak (0),
	m_action_msg(String::empty)
{
	setName(name);

	m_action_msg = String::formatted(":%02d+%02d=", m_mixer_id, m_channel_id);

	m_pan = new Slider("pan");
	m_pan->setSliderStyle(Slider::RotaryHorizontalDrag);
	m_pan->setRange(-50, 50, 1);
	m_pan->setTextBoxStyle(Slider::NoTextBox, true, 10, 10);
	addAndMakeVisible(m_pan);
	m_gain = new MixSlider("gain");
	addAndMakeVisible(m_gain);
    m_mute = new ToggleButton("~M");	// tilde means use the character instead of a checkmark
	addAndMakeVisible(m_mute);
    m_solo = new ToggleButton("~S");	// tilde means use the character instead of a checkmark
	addAndMakeVisible(m_solo);
	m_meter = new VMeter();
	m_meter->setPeak(m_peak);
	addAndMakeVisible(m_meter);

	m_mute->addListener(this);
	m_solo->addListener(this);
	m_pan->addListener(this);
	m_gain->addListener(this);

	lookAndFeelChanged();
}

MixChannelStrip::~MixChannelStrip()
{
	deleteAllChildren();
}

void MixChannelStrip::paint(Graphics &g)
{
	Path p;

	// background
	p.addRectangle(0, 35, (float)getWidth(), (float)getHeight()-35);
	Colour backgroundColour = findColour(TCATLookAndFeel::tcatDeviceTabAreaColourId);
	g.setColour (backgroundColour.brighter(0.3f));
	g.fillPath (p);

	// outline
	g.setColour (backgroundColour.darker());
	g.strokePath(p, PathStrokeType (2.0f));
	
	// name
	g.setColour(backgroundColour.contrasting());
	g.drawFittedText(getName(), 2, 0, getWidth()-2, 33, Justification::centredBottom, 3, 0.8f);
}

void MixChannelStrip::setXOffset(int offset)
{
	m_x_offset = offset;
}

int MixChannelStrip::getXOffset()
{
	return m_x_offset;
}

void MixChannelStrip::resized()
{
	int w = getWidth();
	int nameHeight = 35;
	int panWidth = 36;
	int gainWidth = 30;
	int gainHeight = 210;
	int meterWidth = 9;
	int meterHeight = 180;
	int buttonWidth = 22;
	int space = 4;

	m_pan->setBounds(w/2 - panWidth/2, nameHeight+space*2, panWidth, panWidth);
	m_gain->setBounds(4, m_pan->getBottom()+space, gainWidth, gainHeight);
	m_meter->setBounds(m_gain->getRight()+space, m_pan->getBottom()+space*4, meterWidth, meterHeight);
	m_mute->setBounds(w/2 - buttonWidth/2, m_gain->getBottom()+space*3, buttonWidth, buttonWidth);
	m_solo->setBounds(w/2 - buttonWidth/2, m_mute->getBottom()+space, buttonWidth, buttonWidth);
}

void MixChannelStrip::lookAndFeelChanged()
{
	Colour backgroundColour = findColour(TCATLookAndFeel::tcatDeviceTabAreaColourId);
	Colour sliderColour = findColour(TCATLookAndFeel::tcatTextButtonColourId);

	repaint();

	m_pan->setColour(Slider::rotarySliderFillColourId, backgroundColour);
	m_meter->setColour (VMeter::backgroundColourId, backgroundColour.brighter(0.3f));			
}

void MixChannelStrip::enablementChanged()
{
	bool bEnabled = isEnabled();

	m_pan->setEnabled(bEnabled);
	m_gain->setEnabled(bEnabled);
	m_mute->setEnabled(bEnabled);
	m_solo->setEnabled(bEnabled);
}

void MixChannelStrip::sliderValueChanged(Slider * slider)
{
	sendActionMessage(slider->getName() 
		+ m_action_msg + String::formatted("%d", (int)slider->getValue()));
}

void MixChannelStrip::buttonClicked(Button * button)
{
	uint32 state = button->getToggleState();
	
	if (button == m_mute)
	{
		sendActionMessage("mute" + m_action_msg + String::formatted("%x", state));
	}
	else if (button == m_solo)
	{
		sendActionMessage("solo" + m_action_msg + String::formatted("%x", state));
	}
}

void MixChannelStrip::setMeterEnabled(bool is_meter_enabled)
{
	if (is_meter_enabled)
	{
		startTimer(50);
	}
	else
	{
		stopTimer();
	}
}

void MixChannelStrip::setPeakHoldEnabled(bool is_hold_enabled)
{
	m_meter->setPeakHoldEnabled(is_hold_enabled);
}

void MixChannelStrip::setPeak(int peak)
{
	m_peak = peak;
}

void MixChannelStrip::timerCallback()
{
	if (!isEnabled())
	{
		m_peak = 0;
	}
	m_meter->setPeak(m_peak);
}

void MixChannelStrip::setPan(int pan)
{
	m_pan->setValue((double)pan, false);
}

void MixChannelStrip::setGain(float value)
{
	m_gain->setdBValue(value);
}

void MixChannelStrip::setGain(int value)
{
	m_gain->setValue(value, false);
}

void MixChannelStrip::setMute(bool value)
{
	m_mute->setToggleState(value, false);
}

void MixChannelStrip::setSolo(bool value)
{
	m_solo->setToggleState(value, false);
}

//==============================================================================

MasterChannelStrip::MasterChannelStrip(const String& name, int mixer_id) :
	m_mixer_id (mixer_id),
	m_x_offset (0),
	m_peak_l(0),
	m_peak_r(0),
	m_is_directout (false)
{
	setName(name);

	m_action_msg = String::formatted(":%02d+00=", m_mixer_id);

	m_gain = new MixSlider("master_gain");
	addAndMakeVisible(m_gain);
    m_mute = new ToggleButton("~M");	// tilde means use the character instead of a checkmark
	addAndMakeVisible(m_mute);
	m_meter_l = new VMeter();
	m_meter_l->setPeak(m_peak_l);
	addAndMakeVisible(m_meter_l);
	m_meter_r = new VMeter();
	m_meter_r->setPeak(m_peak_r);
	m_meter_r->setPeakHoldEnabled(true);
	addAndMakeVisible(m_meter_r);

	m_mute->addListener(this);
	m_gain->addListener(this);

	lookAndFeelChanged();
}

MasterChannelStrip::~MasterChannelStrip()
{
	deleteAllChildren();
}

void MasterChannelStrip::paint(Graphics &g)
{
	Path p;

	// background
	p.addRectangle(0, 35, (float)getWidth(), (float)getHeight()-35);
	Colour backgroundColour = findColour(TCATLookAndFeel::tcatDeviceTabAreaColourId);
	g.setColour (backgroundColour.brighter(0.3f));
	g.fillPath (p);

	// outline
	g.setColour (backgroundColour.darker());
	g.strokePath(p, PathStrokeType (2.0f));
	
	// name
	g.setColour(backgroundColour.contrasting());
	g.drawFittedText(getName(), 2, 0, getWidth()-2, 33, Justification::centredBottom, 3, 0.8f);
}


void MasterChannelStrip::setXOffset(int offset)
{
	m_x_offset = offset;
}

int MasterChannelStrip::getXOffset()
{
	return m_x_offset;
}

void MasterChannelStrip::resized()
{
	int w = getWidth();
	int nameHeight = 35;
	int gainWidth = 28;
	int gainHeight = 210;
	int meterWidth = 9;
	int meterHeight = 180;
	int buttonWidth = 22;
	int space = 4;

	m_gain->setBounds(4, nameHeight+space*3, gainWidth, gainHeight);
	m_meter_l->setBounds(m_gain->getRight()+space, nameHeight+space*5, meterWidth, meterHeight);
	m_meter_r->setBounds(m_meter_l->getRight()+space, nameHeight+space*5, meterWidth, meterHeight);
	m_mute->setBounds(w/2 - buttonWidth/2, m_gain->getBottom()+space*3, buttonWidth, buttonWidth);
}

void MasterChannelStrip::lookAndFeelChanged()
{
	Colour backgroundColour = findColour(TCATLookAndFeel::tcatDeviceTabAreaColourId);

	repaint();

	m_meter_l->setColour (VMeter::backgroundColourId, backgroundColour.brighter(0.3f));			
	m_meter_r->setColour (VMeter::backgroundColourId, backgroundColour.brighter(0.3f));			
}

void MasterChannelStrip::enablementChanged()
{
	bool bEnabled = isEnabled();

	m_gain->setEnabled(bEnabled);
	m_mute->setEnabled(bEnabled);
}

void MasterChannelStrip::setMeterEnabled(bool is_meter_enabled)
{
	if (is_meter_enabled)
	{
		startTimer(50);
	}
	else
	{
		stopTimer();
	}
}

void MasterChannelStrip::setPeakHoldEnabled(bool is_hold_enabled)
{
	m_meter_l->setPeakHoldEnabled(is_hold_enabled);
	m_meter_r->setPeakHoldEnabled(is_hold_enabled);
}
void MasterChannelStrip::setClipThreshold(int thresh_l, int thresh_r)
{
	m_meter_l->setClipThreshold (thresh_l);
	m_meter_r->setClipThreshold (thresh_r);
}

void MasterChannelStrip::setSaturated(bool sat_l, bool sat_r)
{
	m_meter_l->setSaturated (sat_l);
	m_meter_r->setSaturated (sat_r);
}

void MasterChannelStrip::setPeaks(int peak_l, int peak_r)
{
	m_peak_l = peak_l;
	m_peak_r = peak_r;
}

void MasterChannelStrip::timerCallback()
{
	m_meter_r->setPeak(m_peak_r);
	m_meter_l->setPeak(m_peak_l);
}

void MasterChannelStrip::sliderValueChanged(Slider * slider)
{
	sendActionMessage(slider->getName() 
		+ m_action_msg + String::formatted("%d", (int)slider->getValue()));
}

void MasterChannelStrip::buttonClicked(Button * button)
{
	sendActionMessage("master_mute" + m_action_msg + String::formatted("%x", (int)button->getToggleState()));
}

void MasterChannelStrip::setGain(float value)
{
	m_gain->setdBValue(value);
}

void MasterChannelStrip::setGain(int value)
{
	m_gain->setValue(value, false);
}

void MasterChannelStrip::setMute(bool value)
{
	m_mute->setToggleState(value ? true : false, false);
}

void MasterChannelStrip::setDirectOut(bool value)
{
	m_is_directout = value;
}
