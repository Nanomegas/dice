///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2009 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
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

//==============================================================================
class VMeter  : public Component
{
public:
	//==============================================================================
	VMeter();
	~VMeter();

	enum ColourIds
	{
		backgroundColourId              = 0x2001900,    /**< The background colour, behind the bar. */
	};

	void resized();
	void paint (Graphics& g);

	// used to light the clip bar based on threshold
	//  set threshold to zero to use saturation
	void setClipThreshold(int threshold);

	// used to light the clip bar
	void setSaturated(bool is_saturated);

	// enable peak hold bar
	void setPeakHoldEnabled(bool is_enabled);

	// set current peak
	void setPeak (int v);
	
protected:
	/** @internal */
	int linToPixel (int v);
	int m_cur;
	int m_over;
	bool m_hold_enabled;
	int m_hold;
	int m_delay;
	int m_clip_threshold;

	Image m_meter_img;
	Image m_meter_bg_img;
};
