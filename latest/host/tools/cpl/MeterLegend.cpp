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

#include "JuceHeader.h"
#include "MeterLegend.h"

MeterLegend::MeterLegend ()
{
	setColour(MeterLegend::backgroundColourId, Colours::white);

	lookAndFeelChanged();
}

void MeterLegend::lookAndFeelChanged()
{
	repaint();
}

MeterLegend::~MeterLegend ()
{
}

#define NB_LEGENDS 8
static const int legends[NB_LEGENDS] = {500,1000,2000,4000,6000,8000,10000,15000};

void MeterLegend::setLegendConfig (int maxVal)
{
	m_max = maxVal;
	repaint ();
}

#define TXT_H 16

void lookAndFeelChanged()
{
}

void MeterLegend::paint (Graphics& g)
{
    int w = getWidth();
    int x = 0;
    int y = 0;
	
	g.setColour (findColour(MeterLegend::backgroundColourId).contrasting());
	g.setFont (TXT_H * 0.7f);
	for (int i=0; i<NB_LEGENDS; i++)
	{
		// draw tickmarks with room for labels
		x = (legends[i]*w)/m_max;
		if (0 == i)
			y = 6 + (TXT_H * 2);
		else if (1 == i)
			y = 6 + (TXT_H);
		else
			y = 6;
			
		g.drawLine((float)x, 1.0f, (float)x, (float)y);
		
		String str;
		str << legends[i] << "us";
		if (i<7)
			x -= 5;
		else
			x -= 22;
			
		g.drawFittedText (str, x, y+2 , 60, TXT_H, Justification::centredLeft, 1);
	}
}
