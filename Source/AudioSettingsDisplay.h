// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#pragma once

#include "MainHeaders.h"
#include "LookAndFeel.h"

namespace aud
{
	class AudioSettingsDisplay : public Component,
		public AudioSettingsListener
	{
	public:
		AudioSettingsDisplay();
		~AudioSettingsDisplay();

		// Component
		void paint( Graphics& g )override;
		void resized() override;

		// AudioSettingsListener
		void audioSettingsChanged( const AudioSettings& newAudioSettings )override;

	private:
		Label srLabel{ "srLabel", "SR" };
		Label srDisplay{ "srDisplay" };
		Label bufferLabel{ "bufferLabel", "Buf" };
		Label bufferDisplay{ "bufferDisplay" };
		Label bitsLabel{ "bitsLabel", "Bits" };
		Label bitsDisplay{ "bitsDisplay" };

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AudioSettingsDisplay );
	};
}
