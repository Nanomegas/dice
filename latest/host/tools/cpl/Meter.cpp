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

#include "juce.h"
#include "Meter.h"



Meter::Meter ()
   : m_cur(0),
	 m_iyellow(13),
	 m_iorange(63),
	 m_ired(130),
	 m_peakhold(0)
{
	int h = 20;

    setColour (backgroundColourId,  Colours::white);
    setOpaque(true);
    
	// shiny meter, black area
    m_image_b = new Image (Image::RGB, 1, h, true);
	Graphics gB (*m_image_b);
	GradientBrush gbB (Colours::black.brighter (1.5f),
					  0.0f, 0.0f,
					  Colours::black.brighter (0.1f),
					  0.0f, (float) h/2, false);
	gB.setBrush (&gbB);
	gB.fillRect(0, 0, 1, (int)(h/3));
    gB.setColour (Colours::black);
    gB.fillRect (0, h/3, 1, (int)(h));
    GradientBrush gbB2 (Colours::black.brighter (0.1f),
                      0.0f, (float)(h - h/4),
                      Colours::black.brighter (0.7f),
                      0.0f, (float) h, false);
    gB.setBrush (&gbB2);
    gB.fillRect (0, h - h/4, 1, (int)(h/4));
    m_image_brush_b = new ImageBrush(m_image_b, 0, 0, 1.0f);
    
	// shiny meter, green area
    m_image_g = new Image (Image::RGB, 1, h, true);
	Graphics gG (*m_image_g);
	GradientBrush gbG (Colour(0x0,0xff,0x0).brighter (1.5f),
					  0.0f, 0.0f,
					  Colour(0x0,0xff,0x0).brighter (0.1f),
					  0.0f, (float) h/2, false);
	gG.setBrush (&gbG);
	gG.fillRect(0, 0, 1, (int)(h/3));
    gG.setColour (Colour(0x0,0xff,0x0));
    gG.fillRect (0, h/3, 1, (int)(h));
    GradientBrush gbG2 (Colour(0x0,0xff,0x0).brighter (0.1f),
                      0.0f, (float)(h - h/4),
                      Colour(0x0,0xff,0x0).brighter (0.7f),
                      0.0f, (float) h, false);
    gG.setBrush (&gbG2);
    gG.fillRect (0, h - h/4, 1, (int)(h/4));
    m_image_brush_g = new ImageBrush(m_image_g, 0, 0, 1.0f);
    
	// shiny meter, yellow area
    m_image_y = new Image (Image::RGB, 1, h, true);
	Graphics gY (*m_image_y);
	GradientBrush gbY (Colour(0xff,0xff,0x0).brighter (1.5f),
					  0.0f, 0.0f,
					  Colour(0xff,0xff,0x0).brighter (0.1f),
					  0.0f, (float) h/2, false);
	gY.setBrush (&gbY);
	gY.fillRect(0, 0, 1, (int)(h/3));
    gY.setColour (Colour(0xff,0xff,0x0));
    gY.fillRect (0, h/3, 1, (int)(h));
    GradientBrush gbY2 (Colour(0xff,0xff,0x0).brighter (0.1f),
                      0.0f, (float)(h - h/4),
                      Colour(0xff,0xff,0x0).brighter (0.7f),
                      0.0f, (float) h, false);
    gY.setBrush (&gbY2);
    gY.fillRect (0, h - h/4, 1, (int)(h/4));
	m_image_brush_y = new ImageBrush(m_image_y, 0, 0, 1.0f);
	
	// shiny meter, orange area
    m_image_o = new Image (Image::RGB, 1, h, true);
	Graphics gO (*m_image_o);
	GradientBrush gbO (Colour(0xff,0x7f,0x0).brighter (1.5f),
					  0.0f, 0.0f,
					  Colour(0xff,0x7f,0x0).brighter (0.1f),
					  0.0f, (float) h/2, false);
	gO.setBrush (&gbO);
	gO.fillRect(0, 0, 1, (int)(h/3));
    gO.setColour (Colour(0xff,0x7f,0x0));
    gO.fillRect (0, h/3, 1, (int)(h));
    GradientBrush gbO2 (Colour(0xff,0x7f,0x0).brighter (0.1f),
                      0.0f, (float)(h - h/4),
                      Colour(0xff,0x7f,0x0).brighter (0.7f),
                      0.0f, (float) h, false);
    gO.setBrush (&gbO2);
    gO.fillRect (0, h - h/4, 1, (int)(h/4));
    m_image_brush_o = new ImageBrush(m_image_o, 0, 0, 1.0f);
    
	// shiny meter, red area
    m_image_r = new Image (Image::RGB, 1, h, true);
	Graphics gR (*m_image_r);
	GradientBrush gbR (Colour(0xff,0x00,0x0).brighter (1.5f),
					  0.0f, 0.0f,
					  Colour(0xff,0x00,0x0).brighter (0.1f),
					  0.0f, (float) h/2, false);
	gR.setBrush (&gbR);
	gR.fillRect(0, 0, 1, (int)(h/3));
    gR.setColour (Colour(0xff,0x00,0x0));
    gR.fillRect (0, h/3, 1, (int)(h));
    GradientBrush gbR2 (Colour(0xff,0x00,0x0).brighter (0.1f),
                      0.0f, (float)(h - h/4),
                      Colour(0xff,0x00,0x0).brighter (0.7f),
                      0.0f, (float) h, false);
    gR.setBrush (&gbR2);
    gR.fillRect (0, h - h/4, 1, (int)(h/4));
    m_image_brush_r = new ImageBrush(m_image_r, 0, 0, 1.0f);
}

Meter::~Meter ()
{
	delete m_image_brush_b;
	delete m_image_brush_g;
	delete m_image_brush_y;
	delete m_image_brush_o;
	delete m_image_brush_r;
	delete m_image_b;
	delete m_image_g;
	delete m_image_y;
	delete m_image_o;
	delete m_image_r;
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
	if (m_max > 0)
	{
		m_iyellow = ((m_vyellow*getWidth())/m_max);
		m_iorange = ((m_vorange*getWidth())/m_max);
		m_ired  = ((m_vred*getWidth())/m_max);
	}
}

void Meter::setValue (int v)
{		
	int x0,x1;
	int new_cur = m_cur;
	
	int w = getWidth();
	int h = getHeight();
	
	if (v > m_max) v = m_max;
	v = (v*w)/m_max;
	
	if (v > m_peakhold) m_peakhold = v;
	if (new_cur > m_fallback)
		new_cur -=m_fallback;
	else
		new_cur = 0;
	if (v > new_cur) new_cur = v;
	
	if (new_cur == m_cur) return;
	
	// set up draw region for difference
	if (new_cur > m_cur)
	{		
		x0 = m_cur+1;
		x1 = new_cur-1;
	}
	else
	{		
		x0 = new_cur+1;
		x1 = m_cur-1;
	}
	m_cur = new_cur;
	repaint (x0, 0, x1, h);
	
}

void Meter::reset()
{
	m_peakhold = 0;
	m_cur = 0;
	repaint ();
}
/*
The overload bar is 4 pixels wide
Then there is a 2 pixel space
Then every second pixel is a bar (1..42)
The whole thing is 4+2+2*42 = 90

*/

#define min(a,b) (((a) < (b))?(a):(b))

void Meter::paint (Graphics& g)
{
    //Check which part is intersected
//    g.drawRect (x, y, w, h);
    int x = getX();
    int y = getY();
    int w = getWidth();
    int h = getHeight();
	int i;

	i=0;
	if (m_cur > 0)
	{
		//fill green up to min(i,m_iyellow)
		g.setBrush(m_image_brush_g);
//		g.setColour (Colour(0x0,0xff,0x0));
		i = min(m_cur,m_iyellow);
		
		g.fillRect (0, 0, i, h);
	}
	if (m_cur > m_iyellow)
	{
		//fill yellow up to min(i,m_iorange)
		g.setBrush(m_image_brush_y);
//		g.setColour (Colour(0xff,0xff,0x0));
		i = min(m_cur,m_iorange);
		g.fillRect (m_iyellow, 0, i, h);
	}
	if (m_cur >= m_iorange)
	{
		//fill orange up to min(i,m_ired)
		g.setBrush(m_image_brush_o);
//		g.setColour (Colour(0xff,0x7f,0x0));
		i = min(m_cur,m_ired);
		g.fillRect (m_iorange, 0, i, h);
	}
	if (m_cur >= m_ired)
	{
		//fill red up to min(i,getHeight()+1)
		g.setBrush(m_image_brush_r);
		// g.setColour (Colour(0xff,0x00,0x0));
		i = min(m_cur,w+1);
		g.fillRect (m_ired, 0, i, h);
	}
    //update back ground Colour
	g.setBrush(m_image_brush_b);
//    g.setColour (findColour (backgroundColourId));    

 	if (i < w)
	{
		//fill bg for the rest
		g.fillRect (i, 0, w-i, h);
		if (m_peakhold && (m_peakhold>=i))
		{
			if (m_peakhold > m_ired)
			{
				g.setBrush(m_image_brush_r);
				//g.setColour (Colour(0xff,0x00,0x0));
			}
			else if (m_peakhold > m_iorange)
			{
				g.setBrush(m_image_brush_o);
				//g.setColour (Colour(0xff,0x7f,0x0));
			}
			else  if (m_peakhold > m_iyellow)
			{
				g.setBrush(m_image_brush_y);
				//g.setColour (Colour(0xff,0xff,0x0));
			}
			else
			{
				g.setBrush(m_image_brush_g);
				//g.setColour (Colour(0x0,0xff,0x0));
			}
			g.fillRect(m_peakhold, 0, 1, h);
		}
	}
}
