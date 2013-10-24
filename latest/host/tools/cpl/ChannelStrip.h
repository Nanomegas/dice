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

#pragma once

#include "JuceHeader.h"
#include "Vmeter.h"

#define MIXSLIDER_NUM_TICKS			130

//==============================================================================
class MixSlider : public Slider
{
public:
    //==============================================================================
    MixSlider(const String& name);
    ~MixSlider();

	void lookAndFeelChanged();

	String getTextFromValue  (  double  value   ) ;

	void labelTextChanged (Label* label);
	bool keyPressed  (const KeyPress & key);

	void setdBValue(float value);

	juce_UseDebuggingNewOperator

    //==============================================================================
    // (prevent copy constructor and operator= being generated..)
    MixSlider (const MixSlider&);
    const MixSlider& operator= (const MixSlider&);
};

//==============================================================================
class MixChannelStrip : 
	public Component, 
	public SliderListener,
	public ButtonListener,
	public ActionBroadcaster,
	private Timer
{
public:
    //==============================================================================
    MixChannelStrip(const String& name, int mixer_id, int channel_id);
    ~MixChannelStrip();

	void paint(Graphics &g);
	void resized();
	void lookAndFeelChanged();
	void enablementChanged();
	void sliderValueChanged(Slider * slider);
	void buttonClicked(Button * button);

	void setMeterEnabled(bool is_meter_enabled);
	void setPeakHoldEnabled(bool is_hold_enabled);
	void setPeak(int peak);

	void setPan(int pan);
	void setGain(float value);
	void setGain(int value);
	void setMute(bool value);
	void setSolo(bool value);

	void setXOffset(int offset);
	int getXOffset();

	juce_UseDebuggingNewOperator

private:
	int				m_mixer_id;
	int				m_channel_id;
	int				m_x_offset;
	Slider			* m_pan;
	MixSlider		* m_gain;
	VMeter			* m_meter;
	ToggleButton	* m_mute;
    ToggleButton	* m_solo;
	int				m_peak;
	String			m_action_msg;

	void timerCallback();
};

//==============================================================================
class MasterChannelStrip : 
	public Component,
	public SliderListener,
	public ButtonListener,
	public ActionBroadcaster,
	private Timer
{
public:
    //==============================================================================
    MasterChannelStrip(const String& name, int mixer_id);
    ~MasterChannelStrip();

	void paint(Graphics &g);
	void resized ();
	void lookAndFeelChanged();
	void enablementChanged();
	void sliderValueChanged(Slider * slider);
	void buttonClicked(Button * button);

	void setMeterEnabled(bool is_meter_enabled);
	void setPeakHoldEnabled(bool is_hold_enabled);
	void setDirectOut(bool is_directout);
	void setPeaks(int peak_l, int peak_r);
	void setClipThreshold(int thresh_l, int thresh_r);
	void setSaturated(bool sat_l, bool sat_r);

	void setGain(float value);
	void setGain(int value);
	void setMute(bool value);

	void setXOffset(int offset);
	int getXOffset();

	juce_UseDebuggingNewOperator

private:
	int				m_mixer_id;
	int				m_x_offset;
	MixSlider		* m_gain;
	VMeter			* m_meter_l;
	VMeter			* m_meter_r;
	ToggleButton	* m_mute;
	int				m_peak_l;
	int				m_peak_r;
	String			m_action_msg;
	bool			m_is_directout;

	void timerCallback();
};
