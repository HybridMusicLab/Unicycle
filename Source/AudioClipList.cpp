// Copyright (c) 2019 Christoph Mann (christoph.mann@gmail.com)
#include "AudioClipList.h"

using namespace unc;
using namespace lnf;

// AudioClipListModel
unc::AudioClipListModel::AudioClipListModel( AudioClips* clips_, AudioClipList* clipList_ ) :
	clips( clips_ ),
	clipList( clipList_ )
{}

// AudioClipListModel - ListBoxModel
int unc::AudioClipListModel::getNumRows()
{
	return clips ? clips->size() : 0;
}

void unc::AudioClipListModel::paintListBoxItem( int rowNumber, Graphics& g, int width, int height, bool rowIsSelected )
{
	// bg
	g.fillAll( greyBgMid );
	g.setColour( rowIsSelected ? greyFgActive : greyFg );
	g.fillRect( 0, 0, width, dims::h ); // leaves room for padding

	// text
	if( auto* c = clips->get( rowNumber )){
		Rectangle<int> b( dims::padM, 0, width, height );
		g.setColour( textColour );

		// length
		g.drawFittedText( String( c->getTotalNumSamples() ), b.removeFromRight( dims::wM ), Justification::left, 1 );

		// bits
		g.drawFittedText( String( c->bitDepth ), b.removeFromRight( dims::wS ), Justification::left, 1 );

		// sr
		g.drawFittedText( String( c->sampleRate ), b.removeFromRight( dims::wM ), Justification::left, 1 );

		// chans
		g.drawFittedText( String( c->getNumChannels() ), b.removeFromRight( dims::wS ), Justification::left, 1 );

		// name
		g.drawFittedText( c->getName(), b, Justification::left, 1 );
	}
}

void unc::AudioClipListModel::selectedRowsChanged( int lastRowSelected )
{
	clipList->selectRow( lastRowSelected );
}

// AudioClipList
unc::AudioClipList::AudioClipList()
{
	// listBox
	addAndMakeVisible( listBox );
	listBox.setColour( ListBox::ColourIds::backgroundColourId, greyBgMid );
	listBox.setRowHeight( getRowHeight() );
	listBox.setMultipleSelectionEnabled( true );

	// outButton
	addAndMakeVisible( outButton );
	outButton.onClick = [&](){
		FileBrowserComponent browser{ FileBrowserComponent::canSelectDirectories | FileBrowserComponent::saveMode, File(), nullptr, nullptr };
		FileChooserDialogBox chooser{ "Target Folder", "Set folder to write output to", browser, false, greyBgMid };
		if( chooser.show() ){
			outPath = browser.getSelectedFile( 0 );
			outDisplay.setText( outPath.getFullPathName(), dontSendNotification );
		}
	};
	addAndMakeVisible( outDisplay );

	// renderButton
	addAndMakeVisible( renderButton );
	renderButton.onClick = [ & ](){
		if( !clips ){
			return;
		}
		if( !File::isAbsolutePath( outPath.getFullPathName())){
			AlertWindow::showMessageBox( AlertWindow::WarningIcon, "Error", "Choose valid outpath." );
			return;
		}
		for( int clipIdx = 0; clipIdx < clips->size(); ++clipIdx ){
			auto* clip = clips->get( clipIdx );
			for( int zoneIdx = 0; zoneIdx < clip->sizeZones(); ++zoneIdx ){
				std::unique_ptr<AudioBuffer<float>> buf( clip->writeAudio( zoneIdx ) );
				if( !buf ){
					return;
				}
				auto path = outPath.getFullPathName();
				path += File::getSeparatorString();
				path += clip->getName();
				path += "_";
				path += String( zoneIdx );
				path += "_";
				path += clip->getZone( zoneIdx ).name;
				path += ".wav";
				File file( path );
				if( file.create().wasOk()){
					aud::writeToFile( file, *buf, getCurrentAudioSettings() );
				}
			}
		}
		AlertWindow::showMessageBox( AlertWindow::InfoIcon, "Success", "Sample render complete." );
	};
	// commands
	getApplicationCommandManager()->registerAllCommandsForTarget( this );
	setWantsKeyboardFocus( true );
}

unc::AudioClipList::~AudioClipList()
{
	if( clips ){
		clips->removeChangeListener( this );
	}
}

// AudioClipList - display
void unc::AudioClipList::display( AudioClips* other )
{
	if( clips ){
		clips->removeChangeListener( this );
	}
	clips = other;
	if( clips ){
		clips->addChangeListener( this );
	}
	listModel.reset( new AudioClipListModel( clips, this ) );
	listBox.setModel( listModel.get());
}

// AudioClipList - Component
void unc::AudioClipList::paint( Graphics& g )
{
	// bg
	g.fillAll( greyBgMid );
}

void unc::AudioClipList::resized()
{
	auto b = getLocalBounds();
	auto lo = b.removeFromBottom( dims::h );
	b.removeFromBottom( dims::padM );

	// listBox
	listBox.setBounds( b.removeFromTop( listBox.getRowHeight() * listBox.getModel()->getNumRows()));
	listBox.repaint(); // force repaint

	// out
	outButton.setBounds( lo.removeFromLeft( dims::wM ));
	lo.removeFromLeft( dims::pad );

	// renderButton
	renderButton.setBounds( lo.removeFromRight( dims::wM ));
	lo.removeFromRight( dims::pad );

	// outDisplay
	outDisplay.setBounds( lo );
}

void unc::AudioClipList::mouseDown( const MouseEvent & e )
{
	// left
	if( !e.mods.isPopupMenu() ){
		listBox.selectRow( -1 );
	}
}

void unc::AudioClipList::selectRow( int row )
{
	if( auto* m = findParentComponentOfClass<MainInterface>() ) {
		m->selectAudioClip( clips->get( row ) );
	};
}

Array<AudioClip*> unc::AudioClipList::getListSelection() const
{
	Array<AudioClip*> ret;
	auto sels = listBox.getSelectedRows().getRanges();
	for( const auto& sel : sels ) {
		for( int i = sel.getStart(); i < sel.getEnd(); ++i ){
			ret.add( clips->get( i ) );
		}
	}
	return ret;
}

// AudioClipList - ApplicationCommandTarget
ApplicationCommandTarget* unc::AudioClipList::getNextCommandTarget()
{
	return findFirstTargetParentComponent();
}

void unc::AudioClipList::getAllCommands( Array<CommandID>& commands )
{
	commands.add( CommandIDs::sortClipsByName );
	commands.add( CommandIDs::sortClipsByLength );
}

void unc::AudioClipList::getCommandInfo( CommandID commandID, ApplicationCommandInfo& result )
{
	switch( commandID )
	{
		case CommandIDs::sortClipsByName: {
			result.setInfo( "Sort by name", "Sort Clips by name", CommandCategories::edit, 0 );
			result.setTicked( clips ? clips->getSortMethod() == AudioClips::SortMethod::Filename : false );
			break;
		}
		case CommandIDs::sortClipsByLength: {
			result.setInfo( "Sort by length", "Sort Clips by length", CommandCategories::edit, 0 );
			result.setTicked( clips ? clips->getSortMethod() == AudioClips::SortMethod::Length : false );
			break;
		}
		default: break;
	}
}

bool unc::AudioClipList::perform( const ApplicationCommandTarget::InvocationInfo& info )
{
	switch( info.commandID )
	{
		case CommandIDs::sortClipsByName: {
			if( clips->getSortMethod() == AudioClips::SortMethod::Filename ){
				clips->sort( AudioClips::SortMethod::Unsorted );
			}
			else{
				clips->sort( AudioClips::SortMethod::Filename );
			}
			getApplicationCommandManager()->commandStatusChanged();
			break;
		}
		case CommandIDs::sortClipsByLength: {
			if( clips->getSortMethod() == AudioClips::SortMethod::Length ){
				clips->sort( AudioClips::SortMethod::Unsorted );
			}
			else{
				clips->sort( AudioClips::SortMethod::Length );
			}
			getApplicationCommandManager()->commandStatusChanged();
			break;
		}
		default: return false;
	}
	return true;
}


// AudioClipList - ChangeBroadcaster
void unc::AudioClipList::changeListenerCallback( ChangeBroadcaster* source )
{
	if( source == clips ){
		listBox.updateContent();
	}
	// list size change
	resized();
}

// AudioClipList - FileDragAndDropTarget
bool unc::AudioClipList::isInterestedInFileDrag( const StringArray& files )
{
	return true;
}

void unc::AudioClipList::filesDropped( const StringArray& files, int x, int y )
{
	if( !clips ){
		jassertfalse;
		return;
	}
	getUndoManager()->beginNewTransaction( "AddAudioClip from files" );
	for( const auto& s : files ){
		
		// add dropped files as AudioClips
		File f( s );
		if( f.existsAsFile() && !clips->containsWith( f )){
			if( auto clip = createAudioClip( f ) ){
				getUndoManager()->perform( new AddAudioClipCommand( clips, clip ));
			}
		}
	}
	// list size change
	resized();
}
