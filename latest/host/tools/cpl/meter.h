///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2004 - 2009 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
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

//==============================================================================
class Meter  : public Component
{
public:
    //==============================================================================
    Meter();
    ~Meter();

   enum ColourIds
    {
        backgroundColourId              = 0x2001900,    /**< The background colour, behind the bar. */
    };

	void setValue (int v);
	void setBarConfig (int maxVal, int yellowVal, int orangeVal, int redVal, int fallback);
	void reset();

    juce_UseDebuggingNewOperator

protected:
    /** @internal */
    void paint (Graphics& g);
    int m_cur;
	int m_iyellow, m_vyellow;
	int m_iorange, m_vorange;
	int m_ired, m_vred;
	int m_max; //max bar value
	int m_peakhold;
	int m_fallback;
	Image * m_image_b;
	Image * m_image_g;
	Image * m_image_y;
	Image * m_image_o;
	Image * m_image_r;
	ImageBrush * m_image_brush_b;
	ImageBrush * m_image_brush_g;
	ImageBrush * m_image_brush_y;
	ImageBrush * m_image_brush_o;
	ImageBrush * m_image_brush_r;
	void resized();
};

