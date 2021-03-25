// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#include "AudioSettingsDisplay.h"

using namespace aud;
using namespace lnf;

aud::AudioSettingsDisplay::AudioSettingsDisplay()
{
	addAndMakeVisible( srLabel );
	addAndMakeVisible( srDisplay );
	addAndMakeVisible( bufferLabel );
	addAndMakeVisible( bufferDisplay );
	addAndMakeVisible( bitsLabel );
	addAndMakeVisible( bitsDisplay );
	addAudioSettingsListener( this );
}

aud::AudioSettingsDisplay::~AudioSettingsDisplay()
{
	removeAudioSettingsListener( this );
}

void aud::AudioSettingsDisplay::paint( Graphics & g )
{
	// bg
	g.fillAll( greyBgMid );
}

void aud::AudioSettingsDisplay::resized()
{
	auto b = getLocalBounds();

	// sr
	srLabel.setBounds( b.removeFromLeft( dims::wS ));
	b.removeFromLeft( dims::pad );
	srDisplay.setBounds( b.removeFromLeft( dims::wM ));
	b.removeFromLeft( dims::padM );

	// buffer
	bufferLabel.setBounds( b.removeFromLeft( dims::wS ) );
	b.removeFromLeft( dims::pad );
	bufferDisplay.setBounds( b.removeFromLeft( dims::wM ) );
	b.removeFromLeft( dims::padM );

	// bits
	bitsLabel.setBounds( b.removeFromLeft( dims::wS ) );
	b.removeFromLeft( dims::pad );
	bitsDisplay.setBounds( b.removeFromLeft( dims::wM ) );
	b.removeFromLeft( dims::padM );
}

void aud::AudioSettingsDisplay::audioSettingsChanged( const AudioSettings& newAudioSettings )
{
	srDisplay.setText( String( newAudioSettings.sampleRate ), dontSendNotification );
	bufferDisplay.setText( String( newAudioSettings.bufferSize ), dontSendNotification );
	bitsDisplay.setText( String( newAudioSettings.bitsPerSample ), dontSendNotification );
}
