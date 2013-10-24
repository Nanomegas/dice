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

#include "HMeter.h"


HMeter::HMeter ()
   : m_cur(0),
	 m_iyellow(13),
	 m_iorange(63),
	 m_ired(130),
	 m_peakhold(0)
{
    setColour (backgroundColourId,  Colours::white);
    setOpaque(true);    
}

HMeter::~HMeter ()
{
}

void HMeter::setBarConfig (int maxVal, int yellowVal, int orangeVal, int redVal, int fallback)
{
	m_max = maxVal;
	m_vyellow = yellowVal;
	m_vorange = orangeVal;
	m_vred  = redVal;
	m_fallback = fallback;
	resized();
	repaint ();
}

void HMeter::resized()
{
	m_iyellow = ((m_vyellow*getWidth())/m_max);
	m_iorange = ((m_vorange*getWidth())/m_max);
	m_ired  = ((m_vred*getWidth())/m_max);
}

void HMeter::setValue (int v)
{		
	int x0,x1;
	int new_cur = m_cur;
	
	if (v > m_max) v = m_max;
	v = (v*getWidth())/m_max;
	
	if (v > m_peakhold) m_peakhold = v;
	if (new_cur > m_fallback) 
		new_cur -=m_fallback;
	else
		new_cur = 0;
	if (v > new_cur) new_cur = v;
	
	if (new_cur == m_cur) return;

	if (new_cur > m_cur)
	{		
		x0 = m_cur-1;
		x1 = new_cur+1;
	}
	else
	{		
		x0 = new_cur-1;
		x1 = m_cur+1;
	}
	m_cur = new_cur;

	repaint (x0, 0, x1-x0, getHeight());
}

void HMeter::reset()
{
	m_peakhold = 0;
	repaint ();
}


void HMeter::Part (Graphics& g, int from, int to)
{
    int h = getHeight();

	if (to <= from) return;
	
    g.fillRect (from,0,to-from,h);
}

#define min(a,b) (((a) < (b))?(a):(b))

void HMeter::paint (Graphics& g)
{
 	int i;
/*
	//Check which part is intersected
    int w = getWidth();
    int h = getHeight();
	

	g.setColour(Colours::black);
    g.fillRect(0, 0, w, h);
*/ 
	i=0;
	if (m_cur > 0)
	{
		//fill green up to min(i,m_iyellow)
		g.setColour (Colour(0x0,0xff,0x0));
		i = min(m_cur,m_iyellow);
		Part (g, 0, i);
	}
	if (m_cur > m_iyellow)
	{
		//fill yellow up to min(i,m_iorange)
		g.setColour (Colour(0xff,0xff,0x0));
		i = min(m_cur,m_iorange);
		Part (g, m_iyellow, i);
	}
	if (m_cur >= m_iorange)
	{
		//fill orange up to min(i,m_ired)
		g.setColour (Colour(0xff,0x7f,0x0));
		i = min(m_cur,m_ired);
		Part (g, m_iorange, i);
	}
	if (m_cur >= m_ired)
	{
		//fill red up to min(i,getHeight()+1)
		g.setColour (Colour(0xff,0x00,0x0));
		i = min(m_cur,getWidth()+1);
		Part (g, m_ired, i);
	}
    //update back ground Colour
    g.setColour (findColour (backgroundColourId));       
 	if (i < getWidth()-1)
	{
		//fill bg for the rest
		Part (g, i, getWidth());
		if (m_peakhold && (m_peakhold>=i))
		{
			if (m_peakhold > m_ired)
				g.setColour (Colour(0xff,0x00,0x0));
			else if (m_peakhold > m_iorange)
				g.setColour (Colour(0xff,0x7f,0x0));
			else  if (m_peakhold > m_iyellow)
				g.setColour (Colour(0xff,0xff,0x0));
			else
				g.setColour (Colour(0x0,0xff,0x0));
			Part (g, m_peakhold, m_peakhold+1);
		}
	}
}
