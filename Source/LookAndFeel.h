// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

// Kontrast - nach hinten weniger
// Helligkeit - nach vorn mehr, exponentiell (bzgl. select)

namespace lnf
{
	namespace dims
	{
		// widths
		static const int w( 18 );
		static const int wS( 36 );
		static const int wM( 80 );
		static const int wL( 150 );
		static const int wXL( 200 );
		static const int wXXL( 320 );

		// heights
		static const int h( 18 );
		static const int hS( 24 );
		static const int hM( 80 );
		static const int hL( 120 ); // for samples or curves
		static const int hXL( 240 );
		static const int hXXL( 360 );

		// padding
		static const int pad( 1 ); // inner bounds
		static const int padM( 4 ); // outer bounds
		static const int padL( 8 ); // large bounds
	}
	// greys
	static const Colour greyBgFar( 0xff323131 );
	static const Colour greyBgMid( 0xff3b3a3a );
	static const Colour greyBgNear( 0xff6b6b6b );
	static const Colour greyBgActive( 0xff7e7e7e );
	static const Colour greyFgInactive( greyBgMid );
	static const Colour greyFg( 0xff4a4a4a );
	static const Colour greyFgActive( greyBgActive );
	static const Colour greyFgActive2( 0xffbababa );
	static const Colour greyFgHighlight( 0xffd6d6d6 );
	static const Colour greyFgOutline( 0xffd6d6d6 );

	// blues
	static const Colour blueBg( 0xff000077 );
	static const Colour blueBgActive( 0xff0000dd );
	static const Colour blueFg( 0xff0000ff );
	static const Colour blueFgActive( 0xffffffff );
	static const Colour blueOutline( 0xffffffff );

	// reds
	static const Colour redBg( 0xff770000 );
	static const Colour redBgActive( 0xffdd0000 );
	static const Colour redFg( 0xffff0000 );
	static const Colour redFgActive( 0xffffffff );
	static const Colour redOutline( 0xffffffff );

	// greens
	static const Colour greenBg( 0xff00aa00 );
	static const Colour greenBgActive( 0xff00dd00 );
	static const Colour greenFg( 0xff00ff00 );
	static const Colour greenFgActive( 0xffffffff );
	static const Colour greenOutline( 0xffffffff );
	
	// playhead and timescale
	static const double defaultViewLength( 1. ); // secs
	static const double minimumViewLength( 0.001 ); // secs
	static const double maximumViewLength( 86400. ); // secs, 24h
	static const float playHeadWidth( 1.5f );
	static const float playHeadWidthHalf( 0.75f );
	static const Colour playHeadColour( 0xff999999 );
	static const Colour timeScaleUnitNotchColour( greyFgActive2 );
	static const Colour timeScaleGridNotchColour( greyFgActive );

    // scrollbars and handles
    static const Colour scrollBarBgColour( 0xffffffff );
    static const int defaultScrollbarSize( 4 );
	static const int handleW( 8 );
	static const int handleH( 8 );
    
    // text
	static const Colour textColour( 0xffeeeeee );
	static const Colour textHighlightColour( greyBgFar );
    static const float textBrightnessMultiplier( 1.5f );
    struct Fonts
    {
        static int getBigHeight()
        {
            return 35;
        }
        
        static Font getBig()
        {
            return Font( "Arial", getBigHeight(), Font::plain );
        }
        
        static int getHeight()
        {
            return dims::h- 4;
        }
        
        static Font get()
        {
            return Font( "Arial", getHeight(), Font::plain );
        }
        
        static int widthFor( const String& text )
        {
            return get().getStringWidth( text ) + 2 * dims::w;
        }
    };

	// Custom look and feel
	class Look : public LookAndFeel_V3
	{
	public:
		Look();
		~Look();
		
        // Label
        void drawLabel( Graphics& g, Label& l )override;
        Font getLabelFont( Label& l ) override;
        
		// Button
		void drawButtonBackground( Graphics&, Button&, const Colour& backgroundColour, bool isMouseOverButton, bool isButtonDown )override;
		Font getTextButtonFont (TextButton&, int buttonHeight )override;
		int getTextButtonWidthToFitText (TextButton&, int buttonHeight )override;
		void drawButtonText( Graphics&, TextButton&, bool isMouseOverButton, bool isButtonDown )override;
		void drawToggleButton( Graphics&, ToggleButton&, bool isMouseOverButton, bool isButtonDown )override;
		void changeToggleButtonWidthToFitText( ToggleButton& )override;
		void drawTickBox( Graphics&, Component&, float x, float y, float w, float h, bool ticked, bool isEnabled, bool isMouseOverButton, bool isButtonDown )override;
		void drawDrawableButton( Graphics&, DrawableButton&, bool isMouseOverButton, bool isButtonDown )override;
        
        // ComboBox
        void drawComboBox( Graphics&, int width, int height, bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, ComboBox& box )override;
        Font getComboBoxFont( ComboBox& box )override;
        Label* createComboBoxTextBox( ComboBox& b )override;
        void positionComboBoxText( ComboBox&, Label& labelToPosition )override;
	};
}
