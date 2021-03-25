// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#pragma once

#include "MainHeaders.h"
#include "LookAndFeel.h"

#include "Commands.h"
#include "MainComponent.h"

namespace unc
{
	class MainWindow : public DocumentWindow,
		public MenuBarModel,
		public ApplicationCommandTarget,
		public FileBasedDocument,
		public ChangeListener
	{
	public:
		MainWindow( const String& name );
		~MainWindow();

		// view
		/// \param andQuit if true, then this will recursively call JUCEApplication::systemRequestedQuit() until all dialogues (plugins etc.) are destroyed.
		bool closeAllDialogues( bool andQuit );

		// modify
		void resetApp();

		// DocumentWindow
		void closeButtonPressed() override;

		// MenuBarModel
		StringArray getMenuBarNames() override;
		PopupMenu getMenuForIndex( int topLevelMenuIndex, const String& menuName )override;
		void menuItemSelected( int menuItemID, int topLevelMenuIndex )override;

		// ApplicationCommandTarget
		ApplicationCommandTarget* getNextCommandTarget() override;
		void getAllCommands( Array<CommandID>& commands ) override;
		void getCommandInfo( CommandID commandID, ApplicationCommandInfo& result ) override;
		bool perform( const InvocationInfo& info ) override;

		// FileBasedDocument
		String getDocumentTitle() override;
		Result loadDocument( const File& file )override;
		Result saveDocument( const File& file )override;
		File getLastDocumentOpened() override;
		void setLastDocumentOpened( const File& file )override;

		// ChangeListener
		void changeListenerCallback( ChangeBroadcaster* source )override;

		// access
		MainComponent* getMainComponent() const{ return mainComponent.get(); }

		/// Decide wether MainWindow resizes to fit the mainComponent.
		bool shouldResizeToFit() const{ return true; }

	private:
		// view
		bool closeChildProcessWindows();

		/// Sets title to "app - fileName", without dirty mark.
		void initWindowTitle();

		// modify
		Result writeAppToXml( XmlElement* xml );
		Result restoreAppFromXml( XmlElement* xml );
		void pointToNewProjectFile( const File& file, bool addToRecent );

		std::unique_ptr<MainComponent> mainComponent{ nullptr };

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( MainWindow )
	};
}
