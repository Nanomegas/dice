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

#include "JuceHeader.h"

//==============================================================================
class MeterLegend  : public Component
{
public:
    //==============================================================================
    MeterLegend();
    ~MeterLegend();

	enum ColourIds
	{
		backgroundColourId              = 0x2001900,    /**< The background colour, behind the bar. */
		textColourId					= 0x2001901,    /**< The text and tickmark colour. */
	};

	void lookAndFeelChanged();
	void setLegendConfig (int maxVal);

	juce_UseDebuggingNewOperator

protected:
	/** @internal */
	void paint (Graphics& g);
	int m_max; //max bar value
};
