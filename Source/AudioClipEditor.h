// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#pragma once

#include "MainHeaders.h"
#include "LookAndFeel.h"

#include "AudioClip.h"
#include "AudioCommands.h"
#include "AudioTimeline.h"
#include "TimelineInspector.h"
#include "TimelineViewport.h"

namespace unc
{
	class AudioClipDisplay : public Component,
		public ChangeListener
	{
	public:
		AudioClipDisplay( Timeline* timeline );
		~AudioClipDisplay();

		// view
		void display( AudioClip* other );

		// Component
		void paint( Graphics& g )override;
		void resized() override;

		// ChangeListener
		void changeListenerCallback( ChangeBroadcaster* source )override;

	private:
		AudioClip* clip = nullptr;
		Timeline* timeline = nullptr;
		AudioThumbnail thumbnail;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AudioClipDisplay );
	};

	class AudioClipEditor : public TimelineViewport
	{
	public:
		AudioClipEditor();
		~AudioClipEditor();

		// view
		void display( AudioClip* other );

		// Component
		void paint( Graphics& g )override;
		void resized() override;
		
		// TimelineViewport
		void handleHorizontalScroll( float deltaX ) override;
		void handleHorizontalZoom( float deltaX ) override;
		void handleVerticalScroll( float deltaY ) override {};

		// access
		AudioClip* getClip() const{ return clip; }

	private:
		AudioClip* clip = nullptr;
		Timeline timeline;
		AudioTimeline audioTimeline;
		AudioClipDisplay audioClipDisplay;
		TimelineInspector timelineInspector;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AudioClipEditor );
	};
}
