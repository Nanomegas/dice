///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2007 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
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
#include "Listener.h"
#include "BusWindow.h"
#include "BusStatsComponent.h"
#include "tcat_dice_pal_device.h"
#include "juce_TCATLookAndFeel.h"
#include "appDefaults.h"

//==============================================================================
// this component displays a moving graph of DPC values
DPCGraph::DPCGraph() :
	m_curVal (0),
	m_xVal (0),
	m_clear (false),
	m_enabled (false)
{
	clear();
}

DPCGraph::~DPCGraph()
{
}

void DPCGraph::paint(Graphics &g)
{
	if (!m_enabled)
	{
        g.setColour (Colours::black);
		g.fillAll(Colours::black);
		g.setColour (Colours::white);
		g.drawFittedText("driver not active\nor no devices found", 10, 10, getWidth()-20, getHeight()-20, Justification::centred, 2);
		return;
	}

	if (m_clear)
	{
        g.setColour (Colours::black);
		g.fillAll(Colours::black);
		m_clear = false;
	}
	else
	{
		int h = getHeight()-3;
		float d = h/15000.0f;

		for (int i=0; i<m_values.size(); i++)
		{
			float x = (float)i;
			float tempY = (float)m_values[i]*d;
			if (tempY < d)
			{
				tempY = d;
			}
			float dotY = h-(float)m_values[i]*d;
			g.setColour(Colours::black);
			g.drawLine(x, 0, x+1, (float)getHeight(), 3);
			g.setColour(Colours::chartreuse);
			if (0xffffffff == m_values[i])
			{
				g.setColour(Colours::white);
				g.drawLine(x, 0, x, (float)getHeight(), 1);
			}
			else if (0 == m_values[i])
			{
				g.setColour(Colours::black);
				g.drawLine(x, 0, x, (float)getHeight(), 1);
			}
			else
			{
				if (m_values[i]>13000)
				{
					g.setColour(Colours::red);
				}
				else if (m_values[i]>6300)
				{
					g.setColour(Colours::orange);
				}
				else if (m_values[i]>1300)
				{
					g.setColour(Colours::yellow);
				}
				g.drawLine(x, dotY, x, dotY+2, 3);
			}
		}
	}
}

void DPCGraph::resized()
{
	m_values.ensureStorageAllocated(getWidth());
}

void DPCGraph::updateValue(uint32 value)
{
	m_curVal = value;
	if (m_curVal > 14000)
	{
		m_curVal = 14000;
	}
	if (m_curVal < 50)
	{
		m_curVal = 50;
	}

	m_values.set(m_xVal, m_curVal);

	m_xVal++;
	if (m_xVal>getWidth())
	{
		m_xVal = 0;
	}
	if (m_xVal != getWidth())
	{
		m_values.set(m_xVal, 0xffffffff);
	}
	repaint(m_xVal-2, 0, 3, getHeight());
}

void DPCGraph::clear()
{
	m_xVal = 0;
	m_curVal = 0;
	m_clear = true;
	for (int i=0; i<getWidth(); i++)
	{
		m_values.set(i, 0);
	}
	m_values.set(1, 0xffffffff);
	repaint();
}

void DPCGraph::enableGraph(bool bEnable)
{
	m_enabled = bEnable;
	clear();
}

//==============================================================================
// this component contains the subcomponents that display DPC info
BusStatsComponent::BusStatsComponent () : m_maxVal (0), m_update (0)
{
	memset(&m_stats, 0, sizeof(tcat::dice::ioctl::BUS_DPC_STATS));

	setName ("Bus Stats");

	addAndMakeVisible (m_clear = new TextButton("Clear", "Clear stats"));
	m_clear->addListener(this);

	addAndMakeVisible(m_graph = new DPCGraph());

	addAndMakeVisible(m_update_rate = new ComboBox("update rate"));
	m_update_rate->addItem("500 ms", 500);
	m_update_rate->addItem("1 s", 1000);
	m_update_rate->addItem("2 s", 2000);
	m_update_rate->addItem("3 s", 3000);
	m_update_rate->addListener(this);
	m_update_rate->setSelectedId(1000, true);

	addAndMakeVisible (m_update_rate_l = new Label( String::empty, "Update Rate"));
	m_update_rate_l->setFont(Font(13));
	m_update_rate_l->setJustificationType(Justification::left);

	addAndMakeVisible(m_cur_val_l = new Label(String::empty, "Current DPC: - n/a -"));
	m_cur_val_l->setFont(Font(13));
	m_cur_val_l->setJustificationType(Justification::left);

	addAndMakeVisible (m_maxVal_l = new Label( String::empty, "Max DPC: - n/a -"));
	m_maxVal_l->setFont(Font(13));
	m_maxVal_l->setJustificationType(Justification::left);

	addAndMakeVisible (m_safe_mode_l = new Label( String::empty, "Recommended Operation Mode:   - n/a -"));
	m_safe_mode_l->setFont(Font(13));
	m_safe_mode_l->setJustificationType(Justification::left);

	Font labelFont(11);
	for (int i=0; i<22; i++)
	{
		m_count_l.add(new Label());
		m_count_l[i]->setFont(labelFont);
		addAndMakeVisible(m_count_l[i]);
		m_count.add(new Label(String::empty, "0"));
		m_count[i]->setFont(labelFont);
		addAndMakeVisible(m_count[i]);
		m_values[i] = 0;
	}
	m_count_l[0]->setText("< 100 us", false);
	m_count_l[1]->setText("< 200", false);
	m_count_l[2]->setText("< 300", false);
	m_count_l[3]->setText("< 400", false);
	m_count_l[4]->setText("< 500", false);
	m_count_l[5]->setText("< 600", false);
	m_count_l[6]->setText("< 700", false);
	m_count_l[7]->setText("< 800", false);
	m_count_l[8]->setText("< 900", false);
	m_count_l[9]->setText("< 1000", false);
	m_count_l[10]->setText("< 1200", false);
	m_count_l[11]->setText("< 1400", false);
	m_count_l[12]->setText("< 1600", false);
	m_count_l[13]->setText("< 1800", false);
	m_count_l[14]->setText("< 2000", false);
	m_count_l[15]->setText("< 3000", false);
	m_count_l[16]->setText("< 4000", false);
	m_count_l[17]->setText("< 5000", false);
	m_count_l[18]->setText("< 10000", false);
	m_count_l[19]->setText("< 20000", false);
	m_count_l[20]->setText("< 30000", false);
	m_count_l[21]->setText("30000+", false);

	m_graph->clear();

	lookAndFeelChanged();
}
	
BusStatsComponent::~BusStatsComponent() 
{
   stopTimer();
}

void BusStatsComponent::resized()
{
	m_clear->setBounds			(10, 11, 80, 24);
	m_update_rate_l->setBounds	(m_clear->getRight()+10, 0, 80, 18);
	m_update_rate->setBounds	(m_update_rate_l->getX(), m_update_rate_l->getBottom()+2, m_update_rate_l->getWidth(), 20);
	m_graph->setBounds			(m_update_rate->getRight()+10, 1, 200, m_update_rate->getBottom());
	m_cur_val_l->setBounds		(m_graph->getRight()+5, 1, 150, 14);
	m_maxVal_l->setBounds		(m_cur_val_l->getRight()+10, 1, 150, 14);
	m_safe_mode_l->setBounds	(m_graph->getRight()+5, 15, getWidth()-m_graph->getRight()-10, 28);

	int x = 8;
	int y = m_graph->getBottom()+1;
	int w = 55;
	int h = 12;
	for (int i=0; i<22; i++)
	{
		if (11 == i)
		{
			y += h+h+1;
			x = 8;
		}

		m_count_l[i]->setBounds(x, y, w, h);
		m_count[i]->setBounds(x, y+13, w, h);

		m_count_l[i]->setJustificationType(Justification::centredTop);
		m_count[i]->setJustificationType(Justification::centredTop);
		m_count[i]->setText("0", false);

		x += w+5;
	}
	updateInfo();
}

void BusStatsComponent::paint(Graphics &g)
{
	g.setColour(findColour(TextEditor::backgroundColourId));
	g.drawRect(m_graph->getX()-1, m_graph->getY()-1, m_graph->getWidth()+2, m_graph->getHeight()+2); 
}

void BusStatsComponent::broughtToFront()
{
	lookAndFeelChanged();
	m_graph->repaint();
}

void BusStatsComponent::lookAndFeelChanged()
{
	Colour backgroundColour = findColour(TCATLookAndFeel::tcatGlobalTabAreaColourId);

	repaint();

	m_update_rate_l->setColour (Label::backgroundColourId, backgroundColour);
	m_update_rate_l->setColour (Label::textColourId, backgroundColour.contrasting());
	m_cur_val_l->setColour (Label::backgroundColourId, backgroundColour);
	m_cur_val_l->setColour (Label::textColourId, backgroundColour.contrasting());
	m_maxVal_l->setColour (Label::backgroundColourId, backgroundColour);
	m_maxVal_l->setColour (Label::textColourId, backgroundColour.contrasting());
	m_safe_mode_l->setColour (Label::backgroundColourId, backgroundColour);
	m_safe_mode_l->setColour (Label::textColourId, backgroundColour.contrasting());
	for (int i=0; i<22; i++)
	{
		m_count_l[i]->setColour (Label::backgroundColourId, backgroundColour);
		m_count_l[i]->setColour (Label::textColourId, backgroundColour.contrasting());
		m_count[i]->setColour (Label::backgroundColourId, Colours::black);
		m_count[i]->setColour (Label::textColourId, Colours::lightgrey);
	}
}

void BusStatsComponent::comboBoxChanged(ComboBox* comboBoxThatHasChanged)
{
	stopTimer();
	startTimer(m_update_rate->getSelectedId());
}

void BusStatsComponent::buttonClicked (Button* buttonThatWasClicked)
{
	if (m_bus)
	{
		m_bus->dpc_stats_clear();
		m_maxVal = 0;
		m_graph->clear();
	}
}

void BusStatsComponent::timerCallback()
{
	updateInfo();
}

void BusStatsComponent::handleAsyncUpdate()
{
	if (m_update & updt_mount)
	{
		enableStats(true);
		m_update &= ~updt_mount;
	}
	else if (m_update & updt_unmount)
	{
		m_update &= ~updt_unmount;
		enableStats(false);
	}
}

enum opModesMaxDpcUs
{
	maxDpcUs_LowLatency =  300,
	maxDpcUs_Normal     = 1300,
	maxDpcUs_Safe1      = 2600,
	maxDpcUs_Safe2      = 5300,
};

static const uint32 m_dpcRange[] = {    99,   199,  299,  399,  499,
							  599,   699,  799,  899,  999,
							 1999,  1399, 1599, 1799, 1999,
							 2999,  3999, 4999, 9999, 19999,
							29999, 30000 };

void BusStatsComponent::updateInfo()
{
	if (m_bus)
	{
		try
		{
			uint32 curDpcIndex = 0;
			m_bus->dpc_stats(m_stats);
			tcat::uint32 * count = &m_stats.delayCount_0to99;
			for (int i=0; i<22; i++)
			{
				if (*count > m_values[i])
				{
					curDpcIndex = i;
				}
				m_values[i] = *count;
				m_count[i]->setText(String::formatted("%d", *count), false);
				count++;
			}
			uint32 highVal = m_dpcRange[curDpcIndex];
			uint32 lowVal = 0;
			if (curDpcIndex>0)
			{
				lowVal = m_dpcRange[curDpcIndex-1]+1;
			}
			m_graph->updateValue(highVal);
			if (highVal>=30000)
			{
				m_cur_val_l->setText(String::formatted("Current DPC: 30000+ us", lowVal, highVal ), false);
			}
			else
			{
				m_cur_val_l->setText(String::formatted("Current DPC: %d-%d us", lowVal, highVal ), false);
			}

			if (m_stats.maxDriverDpcDelayUs > m_maxVal)
			{
				m_maxVal = m_stats.maxDriverDpcDelayUs;
				m_maxVal_l->setText(String::formatted("Max DPC: %d us", m_maxVal), false);
				if (m_maxVal < maxDpcUs_LowLatency)
				{
					m_safe_mode_l->setText("Recommended Operation Mode:   Lowest Latency", false);
				}
				else if (m_maxVal < maxDpcUs_Normal)
				{
					m_safe_mode_l->setText("Recommended Operation Mode:   Normal Mode", false);
				}
				else if (m_maxVal < maxDpcUs_Safe1)
				{
					m_safe_mode_l->setText("Recommended Operation Mode:   Safe Mode 1", false);
				}
				else
				{
					m_safe_mode_l->setText("Recommended Operation Mode:   Safe Mode 2", false);
				}
			}
		}
		catch (...)
		{
		}
	}
}

void BusStatsComponent::enableStats(bool bShouldBeEnabled)
{
	m_maxVal = 0;
	m_graph->enableGraph(bShouldBeEnabled);

	if (bShouldBeEnabled)
	{
		startTimer(m_update_rate->getSelectedId());
	}
	else
	{
		stopTimer();
		m_cur_val_l->setText("Current DPC: - n/a -", false);
		m_maxVal_l->setText("Max DPC: - n/a -", false);
		m_safe_mode_l->setText("Recommended Operation Mode:   - n/a -", false);
		for (int i=0; i<22; i++)
		{
			m_count[i]->setText(String::empty, false);
		}
	}
}

void BusStatsComponent::mount(tcat::dice::reference<event_bus> bus)
{
	// call super's mount() first
	bus_listener::mount(bus);
	// which gives us an m_bus reference

	m_update |= updt_mount;
	triggerAsyncUpdate();		// notify the gui thread
}

void BusStatsComponent::unmount()
{
	// call super's unmount first
	bus_listener::unmount();

	m_update |= updt_unmount;
	triggerAsyncUpdate();		// notify the gui thread
}

