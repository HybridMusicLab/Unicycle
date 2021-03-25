// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#pragma once

#include "AudioPlayback.h"

namespace aud
{
	class AudioPlaybackTest : public UnitTest
	{
	public:
		AudioPlaybackTest() : UnitTest( "AudioPlaybackTest" ){}

		void runTest() override
		{
			testResampler();
			testResamplerOdd();
			testResamplerComplete();
			testResamplerShort();
			testResamplerShortOdd();
			testResamplerOne();
			testResamplerOneOdd();
			testResamplerFadeIn();
			testResamplerFadeOut();
			testResamplerPlaySpeedBounds();
		}

		void testResampler()
		{
			beginTest( "testResampler" );

			// read from this buffer
			AudioBuffer<float> b( 1, 6 );
			b.setSample( 0, 0, 0.1f );
			b.setSample( 0, 1, 0.2f );
			b.setSample( 0, 2, 0.3f );
			b.setSample( 0, 3, 0.4f );
			b.setSample( 0, 4, 0.5f );
			b.setSample( 0, 5, 0.6f );

			// use this resampler
			Resampler r( b );
			r.setRange( 1, 4 );
			r.reset( -2 );
			AudioBuffer<float> o( 1, 2 );

			// reading before range clears
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 0 );
			expectEquals( o.getSample( 0, 0 ), 0.f );
			expectEquals( o.getSample( 0, 1 ), 0.f );

			// start of range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 2 );
			expectEquals( o.getSample( 0, 0 ), 0.2f );
			expectEquals( o.getSample( 0, 1 ), 0.3f );

			// end of range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 4 );
			expectEquals( o.getSample( 0, 0 ), 0.4f );
			expectEquals( o.getSample( 0, 1 ), 0.5f );

			// after range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 6 );
			expectEquals( o.getSample( 0, 0 ), 0.f );
			expectEquals( o.getSample( 0, 1 ), 0.f );
		}

		void testResamplerOdd()
		{
			beginTest( "testResamplerOdd" );

			// read from this buffer
			AudioBuffer<float> b( 1, 6 );
			b.setSample( 0, 0, 0.1f );
			b.setSample( 0, 1, 0.2f );
			b.setSample( 0, 2, 0.3f );
			b.setSample( 0, 3, 0.4f );
			b.setSample( 0, 4, 0.5f );
			b.setSample( 0, 5, 0.6f );

			// use this resampler
			Resampler r( b );
			r.setRange( 1, 4 );
			r.reset( -3 );
			AudioBuffer<float> o( 1, 2 );

			// reading before range clears
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), -1 );
			expectEquals( o.getSample( 0, 0 ), 0.f );
			expectEquals( o.getSample( 0, 1 ), 0.f );

			// start of range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 1 );
			expectEquals( o.getSample( 0, 0 ), 0.f );
			expectEquals( o.getSample( 0, 1 ), 0.2f );

			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 3 );
			expectEquals( o.getSample( 0, 0 ), 0.3f );
			expectEquals( o.getSample( 0, 1 ), 0.4f );

			// end of range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 5 );
			expectEquals( o.getSample( 0, 0 ), 0.5f );
			expectEquals( o.getSample( 0, 1 ), 0.f );

			// after range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 7 );
			expectEquals( o.getSample( 0, 0 ), 0.f );
			expectEquals( o.getSample( 0, 1 ), 0.f );
		}

		void testResamplerComplete()
		{
			beginTest( "testResamplerComplete" );

			// read from this buffer
			AudioBuffer<float> b( 1, 6 );
			b.setSample( 0, 0, 0.1f );
			b.setSample( 0, 1, 0.2f );
			b.setSample( 0, 2, 0.3f );
			b.setSample( 0, 3, 0.4f );
			b.setSample( 0, 4, 0.5f );
			b.setSample( 0, 5, 0.6f );

			// use this resampler
			Resampler r( b );
			r.setRange( 0, 6 );
			r.reset( -2 );
			AudioBuffer<float> o( 1, 2 );

			// reading before range clears
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 0 );
			expectEquals( o.getSample( 0, 0 ), 0.f );
			expectEquals( o.getSample( 0, 1 ), 0.f );

			// start of range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 2 );
			expectEquals( o.getSample( 0, 0 ), 0.1f );
			expectEquals( o.getSample( 0, 1 ), 0.2f );

			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 4 );
			expectEquals( o.getSample( 0, 0 ), 0.3f );
			expectEquals( o.getSample( 0, 1 ), 0.4f );

			// end of range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 6 );
			expectEquals( o.getSample( 0, 0 ), 0.5f );
			expectEquals( o.getSample( 0, 1 ), 0.6f );

			// after range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 8 );
			expectEquals( o.getSample( 0, 0 ), 0.f );
			expectEquals( o.getSample( 0, 1 ), 0.f );
		}

		void testResamplerShort()
		{
			beginTest( "testResamplerShort" );

			// read from this buffer
			AudioBuffer<float> b( 1, 6 );
			b.setSample( 0, 0, 0.1f );
			b.setSample( 0, 1, 0.2f );
			b.setSample( 0, 2, 0.3f );
			b.setSample( 0, 3, 0.4f );
			b.setSample( 0, 4, 0.5f );
			b.setSample( 0, 5, 0.6f );

			// use this resampler
			Resampler r( b );
			r.setRange( 0, 5 );
			r.reset( -2 );
			AudioBuffer<float> o( 1, 2 );

			// reading before range clears
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 0 );
			expectEquals( o.getSample( 0, 0 ), 0.f );
			expectEquals( o.getSample( 0, 1 ), 0.f );

			// start of range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 2 );
			expectEquals( o.getSample( 0, 0 ), 0.1f );
			expectEquals( o.getSample( 0, 1 ), 0.2f );

			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 4 );
			expectEquals( o.getSample( 0, 0 ), 0.3f );
			expectEquals( o.getSample( 0, 1 ), 0.4f );

			// end of range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 6 );
			expectEquals( o.getSample( 0, 0 ), 0.5f );
			expectEquals( o.getSample( 0, 1 ), 0.f );

			// after range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 8 );
			expectEquals( o.getSample( 0, 0 ), 0.f );
			expectEquals( o.getSample( 0, 1 ), 0.f );
		}

		void testResamplerShortOdd()
		{
			beginTest( "testResamplerShortOdd" );

			// read from this buffer
			AudioBuffer<float> b( 1, 6 );
			b.setSample( 0, 0, 0.1f );
			b.setSample( 0, 1, 0.2f );
			b.setSample( 0, 2, 0.3f );
			b.setSample( 0, 3, 0.4f );
			b.setSample( 0, 4, 0.5f );
			b.setSample( 0, 5, 0.6f );

			// use this resampler
			Resampler r( b );
			r.setRange( 1, 3 );
			r.reset( -3 );
			AudioBuffer<float> o( 1, 2 );

			// reading before range clears
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), -1 );
			expectEquals( o.getSample( 0, 0 ), 0.f );
			expectEquals( o.getSample( 0, 1 ), 0.f );

			// start of range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 1 );
			expectEquals( o.getSample( 0, 0 ), 0.f );
			expectEquals( o.getSample( 0, 1 ), 0.2f );

			// end of range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 3 );
			expectEquals( o.getSample( 0, 0 ), 0.3f );
			expectEquals( o.getSample( 0, 1 ), 0.4f );

			// after range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 5 );
			expectEquals( o.getSample( 0, 0 ), 0.f );
			expectEquals( o.getSample( 0, 1 ), 0.f );
		}

		void testResamplerOne()
		{
			beginTest( "testResamplerOne" );

			// read from this buffer
			AudioBuffer<float> b( 1, 6 );
			b.setSample( 0, 0, 0.1f );
			b.setSample( 0, 1, 0.2f );
			b.setSample( 0, 2, 0.3f );
			b.setSample( 0, 3, 0.4f );
			b.setSample( 0, 4, 0.5f );
			b.setSample( 0, 5, 0.6f );

			// use this resampler
			Resampler r( b );
			r.setRange( 1, 2 );
			r.reset( -2 );
			AudioBuffer<float> o( 1, 2 );

			// reading before range clears
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 0 );
			expectEquals( o.getSample( 0, 0 ), 0.f );
			expectEquals( o.getSample( 0, 1 ), 0.f );

			// start/end of range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 2 );
			expectEquals( o.getSample( 0, 0 ), 0.2f );
			expectEquals( o.getSample( 0, 1 ), 0.3f );

			// after range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 4 );
			expectEquals( o.getSample( 0, 0 ), 0.f );
			expectEquals( o.getSample( 0, 1 ), 0.f );
		}

		void testResamplerOneOdd()
		{
			beginTest( "testResamplerOneOdd" );

			// read from this buffer
			AudioBuffer<float> b( 1, 6 );
			b.setSample( 0, 0, 0.1f );
			b.setSample( 0, 1, 0.2f );
			b.setSample( 0, 2, 0.3f );
			b.setSample( 0, 3, 0.4f );
			b.setSample( 0, 4, 0.5f );
			b.setSample( 0, 5, 0.6f );

			// use this resampler
			Resampler r( b );
			r.setRange( 1, 2 );
			r.reset( -3 );
			AudioBuffer<float> o( 1, 2 );

			// reading before range clears
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), -1 );
			expectEquals( o.getSample( 0, 0 ), 0.f );
			expectEquals( o.getSample( 0, 1 ), 0.f );

			// start/end of range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 1 );
			expectEquals( o.getSample( 0, 0 ), 0.f );
			expectEquals( o.getSample( 0, 1 ), 0.2f );

			// after range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 3 );
			expectEquals( o.getSample( 0, 0 ), 0.3f );
			expectEquals( o.getSample( 0, 1 ), 0.f );
		}

		void testResamplerFadeIn()
		{
			beginTest( "testResamplerFadeIn" );

			// read from this buffer
			AudioBuffer<float> b( 1, 8 );
			b.setSample( 0, 0, 1.f );
			b.setSample( 0, 1, 1.f );
			b.setSample( 0, 2, 1.f );
			b.setSample( 0, 3, 1.f );
			b.setSample( 0, 4, 1.f );
			b.setSample( 0, 5, 1.f );
			b.setSample( 0, 6, 1.f );
			b.setSample( 0, 7, 1.f );

			// use this resampler
			Resampler r( b );
			r.setRange( 1, 6 );
			r.setFadeIn( 4 );
			r.reset( -2 );
			AudioBuffer<float> o( 1, 2 );

			// reading before range clears
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 0 );
			expectEquals( o.getSample( 0, 0 ), 0.f );
			expectEquals( o.getSample( 0, 1 ), 0.f );

			// start of fade
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 2 );
			expectEquals( o.getSample( 0, 0 ), 0.f ); // first sample
			expectEquals( o.getSample( 0, 1 ), 0.25f );

			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 4 );
			expectEquals( o.getSample( 0, 0 ), 0.5f );
			expectEquals( o.getSample( 0, 1 ), 0.75f );

			// end of range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 6 );
			expectEquals( o.getSample( 0, 0 ), 1.f );
			expectEquals( o.getSample( 0, 1 ), 1.f );

			// after range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 8 );
			expectEquals( o.getSample( 0, 0 ), 0.f );
			expectEquals( o.getSample( 0, 1 ), 0.f );
		}

		void testResamplerFadeOut()
		{
			beginTest( "testResamplerFadeOut" );

			// read from this buffer
			AudioBuffer<float> b( 1, 8 );
			b.setSample( 0, 0, 1.f );
			b.setSample( 0, 1, 1.f );
			b.setSample( 0, 2, 1.f );
			b.setSample( 0, 3, 1.f );
			b.setSample( 0, 4, 1.f );
			b.setSample( 0, 5, 1.f );
			b.setSample( 0, 6, 1.f );
			b.setSample( 0, 7, 1.f );

			// use this resampler
			Resampler r( b );
			r.setRange( 1, 6 );
			r.setFadeOut( 4 );
			r.reset( -2 );
			AudioBuffer<float> o( 1, 2 );

			// reading before range clears
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 0 );
			expectEquals( o.getSample( 0, 0 ), 0.f );
			expectEquals( o.getSample( 0, 1 ), 0.f );

			// start of fade
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 2 );
			expectEquals( o.getSample( 0, 0 ), 1.f ); // first sample
			expectEquals( o.getSample( 0, 1 ), 1.f );

			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 4 );
			expectEquals( o.getSample( 0, 0 ), 1.f );
			expectEquals( o.getSample( 0, 1 ), 0.75f );

			// end of range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 6 );
			expectEquals( o.getSample( 0, 0 ), 0.5f );
			expectEquals( o.getSample( 0, 1 ), 0.25f );

			// after range
			o.setSample( 0, 0, 1.f );
			o.setSample( 0, 1, 1.f );
			expectEquals( r.process( o ), 8 );
			expectEquals( o.getSample( 0, 0 ), 0.f );
			expectEquals( o.getSample( 0, 1 ), 0.f );
		}

		void testResamplerPlaySpeedBounds()
		{
			beginTest( "testResamplerPlaySpeedBounds" );

			// read from this buffer
			AudioBuffer<float> b( 1, 4 );
			b.setSample( 0, 0, 0.1f );
			b.setSample( 0, 1, 0.2f );
			b.setSample( 0, 2, 0.3f );
			b.setSample( 0, 3, 0.4f );

			// output to o, which is padded with one sample front/back
			float inv = std::numeric_limits<float>::min();
			AudioBuffer<float> mem( 1, 4 );
			for( int i = 0; i < mem.getNumSamples(); ++i ){
				mem.setSample( 0, i, inv );
			}
			AudioBuffer<float> o( mem.getArrayOfWritePointers(), 1, 1, 2 );

			// check at different speeds, add some slack at the end
			for( float speed = 0.1f; speed <= 4.f; speed += 0.02f ){
				auto steps = b.getNumSamples() / speed + 5;

				// use this resampler
				Resampler r( b );
				r.setRange( 0, 4 );
				r.setRatio( speed );
				r.reset( 0 );
				for( int step = 0; step < steps; ++step ){
					for( int i = 0; i < mem.getNumSamples(); ++i ){
						mem.setSample( 0, i, inv );
					}
					r.process( o );
					expectEquals( mem.getSample( 0, 0 ), inv );
					expectEquals( mem.getSample( 0, 3 ), inv );
				}
			}
		}
	};
	static AudioPlaybackTest audioPlaybackTest;
}
