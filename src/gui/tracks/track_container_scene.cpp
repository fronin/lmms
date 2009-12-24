/*
 * track_container_scene.cpp - A TrackContainer is represented in the
 * SongEditor as the GraphicsScene.  This is the implementation.
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


#include <QKeyEvent>
#include <QGraphicsSceneMouseEvent>

#include <stdio.h>

#include "gui/tracks/track_container_scene.h"
#include "gui/tracks/track_content_object_item.h"
#include "gui/tracks/track_item.h"
#include "TrackContainer.h"

TrackContainerScene::TrackContainerScene( QObject * parent, TrackContainer * _tc ) :
	QGraphicsScene( parent ),
	m_trackContainer( _tc ),
	m_ppt( 16 )
{
	connect( m_trackContainer, SIGNAL( trackAdded( Track * ) ),
			 this, SLOT( addTrack( Track * ) ),
	         Qt::QueuedConnection );

	connect( m_trackContainer, SIGNAL( trackRemoved( Track * ) ),
			 this, SLOT( removeTrack( Track * ) ) );
}


TrackContainerScene::~TrackContainerScene()
{

}



void TrackContainerScene::setPixelsPerTact( float _ppt )
{
	m_ppt = _ppt;
}



void TrackContainerScene::addTrack( Track * _t )
{
	TrackItem * item = new TrackItem( this, _t );
	item->setHeight( DEFAULT_CELL_HEIGHT );
	item->setY( m_trackItems.size() * DEFAULT_CELL_HEIGHT );

	m_trackItems.insert( _t, item );

	connect( _t, SIGNAL( trackSegmentAdded(TrackSegment*) ),
			 this, SLOT( addTrackSegment(TrackSegment*) ),
			 Qt::QueuedConnection );

	connect( _t, SIGNAL( trackSegmentRemoved(TrackSegment*) ),
		 this, SLOT( removeTrackSegment(TrackSegment*) ),
		 Qt::QueuedConnection );;
}


void TrackContainerScene::removeTrack( Track * _t )
{
	QMap<Track*,  TrackItem*>::iterator i = m_trackItems.find( _t );

	if( i != m_trackItems.end() && i.key() == _t )
	{
		TrackItem * item = i.value();
		qreal h = item->height();

		i = m_trackItems.erase(i);
		delete item;

		// Now move everything after back up
		while( i != m_trackItems.end() )
		{
			(*i)->setY( (*i)->y() - h );
			++i;
		}
	}
}



void TrackContainerScene::addTrackSegment( TrackSegment * _ts )
{
	QMap<Track*,  TrackItem*>::iterator i =
			m_trackItems.find( _ts->getTrack() );

	TrackItem * item = i.value();
	item->addSegment( _ts );
}



void TrackContainerScene::removeTrackSegment( TrackSegment * _ts )
{
}



void TrackContainerScene::mouseDoubleClickEvent( QGraphicsSceneMouseEvent * _me )
{
	QPointF p = _me->pos();
	QPointF sp = _me->scenePos();
	if( itemAt( sp ) != NULL )
	{
		QGraphicsScene::mousePressEvent( _me );
	}
	else
	{
		printf("clk (%.4f %.4f)  (%.4f %.4f)\n", p.x(), p.y(), sp.x(), sp.y());
		// Determine which track was hit - assumes equal height tracks
		int trackIdx = sp.y() / DEFAULT_CELL_HEIGHT;
		if( m_trackContainer->tracks().size() > trackIdx )
		{
			Track * t = m_trackContainer->tracks().at(trackIdx);
			{
				TrackSegment * seg = t->createSegment( midiTime(0, 0) );
			}
		}
	}
}


void TrackContainerScene::keyPressEvent( QKeyEvent * event )
{/*
	if( event->modifiers() == Qt::ShiftModifier )
	{
		const qreal cellWidth = TrackContainerScene::DEFAULT_CELL_WIDTH;

		if( event->key() == Qt::Key_Left )
		{
		}
		else if( event->key() == Qt::Key_Right )
		{
			QTimeLine * timeLine = new QTimeLine();

			// TODO: Cleanup the friendly references
			QList<QGraphicsItem*> selItems = selectedItems();
			for( QList<QGraphicsItem *>::iterator it = selItems.begin();
					it != selItems.end(); ++it )
			{
				TrackSegmentItem * tcoItem =
					dynamic_cast<TrackSegmentItem*>( *it );
				if( tcoItem )
				{
					qreal destPos = tcoItem->m_snapBackAnimation->posAt( 1.0 ).x();
					tcoItem->prepareSnapBackAnimation( timeLine, destPos + cellWidth );
				}
			}

			timeLine->setCurrentTime( 0.0f );
			timeLine->setDuration( 300 );
			timeLine->setCurveShape( QTimeLine::EaseInOutCurve );
			connect( timeLine, SIGNAL(finished()), timeLine, SLOT(deleteLater()));
			timeLine->start();
		}
	}
*/}



#include "gui/tracks/moc_track_container_scene.cxx"

