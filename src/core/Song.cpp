/*
 * song.cpp - root of the model-tree
 *
 * Copyright (c) 2004-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 * 
 * This file is part of Linux MultiMedia Studio - http://lmms.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#include <QtCore/QCoreApplication>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

#include <math.h>

#include "Song.h"
#include "automation_track.h"
#include "automation_editor.h"
#include "automation_recorder.h"
#include "bb_editor.h"
#include "bb_track.h"
#include "bb_track_container.h"
#include "config_mgr.h"
#include "ControllerRackView.h"
#include "ControllerConnection.h"
#include "embed.h"
#include "EnvelopeAndLfoParameters.h"
#include "ExportProjectDialog.h"
#include "FxMixer.h"
#include "FxMixerView.h"
#include "ImportFilter.h"
#include "InstrumentTrack.h"
#include "MainWindow.h"
#include "mmp.h"
#include "note_play_handle.h"
#include "pattern.h"
#include "piano_roll.h"
#include "ProjectJournal.h"
#include "project_notes.h"
#include "ProjectRenderer.h"
#include "rename_dialog.h"
#include "song_editor.h"
#include "templates.h"
#include "text_float.h"
#include "timeline.h"


//tick_t midiTime::s_ticksPerTact = DefaultTicksPerTact;



Song::Song( void ) :
	trackContainer(),
	m_globalAutomationTrack( dynamic_cast<automationTrack *>(
				track::create( track::HiddenAutomationTrack,
								this ) ) ),
	//m_tempoModel( DefaultTempo, MinTempo, MaxTempo, this, tr( "Tempo" ) ),
	//m_timeSigModel( this ),
	//m_oldTicksPerTact( DefaultTicksPerTact ),
	//m_masterVolumeModel( 100, 0, 200, this, tr( "Master volume" ) ),
	//m_masterPitchModel( 0, -12, 12, this, tr( "Master pitch" ) ),
	m_fileName(),
	m_oldFileName(),
	m_modified( false ),
	m_loadingProject( false ),
	m_length( 0 )
{
	/*
	connect( &m_tempoModel, SIGNAL( dataChanged() ),
						this, SLOT( setTempo() ) );
	connect( &m_tempoModel, SIGNAL( dataUnchanged() ),
						this, SLOT( setTempo() ) );
	connect( &m_timeSigModel, SIGNAL( dataChanged() ),
					this, SLOT( setTimeSignature() ) );

	connect( engine::getMixer(), SIGNAL( sampleRateChanged() ), this,
						SLOT( updateFramesPerTick() ) );


	connect( &m_masterVolumeModel, SIGNAL( dataChanged() ),
			this, SLOT( masterVolumeChanged() ) );
	*/
/*	connect( &m_masterPitchModel, SIGNAL( dataChanged() ),
			this, SLOT( masterPitchChanged() ) );*/

	// TODO! WTF is this???
	qRegisterMetaType<note>( "note" );

}




Song::~Song()
{
	m_playing = false;
	delete m_globalAutomationTrack;
}



/*
void Song::masterVolumeChanged( void )
{
	engine::getMixer()->setMasterGain( m_masterVolumeModel.value() /
								100.0f );
}




void song::setTempo( void )
{
	const bpm_t tempo = (bpm_t) m_tempoModel.value();
	engine::getMixer()->lock();
	playHandleVector & phv = engine::getMixer()->playHandles();
	for( playHandleVector::iterator it = phv.begin(); it != phv.end();
									++it )
	{
		notePlayHandle * nph = dynamic_cast<notePlayHandle *>( *it );
		if( nph && !nph->released() )
		{
			nph->resize( tempo );
		}
	}
	engine::getMixer()->unlock();

	engine::updateFramesPerTick();

	emit tempoChanged( tempo );
}




void song::setTimeSignature( void )
{
	midiTime::setTicksPerTact( ticksPerTact() );
	emit timeSignatureChanged( m_oldTicksPerTact, ticksPerTact() );
	emit dataChanged();
	m_oldTicksPerTact = ticksPerTact();
}

*/


/* TODO! Should be based on the beatMap */
void Song::updateLength( void )
{
	m_length = 0;
	m_tracksMutex.lockForRead();
	for( trackList::const_iterator it = tracks().begin();
						it != tracks().end(); ++it )
	{
		const tact_t cur = ( *it )->length();
		if( cur > m_length )
		{
			m_length = cur;
		}
	}
	m_tracksMutex.unlock();

	emit lengthChanged( m_length );
}


/* TODO! Should be based on the beatMap and needs pos */
void Song::insertBar( void )
{
	/*
	m_tracksMutex.lockForRead();
	for( trackList::const_iterator it = tracks().begin();
					it != tracks().end(); ++it )
	{
		( *it )->insertTact( m_playPos[Mode_PlaySong] );
	}
	m_tracksMutex.unlock();
	*/
}



/* TODO! Should be based on the beatMap */
void Song::removeBar( void )
{
	/*
	m_tracksMutex.lockForRead();
	for( trackList::const_iterator it = tracks().begin();
					it != tracks().end(); ++it )
	{
		( *it )->removeTact( m_playPos[Mode_PlaySong] );
	}
	m_tracksMutex.unlock();
	*/
}




/* TODO! Why lock mixer??? */
void Song::addBBTrack( void )
{
	/*
	engine::getMixer()->lock();
	track * t = track::create( track::BBTrack, this );
	engine::getBBTrackContainer()->setCurrentBB(
						bbTrack::numOfBBTrack( t ) );
	engine::getMixer()->unlock();
	*/
}




/* TODO! Why lock mixer??? */
void Song::addSampleTrack( void )
{
	/*
	engine::getMixer()->lock();
	(void) track::create( track::SampleTrack, this );
	engine::getMixer()->unlock();
	*/
}



/* TODO! Why lock mixer??? */
void Song::addAutomationTrack( void )
{
	/*
	engine::getMixer()->lock();
	(void) track::create( track::AutomationTrack, this );
	engine::getMixer()->unlock();
	*/
}



//automationPattern * song::tempoAutomationPattern( void )
//{
//	return automationPattern::globalAutomationPattern( &m_tempoModel );
//}



/* TODO! Implement correctly !! */
void Song::clearProject( void )
{
	/*
	engine::getProjectJournal()->setJournalling( false );

	if( m_playing )
	{
		stop();
	}

	engine::getMixer()->lock();
	if( engine::getBBEditor() )
	{
		engine::getBBEditor()->clearAllTracks();
	}
	if( engine::getSongEditor() )
	{
		engine::getSongEditor()->clearAllTracks();
	}
	if( engine::getFxMixerView() )
	{
		engine::getFxMixerView()->clear();
	}
	QCoreApplication::sendPostedEvents();
	engine::getBBTrackContainer()->clearAllTracks();
	clearAllTracks();

	engine::getFxMixer()->clear();

	if( engine::getAutomationEditor() )
	{
		engine::getAutomationEditor()->setCurrentPattern( NULL );
	}
	automationPattern::globalAutomationPattern( &m_tempoModel )->clear();
	automationPattern::globalAutomationPattern( &m_masterVolumeModel )->
									clear();
	automationPattern::globalAutomationPattern( &m_masterPitchModel )->
									clear();

	engine::getMixer()->unlock();

	if( engine::getProjectNotes() )
	{
		engine::getProjectNotes()->clear();
	}

	// Move to function
	while( !m_controllers.empty() )
	{
		delete m_controllers.last();
	}

	emit dataChanged();

	engine::getProjectJournal()->clearJournal();

	engine::getProjectJournal()->setJournalling( true );
	*/
}





// create new file
void Song::createNewProject( void )
{
	QString default_template = configManager::inst()->userProjectsDir()
						+ "templates/default.mpt";
	if( QFile::exists( default_template ) )
	{
		createNewProjectFromTemplate( default_template );
		return;
	}

	default_template = configManager::inst()->factoryProjectsDir()
						+ "templates/default.mpt";
	if( QFile::exists( default_template ) )
	{
		createNewProjectFromTemplate( default_template );
		return;
	}

	m_loadingProject = true;

	clearProject();

	engine::projectJournal()->setJournalling( false );

	m_fileName = m_oldFileName = "";

	track * t;
	t = track::create( track::InstrumentTrack, this );
	dynamic_cast<InstrumentTrack * >( t )->loadInstrument(
					"tripleoscillator" );
	t = track::create( track::InstrumentTrack,
						engine::getBBTrackContainer() );
	dynamic_cast<InstrumentTrack * >( t )->loadInstrument(
						"tripleoscillator" );
	track::create( track::SampleTrack, this );
	track::create( track::BBTrack, this );
	track::create( track::AutomationTrack, this );

	/*
	m_tempoModel.setInitValue( DefaultTempo );
	m_timeSigModel.reset();
	m_masterVolumeModel.setInitValue( 100 );
	m_masterPitchModel.setInitValue( 0 );
	*/

	QCoreApplication::instance()->processEvents();

	m_loadingProject = false;

	engine::getBBTrackContainer()->updateAfterTrackAdd();

	engine::projectJournal()->setJournalling( true );

	QCoreApplication::sendPostedEvents();

	m_modified = false;

	if( engine::mainWindow() )
	{
		engine::mainWindow()->resetWindowTitle();
	}
}




void Song::createNewProjectFromTemplate( const QString & _template )
{
	loadProject( _template );
	// clear file-name so that user doesn't overwrite template when
	// saving...
	m_fileName = m_oldFileName = "";
}




// load given song
void Song::loadProject( const QString & _file_name )
{
	m_loadingProject = true;

	clearProject();

	engine::projectJournal()->setJournalling( false );

	m_fileName = _file_name;
	m_oldFileName = _file_name;

	multimediaProject mmp( m_fileName );
	// if file could not be opened, head-node is null and we create
	// new project
	if( mmp.head().isNull() )
	{
		createNewProject();
		return;
	}

	if( engine::mainWindow() )
	{
		engine::mainWindow()->resetWindowTitle();
	}

	engine::getMixer()->lock();

	// get the header information from the DOM
	/*
	m_tempoModel.loadSettings( mmp.head(), "bpm" );
	m_timeSigModel.loadSettings( mmp.head(), "timesig" );
	m_masterVolumeModel.loadSettings( mmp.head(), "mastervol" );
	m_masterPitchModel.loadSettings( mmp.head(), "masterpitch" );

	if( m_playPos[Mode_PlaySong].m_timeLine )
	{
		// reset loop-point-state
		m_playPos[Mode_PlaySong].m_timeLine->toggleLoopPoints( 0 );
	}
	*/

	if( !mmp.content().firstChildElement( "track" ).isNull() )
	{
		m_globalAutomationTrack->restoreState( mmp.content().
						firstChildElement( "track" ) );
	}
	QDomNode node = mmp.content().firstChild();
	while( !node.isNull() )
	{
		if( node.isElement() )
		{
			if( node.nodeName() == "trackcontainer" )
			{
				( (JournallingObject *)( this ) )->
					restoreState( node.toElement() );
			}
			else if( node.nodeName() == "controllers" )
			{
				restoreControllerStates( node.toElement() );
			}
			else if( node.nodeName() ==
					engine::fxMixer()->nodeName() )
			{
				engine::fxMixer()->restoreState(
							node.toElement() );
			}
			else if( engine::hasGUI() )
			{
				if( node.nodeName() ==
					engine::getControllerRackView()->nodeName() )
				{
					engine::getControllerRackView()->
						restoreState( node.toElement() );
				}
				else if( node.nodeName() ==
					engine::getPianoRoll()->nodeName() )
				{
					engine::getPianoRoll()->restoreState(
							node.toElement() );
				}
				else if( node.nodeName() ==
					engine::getAutomationEditor()->
								nodeName() )
				{
					engine::getAutomationEditor()->
						restoreState( node.toElement() );
				}
				else if( node.nodeName() ==
						engine::getProjectNotes()->
								nodeName() )
				{
					 engine::getProjectNotes()->
			SerializingObject::restoreState( node.toElement() );
				}
				/* TODO! What was this doing??
				else if( node.nodeName() ==
						m_playPos[Mode_PlaySong].
							m_timeLine->nodeName() )
				{
					m_playPos[Mode_PlaySong].
						m_timeLine->restoreState(
							node.toElement() );
				}
				*/
			}
		}
		node = node.nextSibling();
	}

	// quirk for fixing projects with broken positions of TCOs inside
	// BB-tracks
	engine::getBBTrackContainer()->fixIncorrectPositions();

	// Connect Controller links to their controllers 
	// now that everything is loaded
	ControllerConnection::finalizeConnections();

	// resolve all IDs so that autoModels are automated
	automationPattern::resolveAllIDs();


	engine::getMixer()->unlock();

	configManager::inst()->addRecentlyOpenedProject( _file_name );

	engine::projectJournal()->setJournalling( true );

	m_loadingProject = false;
	m_modified = false;

	if( engine::mainWindow() )
	{
		engine::mainWindow()->resetWindowTitle();
	}
	if( engine::getSongEditor() )
	{
		engine::getSongEditor()->scrolled( 0 );
	}
}




// save current song
bool Song::saveProject( void )
{
	multimediaProject mmp( multimediaProject::SongProject );

	/*
	m_tempoModel.saveSettings( mmp, mmp.head(), "bpm" );
	m_timeSigModel.saveSettings( mmp, mmp.head(), "timesig" );
	m_masterVolumeModel.saveSettings( mmp, mmp.head(), "mastervol" );
	m_masterPitchModel.saveSettings( mmp, mmp.head(), "masterpitch" );
	*/

	saveState( mmp, mmp.content() );

	m_globalAutomationTrack->saveState( mmp, mmp.content() );
	engine::fxMixer()->saveState( mmp, mmp.content() );
	if( engine::hasGUI() )
	{
		engine::getControllerRackView()->saveState( mmp, mmp.content() );
		engine::getPianoRoll()->saveState( mmp, mmp.content() );
		engine::getAutomationEditor()->saveState( mmp, mmp.content() );
		engine::getProjectNotes()->
			SerializingObject::saveState( mmp, mmp.content() );
		/*
		m_playPos[Mode_PlaySong].m_timeLine->saveState(
							mmp, mmp.content() );
		*/
	}

	saveControllerStates( mmp, mmp.content() );

	m_fileName = mmp.nameWithExtension( m_fileName );
	if( mmp.writeFile( m_fileName ) == true && engine::hasGUI() )
	{
		textFloat::displayMessage( tr( "Project saved" ),
					tr( "The project %1 is now saved."
							).arg( m_fileName ),
				embed::getIconPixmap( "project_save", 24, 24 ),
									2000 );
		configManager::inst()->addRecentlyOpenedProject( m_fileName );
		m_modified = false;
		engine::mainWindow()->resetWindowTitle();
	}
	else if( engine::hasGUI() )
	{
		textFloat::displayMessage( tr( "Project NOT saved." ),
				tr( "The project %1 was not saved!" ).arg(
							m_fileName ),
				embed::getIconPixmap( "error" ), 4000 );
		return false;
	}

	return true;
}




// save current song in given filename
bool Song::saveProjectAs( const QString & _file_name )
{
	QString o = m_oldFileName;
	m_oldFileName = m_fileName;
	m_fileName = _file_name;
	if( saveProject() == false )
	{
		m_fileName = m_oldFileName;
		m_oldFileName = o;
		return false;
	}
	m_oldFileName = m_fileName;
	return true;
}




void Song::importProject( void )
{
	QFileDialog ofd( NULL, tr( "Import file" ),
			configManager::inst()->userProjectsDir(),
			tr("MIDI sequences (*.mid *.rif);;FL Studio projects (*.flp"
#ifdef LMMS_HAVE_ZIP	
			 " *.zip)") );
#else 
			 ")") );
#endif

	ofd.setFileMode( QFileDialog::ExistingFiles );
	if( ofd.exec () == QDialog::Accepted && !ofd.selectedFiles().isEmpty() )
	{
		ImportFilter::import( ofd.selectedFiles()[0], this );
	}
}




void Song::saveControllerStates( QDomDocument & _doc, QDomElement & _this )
{
	// save settings of controllers
	QDomElement controllersNode =_doc.createElement( "controllers" );
	_this.appendChild( controllersNode );
	for( int i = 0; i < m_controllers.size(); ++i )
	{
		m_controllers[i]->saveState( _doc, controllersNode );
	}
}




void Song::restoreControllerStates( const QDomElement & _this )
{
	QDomNode node = _this.firstChild();
	while( !node.isNull() )
	{
		addController( Controller::create( node.toElement(), this ) );

		node = node.nextSibling();
	}
}




void Song::exportProject( void )
{
	if( isEmpty() )
	{
		QMessageBox::information( engine::mainWindow(),
				tr( "Empty project" ),
				tr( "This project is empty so exporting makes "
					"no sense. Please put some items into "
					"Song Editor first!" ) );
		return;
	}

	QFileDialog efd( engine::mainWindow() );
	efd.setFileMode( QFileDialog::AnyFile );
	efd.setAcceptMode( QFileDialog::AcceptSave );
	int idx = 0;
	QStringList types;
	while( __fileEncodeDevices[idx].m_fileFormat !=
					ProjectRenderer::NumFileFormats )
	{
		types << tr( __fileEncodeDevices[idx].m_description );
		++idx;
	}
	efd.setFilters( types );

	QString base_filename;
	if( m_fileName != "" )
	{
		efd.setDirectory( QFileInfo( m_fileName ).absolutePath() );
		base_filename = QFileInfo( m_fileName ).completeBaseName();
	}
	else
	{
		efd.setDirectory( configManager::inst()->userProjectsDir() );
		base_filename = tr( "untitled" );
	}
	efd.selectFile( base_filename + __fileEncodeDevices[0].m_extension );
	efd.setWindowTitle( tr( "Select file for project-export..." ) );

	if( efd.exec() == QDialog::Accepted &&
		!efd.selectedFiles().isEmpty() && efd.selectedFiles()[0] != "" )
	{
		const QString export_file_name = efd.selectedFiles()[0];
		ExportProjectDialog epd( export_file_name,
						engine::mainWindow() );
		epd.exec();
	}
}




void Song::setModified( void )
{
	if( !m_loadingProject )
	{
		m_modified = true;
		if( engine::mainWindow() &&
			QThread::currentThread() ==
					engine::mainWindow()->thread() )
		{
			engine::mainWindow()->resetWindowTitle();
		}
	}
}




void Song::addController( Controller * _c )
{
	if( _c != NULL && !m_controllers.contains( _c ) ) 
	{
		m_controllers.append( _c );
		emit dataChanged();
	}
}




void Song::removeController( Controller * _controller )
{
	int index = m_controllers.indexOf( _controller );
	if( index != -1 )
	{
		m_controllers.remove( index );

		if( engine::getSong() )
		{
			engine::getSong()->setModified();
		}
		emit dataChanged();
	}
}



#include "moc_Song.cxx"


