///////////////////////////////////////////////////////////////////////////////
//
// The contents of this file are derived from the file juce_LookAndFeel.cpp, in
// the JUCE library. The JUCE copyright is included below.
//
///////////////////////////////////////////////////////////////////////////////

/*
 ==============================================================================
 
 This file is part of the JUCE library - "Jules' Utility Class Extensions"
 Copyright 2004-7 by Raw Material Software ltd.
 
 ------------------------------------------------------------------------------
 
 JUCE can be redistributed and/or modified under the terms of the
 GNU General Public License, as published by the Free Software Foundation;
 either version 2 of the License, or (at your option) any later version.
 
 JUCE is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with JUCE; if not, visit www.gnu.org/licenses or write to the
 Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
 Boston, MA 02111-1307 USA
 
 ------------------------------------------------------------------------------
 
 If you'd like to release a closed-source product which uses JUCE, commercial
 licenses are also available: visit www.rawmaterialsoftware.com/juce for
 more information.
 
 ==============================================================================
*/

#include "juce_TCATLookAndFeel.h"
#include "appDefaults.h"
#include "tcat_dice_pal_system.h"
#include "EventLogger.h"

//==============================================================================
juce_ImplementSingleton (TCATLookAndFeel)

//==============================================================================
TCATLookAndFeel::TCATLookAndFeel() :
	m_use_grey (true)
{
	initColours();
}

TCATLookAndFeel::~TCATLookAndFeel()
{
}

void TCATLookAndFeel::initColours()
{
	// look in default file to see if we're going to 
	//  generate the color scheme or use the specified values
	
	if (appDefaults->getAppSettings()->getBoolValue("UseBlendedColours"))
	{
		setAppDefaultBlendedColours();
	}
	else if (appDefaults->getAppSettings()->getBoolValue("UseGeneratedColourShades"))
	{
		setAppDefaultShadedColours();
	}
	else
	{
		setAppDefaultSpecifiedColours();
	}
	
	// coordinate the standard control colours with the ones we have so far
	setColour (PopupMenu::backgroundColourId,				findColour(TCATLookAndFeel::tcatPopupMenuBackgroundColourId));
	setColour (PopupMenu::highlightedTextColourId,			findColour(TCATLookAndFeel::tcatPopupMenuBackgroundColourId).contrasting());
	setColour (PopupMenu::highlightedBackgroundColourId,	findColour(TCATLookAndFeel::tcatPopupMenuBackgroundColourId).darker());
	setColour (TextEditor::backgroundColourId,				findColour(TCATLookAndFeel::tcatTextboxBackgroundColourId));
	setColour (TextEditor::outlineColourId,					findColour(TCATLookAndFeel::tcatTextboxOutlineColourId));
	setColour (TextEditor::focusedOutlineColourId,			findColour(TCATLookAndFeel::tcatTextboxOutlineColourId).darker());
	setColour (ComboBox::buttonColourId,					findColour(TCATLookAndFeel::tcatComboBackgroundColourId));
	setColour (ComboBox::backgroundColourId,				findColour(TCATLookAndFeel::tcatComboBackgroundColourId));
	setColour (ComboBox::outlineColourId,					findColour(TCATLookAndFeel::tcatComboOutlineColourId));
	setColour (TextButton::buttonColourId,					findColour(TCATLookAndFeel::tcatTextButtonColourId));
	setColour (ListBox::backgroundColourId,					findColour(TCATLookAndFeel::tcatListboxBackgroundColourId));
	setColour (ListBox::outlineColourId,					findColour(TCATLookAndFeel::tcatListboxOutlineColourId));
	setColour (GroupComponent::outlineColourId,				findColour(TCATLookAndFeel::tcatGroupOutlineColourId));
	setColour (TooltipWindow::backgroundColourId,			findColour(TCATLookAndFeel::tcatToolTipBackgroundColourId));
	setColour (TooltipWindow::outlineColourId,				findColour(TCATLookAndFeel::tcatToolTipOutlineColourId));
	setColour (TooltipWindow::textColourId,					findColour(TCATLookAndFeel::tcatToolTipTextColourId));
	setColour (ProgressBar::backgroundColourId,				findColour(TCATLookAndFeel::tcatProgressBarBackgroundColourId));
	setColour (ProgressBar::foregroundColourId,				findColour(TCATLookAndFeel::tcatProgressBarForegroundColourId));
	setColour (ScrollBar::backgroundColourId,				findColour(TextButton::buttonColourId));
	setColour (ScrollBar::thumbColourId,					findColour(TextButton::buttonColourId).brighter());
	setColour (Slider::thumbColourId,						findColour(TextButton::buttonColourId));
	setColour (Slider::trackColourId,						findColour(TextButton::buttonColourId).brighter()); //Colour (0x7fffffff));
	setColour (TableListBox::outlineColourId,				findColour(TCATLookAndFeel::tcatTextboxOutlineColourId));
	setColour (TableListBox::backgroundColourId,			findColour(TCATLookAndFeel::tcatDeviceTabAreaColourId));
	setColour (AlertWindow::backgroundColourId,				findColour(TCATLookAndFeel::tcatPopupWindowBackgroundColourId));
	setColour (AlertWindow::textColourId,					findColour(TCATLookAndFeel::tcatPopupWindowBackgroundColourId).contrasting());
	setColour (AlertWindow::outlineColourId,				findColour(TCATLookAndFeel::tcatPopupWindowBackgroundColourId).contrasting());
}

void TCATLookAndFeel::dumpColours()
{
	String eventStr = String::empty;
	
	EventLogger::getInstance()->logMessage("Dumping current color settings:");
	
	eventStr << "<VALUE name=\""  
	<< "TitleBar\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatTitleBarColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "TitleBarButton\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatTitleBarButtonColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "WindowBackground\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatWindowBackgroundColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "PopupTitleBar\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatPopupTitleBarColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "PopupWindowBackground\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatPopupWindowBackgroundColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "GlobalAreaBackground\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatGlobalAreaBackgroundColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "GlobalHeading\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatGlobalHeadingColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "GlobalTabArea\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatGlobalTabAreaColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "GlobalTabButton\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatGlobalTabButtonColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "DeviceAreaBackground\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatDeviceAreaBackgroundColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "DeviceHeading\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatDeviceHeadingColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "DevicesHeading\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatDevicesHeadingColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "DeviceTabArea\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatDeviceTabAreaColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "DeviceTabButton\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatDeviceTabButtonColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "DeviceAreaBackground\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatDeviceAreaBackgroundColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "DeviceBackground\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatDeviceBackgroundColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "DeviceOutline\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatDeviceOutlineColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "PopupMenuBackground\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatPopupMenuBackgroundColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "TextboxBackground\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatTextboxBackgroundColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "TextboxOutline\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatTextboxOutlineColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "ComboBackground\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatComboBackgroundColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "ComboOutline\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatComboOutlineColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "TextButton\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatTextButtonColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "ToggleButton\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatToggleButtonColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "ListboxBackground\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatListboxBackgroundColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "ListboxOutline\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatListboxOutlineColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "GroupOutline\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatGroupOutlineColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "ToolTipBackground\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatToolTipBackgroundColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "ToolTipOutline\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatToolTipOutlineColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "ToolTipText\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatToolTipTextColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "ProgressBarBackground\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatProgressBarBackgroundColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
	
	eventStr = String::empty;
	eventStr << "<VALUE name=\""  
	<< "ProgressBarForeground\" val=\"" 
	<< String::formatted("%06X", findColour (TCATLookAndFeel::tcatProgressBarForegroundColourId).getARGB() & 0xffffff) 
	<< "\"/>";
	EventLogger::getInstance()->logMessage(eventStr);
}

//==============================================================================
static const Colour createBaseColour (const Colour& buttonColour,
                                      const bool hasKeyboardFocus,
                                      const bool isMouseOverButton,
                                      const bool isButtonDown)
{
    float sat = hasKeyboardFocus ? 1.3f : 0.9f;
	
    Colour baseColour (buttonColour.withMultipliedSaturation (sat));
	
    if (isButtonDown)
        return baseColour.contrasting (0.4f);
    else if (isMouseOverButton)
        return baseColour.contrasting (0.2f);
	
    return baseColour;
}

Colour TCATLookAndFeel::getAppDefaultColour(const String &colourStr)
{
	return Colour(0xff000000 | appDefaults->getAppSettings()->getValue(colourStr).getHexValue32());
}

void TCATLookAndFeel::setAppDefaultSpecifiedColours(void)
{
	setColour (TCATLookAndFeel::tcatTitleBarColourId, getAppDefaultColour("TitleBar"));
	setColour (TCATLookAndFeel::tcatTitleBarButtonColourId, getAppDefaultColour("TitleBarButton"));
	setColour (TCATLookAndFeel::tcatWindowBackgroundColourId, getAppDefaultColour("WindowBackground"));
	setColour (TCATLookAndFeel::tcatPopupTitleBarColourId, getAppDefaultColour("PopupTitleBar"));
	setColour (TCATLookAndFeel::tcatPopupWindowBackgroundColourId, getAppDefaultColour("PopupWindowBackground"));
	setColour (TCATLookAndFeel::tcatGlobalAreaBackgroundColourId, getAppDefaultColour("GlobalAreaBackground"));
	setColour (TCATLookAndFeel::tcatGlobalHeadingColourId, getAppDefaultColour("GlobalHeading"));
	setColour (TCATLookAndFeel::tcatGlobalTabAreaColourId, getAppDefaultColour("GlobalTabArea"));
	setColour (TCATLookAndFeel::tcatGlobalTabButtonColourId, getAppDefaultColour("GlobalTabButton"));
	setColour (TCATLookAndFeel::tcatDeviceAreaBackgroundColourId, getAppDefaultColour("DeviceAreaBackground"));
	setColour (TCATLookAndFeel::tcatDeviceHeadingColourId, getAppDefaultColour("DeviceHeading"));
	setColour (TCATLookAndFeel::tcatDevicesHeadingColourId, getAppDefaultColour("DevicesHeading"));
	setColour (TCATLookAndFeel::tcatDeviceTabAreaColourId, getAppDefaultColour("DeviceTabArea"));
	setColour (TCATLookAndFeel::tcatDeviceTabButtonColourId, getAppDefaultColour("DeviceTabButton"));
	setColour (TCATLookAndFeel::tcatDeviceAreaBackgroundColourId, getAppDefaultColour("DeviceAreaBackground"));
	setColour (TCATLookAndFeel::tcatDeviceBackgroundColourId, getAppDefaultColour("DeviceBackground"));
	setColour (TCATLookAndFeel::tcatDeviceOutlineColourId, getAppDefaultColour("DeviceOutline"));
	setColour (TCATLookAndFeel::tcatPopupMenuBackgroundColourId, getAppDefaultColour("PopupMenuBackground"));	
	setColour (TCATLookAndFeel::tcatTextboxBackgroundColourId, getAppDefaultColour("TextboxBackground"));
	setColour (TCATLookAndFeel::tcatTextboxOutlineColourId, getAppDefaultColour("TextboxOutline"));
	setColour (TCATLookAndFeel::tcatComboBackgroundColourId, getAppDefaultColour("ComboBackground"));
	setColour (TCATLookAndFeel::tcatComboOutlineColourId, getAppDefaultColour("ComboOutline"));
	setColour (TCATLookAndFeel::tcatTextButtonColourId, getAppDefaultColour("TextButton"));
	setColour (TCATLookAndFeel::tcatToggleButtonColourId, getAppDefaultColour("ToggleButton"));
	setColour (TCATLookAndFeel::tcatListboxBackgroundColourId, getAppDefaultColour("ListboxBackground"));
	setColour (TCATLookAndFeel::tcatListboxOutlineColourId, getAppDefaultColour("ListboxOutline"));
	setColour (TCATLookAndFeel::tcatGroupOutlineColourId, getAppDefaultColour("GroupOutline"));
	setColour (TCATLookAndFeel::tcatToolTipBackgroundColourId, getAppDefaultColour("ToolTipBackground"));
	setColour (TCATLookAndFeel::tcatToolTipOutlineColourId, getAppDefaultColour("ToolTipOutline"));
	setColour (TCATLookAndFeel::tcatToolTipTextColourId, getAppDefaultColour("ToolTipText"));
	setColour (TCATLookAndFeel::tcatProgressBarBackgroundColourId, getAppDefaultColour("ProgressBarBackground"));
 	setColour (TCATLookAndFeel::tcatProgressBarForegroundColourId, getAppDefaultColour("ProgressBarForeground"));
}

void TCATLookAndFeel::stepShade(int nSteps)
{
	float h, s, b;
	m_shade.getHSB(h, s, b);
	
	if (m_use_grey)
	{
		b += (float)(0.05f * nSteps);
		m_shade = m_shade.greyLevel(b);
	}
	else
	{
		s += (0.010f * (float)nSteps);
		b += (float)(0.10f * nSteps);
		m_shade = m_shade.withBrightness(b);
		m_shade = m_shade.withSaturation(s);
		m_shade = m_shade.withHue(h);
	}
}

void TCATLookAndFeel::setAppDefaultShadedColours(void)
{
	uint32 h = (uint32)appDefaults->getAppSettings()->getIntValue("BaseColourHue");
	uint8 s = (uint8)jmin(appDefaults->getAppSettings()->getIntValue("BaseColourSaturation"), 255);
	uint8 v = (uint8)jmin(appDefaults->getAppSettings()->getIntValue("BaseColourValue"), 255);
	
	m_use_grey = (h>360);
	
	if (m_use_grey)
	{
		// change brightness value to make shades
		m_shade = Colour(0.0f, 0.0f, v/255.0f, 1.0f);	// fix saturation to get greys, hue is not relevant
	}
	else
	{
		// change saturation value to make shades
		m_shade = Colour(h/360.0f, s/255.0f, v/255.0f, 1.0f);
	}
	
	setColour (TCATLookAndFeel::tcatTitleBarButtonColourId, m_shade);
	setColour (TCATLookAndFeel::tcatToolTipTextColourId, m_shade);
	
	stepShade(3);
	setColour (TCATLookAndFeel::tcatWindowBackgroundColourId, m_shade);
	setColour (TCATLookAndFeel::tcatToolTipOutlineColourId, m_shade);
	setColour (TCATLookAndFeel::tcatComboOutlineColourId, m_shade);
	setColour (TCATLookAndFeel::tcatTextboxOutlineColourId, m_shade);
	
	stepShade(-4);
	setColour (TCATLookAndFeel::tcatTitleBarColourId, m_shade);
	setColour (TCATLookAndFeel::tcatGlobalAreaBackgroundColourId, m_shade);
	setColour (TCATLookAndFeel::tcatDeviceAreaBackgroundColourId, m_shade);
	setColour (TCATLookAndFeel::tcatListboxOutlineColourId, m_shade);
	
	stepShade(2);
	setColour (TCATLookAndFeel::tcatGlobalHeadingColourId, m_shade);
	setColour (TCATLookAndFeel::tcatDeviceHeadingColourId, m_shade);
	setColour (TCATLookAndFeel::tcatGlobalTabAreaColourId, m_shade);
	setColour (TCATLookAndFeel::tcatDeviceTabAreaColourId, m_shade);
	setColour (TCATLookAndFeel::tcatDeviceBackgroundColourId, m_shade);
	
	stepShade(2);
	setColour (TCATLookAndFeel::tcatDevicesHeadingColourId, m_shade);
	setColour (TCATLookAndFeel::tcatGlobalTabButtonColourId, m_shade);
	setColour (TCATLookAndFeel::tcatDeviceTabButtonColourId, m_shade);
	setColour (TCATLookAndFeel::tcatPopupTitleBarColourId, m_shade);
	setColour (TCATLookAndFeel::tcatPopupWindowBackgroundColourId, m_shade);
	
	
	stepShade(2);
	setColour (TCATLookAndFeel::tcatPopupMenuBackgroundColourId, m_shade);	
	setColour (TCATLookAndFeel::tcatTextButtonColourId, m_shade);
	setColour (TCATLookAndFeel::tcatTextboxBackgroundColourId, m_shade);
	setColour (TCATLookAndFeel::tcatComboBackgroundColourId, m_shade);
	
	stepShade(2);
	setColour (TCATLookAndFeel::tcatDeviceOutlineColourId, m_shade);
	setColour (TCATLookAndFeel::tcatToggleButtonColourId, m_shade);
	setColour (TCATLookAndFeel::tcatListboxBackgroundColourId, m_shade);
	
	stepShade(2);
	setColour (TCATLookAndFeel::tcatGroupOutlineColourId, m_shade);
	setColour (TCATLookAndFeel::tcatProgressBarBackgroundColourId, m_shade);
 	setColour (TCATLookAndFeel::tcatProgressBarForegroundColourId, m_shade);
	
	stepShade(3);
	setColour (TCATLookAndFeel::tcatToolTipBackgroundColourId, m_shade);
}


void TCATLookAndFeel::setAppDefaultBlendedColours(void)
{
	uint32 val1 = appDefaults->getAppSettings()->getValue("BlendColour1").getHexValue32();
	uint32 val2 = appDefaults->getAppSettings()->getValue("BlendColour2").getHexValue32();
	
	int r1 = (val1 & 0xff0000)>>16;
	int g1 = (val1 & 0xff00)>>8;
	int b1 = val1 & 0xff;
	int r2 = (val2 & 0xff0000)>>16;
	int g2 = (val2 & 0xff00)>>8;
	int b2 = val2 & 0xff;
	
	int rStep = (r1-r2)/19;
	int gStep = (g1-g2)/19;
	int bStep = (b1-b2)/19;
	
	int step=0;
	int rAdd = rStep*step;
	int gAdd = gStep*step;
	int bAdd = bStep*step;
	m_shade = Colour(jmin<uint8>((uint8)(r2+rAdd), 255), jmin<uint8>((uint8)(g2+gAdd), 255), jmin<uint8>((uint8)(b2+bAdd), 255));
	setColour (TCATLookAndFeel::tcatTitleBarColourId, m_shade);
	setColour (TCATLookAndFeel::tcatGlobalAreaBackgroundColourId, m_shade);
	setColour (TCATLookAndFeel::tcatDeviceAreaBackgroundColourId, m_shade);
	setColour (TCATLookAndFeel::tcatListboxOutlineColourId, m_shade);
	
	m_shade = Colour(jmin<uint8>((uint8)(r2+rAdd), 255), jmin<uint8>((uint8)(g2+gAdd), 255), jmin<uint8>((uint8)(b2+bAdd), 255));
	setColour (TCATLookAndFeel::tcatTitleBarButtonColourId, m_shade);
	setColour (TCATLookAndFeel::tcatToolTipTextColourId, m_shade);
	
	step+=3;
	rAdd = rStep*step;
	gAdd = gStep*step;
	bAdd = bStep*step++;
	m_shade = Colour(jmin<uint8>((uint8)(r2+rAdd), 255), jmin<uint8>((uint8)(g2+gAdd), 255), jmin<uint8>((uint8)(b2+bAdd), 255));
	setColour (TCATLookAndFeel::tcatWindowBackgroundColourId, m_shade);
	setColour (TCATLookAndFeel::tcatToolTipOutlineColourId, m_shade);
	setColour (TCATLookAndFeel::tcatComboOutlineColourId, m_shade);
	setColour (TCATLookAndFeel::tcatTextboxOutlineColourId, m_shade);
	
	step += 2;
	rAdd = rStep*step;
	gAdd = gStep*step;
	bAdd = bStep*step;
	m_shade = Colour(jmin<uint8>((uint8)(r2+rAdd), 255), jmin<uint8>((uint8)(g2+gAdd), 255), jmin<uint8>((uint8)(b2+bAdd), 255));
	setColour (TCATLookAndFeel::tcatGlobalHeadingColourId, m_shade);
	setColour (TCATLookAndFeel::tcatDeviceHeadingColourId, m_shade);
	setColour (TCATLookAndFeel::tcatGlobalTabAreaColourId, m_shade);
	setColour (TCATLookAndFeel::tcatDeviceTabAreaColourId, m_shade);
	
	step += 2;
	rAdd = rStep*step;
	gAdd = gStep*step;
	bAdd = bStep*step;
	m_shade = Colour(jmin<uint8>((uint8)(r2+rAdd), 255), jmin<uint8>((uint8)(g2+gAdd), 255), jmin<uint8>((uint8)(b2+bAdd), 255));
	setColour (TCATLookAndFeel::tcatDevicesHeadingColourId, m_shade);
	setColour (TCATLookAndFeel::tcatGlobalTabButtonColourId, m_shade);
	setColour (TCATLookAndFeel::tcatDeviceTabButtonColourId, m_shade);
	setColour (TCATLookAndFeel::tcatPopupTitleBarColourId, m_shade);
	setColour (TCATLookAndFeel::tcatPopupWindowBackgroundColourId, m_shade);
	
	step += 2;
	rAdd = rStep*step;
	gAdd = gStep*step;
	bAdd = bStep*step;
	m_shade = Colour(jmin<uint8>((uint8)(r2+rAdd), 255), jmin<uint8>((uint8)(g2+gAdd), 255), jmin<uint8>((uint8)(b2+bAdd), 255));
	setColour (TCATLookAndFeel::tcatPopupMenuBackgroundColourId, m_shade);	
	setColour (TCATLookAndFeel::tcatTextButtonColourId, m_shade);
	setColour (TCATLookAndFeel::tcatTextboxBackgroundColourId, m_shade);
	setColour (TCATLookAndFeel::tcatComboBackgroundColourId, m_shade);
	setColour (TCATLookAndFeel::tcatListboxBackgroundColourId, m_shade);
	
	step += 2;
	rAdd = rStep*step;
	gAdd = gStep*step;
	bAdd = bStep*step;
	m_shade = Colour(jmin<uint8>((uint8)(r2+rAdd), 255), jmin<uint8>((uint8)(g2+gAdd), 255), jmin<uint8>((uint8)(b2+bAdd), 255));
	setColour (TCATLookAndFeel::tcatDeviceOutlineColourId, m_shade);
	setColour (TCATLookAndFeel::tcatToggleButtonColourId, m_shade);
	
	step += 2;
	rAdd = rStep*step;
	gAdd = gStep*step;
	bAdd = bStep*step;
	m_shade = Colour(jmin<uint8>((uint8)(r2+rAdd), 255), jmin<uint8>((uint8)(g2+gAdd), 255), jmin<uint8>((uint8)(b2+bAdd), 255));
	setColour (TCATLookAndFeel::tcatGroupOutlineColourId, m_shade);
	setColour (TCATLookAndFeel::tcatProgressBarBackgroundColourId, m_shade);
 	setColour (TCATLookAndFeel::tcatProgressBarForegroundColourId, m_shade);
	
	step += 5;
	rAdd = rStep*step;
	gAdd = gStep*step;
	bAdd = bStep*step;
	m_shade = Colour(jmin<uint8>((uint8)(r2+rAdd), 255), jmin<uint8>((uint8)(g2+gAdd), 255), jmin<uint8>((uint8)(b2+bAdd), 255));
	setColour (TCATLookAndFeel::tcatToolTipBackgroundColourId, m_shade);
	setColour (TCATLookAndFeel::tcatDeviceBackgroundColourId, m_shade);
}

//==============================================================================
void TCATLookAndFeel::drawBubble (Graphics& g,
								  float tipX, float tipY,
								  float boxX, float boxY,
								  float boxW, float boxH)
{
    int side = 0;
	
    if (tipX < boxX)
        side = 1;
    else if (tipX > boxX + boxW)
        side = 3;
    else if (tipY > boxY + boxH)
        side = 2;
	
    const float indent = 2.0f;
    Path p;
    p.addBubble (boxX + indent,
                 boxY + indent,
                 boxW - indent * 2.0f,
                 boxH - indent * 2.0f,
                 5.0f,
                 tipX, tipY,
                 side,
                 0.5f,
                 jmin (15.0f, boxW * 0.3f, boxH * 0.3f));
	
    //xxx need to take comp as param for colour
    g.setColour (findColour (TooltipWindow::backgroundColourId).withAlpha (0.9f));
    g.fillPath (p);
	
    //xxx as above
    g.setColour (findColour (TooltipWindow::textColourId).withAlpha (0.4f));
    g.strokePath (p, PathStrokeType (1.33f));
}

//==============================================================================
static const TextLayout layoutTooltipText (const String& text, const Colour& colour) noexcept
{
	const float tooltipFontSize = 14.0f;
	const int maxToolTipWidth = 1000;
	
    AttributedString s;
	s.setJustification (Justification::centred);
    s.append (text, Font (tooltipFontSize, Font::bold), colour);
	
    TextLayout tl;
    tl.createLayoutWithBalancedLineLengths (s, (float) maxToolTipWidth);
	return tl;
}

void TCATLookAndFeel::getTooltipSize (const String& tipText, int& width, int& height)
{
	const TextLayout tl (layoutTooltipText (tipText, Colours::black));
	
	width = (int) (tl.getWidth() + 14.0f);
	height = (int) (tl.getHeight() + 6.0f);
}

void TCATLookAndFeel::drawTooltip (Graphics& g, const String& text, int width, int height)
{
	g.fillAll (findColour (TooltipWindow::backgroundColourId));
	
	const Colour textCol (findColour (TooltipWindow::textColourId));
	
#if ! JUCE_MAC // The mac windows already have a non-optional 1 pix outline, so don't double it here..
	g.setColour (findColour (TooltipWindow::outlineColourId));
	g.drawRect (0, 0, width, height, 1);
#endif
	
    const TextLayout tl (layoutTooltipText (text, findColour (TooltipWindow::textColourId)));
    tl.draw (g, Rectangle<float> ((float) width, (float) height));
}

//==============================================================================
void TCATLookAndFeel::drawTextEditorOutline (Graphics& g, int width, int height, TextEditor& textEditor)
{
    if (textEditor.isEnabled())
    {
        if (textEditor.hasKeyboardFocus (true) && ! textEditor.isReadOnly())
        {
            g.setColour (textEditor.findColour (TextEditor::focusedOutlineColourId));
			g.drawRect(0, 0, width, height, 2);
		}
        else
        {
            g.setColour (textEditor.findColour (TextEditor::outlineColourId));
			g.drawRect(0, 0, width, height, 1);
			// g.drawRoundedRectangle(0.0f, 0.0f, (float)width, (float)height, 4.0f, 4.0f);
		}
    }
}

//==============================================================================
void TCATLookAndFeel::drawComboBox (Graphics& g, int width, int height,
									const bool isButtonDown,
									int buttonX, int buttonY,
									int buttonW, int buttonH,
									ComboBox& box)
{
    g.fillAll (box.findColour (ComboBox::backgroundColourId));
	
    if (box.isEnabled() && box.hasKeyboardFocus (false))
    {
        g.setColour (box.findColour (TextButton::buttonColourId));
        g.drawRect (0, 0, width, height, 2);
    }
    else
    {
        g.setColour (box.findColour (ComboBox::outlineColourId));
        g.drawRect (0, 0, width, height);
    }
	
    const float outlineThickness = box.isEnabled() ? 1.0f : 0.3f;
	
    const Colour baseColour (createBaseColour (box.findColour (ComboBox::buttonColourId),
                                               box.hasKeyboardFocus (true),
                                               false, false)
							 .darker().withMultipliedAlpha (box.isEnabled() ? 1.0f : 0.5f));
	
    drawOblong (g,
				buttonX + outlineThickness, buttonY + outlineThickness,
				buttonW - outlineThickness, buttonH - outlineThickness * 2.0f,
				baseColour, outlineThickness, -1.0f,
				true, true, true, true,
				box.isEnabled(), box.isMouseOver(), false);
	
    if (box.isEnabled())
    {
        const float arrowX = 0.3f;
        const float arrowH = 0.2f;
		
        Path p;
        p.addTriangle (buttonX + buttonW * 0.5f,            buttonY + buttonH * (0.45f - arrowH),
                       buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.45f,
                       buttonX + buttonW * arrowX,          buttonY + buttonH * 0.45f);
		
        p.addTriangle (buttonX + buttonW * 0.5f,            buttonY + buttonH * (0.55f + arrowH),
                       buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.55f,
                       buttonX + buttonW * arrowX,          buttonY + buttonH * 0.55f);
		
        g.setColour (Colours::black.withAlpha (0.6f));
        g.fillPath (p);
    }
}

const Font TCATLookAndFeel::getComboBoxFont (ComboBox& box)
{
    Font f (jmin (15.0f, box.getHeight() * 0.85f));
    return f;
}


//==============================================================================
void TCATLookAndFeel::drawScrollbarButton (Graphics& g,
										   ScrollBar& scrollbar,
										   int width, int height,
										   int buttonDirection,
										   bool /*isScrollbarVertical*/,
										   bool /*isMouseOverButton*/,
										   bool isButtonDown)
{
    Path p;
	
    if (buttonDirection == 0)
        p.addTriangle (width * 0.5f, height * 0.2f,
                       width * 0.1f, height * 0.7f,
                       width * 0.9f, height * 0.7f);
    else if (buttonDirection == 1)
        p.addTriangle (width * 0.8f, height * 0.5f,
                       width * 0.3f, height * 0.1f,
                       width * 0.3f, height * 0.9f);
    else if (buttonDirection == 2)
        p.addTriangle (width * 0.5f, height * 0.8f,
                       width * 0.1f, height * 0.3f,
                       width * 0.9f, height * 0.3f);
    else if (buttonDirection == 3)
        p.addTriangle (width * 0.2f, height * 0.5f,
                       width * 0.7f, height * 0.1f,
                       width * 0.7f, height * 0.9f);
	
    if (isButtonDown)
        g.setColour (scrollbar.findColour (ScrollBar::thumbColourId).contrasting (0.2f));
    else
        g.setColour (scrollbar.findColour (ScrollBar::thumbColourId));
	
    g.fillPath (p);
	
    g.setColour (Colours::black.withAlpha (0.5f));
    g.strokePath (p, PathStrokeType (0.5f));
}

void TCATLookAndFeel::drawScrollbar (Graphics& g,
									 ScrollBar& scrollbar,
									 int x, int y,
									 int width, int height,
									 bool isScrollbarVertical,
									 int thumbStartPosition,
									 int thumbSize,
									 bool /*isMouseOver*/,
									 bool /*isMouseDown*/)
{
    g.fillAll (scrollbar.findColour (ScrollBar::backgroundColourId));
	
    Path slotPath, thumbPath;
	
    const float slotIndent = jmin (width, height) > 15 ? 1.0f : 0.0f;
    const float thumbIndent = slotIndent + 1.0f;
	
    float gx1 = 0.0f, gy1 = 0.0f, gx2 = 0.0f, gy2 = 0.0f;
	
    if (isScrollbarVertical)
    {
        slotPath.addRoundedRectangle (x + slotIndent,
                                      y + slotIndent,
                                      width - slotIndent * 2.0f,
                                      height - slotIndent * 2.0f,
                                      (width - slotIndent * 2.0f) * 0.5f);
		
        if (thumbSize > 0)
            thumbPath.addRoundedRectangle (x + thumbIndent,
                                           thumbStartPosition + thumbIndent,
                                           width - thumbIndent * 2.0f,
                                           thumbSize - thumbIndent * 2.0f,
                                           (width - thumbIndent * 2.0f) * 0.5f);
        gx1 = (float) x;
        gx2 = x + width * 0.7f;
    }
    else
    {
        slotPath.addRoundedRectangle (x + slotIndent,
                                      y + slotIndent,
                                      width - slotIndent * 2.0f,
                                      height - slotIndent * 2.0f,
                                      (height - slotIndent * 2.0f) * 0.5f);
		
        if (thumbSize > 0)
            thumbPath.addRoundedRectangle (thumbStartPosition + thumbIndent,
                                           y + thumbIndent,
                                           thumbSize - thumbIndent * 2.0f,
                                           height - thumbIndent * 2.0f,
                                           (height - thumbIndent * 2.0f) * 0.5f);
        gy1 = (float) y;
        gy2 = y + height * 0.7f;
    }
	
    const Colour thumbColour (scrollbar.findColour (ScrollBar::thumbColourId));
	ColourGradient gradient1 (thumbColour.overlaidWith (Colours::black.withAlpha (0.27f)),
							  gx1, gy1,
							  thumbColour.overlaidWith (Colours::black.withAlpha (0.1f)),
							  gx2, gy2, false);
	
	g.setGradientFill (gradient1);
    g.fillPath (slotPath);
	
    if (isScrollbarVertical)
    {
        gx1 = x + width * 0.6f;
        gx2 = (float) x + width;
    }
    else
    {
        gy1 = y + height * 0.6f;
        gy2 = (float) y + height;
    }
	
	ColourGradient gradient2 (Colours::black.withAlpha (0.0f),
							  gx1, gy1,
							  Colours::black.withAlpha (0.1f),
							  gx2, gy2, false);
	
	g.setGradientFill (gradient2);
	g.fillPath (slotPath);
	
    g.setColour (thumbColour);
    g.fillPath (thumbPath);
	
    g.saveState();
	
 	ColourGradient gradient3 (Colours::black.withAlpha (0.05f),
							  gx1, gy1,
							  Colours::black.withAlpha (0.0f),
							  gx2, gy2, false);
	
	g.setGradientFill (gradient3);
	
	if (isScrollbarVertical)
        g.reduceClipRegion (x + width / 2, y, width, height);
    else
        g.reduceClipRegion (x, y + height / 2, width, height);
	
    g.fillPath (thumbPath);
    g.restoreState();
	
    g.setColour (Colours::black.withAlpha (0.3f));
    g.strokePath (thumbPath, PathStrokeType (0.4f));
}

ImageEffectFilter* TCATLookAndFeel::getScrollbarEffect()
{
    return 0;
}

//==============================================================================
void TCATLookAndFeel::drawButtonBackground (Graphics& g,
											Button& button,
											const Colour& backgroundColour,
											bool isMouseOverButton,
											bool isButtonDown)
{
    const int width = button.getWidth();
    const int height = button.getHeight();
	
    const float outlineThickness = button.isEnabled() ? ((isButtonDown || isMouseOverButton) ? 2.2f : 1.7f) : 1.4f;
	
    const float indentL = button.isConnectedOnLeft()   ? 0.1f : outlineThickness * 0.5f;
    const float indentR = button.isConnectedOnRight()  ? 0.1f : outlineThickness * 0.5f;
    const float indentT = button.isConnectedOnTop()    ? 0.1f : outlineThickness * 0.5f;
    const float indentB = button.isConnectedOnBottom() ? 0.1f : outlineThickness * 0.5f;
	
    const Colour baseColour (createBaseColour ( button.findColour(tcatTextButtonColourId)
											   .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f),
											   button.isEnabled(),
											   isMouseOverButton, 
											   isButtonDown)
							 .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));
	
    drawOblong (g,
				indentL,
				indentT,
				width - indentL - indentR,
				height - indentT - indentB,
				baseColour, outlineThickness, -1.0f,
				button.isConnectedOnLeft(),
				button.isConnectedOnRight(),
				button.isConnectedOnTop(),
				button.isConnectedOnBottom(),
				button.isEnabled(), button.isMouseOver(), button.isDown());
}

//==============================================================================

//==============================================================================
int TCATLookAndFeel::getSliderThumbRadius (Slider& slider)
{
    return jmin (7,
                 slider.getHeight() / 2,
                 slider.getWidth() / 2);
}

void TCATLookAndFeel::drawLinearSlider (Graphics& g,
										int x, int y,
										int width, int height,
										float sliderPos,
										float minSliderPos,
										float maxSliderPos,
										const Slider::SliderStyle style,
										Slider& slider)
{
    g.fillAll (slider.findColour (Slider::backgroundColourId));
	
    const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();
	
    if (style == Slider::LinearBar)
    {
        Colour baseColour (createBaseColour (slider.findColour (Slider::thumbColourId)
											 .withMultipliedSaturation (slider.isEnabled() ? 1.0f : 0.5f),
                                             false,
                                             isMouseOver, isMouseOver || slider.isMouseButtonDown()));
		
		Path p;
		p.addRectangle((float)x, (float)y, sliderPos - (float) x, (float) height);
		g.setColour(baseColour);
		g.fillPath(p);
	}
    else
    {
        const float sliderRadius = (float) getSliderThumbRadius (slider);
		
        const Colour trackColour (slider.findColour (tcatTextButtonColourId));
        const Colour gradCol1 (trackColour.overlaidWith (Colours::black.withAlpha (slider.isEnabled() ? 0.25f : 0.13f)));
        const Colour gradCol2 (trackColour.overlaidWith (Colours::black.withAlpha (0.08f)));
        Path indent;
		
        if (slider.isHorizontal())
        {
            const float iy = y + height * 0.5f - sliderRadius * 0.5f;
            const float ih = sliderRadius;
			
            ColourGradient gradient (gradCol1, 0.0f, iy,
									 gradCol2, 0.0f, iy + ih, false);
		 	g.setGradientFill (gradient);
			
			indent.addRoundedRectangle (x - sliderRadius * 0.5f, iy,
                                        width + sliderRadius, ih,
                                        5.0f);
            g.fillPath (indent);
        }
        else
        {
			Path tick;
			
			float ty = height*0.26f;
			
			// 0db tickmark
			tick.addTriangle(0, ty+2, 5, ty, 0, ty-2); // tick to left of slider
			Colour tc = slider.findColour (tcatTextButtonColourId);
			g.setColour(createBaseColour (tc.darker(2.0f)
										  .withMultipliedAlpha (slider.isEnabled() ? 1.0f : 0.5f),
										  slider.isEnabled(),
										  false,
										  false));
			
			g.strokePath(tick, PathStrokeType (1.5f));
			
            const float ix = x + width * 0.5f - sliderRadius * 0.5f;
            const float iw = sliderRadius;
			
            ColourGradient gradient (gradCol1, ix, 0.0f,
									 gradCol2, ix + iw, 0.0f, false);
		 	g.setGradientFill (gradient);
			
            indent.addRoundedRectangle (ix, y - sliderRadius * 0.5f,
                                        iw, height + sliderRadius,
                                        5.0f);
            g.fillPath (indent);
        }
		
        g.setColour (Colours::black.withAlpha (0.3f));
        g.strokePath (indent, PathStrokeType (0.5f));
		
        Colour knobColour (createBaseColour (slider.findColour (Slider::thumbColourId),
                                             slider.hasKeyboardFocus (false) && slider.isEnabled(),
                                             isMouseOver,
                                             slider.isMouseButtonDown() && slider.isEnabled()));
		
        const float outlineThickness = slider.isEnabled() ? 0.8f : 0.3f;
		
        if (style == Slider::LinearHorizontal || style == Slider::LinearVertical)
        {
			int tx, ty, tw, th;
			
            if (style == Slider::LinearVertical)
            {
                tx = (int)(width/2) - (int)(sliderRadius/2) -2;
                ty = (int)(sliderPos - 1 - sliderRadius);
				tw = (int)(sliderRadius * 2) + 4;
				th = (int)(sliderRadius * 2.4);
            }
            else
            {
                tx = (int)(sliderPos + 3 - sliderRadius);
                ty = y + (int)(height/2 - sliderRadius);
				tw = (int)(sliderRadius * 2.4);
				th = (int)(sliderRadius * 2);
            }
			
			drawSliderThumb(g,
							slider,
							tx, ty, tw, th,
							slider.isEnabled(), true, true);
        }
        else
        {
            if (style == Slider::ThreeValueVertical)
            {
                drawGlassSphere (g, x + width * 0.5f - sliderRadius,
                                 sliderPos - sliderRadius,
                                 sliderRadius * 2.0f,
                                 knobColour, outlineThickness);
            }
            else if (style == Slider::ThreeValueHorizontal)
            {
                drawGlassSphere (g,sliderPos - sliderRadius,
                                 y + height * 0.5f - sliderRadius,
                                 sliderRadius * 2.0f,
                                 knobColour, outlineThickness);
            }
			
            if (style == Slider::TwoValueVertical || style == Slider::ThreeValueVertical)
            {
                const float sr = jmin (sliderRadius, width * 0.4f);
				
                drawGlassPointer (g, jmax (0.0f, x + width * 0.5f - sliderRadius * 2.0f),
                                  minSliderPos - sliderRadius,
                                  sliderRadius * 2.0f, knobColour, outlineThickness, 1);
				
                drawGlassPointer (g, jmin (x + width - sliderRadius * 2.0f, x + width * 0.5f), maxSliderPos - sr,
                                  sliderRadius * 2.0f, knobColour, outlineThickness, 3);
            }
            else if (style == Slider::TwoValueHorizontal || style == Slider::ThreeValueHorizontal)
            {
                const float sr = jmin (sliderRadius, height * 0.4f);
				
                drawGlassPointer (g, minSliderPos - sr,
                                  jmax (0.0f, y + height * 0.5f - sliderRadius * 2.0f),
                                  sliderRadius * 2.0f, knobColour, outlineThickness, 2);
				
                drawGlassPointer (g, maxSliderPos - sliderRadius,
                                  jmin (y + height - sliderRadius * 2.0f, y + height * 0.5f),
                                  sliderRadius * 2.0f, knobColour, outlineThickness, 4);
            }
        }
    }
}

Button* TCATLookAndFeel::createSliderButton (const bool isIncrement)
{
    return new TextButton (isIncrement ? "+" : "-", String::empty);
}

ImageEffectFilter* TCATLookAndFeel::getSliderEffect()
{
    return 0;
}


//==============================================================================
void TCATLookAndFeel::drawPopupMenuBackground (Graphics& g, int width, int height)
{
    const Colour background (findColour (PopupMenu::backgroundColourId));
	
    g.fillAll (background);
    g.setColour (background.overlaidWith (Colours::lightblue.withAlpha (0.17f)));
	
    for (int i = 0; i < height; i += 3)
        g.fillRect (0, i, width, 1);
	
    g.setColour (findColour (PopupMenu::textColourId).withAlpha (0.6f));
    g.drawRect (0, 0, width, height);
}

void TCATLookAndFeel::drawMenuBarBackground (Graphics& g, int width, int height,
											 bool /*isMouseOverBar*/,
											 MenuBarComponent& menuBar)
{
    const Colour baseColour (createBaseColour (menuBar.findColour (PopupMenu::backgroundColourId), false, false, false));
	
    if (menuBar.isEnabled())
    {
        drawShinyButtonShape (g,
                              -4.0f, 0.0f,
                              width + 8.0f, (float) height,
                              0.0f,
                              baseColour,
                              0.4f,
                              true, true, true, true);
    }
    else
    {
        g.fillAll (baseColour);
    }
}

const Font TCATLookAndFeel::getPopupMenuFont()
{
    return Font (14.0f);
}

//==============================================================================
void TCATLookAndFeel::drawCornerResizer (Graphics& g,
										 int w, int h,
										 bool /*isMouseOver*/,
										 bool /*isMouseDragging*/)
{
    const float lineThickness = jmin (w, h) * 0.075f;
	
    for (float i = 0.0f; i < 1.0f; i += 0.3f)
    {
        g.setColour (Colours::lightgrey);
		
        g.drawLine (w * i,
                    h + 1.0f,
                    w + 1.0f,
                    h * i,
                    lineThickness);
		
        g.setColour (Colours::darkgrey);
		
        g.drawLine (w * i + lineThickness,
                    h + 1.0f,
                    w + 1.0f,
                    h * i + lineThickness,
                    lineThickness);
    }
}

//==============================================================================
/* int TCATLookAndFeel::drawProgressBar (Graphics& g, ProgressBar& progressBar,
 int x, int y, int w, int h,
 float progress)
 {
 const Colour background (progressBar.findColour (ProgressBar::backgroundColourId));
 g.fillAll (background);
 
 g.setColour (background.contrasting (0.2f));
 g.drawRect (x, y, w, h);
 
 drawOblong (g,
 (float) (x + 1),
 (float) (y + 1),
 jlimit (0.0f, w - 2.0f, progress * (w - 2.0f)),
 (float) (h - 2),
 progressBar.findColour (ProgressBar::foregroundColourId).contrasting(0.5f),
 0.5f,
 0.0f,
 true, true, true, true,
 progressBar.isEnabled(), progressBar.isMouseOver(), false);
 return 0;
 }
 */
//==============================================================================
class GlassWindowButton   : public Button
{
public:
    //==============================================================================
    GlassWindowButton (const String& name, const Colour& col,
                       const Path& normalShape_,
                       const Path& toggledShape_) throw()
	: Button (name),
	colour (col),
	normalShape (normalShape_),
	toggledShape (toggledShape_)
    {
    }
	
    ~GlassWindowButton()
    {
    }
	
    //==============================================================================
    void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown)
    {
        float alpha = isMouseOverButton ? (isButtonDown ? 1.0f : 0.8f) : 0.55f;
		
        if (! isEnabled())
            alpha *= 0.5f;
		
        float x = 0, y = 0, diam;
		
        if (getWidth() < getHeight())
        {
            diam = (float) getWidth();
            y = (getHeight() - getWidth()) * 0.5f;
        }
        else
        {
            diam = (float) getHeight();
            y = (getWidth() - getHeight()) * 0.5f;
        }
		
        x += diam * 0.05f;
        y += diam * 0.05f;
		
        diam *= 0.9f;
		
		ColourGradient gradient (Colours::white.withAlpha (0.6f), 0, y + diam,
								 Colours::black.withAlpha (0.6f), 0, y, false);
	 	g.setGradientFill (gradient);
		
		g.fillEllipse (x+1, y+1, diam-2, diam-2);
		
        x += 2.0f;
        y += 2.0f;
        diam -= 4.0f;
		
        LookAndFeel::drawGlassSphere (g, x, y, diam, colour.withAlpha (alpha), 1.0f);
		
        Path& p = getToggleState() ? toggledShape : normalShape;
		
        const AffineTransform t (p.getTransformToScaleToFit (x + diam * 0.3f, y + diam * 0.3f,
                                                             diam * 0.4f, diam * 0.4f, true));
		
        g.setColour (Colours::black.withAlpha (alpha * 0.6f));
        g.fillPath (p, t);
    }
	
    //==============================================================================
    juce_UseDebuggingNewOperator
	
private:
    Colour colour;
    Path normalShape, toggledShape;
	
    GlassWindowButton (const GlassWindowButton&);
    const GlassWindowButton& operator= (const GlassWindowButton&);
};

//==============================================================================
class SquareWindowButton   : public Button
{
public:
    //==============================================================================
    SquareWindowButton (const String& name, const Colour& col,
						const Path& normalShape_,
						const Path& toggledShape_) throw()
	: Button (name),
	colour (col),
	normalShape (normalShape_),
	toggledShape (toggledShape_)
    {
    }
	
    ~SquareWindowButton()
    {
    }
	
    //==============================================================================
    void paintButton (Graphics& g, bool isMouseOverButton, bool isButtonDown)
    {
        float alpha = isMouseOverButton ? (isButtonDown ? 1.0f : 0.8f) : 0.55f;
		bool bIsCloseButton = (0 == getName().compare("close"));
        float x = 0, y = 0, len;
		
        if (! isEnabled())
            alpha *= 0.5f;
		
		// button background highlight
        if (getWidth() < getHeight())
        {
            len = (float) getWidth();
            y = (getHeight() - getWidth()) * 0.5f;
        }
        else
        {
            len = (float) getHeight();
            y = (getWidth() - getHeight()) * 0.5f;
        }
		
        x += len * 0.075f;
        y += len * 0.075f;
        len *= 0.8f;
		
		g.setColour(Colour::greyLevel (3.0f).withAlpha (alpha));
        g.fillRoundedRectangle(x, y, len, len, 2);
		
		// button color fill
		Colour buttonColour;
		if (bIsCloseButton)
		{
			buttonColour = Colours::firebrick;
		}
		else
		{
			buttonColour = findColour(TCATLookAndFeel::tcatTitleBarColourId).darker();
		}
		
		x += 1.0f;
        y += 1.0f;
        len -= 2.0f;
		g.setColour(buttonColour);
		g.fillRoundedRectangle(x, y, len, len, 2.0f);
		
		// specular reflection
		x += 1.0f;
        y += 1.0f;
        len -= 2.0f;
		
		Path p;
		p.addRoundedRectangle(x, y, len, len, 2.0f);
		
		Colour specularColour(Colours::white.withAlpha(0.5f));
		if (isMouseOverButton)
		{
			specularColour = specularColour.brighter(1.5f);
		}
		if (isButtonDown)
		{
			specularColour = specularColour.darker();
		}
		
		ColourGradient cg (specularColour.overlaidWith (colour.withMultipliedAlpha (0.3f)), x+2, y+2,
						   specularColour.overlaidWith (colour.withMultipliedAlpha (0.9f)), x+len-4.0f, y+len-4.0f, true);
		g.setGradientFill(cg);
		
		g.fillPath (p);
		
		// button glyph
        p.clear();
		p = getToggleState() ? toggledShape : normalShape;
		
		float y_offset = 0;
		float x_offset = 0;
		if (0 == getName().compare("minimise"))
		{
	        y_offset = 0.4f;
	        x_offset = 0.2f;
		}
		else if (0 == getName().compare("maximise"))
		{
	        x_offset = 0.1f;
		}
		
        const AffineTransform t (p.getTransformToScaleToFit (x + len * (0.15f) + x_offset, y + len * (0.15f + y_offset),
	                                                         len * (0.7f - x_offset), len * (0.7f - y_offset), true));
		
		if (isButtonDown)
		{
			g.setColour (buttonColour.brighter(0.6f));
		}
		else
		{
			g.setColour (Colours::white);
		}
        g.fillPath (p, t);
    }
	
    //==============================================================================
    juce_UseDebuggingNewOperator
	
private:
    Colour colour;
    Path normalShape, toggledShape;
	
    SquareWindowButton (const SquareWindowButton&);
    const SquareWindowButton& operator= (const SquareWindowButton&);
};

Button* TCATLookAndFeel::createDocumentWindowButton (int buttonType)
{
    Path shape;
    const float crossThickness = 0.25f;
	
#ifdef __MACH__
    if (buttonType == DocumentWindow::closeButton)
    {
        shape.addLineSegment (Line<float>(0.0f, 0.0f, 1.0f, 1.0f), crossThickness * 1.4f);
        shape.addLineSegment (Line<float>(1.0f, 0.0f, 0.0f, 1.0f), crossThickness * 1.4f);
		
        return new GlassWindowButton ("close", Colour (0xffdd1100), shape, shape);
    }
    else if (buttonType == DocumentWindow::minimiseButton)
    {
        shape.addLineSegment (Line<float>(0.0f, 0.5f, 1.0f, 0.5f), crossThickness);
		
        return new GlassWindowButton ("minimise", Colour (0xffaa8811), shape, shape);
    }
    else if (buttonType == DocumentWindow::maximiseButton)
    {
        shape.addLineSegment (Line<float>(0.5f, 0.0f, 0.5f, 1.0f), crossThickness);
        shape.addLineSegment (Line<float>(0.0f, 0.5f, 1.0f, 0.5f), crossThickness);
		
        Path fullscreenShape;
        fullscreenShape.startNewSubPath (45.0f, 100.0f);
        fullscreenShape.lineTo (0.0f, 100.0f);
        fullscreenShape.lineTo (0.0f, 0.0f);
        fullscreenShape.lineTo (100.0f, 0.0f);
        fullscreenShape.lineTo (100.0f, 45.0f);
        fullscreenShape.addRectangle (45.0f, 45.0f, 100.0f, 100.0f);
        PathStrokeType (30.0f).createStrokedPath (fullscreenShape, fullscreenShape);
		
        return new GlassWindowButton ("maximise", Colour (0xff119911), shape, fullscreenShape);
    }
#else
    if (buttonType == DocumentWindow::closeButton)
    {
        shape.addLineSegment (Line<float>(0.0f, 0.0f, 4.0f, 4.0f), crossThickness * 4.0f);
        shape.addLineSegment (Line<float>(4.0f, 0.0f, 0.0f, 4.0f), crossThickness * 4.0f);
		
		return new SquareWindowButton ("close", Colours::firebrick, shape, shape);
    }
    else if (buttonType == DocumentWindow::minimiseButton)
    {
        shape.addLineSegment (Line<float>(0.0f, 1.0f, 0.6f, 1.0f), crossThickness * 0.8f);
		
        return new SquareWindowButton ("minimise", findColour(TCATLookAndFeel::tcatWindowBackgroundColourId), shape, shape);
    }
    else if (buttonType == DocumentWindow::maximiseButton)
    {
        shape.addLineSegment (Line<float>(0.0f, 0.0f, 100.0f, 0.0f), crossThickness);
        shape.addLineSegment (Line<float>(0.0f, 10.0f, 100.0f, 10.0f), crossThickness);
        shape.addLineSegment (Line<float>(0.0f, 20.0f, 100.0f, 20.0f), crossThickness);
        shape.addLineSegment (Line<float>(100.0f, 0.0f, 100.0f, 100.0f), crossThickness);
        shape.addLineSegment (Line<float>(100.0f, 100.0f, 0.0f, 100.0f), crossThickness);
        shape.addLineSegment (Line<float>(0.0f, 100.0f, 0.0f, 0.0f), crossThickness);
        PathStrokeType (10.0f).createStrokedPath (shape, shape);
		
        Path fullscreenShape;
        fullscreenShape.startNewSubPath (45.0f, 100.0f);
        fullscreenShape.lineTo (0.0f, 100.0f);
        fullscreenShape.lineTo (0.0f, 0.0f);
        fullscreenShape.lineTo (100.0f, 0.0f);
        fullscreenShape.lineTo (100.0f, 45.0f);
        fullscreenShape.addRectangle (45.0f, 45.0f, 100.0f, 100.0f);
        PathStrokeType (30.0f).createStrokedPath (fullscreenShape, fullscreenShape);
		
		return new SquareWindowButton ("maximise", findColour(TCATLookAndFeel::tcatWindowBackgroundColourId), shape, fullscreenShape);
    }
#endif // __MACH__
	
    jassertfalse
    return 0;
}

void TCATLookAndFeel::positionDocumentWindowButtons (DocumentWindow&,
													 int titleBarX, int titleBarY,
													 int titleBarW, int titleBarH,
													 Button* minimiseButton,
													 Button* maximiseButton,
													 Button* closeButton,
													 bool positionTitleBarButtonsOnLeft)
{
#ifdef WIN32
    const int buttonW = titleBarH - titleBarH / 8;
#else
    const int buttonW = titleBarH - titleBarH / 4;
#endif
	
    int x = positionTitleBarButtonsOnLeft ? titleBarX + 4
	: titleBarX + titleBarW - buttonW - buttonW / 4;
	
    if (closeButton != 0)
    {
        closeButton->setBounds (x, titleBarY, buttonW, titleBarH);
        x += positionTitleBarButtonsOnLeft ? buttonW : -(buttonW + buttonW / 4);
    }
	
    if (positionTitleBarButtonsOnLeft)
        swapVariables (minimiseButton, maximiseButton);
	
    if (maximiseButton != 0)
    {
        maximiseButton->setBounds (x, titleBarY, buttonW, titleBarH);
        x += positionTitleBarButtonsOnLeft ? buttonW : -buttonW;
    }
	
    if (minimiseButton != 0)
        minimiseButton->setBounds (x, titleBarY, buttonW, titleBarH);
}

//==============================================================================
static void createRoundedPath (Path& p,
                               const float x, const float y,
                               const float w, const float h,
                               const float cs,
                               const bool curveTopLeft, const bool curveTopRight,
                               const bool curveBottomLeft, const bool curveBottomRight)
{
    const float cs2 = 2.0f * cs;
	
    if (curveTopLeft)
    {
        p.startNewSubPath (x, y + cs);
        p.addArc (x, y, cs2, cs2, float_Pi * 1.5f, float_Pi * 2.0f);
    }
    else
    {
        p.startNewSubPath (x, y);
    }
	
    if (curveTopRight)
    {
        p.lineTo (x + w - cs, y);
        p.addArc (x + w - cs2, y, cs2, cs2, 0.0f, float_Pi * 0.5f);
    }
    else
    {
        p.lineTo (x + w, y);
    }
	
    if (curveBottomRight)
    {
        p.lineTo (x + w, y + h - cs);
        p.addArc (x + w - cs2, y + h - cs2, cs2, cs2, float_Pi * 0.5f, float_Pi);
    }
    else
    {
        p.lineTo (x + w, y + h);
    }
	
    if (curveBottomLeft)
    {
        p.lineTo (x + cs, y + h);
        p.addArc (x, y + h - cs2, cs2, cs2, float_Pi, float_Pi * 1.5f);
    }
    else
    {
        p.lineTo (x, y + h);
    }
	
    p.closeSubPath();
}


void TCATLookAndFeel::drawShinyButtonShape (Graphics& g,
											float x, float y, float w, float h,
											float maxCornerSize,
											const Colour& baseColour,
											const float strokeWidth,
											const bool flatOnLeft,
											const bool flatOnRight,
											const bool flatOnTop,
											const bool flatOnBottom)
{
    if (w <= strokeWidth * 1.1f || h <= strokeWidth * 1.1f)
        return;
	
    const float cs = jmin (maxCornerSize, w * 0.5f, h * 0.5f);
	
    Path outline;
    createRoundedPath (outline, x, y, w, h, cs,
					   ! (flatOnLeft || flatOnTop),
					   ! (flatOnRight || flatOnTop),
					   ! (flatOnLeft || flatOnBottom),
					   ! (flatOnRight || flatOnBottom));
	
    ColourGradient cg (baseColour.overlaidWith (Colours::white.withAlpha (0.0f)), 0.0f, y,
                       baseColour.overlaidWith (Colours::blue.withAlpha (0.03f)), 0.0f, y + h,
                       false);
	
    cg.addColour (0.5, baseColour.overlaidWith (Colours::white.withAlpha (0.2f)));
    cg.addColour (0.51, baseColour.overlaidWith (Colours::blue.withAlpha (0.07f)));
	
    g.setGradientFill(cg);
	
    g.fillPath (outline);
	
    g.setColour (Colours::black.withAlpha (0.5f));
    g.strokePath (outline, PathStrokeType (strokeWidth));
}

//==============================================================================
void TCATLookAndFeel::drawGlassSphere (Graphics& g,
									   float x, float y, float diameter,
									   const Colour& colour,
									   const float outlineThickness)
{
    if (diameter <= outlineThickness)
        return;
	
    Path p;
    p.addEllipse (x, y, diameter, diameter);
	
    {
        ColourGradient cg (Colours::white.overlaidWith (colour.withMultipliedAlpha (0.3f)), 0, y,
                           Colours::white.overlaidWith (colour.withMultipliedAlpha (0.3f)), 0, y + diameter, false);
		
        cg.addColour (0.4, Colours::white.overlaidWith (colour));
		
        g.setGradientFill(cg);
        g.fillPath (p);
    }
	
    {
        ColourGradient cg (Colours::white, 0, y + diameter * 0.06f,
						   Colours::transparentWhite, 0, y + diameter * 0.3f, false);
		
        g.setGradientFill(cg);
        g.fillEllipse (x + diameter * 0.2f, y + diameter * 0.05f, diameter * 0.6f, diameter * 0.4f);
    }
	
    {
        ColourGradient cg (Colours::transparentBlack,
                           x + diameter * 0.5f, y + diameter * 0.5f,
                           Colours::black.withAlpha (0.5f * outlineThickness * colour.getFloatAlpha()),
                           x, y + diameter * 0.5f, true);
		
        cg.addColour (0.7, Colours::transparentBlack);
        cg.addColour (0.8, Colours::black.withAlpha (0.1f * outlineThickness));
		
        g.setGradientFill(cg);
        g.fillPath (p);
    }
	
    g.setColour (Colours::black.withAlpha (0.5f * colour.getFloatAlpha()));
    g.drawEllipse (x, y, diameter, diameter, outlineThickness);
}


void TCATLookAndFeel::drawGlassPointer (Graphics& g, float x, float y, float diameter,
										const Colour& colour, const float outlineThickness,
										const int direction)
{
    if (diameter <= outlineThickness)
        return;
	
    Path p;
    p.startNewSubPath (x + diameter * 0.5f, y);
    p.lineTo (x + diameter, y + diameter * 0.6f);
    p.lineTo (x + diameter, y + diameter);
    p.lineTo (x, y + diameter);
    p.lineTo (x, y + diameter * 0.6f);
    p.closeSubPath();
	
    p.applyTransform (AffineTransform::rotation (direction * (float_Pi * 0.5f), x + diameter * 0.5f, y + diameter * 0.5f));
	
    {
        ColourGradient cg (Colours::white.overlaidWith (colour.withMultipliedAlpha (0.3f)), 0, y,
                           Colours::white.overlaidWith (colour.withMultipliedAlpha (0.3f)), 0, y + diameter, false);
		
        cg.addColour (0.4, Colours::white.overlaidWith (colour));
		
        g.setGradientFill(cg);
        g.fillPath (p);
    }
	
    {
        ColourGradient cg (Colours::transparentBlack,
                           x + diameter * 0.5f, y + diameter * 0.5f,
                           Colours::black.withAlpha (0.5f * outlineThickness * colour.getFloatAlpha()),
                           x - diameter * 0.2f, y + diameter * 0.5f, true);
		
        cg.addColour (0.5, Colours::transparentBlack);
        cg.addColour (0.7, Colours::black.withAlpha (0.07f * outlineThickness));
		
        g.setGradientFill (cg);
        g.fillPath (p);
    }
	
    g.setColour (Colours::black.withAlpha (0.5f * colour.getFloatAlpha()));
    g.strokePath (p, PathStrokeType (outlineThickness));
}


void TCATLookAndFeel::drawOblong (Graphics& g, float x, float y, float width, float height,
								  const Colour& colour, const float outlineThickness, const float cornerSize,
								  const bool flatOnLeft, const bool flatOnRight, const bool flatOnTop, const bool flatOnBottom,
								  const bool isEnabled, const bool isMouseOverButton, const bool isButtonDown)
{
    if (width <= outlineThickness || height <= outlineThickness)
        return;
	
	const float cs = cornerSize < 0 ? jmin (width * 0.5f, height * 0.5f) : cornerSize;
	
    Path outline;
    createRoundedPath (outline, x, y, width, height, cs,
					   ! (flatOnLeft || flatOnTop),
					   ! (flatOnRight || flatOnTop),
					   ! (flatOnLeft || flatOnBottom),
					   ! (flatOnRight || flatOnBottom));
	
    {
		// background fill
		g.setColour(createBaseColour (colour
									  .withMultipliedAlpha (isEnabled ? 1.0f : 0.5f),
									  isEnabled,
									  isMouseOverButton,
									  isButtonDown));
        g.fillPath (outline);
		
		g.setColour(createBaseColour (colour.darker(2.0f)
									  .withMultipliedAlpha (isEnabled ? 1.0f : 0.5f),
									  isEnabled,
									  isMouseOverButton,
									  isButtonDown));
		g.strokePath (outline, PathStrokeType (outlineThickness));
    }
}



//==============================================================================
void TCATLookAndFeel::drawDocumentWindowTitleBar (DocumentWindow& window,
												  Graphics& g, int w, int h,
												  int titleSpaceX, int titleSpaceW,
												  const Image* icon,
												  bool drawTitleTextOnLeft)
{
    const bool isActive = window.isActiveWindow();
	
	Colour titleBarColour(window.getBackgroundColour());
	
	if (0 == (window.getName().compare(kTCAT_DICE_USR_STR)))
	{
		titleBarColour = findColour(TCATLookAndFeel::tcatTitleBarColourId);
	}
	else
	{
		titleBarColour = findColour(TCATLookAndFeel::tcatPopupTitleBarColourId);
	}
	
    g.setColour (titleBarColour);
	g.fillRect (0, 0, w, h);
	
    Font font (h * 0.65f, Font::bold);
    g.setFont (font);
	
    int textW = font.getStringWidth (window.getName());
    int iconW = 0;
    int iconH = 0;
	
    if (icon != 0)
    {
        iconH = (int) font.getHeight();
        iconW = icon->getWidth() * iconH / icon->getHeight() + 4;
    }
	
    textW = jmin (titleSpaceW, textW + iconW);
    int textX = drawTitleTextOnLeft ? titleSpaceX
	: jmax (titleSpaceX, (w - textW) / 2);
	
    if (textX + textW > titleSpaceX + titleSpaceW)
        textX = titleSpaceX + titleSpaceW - textW;
	
    if (icon != 0)
    {
        g.setOpacity (isActive ? 1.0f : 0.6f);
        g.drawImageWithin (*icon, textX, (h - iconH) / 2, iconW, iconH,
                           RectanglePlacement::centred, false);
        textX += iconW;
        textW -= iconW;
    }
	
    g.setColour (titleBarColour.contrasting());
	g.drawText (window.getName(), textX, 0, textW, h, Justification::centredLeft, true);
}


//==============================================================================
void TCATLookAndFeel::createTabButtonShape (Path& p,
											int width, int height,
											int /*tabIndex*/,
											const String& /*text*/,
											Button& /*button*/,
											TabbedButtonBar::Orientation orientation,
											const bool /*isMouseOver*/,
											const bool /*isMouseDown*/,
											const bool /*isFrontTab*/)
{
    const float w = (float) width;
    const float h = (float) height;
	
    if (orientation == TabbedButtonBar::TabsAtTop)
    {
		p.addRoundedRectangle(6.0f, 0.0f, w-6.0f, h-6.0f, 3.0f);
    }
    else
    {
		// ui supports top tabs only
		jassert(false);
    }
}

void TCATLookAndFeel::fillTabButtonShape (Graphics& g,
										  const Path& path,
										  const Colour& preferredColour,
										  int /*tabIndex*/,
										  const String& tabStr,
										  Button& button,
										  TabbedButtonBar::Orientation /*orientation*/,
										  const bool isMouseOverButton,
										  const bool isButtonDown,
										  const bool isFrontTab)
{
	Colour tabButtonColour (preferredColour);
	
	// hardcoded tab names here for now
	/*	if ((0 == tabStr.compare("Bus")) || 
	 (0 == tabStr.compare("WDM")) || 
	 (0 == tabStr.compare("DPC")) || 
	 (0 == tabStr.compare("Info")) )
	 {
	 tabButtonColour  = createBaseColour ( findColour(TCATLookAndFeel::tcatGlobalTabButtonColourId),
	 button.isEnabled(),
	 isMouseOverButton, 
	 isButtonDown)
	 .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);
	 }
	 else */
	{
		tabButtonColour = createBaseColour ( findColour(TCATLookAndFeel::tcatDeviceTabButtonColourId),
											button.isEnabled(),
											isMouseOverButton, 
											isButtonDown)
		.withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f);
	}
	
    g.setColour (isFrontTab ? tabButtonColour.brighter(0.4f)
				 : tabButtonColour);
	
    g.fillPath (path);
	
    g.setColour (tabButtonColour.darker(button.isEnabled() ? 1.0f : 0.25f));
    g.strokePath (path, PathStrokeType (2.0f));
}

void TCATLookAndFeel::drawTabButtonText (Graphics& g,
										 int x, int y, int w, int h,
										 const Colour& preferredBackgroundColour,
										 int /*tabIndex*/,
										 const String& text,
										 Button& button,
										 TabbedButtonBar::Orientation orientation,
										 const bool isMouseOver,
										 const bool isMouseDown,
										 const bool /*isFrontTab*/)
{
    int length = w;
    int depth = h;
	
    if (orientation != TabbedButtonBar::TabsAtTop)
    {
		// ui supports top tabs only
		jassert(false);
	}
	
    Font font (depth * 0.5f);
    font.setUnderline (button.hasKeyboardFocus (false));
	
    GlyphArrangement textLayout;
    textLayout.addFittedText (font, text.trim(),
                              0.0f, 0.0f, (float) length, (float) depth,
                              Justification::centred,
                              jmax (1, depth / 12));
	
    AffineTransform transform;
	
    transform = transform.translated ((float) x+3.0f, (float) y-4.0f);
	
    g.setColour (preferredBackgroundColour.contrasting());
	
    if (! (isMouseOver || isMouseDown))
        g.setOpacity (0.8f);
	
    if (! button.isEnabled())
        g.setOpacity (0.3f);
	
    textLayout.draw (g, transform);
}

int TCATLookAndFeel::getTabButtonOverlap (int tabDepth)
{
	return 0;
}

void TCATLookAndFeel::drawTabAreaBehindFrontButton (Graphics& g,
													int w, int h,
													TabbedButtonBar& tabBar,
													TabbedButtonBar::Orientation orientation)
{
	return;
}

void TCATLookAndFeel::drawTickBox (Graphics& g,
								   Component& component,
								   float x, float y, float w, float h,
								   const bool ticked,
								   bool isEnabled,
								   bool isMouseOverButton,
								   bool isButtonDown)
{
	String nameStr = component.getName();
	
	Colour colour = component.findColour (tcatTextButtonColourId);
	
	Path p;
	
	p.addRoundedRectangle(x-1.0f, y+1.0f, w-2.0f, h-2.0f, 3.0f);
	
	// specific to 'Mute' and 'Solo' ToggleButtons
	// name starting with '~' indicates use color background to indicate on state
	if (ticked && nameStr.startsWith("~"))
	{
		// if component is named, use color based on name
		String typeStr = nameStr.fromFirstOccurrenceOf("~", false, false);
		if (0 == typeStr.compare("M"))
		{
			g.setColour(Colours::mediumseagreen);
		}
		else if (0 == typeStr.compare("S"))
		{
			g.setColour(Colours::steelblue);
		}
	}
	else
	{
		g.setColour(createBaseColour (colour
									  .withMultipliedAlpha (isEnabled ? 1.0f : 0.5f),
									  isEnabled,
									  isMouseOverButton,
									  isButtonDown));
	}
	
	g.fillPath(p);
	
	// outline
	g.setColour(createBaseColour (colour.darker(2.0f)
								  .withMultipliedAlpha (isEnabled ? 1.0f : 0.5f),
								  isEnabled,
								  isMouseOverButton,
								  isButtonDown));
	
	g.strokePath(p, PathStrokeType (2.0f));
	
	// checkmark
    if (ticked)
    {
		// name starting with '~' indicates use the name instead of the x-mark regardless of on state
		if (nameStr.startsWith("~"))
		{
			g.drawText(nameStr.fromFirstOccurrenceOf("~", false, false), (int)x-1, (int)y, (int)w-1, (int)h-1, Justification::centred, true);
		}
		else
		{
			Path tick;
			tick.startNewSubPath (x+4.0f, y+6.0f);
			tick.lineTo (w-4.0f, h-2.0f);
			tick.startNewSubPath (w-4.0f, y+6.0f);
			tick.lineTo (x+4.0f, h-2.0f);
			tick.closeSubPath();
			
			g.setColour(createBaseColour (colour.darker(2.0f)
										  .withMultipliedAlpha (isEnabled ? 1.0f : 0.5f),
										  isEnabled,
										  false,
										  false));
			g.strokePath (tick, PathStrokeType (2.0f, PathStrokeType::mitered, PathStrokeType::rounded));
		}
	}
	else
	{
		if (nameStr.startsWith("~"))
		{
			g.drawText(nameStr.fromFirstOccurrenceOf("~", false, false), (int)x-1, (int)y+1, (int)w-1, (int)h-1, Justification::centred, true);
		}
	}
	
}

void TCATLookAndFeel::drawSliderThumb (Graphics& g,
									   Component& component,
									   int x, int y, int w, int h,
									   const bool isEnabled,
									   const bool isMouseOverButton,
									   const bool isButtonDown)
{
	Colour colour = component.findColour (tcatTextButtonColourId);
	
	Path p;
	
	p.addRoundedRectangle((float)(x-2), (float)(y+2), (float)(w-4), (float)(h-4), 3.0f);
	
	// background fill
	g.setColour(createBaseColour (colour
								  .withMultipliedAlpha (isEnabled ? 1.0f : 0.5f),
								  isEnabled,
								  false,
								  false));
	g.fillPath(p);
	
	// outline
	g.setColour(createBaseColour (colour.darker(2.0f)
								  .withMultipliedAlpha (isEnabled ? 1.0f : 0.5f),
								  isEnabled,
								  false,
								  false));
	
	g.strokePath(p, PathStrokeType (2.0f));
}

Label* TCATLookAndFeel::createSliderTextBox (Slider& slider)
{
    Label* const l = new Label ("n", String::empty);
	
	l->setFont(l->getFont().getHeight() * 0.75f);
	
    l->setJustificationType (Justification::centred);
	
    l->setColour (Label::textColourId, slider.findColour (Slider::textBoxTextColourId));
	
    l->setColour (Label::backgroundColourId,
                  (slider.getSliderStyle() == Slider::LinearBar) ? Colours::transparentBlack
				  : slider.findColour (Slider::textBoxBackgroundColourId));
    l->setColour (Label::outlineColourId, slider.findColour (Slider::textBoxOutlineColourId));
	
    l->setColour (TextEditor::textColourId, slider.findColour (Slider::textBoxTextColourId));
	
    l->setColour (TextEditor::backgroundColourId,
                  slider.findColour (Slider::textBoxBackgroundColourId)
				  .withAlpha (slider.getSliderStyle() == Slider::LinearBar ? 0.7f : 1.0f));
	
    l->setColour (TextEditor::outlineColourId, slider.findColour (Slider::textBoxOutlineColourId));
	
	
    return l;
}

void TCATLookAndFeel::drawRotarySlider (Graphics& g,
										int x, int y,
										int width, int height,
										float sliderPos,
										const float rotaryStartAngle,
										const float rotaryEndAngle,
										Slider& slider)
{
    const float radius = jmin (width / 2, height / 2) - 2.0f;
    const float centreX = x + width * 0.5f;
    const float centreY = y + height * 0.5f;
    const float rx = centreX - radius;
    const float ry = centreY - radius;
    const float rw = radius * 2.0f;
    const float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();
	
	// draw the filled arc from top of ellipse to the current angle
	float twoPi = 6.28318f;	// top of the arc
	const float startAngle = (sliderPos <= 0.5f) ? angle : twoPi;
	const float endAngle = (sliderPos <= 0.5f) ? twoPi : angle;
	
	Colour colour = slider.findColour (tcatTextButtonColourId);
	
	if (radius > 12.0f)
    {
		g.setColour(createBaseColour (colour.darker(2.0f)
									  .withMultipliedAlpha (slider.isEnabled() ? 1.0f : 0.5f),
									  slider.isEnabled(),
									  false,
									  false));
		
		// needle
        const float thickness = 0.7f;
        {
            Path filledArc;
            filledArc.addPieSegment (rx, ry, rw, rw,
									 startAngle,
									 endAngle,
									 thickness);
			
            g.fillPath (filledArc);
        }
		
        if (thickness > 0)
        {
            const float innerRadius = radius * 0.2f;
            Path p;
            p.addTriangle (-innerRadius, 0.0f,
                           0.0f, -radius * thickness * 1.1f,
                           innerRadius, 0.0f);
			
            p.addEllipse (-innerRadius, -innerRadius, innerRadius * 2.0f, innerRadius * 2.0f);
			
            g.fillPath (p, AffineTransform::rotation (angle).translated (centreX, centreY));
        }
		
        Path outlineArc;
        outlineArc.addPieSegment (rx, ry, rw, rw, rotaryStartAngle, rotaryEndAngle, thickness);
        outlineArc.closeSubPath();
		
        g.strokePath (outlineArc, PathStrokeType (slider.isEnabled() ? (isMouseOver ? 2.0f : 1.2f) : 0.3f));
    }
    else
    {
		g.setColour(createBaseColour (colour.darker(2.0f)
									  .withMultipliedAlpha (slider.isEnabled() ? 1.0f : 0.5f),
									  slider.isEnabled(),
									  false,
									  false));
		
        Path p;
        p.addEllipse (-0.4f * rw, -0.4f * rw, rw * 0.8f, rw * 0.8f);
        PathStrokeType (rw * 0.1f).createStrokedPath (p, p);
		
        p.addLineSegment (Line<float>(0.0f, 0.0f, 0.0f, -radius), rw * 0.2f);
		
        g.fillPath (p, AffineTransform::rotation (angle).translated (centreX, centreY));
    }
}
