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

#include "BusInfoComponent.h"
#include "Listener.h"
#include "BusWindow.h"
#include "tcat_dice_pal_device.h"
#include "juce_TCATLookAndFeel.h"
#include "appDefaults.h"

//==============================================================================
// this component contains the subcomponents that display global info
BusInfoComponent::BusInfoComponent () :
	m_logo(0)
{
	setName ("Bus Info");

#if JUCE_MAC
	File dir (File::getSpecialLocation (File::currentApplicationFile));

    dir = dir.getChildFile ("Contents/Resources");
#endif

#if JUCE_WIN32
	File dir (File::getSpecialLocation (File::currentApplicationFile));
	dir = dir.getParentDirectory();
#endif

	File logo_file = dir.getChildFile ("logo")
						.withFileExtension ("png");

	if (logo_file.exists())
	{
		m_logo = Drawable::createFromImageFile(logo_file);
	}

	m_driver_info_l = new Label( String::empty, String::empty);
	addAndMakeVisible (m_driver_info_l);
	m_driver_info_l->setJustificationType(Justification::centredLeft);

	m_pal_info_l = new Label( String::empty, String::empty);
	addAndMakeVisible (m_pal_info_l);
	m_pal_info_l->setJustificationType(Justification::centredLeft);

	m_vendor_info_l1 = new Label( String::empty, String::empty);
	addAndMakeVisible (m_vendor_info_l1);
	m_vendor_info_l1->setJustificationType(Justification::centred);

	m_vendor_info_l2 = new Label( String::empty, String::empty);
	addAndMakeVisible (m_vendor_info_l2);
	m_vendor_info_l2->setJustificationType(Justification::centred);

	m_vendor_info_l3 = new Label( String::empty, String::empty);
	addAndMakeVisible (m_vendor_info_l3);
	m_vendor_info_l3->setJustificationType(Justification::centred);

	updateInfo();

	lookAndFeelChanged();
}
	
BusInfoComponent::~BusInfoComponent() 
{
	if (m_logo) delete (m_logo);
    deleteAllChildren();
}

void BusInfoComponent::paint (Graphics& g)
{
	if (m_logo)
	{
		float square_len = 80.0f;	// size of the square the logo will be fit into
		float offset_len = (float)(getHeight()-square_len);
		g.setOpacity(0.5f);
		m_logo->drawWithin (g, Rectangle<float>((float)getWidth()-square_len-offset_len, 0, square_len, square_len), RectanglePlacement::centred, 1.0f);
	}
}

void BusInfoComponent::resized()
{
	m_driver_info_l->setBounds  (10,0,200,48);
	m_pal_info_l->setBounds     (10,45,200,48);
	m_vendor_info_l1->setBounds (210,10,getWidth()-320,24);
	m_vendor_info_l2->setBounds (210,35,getWidth()-320,24);
	m_vendor_info_l3->setBounds (210,60,getWidth()-320,24);
	updateInfo();
}

void BusInfoComponent::broughtToFront()
{
	lookAndFeelChanged();
}

void BusInfoComponent::lookAndFeelChanged()
{
	Colour backgroundColour = findColour(TCATLookAndFeel::tcatGlobalTabAreaColourId);

	repaint();

	m_driver_info_l->setColour (Label::backgroundColourId, backgroundColour);
	m_driver_info_l->setColour (Label::textColourId, backgroundColour.contrasting());
	m_pal_info_l->setColour (Label::backgroundColourId, backgroundColour);
	m_pal_info_l->setColour (Label::textColourId, backgroundColour.contrasting());
	m_vendor_info_l1->setColour (Label::backgroundColourId, backgroundColour);
	m_vendor_info_l1->setColour (Label::textColourId, backgroundColour.contrasting());
	m_vendor_info_l2->setColour (Label::backgroundColourId, backgroundColour);
	m_vendor_info_l2->setColour (Label::textColourId, backgroundColour.contrasting());
	m_vendor_info_l3->setColour (Label::backgroundColourId, backgroundColour);
	m_vendor_info_l3->setColour (Label::textColourId, backgroundColour.contrasting());
}

void BusInfoComponent::updateInfo()
{
	String info;
	info << "PAL version: \n";
	info << kTCAT_DICE_VERSION_EXTENDED_STR;
	m_pal_info_l->setText(info, false);

	info = "Driver version: \n";
	if (m_bus)
	{
			info << String::formatted ("%i.%i.%i.%i",
				TCAT_VERSION32_MAJOR(m_bus->driver_version()),
				TCAT_VERSION32_MINOR(m_bus->driver_version()),
				TCAT_VERSION32_SUB(m_bus->driver_version()),
				TCAT_VERSION32_BUILD(m_bus->driver_version())) <<
			(m_bus->driver_version()&kTCAT_DICE_VERSION_MASK_BUILDSERVER?"":" local") <<
			(m_bus->driver_version()&kTCAT_DICE_VERSION_MASK_BETA?" beta":"") <<
			(m_bus->driver_version()&kTCAT_DICE_VERSION_MASK_INSTRUMENTED?" instrumented":"");
	}
	else
	{
		info << "- unknown -  no devices found.";
	}

	m_driver_info_l->setText(info,false);

	m_vendor_info_l1->setText(appDefaults->getAppSettings()->getValue("vendorInfo1"), false);
	m_vendor_info_l2->setText(appDefaults->getAppSettings()->getValue("vendorInfo2"), false);
	m_vendor_info_l3->setText(appDefaults->getAppSettings()->getValue("vendorInfo3"), false);
}

void BusInfoComponent::mount(tcat::dice::reference<event_bus> bus)
{
	// call super's mount() first
	bus_listener::mount(bus);
	// which gives us an m_bus reference
	triggerAsyncUpdate();
}

void BusInfoComponent::unmount()
{
	// call super's umnount first
	bus_listener::unmount();
	triggerAsyncUpdate();
}

void BusInfoComponent::handleAsyncUpdate()
{
	updateInfo();
}
