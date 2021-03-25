// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#include "Timeline.h"

using namespace unc;
using namespace lnf;

// Timeline
unc::Timeline::Timeline( double viewStart_, double viewLength_ ) :
	viewStart( viewStart_ ),
	viewLength( viewLength_ )
{
	static_assert( spacing > 1 ); // spacing is used as divisor
	setInterceptsMouseClicks( false, false );

	// listen
	addAudioSettingsListener( this );
	updateSampleRate();
}

unc::Timeline::~Timeline()
{
	// listen
	removeAudioSettingsListener( this );
}

// Timeline - Component
void unc::Timeline::paint( Graphics & g )
{
	// bg
	g.fillAll( greyBgNear );

	// grid ruler
	auto gridSteps = numGridSteps();
	auto gridStepping = gridSize;
	auto gridStart = nearestFloorOf( getViewStart(), gridStepping );
	for( int i = 0; i < gridSteps; ++i ){
		auto sec = gridStart + i * gridStepping;
		auto x = timepointToPix( sec );

		// notch
		g.setColour( timeScaleGridNotchColour );
		g.fillRect( x, 0, 1., dims::h );
	}
	// unit ruler
	auto unitSteps = numUnitSteps();
	auto unitStepping = calculateStepping( getViewLength(), unitSteps );
	auto unitStart = nearestFloorOf( getViewStart(), unitStepping );
	for( int i = 0; i < unitSteps; ++i ) {
		auto sec = unitStart + i * unitStepping;
		auto x = timepointToPix( sec );

		// text
		String text;
		text.preallocateBytes( 48 );
		int h = ( ( int )std::abs( sec / 3600 ) ) % 24;
		if( h > 0 ){
			text << h << ":";
		}
		int m = ( ( int )std::abs( sec / 60 ) ) % 60;
		text << m << ":";
		int s = ( ( int )std::abs( sec ) ) % 60;
		text << s << ".";
		int ms = ( ( int )std::abs( sec * 1000 ) ) % 1000;
		text << ms;
		g.setColour( textColour );
		g.drawText( text, x + 2, 0, spacing, dims::h, Justification::left );

		// notch
		g.setColour( timeScaleUnitNotchColour );
		g.fillRect( x, 0, 1., dims::h );
	}
}

// Timeline - AudioSettingsListener
void unc::Timeline::audioSettingsChanged( const AudioSettings& newAudioSettings )
{
	updateSampleRate();
}

// Timeline - modify
void unc::Timeline::setHorizontalView( double startInSeconds, double lengthInSeconds )
{
	viewStart = jlimit( 0., maximumViewLength, startInSeconds );
	viewLength = jlimit( minimumViewLength, maximumViewLength, lengthInSeconds );
	repaint();
	sendChangeMessage();
}

void unc::Timeline::setGridSize( double lengthInSeconds )
{
	gridSize = lengthInSeconds;
	repaint();
	sendChangeMessage();
}

void unc::Timeline::updateSampleRate()
{
	sampleRate = getCurrentAudioSettings().sampleRate;
	repaint();
	sendChangeMessage();
}

// Timeline - access
double unc::Timeline::toSeconds( int samples ) const
{
	return samples / jmax( 1., sampleRate );
}

int unc::Timeline::toSamples( double seconds ) const
{
	return roundToIntAccurate( seconds * sampleRate );
}

double unc::Timeline::pixToTimepoint( int compX ) const
{
	auto normalized = ( double )compX / getWidth();
	return viewStart + ( normalized * viewLength );
}

double unc::Timeline::pixToDuration( int distX ) const
{
	auto normalized = ( double )distX / getWidth();
	return normalized * viewLength;
}

int unc::Timeline::timepointToPix( double seconds ) const
{
	auto normalized = ( seconds - viewStart ) / viewLength;
	return normalized * getWidth();
}

int unc::Timeline::durationToPix( double seconds ) const
{
	auto normalized = seconds / viewLength;
	return normalized * getWidth();
}

int unc::Timeline::getGridSizePix() const
{
	return durationToPix( gridSize );
}

int unc::Timeline::getGridSizeSamps() const
{
	return toSamples( gridSize );
}

double unc::Timeline::calculateStepping( double secs, int steps ) const
{
	// which number of steps fit secs best
	auto f = secs / jmax( 1, steps );

	// ms...s
	if( f < 0.001 ) return 0.001;
	if( f < 0.0025 ) return 0.0025;
	if( f < 0.005 ) return 0.005;
	if( f < 0.01 ) return 0.01;
	if( f < 0.025 ) return 0.025;
	if( f < 0.05 ) return 0.05;
	if( f < 0.1 ) return 0.1;
	if( f < 0.25 ) return 0.25;
	if( f < 0.5 ) return 0.5;

	// s...min
	if( f < 1. ) return 1;
	if( f < 2.5 ) return 2.5;
	if( f < 5.0 ) return 5.0;
	if( f < 10. ) return 10.;
	if( f < 30. ) return 30.;

	// mins
	for( int i = 1; i < 360; ++i ) {
		if( f < ( i * 60 ) ) {
			return i * 60;
		}
	}
	return -1.;
}

int unc::Timeline::numUnitSteps() const
{
	return std::ceilf( ( float )getWidth() / spacing );
}

int unc::Timeline::numGridSteps() const
{
	return std::ceilf( ( float )getWidth() / getGridSizePix() );
}
