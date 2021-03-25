// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#pragma once

#include "MainHeaders.h"

#include "AudioFunctions.h"
#include "AudioPlayback.h"

namespace unc
{
	/// How audio can be played back.
	enum class AudioPlayMode
	{
		Play, Loop, NumModes
	};

	inline String toString( AudioPlayMode mode )
	{
		switch( mode ){
			case AudioPlayMode::Play: return "Play";
			case AudioPlayMode::Loop: return "Loop";
			default: return "Invalid";
		}
	}
	AudioPlayMode fromString( const String& mode );

	/// Positions and play style.
	struct AudioPlayZone
	{
		int start = 0;
		int length = 0;
		int fadeIn = 0;
		int fadeOut = 0;
		AudioPlayMode mode = AudioPlayMode::NumModes;
		String name = toString( mode );

		// access
		bool isValid() const{ return start >= 0 && length > 0; }
		bool operator==( const AudioPlayZone& other )const;

		// persistence
		void toXml( XmlElement* xml )const;
		void fromXml( XmlElement* xml );
	};
	using AudioPlayZones = std::vector<AudioPlayZone>;

	AudioBuffer<float>* writePlay( const AudioBuffer<float>& source, int start, int length, int fadeIn, int fadeOut );
	AudioBuffer<float>* writeLoop( const AudioBuffer<float>& source, int start, int length, int xfade );
	
	/// Binds audio data to AudioPlayZones.
	class AudioClip :	public ChangeBroadcaster
	{
	public:
		AudioClip(){};

		using Ptr = std::shared_ptr<AudioClip>;

		// process
		AudioBuffer<float>* writeAudio( int zoneIndex );

		// modify
		bool addZone( const AudioPlayZone& zone );
		bool setZone( int zoneIndex, const AudioPlayZone& zone );
		bool removeZone( int zoneIndex );
		void clearZones();

		// access
		String getName() const{ return name; }
		AudioPlayZone getZone( int zoneIndex ) const;
		int indexOfZone( const AudioPlayZone& zone )const;
		int sizeZones() const{ return zones.size(); }
		int getTotalNumSamples() const{ return buffer.getNumSamples(); }
		int getNumChannels() const{ return buffer.getNumChannels(); }
		bool containsZone( const AudioPlayZone& zone )const;

		// persistence
		Result toXml( XmlElement* xml )const;
		Result fromXml( XmlElement* xml );

		File file;
		String name;
		AudioBuffer<float> buffer;
		AudioPlayZones zones;
		double sampleRate = 0.;
		int bitDepth = 0;

	private:
		// modify
		void sort();

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AudioClip );
	};
	AudioClip::Ptr createAudioClip( const AudioBuffer<float>& buffer, const AudioSettings& settings, const String& name = String());
	AudioClip::Ptr createAudioClip( const File& file );
	AudioClip::Ptr createAudioClip();

	/// Handling AudioClips as a list.
	class AudioClips : public ChangeBroadcaster
	{
	public:
		AudioClips(){}

		// modify
		bool add( const AudioClip::Ptr& clip );
		bool remove( int index );
		enum SortMethod
		{
			Filename, Length, Unsorted
		};
		void sort( SortMethod sort );

		// access
		SortMethod getSortMethod() const{ return sortMethod; }
		AudioClip* get( int index ) const{ return getPtr( index ).get(); }
		AudioClip::Ptr getPtr( int index ) const;
		int indexOf( AudioClip* clip ) const;
		int indexFor( const File& file ) const;
		int size() const{ return clips.size(); }
		bool contains( AudioClip* clip ) const;
		bool containsWith( const File& file ) const;

		// persistence
		Result toXml( XmlElement* xml )const;
		Result fromXml( XmlElement* xml );

	private:
		std::vector<AudioClip::Ptr> clips;
		SortMethod sortMethod = Filename;

		JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR( AudioClips );
	};
}
