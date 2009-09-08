/*
 * TrackContainer.cpp - implementation of base-class for all track-containers
 *                       like Song-Editor, BB...
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

#include <QtXml/QDomElement>

#include "TrackContainer.h"
#include "engine.h"

#include "InstrumentTrack.h"		// For DummyTrackContainer


TrackContainer::TrackContainer() :
	Model( NULL ),
	SerializingObject(),
	m_tracksMutex(),
	m_tracks()
{
}




TrackContainer::~TrackContainer()
{
	clearAllTracks();
}




void TrackContainer::saveSettings( QDomDocument & _doc, QDomElement & _this )
{
	_this.setTagName( classNodeName() );
	_this.setAttribute( "type", nodeName() );

	// save settings of each track
	m_tracksMutex.lockForRead();
	for( int i = 0; i < m_tracks.size(); ++i )
	{
		m_tracks[i]->saveState( _doc, _this );
	}
	m_tracksMutex.unlock();
}




void TrackContainer::loadSettings( const QDomElement & _this )
{
	QDomNode node = _this.firstChild();
	while( !node.isNull() )
	{
		if( node.isElement() &&
			!node.toElement().attribute( "metadata" ).toInt() )
		{
			Track::create( node.toElement(), this );
		}
		node = node.nextSibling();
	}
}




int TrackContainer::countTracks( Track::TrackTypes _tt ) const
{
	int cnt = 0;
	m_tracksMutex.lockForRead();
	for( int i = 0; i < m_tracks.size(); ++i )
	{
		if( m_tracks[i]->type() == _tt )
		{
			++cnt;
		}
	}
	m_tracksMutex.unlock();
	return( cnt );
}




void TrackContainer::addTrack( Track * _track )
{
	if( _track->type() != Track::HiddenAutomationTrack )
	{
		m_tracksMutex.lockForWrite();
		m_tracks.push_back( _track );
		m_tracksMutex.unlock();
		emit trackAdded( _track );
	}
}




void TrackContainer::removeTrack( Track * _track )
{
	int index;
	m_tracksMutex.lockForRead();
	index = m_tracks.indexOf( _track );
	m_tracksMutex.unlock();
	if( index != -1 )
	{
		m_tracksMutex.lockForWrite();
		m_tracks.remove( index );
		m_tracksMutex.unlock();
		emit trackRemoved( _track );

		/* TODO{TNG}: Better signalling
		if( engine::song() )
		{
			engine::song()->setModified();
		}
		*/
	}
}




void TrackContainer::clearAllTracks()
{
	//m_tracksMutex.lockForWrite();
	while( !m_tracks.isEmpty() )
	{
		emit trackRemoved( m_tracks.first() );
		delete m_tracks.first();
	}
	//m_tracksMutex.unlock();
}



bool TrackContainer::isEmpty() const
{
	for( TrackList::const_iterator it = m_tracks.begin();
						it != m_tracks.end(); ++it )
	{
		if( ( *it )->countSegments() > 0 )
		{
			return false;
		}
	}
	return true;
}




DummyTrackContainer::DummyTrackContainer() :
		TrackContainer(),
		m_dummyInstrumentTrack( NULL )
{
	m_dummyInstrumentTrack = dynamic_cast<InstrumentTrack *>(
			Track::create( Track::InstrumentTrack,
			this ) );
}


#include "moc_TrackContainer.cxx"

/* vim: set tw=0 noexpandtab: */
