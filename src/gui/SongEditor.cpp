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
#include <QScrollBar>

#include "Song.h"
#include "TimeLine.h"

#include "gui/SongEditor.h"
#include "gui/tracks/track_container_scene.h"


class CornerWidget : public QWidget
{
	//Q_OBJECT
public:
	CornerWidget( QWidget * _parent ) :
		QWidget( _parent )
	{
		setAttribute( Qt::WA_OpaquePaintEvent, true );
	}

	void paintEvent ( QPaintEvent * _pe );
};


void CornerWidget::paintEvent ( QPaintEvent * _pe )
	{
		const int RowHeight = 16;

		// Init pens
		QPainter p( this );
		QPen gridPen( QColor(192, 192, 192), 1 );
		QPen textPen( QColor(0, 0, 0), 1 );
		p.fillRect( rect(), QColor(255, 255, 255) );

		// Draw splitter
		p.setPen( gridPen );
		p.drawLine( 0, RowHeight - 1, width() - 1, RowHeight - 1);
		p.drawLine( width() - 1, 0, width() - 1, height() - 1);

		p.setPen( textPen );
		p.drawText( 0, RowHeight*0 - 2, width() - 5, RowHeight, Qt::AlignRight, "Time" );
		p.drawText( 0, RowHeight*1 - 2, width() - 5, RowHeight, Qt::AlignRight, "Bar,Beat" );
		p.drawText( 0, RowHeight*2 - 2, width() - 5, RowHeight, Qt::AlignRight, "Tempo" );
		p.drawText( 0, RowHeight*3 - 2, width() - 5, RowHeight, Qt::AlignRight, "Meter" );
	}
//};

SongEditor::SongEditor( Song * _song, QWidget * _parent ) :
		QGraphicsView( NULL, _parent ),
		m_song( _song )
{
	m_scene = new TrackContainerScene( this, m_song ),
	setScene( m_scene );
	setViewportMargins( 300, 64, 0, 0 );

	m_timeLine = new TimeLine( this,  m_song->metricMap() );
	m_timeLine->show();

	m_corner = new CornerWidget( this );
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

	m_timeLine->setGeometry( vg.left(), vg.top() - 64, vg.width(), 64 );
	m_left->setGeometry( cg.left(), vg.top(), vg.left() - cg.left(), vg.height());
	m_corner->setGeometry( cg.left(), cg.top(), vg.left() - cg.left(), 64 );
}


void SongEditor::scrollContentsBy( int _dx, int _dy )
{
	QGraphicsView::scrollContentsBy( _dx, _dy );
	// I *think* this is a good thing to do...
	//m_timeLine->setSceneRect( m_timeLine->sceneRect().translated( -_dx, 0 ) );
	m_timeLine->horizontalScrollBar()->setValue(
			horizontalScrollBar()->value() );
	//m_timeLine->translate( _dx, 0 );
	//m_timeLine->scroll( _dx, 0 );
	m_left->scroll( 0, _dy );
}

void SongEditor::drawBackground ( QPainter * painter, const QRectF & rect )
{
	int clr = rand();

	painter->fillRect( rect, QColor(clr&0xff, (clr>>8)&0xff, (clr>>16)&0xff) );

}

#include "gui/moc_SongEditor.cxx"
