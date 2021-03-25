// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#include "MainComponent.h"

using namespace unc;
using namespace lnf;

// MainComponent
unc::MainComponent::MainComponent( MenuBarModel* menuBarModel )
{
	// audioClipList
	addAndMakeVisible( audioClipList );
	audioClipList.display( &audioClips );
	addAndMakeVisible( audioClipEditor );
	addAndMakeVisible( audioSettingsDisplay );
	
	// menuBar
	addAndMakeVisible( menuBar );
	menuBar.setModel( menuBarModel );
	menuBarModel->setApplicationCommandManagerToWatch( getApplicationCommandManager() );

	// commands
	getApplicationCommandManager()->registerAllCommandsForTarget( this );
	setWantsKeyboardFocus( true );

	// size not set by parent
    setSize( 800, 600 );

	// audio
	getAudioDeviceManager()->addAudioCallback( &soundPlayer );
}

unc::MainComponent::~MainComponent()
{
	stopPlaying();

	// audio
	getAudioDeviceManager()->removeAudioCallback( &soundPlayer );
}

// MainComponent - Component
void unc::MainComponent::paint( Graphics& g )
{
	// bg
	g.fillAll( greyBgFar );
}

void unc::MainComponent::resized()
{
	auto b = getLocalBounds();
	
	// menuBar
	menuBar.setBounds( b.removeFromTop( dims::h ) );
	b.reduce( dims::padM, dims::padM );

	// header, body, footer
	auto foot = b.removeFromBottom( dims::h );
	b.removeFromBottom( dims::padM );

	// audioClipList
	audioClipList.setBounds( b.removeFromTop( dims::hXXL ));
	b.removeFromTop( dims::padM );

	// audioSettingsDisplay
	audioSettingsDisplay.setBounds( foot.removeFromLeft( dims::wXXL ));
	foot.removeFromLeft( dims::padM );

	// audioClipEditor
	audioClipEditor.setBounds( b );
}

// MainComponent - MainInterface
void unc::MainComponent::playAudioBuffer( AudioBuffer<float>* buffer, bool shouldLoop )
{
	stopPlaying();
	playedBuffer.reset( buffer );
	playedSource.reset( new MemoryAudioSource( *playedBuffer, false, shouldLoop ) );
	soundPlayer.setSource( playedSource.get() );
}

void unc::MainComponent::stopPlaying()
{
	soundPlayer.setSource( nullptr );
	playedSource.reset();
	playedBuffer.reset();
}

// MainComponent - ApplicationCommandTarget
ApplicationCommandTarget* unc::MainComponent::getNextCommandTarget()
{
	return findFirstTargetParentComponent();
}

void unc::MainComponent::getAllCommands( Array<CommandID>& commands )
{
	commands.add( StandardApplicationCommandIDs::undo );
	commands.add( StandardApplicationCommandIDs::redo );
	commands.add( StandardApplicationCommandIDs::del );
	commands.add( CommandIDs::writeAllZones );
	commands.add( CommandIDs::writeZoneToSelected );
	commands.add( CommandIDs::removeZoneFromSelected );
}

void unc::MainComponent::getCommandInfo( CommandID commandID, ApplicationCommandInfo& result )
{
	switch( commandID )
	{
		case StandardApplicationCommandIDs::undo:{
			result.setInfo( "Undo", "Undo last command", CommandCategories::edit, 0 );
			result.addDefaultKeypress( 'z', ModifierKeys::commandModifier );
			result.setActive( getUndoManager()->canUndo() );
			break;
		}
		case StandardApplicationCommandIDs::redo:{
			result.setInfo( "Redo", "Redo last command", CommandCategories::edit, 0 );
			result.addDefaultKeypress( 'y', ModifierKeys::commandModifier );
			result.setActive( getUndoManager()->canRedo() );
			break;
		}
		case StandardApplicationCommandIDs::del: {
			result.setInfo( "Remove", "Remove selected", CommandCategories::edit, 0 );
			result.addDefaultKeypress( KeyPress::deleteKey, ModifierKeys::noModifiers );
			result.setActive( selectedRecently );
			break;
		}
		case CommandIDs::writeAllZones:{
			result.setInfo( "Write all zones", "Write all zones", CommandCategories::edit, 0 );
			result.setActive( getSelectedAudioClip() );
			break;
		}
		case CommandIDs::writeZoneToSelected: {
			result.setInfo( "Write to selected", "Write Zone to selected Clips", CommandCategories::edit, 0 );
			result.setActive( getSelectedPlayZone().isValid() );
			break;
		}
		case CommandIDs::removeZoneFromSelected: {
			result.setInfo( "Remove from selected", "Remove Zone from selected Clips", CommandCategories::edit, 0 );
			result.setActive( getSelectedPlayZone().isValid() );
			break;
		}
		default: break;
	}
}

bool unc::MainComponent::perform( const ApplicationCommandTarget::InvocationInfo& info )
{
	switch( info.commandID )
	{
		case StandardApplicationCommandIDs::undo: {
			getUndoManager()->undo();
			break;
		}
		case StandardApplicationCommandIDs::redo: {
			getUndoManager()->redo();
			break;
		}
		case StandardApplicationCommandIDs::del: {
			if( selectedRecently == &selectedAudioClip ){
				getUndoManager()->beginNewTransaction( "RemoveAudioClip" );
				auto sel = getSelectedAudioClips();
				for( auto* clip : sel ){
					auto ptr = audioClips.getPtr( audioClips.indexOf( clip ) );
					getUndoManager()->perform( new RemoveAudioClipCommand( &audioClips, ptr ) );
				}
			}
			else if( selectedRecently == &selectedPlayZone && selectedPlayZone.isValid() ){
				getUndoManager()->beginNewTransaction( "RemovePlayZone" );
				getUndoManager()->perform( new RemovePlayZoneCommand( selectedAudioClip, selectedPlayZone ) );
			}
			break;
		}
		case CommandIDs::writeAllZones: {
			auto* selected = getSelectedAudioClip();
			if( !selected ){
				break;
			}
			getUndoManager()->beginNewTransaction( "writeAllZones" );
			for( int clipIdx = 0; clipIdx < audioClips.size(); ++clipIdx ){
				auto* clip = audioClips.get( clipIdx );
				if( clip == selected ){
					continue;
				}
				getUndoManager()->perform( new ClearPlayZonesCommand( clip ) );
				for( int zoneIdx = 0; zoneIdx < selected->sizeZones(); ++zoneIdx ){
					getUndoManager()->perform( new AddPlayZoneCommand( clip, selected->getZone( zoneIdx ) ) );
				}
			}
			break;
		}
		case CommandIDs::writeZoneToSelected: {
			auto* selected = getSelectedAudioClip();
			if( !selected ){
				break;
			}
			auto zone = getSelectedPlayZone();
			if( !zone.isValid() ){
				break;
			}
			getUndoManager()->beginNewTransaction( "writeZoneToSelected" );
			auto clips = getSelectedAudioClips();
			for( auto* clip : clips ){
				if( clip == selected ){
					continue;
				}
				getUndoManager()->perform( new AddPlayZoneCommand( clip, zone ) );
			}
			break;
		}
		case CommandIDs::removeZoneFromSelected: {
			auto* selected = getSelectedAudioClip();
			if( !selected ){
				break;
			}
			auto zone = getSelectedPlayZone();
			if( !zone.isValid() ){
				break;
			}
			getUndoManager()->beginNewTransaction( "removeZoneFromSelected" );
			auto clips = getSelectedAudioClips();
			for( auto* clip : clips ){
				getUndoManager()->perform( new RemovePlayZoneCommand( clip, zone ) );
			}
			break;
		}
		default: return false;
	}
	return true;
}

void unc::MainComponent::selectAudioClip( AudioClip* audioClip )
{
	selectedAudioClip = audioClip;
	selectedPlayZone = AudioPlayZone();
	selectedRecently = &selectedAudioClip;
	audioClipEditor.display( audioClip );
}

void unc::MainComponent::selectAudioPlayZone( const AudioPlayZone& playZone )
{
	selectedPlayZone = playZone;
	selectedRecently = &selectedPlayZone;
}

Array<AudioClip*> unc::MainComponent::getSelectedAudioClips() const
{
	return audioClipList.getListSelection();
}

// MainComponent - persistence
Result unc::MainComponent::toXml( XmlElement* xml )const
{
	return audioClips.toXml( xml->createNewChildElement( "AudioClips" ) );
}

Result unc::MainComponent::fromXml( XmlElement* xml )
{
	auto* clipsXml = xml->getChildByName( "AudioClips" );
	if( !clipsXml ){
		return Result::fail( "MainComponent::fromXml() no clipsXml found" );
	}
	return audioClips.fromXml( clipsXml );
}
