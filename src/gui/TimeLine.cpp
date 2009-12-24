/*
 * timeline.cpp - class timeLine, represents a time-line with position marker
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


#include <QApplication>
#include <QGraphicsScene>
#include <QLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QScrollBar>

#include "TimeLine.h"
#include "MetricMap.h"
#include "embed.h"
#include "engine.h"
#include "lmms_style.h"
#include "MainWindow.h"
#include "nstate_button.h"
#include "templates.h"
#include "text_float.h"

class TimeLineScene : public QGraphicsScene
{
public:
	TimeLineScene( QObject * _parent ) :
			QGraphicsScene( _parent )
	{
		setSceneRect( 0, 0, 5000, 64 );
	}
};


TimeLine::TimeLine( QWidget * _parent, MetricMap & _metric ) :
	QGraphicsView( _parent ),
	m_metricMap( _metric )
{
	TimeLineScene * tls = new TimeLineScene( this );
	setScene( tls );

	setRenderHints( QPainter::Antialiasing | QPainter::SmoothPixmapTransform );
	setAttribute( Qt::WA_OpaquePaintEvent, true );
	setFrameStyle( QFrame::NoFrame );
	setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	setAlignment( Qt::AlignLeft | Qt::AlignTop );
	updateView( 0, m_metricMap.length() );
}



TimeLine::~TimeLine()
{
}



void TimeLine::updateView( f_cnt_t _begin, f_cnt_t _end )
{
	// TODO: Perhaps optimize
	m_meats = m_metricMap.meats(0, m_metricMap.length());
	update();
}



void TimeLine::drawBackground ( QPainter * _p, const QRectF & _rect )
{
	const int FramesPerPixel = 800;
	const int SecStep = 5;
	const int RowHeight = 16;
	const double PixelsPerSecond = 44100.0f / FramesPerPixel;

	// Init pens
	QPen gridPen( QColor(192, 192, 192), 1 );
	QPen textPen( QColor(0, 0, 0), 1 );
	QPen barPen( QColor(255, 0, 0), 2 );
	QPen beatPen( QColor(0, 0, 255), 2 );
	QPen secPen( QColor(0, 255, 0), 2 );
	_p->fillRect( _rect, QColor(255, 255, 255) );

	// Draw splitter
	_p->setPen( gridPen );
	_p->drawLine( _rect.left(), RowHeight - 1, _rect.right(), RowHeight - 1);

	// Draw Seconds Bar
	// Latest 10-second time before the region

	//pixels * second
	//----------------------------
	//pixels
	int sec = (int)( _rect.left() / (PixelsPerSecond * SecStep) );
	sec *= SecStep;
	float x;
	do
	{
		x = (sec * PixelsPerSecond);
		_p->setPen( secPen );
		_p->drawLine( x, 0, x, RowHeight - 1);
		_p->setPen( textPen );
		_p->drawText( x + 2, RowHeight - 2,
				QString("00:00:") + QString::number(sec).rightJustified(2, '0') );
		sec += SecStep;
	}
	while( x < _rect.right() );


	// Find the first visible beat, then go back one
	MetricBeat beatToFind = MetricBeat( 0, 0,
			(- _rect.left()) * FramesPerPixel,
			Tempo(0), Meter(0,0) );

	MeatList::iterator i = qLowerBound(
			m_meats.begin(), m_meats.end(),
			beatToFind, MetricBeat::ascendingPredicate );
	if( i != m_meats.begin() )
	{
		i--;
	}

	Tempo lastTempo(i->tempo);
	Meter lastMeter(i->meter);

	x = 0;
	while( i != m_meats.end() && x <= _rect.right() )
	{
		MetricBeat b = *i;
		x = b.frame / FramesPerPixel;

		// Grid Line
		_p->setPen(gridPen);
		_p->drawLine( x, RowHeight, x, height()-1 );

		if( b.beat == 0 )
		{
			_p->setPen( barPen );
			_p->drawLine( x, RowHeight, x, RowHeight*2 );
			_p->setPen( textPen );
			_p->drawText( x+2, RowHeight*2 - 2, QString::number( i->bar + 1 ) );
		}
		else
		{
			_p->setPen( beatPen );
			_p->drawLine( x, RowHeight, x, RowHeight*1.5f );
		}
		if( lastTempo.bpm() != i->tempo.bpm() )
		{
			_p->setPen( textPen );
			_p->drawText( x+1, RowHeight*3 - 2, QString::number( i->tempo.bpm() ) );
		}
		if( lastMeter.beatsPerBar() != i->meter.beatsPerBar() ||
			lastMeter.noteType() != i->meter.noteType() )
		{
			_p->setPen( textPen );
			_p->drawText( x+1, RowHeight*4 - 2, QString::number( i->meter.beatsPerBar() ) + "/" + QString::number( i->meter.noteType() ) );
		}

		lastTempo = i->tempo;
		lastMeter = i->meter;
		++i;
	}

}

/*
void TimeLine::mousePressEvent( QMouseEvent * _me )
{
}



void TimeLine::mouseMoveEvent( QMouseEvent * _me )
{
}



void TimeLine::mouseReleaseEvent( QMouseEvent * _me )
{
}
*/


#include "moc_TimeLine.cxx"
