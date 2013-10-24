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

#include "DPCTool_headers.h"
#include "MeterLegend.h"



MeterLegend::MeterLegend ()
{
    setColour (backgroundColourId,  Colours::white);
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

void MeterLegend::paint (Graphics& g)
{
    //Check which part is intersected
    //g.drawRect (x, y, w, h);
    int w = getWidth();
    int h = getHeight();
	int i;
	
	
	g.setColour (Colour(0x0,0x00,0x0));
	g.setFont (TXT_H * 0.7f);
	for (i=0; i<NB_LEGENDS; i++)
	{
		int y = (legends[i]*getHeight())/m_max;
		y = h-y;
		g.fillRect (1,y,6,1);
		String str;
		str << legends[i] << "us";
		g.drawFittedText (str,8,y-TXT_H/2,w-8,TXT_H,Justification::centredLeft,1);
	}
}
