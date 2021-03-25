// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#pragma once

#include "MainHeaders.h"

namespace unc
{
	class MainInterface
	{
	public:
		virtual ~MainInterface(){}

		// process
		virtual void playAudioBuffer( AudioBuffer<float>* buffer, bool shouldLoop ) = 0;
		virtual void stopPlaying() = 0;

		// modify
		virtual void selectAudioClip( AudioClip* audioClip ) = 0;
		virtual void selectAudioPlayZone( const AudioPlayZone& playZone ) = 0;

		// access
		virtual AudioClip* getSelectedAudioClip() const = 0;
		virtual AudioPlayZone getSelectedPlayZone() const = 0;
	};
}
