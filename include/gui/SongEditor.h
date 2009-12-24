/*
 * SongEditor.h - The main interface for editing a song
 *
 * Copyright (c) 2009 Paul Giblock <pgib/at/users/sourceforge/net>
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

#ifndef _SONG_EDITOR_H
#define _SONG_EDITOR_H

#include <QtGui/QGraphicsView>

#include "gui/SongEditor.h"
#include "gui/tracks/track_container_scene.h"

class Song;
class TrackContainerScene;
class TimeLine;
class QGraphicsView;


class SongEditor : public QGraphicsView
{
	Q_OBJECT
public:
	SongEditor( Song * _song, QWidget * _parent = 0 );

	inline Song * song() const
	{
		return m_song;
	}

protected:
	void resizeEvent( QResizeEvent * _re );
	void scrollContentsBy( int _dx, int _dy );
	void drawBackground ( QPainter * painter, const QRectF & rect );

	Song * m_song;
	TrackContainerScene * m_scene;

	QWidget * m_corner;
	QGraphicsView * m_left;
	TimeLine * m_timeLine;

};

#endif
