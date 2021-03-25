// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#pragma once

#include "MainHeaders.h"

namespace aud
{
	const static size_t MaxNumAudioChannels( 2 );
	const static double MaxPlaybackRatio( 4 );

	/// A fade in with a dynamically changeable length.
	class FadeIn
	{
	public:
		FadeIn( int fadeLength );

		// process
		/// \param newPos should be relative to fade start and <= 0.
		/// \returns next fade position.
		int process( AudioBuffer<float>& audioBuffer, int fadePos, double playRatio );

		// modify
		void reset(){ alpha = 0.; }
		void setLength( int newLength ){ fadeLength = newLength; }

		// access
		int getLength() const{ return fadeLength; }

	private:
		int fadeLength;
		double alpha = 0.f;
	};

	/// A fade out with a dynamically changeable length.
	class FadeOut
	{
	public:
		FadeOut( int fadeLength );

		// process
		/// \param newPos should be relative to fade start and <= 0.
		/// \returns next fade position.
		int process( AudioBuffer<float>& audioBuffer, int fadePos, double playRatio );

		// modify
		void reset(){ alpha = 1.; }
		void setLength( int newLength ){ fadeLength = newLength; }

		// access
		int getLength() const{ return fadeLength; }

	private:
		int fadeLength;
		double alpha = 1.f;
	};

	/// Play a given range inside an audio sample with variable speed. All access should be from within or before entering audio thread.
	class Resampler
	{
	public:
		Resampler( const AudioBuffer<float>& sample_ );

		// process
		/// \returns next playPos relative to range.
		int process( AudioBuffer<float>& audioBuffer );

		// modify
		/// Positions are relative to audio sample, must be within bounds.
		void setRange( int start, int length );

		/// \param pos is relative to range.
		void reset( int playPos );

		/// How many samples are consumed per one buffer sample.
		void setRatio( double newRatio );

		/// \param fadelength must not exceed range.
		/// @{
		void setFadeIn( int fadeLength );
		void setFadeOut( int fadeLength );
		/// @}

		// access
		/// \returns playPos relative to range.
		int getPlayPos() const{ return playPosition; }

		/// Ranges are relative to audio sample.
		/// @{
		int getRangeStart() const{ return rangeStart; }
		int getRangeLength() const{ return rangeLength; }
		/// @}

		int getFadeInLength() const{ return fadeIn.getLength(); }
		int getFadeOutLength() const{ return fadeOut.getLength(); }

	private:
		/// Play dim is relative to range
		int playPosition = 0;

		/// Range dim is relative to sample
		int rangeStart = 0;
		int rangeLength;
		double sampleRatio = 1.f;
		const AudioBuffer<float>& sample;

		/// Fades are relative to range.
		FadeIn fadeIn{ 0 };
		FadeOut fadeOut{ 0 };
        std::array<CatmullRomInterpolator, MaxNumAudioChannels> resampler{};

		JUCE_DECLARE_NON_COPYABLE( Resampler );
	};
	    
    /// \param normalized between 0. and 1., with 0.5 being neutral.
    /// \returns speed of playback between 0.25 and 4.
    inline double asPlaybackRatio( double normalized )
    {
        return pow( MaxPlaybackRatio, ( normalized - 0.5f )* 2.f );
    }
}
