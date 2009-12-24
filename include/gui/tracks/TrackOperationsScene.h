/*
 * TrackOperationsScene.h - A GraphicsScene used in the song editor
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



#ifndef _TRACK_OPERATIONS_SCENE_H
#define _TRACK_OPERATIONS_SCENE_H

#include <QtCore/QVector>
#include <QtGui/QGraphicsScene>

#include "Track.h"

class TrackContainer;
class QGraphicsItem;

class TrackOperationsScene : public QGraphicsScene
{
	Q_OBJECT

public:
	const static float DEFAULT_CELL_HEIGHT = 32;
	
	TrackOperationsScene( QObject * parent, TrackContainer * _tc );
	virtual ~TrackOperationsScene();


	TrackContainer * model()
	{
		return( m_trackContainer );
	}


	const TrackContainer * model() const
	{
		return( m_trackContainer );
	}


	virtual QString nodeName() const
	{
		return( "trackOperationsScene" );
	}

private slots:

	void addTrack( Track * _t );
	void removeTrack( Track * _t );


protected:
	void scrollContentsBy( int _dx, int _dy );

	midiTime m_currentPosition;
	TrackContainer * m_trackContainer;
	float m_ppt;

	QMap<Track*, QGraphicsItem*> m_operationItems;

} ;

#endif
