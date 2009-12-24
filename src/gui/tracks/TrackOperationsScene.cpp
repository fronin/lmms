/*
 * TrackOperationsScene.cpp - A GraphicsScene used in the song editor
 * for displaying TrackOperationItems.
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


#include <QGraphicsEllipseItem>

#include "gui/tracks/TrackOperationsScene.h"
#include "TrackContainer.h"

TrackOperationsScene::TrackOperationsScene( 
		QObject * parent, TrackContainer * _tc ) :
	QGraphicsScene( parent ),
	m_trackContainer( _tc )
{
	connect( m_trackContainer, SIGNAL( trackAdded( Track * ) ),
			 this, SLOT( addTrack( Track * ) ),
	         Qt::QueuedConnection );

	connect( m_trackContainer, SIGNAL( trackRemoved( Track * ) ),
			 this, SLOT( removeTrack( Track * ) ) );
}



TrackOperationsScene::~TrackOperationsScene()
{

}



void TrackOperationsScene::addTrack( Track * _t )
{
	// TODO: Create a TrackOperations Dealie
	QGraphicsEllipseItem * oper = new QGraphicsEllipseItem(
		0,
		m_operationItems.size() * DEFAULT_CELL_HEIGHT,
		300,
		DEFAULT_CELL_HEIGHT );
	oper->setBrush( QColor(255, 0, 0) );

	printf("Adding TRACK TO OPERSCENE\n"
	       "       (%f %f %f %f)\n",
	       oper->rect().x(),
	       oper->rect().y(),
	       oper->rect().width(),
	       oper->rect().height());
	m_operationItems.insert( _t, oper );
	addItem( oper );
}



void TrackOperationsScene::removeTrack( Track * _t )
{
	QMap<Track*, QGraphicsItem*>::iterator i = m_operationItems.find( _t );

	if( i != m_operationItems.end() && i.key() == _t )
	{
		QGraphicsItem * item = i.value();
		qreal h = item->boundingRect().height();

		i = m_operationItems.erase(i);
		delete item;

		// Now move everything after back up
		while( i != m_operationItems.end() )
		{
			(*i)->setY( (*i)->y() - h );
			++i;
		}
	}
}


#include "gui/tracks/moc_TrackOperationsScene.cxx"

