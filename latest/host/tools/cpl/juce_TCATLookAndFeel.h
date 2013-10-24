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

#pragma once

#include "JuceHeader.h"

//==============================================================================
/**
 To make this the default look for your app, just set it as the default in
 your initialisation code.
 
 e.g. @code
 void initialise (const String& commandLine)
 {
 static TCATLookAndFeel tcatLookAndFeel;
 LookAndFeel::setDefaultLookAndFeel (&tcatLookAndFeel);
 }
 @endcode
*/
class TCATLookAndFeel  : public LookAndFeel, public DeletedAtShutdown
{
public:
	
	juce_DeclareSingleton (TCATLookAndFeel, true);
	
    //==============================================================================
    /** Creates a TCATLookAndFeel look and feel object. */
    TCATLookAndFeel();
	
    /** Destructor. */
	~TCATLookAndFeel();
	
    //==============================================================================
	void initColours();
	void dumpColours();
	Colour getAppDefaultColour(const String &colourStr);
	void stepShade(int nSteps);
	void setAppDefaultSpecifiedColours(void);
	void setAppDefaultShadedColours(void);
	void setAppDefaultBlendedColours(void);
	
    //==============================================================================
    void drawBubble (Graphics& g,
                     float tipX, float tipY,
                     float boxX, float boxY,
                     float boxW, float boxH);
	
	void getTooltipSize (const String& tipText, int& width, int& height);
	void drawTooltip (Graphics& g, const String& text, int width, int height);
	
    //==============================================================================
    void drawTextEditorOutline (Graphics& g,
                                int width, int height,
                                TextEditor& textEditor);
	
    //==============================================================================
    void drawComboBox (Graphics& g, int width, int height,
                       bool isButtonDown,
                       int buttonX, int buttonY,
                       int buttonW, int buttonH,
                       ComboBox& box);
	
    const Font getComboBoxFont (ComboBox& box);
	
    //==============================================================================
    void drawScrollbarButton (Graphics& g,
                              ScrollBar& scrollbar,
                              int width, int height,
                              int buttonDirection,
                              bool isScrollbarVertical,
                              bool isMouseOverButton,
                              bool isButtonDown);
	
    void drawScrollbar (Graphics& g,
                        ScrollBar& scrollbar,
                        int x, int y,
                        int width, int height,
                        bool isScrollbarVertical,
                        int thumbStartPosition,
                        int thumbSize,
                        bool isMouseOver,
                        bool isMouseDown);
	
    ImageEffectFilter* getScrollbarEffect();
	
    //==============================================================================
    void drawButtonBackground (Graphics& g,
                               Button& button,
                               const Colour& backgroundColour,
                               bool isMouseOverButton,
                               bool isButtonDown);
	
    void drawTickBox (Graphics& g,
                      Component& component,
                      float x, float y, float w, float h,
                      bool ticked,
                      bool isEnabled,
                      bool isMouseOverButton,
                      bool isButtonDown);
	
    //==============================================================================
	void drawLinearSlider (Graphics& g,
						   int x, int y,
						   int width, int height,
						   float sliderPos,
						   float minSliderPos,
						   float maxSliderPos,
						   const Slider::SliderStyle style,
						   Slider& slider);
	
    int getSliderThumbRadius (Slider& slider);
	
    Button* createSliderButton (bool isIncrement);
	
    ImageEffectFilter* getSliderEffect();
	
	void drawSliderThumb (Graphics& g,
						  Component& component,
						  int x, int y, int w, int h,
						  const bool isEnabled,
						  const bool isMouseOverButton,
						  const bool isButtonDown);
	
	Label* createSliderTextBox (Slider& slider);
	
    void drawRotarySlider (Graphics& g,
						   int x, int y,
						   int width, int height,
						   float sliderPosProportional,
						   float rotaryStartAngle,
						   float rotaryEndAngle,
						   Slider& slider);
	//==============================================================================
 	const Font getPopupMenuFont();
	
    void drawPopupMenuBackground (Graphics& g, int width, int height);
	
    void drawMenuBarBackground (Graphics& g, int width, int height,
                                bool isMouseOverBar, MenuBarComponent& menuBar);
	
	
    //==============================================================================
	void positionDocumentWindowButtons (DocumentWindow& window,
										int titleBarX, int titleBarY,
										int titleBarW, int titleBarH,
										Button* minimiseButton,
										Button* maximiseButton,
										Button* closeButton,
										bool positionTitleBarButtonsOnLeft);
	
	Button* createDocumentWindowButton (int buttonType);
	
    //==============================================================================
    void drawCornerResizer (Graphics& g,
                            int w, int h,
                            bool isMouseOver,
                            bool isMouseDragging);
	
    //==============================================================================
    /** Utility function to draw a shiny, glassy circle (for round LED-type buttons). */
    static void drawGlassSphere (Graphics& g, float x, float y, float diameter,
								 const Colour& colour, const float outlineThickness);
	
	static void drawGlassPointer (Graphics& g, float x, float y, float diameter,
								  const Colour& colour, const float outlineThickness,
								  const int direction);
	
    /** Utility function to draw a basic oblong (for text buttons). */
    static void drawOblong (Graphics& g, float x, float y, float width, float height,
							const Colour& colour, const float outlineThickness, const float cornerSize,
							const bool flatOnLeft, const bool flatOnRight, const bool flatOnTop, const bool flatOnBottom,
							const bool isEnabled, const bool isMouseOverButton, const bool isButtonDown);
	
	
    //==============================================================================
	void drawDocumentWindowTitleBar (DocumentWindow& window,
									 Graphics& g, int w, int h,
									 int titleSpaceX, int titleSpaceW,
									 const Image* icon,
									 bool drawTitleTextOnLeft);
	
	//==============================================================================
	enum ColourIds
	{
		tcatTitleBarColourId =				0x3000001,
		tcatTitleBarButtonColourId =		0x3000002,
		tcatWindowBackgroundColourId =		0x3000003,
		tcatPopupTitleBarColourId =			0x3000004,
		tcatPopupWindowBackgroundColourId =	0x3000005,
		tcatGlobalAreaBackgroundColourId =	0x3000006,
		tcatGlobalHeadingColourId =			0x3000007,
		tcatGlobalTabAreaColourId =			0x3000008,
		tcatGlobalTabButtonColourId =		0x3000009,
		tcatDeviceAreaBackgroundColourId =	0x3000010,
		tcatDeviceHeadingColourId =			0x3000011,
		tcatDevicesHeadingColourId =		0x3000012,
		tcatDeviceTabAreaColourId =			0x3000013,
		tcatDeviceTabButtonColourId =		0x3000014,
		tcatDeviceBackgroundColourId =		0x3000015,
		tcatDeviceOutlineColourId =			0x3000016,
		tcatPopupMenuBackgroundColourId =	0x3000017,
		tcatTextboxBackgroundColourId =		0x3000018,
		tcatTextboxOutlineColourId =		0x3000019,
		tcatComboBackgroundColourId =		0x3000020,
		tcatComboOutlineColourId =			0x3000021,
		tcatTextButtonColourId =			0x3000022,
		tcatToggleButtonColourId =			0x3000023,
		tcatListboxBackgroundColourId =		0x3000024,
		tcatListboxOutlineColourId =		0x3000025,
		tcatGroupOutlineColourId =			0x3000026,
		tcatToolTipBackgroundColourId =		0x3000027,
		tcatToolTipOutlineColourId =		0x3000028,
		tcatToolTipTextColourId =			0x3000029,
		tcatProgressBarBackgroundColourId =	0x3000030,
		tcatProgressBarForegroundColourId =	0x3000031
	};
	
    //==============================================================================
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
							 const Colour& preferredBackgroundColour,
							 int tabIndex,
							 const String& text,
							 Button& button,
							 TabbedButtonBar::Orientation orientation,
							 bool isMouseOver,
							 bool isMouseDown,
							 bool isFrontTab);
	
    void drawTabButtonText (Graphics& g,
							int x, int y, int w, int h,
							const Colour& preferredBackgroundColour,
							int tabIndex,
							const String& text,
							Button& button,
							TabbedButtonBar::Orientation orientation,
							bool isMouseOver,
							bool isMouseDown,
							bool isFrontTab);
	
    int getTabButtonOverlap (int tabDepth);
	
	void drawTabAreaBehindFrontButton (Graphics& g,
									   int w, int h,
									   TabbedButtonBar& tabBar,
									   TabbedButtonBar::Orientation orientation);
	
	//==============================================================================
    juce_UseDebuggingNewOperator
	
private:
	bool m_use_grey;
	Colour m_shade;
	void drawShinyButtonShape (Graphics& g,
							   float x, float y, float w, float h, float maxCornerSize,
							   const Colour& baseColour,
							   const float strokeWidth,
							   const bool flatOnLeft,
							   const bool flatOnRight,
							   const bool flatOnTop,
							   const bool flatOnBottom);
	
};

