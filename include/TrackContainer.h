/*
 * TrackContainer.h - base-class for all track-containers like Song, BB
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


#ifndef _TRACK_CONTAINER_TNG_H
#define _TRACK_CONTAINER_TNG_H

#include <QReadWriteLock>

#include "Track.h"
#include "SerializingObject.h"


class InstrumentTrack;	// For DummyTrackContainer


class EXPORT TrackContainer : public Model, public SerializingObject
{
	Q_OBJECT
public:
	typedef QVector<Track *> TrackList;

	TrackContainer();
	virtual ~TrackContainer();

	virtual void saveSettings( QDomDocument & _doc, QDomElement & _parent );

	virtual void loadSettings( const QDomElement & _this );

	int countTracks( Track::TrackTypes _tt = Track::NumTrackTypes ) const;

	void addTrack( Track * _track );
	void removeTrack( Track * _track );

	//virtual void updateAfterTrackAdd();

	void clearAllTracks();

	const TrackList & tracks() const
	{
		return m_tracks;
	}

	bool isEmpty() const;

	static const QString classNodeName()
	{
		return "trackContainer";
	}


signals:
	void trackAdded( Track * _track );
	void trackRemoved( Track * _track );

protected:
	mutable QReadWriteLock m_tracksMutex;

private:
	TrackList m_tracks;

};




class DummyTrackContainer : public TrackContainer
{
public:
	DummyTrackContainer();

	virtual ~DummyTrackContainer()
	{
	}

	virtual QString nodeName() const
	{
		return "DummyTrackContainer";
	}

	InstrumentTrack * dummyInstrumentTrack()
	{
		return m_dummyInstrumentTrack;
	}


private:
	InstrumentTrack * m_dummyInstrumentTrack;
} ;


#endif
