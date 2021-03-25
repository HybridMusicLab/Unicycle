// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#include "MainHeaders.h"

#include "LookAndFeel.h"
#include "MainWindow.h"
#include "Tests.h"

using namespace unc;

class UnicycleApplication  : public JUCEApplication,
	public ChangeListener
{
public:
	UnicycleApplication() {}

	// access
    const String getApplicationName() override{ return ProjectInfo::projectName; }
    const String getApplicationVersion() override{ return ProjectInfo::versionString; }
    bool moreThanOneInstanceAllowed() override{ return true; }

    void initialise( const String& commandLine )override
    {
		// run tests
		UnitTestRunner runner;
		runner.runAllTests();

		// appProperties
		PropertiesFile::Options options;
		options.applicationName = getApplicationName();
		options.filenameSuffix = "settings";
		options.osxLibrarySubFolder = String( "Application Support/" ) + getApplicationName();
		appProperties.setStorageParameters( options );

		// init project dir
		lastDocOpened = File::getCurrentWorkingDirectory();

		// recentFilesList
		auto recentFiles = appProperties.getUserSettings()->getValue( recentFilesId );
		if( !recentFiles.isEmpty() ){
			recentFilesList.restoreFromString( recentFiles );
		}
		recentFilesList.setMaxNumberOfItems( 10 );

		// audio formats
		audioFormatManager.registerBasicFormats();

		// audio devices manager
		audioDeviceManager.addChangeListener( this );
		std::unique_ptr<XmlElement> savedAudioState( appProperties.getUserSettings()->getXmlValue( audioDeviceStateId ));
		auto audioErr = audioDeviceManager.initialise( 0, 2, savedAudioState.get(), true );
		if( !audioErr.isEmpty() ) {
			// something went wrong during audio init
			jassertfalse;
		}
		// audio device manager - midi
		for( const auto& s : MidiInput::getDevices() ) {
			audioDeviceManager.setMidiInputEnabled( s, true );
		}
		// lnf
		LookAndFeel::setDefaultLookAndFeel( &look );

		// mainWindow
        mainWindow.reset( new MainWindow( getApplicationName()));

		// keypresses after mainWindow initialized
		appCommandManager.getKeyMappings()->resetToDefaultMappings();
    }

    void shutdown() override
    {
		// recentFilesList
		appProperties.getUserSettings()->setValue( recentFilesId, recentFilesList.toString() );

		// appProperties
		std::unique_ptr<XmlElement> savedAudioState( audioDeviceManager.createStateXml() );
		appProperties.getUserSettings()->setValue( audioDeviceStateId, savedAudioState.get() );
		appProperties.saveIfNeeded();

		// mainWindow
        mainWindow = nullptr;

		// lnf
		LookAndFeel::setDefaultLookAndFeel( nullptr );
    }

	void systemRequestedQuit() override
	{
		// this will call systemRequestedQuit() recursively if any window is still open
		if( !mainWindow->closeAllDialogues( true ) ){
			return;
		}
		if( mainWindow->saveIfNeededAndUserAgrees() != FileBasedDocument::userCancelledSave ){
			quit();
		}
	}

    void anotherInstanceStarted (const String& commandLine) override
    {}

	// ChangeListener
	void changeListenerCallback( ChangeBroadcaster* source )override
	{
		if( source == &audioDeviceManager ){
			auto setup = audioDeviceManager.getAudioDeviceSetup();
			audioSettings.sampleRate = setup.sampleRate;
			audioSettings.bufferSize = setup.bufferSize;
			audioSettingsListeners.call( &AudioSettingsListener::audioSettingsChanged, audioSettings );
		}
	}
	
	lnf::Look look;
    std::unique_ptr<MainWindow> mainWindow;
	
	// app
	UndoManager undoManager;
	ApplicationCommandManager appCommandManager;
	ApplicationProperties appProperties;
	RecentlyOpenedFilesList recentFilesList;
	File lastDocOpened;
	const String recentFilesId{ "recentFiles" };
	const String audioDeviceStateId{ "audioDeviceState" };

	// audio
	AudioDeviceManager audioDeviceManager;
	AudioFormatManager audioFormatManager;
	AudioThumbnailCache audioThumbnailCache{ ( 512 ) };
	AudioSettings audioSettings{ 44100., 1024, 24 };
	ListenerList<AudioSettingsListener> audioSettingsListeners;
};

// app
UnicycleApplication* getApp()
{
	return static_cast< UnicycleApplication* >( JUCEApplication::getInstance() );
}

UndoManager* getUndoManager()
{
	return &getApp()->undoManager;
}

ApplicationCommandManager* getApplicationCommandManager()
{
	return &getApp()->appCommandManager;
}

ApplicationProperties* getApplicationProperties()
{
	return &getApp()->appProperties;
}

RecentlyOpenedFilesList* getRecentlyOpenedFilesList()
{
	return &getApp()->recentFilesList;
}

File getLastDocumentOpened()
{
	return getApp()->lastDocOpened;
}

void setLastDocumentOpened( const File& file )
{
	getApp()->lastDocOpened = file;
}

// audio
AudioDeviceManager* getAudioDeviceManager()
{
	return &getApp()->audioDeviceManager;
}

AudioFormatManager* getAudioFormatManager()
{
	return &getApp()->audioFormatManager;
}

AudioThumbnailCache* getAudioThumbnailCache()
{
	return &getApp()->audioThumbnailCache;
}

AudioSettings getCurrentAudioSettings()
{
	return getApp()->audioSettings;
}

void addAudioSettingsListener( AudioSettingsListener* listener )
{
	getApp()->audioSettingsListeners.add( listener );
}

void removeAudioSettingsListener( AudioSettingsListener* listener )
{
	getApp()->audioSettingsListeners.remove( listener );
}

// This macro generates the main() routine that launches the app.
START_JUCE_APPLICATION( UnicycleApplication )
