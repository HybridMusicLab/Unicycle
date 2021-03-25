// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#include "AudioClipEditor.h"

using namespace unc;
using namespace lnf;

// AudioClipDisplay
unc::AudioClipDisplay::AudioClipDisplay( Timeline* timeline_ ) :
	timeline( timeline_ ),
	thumbnail( 2, *getAudioFormatManager(), *getAudioThumbnailCache())
{
	setInterceptsMouseClicks( false, false );
	timeline->addChangeListener( this );
	thumbnail.addChangeListener( this );
}

unc::AudioClipDisplay::~AudioClipDisplay()
{
	timeline->removeChangeListener( this );
	thumbnail.removeChangeListener( this );
}

// AudioClipDisplay - view
void AudioClipDisplay::display( AudioClip* other )
{
	clip = other;
	if( other && other->file.existsAsFile() ){
		thumbnail.setSource( new FileInputSource( other->file ));
	}
	else{
		thumbnail.setSource( nullptr );
	}
	repaint();
}

// AudioClipDisplay - Component
void unc::AudioClipDisplay::paint( Graphics& g )
{
	// bg
	g.fillAll( Colours::transparentBlack );

	// waveform
	auto start = timeline->getViewStart();
	auto end = start + timeline->getViewLength();
	g.setColour( greyBgActive );
	thumbnail.drawChannels( g, getLocalBounds(), start, end, 1.f );
}

void unc::AudioClipDisplay::resized()
{}

void AudioClipDisplay::changeListenerCallback( ChangeBroadcaster* source )
{
	if( source == timeline ){
		repaint();
	}
	if( source == &thumbnail ){
		repaint();
	}
}

// AudioClipEditor
AudioClipEditor::AudioClipEditor() :
	timelineInspector( &timeline ),
	audioTimeline( &timeline ),
	audioClipDisplay( &timeline )
{
	addAndMakeVisible( timeline );
	addAndMakeVisible( timelineInspector );
	addAndMakeVisible( audioTimeline );
	addAndMakeVisible( audioClipDisplay );
}

AudioClipEditor::~AudioClipEditor()
{}

// AudioClipEditor - view
void AudioClipEditor::display( AudioClip* other )
{
	clip = other;
	audioTimeline.display( other );
	audioClipDisplay.display( other );

	// need to initialize view here, else handleHorizontalScroll() and handleHorizontalZoom() will fail
	if( other ){
		timeline.setHorizontalView( 0., timeline.toSeconds( other->getTotalNumSamples() ) );
	}
	else{
		timeline.setHorizontalView( 0., defaultViewLength );
	}
}

// AudioClipEditor - Component
void AudioClipEditor::paint( Graphics& g )
{
	// bg
	g.fillAll( greyBgMid );
}

void AudioClipEditor::resized()
{
	auto b = getLocalBounds();

	// timeline header
	timeline.setBounds( b.removeFromTop( dims::h ));
	b.removeFromTop( dims::padM );
	
	// timelineInspector footer
	timelineInspector.setBounds( b.removeFromBottom( dims::h ) );
	b.removeFromBottom( dims::padM );
	
	// audioTimeline over audioClipDisplay
	audioClipDisplay.setBounds( b );
	audioTimeline.setBounds( b );
}

void unc::AudioClipEditor::handleHorizontalScroll( float deltaX )
{
	auto startMax = maximumViewLength;
	if( clip ){
		startMax = timeline.toSeconds( clip->getTotalNumSamples() ) - timeline.getViewLength();
	}
	auto deltaSec = timeline.getViewLength() * deltaX * 0.5f;
	auto newStart = jlimit( 0., startMax, timeline.getViewStart() + deltaSec );
	timeline.setHorizontalView( newStart, timeline.getViewLength());
}

void unc::AudioClipEditor::handleHorizontalZoom( float deltaX )
{
	auto lenMax = maximumViewLength;
	if( clip ){
		lenMax = timeline.toSeconds( clip->getTotalNumSamples() ) - timeline.getViewStart();
	}
	auto deltaSecs = timeline.getViewLength() * deltaX * 0.5f;
	auto newLen = jlimit( minimumViewLength, lenMax, timeline.getViewLength() + deltaSecs );
	auto newStart = jmax( 0., timeline.getViewStart() - deltaSecs * 0.5f );
	timeline.setHorizontalView( newStart, newLen );
}
