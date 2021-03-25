// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#include "TimelineInspector.h"

using namespace unc;
using namespace lnf;

// TimelineInspector
unc::TimelineInspector::TimelineInspector( Timeline* timeline_ ) :
	timeline( timeline_ ),
	gridSize( timeline->getGridSize())
{
	// gridSwitch
	addAndMakeVisible( gridSwitch );
	gridSwitch.setToggleState( timeline->getGridSize() != 0., dontSendNotification );
	gridSwitch.onClick = [ & ](){
		auto useGrid = !gridSwitch.getToggleState();
		timeline->setGridSize( useGrid ? gridSize : 0. );
	};
	// gridInput
	addAndMakeVisible( gridInput );
	gridInput.setEditable( true );
	gridInput.onTextChange = [ & ](){
		auto v = gridInput.getText().getDoubleValue();
		gridSize = jlimit( 0., 10000., v );
		timeline->setGridSize( v );
	};
	addAndMakeVisible( gridLabel );

	timeline->addChangeListener( this );
}

unc::TimelineInspector::~TimelineInspector()
{
	timeline->removeChangeListener( this );
}

void unc::TimelineInspector::paint( Graphics & g )
{
	// bg
	g.fillAll( greyBgMid);
}

void unc::TimelineInspector::resized()
{
	auto b = getLocalBounds();
	gridSwitch.setBounds( b.removeFromLeft( dims::wM ) );
	b.removeFromLeft( dims::pad );
	gridInput.setBounds( b.removeFromLeft( dims::wM ) );
	b.removeFromLeft( dims::pad );
	gridLabel.setBounds( b.removeFromLeft( dims::wS ) );
}

void unc::TimelineInspector::changeListenerCallback( ChangeBroadcaster* source )
{
	if( source == timeline ){
		auto g = timeline->getGridSize();
		if( g != 0. ){
			gridSize == g;
		}
		gridSwitch.setToggleState( g != 0., dontSendNotification );
		gridInput.setText( String( gridSize ), dontSendNotification );
	}
}
