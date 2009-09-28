/*
 * track_item.cpp - the base-class for Tracks in the TrackContainerScene.
 * Despite the name, it really isn't a GraphicsItem at all.
 *
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


#include "Track.h"
#include "gui/tracks/track_item.h"
#include "gui/tracks/track_content_object_item.h"
#include "gui/tracks/track_container_scene.h"


// Needed for addSegment
#include "BbTrack.h"
#include "Pattern.h"
#include "SampleTrack.h"
#include "gui/tracks/bb_tco_item.h"
#include "gui/tracks/pattern_item.h"
#include "gui/tracks/SampleSegmentItem.h"

TrackItem::TrackItem( TrackContainerScene * _scene, Track * _track )
{
	m_scene = _scene;
	m_track = _track;

	// create views for already existing TCOs
	const Track::SegmentVector & segs = m_track->segments();
	for (Track::SegmentVector::const_iterator it = segs.begin(); it != segs.end(); ++it)
	{
		addSegment( *it );
	}

	QObject * obj = _track;
	connect( obj, SIGNAL( trackContentObjectAdded( trackContentObject * ) ),
	         this, SLOT( addTCO( trackContentObject * ) ),
	         Qt::QueuedConnection );

	connect( obj, SIGNAL( trackContentObjectRemoved( trackContentObject * ) ),
	         this, SLOT( removeTCO( trackContentObject * ) ) );
}



TrackItem::~TrackItem()
{
	for (QMap<TrackSegment*, TrackSegmentItem*>::iterator i = m_segmentItems.begin();
			i != m_segmentItems.end(); ++i)
	{
		TrackSegmentItem * item = i.value();
		m_scene->removeItem(item);
		m_segmentItems.erase(i);
		delete item;
	}
}


void TrackItem::addSegment( TrackSegment * _tco )
{
	// TODO move into a factory?
	TrackSegmentItem * tcoItem;

	if( BbSegment * bbSegment = dynamic_cast<BbSegment *>( _tco ) )
	{
		tcoItem = new BbSegmentItem( this, bbSegment );
	}
	else if( Pattern * pat = dynamic_cast<Pattern *>( _tco ) )
	{
		tcoItem = new PatternItem( this, pat );
	}
	else if( SampleSegment * ss = dynamic_cast<SampleSegment *>( _tco ) )
	{
		tcoItem = new SampleSegmentItem( this, ss );
	}
	else
	{
		// Whoa..
		return;
	}
	
	// TODO refactor to private updateTCOGeometry
	tcoItem->setPos( tcoItem->x(), y() );

	m_segmentItems.insert( _tco, tcoItem );
	m_scene->addItem( tcoItem );
}



void TrackItem::removeSegment( TrackSegment * _tco )
{
	QMap<TrackSegment*, TrackSegmentItem*>::iterator i =
			m_segmentItems.find( _tco );

	if( i != m_segmentItems.end() && i.key() == _tco )
	{
		TrackSegmentItem * item = i.value();
		m_scene->removeItem(*i);
		m_segmentItems.erase(i);
		delete item;
	}
 }


void TrackItem::setHeight( float _height )
{
	m_rect.setHeight( _height );
	for( QMap<TrackSegment*, TrackSegmentItem*>::const_iterator it = m_segmentItems.constBegin();
			it != m_segmentItems.constEnd(); ++it )
	{
		(*it)->updateGeometry();
	}
}



void TrackItem::setY( float _y )
{
    //printf("TRK %lld: setY(%.2f)\n", m_track, _y);
	m_rect.moveTop( _y );
	for( QMap<TrackSegment*, TrackSegmentItem*>::const_iterator it = m_segmentItems.constBegin();
			it != m_segmentItems.constEnd(); ++it )
	{
		(*it)->setPos( (*it)->x(), y() );
	}
}



float TrackItem::height()
{
	return m_rect.height();
}



float TrackItem::y()
{
	return m_rect.y();
}

#include "gui/tracks/moc_track_item.cxx"
