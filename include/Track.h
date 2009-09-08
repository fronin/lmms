/*
 * Track.h - declaration of classes concerning tracks -> neccessary for all
 *           track-like objects (beat/bassline, sample-track...)
 *
 * Copyright (c) 2004-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef _TRACK_TNG_H
#define _TRACK_TNG_H

#include <QtCore/QVector>
#include <QtCore/QList>
#include <QtGui/QWidget>

#include "lmms_basics.h"
#include "midi_time.h"
#include "JournallingObject.h"
#include "AutomatableModel.h"
#include "ModelView.h"


class QMenu;
class QPushButton;

class Track;
class TrackContainer;


class TrackSegment : public Model, public JournallingObject
{
	Q_OBJECT
	mapPropertyFromModel( bool, isMuted, setMuted, m_muted)

public:
	TrackSegment( Track * _parent );
	virtual ~TrackSegment();

	inline Track * getTrack() const
	{
		return m_track;
	}

	inline const QString & name() const
	{
		return m_name;
	}

	inline void setName( const QString & _name )
	{
		m_name = _name;
		emit dataChanged();
	}

	virtual QString displayName() const
	{
		return name();
	}

	inline const midiTime & startPosition() const
	{
		return m_startPosition;
	}

	inline midiTime endPosition() const
	{
		const int sp = m_startPosition;
		return sp + m_length;
	}

	inline const midiTime & length() const
	{
		return m_length;
	}

	virtual void movePosition( const midiTime & _pos );
	virtual void changeLength( const midiTime & _length );


public slots:
	void copy();
	void paste();
	void toggleMute();


protected:
	virtual void undoStep( JournalEntry & _je );
	virtual void redoStep( JournalEntry & _je );


signals:
	void lengthChanged();
	void positionChanged();
	void destroyedTCO();


private:
	enum Actions
	{
		NoAction,
		Move,
		Resize
	} ;

	Track * m_track;
	QString m_name;

	midiTime m_startPosition;
	midiTime m_length;

	BoolModel m_muted;
} ;




// base-class for all tracks
class EXPORT Track : public Model, public JournallingObject
{
	Q_OBJECT
	mapPropertyFromModel( bool, isMuted, setMuted, m_muted )
public:
	typedef QVector<TrackSegment *> SegmentVector;

	enum TrackTypes
	{
		InstrumentTrack = 0,
		BBTrack = 1,
		SampleTrack = 2,
		//EventTrack = 3,
		//VideoTrack = 4,
		AutomationTrack = 5,
		HiddenAutomationTrack = 6,
		NumTrackTypes = 7
	} ;

	Track( TrackTypes _type, TrackContainer * _tc );
	virtual ~Track();

	static Track * create( TrackTypes _tt, TrackContainer * _tc );
	static Track * create( const QDomElement & _this,
							TrackContainer * _tc );

	void clone();

	TrackTypes type() const
	{
		return m_type;
	}

	/* TODO{TNG} keep here or put in sequencer?? */
	virtual bool play( const midiTime & _start, const fpp_t _frames,
				const f_cnt_t _frame_base,
						Sint16 _tco_num = -1 ) = 0;

	virtual TrackSegment * createSegment( const midiTime & _pos ) = 0;

	virtual void saveTrackSpecificSettings( QDomDocument & _doc,
						QDomElement & _parent ) = 0;
	virtual void loadTrackSpecificSettings( const QDomElement & _this ) = 0;

	virtual void saveSettings( QDomDocument & _doc, QDomElement & _this );
	virtual void loadSettings( const QDomElement & _this );


	void setSimpleSerializing()
	{
		m_simpleSerializingMode = true;
	}

	// -- for usage by trackContentObject only ---------------
	//trackContentObject * addTCO( TrackContentObject * _tco );
	//void removeTCO( TrackContentObject * _tco );
	// -------------------------------------------------------

	int countSegments();
	/* TODO{TNG}: Maybe something better than index-based access? */
	TrackSegment * getSegment( int _tco_num );
	int getSegmentNum( TrackSegment * _segment );
	/*
	void getTCOsInRange( TcoVector & _tco_v, const midiTime & _start,
							const midiTime & _end );
	void swapPositionOfTCOs( int _tco_num1, int _tco_num2 );
	*/

	const SegmentVector & segments() const
	{
		return( m_trackSegments );
	}


	/* TODO{TNG}: Just use length of container?
	void insertTact( const midiTime & _pos );
	void removeTact( const midiTime & _pos );

	tact_t length() const;
	*/

	inline TrackContainer * trackContainer() const
	{
		return( m_trackContainer );
	}

	// name-stuff
	virtual const QString & name() const
	{
		return( m_name );
	}

	virtual QString displayName() const
	{
		return( name() );
	}

	using Model::dataChanged;


public slots:
	virtual void setName( const QString & _new_name )
	{
		m_name = _new_name;
		emit nameChanged();
	}

	void toggleSolo();


private:
	TrackContainer * m_trackContainer;
	TrackTypes m_type;
	QString m_name;

	BoolModel m_muted;
	BoolModel m_solo;
	bool m_mutedBeforeSolo;

	bool m_simpleSerializingMode;

	SegmentVector m_trackSegments;


signals:
	void destroyedTrack();
	void nameChanged();
	void trackSegmentAdded( TrackSegment * );
	void trackSegmentRemoved( TrackSegment * );

} ;

#endif
