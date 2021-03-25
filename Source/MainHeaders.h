// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <array>
#include <atomic>
#include <bitset>
#include <cmath>
#include <map>
#include <memory>
#include <numeric>
#include <random>
#include <vector>

// app
UndoManager* getUndoManager();
ApplicationCommandManager* getApplicationCommandManager();
ApplicationProperties* getApplicationProperties();
RecentlyOpenedFilesList* getRecentlyOpenedFilesList();
File getLastDocumentOpened();
void setLastDocumentOpened( const File& file );

// audio
AudioDeviceManager* getAudioDeviceManager();
AudioFormatManager* getAudioFormatManager();
AudioThumbnailCache* getAudioThumbnailCache();
struct AudioSettings
{
	double sampleRate = 0.;
	int bufferSize = 0;
	int bitsPerSample = 0;
};
AudioSettings getCurrentAudioSettings();

class AudioSettingsListener
{
public:
	virtual ~AudioSettingsListener(){}
	virtual void audioSettingsChanged( const AudioSettings& newAudioSettings ) = 0;
};
void addAudioSettingsListener( AudioSettingsListener* listener );
void removeAudioSettingsListener( AudioSettingsListener* listener );