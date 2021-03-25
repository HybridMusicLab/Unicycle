// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#pragma once

#include "MainHeaders.h"
#include "LookAndFeel.h"

namespace unc
{
	inline double nearestFloorOf( double value, double divisor )
	{
		if( divisor == 0. ){
			return value;
		}
		return std::floor( value / divisor ) * divisor;
	}

	inline double nearestCeilOf( double value, double divisor )
	{
		if( divisor == 0. ){
			return value;
		}
		return std::ceil( value / divisor ) * divisor;
	}

	inline double nearestOf( double value, double divisor )
	{
		if( divisor == 0. ){
			return value;
		}
		return std::round( value / divisor ) * divisor;
	}

	/// The base model for viewable timespans.
	class Timeline :	public Component,
		public AudioSettingsListener,
		public ChangeBroadcaster
	{
	public:
		Timeline( double viewStart = 0., double viewLength = lnf::defaultViewLength );
		~Timeline();

		// Component
		void paint( Graphics& g ) override;

		// AudioSettingsListener
		void audioSettingsChanged( const AudioSettings& newAudioSettings )override;

		// modify
		void setHorizontalView( double startInSeconds, double lengthInSeconds );
		void setGridSize( double lengthInSeconds );
		void updateSampleRate();

		// access
		double toSeconds( int samples )const;
		int toSamples( double seconds )const;
		double pixToTimepoint( int compX )const;
		double pixToDuration( int distX )const;
		int timepointToPix( double seconds )const;
		int durationToPix( double seconds )const;
		int getGridSizePix() const;
		int getGridSizeSamps() const;
		double getGridSize() const{ return gridSize; }
		double getViewStart() const{ return viewStart; }
		double getViewLength() const{ return viewLength; }

	private:
		// access
		double calculateStepping( double secs, int steps ) const;
		int numUnitSteps() const;
		int numGridSteps() const;

		double viewStart; // secs, horizontal
		double viewLength; // secs, horizontal
		double sampleRate = 1;
		double gridSize = 0.1;
		static const int spacing = 100;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( Timeline );
	};
}
