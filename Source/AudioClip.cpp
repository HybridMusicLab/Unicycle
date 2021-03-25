// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#include "AudioClip.h"

using namespace unc;

// AudioPlayMode
AudioPlayMode unc::fromString( const String& mode )
{
	auto numModes = static_cast< int >( AudioPlayMode::NumModes );
	for( int i = 0; i < numModes; ++i ){
		auto m = static_cast< AudioPlayMode >( i );
		if( toString( m ) == mode ){
			return m;
		}
	}
	return AudioPlayMode::NumModes;
}

// AudioPlayZone
bool unc::AudioPlayZone::operator==( const AudioPlayZone & other ) const
{
	return start == other.start
		&& length == other.length
		&& fadeIn == other.fadeIn
		&& fadeOut == other.fadeOut
		&& mode == other.mode
		&& name == other.name;
}

void unc::AudioPlayZone::toXml( XmlElement * xml )const
{
	xml->setAttribute( "start", start );
	xml->setAttribute( "length", length );
	xml->setAttribute( "fadeIn", fadeIn );
	xml->setAttribute( "fadeOut", fadeOut );
	xml->setAttribute( "mode", toString( mode ) );
	xml->setAttribute( "name", name );
}

void unc::AudioPlayZone::fromXml( XmlElement * xml )
{
	start = xml->getIntAttribute( "start", 0 );
	length = xml->getIntAttribute( "length", 0 );
	fadeIn = xml->getIntAttribute( "fadeIn", 0 );
	fadeOut = xml->getIntAttribute( "fadeOut", 0 );
	mode = fromString( xml->getStringAttribute( "mode", toString( AudioPlayMode::Play ) ) );
	name = xml->getStringAttribute( "name" );
}

AudioBuffer<float>* unc::writePlay( const AudioBuffer<float>& source, int start, int length, int fadeIn, int fadeOut )
{
	jassert( fadeIn + fadeOut <= length );

	// play from start to end
	aud::Resampler play( source );
	play.setRange( start, length );
	play.setFadeIn( fadeIn );
	play.setFadeOut( fadeOut );
	auto ret = new AudioBuffer<float>( aud::MaxNumAudioChannels, length );
	play.process( *ret );
	return ret;
}

AudioBuffer<float>* unc::writeLoop( const AudioBuffer<float>& source, int start, int length, int xFade )
{
	jassert( xFade <= length );

	// loop starts after fade in, length gets trimmed by that amount
	length -= xFade;

	// loop play from xFade
	aud::Resampler loop( source );
	loop.setRange( start + xFade, length );
	loop.setFadeOut( xFade );
	auto ret = new AudioBuffer<float>( aud::MaxNumAudioChannels, length );
	loop.process( *ret );

	// xfade
	auto end = start + length;
	aud::Resampler fade( source );
	fade.setRange( start, xFade );
	fade.setFadeIn( xFade );
	AudioBuffer<float> tmp( aud::MaxNumAudioChannels, xFade );
	fade.process( tmp );
	aud::addBuffer( tmp, *ret, length - xFade );
	return ret;
}

// AudioClip - process
AudioBuffer<float>* AudioClip::writeAudio( int zoneIndex )
{
	if( !isPositiveAndBelow( zoneIndex, sizeZones() ) ){
		return nullptr;
	}
	auto zone = zones[ zoneIndex ];
	switch( zone.mode ){
		case AudioPlayMode::Play:{
			return writePlay( buffer, zone.start, zone.length, zone.fadeIn, zone.fadeOut );
		}
		case AudioPlayMode::Loop:{
			return writeLoop( buffer, zone.start, zone.length, zone.fadeOut );
		}
		default:{
			jassertfalse;
			return nullptr;
		}
	}
}

// AudioClip - modify
bool unc::AudioClip::addZone( const AudioPlayZone& zone )
{
	if( !zone.isValid()){
		sendChangeMessage();
		return false;
	}
	if( zone.start + zone.length > getTotalNumSamples() ){
		sendChangeMessage();
		return false;
	}
	if( containsZone( zone ) ) {
		return false;
	}
	zones.push_back( zone );
	sort();
	sendChangeMessage();
	return true;
}

bool unc::AudioClip::setZone( int zoneIndex, const AudioPlayZone& zone )
{
	if( !isPositiveAndBelow( zoneIndex, sizeZones() ) ){
		return false;
	}
	if( zone.length == 0 ){
		sendChangeMessage();
		return false;
	}
	zones[ zoneIndex ] = zone;
	sort();
	sendChangeMessage();
	return true;
}

bool unc::AudioClip::removeZone( int zoneIndex )
{
	if( !isPositiveAndBelow( zoneIndex, sizeZones())){
		return false;
	}
	zones.erase( zones.begin() + zoneIndex );
	sendChangeMessage();
	return true;
}

void unc::AudioClip::clearZones()
{
	zones.clear();
	sendChangeMessage();
}

void unc::AudioClip::sort()
{
	std::sort( zones.begin(), zones.end(), []( const auto& first, const auto& second ){
		if( first.start == second.start ){
			return first.length < second.length;
		}
		return first.start < second.start;
	});
}

AudioPlayZone unc::AudioClip::getZone( int zoneIndex ) const
{
	if( !isPositiveAndBelow( zoneIndex, sizeZones() ) ){
		return AudioPlayZone();
	}
	return zones[ zoneIndex ];
}

// AudioClip - access
int unc::AudioClip::indexOfZone( const AudioPlayZone& zone )const
{
	for( int i = 0; i < sizeZones(); ++i ){
		if( zones[ i ] == zone ){
			return i;
		}
	}
	return -1;
}

bool unc::AudioClip::containsZone( const AudioPlayZone& zone ) const
{
	return indexOfZone( zone ) >= 0;
}

Result unc::AudioClip::toXml( XmlElement* xml )const
{
	if( !file.existsAsFile() ){
		return Result::fail( "AudioClip::toXml() No file" );
	}
	xml->setAttribute( "file", file.getRelativePathFrom( File::getCurrentWorkingDirectory() ) );
	xml->setAttribute( "name", name );
	for( const auto& zone : zones ){
		zone.toXml( xml->createNewChildElement( "AudioPlayZone" ) );
	}
	return Result::ok();
}

Result unc::AudioClip::fromXml( XmlElement* xml )
{
	file = File::getCurrentWorkingDirectory().getChildFile( xml->getStringAttribute( "file" ) );
	name = xml->getStringAttribute( "name" );
	String err;
	bool success = true;
	AudioSettings settings;
	buffer = aud::createOrGetBufferFor( file, settings );
	if( buffer.getNumSamples() == 0 ){
		success = false;
		err += "AudioClip::fromXml() Error reading file " + file.getFullPathName();
	}
	sampleRate = settings.sampleRate;
	bitDepth = settings.bitsPerSample;
	forEachXmlChildElementWithTagName( *xml, zoneXml, "AudioPlayZone" ){
		AudioPlayZone zone;
		zone.fromXml( zoneXml );
		success &= addZone( zone );
	}
	return success ? Result::ok() : Result::fail( err );
}

AudioClip::Ptr unc::createAudioClip( const File& file )
{
	// make audio buffer, overwrite app audio settings
	AudioSettings settings = getCurrentAudioSettings();
	auto buf = aud::createOrGetBufferFor( file, settings );

	// create audio clip
	auto ret = createAudioClip( buf, settings, file.getFileNameWithoutExtension());
	if( !ret ){
		return ret;
	}
	ret->file = file;
	return ret;
}

AudioClip::Ptr unc::createAudioClip( const AudioBuffer<float>& buffer, const AudioSettings& settings, const String& name )
{
	if( buffer.getNumSamples() == 0 ){
		return nullptr;
	}
	auto ret = std::make_shared<AudioClip>();
	ret->name = name.isEmpty() ? "Unnamed" : name;
	ret->buffer = buffer;
	ret->sampleRate = settings.sampleRate;
	ret->bitDepth = settings.bitsPerSample;
	return ret;
}

AudioClip::Ptr unc::createAudioClip()
{
	return std::make_shared<AudioClip>();
}

// AudioClips - modify
bool unc::AudioClips::add( const AudioClip::Ptr& clip )
{
	if( !clip ){
		return false;
	}
	// add unique
	if( contains( clip.get()) || containsWith( clip->file )){
		return false;
	}
	clips.push_back( clip );
	sort( sortMethod );
	sendChangeMessage();
	return true;
}

bool unc::AudioClips::remove( int index )
{
	if( !isPositiveAndBelow( index, size())){
		return false;
	}
	clips.erase( clips.begin() + index );
	sendChangeMessage();
	return true;
}

void unc::AudioClips::sort( SortMethod sort )
{
	if( sort == Filename ){
		std::sort( clips.begin(), clips.end(), []( const auto& first, const auto& second ){
			return first->getName().compareNatural( second->getName() ) < 0;
		} );
	}
	else if( sort == Length ){
		std::sort( clips.begin(), clips.end(), []( const auto& first, const auto& second ){
			return first->getTotalNumSamples() < second->getTotalNumSamples();
		} );
	}
	sortMethod = sort;
	sendChangeMessage();
}

// AudioClips - access
AudioClip::Ptr unc::AudioClips::getPtr( int index ) const
{
	if( !isPositiveAndBelow( index, size() ) ){
		return nullptr;
	}
	return clips[ index ];
}

int unc::AudioClips::indexOf( AudioClip* clip )const
{
	for( int i = 0; i < size(); ++i ){
		if( clips[ i ].get() == clip ){
			return i;
		}
	}
	return -1;
}

int unc::AudioClips::indexFor( const File& file )const
{
	for( int i = 0; i < size(); ++i ){
		if( clips[ i ]->file == file ){
			return i;
		}
	}
	return -1;
}

bool unc::AudioClips::contains( AudioClip* clip )const
{
	return indexOf( clip ) >= 0;
}

bool unc::AudioClips::containsWith( const File& file )const
{
	return indexFor( file ) >= 0;
}

String toString( AudioClips::SortMethod m )
{
	switch( m ){
		case AudioClips::SortMethod::Filename: return "Filename";
		case AudioClips::SortMethod::Length: return "Length";
		default: return "Unsorted";
	}
}

AudioClips::SortMethod fromString( const String& s )
{
	if( s == "Filename" ){
		return AudioClips::SortMethod::Filename;
	}
	else if( s == "Length" ){
		return AudioClips::SortMethod::Length;
	}
	else{
		return AudioClips::SortMethod::Unsorted;
	}
}

// AudioClips - persistence
Result unc::AudioClips::toXml( XmlElement * xml ) const
{
	String err;
	bool success = true;

	// clips
	for( int i = 0; i < size(); ++i ){
		auto clipRes = get( i )->toXml( xml->createNewChildElement( "AudioClip" ) );
		if( clipRes.failed() ){
			err += clipRes.getErrorMessage();
			err += newLine;
			success = false;
		}
	}
	// sortMethod
	xml->setAttribute( "sortMethod", ::toString( sortMethod ) );
	return success ? Result::ok() : Result::fail( err );
}

Result unc::AudioClips::fromXml( XmlElement * xml )
{
	String err;
	bool success = true;

	// sortMethod
	sortMethod = ::fromString( xml->getStringAttribute( "sortMethod" ) );

	// clips
	forEachXmlChildElementWithTagName( *xml, clipXml, "AudioClip" ){
		auto clip = createAudioClip();
		auto parse = clip->fromXml( clipXml );
		if( parse.failed() ){
			err += parse.getErrorMessage();
			err += newLine;
			success = false;
			continue;
		}
		auto addWorked = add( clip );
		if( !addWorked ){
			err += "MainComponent::fromXml() error adding AudioClip";
			err += newLine;
			success = false;
		}
	}
	return success ? Result::ok() : Result::fail( err );
}
