#pragma once

#include "DPCTool_headers.h"



class JUCE_API  TCATLookAndFeel  : public LookAndFeel
{
public:
    //==============================================================================
    /** Creates a TCATLookAndFeel look and feel object. */
    TCATLookAndFeel();

    static void drawGlassSphere (Graphics& g, float x, float y, float diameter,
                                 const Colour& colour, const float outlineThickness);

    static void drawGlassPointer (Graphics& g, float x, float y, float diameter,
                                  const Colour& colour, const float outlineThickness,
                                  const int direction);

    void drawLinearSlider (Graphics& g,
                           int x, int y,
                           int width, int height,
                           float sliderPos,
                           float minSliderPos,
                           float maxSliderPos,
                           const Slider::SliderStyle style,
                           Slider& slider);

    int getSliderThumbRadius (Slider& slider);

    enum ColourIds
    {
        tcattcatSurroundColourId     = 0x2000001,
        tcatBackgroundColourId       = 0x2000002,
        tcatHeadingColourId       = 0x2000003
    };

    static const Colour tcatSurround,tcatBackground,tcatHeading;
    

    /** Destructor. */
    virtual ~TCATLookAndFeel();

private:
    void drawShinyButtonShape (Graphics& g,
                               float x, float y, float w, float h, float maxCornerSize,
                               const Colour& baseColour,
                               const float strokeWidth,
                               const bool flatOnLeft,
                               const bool flatOnRight,
                               const bool flatOnTop,
                               const bool flatOnBottom);

};


