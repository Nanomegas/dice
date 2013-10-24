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
#include "Meter.h"



Meter::Meter ()
   : m_cur(0),
	 m_iyellow(13),
	 m_iorange(63),
	 m_ired(130),
	 m_peakhold(0)
{
    setColour (backgroundColourId,  Colours::white);
    setOpaque(true);    
}

Meter::~Meter ()
{
}

void Meter::setBarConfig (int maxVal, int yellowVal, int orangeVal, int redVal, int fallback)
{
	m_max = maxVal;
	m_vyellow = yellowVal;
	m_vorange = orangeVal;
	m_vred  = redVal;
	m_fallback = fallback;
	resized();
	repaint ();
}

void Meter::resized()
{
	m_iyellow = ((m_vyellow*getHeight())/m_max);
	m_iorange = ((m_vorange*getHeight())/m_max);
	m_ired  = ((m_vred*getHeight())/m_max);
}

void Meter::setValue (int v)
{		
	int y0,y1;
	int new_cur = m_cur;
	
	if (v > m_max) v = m_max;
	v = (v*getHeight())/m_max;
	
	if (v > m_peakhold) m_peakhold = v;
	if (new_cur > m_fallback) 
		new_cur -=m_fallback;
	else
		new_cur = 0;
	if (v > new_cur) new_cur = v;
	
	if (new_cur == m_cur) return;
	if (new_cur > m_cur)
	{		
		y0 = getHeight()-new_cur-1;
		y1 = getHeight()-m_cur+1;
	}
	else
	{		
		y0 = getHeight()-m_cur-1;
		y1 = getHeight()-new_cur+1;
	}
	m_cur = new_cur;
	repaint (0, y0, getWidth(), y1-y0);
	
}

void Meter::reset()
{
	m_peakhold = 0;
	repaint ();
}
/*
The overload bar is 4 pixels high
Then there is a 2 pixel space
Then every second pixel is a bar (1..42)
The whole thing is 4+2+2*42 = 90

*/

void Meter::Part (Graphics& g, int from, int to)
{
    int w = getWidth();

	//[from;to[
	if (to <= from) return;
	
	from = getHeight()-from;
	to = getHeight()-to;
	
    g.fillRect (0,to,w,from-to);
}

#define min(a,b) (((a) < (b))?(a):(b))

void Meter::paint (Graphics& g)
{
    //Check which part is intersected
    //g.drawRect (x, y, w, h);
    int w = getWidth();
    int h = getHeight();
	int i;
	
 
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
		i = min(m_cur,getHeight()+1);
		Part (g, m_ired, i);
	}
    //update back ground Colour
    g.setColour (findColour (backgroundColourId));    
 	if (i < getHeight()+1)
	{
		//fill bg for the rest
		Part (g, i, getHeight()+1);
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
			Part (g, m_peakhold-1, m_peakhold);
		}
	}
}
