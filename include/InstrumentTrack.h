/*
 * InstrumentTrack.h - declaration of class InstrumentTrack, a track + window
 *                     which holds an instrument-plugin
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

#ifndef _INSTRUMENT_TRACK_H
#define _INSTRUMENT_TRACK_H

#include <QtGui/QPushButton>

#include "AudioPort.h"
#include "InstrumentFunctions.h"
#include "InstrumentSoundShaping.h"
#include "MidiEventProcessor.h"
#include "MidiPort.h"
#include "Piano.h"
#include "PianoView.h"
#include "Track.h"
#include "note_play_handle.h"


class QLineEdit;
template<class T> class QQueue;
class ArpeggiatorView;
class ChordCreatorView;
class EffectRackView;
class InstrumentSoundShapingView;
class fadeButton;
class Instrument;
class InstrumentTrackWindow;
class InstrumentMidiIOView;
class knob;
class lcdSpinBox;
class midiPortMenu;
class multimediaProject;
class notePlayHandle;
class PluginView;
class tabWidget;
class trackLabelButton;


class EXPORT InstrumentTrack : public Track, public MidiEventProcessor
{
	Q_OBJECT
	mapPropertyFromModel(int,getVolume,setVolume,m_volumeModel);
public:
	InstrumentTrack( TrackContainer * _tc );
	virtual ~InstrumentTrack();

	// used by instrument
	void processAudioBuffer( sampleFrame * _buf, const fpp_t _frames,
							notePlayHandle * _n );

	midiEvent applyMasterKey( const midiEvent & _me );

	virtual void processInEvent( const midiEvent & _me,
					const midiTime & _time );
	virtual void processOutEvent( const midiEvent & _me,
						const midiTime & _time );
	// silence all running notes played by this track
	void silenceAllNotes();

	f_cnt_t beatLen( notePlayHandle * _n ) const;


	// for capturing note-play-events -> need that for arpeggio,
	// filter and so on
	void playNote( notePlayHandle * _n, sampleFrame * _working_buffer );

	QString instrumentName() const;
	inline const Instrument * instrument() const
	{
		return m_instrument;
	}

	inline Instrument * instrument()
	{
		return m_instrument;
	}

	void deleteNotePluginData( notePlayHandle * _n );

	// name-stuff
	virtual void setName( const QString & _new_name );

	// translate given key of a note-event to absolute key (i.e.
	// add global master-pitch and base-note of this instrument track)
	int masterKey( int _midi_key ) const;

	// translate pitch to midi-pitch [0,16383]
	inline int midiPitch() const
	{
		return (int)( ( m_pitchModel.value()+100 ) * 16383 ) / 200;
	}

	// play everything in given frame-range - creates note-play-handles
	virtual bool play( const midiTime & _start, const fpp_t _frames,
					const f_cnt_t _frame_base,
							Sint16 _tco_num = -1 );

	// create new pattern
	virtual TrackSegment * createSegment( const midiTime & _pos );


	// called by track
	virtual void saveTrackSpecificSettings( QDomDocument & _doc,
							QDomElement & _parent );
	virtual void loadTrackSpecificSettings( const QDomElement & _this );

	using Track::setJournalling;


	// load instrument whose name matches given one
	Instrument * loadInstrument( const QString & _instrument_name );

	inline AudioPort * audioPort()
	{
		return &m_audioPort;
	}

	inline MidiPort * midiPort()
	{
		return &m_midiPort;
	}

	Piano * pianoModel()
	{
		return &m_piano;
	}

	bool isArpeggiatorEnabled() const
	{
		return m_arpeggiator.m_arpEnabledModel.value();
	}

	// simple helper for removing midiport-XML-node when loading presets
	static void removeMidiPortNode( multimediaProject & _mmp );

	FloatModel * volumeModel()
	{
		return &m_volumeModel;
	}

	FloatModel * panningModel()
	{
		return &m_panningModel;
	}

	FloatModel * pitchModel()
	{
		return &m_pitchModel;
	}

	IntModel * pitchRangeModel()
	{
		return &m_pitchRangeModel;
	}

	IntModel * effectChannelModel()
	{
		return &m_effectChannelModel;
	}


signals:
	void instrumentChanged();
	void newNote();
	void noteOn( const note & _n );
	void noteOff( const note & _n );
	void nameChanged();


protected:
	virtual QString nodeName() const
	{
		return "instrumentTrack";
	}


protected slots:
	void updateBaseNote();
	void updatePitch();
	void updatePitchRange();


private:
	AudioPort m_audioPort;
	MidiPort m_midiPort;

	notePlayHandle * m_notes[NumKeys];
	int m_runningMidiNotes[NumKeys];

	NotePlayHandleList m_processHandles;


	FloatModel m_volumeModel;
	FloatModel m_panningModel;
	FloatModel m_pitchModel;
	IntModel m_pitchRangeModel;
	IntModel m_effectChannelModel;


	Instrument * m_instrument;
	InstrumentSoundShaping m_soundShaping;
	Arpeggiator m_arpeggiator;
	ChordCreator m_chordCreator;

	Piano m_piano;

	friend class notePlayHandle;
	friend class FlpImport;
} ;


#endif
