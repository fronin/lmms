/*
 * InstrumentTrack.cpp - implementation of instrument-track-class
 *                        (window + data-structures)
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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QQueue>
#include <QtGui/QApplication>
#include <QtGui/QCloseEvent>
#include <QtGui/QFileDialog>
#include <QtGui/QLabel>
#include <QtGui/QLayout>
#include <QtGui/QLineEdit>
#include <QtGui/QMdiArea>
#include <QtGui/QMenu>
#include <QtGui/QMessageBox>
#include <QtGui/QMdiSubWindow>
#include <QtGui/QPainter>


#include "AudioPort.h"
#include "EffectChain.h"
#include "EffectRackView.h"
#include "FxMixerView.h"
#include "Instrument.h"
#include "InstrumentFunctionViews.h"
#include "InstrumentMidiIOView.h"
#include "InstrumentSoundShaping.h"
#include "InstrumentSoundShapingView.h"
#include "InstrumentTrack.h"
#include "MainWindow.h"
#include "MidiClient.h"
#include "MidiPortMenu.h"
#include "Pattern.h"
#include "PluginView.h"
#include "Song.h"
#include "ResourceAction.h"
#include "ResourceDB.h"


#include "BbTrack.h"
#include "config_mgr.h"
#include "debug.h"
#include "embed.h"
#include "engine.h"
#include "fade_button.h"
#include "gui_templates.h"
#include "knob.h"
#include "lcd_spinbox.h"
#include "led_checkbox.h"
#include "mmp.h"
#include "note_play_handle.h"
#include "sample_play_handle.h"
#include "string_pair_drag.h"
#include "surround_area.h"
#include "tab_widget.h"
#include "tooltip.h"


// #### IT:
InstrumentTrack::InstrumentTrack( TrackContainer * _tc ) :
	Track( Track::InstrumentTrack, _tc ),
	MidiEventProcessor(),
	m_audioPort( tr( "unnamed_track" ) ),
	m_midiPort( tr( "unnamed_track" ), engine::getMixer()->midiClient(),
								this, this ),
	m_notes(),
        m_volumeModel( DefaultVolume, MinVolume, MaxVolume, 0.1f, this,
							tr( "Volume" ) ),
        m_panningModel( DefaultPanning, PanningLeft, PanningRight, 0.1f,
							this, tr( "Panning" ) ),
	m_pitchModel( 0, -100, 100, 1, this, tr( "Pitch" ) ),
	m_pitchRangeModel( 1, 1, 24, this, tr( "Pitch range" ) ),
	m_effectChannelModel( 0, 0, 0, this, tr( "FX channel" ) ), // change this so it's a combo box, all the channels and then new.
	m_instrument( NULL ),
	m_soundShaping( this ),
	m_arpeggiator( this ),
	m_chordCreator( this ),
	m_piano( this )
{
	m_effectChannelModel.setRange( 0, engine::fxMixer()->numChannels()-1, 1);
	connect( baseNoteModel(), SIGNAL( dataChanged() ),
			this, SLOT( updateBaseNote() ) );
	connect( &m_pitchModel, SIGNAL( dataChanged() ),
			this, SLOT( updatePitch() ) );
	connect( &m_pitchRangeModel, SIGNAL( dataChanged() ),
			this, SLOT( updatePitchRange() ) );


	for( int i = 0; i < NumKeys; ++i )
	{
		m_notes[i] = NULL;
		m_runningMidiNotes[i] = 0;
	}


	setName( tr( "Default preset" ) );

}




InstrumentTrack::~InstrumentTrack()
{
	// kill all running notes
	silenceAllNotes();

	/* TODO{TNG}: Uncomment
	engine::getMixer()->removePlayHandles( this );
	*/

	// now we're safe deleting the instrument
	delete m_instrument;
}




void InstrumentTrack::processAudioBuffer( sampleFrame * _buf,
							const fpp_t _frames,
							notePlayHandle * _n )
{
	// we must not play the sound if this instrumentTrack is muted...
	if( isMuted() || ( _n && _n->bbTrackMuted() ) )
	{
		return;
	}

	// if effects "went to sleep" because there was no input, wake them up
	// now
	m_audioPort.effects()->startRunning();

	float v_scale = (float) getVolume() / DefaultVolume;

	// instruments using instrument-play-handles will call this method
	// without any knowledge about notes, so they pass NULL for _n, which
	// is no problem for us since we just bypass the envelopes+LFOs
	if( _n != NULL )
	{
		m_soundShaping.processAudioBuffer( _buf, _frames, _n );
		v_scale *= ( (float) _n->getVolume() / DefaultVolume );
	}
	else
	{
		if( getVolume() < DefaultVolume &&
					m_instrument->isMidiBased() )
		{
			v_scale = 1;
		}
	}

	m_audioPort.setNextFxChannel( m_effectChannelModel.value() );
	
	int panning = m_panningModel.value();
	if( _n != NULL )
	{
		panning += _n->getPanning();
		panning = tLimit<int>( panning, PanningLeft, PanningRight );
	}
	engine::getMixer()->bufferToPort( _buf, ( _n != NULL ) ?
		qMin<f_cnt_t>(_n->framesLeftForCurrentPeriod(), _frames ) :
								_frames,
			( _n != NULL ) ? _n->offset() : 0,
			panningToVolumeVector( panning,	v_scale ),
								&m_audioPort );
}




midiEvent InstrumentTrack::applyMasterKey( const midiEvent & _me )
{
	midiEvent copy( _me );
	switch( _me.m_type )
	{
		case MidiNoteOn:
		case MidiNoteOff:
		case MidiKeyPressure:
			copy.key() = masterKey( _me.key() );
			break;
		default:
			break;
	}
	return copy;
}




void InstrumentTrack::processInEvent( const midiEvent & _me,
							const midiTime & _time )
{
	engine::getMixer()->lock();
	switch( _me.m_type )
	{
		// we don't send MidiNoteOn, MidiNoteOff and MidiKeyPressure
		// events to instrument as notePlayHandle will send them on its
		// own
		case MidiNoteOn: 
			if( _me.velocity() > 0 )
			{
				if( m_notes[_me.key()] == NULL )
				{
					if( !configManager::inst()->value( "ui",
						"manualchannelpiano" ).toInt() )
					{
						m_piano.setKeyState(
							_me.key(), true );
					}
					// create temporary note
					note n;
					n.setKey( _me.key() );
					n.setVolume( _me.getVolume() );

					// create (timed) note-play-handle
					notePlayHandle * nph = new
						notePlayHandle( this,
							_time.frames(
						engine::framesPerTick() ),
						typeInfo<f_cnt_t>::max() / 2,
									n );
					if( engine::getMixer()->addPlayHandle(
									nph ) )
					{
						m_notes[_me.key()] = nph;
					}

					emit noteOn( n );
				}
				break;
			}

		case MidiNoteOff:
		{
			notePlayHandle * n = m_notes[_me.key()];
			if( n != NULL )
			{
				// create dummy-note which has the same length
				// as the played note for sending it later
				// to all slots connected to signal noteOff()
				// this is for example needed by piano-roll for
				// recording notes into a pattern
				note done_note(
					midiTime( static_cast<f_cnt_t>(
						n->totalFramesPlayed() /
						engine::framesPerTick() ) ),
							0,
							n->key(),
							n->getVolume(),
							n->getPanning() );

				n->noteOff();
				m_notes[_me.key()] = NULL;

				emit noteOff( done_note );
			}
			break;
		}

		case MidiKeyPressure:
			if( m_notes[_me.key()] != NULL )
			{
				m_notes[_me.key()]->setVolume( _me.getVolume() );
			}
			break;

		case MidiPitchBend:
			// updatePitch() is connected to
			// m_pitchModel::dataChanged() which will send out
			// MidiPitchBend events
			m_pitchModel.setValue( m_pitchModel.minValue() +
					_me.m_data.m_param[0] *
						m_pitchModel.range() / 16384 );
			break;

		case MidiControlChange:
		case MidiProgramChange:
			m_instrument->handleMidiEvent( _me, _time );
			break;
		
		case MidiMetaEvent:
			// handle special cases such as note panning
			switch( _me.m_metaEvent )
			{
				case MidiNotePanning:
					if( m_notes[_me.key()] != NULL )
					{
		m_notes[_me.key()]->setPanning( _me.getPanning() );
					}
					break;
				default:
					printf( "instrument-track: unhandled "
						    "MIDI meta event: %i\n",
							_me.m_metaEvent );
					break;
			}
			break;
			
		default:
			if( !m_instrument->handleMidiEvent( _me, _time ) )
			{
				printf( "instrument-track: unhandled "
					"MIDI event %d\n", _me.m_type );
			}
			break;
	}
	engine::getMixer()->unlock();
}




void InstrumentTrack::processOutEvent( const midiEvent & _me,
							const midiTime & _time )
{
	int k;

	switch( _me.m_type )
	{
		case MidiNoteOn:
			if( !configManager::inst()->value( "ui",
						"manualchannelpiano" ).toInt() )
			{
				m_piano.setKeyState( _me.key(), true );
			}
			if( !configManager::inst()->value( "ui",
				"disablechannelactivityindicators" ).toInt() )
			{
				if( m_notes[_me.key()] == NULL )
				{
					emit newNote();
				}
			}
			k = masterKey( _me.key() );
			if( k >= 0 && k < NumKeys )
			{
				if( m_runningMidiNotes[k] > 0 )
				{
					m_instrument->handleMidiEvent(
	midiEvent( MidiNoteOff, midiPort()->realOutputChannel(), k, 0 ),
									_time );
				}
				++m_runningMidiNotes[k];
				m_instrument->handleMidiEvent(
	midiEvent( MidiNoteOn, midiPort()->realOutputChannel(), k,
						_me.velocity() ), _time );
			}
			break;

		case MidiNoteOff:
			if( !configManager::inst()->value( "ui",
						"manualchannelpiano" ).toInt() )
			{
				m_piano.setKeyState( _me.key(), false );
			}
			k = masterKey( _me.key() );
			if( k >= 0 && k < NumKeys &&
						--m_runningMidiNotes[k] <= 0 )
			{
				m_instrument->handleMidiEvent(
	midiEvent( MidiNoteOff, midiPort()->realOutputChannel(), k, 0 ),
									_time );
			}
			break;

		default:
			if( m_instrument != NULL )
			{
				m_instrument->handleMidiEvent(
							applyMasterKey( _me ),
									_time );
			}
			break;
	}

	// if appropriate, midi-port does futher routing
	m_midiPort.processOutEvent( _me, _time );
}




void InstrumentTrack::silenceAllNotes()
{
	engine::getMixer()->lock();
	for( int i = 0; i < NumKeys; ++i )
	{
		m_notes[i] = NULL;
		m_runningMidiNotes[i] = 0;
	}

	// invalidate all NotePlayHandles linked to this track
	m_processHandles.clear();
	/* TODO{TNG}: Uncomment
	engine::getMixer()->removePlayHandles( this,
						playHandle::NotePlayHandle );
	*/
	engine::getMixer()->unlock();
}




f_cnt_t InstrumentTrack::beatLen( notePlayHandle * _n ) const
{
	if( m_instrument != NULL )
	{
		const f_cnt_t len = m_instrument->beatLen( _n );
		if( len > 0 )
		{
			return len;
		}
	}
	return m_soundShaping.envFrames();
}




void InstrumentTrack::playNote( notePlayHandle * _n, 
						sampleFrame * _working_buffer )
{
	// arpeggio- and chord-widget has to do its work -> adding sub-notes
	// for chords/arpeggios
	m_chordCreator.processNote( _n );
	m_arpeggiator.processNote( _n );

	if( !_n->isArpeggioBaseNote() && m_instrument != NULL )
	{
		// all is done, so now lets play the note!
		m_instrument->playNote( _n, _working_buffer );
	}
}




QString InstrumentTrack::instrumentName() const
{
	if( m_instrument != NULL )
	{
		return m_instrument->displayName();
	}
	return QString::null;
}




void InstrumentTrack::deleteNotePluginData( notePlayHandle * _n )
{
	if( m_instrument != NULL )
	{
		m_instrument->deleteNotePluginData( _n );
	}

	// Notes deleted when keys still pressed
	if( m_notes[_n->key()] == _n )
	{
		note done_note( midiTime( static_cast<f_cnt_t>(
						_n->totalFramesPlayed() /
						engine::framesPerTick() ) ),
					0, _n->key(),
					_n->getVolume(), _n->getPanning() );
		_n->noteOff();
		m_notes[_n->key()] = NULL;
		emit noteOff( done_note );
	}
}




void InstrumentTrack::setName( const QString & _new_name )
{
	// when changing name of track, also change name of those patterns,
	// which have the same name as the instrument-track
	for( SegmentVector::const_iterator it = segments().begin();
			it != segments().end(); ++it )
	{
		Pattern * p = dynamic_cast<Pattern *>( *it );
		if( ( p != NULL && p->name() == name() ) || p->name() == "" )
		{
			p->setName( _new_name );
		}
	}

	Track::setName( _new_name );
	m_midiPort.setName( name() );
	m_audioPort.setName( name() );

	emit nameChanged();
}






void InstrumentTrack::updateBaseNote()
{
	engine::getMixer()->lock();
	for( NotePlayHandleList::Iterator it = m_processHandles.begin();
					it != m_processHandles.end(); ++it )
	{
		( *it )->updateFrequency();
	}
	engine::getMixer()->unlock();
}




void InstrumentTrack::updatePitch()
{
	updateBaseNote();
	processOutEvent( midiEvent( MidiPitchBend,
					midiPort()->realOutputChannel(),
					midiPitch() ), 0 );
}




void InstrumentTrack::updatePitchRange()
{
	const int r = m_pitchRangeModel.value();
	m_pitchModel.setRange( -100 * r, 100 * r );
}




int InstrumentTrack::masterKey( int _midi_key ) const
{
	int key = baseNoteModel()->value() - engine::song()->masterPitch();
	return tLimit<int>( _midi_key - ( key - DefaultKey ), 0, NumKeys );
}




void InstrumentTrack::removeMidiPortNode( multimediaProject & _mmp )
{
	QDomNodeList n = _mmp.elementsByTagName( "midiport" );
	n.item( 0 ).parentNode().removeChild( n.item( 0 ) );
}




bool InstrumentTrack::play( const midiTime & _start,
					const fpp_t _frames,
					const f_cnt_t _offset,
							Sint16 _tco_num )
{
	/* TODO{TNG} Reimplement Play()
	const float frames_per_tick = engine::framesPerTick();

	SegmentVector tcos;
	bbTrack * bb_track = NULL;
	if( _tco_num >= 0 )
	{
		trackContentObject * tco = getTCO( _tco_num );
		tcos.push_back( tco );
		bb_track = bbTrack::findBBTrack( _tco_num );
	}
	else
	{
		getTCOsInRange( tcos, _start, _start + static_cast<int>(
					_frames / frames_per_tick ) );
	}

	// Handle automation: detuning
	for( NotePlayHandleList::Iterator it = m_processHandles.begin();
					it != m_processHandles.end(); ++it )
	{
		( *it )->processMidiTime( _start );
	}

	if ( tcos.size() == 0 )
	{
		return false;
	}

	bool played_a_note = false;	// will be return variable

	for( tcoVector::Iterator it = tcos.begin(); it != tcos.end(); ++it )
	{
		pattern * p = dynamic_cast<pattern *>( *it );
		// everything which is not a pattern or muted won't be played
		if( p == NULL || ( *it )->isMuted() )
		{
			continue;
		}
		midiTime cur_start = _start;
        // Make cur_start the offset within the pattern
		if( _tco_num < 0 )
		{
			cur_start -= p->startPosition();
		}
		if( p->frozen() && !engine::song()->isExporting() )
		{
			if( cur_start > 0 )
			{
				continue;
			}

			samplePlayHandle * handle = new samplePlayHandle( p );
			handle->setBBTrack( bb_track );
			handle->setOffset( _offset );
			// send it to the mixer
			engine::getMixer()->addPlayHandle( handle );
			played_a_note = true;
			continue;
		}

		// get all notes from the given pattern...
		const NoteVector & notes = p->notes();
		// ...and set our index to zero
		NoteVector::ConstIterator it = notes.begin();
#if LMMS_SINGERBOT_SUPPORT
		int note_idx = 0;
#endif

		// very effective algorithm for playing notes that are
		// positioned within the current sample-frame


		if( cur_start > 0 )
		{
			// skip notes which are posated before start-tact
			while( it != notes.end() && ( *it )->pos() < cur_start )
			{
#if LMMS_SINGERBOT_SUPPORT
				if( ( *it )->length() != 0 )
				{
					++note_idx;
				}
#endif
				++it;
			}
		}

		note * cur_note;
		while( it != notes.end() &&
					( cur_note = *it )->pos() == cur_start )
		{
			if( cur_note->length() != 0 )
			{
				const f_cnt_t note_frames =
					cur_note->length().frames(
							frames_per_tick );

				notePlayHandle * note_play_handle =
					new notePlayHandle( this, _offset,
								note_frames,
								*cur_note );
				note_play_handle->setBBTrack( bb_track );
#if LMMS_SINGERBOT_SUPPORT
				note_play_handle->setPatternIndex( note_idx );
#endif
				engine::getMixer()->addPlayHandle(
							note_play_handle );
				played_a_note = true;
#if LMMS_SINGERBOT_SUPPORT
				++note_idx;
#endif
			}
			++it;
		}
	}
	return played_a_note;
	*/
	return false;
}




TrackSegment * InstrumentTrack::createSegment( const midiTime & )
{
	return new Pattern( this );
}



void InstrumentTrack::saveTrackSpecificSettings( QDomDocument & _doc,
							QDomElement & _this )
{
	m_volumeModel.saveSettings( _doc, _this, "vol" );
	m_panningModel.saveSettings( _doc, _this, "pan" );
	m_pitchModel.saveSettings( _doc, _this, "pitch" );
	m_pitchRangeModel.saveSettings( _doc, _this, "pitchrange" );

	m_effectChannelModel.saveSettings( _doc, _this, "fxch" );
	baseNoteModel()->saveSettings( _doc, _this, "basenote" );

	if( m_instrument != NULL )
	{
		QDomElement i = _doc.createElement( "instrument" );
		i.setAttribute( "name", m_instrument->descriptor()->name );
		m_instrument->saveState( _doc, i );
		_this.appendChild( i );
	}
	m_soundShaping.saveState( _doc, _this );
	m_chordCreator.saveState( _doc, _this );
	m_arpeggiator.saveState( _doc, _this );
	m_midiPort.saveState( _doc, _this );
	m_audioPort.effects()->saveState( _doc, _this );
}




void InstrumentTrack::loadTrackSpecificSettings( const QDomElement & _this )
{
	silenceAllNotes();

	engine::getMixer()->lock();

	m_volumeModel.loadSettings( _this, "vol" );

	// compat-hacks - move to mmp::upgrade
	if( _this.hasAttribute( "surpos" ) || _this.hasAttribute( "surpos-x" )
		|| !_this.firstChildElement( "automationpattern" ).
				firstChildElement( "surpos-x" ).isNull() )
	{
		surroundAreaModel m( this, this );
		m.loadSettings( _this, "surpos" );
		m_panningModel.setValue( m.x() * 100 / SURROUND_AREA_SIZE );
	}
	else
	{
		m_panningModel.loadSettings( _this, "pan" );
	}

	m_pitchRangeModel.loadSettings( _this, "pitchrange" );
	m_pitchModel.loadSettings( _this, "pitch" );
	m_effectChannelModel.loadSettings( _this, "fxch" );

	if( _this.hasAttribute( "baseoct" ) )
	{
		// TODO: move this compat code to mmp.cpp -> upgrade()
		baseNoteModel()->setInitValue( _this.
			attribute( "baseoct" ).toInt()
				* KeysPerOctave
				+ _this.attribute( "basetone" ).toInt() );
	}
	else
	{
		baseNoteModel()->loadSettings( _this, "basenote" );
	}

	// clear effect-chain just in case we load an old preset without FX-data
	m_audioPort.effects()->clear();

	QDomNode node = _this.firstChild();
	while( !node.isNull() )
	{
		if( node.isElement() )
		{
			if( m_soundShaping.nodeName() == node.nodeName() )
			{
				m_soundShaping.restoreState( node.toElement() );
			}
			else if( m_chordCreator.nodeName() == node.nodeName() )
			{
				m_chordCreator.restoreState( node.toElement() );
			}
			else if( m_arpeggiator.nodeName() == node.nodeName() )
			{
				m_arpeggiator.restoreState( node.toElement() );
			}
			else if( m_midiPort.nodeName() == node.nodeName() )
			{
				m_midiPort.restoreState( node.toElement() );
			}
			else if( m_audioPort.effects()->nodeName() == node.nodeName() )
			{
				m_audioPort.effects()->restoreState( node.toElement() );
			}
			else if( node.nodeName() == "instrument" )
			{
				delete m_instrument;
				m_instrument = NULL;
				m_instrument = Instrument::instantiate(
					node.toElement().attribute( "name" ),
									this );
				m_instrument->restoreState(
						node.firstChildElement() );
				emit instrumentChanged();
			}
			// compat code - if node-name doesn't match any known
			// one, we assume that it is an instrument-plugin
			// which we'll try to load
			/* TODO{TNG}: bring back automation and move this compat code away
			else if( node.nodeName() != "connection" &&
					automationPattern::classNodeName() !=
							node.nodeName() &&
					!node.toElement().hasAttribute( "id" ) )
			{
				delete m_instrument;
				m_instrument = NULL;
				m_instrument = Instrument::instantiate(
							node.nodeName(), this );
				if( m_instrument->nodeName() ==
							node.nodeName() )
				{
					m_instrument->restoreState(
							node.toElement() );
				}
				emit instrumentChanged();
			}
			*/
		}
		node = node.nextSibling();
        }
	engine::getMixer()->unlock();
}




Instrument * InstrumentTrack::loadInstrument( const QString & _plugin_name )
{
	silenceAllNotes();

	engine::getMixer()->lock();
	delete m_instrument;
	m_instrument = Instrument::instantiate( _plugin_name, this );
	engine::getMixer()->unlock();

	emit instrumentChanged();

	return m_instrument;
}


#include "moc_InstrumentTrack.cxx"

