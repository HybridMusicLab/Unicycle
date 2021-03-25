// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

namespace unc
{
	enum CommandIDs
	{
		// File
		newProject = 1,
		openProject,
		saveProject,
		saveProjectAs,

		// Edit
		writeAllZones,
		writeZoneToSelected,
		removeZoneFromSelected,
		sortClipsByName,
		sortClipsByLength
	};

	namespace CommandCategories
	{
		static const String file( "File" );
		static const String edit( "Edit" );
	}
}
