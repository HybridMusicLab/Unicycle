// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#pragma once

#include "AudioFunctions.h"

namespace aud
{
	class AudioFunctionTest : public UnitTest
	{
	public:
		AudioFunctionTest() : UnitTest( "AudioFunctionTest" ){};

		void runTest() override
		{
			testAudioCopy();
		}

		void testAudioCopy()
		{
			beginTest( "testAudioCopy" );

			AudioBuffer<float> s( 2, 2 );
			s.setSample( 0, 0, 0.1f );
			s.setSample( 0, 1, 0.2f );
			s.setSample( 1, 0, 0.3f );
			s.setSample( 1, 1, 0.4f );
			AudioBuffer<float> d( 2, 2 );
			aud::copyBuffer( s, d );
			expectEquals( d.getSample( 0, 0 ), 0.1f );
			expectEquals( d.getSample( 0, 1 ), 0.2f );
			expectEquals( d.getSample( 1, 0 ), 0.3f );
			expectEquals( d.getSample( 1, 1 ), 0.4f );
		}
	};
	static AudioFunctionTest audioFunctionTest;
}
