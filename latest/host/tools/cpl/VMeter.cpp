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

#include "VMeter.h"
#include "BinaryData.h"

#define METER_HEIGHT		180

VMeter::VMeter ()
   : m_cur(10),
     m_over(0),
	 m_hold_enabled(false),
     m_hold(0),
     m_delay(10),
	 m_clip_threshold (86),
	 m_meter_img (0),
	 m_meter_bg_img (0)
{
    setOpaque(false); 
	setBufferedToImage(true);

	m_meter_img = ImageFileFormat::loadFrom (BinaryData::meter_png, BinaryData::meter_pngSize);
	m_meter_bg_img = ImageFileFormat::loadFrom (BinaryData::meter_bg_png, BinaryData::meter_bg_pngSize);
}

VMeter::~VMeter ()
{
}

void VMeter::resized()
{
}

int VMeter::linToPixel (int v)
{
	//0 all off
	//1 = -72dB
	//2 = -68dB (=-(72-4*(i-1)))
	//10 = -36
	//11 = -34 ( = -(36 - 2*(i-10)))
	//19 = -18
	//20 = -17 ( = -(18 - (i-19)))
	//31 = -6
	//32 = -5.5 ( = -(6 - (i-31)/2))
	//33 = 5
	//43 = 0 dB
    int halfdb;
    if (v<=0)
		halfdb = -200;
	else
		halfdb = (int)floor(40.0*((float)log10((float)v)-(float)log10(4096.0f))+0.5f);

	if (halfdb < -72)
		return 0;
	else if (halfdb <= -36)
		return (19 + (halfdb/8))*2;
	else if (halfdb <= -18)
		return (28 + (halfdb/4))*2;
	else if (halfdb <= -6)
		return (37 + (halfdb/2))*2;
	else
		return (43 + halfdb)*2;
}

void VMeter::setClipThreshold(int threshold)
{
	int t = jlimit((int)0,(int)0xffff, (int)threshold);
	m_clip_threshold = linToPixel(t);
}

void VMeter::setPeakHoldEnabled(bool is_enabled)
{
	m_hold_enabled = is_enabled;
}

void VMeter::setSaturated(bool is_saturated)
{
	if (is_saturated)
	{
		m_over = 80;
	}
}

void VMeter::setPeak (int v)
{
	int over = m_over;
	int cur = m_cur;
	int hold = m_hold;
	int x = 1;
	int w = getWidth()-2;

	if (m_hold_enabled)
	{
		if (m_hold < m_cur)
		{
			m_hold = m_cur;
			m_delay = 10;
		}

		if (m_delay <= 0)
		{
			if (m_hold > 0)
			{
				m_hold--;
			}
		}
		else
		{
			m_delay--;
		}
	}

	if (over)
	{
		over-=2;
	}
	if (cur) cur-=3;
	
	int new_cur = linToPixel (v);
	if (new_cur > cur) cur = new_cur;	
	
	if (0 != m_clip_threshold)
	{
		if (new_cur >= m_clip_threshold) over = 80;
	}

	if (over != m_over)
	{
		m_over = over;
		repaint (x, 0, w, 4);
	}
	if (cur != m_cur)
	{
		int y0 = METER_HEIGHT-2*m_cur; //first lit bar from top
		int y1 = METER_HEIGHT-2*cur; //first lit bar from top
		m_cur = cur;
		if (y0<y1)
			repaint(x, y0, w, y1-y0+2);
		else
			repaint(x, y1, w, y0-y1+2);
	}

	if (m_hold_enabled && (hold != m_hold))
	{
		int y0 = METER_HEIGHT-2*m_hold;
		int y1 = METER_HEIGHT-2*hold;
		if (y0>y1)
			repaint(x, y1-2, w, y0-y1+4);
	}
}

void VMeter::paint (Graphics& g)
{
	//Check which part is intersected
	int x = 1;
	int w = getWidth()-2;

	//update overload
	Colour cover((uint8)0xff,0,0,(float)m_over/40.0f);
	g.setColour (Colour(0x50,0x50,0x50).overlaidWith (cover)); // fade toward black
	g.fillRect (Rectangle<int>(x,0,w,4));

	// paint the non-lit bars
	int y0 = METER_HEIGHT-2*m_cur; //first lit bar from top
	if (y0 > 8)
	{
		g.drawImage(m_meter_bg_img, x,8,w,y0-8, x,8,w,y0-8);
	}	

	// paint the peak hold bar
	if (m_hold_enabled)
	{
		int y_hold = METER_HEIGHT-2*m_hold;
		g.drawImage(m_meter_img, x,y_hold+1,w,1, x,y_hold+1,w,1);
	}

	// paint the lit bars
	int y1 = METER_HEIGHT-2*m_cur; //first lit bar from top
	{
		g.drawImage(m_meter_img, x,y1,w,METER_HEIGHT-y1, x,y1,w,METER_HEIGHT-y1);
	}

	// paint the zero crossing
	g.setColour (Colour(0,0,0x50));
	g.drawLine(0, 56.0f, (float)w+2, 56.0f, 0.5f);
}
