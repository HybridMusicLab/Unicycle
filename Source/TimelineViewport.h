// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#pragma once

#include "MainHeaders.h"
#include "LookAndFeel.h"

namespace unc
{
	class TimelineViewport : public Component
	{
	public:
		virtual ~TimelineViewport() {}

		virtual void handleHorizontalScroll( float deltaX ){};
		virtual void handleHorizontalZoom( float deltaX ){}
		virtual void handleVerticalScroll( float deltaY ){}

		// Compoment
		void mouseWheelMove( const MouseEvent& event, const MouseWheelDetails& wheel ) override;
	};
}
