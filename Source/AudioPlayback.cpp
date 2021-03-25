// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#include "AudioPlayback.h"

using namespace aud;

// FadeIn
FadeIn::FadeIn( int fadeLength_ ) :
	fadeLength( fadeLength_ )
{}

int FadeIn::process( AudioBuffer<float>& audioBuffer, int fadePos, double playRatio )
{
	// pos fetch and update
	auto numSamps = audioBuffer.getNumSamples();
	auto srcPos = fadePos;
	auto srcLen = playRatio * numSamps;
	auto srcEnd = srcPos + srcLen;

	// out of bounds
	if( srcEnd <= 0 ){
		audioBuffer.clear();
		return srcEnd;
	}
	if( srcPos > fadeLength ){
		return srcEnd;
	}
	// offset fade
	auto destStart = 0;
	auto destLen = numSamps;
	if( srcPos < 0 && srcPos + numSamps > 0 ){
		destStart = srcPos * -1;
		destLen -= destStart;
		audioBuffer.clear( 0, destStart );
	}
	// shorten fade
	if( srcPos <= fadeLength && srcPos + numSamps > fadeLength ){
		destLen = fadeLength - srcPos;
	}
	// zero fade
	if( fadeLength <= 0 ){
		return srcEnd;
	}
	// apply gain ramp
	double inc = playRatio / fadeLength;
	double a = alpha;
	auto len = destLen;

	// channels
	for( int ch = 0; ch < audioBuffer.getNumChannels(); ++ch ){
		auto* samp = audioBuffer.getWritePointer( ch, destStart );
		a = alpha;
		len = destLen;

		// samples
		while( --len >= 0 ){
			*samp++ *= a;
			a += inc;
		}
	}
	alpha = a;
	return srcEnd;
}

// FadeOut
FadeOut::FadeOut( int fadeLength_ ) :
	fadeLength( fadeLength_ )
{}

int FadeOut::process( AudioBuffer<float>& audioBuffer, int fadePos, double playRatio )
{
	// pos fetch and update
	auto numSamps = audioBuffer.getNumSamples();
	auto srcPos = fadePos;
	auto srcLen = playRatio * numSamps;
	auto srcEnd = srcPos + srcLen;

	// out of bounds
	if( srcEnd <= 0 ){
		return srcEnd;
	}
	if( srcPos >= fadeLength ){
		audioBuffer.clear();
		return srcEnd;
	}
	// offset fade
	auto destStart = 0;
	auto destLen = numSamps;
	if( srcPos < 0 && srcPos + numSamps > 0 ){
		destStart = srcPos * -1;
		destLen -= destStart;
	}
	// shorten fade
	if( srcPos <= fadeLength && srcPos + numSamps > fadeLength ){
		destLen = fadeLength - srcPos;
		audioBuffer.clear( destLen, numSamps - destLen );
	}
	// zero fade
	if( fadeLength == 0 ){
		return srcEnd;
	}
	// apply gain ramp
	double i = playRatio / fadeLength;
	double a = alpha;
	auto l = destLen;
	for( int ch = 0; ch < audioBuffer.getNumChannels(); ++ch ){
		auto* s = audioBuffer.getWritePointer( ch, destStart );
		a = alpha;
		l = destLen;
		while( --l >= 0 ){
			*s++ *= a;
			a -= i;
		}
	}
	alpha = a;
	return srcEnd;
}

// Resampler
Resampler::Resampler( const AudioBuffer<float>& sample_ ) :
	rangeLength( sample_.getNumSamples() ),
	sample( sample_ )
{}

// Resampler - process
int Resampler::process( AudioBuffer<float>& audioBuffer )
{
	audioBuffer.clear();

	// initialize play positions relative to range
	int destLen = audioBuffer.getNumSamples();
	int playPos = playPosition;
	const int playEnd = playPos + destLen;
	playPosition = playEnd;

	// we are before range bounds
	if( playEnd <= 0 ){
		return playPosition;
	}
	// offset destination if just before start
	int destPos = 0;
	if( playPos < 0 && playEnd > 0 ){
		destPos = playPos * -1;
		destLen = playEnd;
		playPos = 0;
	}
	// we are after clip bounds
	if( playPos >= rangeLength ){
		return playPosition;
	}
	// scale len with timestretch, offset pos to actual read range
	const int srcLen = sample.getNumSamples() - playPos;
	const int srcPos = playPos + rangeStart;

	// resample channels with some mono-stereo channel wrapping
	int numDestChans = jmin( ( int )resampler.size(), audioBuffer.getNumChannels() );
	int numRead = 0;
	for( int destCh = 0; destCh < numDestChans; ++destCh ){
		int sourceCh = destCh % sample.getNumChannels();
		auto* read = sample.getReadPointer( sourceCh, srcPos );
		auto* write = audioBuffer.getWritePointer( destCh, destPos );
		numRead = resampler[ destCh ].process( sampleRatio, read, write, destLen, srcLen, 0 );
	}
	// when timestretching, this might differ from playEnd
	playPosition = playPos + numRead;

	// fade in begin is at range 0, destPos is 0 except at fade bounds
	fadeIn.process( audioBuffer, playPos - destPos, sampleRatio );

	// fade out begins before range end
	auto fadeOutBegin = rangeLength - fadeOut.getLength();
	fadeOut.process( audioBuffer, playPos - destPos - fadeOutBegin, sampleRatio );
	return playPosition;
}

// Resampler - modify
void Resampler::setRange( int start, int length )
{
	if( length < 0 ){
        length = 0;
	}
	auto numSamps = sample.getNumSamples();
	auto end = start + length;
	start = jlimit( 0, numSamps, start );
	end = jlimit( 0, numSamps, end );
	rangeStart = start;
	rangeLength = end - start;
}

void Resampler::reset( int playPos )
{
	playPosition = playPos;
	for( auto& r : resampler ){
		r.reset();
	}
	fadeIn.reset();
	fadeOut.reset();
}

void Resampler::setRatio( double newRatio )
{
	sampleRatio = newRatio;
}

void Resampler::setFadeIn( int fadeLength )
{
	if( !isPositiveAndNotGreaterThan( fadeLength, rangeLength )){
		jassertfalse;
		return;
	}
	fadeIn.setLength( fadeLength );
}

void Resampler::setFadeOut( int fadeLength )
{
	if( !isPositiveAndNotGreaterThan( fadeLength, rangeLength ) ){
		jassertfalse;
		return;
	}
	fadeOut.setLength( fadeLength );
}
