// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#pragma once

#include "AudioClip.h"

namespace unc
{
	class AudioClipTest : public UnitTest
	{
	public:
		AudioClipTest() : UnitTest( "AudioClipTest" ){}

		void runTest() override
		{
			testWriteZone();
		}

		void testWriteZone()
		{
			beginTest( "testWriteZone" );

			// source buffer
			AudioBuffer<float> b( 1, 8 );
			b.setSample( 0, 0, 0.1f );
			b.setSample( 0, 1, 0.2f );
			b.setSample( 0, 2, 0.3f );
			b.setSample( 0, 3, 0.4f );
			b.setSample( 0, 4, 0.5f );
			b.setSample( 0, 5, 0.6f );
			b.setSample( 0, 6, 0.7f );
			b.setSample( 0, 7, 0.8f );

			// test zone play
			std::unique_ptr<AudioBuffer<float>> play( writePlay( b, 0, 4, 0, 0 ) );
			expectWithinAbsoluteError( play->getSample( 0, 0 ), 0.1f, 0.00001f );
			expectWithinAbsoluteError( play->getSample( 0, 1 ), 0.2f, 0.00001f );
			expectWithinAbsoluteError( play->getSample( 0, 2 ), 0.3f, 0.00001f );
			expectWithinAbsoluteError( play->getSample( 0, 3 ), 0.4f, 0.00001f );

			// test zone loop
			std::unique_ptr<AudioBuffer<float>> loop( writeLoop( b, 0, 6, 2 ) );
			expectWithinAbsoluteError( loop->getSample( 0, 0 ), 0.3f, 0.00001f );
			expectWithinAbsoluteError( loop->getSample( 0, 1 ), 0.4f, 0.00001f );
			expectWithinAbsoluteError( loop->getSample( 0, 2 ), 0.5f, 0.00001f );
			expectWithinAbsoluteError( loop->getSample( 0, 3 ), 0.4f, 0.00001f );
		}
	};
	static AudioClipTest audioClipTest;
}
