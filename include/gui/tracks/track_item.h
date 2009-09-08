/*
 * track_item.cpp - the base-class for Tracks in the TrackContainerScene
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



#ifndef TRACK_ITEM_H_
#define TRACK_ITEM_H_

#include <QObject>
#include <QRectF>
#include <QMap>

class TrackContainerScene;
class Track;
class TrackSegment;
class TrackSegmentItem;

class TrackItem : public QObject
{
	Q_OBJECT;
public:
	TrackItem( TrackContainerScene * _scene, Track * _track );
	virtual ~TrackItem( );

	float height();
	void setHeight( float _height );

	float y();
	void setY( float _x );

private slots:
	void addSegment( TrackSegment * _segment );
	void removeSegment( TrackSegment * _segment );

private:
	QRectF m_rect;

	TrackContainerScene * m_scene;
	Track * m_track;
	QMap<TrackSegment*, TrackSegmentItem *> m_segmentItems;

};


#endif
