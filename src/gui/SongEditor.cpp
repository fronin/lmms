/*
 * SongEditor.cpp - The main interface for editing a song
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

#include <QtGui/QGraphicsView>
#include <QtGui/QLabel>

#include "Song.h"
#include "TimeLine.h"

#include "gui/SongEditor.h"
#include "gui/tracks/track_container_scene.h"


SongEditor::SongEditor( Song * _song, QWidget * _parent ) :
		QGraphicsView( NULL, _parent ),
		m_song( _song )
{
	m_scene = new TrackContainerScene( this, m_song ),
	setScene( m_scene );
	setViewportMargins( 300, 48, 0, 0 );

	m_timeLine = new TimeLine( this,  m_song->metricMap() );
	m_timeLine->show();

	m_corner = new QLabel( "I am the:\nCORNER!!", this );
	m_corner->setStyleSheet("QLabel { background: blue }");
	m_corner->show();

	m_left = new QLabel( "Left\nLeft\nLeft\nLeft", this );
	m_left->setStyleSheet("QLabel { background: red }");
	m_left->show();

	QPixmap * sceneBg = new QPixmap( 16*4*2, 32 );
	QPainter * sceneBgPainter = new QPainter( sceneBg );
	engine::getLmmsStyle()->drawTrackContentBackground(
			sceneBgPainter,
			QSize( 16*4, 32 ),
			16 );

	setRenderHints( QPainter::Antialiasing | QPainter::SmoothPixmapTransform );
	setDragMode( QGraphicsView::RubberBandDrag );
	setAlignment( Qt::AlignLeft | Qt::AlignTop );
	setBackgroundBrush( QBrush(*sceneBg) );

	// Just some scrolling fun
	m_scene->setSceneRect( 0, 0, 5000, 5000 );
}


void SongEditor::resizeEvent( QResizeEvent * _re )
{
	QGraphicsView::resizeEvent( _re );

	QRect cg = contentsRect();
	QRect vg = viewport()->geometry();

	m_timeLine->setGeometry( vg.left(), vg.top() - 48, vg.width(), 48 );
	m_left->setGeometry( cg.left(), vg.top(), vg.left() - cg.left(), vg.height());
	m_corner->setGeometry( cg.left(), cg.top(), vg.left() - cg.left(), 48 );
}


void SongEditor::scrollContentsBy( int _dx, int _dy )
{
	QGraphicsView::scrollContentsBy( _dx, _dy );
	m_timeLine->scroll( _dx, 0 );
	m_left->scroll( 0, _dy );
}

#include "gui/moc_SongEditor.cxx"
