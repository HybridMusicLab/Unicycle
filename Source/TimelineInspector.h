// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#pragma once

#include "MainHeaders.h"
#include "LookAndFeel.h"

#include "Timeline.h"

namespace unc
{
	class TimelineInspector :	public Component,
		public ChangeListener
	{
	public:
		TimelineInspector( Timeline* timeline );
		~TimelineInspector();

			// Component
			void paint( Graphics& g )override;
			void resized() override;

			// ChangeListener
			void changeListenerCallback( ChangeBroadcaster* source )override;

	private:
		Timeline* timeline;
		TextButton gridSwitch{ "Grid" };
		Label gridInput{ "gridInput" };
		Label gridLabel{ "gridLabel", "Sec" };
		double gridSize = 0.;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( TimelineInspector );
	};
}
