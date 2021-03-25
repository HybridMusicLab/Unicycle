// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#include "AudioFunctions.h"

using namespace aud;

// writeToFile
bool aud::writeToFile( const File& targetFile, const AudioBuffer<float>& audio, const AudioSettings& settings )
{
	std::unique_ptr<FileOutputStream> fos( new FileOutputStream( targetFile.withFileExtension( "wav" ) ) );

	// prepare to overwrite file
	if( fos->openedOk() ){
		fos->setPosition( 0 );
		fos->truncate();

		// create wav writer and write, writer owns fos
		std::unique_ptr<AudioFormatWriter> writer( WavAudioFormat().createWriterFor( fos.release(), settings.sampleRate, audio.getNumChannels(), settings.bitsPerSample, StringPairArray(), 0 ) );
		writer->writeFromAudioSampleBuffer( audio, 0, audio.getNumSamples() );
		return true;
	}
	return false;
}

// createOrGetBufferFor
std::map<File, AudioBuffer<float>>& getAudioCache()
{
	static std::map<File, AudioBuffer<float>> ret;
	return ret;
}

bool hasSharedAudioBuffer( const File& name )
{
	return getAudioCache().find( name ) != getAudioCache().end();
}

void addSharedAudioBuffer( const File& name, AudioBuffer<float>&& buffer )
{
	if( hasSharedAudioBuffer( name ) ){
		return;
	}
	getAudioCache()[ name ] = std::move( buffer );
}

void clearSharedAudioBuffers()
{
	getAudioCache().clear();
}

AudioBuffer<float> getSharedAudioBuffer( const File& name )
{
	// should call aud::createOrGetBufferFor()
	if( !hasSharedAudioBuffer( name ) ){
		jassertfalse;
		return AudioBuffer<float>();
	}
	// add to cache
	auto& buf = getAudioCache()[ name ];
	AudioBuffer<float> ret( buf.getArrayOfWritePointers(), buf.getNumChannels(), buf.getNumSamples() );
	return ret;
}

AudioBuffer<float> aud::createOrGetBufferFor( const File& file, AudioSettings& settings )
{
	// create reader
	std::unique_ptr<AudioFormatReader> rd( getAudioFormatManager()->createReaderFor( file ) );
	if( rd == nullptr ){
		return AudioBuffer<float>();
	}
	// read sample data into buffer
	auto len = rd->lengthInSamples;
	AudioBuffer<float> buf( rd->numChannels, len );
	rd->read( &buf, 0, ( int32 )len, 0, true, true );
	settings.sampleRate = rd->sampleRate;
	settings.bufferSize = rd->lengthInSamples;
	settings.bitsPerSample = rd->bitsPerSample;

	// check if sample already exists in shared cache.
	if( hasSharedAudioBuffer( file ) ){
		auto shared = getSharedAudioBuffer( file );
		if( compareBuffer( buf, shared )){
			return shared;
		}
	}
	// put in cache and return
	addSharedAudioBuffer( file, std::move( buf ) );
	return getSharedAudioBuffer( file );
}
