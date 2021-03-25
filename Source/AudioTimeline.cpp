// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#include "AudioTimeline.h"

using namespace unc;
using namespace lnf;

// ZoneBox
unc::ZoneBox::ZoneBox( const AudioPlayZone& zone_, AudioTimeline* parent_ ) :
	zone( zone_ ),
	parent( parent_ ),
	clip( parent->clip )
{}

// ZoneBox - Component
void unc::ZoneBox::paint( Graphics & g )
{
	// bg
	g.fillAll( greyBgMid );

	auto st = startHandle();
	auto fi = fadeInHandle();
	auto fo = fadeOutHandle();
	auto end = endHandle();

	// fades
	Path p;
	p.startNewSubPath( st.getBottomLeft().toFloat() );
	p.lineTo( st.getTopLeft().toFloat() );
	p.lineTo( fi.getTopLeft().toFloat() );
	p.lineTo( fo.getTopRight().toFloat() );
	p.lineTo( end.getTopRight().toFloat() );
	p.lineTo( end.getBottomRight().toFloat() );
	p.closeSubPath();
	g.setColour( parent->isSelected( this ) ? redBgActive : redBg );
	g.fillPath( p );

	// handles
	g.setColour( parent->isSelected( this ) ? redFgActive : redFg );
	g.fillRect( st );
	g.fillRect( end );
	g.fillRect( fo );
	g.fillRect( fi );

	// text
	g.setColour( textColour );
	g.drawFittedText( zone.name, getLocalBounds(), Justification::centred, 1 );
}

void unc::ZoneBox::mouseDown( const MouseEvent& e )
{
	if( !clip ){
		return;
	}
	parent->setSelected( this );

	// left
	if( !e.mods.isPopupMenu() ){
		
		if( startHandle().contains( e.getPosition())){
			mouseMode = DragStart;
			old = zone;
		}
		else if( endHandle().contains( e.getPosition() ) ){
			mouseMode = DragLen;
			old = zone;
		}
		else if( fadeInHandle().contains( e.getPosition() ) ){
			mouseMode = DragFadeIn;
			old = zone;
		}
		else if( fadeOutHandle().contains( e.getPosition() ) ){
			mouseMode = DragFadeOut;
			old = zone;
		}
		else{
			if( auto* m = findParentComponentOfClass<MainInterface>() ){
				mouseMode = Play;
				m->playAudioBuffer( clip->writeAudio( clip->indexOfZone( zone ) ), zone.mode == AudioPlayMode::Loop );
			}
		}
	}
	// right
	else{
		PopupMenu m;
		m.addItem( "Play", true, zone.mode == AudioPlayMode::Play, [&](){
			old = zone;
			zone.mode = AudioPlayMode::Play;
			zone.name = toString( zone.mode );
			getUndoManager()->beginNewTransaction( "SetPlayZone mode Play" );
			getUndoManager()->perform( new SetPlayZoneCommand( parent->clip, old, zone ) );
		} );
		m.addItem( "Loop", true, zone.mode == AudioPlayMode::Loop, [ & ](){
			old = zone;
			zone.mode = AudioPlayMode::Loop;
			zone.name = toString( zone.mode );
			zone.fadeIn = 0; // fades are synced in loopmode, just reset
			zone.fadeOut = 0;
			getUndoManager()->beginNewTransaction( "SetPlayZone mode Loop" );
			getUndoManager()->perform( new SetPlayZoneCommand( parent->clip, old, zone ) );
		} );
		m.addSeparator();
		m.addItem( "Rename", true, false, [ & ](){
			old = zone;
			AlertWindow win( "Rename", "Enter new name", AlertWindow::InfoIcon, nullptr );
			win.addTextEditor( "setName", zone.name );
			win.addButton( "OK", 1 );
			win.runModalLoop();
			zone.name = win.getTextEditorContents( "setName" );
			getUndoManager()->beginNewTransaction( "SetPlayZone name" );
			getUndoManager()->perform( new SetPlayZoneCommand( parent->clip, old, zone ) );
		} );
		m.addCommandItem( getApplicationCommandManager(), writeZoneToSelected );
		m.addCommandItem( getApplicationCommandManager(), StandardApplicationCommandIDs::del );
		m.addCommandItem( getApplicationCommandManager(), removeZoneFromSelected );
		auto r = m.show();
	}
	repaint();
}

void unc::ZoneBox::mouseDrag( const MouseEvent& e )
{
	if( !clip ){
		return;
	}
	// left
	if( !e.mods.isPopupMenu() ){
		switch( mouseMode ){
			case DragStart: {
				auto v = ( int )nearestOf( old.start + parent->pixToSamples( e.getDistanceFromDragStartX() ), parent->getGridSize() );
				auto min = 0;
				auto max = clip->getTotalNumSamples() - zone.length;
				zone.start = jlimit( min, max, v );
				break;
			}
			case DragLen:{
				auto v = ( int )nearestOf( old.length + parent->pixToSamples( e.getDistanceFromDragStartX() ), parent->getGridSize() );
				auto min = zone.fadeIn + zone.fadeOut;
				auto max = clip->getTotalNumSamples() - zone.start;
				zone.length = jlimit( min, max, v );
				break;
			}
			case DragFadeIn:{
				auto v = ( int )nearestOf( old.fadeIn + parent->pixToSamples( e.getDistanceFromDragStartX() ), parent->getGridSize() );
				auto min = 0;
				auto max = isLooping() ? zone.length / 2 : zone.length - zone.fadeOut;
				zone.fadeIn = jlimit( min, max, v );
				if( isLooping() ){
					zone.fadeOut = zone.fadeIn;
				}
				break;
			}
			case DragFadeOut:{
				auto v = ( int )nearestOf( old.fadeOut + parent->pixToSamples( -e.getDistanceFromDragStartX() ), parent->getGridSize() );
				auto min = 0;
				auto max = isLooping() ? zone.length / 2 : zone.length - zone.fadeIn;
				zone.fadeOut = jlimit( min, max, v );
				if( isLooping() ){
					zone.fadeIn = zone.fadeOut;
				}
				break;
			}
			default: break;
		}
		setBounds( parent->getBoundsFor( this ));
	}
	repaint();
}

void unc::ZoneBox::mouseUp( const MouseEvent& e )
{
	if( !clip ){
		return;
	}
	// left
	if( !e.mods.isPopupMenu() ){
		switch( mouseMode ){
			case DragStart:
			case DragLen:
			case DragFadeIn:
			case DragFadeOut:{
				getUndoManager()->beginNewTransaction( "SetPlayZone position" );
				getUndoManager()->perform( new SetPlayZoneCommand( clip, old, zone ) );
				break;
			}
			case Play:{
				if( auto* m = findParentComponentOfClass<MainInterface>() ){
					m->stopPlaying();
				}
				break;
			}
			default: break;
		}
	}
	mouseMode = Idle;
	repaint();
}

Rectangle<int> unc::ZoneBox::startHandle() const
{
	auto x = 0;
	auto y = getHeight() - handleH;
	auto w = handleW;
	auto h = handleH;
	return { x, y, w, h };
}

Rectangle<int> unc::ZoneBox::endHandle() const
{
	auto x = getWidth() - handleW;
	auto y = getHeight() - handleH;
	auto w = handleW;
	auto h = handleH;
	return{ x, y, w, h };
}

Rectangle<int> unc::ZoneBox::fadeInHandle() const
{
	auto x = parent->samplesToPix( zone.fadeIn );
	auto y = 0;
	auto w = handleW;
	auto h = handleH;
	return{ x, y, w, h };
}

Rectangle<int> unc::ZoneBox::fadeOutHandle() const
{
	auto x = getWidth() - parent->samplesToPix( zone.fadeOut ) - handleW;
	auto y = 0;
	auto w = handleW;
	auto h = handleH;
	return{ x, y, w, h };
}

// AudioTimeline
unc::AudioTimeline::AudioTimeline( Timeline* timeline_ ) :
timeline( timeline_ )
{
	timeline->addChangeListener( this );
}

unc::AudioTimeline::~AudioTimeline()
{
	timeline->removeChangeListener( this );
	if( clip ){
		clip->removeChangeListener( this );
	}
}

// AudioTimeline - view
void unc::AudioTimeline::display( AudioClip* other )
{
	if( clip ){
		clip->removeChangeListener( this );
	}
	clip = other;
	if( clip ){
		clip->addChangeListener( this );
	}
	rebuild();
}

void unc::AudioTimeline::rebuild()
{
	boxes.clear();
	if( clip ){
		for( int i = 0; i < clip->sizeZones(); ++i ){
			auto* box = new ZoneBox( clip->getZone( i ), this );
			boxes.add( box );
			addAndMakeVisible( box );
		}
	}
	update();
}

void unc::AudioTimeline::update()
{
	for( auto* box : boxes ){
		box->setBounds( getBoundsFor( box ) );
	}
}

// AudioTimeline - Component
void unc::AudioTimeline::paint( Graphics& g )
{
	// bg
	g.fillAll( greyBgMid );
}

void unc::AudioTimeline::paintOverChildren( Graphics & g )
{
	// dragRange
	if( dragRect.getWidth() != 0 ){
		g.setColour( redFgActive );
		g.fillRect( dragRect );
	}
}

void unc::AudioTimeline::resized()
{
	update();
}

void unc::AudioTimeline::mouseDown( const MouseEvent& e )
{
	if( !clip ){
		return;
	}
	// left
	if( !e.mods.isPopupMenu() ){

		// deselect
		setSelected( nullptr );

		// drag new
		mouseMode = DragNew;
		auto x1 = pixPosFloor( e.getMouseDownX());
		auto x2 = pixPosCeil( e.getMouseDownX());
		auto y = 0;
		auto w = x2 - x1;
		auto h = getHeight();
		dragRect = Rectangle<int>( x1, y, w, h );
	}
	repaint();
}

void unc::AudioTimeline::mouseDrag( const MouseEvent & e )
{
	if( !clip ){
		return;
	}
	// left
	if( !e.mods.isPopupMenu() ){

		// drag new
		if( mouseMode == DragNew ){
			auto x1 = e.getMouseDownX();
			auto x2 = x1 + e.getDistanceFromDragStartX();
			auto r = Range<int>::between( x1, x2 );
			dragRect.setLeft( pixPosFloor( r.getStart()));
			dragRect.setRight( pixPosCeil( r.getEnd()));
		}
	}
	repaint();
}

void unc::AudioTimeline::mouseUp( const MouseEvent& e )
{
	if( !clip ){
		return;
	}
	// left
	if( !e.mods.isPopupMenu() ){

		// drag new
		if( mouseMode == DragNew ){
			AudioPlayZone zone;
			zone.start = timeline->toSamples( timeline->pixToTimepoint( dragRect.getX() ) );
			zone.length = timeline->toSamples( timeline->pixToDuration( dragRect.getWidth() ) );
			zone.mode = AudioPlayMode::Play;
			zone.name = toString( zone.mode );
			auto zoneEnd = zone.start + zone.length;
			if( zone.start >= 0
				&& zone.length > 0
				&& zone.start < clip->getTotalNumSamples()
				&& zoneEnd <= clip->getTotalNumSamples() ){
				getUndoManager()->beginNewTransaction( "AddPlayZone" );
				getUndoManager()->perform( new AddPlayZoneCommand( clip, zone ) );
			}
		}
	}
	mouseMode = Idle;
	dragRect.setSize( 0, 0 );
	repaint();
}

// AudioTimeline - modify
void unc::AudioTimeline::setSelected( ZoneBox* box )
{
	if( auto* m = findParentComponentOfClass<MainInterface>() ) {
		m->selectAudioPlayZone( box ? box->zone : AudioPlayZone() );
	}
	for( auto* b : boxes ){
		b->repaint();
	}
}

// AudioTimeline - access
Rectangle<int> unc::AudioTimeline::getBoundsFor( ZoneBox* box )const
{
	auto x = box ? timeline->timepointToPix( timeline->toSeconds( box->zone.start )) : 0;
	auto y = 0;
	auto w = box ? timeline->durationToPix( timeline->toSeconds( box->zone.length )) : 0;
	auto h = getHeight();
	return { x, y, w, h };
}

int unc::AudioTimeline::samplesToPix( int samples )const
{
	return timeline->durationToPix( timeline->toSeconds( samples ));
}

int unc::AudioTimeline::pixPosFloor( int compX )const
{
	return timeline->timepointToPix( nearestFloorOf( timeline->pixToTimepoint( compX ), timeline->getGridSize()));
}

int unc::AudioTimeline::pixPosCeil( int compX )const
{
	return timeline->timepointToPix( nearestCeilOf( timeline->pixToTimepoint( compX ), timeline->getGridSize() ) );
}

int unc::AudioTimeline::pixToSamples( int distX )const
{
	return timeline->toSamples( timeline->pixToDuration( distX ));
}

bool unc::AudioTimeline::isSelected( ZoneBox* box ) const
{
	if( auto* m = findParentComponentOfClass<MainInterface>() ){
		return m->getSelectedAudioClip() == clip && m->getSelectedPlayZone() == box->zone;
	}
}

// AudioTimeline - ChangeListener
void unc::AudioTimeline::changeListenerCallback( ChangeBroadcaster* source )
{
	if( source == clip ){
		rebuild();
	}
	if( source == timeline ){
		update();
	}
}
