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
#include "gui/tracks/TrackOperationsScene.h"


class CornerWidget : public QWidget
{
	//Q_OBJECT
public:
	CornerWidget( QWidget * _parent ) :
		QWidget( _parent )
	{
		setAttribute( Qt::WA_OpaquePaintEvent, true );
	}

	void paintEvent ( QPaintEvent * _pe )
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
		p.drawText( 0, RowHeight*0 - 2, width() - 5, RowHeight,
				Qt::AlignRight, "Time" );
		p.drawText( 0, RowHeight*1 - 2, width() - 5, RowHeight,
				Qt::AlignRight, "Bar,Beat" );
		p.drawText( 0, RowHeight*2 - 2, width() - 5, RowHeight,
				Qt::AlignRight, "Tempo" );
		p.drawText( 0, RowHeight*3 - 2, width() - 5, RowHeight,
				Qt::AlignRight, "Meter" );
	}
};



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

	m_left = new QGraphicsView( this );

	TrackOperationsScene * operationsScene =
		new TrackOperationsScene( m_left, _song );
	
	m_left->setScene( operationsScene );
	m_left->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	m_left->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	m_left->setStyleSheet( "QWidget { border: none }" );
	m_left->show();

	setRenderHints( QPainter::Antialiasing | QPainter::SmoothPixmapTransform );
	setAttribute( Qt::WA_OpaquePaintEvent, true );
	setDragMode( QGraphicsView::RubberBandDrag );
	setAlignment( Qt::AlignLeft | Qt::AlignTop );
	int x;

	// Just some scrolling fun
	m_scene->setSceneRect( 0, 0, 5000, 5000 );
	m_left->setSceneRect( 0, 0, 300, 5000 );

	// Connections for mouse-wheel on the margins
	connect( m_left->verticalScrollBar(), SIGNAL( valueChanged(int) ),
		 verticalScrollBar(), SLOT( setValue(int) ) );

	connect( m_timeLine->horizontalScrollBar(), SIGNAL( valueChanged(int) ),
		 horizontalScrollBar(), SLOT( setValue(int) ) );

	updateView( 0, m_song->metricMap().length() );
}


void SongEditor::resizeEvent( QResizeEvent * _re )
{
	QGraphicsView::resizeEvent( _re );

	QRect cg = contentsRect();
	QRect vg = viewport()->geometry();

	m_timeLine->setGeometry( vg.left(), vg.top() - 64, vg.width(), 64 );
	m_corner->setGeometry( cg.left(), cg.top(), vg.left() - cg.left(), 64 );
	m_left->setGeometry( cg.left(), vg.top(), 300, vg.height() );
}


void SongEditor::scrollContentsBy( int _dx, int _dy )
{
	QGraphicsView::scrollContentsBy( _dx, _dy );
	// Avoid recursion
	if( _dx != 0 ) {
		m_timeLine->horizontalScrollBar()->setValue(
				horizontalScrollBar()->value() );
	}
	if( _dy != 0 ) {
		m_left->verticalScrollBar()->setValue(
				verticalScrollBar()->value() );
	}
}



void SongEditor::updateView( f_cnt_t _begin, f_cnt_t _end )
{
	// TODO: Perhaps optimize
	m_meats = m_song->metricMap().meats( 0, m_song->metricMap().length() );
	update();
}



void SongEditor::drawBackground ( QPainter * _p, const QRectF & _rect )
{
	const int FramesPerPixel = 800;
	const double PixelsPerSecond = 44100.0f / FramesPerPixel;
	int x;

	// Init pens
	QPen gridPen( QColor(128, 128, 128), 1 );
	_p->fillRect( _rect, QColor(224, 224, 224) );

	// Find the first visible beat, then go back two
	MetricBeat beatToFind = MetricBeat( 0, 0,
			(- _rect.left()) * FramesPerPixel,
			Tempo(0), Meter(0,0) );

	MeatList::iterator i = qLowerBound(
			m_meats.begin(), m_meats.end(),
			beatToFind, MetricBeat::ascendingPredicate );
	if( i != m_meats.begin() )
	{
		i--;
		if( i != m_meats.begin() )
		{
			i--;
		}
	}

	x = 0;
	while( i != m_meats.end() && x <= _rect.right() )
	{
		MetricBeat b = *i;
		x = b.frame / FramesPerPixel;

		// Grid Line
		_p->setPen(gridPen);
		_p->drawLine( x, _rect.top(), x, _rect.bottom() );

		++i;
	}

}


#include "gui/moc_SongEditor.cxx"
