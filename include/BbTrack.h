/*
 * BbTrack.h - class BbTrack, a wrapper for using bbEditor
 *              (which is a singleton-class) as track
 *
 * Copyright (c) 2004-2008 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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


#ifndef _BB_TRACK_TNG_H
#define _BB_TRACK_TNG_H

#include <QtCore/QObject>
#include <QtCore/QMap>

#include "Track.h"

class trackLabelButton;
class TrackContainer;


class BbSegment : public TrackSegment
{
public:
	BbSegment( Track * _track, unsigned int _color = 0 );
	virtual ~BbSegment();

	virtual void saveSettings( QDomDocument & _doc, QDomElement & _parent );
	virtual void loadSettings( const QDomElement & _this );
	inline virtual QString nodeName() const
	{
		return( "bbSegment" );
	}

	inline unsigned int color() const
	{
		return( m_color );
	}


private:
	unsigned int m_color;

} ;




class EXPORT BbTrack : public Track
{
	Q_OBJECT
public:
	BbTrack( TrackContainer * _tc );
	virtual ~BbTrack();

	virtual bool play( const midiTime & _start,
					const fpp_t _frames,
					const f_cnt_t _frame_base,
							Sint16 _tco_num = -1 );
	virtual TrackSegment * createSegment( const midiTime & _pos );

	virtual void saveTrackSpecificSettings( QDomDocument & _doc,
							QDomElement & _parent );
	virtual void loadTrackSpecificSettings( const QDomElement & _this );

	static BbTrack * findBBTrack( int _bb_num );
	static int numOfBBTrack( Track * _track );
	static void swapBBTracks( Track * _track1, Track * _track2 );

	bool automationDisabled( Track * _track )
	{
		return( m_disabledTracks.contains( _track ) );
	}
	void disableAutomation( Track * _track )
	{
		m_disabledTracks.append( _track );
	}
	void enableAutomation( Track * _track )
	{
		m_disabledTracks.removeAll( _track );
	}


protected:
	inline virtual QString nodeName() const
	{
		return( "bbTrack" );
	}


private:
	QList<Track *> m_disabledTracks;

	typedef QMap<BbTrack *, int> infoMap;
	static infoMap s_infoMap;


	friend class bbTrackView;

} ;


#endif
