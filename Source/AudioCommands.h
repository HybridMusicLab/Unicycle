// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#pragma once

#include "MainHeaders.h"

#include "AudioClip.h"

namespace unc
{
	class AddAudioClipCommand : public UndoableAction
	{
	public:
		AddAudioClipCommand( AudioClips* clips_, const AudioClip::Ptr& clip_ ) :
			clips( clips_ ),
			clip( clip_ )
		{}

		bool perform() override
		{
			return clips->add( clip );
		}

		bool undo() override
		{
			return clips->remove( clips->indexOf( clip.get()));
		}

	private:
		AudioClips* clips;
		AudioClip::Ptr clip;
	};

	class RemoveAudioClipCommand : public UndoableAction
	{
	public:
		RemoveAudioClipCommand( AudioClips* clips_, const AudioClip::Ptr& clip_ ) :
			clips( clips_ ),
			clip( clip_ )
		{}

		bool perform() override
		{
			return clips->remove( clips->indexOf( clip.get() ) );
		}

		bool undo() override
		{
			return clips->add( clip );
		}

	private:
		AudioClips* clips;
		AudioClip::Ptr clip;
	};

	class AddPlayZoneCommand : public UndoableAction
	{
	public:
		AddPlayZoneCommand( AudioClip* clip_, const AudioPlayZone& playZone_ ) :
			clip( clip_ ),
			playZone( playZone_ )
		{}

		bool perform() override
		{
			return clip->addZone( playZone );
		}

		bool undo() override
		{
			return clip->removeZone( clip->indexOfZone( playZone ));
		}

	private:
		AudioClip* clip;
		AudioPlayZone playZone;
	};

	class SetPlayZoneCommand : public UndoableAction
	{
	public:
		SetPlayZoneCommand( AudioClip* clip_, const AudioPlayZone& oldZone_, const AudioPlayZone& newZone_ ) :
			clip( clip_ ),
			oldZone( oldZone_ ),
			newZone( newZone_ )
		{}

		bool perform() override
		{
			return clip->setZone( clip->indexOfZone( oldZone ), newZone );
		}

		bool undo() override
		{
			return clip->setZone( clip->indexOfZone( newZone ), oldZone );
		}

	private:
		AudioClip* clip;
		AudioPlayZone oldZone;
		AudioPlayZone newZone;
	};

	class RemovePlayZoneCommand : public UndoableAction
	{
	public:
		RemovePlayZoneCommand( AudioClip* clip_, const AudioPlayZone& playZone_ ) :
			clip( clip_ ),
			playZone( playZone_ )
		{}

		bool perform() override
		{
			return clip->removeZone( clip->indexOfZone( playZone ));
		}

		bool undo() override
		{
			return clip->addZone( playZone );
		}

	private:
		AudioClip* clip;
		AudioPlayZone playZone;
	};

	class ClearPlayZonesCommand : public UndoableAction
	{
	public:
		ClearPlayZonesCommand( AudioClip* clip_ ) :
			clip( clip_ )
		{
			for( int i = clip->sizeZones(); --i >= 0; ){
				auto* cmd = new RemovePlayZoneCommand( clip, clip->getZone( i ) );
				removes.add( cmd );
			}
		}

		bool perform() override
		{
			bool ret = true;
			for( auto* rem : removes ){
				ret &= rem->perform();
			}
			return ret;
		}

		bool undo() override
		{
			bool ret = true;
			for( auto* rem : removes ){
				ret &= rem->undo();
			}
			return ret;
		}

	private:
		AudioClip* clip;
		OwnedArray<RemovePlayZoneCommand> removes;
	};
}
