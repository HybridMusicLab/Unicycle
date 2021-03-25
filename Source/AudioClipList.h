// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#pragma once

#include "MainHeaders.h"
#include "LookAndFeel.h"

#include "AudioClip.h"
#include "AudioCommands.h"
#include "Commands.h"
#include "MainInterface.h"

namespace unc
{
	class AudioClipListModel : public ListBoxModel
	{
	public:
		AudioClipListModel( AudioClips* clips, class AudioClipList* clipList );

		// ListBoxModel
		int getNumRows() override;
		void paintListBoxItem( int rowNumber, Graphics& g, int width, int height, bool rowIsSelected ) override;
		void selectedRowsChanged( int lastRowSelected )override;

		AudioClips* clips = nullptr;
		AudioClipList* clipList = nullptr;
	};

	class AudioClipList : public Component,
		public ApplicationCommandTarget,
		public ChangeListener,
		public FileDragAndDropTarget
	{
	public:
		AudioClipList();
		~AudioClipList();

		// view
		void display( AudioClips* other );

		// Component
		void paint( Graphics& g )override;
		void resized() override;
		void mouseDown( const MouseEvent& e )override;

		// modify
		void selectRow( int row );

		// access
		Array<AudioClip*> getListSelection() const;
		int getRowHeight() const{ return lnf::dims::h + lnf::dims::pad; }

		// ApplicationCommandTarget
		ApplicationCommandTarget* getNextCommandTarget() override;
		void getAllCommands( Array<CommandID>& commands ) override;
		void getCommandInfo( CommandID commandID, ApplicationCommandInfo& result ) override;
		bool perform( const InvocationInfo& info ) override;

		// ChangeListener
		void changeListenerCallback( ChangeBroadcaster* source )override;

		// FileDragAndDropTarget
		bool isInterestedInFileDrag( const StringArray& files )override;
		void filesDropped( const StringArray& files, int x, int y )override;
		
		AudioClips* clips = nullptr;
		
	private:
		std::unique_ptr<ListBoxModel> listModel{ nullptr };
		ListBox listBox;
		TextButton outButton{ "Outpath" };
		Label outDisplay{ "outDisplay" };
		File outPath;
		TextButton renderButton{ "Render" };

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AudioClipList );
	};
}
