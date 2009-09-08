/*
 * Pattern.cpp - implementation of class pattern which holds notes
 *
 * Copyright (c) 2004-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 * Copyright (c) 2005-2007 Danny McRae <khjklujn/at/yahoo.com>
 * Copyright (c) 2009 Paul Giblock <pgib/at/users.sourceforge.net>
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

#include <QtXml/QDomElement>
#include <QtCore/QTimer>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>
#include <QtGui/QProgressBar>
#include <QtGui/QPushButton>
#include <QtAlgorithms>

#include "AudioSampleRecorder.h"
#include "InstrumentTrack.h"
#include "Pattern.h"
#include "Song.h"
#include "templates.h"
#include "gui_templates.h"
#include "embed.h"
#include "engine.h"
#include "TrackContainer.h"
#include "rename_dialog.h"
#include "sample_buffer.h"
#include "tooltip.h"
#include "bb_track_container.h"
#include "string_pair_drag.h"



Pattern::Pattern( InstrumentTrack * _instrument_track ) :
	TrackSegment( _instrument_track ),
	m_instrumentTrack( _instrument_track ),
	m_patternType( BeatPattern ),
	m_steps( midiTime::stepsPerTact() ),
	m_frozenPattern( NULL ),
	m_freezing( false ),
	m_freezeAborted( false )
{
	setName( _instrument_track->name() );
	init();
}




Pattern::Pattern( const Pattern & _pat_to_copy ) :
	TrackSegment( _pat_to_copy.m_instrumentTrack ),
	m_instrumentTrack( _pat_to_copy.m_instrumentTrack ),
	m_patternType( _pat_to_copy.m_patternType ),
	m_steps( _pat_to_copy.m_steps ),
	m_frozenPattern( NULL ),
	m_freezeAborted( false )
{
	for( NoteVector::ConstIterator it = _pat_to_copy.m_notes.begin();
					it != _pat_to_copy.m_notes.end(); ++it )
	{
		m_notes.push_back( new note( **it ) );
	}

	init();
}


Pattern::~Pattern()
{
	for( NoteVector::Iterator it = m_notes.begin();
						it != m_notes.end(); ++it )
	{
		delete *it;
	}

	m_notes.clear();

	if( m_frozenPattern )
	{
		sharedObject::unref( m_frozenPattern );
	}
}




void Pattern::init()
{
	saveJournallingState( false );

	ensureBeatNotes();

	changeLength( length() );
	restoreJournallingState();
}




midiTime Pattern::length() const
{
	if( m_patternType == BeatPattern )
	{
		return beatPatternLength();
	}

	tick_t max_length = midiTime::ticksPerTact();

	for( NoteVector::ConstIterator it = m_notes.begin();
						it != m_notes.end(); ++it )
	{
		if( ( *it )->length() > 0 )
		{
			max_length = qMax<tick_t>( max_length,
							( *it )->endPos() );
		}
	}
	return midiTime( max_length ).nextFullTact() *
						midiTime::ticksPerTact();
}




midiTime Pattern::beatPatternLength() const
{
	tick_t max_length = midiTime::ticksPerTact();

	for( NoteVector::ConstIterator it = m_notes.begin();
						it != m_notes.end(); ++it )
	{
		if( ( *it )->length() < 0 )
		{
			max_length = qMax<tick_t>( max_length,
				( *it )->pos() +
					midiTime::ticksPerTact() /
						midiTime::stepsPerTact() );
		}
	}

	if( m_steps != midiTime::stepsPerTact() )
	{
		max_length = m_steps * midiTime::ticksPerTact() /
						midiTime::stepsPerTact() ;
	}

	return midiTime( max_length ).nextFullTact() * midiTime::ticksPerTact();
}




note * Pattern::addNote( const note & _new_note, const bool _quant_pos )
{
	note * new_note = new note( _new_note );
	/* TODO{TNG} Use MetricMap to Quantize note
	if( _quant_pos && engine::getPianoRoll() )
	{
		new_note->quantizePos( engine::getPianoRoll()->quantization() );
	}
	*/

	engine::getMixer()->lock();
	if( m_notes.size() == 0 || m_notes.back()->pos() <= new_note->pos() )
	{
		m_notes.push_back( new_note );
	}
	else
	{
		// simple algorithm for inserting the note between two
		// notes with smaller and greater position
		// maybe it could be optimized by starting in the middle and
		// going forward or backward but note-inserting isn't that
		// time-critical since it is usually not done while playing...
		long new_note_abs_time = new_note->pos();
		NoteVector::Iterator it = m_notes.begin();

		while( it != m_notes.end() &&
					( *it )->pos() < new_note_abs_time )
		{
			++it;
		}

		m_notes.insert( it, new_note );
	}
	engine::getMixer()->unlock();

	checkType();
	changeLength( length() );

	emit dataChanged();

	updateBBTrack();

	return new_note;
}




void Pattern::removeNote( const note * _note_to_del )
{
	engine::getMixer()->lock();
	NoteVector::Iterator it = m_notes.begin();
	while( it != m_notes.end() )
	{
		if( *it == _note_to_del )
		{
			delete *it;
			m_notes.erase( it );
			break;
		}
		++it;
	}
	engine::getMixer()->unlock();

	checkType();
	changeLength( length() );

	emit dataChanged();

	updateBBTrack();
}




note * Pattern::rearrangeNote( const note * _note_to_proc,
							const bool _quant_pos )
{
	// just rearrange the position of the note by removing it and adding
	// a copy of it -> addNote inserts it at the correct position
	note copy_of_note( *_note_to_proc );
	removeNote( _note_to_proc );

	return addNote( copy_of_note, _quant_pos );
}



void Pattern::rearrangeAllNotes()
{
	// sort notes by start time
	qSort(m_notes.begin(), m_notes.end(), note::lessThan );
}



void Pattern::clearNotes()
{
	engine::getMixer()->lock();
	for( NoteVector::Iterator it = m_notes.begin(); it != m_notes.end();
									++it )
	{
		delete *it;
	}
	m_notes.clear();
	engine::getMixer()->unlock();

	checkType();
	emit dataChanged();
}




void Pattern::setStep( int _step, bool _enabled )
{
	for( NoteVector::Iterator it = m_notes.begin(); it != m_notes.end();
									++it )
	{
		if( ( *it )->pos() == _step*DefaultTicksPerTact/16 &&
						( *it )->length() <= 0 )
		{
			( *it )->setLength( _enabled ?
						-DefaultTicksPerTact : 0 );
		}
	}
}




void Pattern::setType( PatternTypes _new_pattern_type )
{
	if( _new_pattern_type == BeatPattern ||
				_new_pattern_type == MelodyPattern )
	{
		m_patternType = _new_pattern_type;
	}
}




void Pattern::checkType()
{
	NoteVector::Iterator it = m_notes.begin();
	while( it != m_notes.end() )
	{
		if( ( *it )->length() > 0 )
		{
			setType( Pattern::MelodyPattern );
			return;
		}
		++it;
	}
	setType( Pattern::BeatPattern );
}




void Pattern::saveSettings( QDomDocument & _doc, QDomElement & _this )
{
	_this.setAttribute( "type", m_patternType );
	_this.setAttribute( "name", name() );
	// as the target of copied/dragged pattern is always an existing
	// pattern, we must not store actual position, instead we store -1
	// which tells loadSettings() not to mess around with position
	if( _this.parentNode().nodeName() == "clipboard" ||
			_this.parentNode().nodeName() == "dnddata" )
	{
		_this.setAttribute( "pos", -1 );
	}
	else
	{
		_this.setAttribute( "pos", startPosition() );
	}
	_this.setAttribute( "len", length() );
	_this.setAttribute( "muted", isMuted() );
	_this.setAttribute( "steps", m_steps );
	_this.setAttribute( "frozen", m_frozenPattern != NULL );

	// now save settings of all notes
	for( NoteVector::Iterator it = m_notes.begin();
						it != m_notes.end(); ++it )
	{
		if( ( *it )->length() )
		{
			( *it )->saveState( _doc, _this );
		}
	}
}




void Pattern::loadSettings( const QDomElement & _this )
{
	unfreeze();

	m_patternType = static_cast<PatternTypes>( _this.attribute( "type"
								).toInt() );
	setName( _this.attribute( "name" ) );
	if( _this.attribute( "pos" ).toInt() >= 0 )
	{
		movePosition( _this.attribute( "pos" ).toInt() );
	}
	changeLength( midiTime( _this.attribute( "len" ).toInt() ) );
	if( _this.attribute( "muted" ).toInt() != isMuted() )
	{
		toggleMute();
	}

	clearNotes();

	QDomNode node = _this.firstChild();
	while( !node.isNull() )
	{
		if( node.isElement() &&
			!node.toElement().attribute( "metadata" ).toInt() )
		{
			note * n = new note;
			n->restoreState( node.toElement() );
			m_notes.push_back( n );
		}
		node = node.nextSibling();
        }

	m_steps = _this.attribute( "steps" ).toInt();
	if( m_steps == 0 )
	{
		m_steps = midiTime::stepsPerTact();
	}

	ensureBeatNotes();
	checkType();
/*	if( _this.attribute( "frozen" ).toInt() )
	{
		freeze();
	}*/

	emit dataChanged();

	updateBBTrack();
}




void Pattern::clear()
{
	clearNotes();
	ensureBeatNotes();
}




void Pattern::freeze()
{
	/* TODO{TNG} Look at sequencer or something instead
	if( engine::song()->isPlaying() )
	{
		QMessageBox::information( 0, tr( "Cannot freeze pattern" ),
						tr( "The pattern currently "
							"cannot be freezed "
							"because you're in "
							"play-mode. Please "
							"stop and try again!" ),
						QMessageBox::Ok );
		return;
	}
	*/

	// already frozen?
	if( m_frozenPattern != NULL )
	{
		// then unfreeze, before freezing it again
		unfreeze();
	}

	new PatternFreezeThread( this );

}




void Pattern::unfreeze()
{
	if( m_frozenPattern != NULL )
	{
		sharedObject::unref( m_frozenPattern );
		m_frozenPattern = NULL;
		emit dataChanged();
	}
}




void Pattern::abortFreeze()
{
	m_freezeAborted = true;
}




void Pattern::addSteps( int _n )
{
	m_steps += _n;
	ensureBeatNotes();
	emit dataChanged();
}




void Pattern::removeSteps( int _n )
{
	if( _n < m_steps )
	{
		for( int i = m_steps - _n; i < m_steps; ++i )
		{
			for( NoteVector::Iterator it = m_notes.begin();
						it != m_notes.end(); ++it )
			{
				if( ( *it )->pos() ==
					i * midiTime::ticksPerTact() /
						midiTime::stepsPerTact() &&
							( *it )->length() <= 0 )
				{
					removeNote( *it );
					break;
				}
			}
		}
		m_steps -= _n;
		emit dataChanged();
	}
}






void Pattern::ensureBeatNotes()
{
	// make sure, that all step-note exist
	for( int i = 0; i < m_steps; ++i )
	{
		bool found = false;
		for( NoteVector::Iterator it = m_notes.begin();
						it != m_notes.end(); ++it )
		{
			if( ( *it )->pos() ==
				i * midiTime::ticksPerTact() /
					midiTime::stepsPerTact() &&
							( *it )->length() <= 0 )
			{
				found = true;
				break;
			}
		}
		if( found == false )
		{
			addNote( note( midiTime( 0 ), midiTime( i *
				midiTime::ticksPerTact() /
					midiTime::stepsPerTact() ) ), false );
		}
	}
}




void Pattern::updateBBTrack()
{
	if( getTrack()->trackContainer() == engine::getBBTrackContainer() )
	{
		engine::getBBTrackContainer()->updateBBTrack( this );
	}
}




bool Pattern::empty()
{
	for( NoteVector::ConstIterator it = m_notes.begin();
						it != m_notes.end(); ++it )
	{
		if( ( *it )->length() != 0 )
		{
			return false;
		}
	}
	return true;
}




void Pattern::changeTimeSignature()
{
	midiTime last_pos = midiTime::ticksPerTact();
	for( NoteVector::ConstIterator cit = m_notes.begin();
						cit != m_notes.end(); ++cit )
	{
		if( ( *cit )->length() < 0 && ( *cit )->pos() > last_pos )
		{
			last_pos = ( *cit )->pos()+midiTime::ticksPerTact() /
						midiTime::stepsPerTact();
		}
	}
	last_pos = last_pos.nextFullTact() * midiTime::ticksPerTact();
	for( NoteVector::Iterator it = m_notes.begin(); it != m_notes.end(); )
	{
		if( ( *it )->length() == 0 && ( *it )->pos() >= last_pos )
		{
			delete *it;
			it = m_notes.erase( it );
			--m_steps;
		}
		else
		{
			++it;
		}
	}
	m_steps = qMax<tick_t>(
		qMax<tick_t>( m_steps, midiTime::stepsPerTact() ),
				last_pos.getTact() * midiTime::stepsPerTact() );
	ensureBeatNotes();
}




PatternFreezeStatusDialog::PatternFreezeStatusDialog( QThread * _thread ) :
	QDialog(),
	m_freezeThread( _thread ),
	m_progress( 0 )
{
	setWindowTitle( tr( "Freezing pattern..." ) );
	setModal( true );

	m_progressBar = new QProgressBar( this );
	m_progressBar->setGeometry( 10, 10, 200, 24 );
	m_progressBar->setMaximum( 100 );
	m_progressBar->setTextVisible( false );
	m_progressBar->show();
	m_cancelBtn = new QPushButton( embed::getIconPixmap( "cancel" ),
							tr( "Cancel" ), this );
	m_cancelBtn->setGeometry( 50, 38, 120, 28 );
	m_cancelBtn->show();
	connect( m_cancelBtn, SIGNAL( clicked() ), this,
						SLOT( cancelBtnClicked() ) );
	show();

	QTimer * update_timer = new QTimer( this );
	connect( update_timer, SIGNAL( timeout() ),
					this, SLOT( updateProgress() ) );
	update_timer->start( 100 );

	setAttribute( Qt::WA_DeleteOnClose, true );
	connect( this, SIGNAL( aborted() ), this, SLOT( reject() ) );

}




PatternFreezeStatusDialog::~PatternFreezeStatusDialog()
{
	m_freezeThread->wait();
	delete m_freezeThread;
}





void PatternFreezeStatusDialog::setProgress( int _p )
{
	m_progress = _p;
}




void PatternFreezeStatusDialog::closeEvent( QCloseEvent * _ce )
{
	_ce->ignore();
	cancelBtnClicked();
}




void PatternFreezeStatusDialog::cancelBtnClicked()
{
	emit( aborted() );
	done( -1 );
}




void PatternFreezeStatusDialog::updateProgress()
{
	if( m_progress < 0 )
	{
		done( 0 );
	}
	else
	{
		m_progressBar->setValue( m_progress );
	}
}








PatternFreezeThread::PatternFreezeThread( Pattern * _pattern ) :
	m_pattern( _pattern )
{
	// create status-dialog
	m_statusDlg = new PatternFreezeStatusDialog( this );
	QObject::connect( m_statusDlg, SIGNAL( aborted() ),
					m_pattern, SLOT( abortFreeze() ) );

	start();
}




PatternFreezeThread::~PatternFreezeThread()
{
	m_pattern->dataChanged();
}




void PatternFreezeThread::run()
{
	/* TODO{TNG} Reimplement play/render
	// create and install audio-sample-recorder
	bool b;
	// we cannot create local copy, because at a later stage
	// mixer::restoreAudioDevice(...) deletes old audio-dev and thus
	// AudioSampleRecorder would be destroyed two times...
	AudioSampleRecorder * freeze_recorder = new AudioSampleRecorder(
							DEFAULT_CHANNELS, b,
							engine::getMixer() );
	engine::getMixer()->setAudioDevice( freeze_recorder );

	// prepare stuff for playing correct things later
	engine::song()->playPattern( m_pattern, false );
	song::playPos & ppp = engine::song()->getPlayPos(
						song::Mode_PlayPattern );
	ppp.setTicks( 0 );
	ppp.setCurrentFrame( 0 );
	ppp.m_timeLineUpdate = false;

	m_pattern->m_freezeAborted = false;
	m_pattern->m_freezing = true;


	// now render everything
	while( ppp < m_pattern->length() &&
					m_pattern->m_freezeAborted == false )
	{
		freeze_recorder->processNextBuffer();
		m_statusDlg->setProgress( ppp * 100 / m_pattern->length() );
	}
	m_statusDlg->setProgress( 100 );
	// render tails
	while( engine::getMixer()->hasPlayHandles() &&
					m_pattern->m_freezeAborted == false )
	{
		freeze_recorder->processNextBuffer();
	}


	m_pattern->m_freezing = false;

	// reset song-editor settings
	engine::song()->stop();
	ppp.m_timeLineUpdate = true;

	// create final sample-buffer if freezing was successful
	if( m_pattern->m_freezeAborted == false )
	{
		freeze_recorder->createSampleBuffer(
						&m_pattern->m_frozenPattern );
	}

	// restore original audio-device
	engine::getMixer()->restoreAudioDevice();
*/
	m_statusDlg->setProgress( -1 );	// we're finished

}



#include "moc_Pattern.cxx"


