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
class HMeter  : public Component
{
public:
    //==============================================================================
	HMeter();
	~HMeter();

	enum ColourIds
	{
		backgroundColourId              = 0x2001900,    /**< The background colour, behind the bar. */
	};

	void setValue (int v);
	void setBarConfig (int maxVal, int yellowVal, int orangeVal, int redVal, int fallback);
	void reset();
	
protected:
    /** @internal */
    void paint (Graphics& g);
    void Part (Graphics& g, int from, int to);
    int m_cur;
	int m_iyellow, m_vyellow;
	int m_iorange, m_vorange;
	int m_ired, m_vred;
	int m_max; //max bar value
	int m_peakhold;
	int m_fallback;
	virtual void resized();
};
