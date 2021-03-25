// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#pragma once

#include "MainHeaders.h"
#include "LookAndFeel.h"

#include "AudioClip.h"
#include "AudioCommands.h"
#include "Commands.h"
#include "MainInterface.h"
#include "Timeline.h"

namespace unc
{
	/// Box representing a AudioPlayZone
	class ZoneBox : public Component
	{
	public:
		ZoneBox( const AudioPlayZone& zone, class AudioTimeline* parent );

		// Component
		void paint( Graphics& g )override;
		void mouseDown( const MouseEvent& e )override;
		void mouseDrag( const MouseEvent& e )override;
		void mouseUp( const MouseEvent& e )override;

		// access
		Rectangle<int> startHandle() const;
		Rectangle<int> endHandle() const;
		Rectangle<int> fadeInHandle() const;
		Rectangle<int> fadeOutHandle() const;
		bool isLooping() const{ return zone.mode == AudioPlayMode::Loop; }

		AudioPlayZone zone;

	private:
		AudioTimeline* parent = nullptr;
		AudioClip* clip = nullptr;
		enum MouseMode
		{
			DragStart, DragLen, DragFadeIn, DragFadeOut, Play, Idle
		};
		MouseMode mouseMode;
		AudioPlayZone old;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( ZoneBox );
	};

	class AudioTimeline : public Component,
		public ChangeListener
	{
	public:
		AudioTimeline( Timeline* timeline );
		~AudioTimeline();

		// view
		void display( AudioClip* other );
		void rebuild();
		void update();

		// Component
		void paint( Graphics& g )override;
		void paintOverChildren( Graphics& g )override;
		void resized() override;
		void mouseDown( const MouseEvent& e )override;
		void mouseDrag( const MouseEvent& e )override;
		void mouseUp( const MouseEvent& e )override;

		// modify
		void setSelected( ZoneBox* box );

		// access
		Rectangle<int> getBoundsFor( ZoneBox* box )const;
		int getGridSize() const{ return timeline->getGridSizeSamps(); }
		int samplesToPix( int samples ) const;
		int pixPosFloor( int compX )const;
		int pixPosCeil( int compX )const;
		int pixToSamples( int distX )const;
		bool isSelected( ZoneBox* box )const;

		// ChangeListener
		void changeListenerCallback( ChangeBroadcaster* source )override;

		AudioClip* clip = nullptr;

	private:
		OwnedArray<ZoneBox> boxes;
		Timeline* timeline = nullptr;
		enum MouseMode
		{
			DragNew, Idle
		};
		MouseMode mouseMode = Idle;
		Rectangle<int> dragRect;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AudioTimeline );
	};
}
