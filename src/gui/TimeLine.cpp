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
#include <QLayout>
#include <QMouseEvent>
#include <QPainter>
#include <QGraphicsScene>

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
	m_metricMap( _metric ),
	m_x( 0 )
{
	TimeLineScene * tls = new TimeLineScene( this );
	setScene( tls );

	setAttribute( Qt::WA_OpaquePaintEvent, true );
	setFrameStyle( QFrame::NoFrame );
	setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	setAlignment( Qt::AlignLeft | Qt::AlignTop );
	//setAutoFillBackground();//setAutoFillBackground();
	updateView( 0, m_metricMap.length() );
}



TimeLine::~TimeLine()
{
}



void TimeLine::scroll ( int dx, int dy )
{
	m_x += dx;
	QWidget::scroll( dx, dy );
}


void TimeLine::updateView( f_cnt_t _begin, f_cnt_t _end )
{	
	// TODO: Perhaps optimize
	m_meats = m_metricMap.meats(0, m_metricMap.length());
	update();
}


bool meatPredicate(const MetricBeat & _l, const MetricBeat & _r)
{
	return _l.frame < _r.frame;
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
	_p->drawLine( 0, RowHeight - 1, width() - 1, RowHeight - 1);

	// Draw Seconds Bar
	// Latest 10-second time before the region

	//pixels * second
	//----------------------------
	//pixels
	int sec = _rect.left() / (PixelsPerSecond * SecStep);
	sec *= SecStep;
	int x;
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
	while( x < width() );


	// Find the first visible beat, then go back two
	MetricBeat beatToFind = MetricBeat( 0, 0,
			(- m_x) * FramesPerPixel,
			Tempo(0), Meter(0,0) );

	MeatList::iterator i = qLowerBound(
			m_meats.begin(), m_meats.end(), beatToFind, meatPredicate );
	if( i != m_meats.begin() )
	{
		i--;
		if( i != m_meats.begin() )
		{
			i--;
			if( i != m_meats.begin() )
			{
				i--;
			}
		}
	}

	x = 0;

	Tempo lastTempo(i->tempo);
	Meter lastMeter(i->meter);

	while( i != m_meats.end() && x <= _rect.right() )
	{
		MetricBeat b = *i;
		x = m_x + ( b.frame / FramesPerPixel );

		//printf("%d:%d:  %d,  %.2f bpm, %d / %d \n", b.bar, b.beat, b.frame, b.tempo.bpm(), b.meter.beatsPerBar(), b.meter.noteType());

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
