// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#pragma once

#include "MainHeaders.h"

namespace aud
{
	/// \returns evenly spaced values for logarithmically decreasing input (1., 0.5., 0.25).
	inline float gainToDb( float gain )
	{
		return log10( gain ) * 0.5f + 1;
	}

	/// \returns gain values for logarithmically scaled param decibels.
	inline float dbToGain( float decibels )
	{
		return pow( 10.f, 2.f * decibels - 2.f );
	}

	inline void printBuffer( const AudioBuffer<float>& buffer )
	{
		String line;
		for( int samp = 0; samp < buffer.getNumSamples(); ++samp ){
			for( int chan = 0; chan < buffer.getNumChannels(); ++chan ){
				line << " " << String( buffer.getSample( chan, samp ));
			}
			DBG( line );
			line.clear();
		}
	}

	/// \returns true if the two buffers are identical.
	inline bool compareBuffer( const AudioBuffer<float>& first, const AudioBuffer<float> second )
	{
		if( first.getNumSamples() != second.getNumSamples() ){
			return false;
		}
		const auto numSamps = first.getNumSamples();
		if( first.getNumChannels() != second.getNumChannels() ){
			return false;
		}
		const auto numChans = first.getNumChannels();
		for( int chan = 0; chan < numChans; ++chan ){
			for( int samp = 0; samp < numSamps; ++samp ){
				if( first.getSample( chan, samp ) != second.getSample( chan, samp ) ){
					return false;
				}
			}
		}
		return true;
	}

	/// Adds source to destination, writing at destPos.
	inline void addBuffer( const AudioBuffer<float>& source, AudioBuffer<float>& dest, int destPos )
	{
		const auto numSamps = source.getNumSamples();
		const auto numChans = source.getNumChannels();
		const auto requestedSamps = destPos + numSamps;
		if( destPos < 0
			|| requestedSamps > dest.getNumSamples()
			|| numChans != dest.getNumChannels() ){
			jassertfalse;
			return;
		}
		for( int chan = 0; chan < numChans; ++chan ) {
			dest.addFrom( chan, destPos, source, chan, 0, numSamps );
		}
	}

	/// Adds source to destination.
	inline void addBuffer( const AudioBuffer<float>& source, AudioBuffer<float>& dest )
	{
		addBuffer( source, dest, 0 );
	}

	/// Overwrites destination with source, writing at destPos.
	inline void copyBuffer( const AudioBuffer<float>& source, AudioBuffer<float>& dest, int destPos )
	{
		const auto numSamps = source.getNumSamples();
		const auto numChans = source.getNumChannels();
		const auto requestedSamps = destPos + numSamps;
		if( destPos < 0
			|| requestedSamps > dest.getNumSamples()
			|| numChans != dest.getNumChannels() ){
			jassertfalse;
			return;
		}
		for( int chan = 0; chan < numChans; ++chan ) {
			dest.copyFrom( chan, destPos, source, chan, 0, numSamps );
		}
	}

	/// Overwrites destination with source.
	inline void copyBuffer( const AudioBuffer<float>& source, AudioBuffer<float>& dest )
	{
		copyBuffer( source, dest, 0 );
	}

	/// Writes wav 24 bit.
	/// \returns true if successful.
	bool writeToFile( const File& targetFile, const AudioBuffer<float>& audio, const AudioSettings& settings );

	/// Convert audio file to an AudioBuffer and cache as shared data.
	AudioBuffer<float> createOrGetBufferFor( const File& audioFile, AudioSettings& settings );
}
