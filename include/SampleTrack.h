/*
 * sample_track.h - class sampleTrack, a track which provides arrangement of
 *                  samples
 *
 * Copyright (c) 2005-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef _SAMPLE_TRACK_H
#define _SAMPLE_TRACK_H

#include <QtGui/QDialog>

#include "AudioPort.h"
#include "Track.h"

class EffectRackView;
class knob;
class sampleBuffer;


class SampleSegment : public TrackSegment
{
	Q_OBJECT
	mapPropertyFromModel(bool,isRecord,setRecord,m_recordModel);
public:
	SampleSegment( Track * _track );
	virtual ~SampleSegment();

	virtual void changeLength( const midiTime & _length );
	const QString & sampleFile() const;

	virtual void saveSettings( QDomDocument & _doc, QDomElement & _parent );
	virtual void loadSettings( const QDomElement & _this );
	inline virtual QString nodeName() const
	{
		return "sampletco";
	}

	sampleBuffer * getSampleBuffer()
	{
		return m_sampleBuffer;
	}

	midiTime sampleLength() const;



public slots:
	void setSampleBuffer( sampleBuffer * _sb );
	void setSampleFile( const QString & _sf );
	void updateLength( bpm_t = 0 );
	void toggleRecord();


private:
	sampleBuffer * m_sampleBuffer;
	BoolModel m_recordModel;


	friend class sampleTCOView;


signals:
	void sampleChanged();

} ;




class SampleTrack : public Track
{
	Q_OBJECT
public:
	SampleTrack( TrackContainer * _tc );
	virtual ~SampleTrack();

	virtual bool play( const midiTime & _start, const fpp_t _frames,
						const f_cnt_t _frame_base,
							Sint16 _tco_num = -1 );

	virtual TrackSegment * createSegment( const midiTime & _pos );


	virtual void saveTrackSpecificSettings( QDomDocument & _doc,
							QDomElement & _parent );
	virtual void loadTrackSpecificSettings( const QDomElement & _this );

	inline AudioPort * audioPort()
	{
		return &m_audioPort;
	}

	virtual QString nodeName() const
	{
		return "sampletrack";
	}


private:
	AudioPort m_audioPort;
	FloatModel m_volumeModel;


	friend class sampleTrackView;

} ;


#endif
