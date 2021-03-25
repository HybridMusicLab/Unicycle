// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#include "LookAndFeel.h"

using namespace lnf;

// Look
Look::Look()
{
	// PopupMenu
	setColour( PopupMenu::backgroundColourId, greyBgNear );
	setColour( PopupMenu::textColourId, textColour );
	setColour( PopupMenu::highlightedBackgroundColourId, greyFgHighlight );
	setColour( PopupMenu::highlightedTextColourId, textHighlightColour );

    // AlertWindow
    setColour( AlertWindow::backgroundColourId, greyBgNear );
    setColour( AlertWindow::textColourId, textColour );
    setColour( AlertWindow::outlineColourId, greyFgOutline );

	// ListBox
	setColour( ListBox::backgroundColourId, greyBgNear );
	setColour( ListBox::outlineColourId, greyFgOutline );
	setColour( ListBox::textColourId, textColour );
    
    // Label
    setColour( Label::backgroundColourId, greyFg );
    setColour( Label::textColourId, textColour );
    setColour( Label::outlineColourId, greyFgOutline );
    setColour( Label::backgroundWhenEditingColourId, greyFgActive2 );
    setColour( Label::textWhenEditingColourId, textColour );
    setColour( Label::outlineWhenEditingColourId, greyFgOutline );
    
    // TextButton
    setColour( TextButton::buttonColourId, greyFg );
    setColour( TextButton::buttonOnColourId, greyFgActive );
    setColour( TextButton::textColourOffId, textColour );
    setColour( TextButton::textColourOnId, textColour );

	// TextEditor
	setColour( TextEditor::backgroundColourId, greyFg );
	setColour( TextEditor::focusedOutlineColourId, greyFgOutline );
	setColour( TextEditor::highlightColourId, greyFgHighlight );
	setColour( TextEditor::textColourId, textColour );
    
    // ComboBox
    setColour( ComboBox::backgroundColourId, greyFg );
    setColour( ComboBox::textColourId, textColour );
    setColour( ComboBox::outlineColourId, greyFgOutline );
    setColour( ComboBox::buttonColourId, greyFgActive );
    setColour( ComboBox::focusedOutlineColourId, greyFgOutline );
    
    // Scrollbar
    setColour( ScrollBar::backgroundColourId, scrollBarBgColour );
}

Look::~Look()
{}

// Look - Label
void Look::drawLabel( Graphics& g, Label& l )
{
    // non-editable
    if( !l.isEditable()){
        
        // user set bg colour, use that
        if( l.isColourSpecified( Label::backgroundColourId )){
            g.fillAll( l.findColour( Label::backgroundColourId ));
        }
        // plain bg label
        else{
            g.fillAll( greyBgNear );
        }
    }
    // editable
    else{
        if( !l.isBeingEdited()){
            g.fillAll( l.findColour( Label::backgroundColourId ));
            g.setColour( l.findColour( Label::outlineColourId ));
            g.drawRect( l.getLocalBounds());
        }
        else{
            g.fillAll( l.findColour( Label::backgroundWhenEditingColourId ));
            g.setColour( l.findColour( Label::outlineWhenEditingColourId ));
            g.drawRect( l.getLocalBounds());
        }
    }
    // text
    auto just = l.getJustificationType();
    auto trim = 0;
    if( just.testFlags( Justification::left )){
        trim = dims::padM;
    }
    g.setColour( l.findColour( Label::textColourId ));
    g.drawFittedText( l.getText(), l.getLocalBounds().withTrimmedLeft( trim ), just, 1 );
}

Font Look::getLabelFont( Label& l )
{
    return Fonts::get();
}

// Look - Button
void Look::drawButtonBackground( Graphics& g, Button& b, const Colour& backgroundColour, bool isMouseOverButton, bool isButtonDown )
{
    // bg
    Colour bg( backgroundColour );
    
    // override buttonOnColourId for mouseDown and hover
    if( b.isEnabled()){
        if( isButtonDown ) {
            bg = greyFgHighlight;
        }
        else if( isMouseOverButton ) {
            bg = greyFgActive2;
        }
    }
    // override buttonColourId for disabled buttons
    else {
        bg = greyBgNear;
    }
    // buttonColour
    g.fillAll( bg );
    
    // text
    g.setColour( textColour );
    g.drawText( b.getButtonText(), b.getLocalBounds(), Justification::centred );
    
    // outline
    if( b.isEnabled()){
        g.setColour( greyFgOutline );
        g.drawRect( b.getLocalBounds());
    }
}

Font Look::getTextButtonFont( TextButton& b, int buttonHeight )
{
    return Fonts::get();
}

int Look::getTextButtonWidthToFitText( TextButton& b, int buttonHeight )
{
	return getTextButtonFont( b, buttonHeight ).getStringWidth( b.getButtonText() ) + dims::padL * 2;
}

void Look::drawButtonText( Graphics& g, TextButton& b, bool isMouseOverButton, bool isButtonDown )
{
    g.setColour( b.findColour( isButtonDown ? TextButton::textColourOnId : TextButton::textColourOffId ));
	g.drawText( b.getButtonText(), b.getLocalBounds(), Justification::centred );
}

void Look::drawToggleButton( Graphics& g, ToggleButton& t, bool isMouseOverButton, bool isButtonDown )
{
	// toggle on
    if( t.getToggleState() ) {
        Colour bg( greyFg );
        
        // hover
        if( isMouseOverButton ) {
            bg = greyFgActive2;
        }
        else {
            bg = greyFgActive;
        }
        // bg
        g.fillAll( bg );
        g.setColour( greyFgOutline );
        g.drawRect( t.getLocalBounds());
        
        // text
        g.setColour( textColour );
        g.drawText( t.getButtonText(), t.getLocalBounds(), Justification::centred );
        return;
    }
    // toggle off
    drawButtonBackground( g, t, greyFg, isMouseOverButton, isButtonDown );
}

void Look::changeToggleButtonWidthToFitText( ToggleButton& t )
{
}

void Look::drawTickBox( Graphics& g, Component& c, float x, float y, float w, float h, bool ticked, bool isEnabled, bool isMouseOverButton, bool isButtonDown )
{
}

void Look::drawDrawableButton( Graphics& g, DrawableButton &b, bool isMouseOverButton, bool isButtonDown )
{
}

// Look - ComboBox
void Look::drawComboBox( Graphics& g, int width, int height, const bool isButtonDown, int buttonX, int buttonY, int buttonW, int buttonH, ComboBox& box )
{
    // bg
    g.fillAll( box.findColour( ComboBox::backgroundColourId ));
    
    // focus
    g.setColour( box.findColour (ComboBox::focusedOutlineColourId));
    if( box.isEnabled() && box.hasKeyboardFocus( false )){
        g.drawRect( 0, 0, width, height, 2 );
    }
    else {
        g.drawRect( 0, 0, width, height, 1 );
    }
    const float arrowX = 0.3f;
    const float arrowH = 0.2f;
    
    // arrow
    Path p;
    p.addTriangle (buttonX + buttonW * 0.5f,            buttonY + buttonH * (0.45f - arrowH),
                   buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.45f,
                   buttonX + buttonW * arrowX,          buttonY + buttonH * 0.45f);
    
    p.addTriangle (buttonX + buttonW * 0.5f,            buttonY + buttonH * (0.55f + arrowH),
                   buttonX + buttonW * (1.0f - arrowX), buttonY + buttonH * 0.55f,
                   buttonX + buttonW * arrowX,          buttonY + buttonH * 0.55f);
    g.setColour (box.findColour (ComboBox::arrowColourId).withMultipliedAlpha (box.isEnabled() ? 1.0f : 0.3f));
    g.fillPath (p);
}

Font Look::getComboBoxFont (ComboBox& box )
{
    return Fonts::get();
}

Label* Look::createComboBoxTextBox( ComboBox& box )
{
    return new Label( String(), String());
}

void Look::positionComboBoxText( ComboBox& box, Label& label )
{
    label.setBounds( 1, 1, box.getWidth() + 3 - box.getHeight(), box.getHeight() - 2 );
    label.setFont( getComboBoxFont( box ));
}
