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

#include "juce_TCATLookAndFeel.h"

//==============================================================================
TCATLookAndFeel::TCATLookAndFeel()
{
}

TCATLookAndFeel::~TCATLookAndFeel()
{
}

//==============================================================================

void TCATLookAndFeel::createRoundedPath (Path& p,
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

const Colour TCATLookAndFeel::createBaseColour (const Colour& buttonColour,
												  const bool hasKeyboardFocus,
												  const bool isMouseOverButton,
												  const bool isButtonDown)
{
	const float sat = hasKeyboardFocus ? 1.3f : 0.9f;
	const Colour baseColour (buttonColour.withMultipliedSaturation (sat));

	if (isButtonDown)
		return baseColour.contrasting (0.2f);
	else if (isMouseOverButton)
		return baseColour.contrasting (0.1f);

	return baseColour;
}

//==============================================================================

void TCATLookAndFeel::drawGradientLozenge (Graphics& g,
                                    const float x, const float y,
                                    const float width, const float height,
                                    const Colour& colour,
                                    const float outlineThickness,
                                    const float cornerSize,
                                    const bool flatOnLeft,
                                    const bool flatOnRight,
                                    const bool flatOnTop,
                                    const bool flatOnBottom,
									const bool invert) noexcept
{
    if (width <= outlineThickness || height <= outlineThickness)
        return;

    const int intX = (int) x;
    const int intY = (int) y;
    const int intW = (int) width;
    const int intH = (int) height;

    const float cs = cornerSize < 0 ? jmin (width * 0.5f, height * 0.5f) : cornerSize;
    const float edgeBlurRadius = height * 0.75f + (height - cs * 2.0f);
    const int intEdge = (int) edgeBlurRadius;

    Path outline;
    createRoundedPath (outline, x, y, width, height, cs,
                                            ! (flatOnLeft || flatOnTop),
                                            ! (flatOnRight || flatOnTop),
                                            ! (flatOnLeft || flatOnBottom),
                                            ! (flatOnRight || flatOnBottom));

	// base fill
    {
		ColourGradient cg (colour.darker (0.2f), 0, y + height,
						   colour.darker (0.2f), 0, y, false);

//		cg.addColour (0.03, colour.withMultipliedAlpha (0.3f));
		cg.addColour (0.4, colour);
		if (!invert)
		{
			cg.addColour (0.97, colour.withMultipliedAlpha (0.5f));
		}

		g.setGradientFill (cg);
		g.fillPath (outline);
    }

    ColourGradient cg (Colours::transparentBlack, x + edgeBlurRadius, y + height * 0.5f,
                       colour.darker (0.2f), x, y + height * 0.5f, true);

    cg.addColour (jlimit (0.0, 1.0, 1.0 - (cs * 0.5f) / edgeBlurRadius), Colours::transparentBlack);
    cg.addColour (jlimit (0.0, 1.0, 1.0 - (cs * 0.25f) / edgeBlurRadius), colour.darker (0.2f).withMultipliedAlpha (0.3f));

	// left blur
	if (! (flatOnLeft || flatOnTop || flatOnBottom))
    {
        g.saveState();
        g.setGradientFill (cg);
        g.reduceClipRegion (intX, intY, intEdge, intH);
        g.fillPath (outline);
        g.restoreState();
    }

	// right blur
    if (! (flatOnRight || flatOnTop || flatOnBottom))
    {
        cg.point1.setX (x + width - edgeBlurRadius);
        cg.point2.setX (x + width);

        g.saveState();
        g.setGradientFill (cg);
        g.reduceClipRegion (intX + intW - intEdge, intY, 2 + intEdge, intH);
        g.fillPath (outline);
        g.restoreState();
    }

    g.setColour (colour.darker().withMultipliedAlpha (1.5f));
    g.strokePath (outline, PathStrokeType (outlineThickness));
}


void TCATLookAndFeel::drawButtonBackground (Graphics& g,
                                        Button& button,
                                        const Colour& backgroundColour,
                                        bool isMouseOverButton,
                                        bool isButtonDown)
{
	const int width = button.getWidth();
	const int height = button.getHeight();

	const float outlineThickness = button.isEnabled() ? ((isButtonDown || isMouseOverButton) ? 1.2f : 0.7f) : 0.4f;
	const float halfThickness = outlineThickness * 0.5f;

	const float indentL = button.isConnectedOnLeft()   ? 0.1f : halfThickness;
	const float indentR = button.isConnectedOnRight()  ? 0.1f : halfThickness;
	const float indentT = button.isConnectedOnTop()    ? 0.1f : halfThickness;
	const float indentB = button.isConnectedOnBottom() ? 0.1f : halfThickness;

	const Colour baseColour (createBaseColour (backgroundColour,
		button.hasKeyboardFocus (true),
		isMouseOverButton, isButtonDown)
		.withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));

	drawGradientLozenge (g,
		indentL,
		indentT,
		width - indentL - indentR,
		height - indentT - indentB,
		baseColour, outlineThickness, 5.0f,
		button.isConnectedOnLeft(),
		button.isConnectedOnRight(),
		button.isConnectedOnTop(),
		button.isConnectedOnBottom(),
		isButtonDown);
}


void TCATLookAndFeel::drawToggleButton (Graphics& g,
                                    ToggleButton& button,
                                    bool isMouseOverButton,
                                    bool isButtonDown)
{
    if (button.hasKeyboardFocus (true))
    {
        g.setColour (button.findColour (TextEditor::focusedOutlineColourId));
        g.drawRect (0, 0, button.getWidth(), button.getHeight());
    }

    float fontSize = jmin (15.0f, button.getHeight() * 0.75f);
    const float tickWidth = fontSize * 1.1f;

    drawTickBox (g, button, 4.0f, (button.getHeight() - tickWidth) * 0.5f,
                 tickWidth, tickWidth,
                 button.getToggleState(),
                 button.isEnabled(),
                 isMouseOverButton,
                 isButtonDown);

    g.setColour (button.findColour (ToggleButton::textColourId));
    g.setFont (fontSize);

    if (! button.isEnabled())
        g.setOpacity (0.5f);

    const int textX = (int) button.getHeight() + 3;

    g.drawFittedText (button.getButtonText(),
                      textX, 0,
                      button.getWidth() - textX - 2, button.getHeight(),
                      Justification::centredLeft, 10);
}

void TCATLookAndFeel::drawTickBox (Graphics& g,
									 Component& component,
									 float x, float y, float w, float h,
									 const bool ticked,
									 bool isEnabled,
									 bool isMouseOverButton,
									 bool isButtonDown)
{
	const int height = component.getHeight()-4;
	const int width = height;

	const float outlineThickness = isEnabled ? ((isButtonDown || isMouseOverButton) ? 1.2f : 0.7f) : 0.4f;

	const float indentL = 3.0f;
	const float indentR = 1.0f;
	const float indentT = 3.0f;
	const float indentB = 1.0f;

	Colour backgroundColour = TCATLookAndFeel::findColour(TextButton::buttonColourId);

	const Colour baseColour (createBaseColour (backgroundColour,
		component.hasKeyboardFocus (true),
		isMouseOverButton, isButtonDown)
		.withMultipliedAlpha (component.isEnabled() ? 1.0f : 0.5f));

	drawGradientLozenge (g,
		indentL,
		indentT,
		width - indentL - indentR,
		height - indentT - indentB,
		baseColour, outlineThickness, 2.0f,
		false, false, false, false,
		isButtonDown || ticked);

    if (ticked)
    {
		g.setColour(createBaseColour (baseColour.darker(2.0f)
								.withMultipliedAlpha (isEnabled ? 1.0f : 0.5f),
								isEnabled,
								false,
								false));

		// checkmark "x"
		float boxLen = (float)width-indentR-indentL;
		float inset = boxLen/4.0f;
		float len = boxLen - inset*2.0f;

		Path tick;
		tick.startNewSubPath (indentL+inset+len, indentT+inset);
		tick.lineTo (indentL+inset, indentT+inset+len);
		tick.startNewSubPath (indentL+inset, indentT+inset);
		tick.lineTo (indentL+inset+len, indentT+inset+len);
		tick.closeSubPath();
		float thickness = len/7.0f;
		g.strokePath (tick, PathStrokeType (thickness, PathStrokeType::mitered, PathStrokeType::rounded));
	}
}

//==============================================================================
void TCATLookAndFeel::drawStretchableLayoutResizerBar (Graphics& g,
														 int w, int h,
														 bool /*isVerticalBar*/,
														 bool isMouseOver,
														 bool isMouseDragging)
{
	float alpha = 0.75f;

	if (isMouseOver || isMouseDragging)
	{
		g.fillAll (Colour (0xffe3e3e3));
		alpha = 1.0f;
	}
	else
	{
		g.fillAll(Colour(0xfffbfbfb));
	}

	const float cx = w * 0.5f;
	const float cy = h * 0.5f;
	const float cr = jmin (w, h) * 0.4f;

	g.setGradientFill (ColourGradient (Colours::white.withAlpha (alpha), cx + cr * 0.1f, cy - 15 + cr,
		Colours::black.withAlpha (alpha), cx, cy - 15 - cr * 4.0f,
		true));
	g.fillEllipse (cx - cr, cy - 15 - cr, cr * 2.0f, cr * 2.0f);

	g.setGradientFill (ColourGradient (Colours::white.withAlpha (alpha), cx + cr * 0.1f, cy + cr,
		Colours::black.withAlpha (alpha), cx, cy - cr * 4.0f,
		true));
	g.fillEllipse (cx - cr, cy - cr, cr * 2.0f, cr * 2.0f);

	g.setGradientFill (ColourGradient (Colours::white.withAlpha (alpha), cx + cr * 0.1f, cy + 15 + cr,
		Colours::black.withAlpha (alpha), cx, cy + 15 - cr * 4.0f,
		true));
	g.fillEllipse (cx - cr, cy + 15 - cr, cr * 2.0f, cr * 2.0f);
}

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


void TCATLookAndFeel::drawLabel (Graphics& g, Label& label)
{
	g.fillAll (label.findColour (Label::backgroundColourId));

	if (! label.isBeingEdited())
	{
		const float alpha = label.isEnabled() ? 1.0f : 0.5f;

		g.setColour (label.findColour (Label::textColourId).withMultipliedAlpha (alpha));
		g.setFont (label.getFont());
		g.drawText (label.getText(),
			label.getHorizontalBorderSize(),
			label.getVerticalBorderSize(),
			label.getWidth() - 2 * label.getHorizontalBorderSize(),
			label.getHeight() - 2 * label.getVerticalBorderSize(),
			label.getJustificationType(),
			true);

		g.setColour (label.findColour (Label::outlineColourId).withMultipliedAlpha (alpha));
		g.drawRect (0, 0, label.getWidth(), label.getHeight());
	}
	else if (label.isEnabled())
	{
		g.setColour (label.findColour (Label::outlineColourId));
		g.drawRect (0, 0, label.getWidth(), label.getHeight());
	}
}

const Typeface::Ptr TCATLookAndFeel::getTypefaceForFont( const Font & font)
{
	Font f(font);
	f.setHeight(10.0f);
	return Typeface::createSystemTypefaceFor(f);
}

int TCATLookAndFeel::getTabButtonOverlap (int tabDepth)
{
	return 0;
}

void TCATLookAndFeel::createTabButtonShape (Path& p,
                                        int width, int height,
                                        int tabIndex,
                                        const String& text,
                                        Button& button,
                                        TabbedButtonBar::Orientation orientation,
                                        bool isMouseOver,
                                        bool isMouseDown,
                                        bool isFrontTab)
{
    const float w = (float) width;
    const float h = (float) height;

    if (orientation == TabbedButtonBar::TabsAtTop)
    {
		p.addRoundedRectangle(0.0f, 0.0f, w, 10, 3.0f);
		p.addRectangle(0, 5, w, h-5);
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
                                      int tabIndex,
                                      const String& tabStr,
                                      Button& button,
                                      TabbedButtonBar::Orientation /*orientation*/,
                                      bool isMouseOverButton,
                                      bool isButtonDown,
                                      bool isFrontTab)
{
	float h = (float)button.getHeight();

	if (isFrontTab)
	{
		ColourGradient cg (preferredColour, 0, 0, preferredColour.darker(0.4f), 0, h, false);
		g.setGradientFill(cg);
	}
	else
	{
		ColourGradient cg (preferredColour.darker(0.1f), 0, 0, preferredColour.darker(0.5f), 0, h, false);
		g.setGradientFill(cg);
	}
	g.fillPath(path);
}

void TCATLookAndFeel::drawTabAreaBehindFrontButton (Graphics& g,
                                                int w, int h,
                                                TabbedButtonBar& tabBar,
                                                TabbedButtonBar::Orientation orientation)
{
	return;
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
	const float slotIndentx2 = slotIndent * 2.0f;
	const float thumbIndent = slotIndent + 2.0f;
	const float thumbIndentx2 = thumbIndent * 2.0f;

	float gx1 = 0.0f, gy1 = 0.0f, gx2 = 0.0f, gy2 = 0.0f;

	if (isScrollbarVertical)
	{
		slotPath.addRoundedRectangle (x + slotIndent,
			y + slotIndent,
			width - slotIndentx2,
			height - slotIndentx2,
			(width - slotIndentx2) * 0.5f);

		if (thumbSize > 0)
			thumbPath.addRoundedRectangle (x + thumbIndent,
			thumbStartPosition + thumbIndent,
			width - thumbIndentx2,
			thumbSize - thumbIndentx2,
			(width - thumbIndentx2) * 0.5f);
		gx1 = (float) x;
		gx2 = x + width * 0.7f;
	}
	else
	{
		slotPath.addRoundedRectangle (x + slotIndent,
			y + slotIndent,
			width - slotIndentx2,
			height - slotIndentx2,
			(height - slotIndentx2) * 0.5f);

		if (thumbSize > 0)
			thumbPath.addRoundedRectangle (thumbStartPosition + thumbIndent,
			y + thumbIndent,
			thumbSize - thumbIndentx2,
			height - thumbIndentx2,
			(height - thumbIndentx2) * 0.5f);
		gy1 = (float) y;
		gy2 = y + height * 0.7f;
	}

	const Colour thumbColour (scrollbar.findColour (ScrollBar::thumbColourId));
	Colour trackColour1, trackColour2;

	if (scrollbar.isColourSpecified (ScrollBar::trackColourId))
	{
		trackColour1 = trackColour2 = scrollbar.findColour (ScrollBar::trackColourId);
	}
	else
	{
		trackColour1 = thumbColour.overlaidWith (Colour (0x0f000000));
		trackColour2 = thumbColour.overlaidWith (Colour (0x02000000));
	}

	g.setGradientFill (ColourGradient (trackColour1, gx1, gy1,
		trackColour2, gx2, gy2, false));
	g.fillPath (slotPath);

	g.setColour(Colours::grey);
	g.fillPath(thumbPath);
}

void TCATLookAndFeel::drawLinearSliderBackground (Graphics& g,
													int x, int y,
													int width, int height,
													float /*sliderPos*/,
													float /*minSliderPos*/,
													float /*maxSliderPos*/,
													const Slider::SliderStyle /*style*/,
													Slider& slider)
{
	const float sliderRadius = (float) (getSliderThumbRadius (slider) - 2);

	const Colour trackColour (slider.findColour (Slider::trackColourId));
	const Colour gradCol1 (trackColour.overlaidWith (Colours::black.withAlpha (slider.isEnabled() ? 0.25f : 0.13f)));
	const Colour gradCol2 (trackColour.overlaidWith (Colour (0x14000000)));
	Path indent;

	if (slider.isHorizontal())
	{
		const float iy = y + height * 0.5f - sliderRadius * 0.5f;
		const float ih = sliderRadius;

		g.setGradientFill (ColourGradient (gradCol1, 0.0f, iy,
			gradCol2, 0.0f, iy + ih, false));

		indent.addRoundedRectangle (x - sliderRadius * 0.5f, iy,
			width + sliderRadius, ih,
			5.0f);
		g.fillPath (indent);
	}
	else
	{
		const float ix = x + width * 0.5f - sliderRadius * 0.5f;
		const float iw = sliderRadius;

		g.setGradientFill (ColourGradient (gradCol1, ix, 0.0f,
			gradCol2, ix + iw, 0.0f, false));

		indent.addRoundedRectangle (ix, y - sliderRadius * 0.5f,
			iw, height + sliderRadius,
			5.0f);
		g.fillPath (indent);
	}

	g.setColour (Colour (0x4c000000));
	g.strokePath (indent, PathStrokeType (0.5f));
}

//==============================================================================
void TCATLookAndFeel::drawGradientSphere (Graphics& g,
                                   const float x, const float y,
                                   const float diameter,
                                   const Colour& colour,
                                   const float outlineThickness)
{
    if (diameter <= outlineThickness)
        return;

    Path p;
    p.addEllipse (x, y, diameter, diameter);

    {
        ColourGradient cg (Colours::white.overlaidWith (colour.withMultipliedAlpha (0.3f)), 0, y,
                            Colours::white.overlaidWith (colour), 0, y + diameter*2, false);
//	                          Colours::white.overlaidWith (colour.withMultipliedAlpha (0.3f)), 0, y + diameter*2, false);

//        cg.addColour (0.4, Colours::white.overlaidWith (colour));

        g.setGradientFill (cg);
        g.fillPath (p);
    }

    g.setGradientFill (ColourGradient (Colours::white, 0, y + diameter * 0.06f,
                                       Colours::transparentWhite, 0, y + diameter * 0.3f, false));
    g.fillEllipse (x + diameter * 0.2f, y + diameter * 0.05f, diameter * 0.6f, diameter * 0.4f);

    ColourGradient cg (Colours::transparentBlack,
                       x + diameter * 0.5f, y + diameter * 0.5f,
                       Colours::black.withAlpha (0.5f * outlineThickness * colour.getFloatAlpha()),
                       x, y + diameter * 0.5f, true);

    cg.addColour (0.7, Colours::transparentBlack);
    cg.addColour (0.8, Colours::black.withAlpha (0.1f * outlineThickness));

    g.setGradientFill (cg);
    g.fillPath (p);

    g.setColour (Colours::black.withAlpha (0.5f * colour.getFloatAlpha()));
    g.drawEllipse (x, y, diameter, diameter, outlineThickness);
}

void TCATLookAndFeel::drawLinearSliderThumb (Graphics& g,
											   int x, int y,
											   int width, int height,
											   float sliderPos,
											   float minSliderPos,
											   float maxSliderPos,
											   const Slider::SliderStyle style,
											   Slider& slider)
{
	const float sliderRadius = (float) (getSliderThumbRadius (slider) - 2);

	Colour knobColour (createBaseColour (slider.findColour (Slider::thumbColourId),
		slider.hasKeyboardFocus (false) && slider.isEnabled(),
		slider.isMouseOverOrDragging() && slider.isEnabled(),
		slider.isMouseButtonDown() && slider.isEnabled()));

	const float outlineThickness = slider.isEnabled() ? 0.8f : 0.3f;

	if (style == Slider::LinearHorizontal || style == Slider::LinearVertical)
	{
		float kx, ky;

		if (style == Slider::LinearVertical)
		{
			kx = x + width * 0.5f;
			ky = sliderPos;
		}
		else
		{
			kx = sliderPos;
			ky = y + height * 0.5f;
		}

		drawGradientSphere (g,
			kx - sliderRadius,
			ky - sliderRadius,
			sliderRadius * 2.0f,
			knobColour, outlineThickness);
	}
	else
	{
		if (style == Slider::ThreeValueVertical)
		{
			drawGradientSphere (g, x + width * 0.5f - sliderRadius,
				sliderPos - sliderRadius,
				sliderRadius * 2.0f,
				knobColour, outlineThickness);
		}
		else if (style == Slider::ThreeValueHorizontal)
		{
			drawGradientSphere (g,sliderPos - sliderRadius,
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

	if (style == Slider::LinearBar)
	{
		const bool isMouseOver = slider.isMouseOverOrDragging() && slider.isEnabled();

		Colour baseColour (createBaseColour (slider.findColour (Slider::thumbColourId)
			.withMultipliedSaturation (slider.isEnabled() ? 1.0f : 0.5f),
			false, isMouseOver,
			isMouseOver || slider.isMouseButtonDown()));
	}
	else
	{
		drawLinearSliderBackground (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
		drawLinearSliderThumb (g, x, y, width, height, sliderPos, minSliderPos, maxSliderPos, style, slider);
	}
}

void TCATLookAndFeel::drawMenuBarBackground (Graphics& g, int width, int height, bool, MenuBarComponent& menuBar)
{
	const Colour baseColour (createBaseColour (menuBar.findColour (PopupMenu::backgroundColourId), false, false, false));

	if (menuBar.isEnabled())
	{
		Path p;
		ColourGradient cg (baseColour, 0, 0, baseColour.darker(0.05f), 0, (float)height, false);
		g.setGradientFill(cg);
		p.addRectangle(0, 0, (float)width, (float)height);
		g.fillPath(p);
/*
		drawShinyButtonShape (g,
			-4.0f, 0.0f,
			width + 8.0f, (float) height,
			0.0f,
			baseColour,
			0.4f,
			true, true, true, true);
*/
	}
	else
	{
		g.fillAll (baseColour);
	}
}


//==============================================================================

