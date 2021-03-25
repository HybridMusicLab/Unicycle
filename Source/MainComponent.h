// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#pragma once

#include "MainHeaders.h"
#include "LookAndFeel.h"

#include "AudioClipEditor.h"
#include "AudioClipList.h"
#include "AudioSettingsDisplay.h"
#include "Commands.h"
#include "MainInterface.h"
#include "Timeline.h"

namespace unc
{
	// MainComponent
	class MainComponent : public Component,
		public MainInterface,
		public ApplicationCommandTarget
	{
	public:
		MainComponent( MenuBarModel* menuBarModel );
		~MainComponent();
		
		// Component
		void paint( Graphics& ) override;
		void resized() override;

		// MainInterface
		void playAudioBuffer( AudioBuffer<float>* buffer, bool shouldLoop )override;
		void stopPlaying();

		// ApplicationCommandTarget
		ApplicationCommandTarget* getNextCommandTarget() override;
		void getAllCommands( Array<CommandID>& commands ) override;
		void getCommandInfo( CommandID commandID, ApplicationCommandInfo& result ) override;
		bool perform( const InvocationInfo& info ) override;

		// modify
		void selectAudioClip( AudioClip* audioClip )override;
		void selectAudioPlayZone( const AudioPlayZone& playZone )override;

		// access
		AudioClips* getAudioClips(){ return &audioClips; }
		AudioClip* getSelectedAudioClip() const override{ return selectedAudioClip; }
		Array<AudioClip*> getSelectedAudioClips() const;
		AudioPlayZone getSelectedPlayZone() const override{ return selectedPlayZone; }

		// persistence
		Result toXml( XmlElement* xml )const;
		Result fromXml( XmlElement* xml );

	private:
		MenuBarComponent menuBar;
		AudioClips audioClips;
		AudioClip* selectedAudioClip = nullptr;
		AudioPlayZone selectedPlayZone;
		void* selectedRecently = nullptr;
		AudioClipList audioClipList;
		AudioClipEditor audioClipEditor;
		aud::AudioSettingsDisplay audioSettingsDisplay;
		AudioSourcePlayer soundPlayer;
		std::unique_ptr<MemoryAudioSource> playedSource;
		std::unique_ptr<AudioBuffer<float>> playedBuffer;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( MainComponent )
	};
}
