///////////////////////////////////////////////////////////////////////////////
//
// This confidential and proprietary software may be used only as authorised
// by a licensing agreement from TC Applied Technologies Ltd.
//
// (C) COPYRIGHT 2010 - 2011 TC Applied Technologies Ltd. (ALL RIGHTS RESERVED)
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
class JUCE_API  TCATLookAndFeel  : public LookAndFeel
{
public:
	//==============================================================================
	/** Creates a TCATLookAndFeel look and feel object. */
	TCATLookAndFeel();

	/** Destructor. */
	~TCATLookAndFeel();

	void createRoundedPath (Path& p,
		const float x, const float y,
		const float w, const float h,
		const float cs,
		const bool curveTopLeft, const bool curveTopRight,
		const bool curveBottomLeft, const bool curveBottomRight);

	const Colour createBaseColour (const Colour& buttonColour,
		const bool hasKeyboardFocus,
		const bool isMouseOverButton,
		const bool isButtonDown);

	void drawGradientLozenge (Graphics& g,
		const float x, const float y,
		const float width, const float height,
		const Colour& colour,
		const float outlineThickness,
		const float cornerSize,
		const bool flatOnLeft,
		const bool flatOnRight,
		const bool flatOnTop,
		const bool flatOnBottom,
		const bool invert) noexcept;

	void drawButtonBackground (Graphics& g,
		Button& button,
		const Colour& backgroundColour,
		bool isMouseOverButton,
		bool isButtonDown);

	void drawToggleButton (Graphics& g,
		ToggleButton& button,
		bool isMouseOverButton,
		bool isButtonDown);

	void drawTickBox (Graphics& g,
		Component& component,
		float x, float y, float w, float h,
		bool ticked,
		bool isEnabled,
		bool isMouseOverButton,
		bool isButtonDown);

	void drawStretchableLayoutResizerBar (Graphics& g,
		int w, int h,
		bool /*isVerticalBar*/,
		bool isMouseOver,
		bool isMouseDragging);

	void getTooltipSize (const String& tipText, int& width, int& height);
	void drawTooltip (Graphics& g, const String& text, int width, int height);
	void drawLabel (Graphics& g, Label& label); // draw text without horizontal scaling
	const Typeface::Ptr getTypefaceForFont( const Font & font);

	int getTabButtonOverlap (int tabDepth);

	void createTabButtonShape (Path& p,
		int width, int height,
		int tabIndex,
		const String& text,
		Button& button,
		TabbedButtonBar::Orientation orientation,
		bool isMouseOver,
		bool isMouseDown,
		bool isFrontTab);

	void fillTabButtonShape (Graphics& g,
		const Path& path,
		const Colour& preferredColour,
		int tabIndex,
		const String& tabStr,
		Button& button,
		TabbedButtonBar::Orientation orientation,
		bool isMouseOverButton,
		bool isButtonDown,
		bool isFrontTab);

	void drawTabAreaBehindFrontButton (Graphics& g,
		int w, int h,
		TabbedButtonBar& tabBar,
		TabbedButtonBar::Orientation orientation);

	void drawScrollbar	(Graphics & g, ScrollBar & scrollbar,
		int x, int y, int width, int height,
		bool isScrollbarVertical,
		int thumbStartPosition, int thumbSize,
		bool isMouseOver, bool isMouseDown );

	void drawLinearSliderBackground (Graphics& g,
		int x, int y,
		int width, int height,
		float sliderPos,
		float minSliderPos,
		float maxSliderPos,
		const Slider::SliderStyle style,
		Slider& slider);

	void drawGradientSphere (Graphics& g,
		const float x, const float y,
		const float diameter,
		const Colour& colour,
		const float outlineThickness);

	void drawLinearSliderThumb (Graphics& g,
		int x, int y,
		int width, int height,
		float sliderPos,
		float minSliderPos,
		float maxSliderPos,
		const Slider::SliderStyle style,
		Slider& slider);

	void drawLinearSlider (Graphics& g,
		int x, int y,
		int width, int height,
		float sliderPos,
		float minSliderPos,
		float maxSliderPos,
		const Slider::SliderStyle style,
		Slider& slider);

	void drawMenuBarBackground	(Graphics & g,
		int width,
		int height,
		bool isMouseOverBar,
		MenuBarComponent & menuBar);
};



