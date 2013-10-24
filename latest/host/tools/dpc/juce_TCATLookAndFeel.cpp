/*
  ==============================================================================

   This file is part of the JUCE library - "Jules' Utility Class Extensions"
   Copyright 2004-6 by Raw Material Software ltd.

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

#include "DPCTool_headers.h"
#include "juce_TCATLookAndFeel.h"

//==============================================================================

//==============================================================================
const Colour TCATLookAndFeel::tcatSurround(168,186,219);
const Colour TCATLookAndFeel::tcatBackground(230,235,255);
const Colour TCATLookAndFeel::tcatHeading(117,141,255);


TCATLookAndFeel::TCATLookAndFeel()
{
	
    setColour (ComboBox::buttonColourId,                    Colour (0xffbbbbff));
    setColour (ComboBox::outlineColourId,                   Colours::grey.withAlpha (0.7f));

    setColour (ListBox::outlineColourId,            findColour (ComboBox::outlineColourId));
    setColour (ListBox::backgroundColourId,			tcatSurround);

    setColour (ScrollBar::backgroundColourId,       Colours::transparentBlack);
    setColour (ScrollBar::thumbColourId,            Colours::white);

    setColour (Slider::thumbColourId,               findColour (TextButton::buttonColourId));
    setColour (Slider::trackColourId,               Colour (0x7fffffff));
    setColour (Slider::textBoxOutlineColourId,      findColour (ComboBox::outlineColourId));

    setColour (ProgressBar::backgroundColourId,     Colours::white);
    setColour (ProgressBar::foregroundColourId,     Colour (0xffaaaaee));

    setColour (PopupMenu::backgroundColourId,             Colours::white);
    setColour (PopupMenu::highlightedTextColourId,        Colours::white);
    setColour (PopupMenu::highlightedBackgroundColourId,  Colour (0x991111aa));

    setColour (TextEditor::focusedOutlineColourId,  findColour (TextButton::buttonColourId));
}

TCATLookAndFeel::~TCATLookAndFeel()
{
}


static const Colour createBaseColour (const Colour& buttonColour,
                                      const bool hasKeyboardFocus,
                                      const bool isMouseOverButton,
                                      const bool isButtonDown)
{
    float sat = hasKeyboardFocus ? 1.3f : 0.9f;

    Colour baseColour (buttonColour.withMultipliedSaturation (sat));

    if (isButtonDown)
        return baseColour.contrasting (0.2f);
    else if (isMouseOverButton)
        return baseColour.contrasting (0.1f);

    return baseColour;
}

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

        g.setGradientFill(cg);
        g.fillPath (p);
    }

    g.setColour (Colours::black.withAlpha (0.5f * colour.getFloatAlpha()));
    g.strokePath (p, PathStrokeType (outlineThickness));
}


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

        drawShinyButtonShape (g,
                              (float) x, (float) y, sliderPos - (float) x, (float) height, 0.0f,
                              baseColour,
                              slider.isEnabled() ? 0.9f : 0.3f,
                              true, true, true, true);
    }
    else
    {
        const float sliderRadius = (float) getSliderThumbRadius (slider);

        const Colour trackColour (slider.findColour (Slider::trackColourId));
        const Colour gradCol1 (trackColour.overlaidWith (Colours::black.withAlpha (slider.isEnabled() ? 0.25f : 0.13f)));
        const Colour gradCol2 (trackColour.overlaidWith (Colours::black.withAlpha (0.08f)));
        Path indent;

        if (slider.isHorizontal())
        {
            const float iy = y + height * 0.5f - sliderRadius * 0.5f;
            const float ih = sliderRadius;

            ColourGradient cg (gradCol1, 0.0f, iy,
                              gradCol2, 0.0f, iy + ih, false);
            g.setGradientFill(cg);

            indent.addRoundedRectangle (x - sliderRadius * 0.5f, iy,
                                        width + sliderRadius, ih,
                                        5.0f);
            g.fillPath (indent);
        }
        else
        {
            const float ix = x + width * 0.5f - sliderRadius * 0.5f;
            const float iw = sliderRadius;

            ColourGradient cg (gradCol1, ix, 0.0f,
                              gradCol2, ix + iw, 0.0f, false);
            g.setGradientFill(cg);

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

            drawGlassSphere (g,
                             kx - sliderRadius,
                             ky - sliderRadius,
                             sliderRadius * 2.0f,
                             knobColour, outlineThickness);
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

