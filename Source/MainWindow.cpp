// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#include "MainWindow.h"

using namespace unc;

// AsyncQuitRetrier
struct AsyncQuitRetrier : private Timer
{
	AsyncQuitRetrier(){ startTimer( 500 ); }

	// Timer
	void timerCallback() override
	{
		stopTimer();
		delete this;
		if( auto* app = JUCEApplicationBase::getInstance() ){
			app->systemRequestedQuit();
		}
	}
};

// MainWindow
MainWindow::MainWindow( const String& name ) :
	DocumentWindow( name, Desktop::getInstance().getDefaultLookAndFeel().findColour( ResizableWindow::backgroundColourId ), DocumentWindow::allButtons ),
	FileBasedDocument( ".xml", "*.xml", "Open project file", "Save project file as")
{
	setUsingNativeTitleBar( true );

	// menu
	setApplicationCommandManagerToWatch( getApplicationCommandManager() );

	// size
#if JUCE_IOS || JUCE_ANDROID
	setFullScreen( true );
#else
	setResizable( true, shouldResizeToFit() );
	setResizeLimits( 400, 300, 4000, 3000 );
	centreWithSize( getWidth(), getHeight() );
#endif
	
	// listen to undo to mark document dirty
	getUndoManager()->addChangeListener( this );

	// show MainComponent
	resetApp();
	setVisible( true );

	// commands
	getApplicationCommandManager()->registerAllCommandsForTarget( this );
	setWantsKeyboardFocus( true );
	addKeyListener( getApplicationCommandManager()->getKeyMappings() );
}

MainWindow::~MainWindow()
{
	// menu
#if JUCE_MAC
	MenuBarModel::setMacMainMenu( nullptr );
#endif

	// commands
	removeKeyListener( getApplicationCommandManager()->getKeyMappings() );

	// listen
	getUndoManager()->removeChangeListener( this );
}

// MainWindow - view
bool MainWindow::closeAllDialogues( bool isQuit )
{
	if( !closeChildProcessWindows() && isQuit ){

		// Really important thing to note here: if the last call just deleted any dependent process windows, we won't exit immediately - instead we'll use our AsyncQuitRetrier to let the message loop run for another brief moment, then try again. This will give any plugins a chance to flush any GUI events that may have been in transit before the app forces them to be unloaded
		new AsyncQuitRetrier();
		return false;
	}
	// modal dialogues
	ModalComponentManager::getInstance()->cancelAllModalComponents();
	return true;
}

bool unc::MainWindow::closeChildProcessWindows()
{
	return true;
}

void MainWindow::initWindowTitle()
{
	auto title = JUCEApplication::getInstance()->getApplicationName();
	title += " - ";
	title += getDocumentTitle();
	setName( title );
}

// MainWindow - modify
void MainWindow::resetApp()
{
	getUndoManager()->clearUndoHistory();
	closeAllDialogues( false );

	// reset mainComponent
	mainComponent.reset( new MainComponent( this ) );
	setContentNonOwned( mainComponent.get(), shouldResizeToFit() );

	// files and title
	File::getSpecialLocation( File::currentApplicationFile ).setAsCurrentWorkingDirectory();
	pointToNewProjectFile( File(), false );
}

Result unc::MainWindow::writeAppToXml( XmlElement* xml )
{
	return mainComponent->toXml( xml );
}

Result unc::MainWindow::restoreAppFromXml( XmlElement * xml )
{
	return mainComponent->fromXml( xml );
}

void unc::MainWindow::pointToNewProjectFile( const File & file, bool addToRecent )
{
	setFile( file );
	setLastDocumentOpened( file );
	if( addToRecent ){
		getRecentlyOpenedFilesList()->addFile( file );
	}
	initWindowTitle();
}

// MainWindow - DocumentWindow
void MainWindow::closeButtonPressed()
{
	getApplicationCommandManager()->invokeDirectly( StandardApplicationCommandIDs::quit, true );
}

// MainWindow - MenuBarModel
StringArray MainWindow::getMenuBarNames()
{
	return { CommandCategories::file,
		CommandCategories::edit };
}

PopupMenu MainWindow::getMenuForIndex( int topLevelMenuIndex, const String& menuName )
{
	PopupMenu m;

	// File
	if( topLevelMenuIndex == 0 ){
		m.addCommandItem( getApplicationCommandManager(), CommandIDs::newProject );
		m.addSeparator();
		m.addCommandItem( getApplicationCommandManager(), CommandIDs::openProject );

		// recent files
		PopupMenu recent;
		getRecentlyOpenedFilesList()->createPopupMenuItems( recent, 100, false, true );
		m.addSubMenu( "Open Recent", recent );
		m.addCommandItem( getApplicationCommandManager(), CommandIDs::saveProject );
		m.addCommandItem( getApplicationCommandManager(), CommandIDs::saveProjectAs );
		m.addSeparator();
		m.addCommandItem( getApplicationCommandManager(), StandardApplicationCommandIDs::quit );
	}
	// Edit
	else if( topLevelMenuIndex == 1 ){
		m.addCommandItem( getApplicationCommandManager(), StandardApplicationCommandIDs::undo );
		m.addCommandItem( getApplicationCommandManager(), StandardApplicationCommandIDs::redo );
		m.addSeparator();
		m.addCommandItem( getApplicationCommandManager(), StandardApplicationCommandIDs::del );
		m.addSeparator();
		m.addCommandItem( getApplicationCommandManager(), CommandIDs::writeAllZones );
		m.addCommandItem( getApplicationCommandManager(), CommandIDs::writeZoneToSelected );
		m.addCommandItem( getApplicationCommandManager(), CommandIDs::removeZoneFromSelected );
		m.addCommandItem( getApplicationCommandManager(), CommandIDs::sortClipsByName );
		m.addCommandItem( getApplicationCommandManager(), CommandIDs::sortClipsByLength );
	}
	return m;
}

void MainWindow::menuItemSelected( int menuItemID, int topLevelMenuIndex )
{
	// File
	if( topLevelMenuIndex == 0 ){

		// recent files
		if( menuItemID >= 100 ){
			auto idx = menuItemID - 100;
			loadFrom( getRecentlyOpenedFilesList()->getFile( idx ), true );
		}
	}
}

// MainWindow - ApplicationCommandTarget
ApplicationCommandTarget* MainWindow::getNextCommandTarget()
{
	return nullptr;
}

void MainWindow::getAllCommands( Array<CommandID>& commands )
{
	// File
	commands.add( CommandIDs::newProject );
	commands.add( CommandIDs::openProject );
	commands.add( CommandIDs::saveProject );
	commands.add( CommandIDs::saveProjectAs );
	commands.add( StandardApplicationCommandIDs::quit );
}

void MainWindow::getCommandInfo( CommandID commandID, ApplicationCommandInfo& result )
{
	switch( commandID )
	{
		// File
		case CommandIDs::newProject:{
			result.setInfo( "New", "Create New Project", CommandCategories::file, 0 );
			break;
		}
		case CommandIDs::openProject:{
			result.setInfo( "Open", "Open Project from Disk", CommandCategories::file, 0 );
			result.addDefaultKeypress( 'o', ModifierKeys::commandModifier );
			break;
		}
		case CommandIDs::saveProject:{
			result.setInfo( "Save", "Save Project to Disk", CommandCategories::file, 0 );
			result.addDefaultKeypress( 's', ModifierKeys::commandModifier );
			break;
		}
		case CommandIDs::saveProjectAs:{
			result.setInfo( "Save as...", "Save Project to new Location", CommandCategories::file, 0 );
			result.addDefaultKeypress( 's', ModifierKeys::commandModifier | ModifierKeys::shiftModifier );
			break;
		}
		case StandardApplicationCommandIDs::quit:{
			result.setInfo( "Quit", "Quit Application", CommandCategories::file, 0 );
			result.addDefaultKeypress( 'q', ModifierKeys::commandModifier );
			break;
		}
		default: break;
	}
}

bool MainWindow::perform( const ApplicationCommandTarget::InvocationInfo& info )
{
	switch( info.commandID )
	{
		// File
		case CommandIDs::newProject:{
			if( saveIfNeededAndUserAgrees() == FileBasedDocument::userCancelledSave ){
				break;
			}
			resetApp();
			break;
		}
		case CommandIDs::openProject:{
			loadFromUserSpecifiedFile( true );
			break;
		}
		case CommandIDs::saveProject:{
			auto res = save( true, true );
			if( res == FileBasedDocument::savedOk ){
				initWindowTitle(); // mark clean
			}
			break;
		}
		case CommandIDs::saveProjectAs:{
			auto res = saveAsInteractive( true );
			if( res == FileBasedDocument::savedOk ){
				initWindowTitle(); // mark clean
			}
			break;
		}
		case StandardApplicationCommandIDs::quit: {
			JUCEApplication::getInstance()->systemRequestedQuit();
			break;
		}
		default:return false;
	}
	return true;
}

// MainWindow - FileBasedDocument
String MainWindow::getDocumentTitle()
{
	if( getFile().existsAsFile() ){
		return getFile().getFileNameWithoutExtension();
	}
	return "New Project";
}

Result MainWindow::loadDocument( const File& file )
{
	// parse xml
	XmlDocument doc( file );
	std::unique_ptr<XmlElement> xml( doc.getDocumentElement() );
	String parseErr = doc.getLastParseError();

	// invalid xml
	if( !xml || !parseErr.isEmpty() ){
		resetApp();
		return Result::fail( "MainWindow::loadDocument() Error parsing xml " + parseErr );
	}
	// we need working dir to correctly resolve file paths
	file.getParentDirectory().setAsCurrentWorkingDirectory();

	// restore project
	auto restored = restoreAppFromXml( xml.get() );
	if( restored.wasOk() ){
		pointToNewProjectFile( file, true );
		return Result::ok();
	}
	else{
		resetApp();
		Logger::getCurrentLogger()->writeToLog( restored.getErrorMessage() );
		return Result::fail( "MainWindow::loadDocument() Error loading project" );
	}
}

Result MainWindow::saveDocument( const File& file )
{
	// we need working dir to correctly resolve file paths
	file.getParentDirectory().setAsCurrentWorkingDirectory();

	// app to xml
	std::unique_ptr<XmlElement> xml{ new XmlElement( JUCEApplication::getInstance()->getApplicationName() ) };
	auto written = writeAppToXml( xml.get() );
	if( written.failed() ){
		Logger::getCurrentLogger()->writeToLog( written.getErrorMessage() );
		return Result::fail( "MainWindow::saveDocument() Error saving project." );
	}
	// xml to file
	auto fileRes = xml->writeToFile( file, String() );
	if( !fileRes ){
		return Result::fail( String( "MainWindow::saveDocument() Error writing to file: " ) + file.getFullPathName() );
	}
	pointToNewProjectFile( file, true );
	return Result::ok();
}

File MainWindow::getLastDocumentOpened()
{
	return ::getLastDocumentOpened();
}

void MainWindow::setLastDocumentOpened( const File& file )
{
	::setLastDocumentOpened( file );
}

// MainWindow - ChangeListener
void MainWindow::changeListenerCallback( ChangeBroadcaster* source )
{
	// the UndoManager executed a command
	if( source == getUndoManager() ){

		// mark window dirty
		auto n = getName();
		if( !n.endsWith( "*" ) ){
			setName( n + "*" );
		}
		// set document changed
		changed();
		return;
	}
	jassertfalse;
}
