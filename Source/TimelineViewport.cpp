// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#include "TimelineViewport.h"

using namespace unc;

// TimelineViewport  - Component
void unc::TimelineViewport::mouseWheelMove( const MouseEvent& e, const MouseWheelDetails& wheel )
{
	float wheelDelta = wheel.deltaY != 0.f ? wheel.deltaY : wheel.deltaX;

	// horizontal zoom
	if( e.mods.isCommandDown() ) {
		handleHorizontalZoom( wheelDelta );
	}
	// vertical scrolling
	else if( e.mods.isShiftDown() ) {
		handleVerticalScroll( wheelDelta );
	}
	// horizontal scrolling
	else {
		handleHorizontalScroll( wheelDelta );
	}
}
