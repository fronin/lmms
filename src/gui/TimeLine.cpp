/*
 * timeline.cpp - class timeLine, representing a time-line with position marker
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


#include <QtGui/QApplication>
#include <QtGui/QLayout>
#include <QtGui/QMouseEvent>
#include <QtGui/QPainter>

#include "TimeLine.h"
#include "MetricMap.h"
#include "embed.h"
#include "engine.h"
#include "lmms_style.h"
#include "MainWindow.h"
#include "nstate_button.h"
#include "templates.h"
#include "text_float.h"


TimeLine::TimeLine( QWidget * _parent, MetricMap & _metric ) :
	QWidget( _parent ),
	m_metricMap( _metric ),
	m_x( 0 )
{
	setAttribute( Qt::WA_OpaquePaintEvent, true );
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

void TimeLine::paintEvent ( QPaintEvent * _pe )
{
	const int PixelsPerBeat = 16;
	const int FramesPerPixel = 800;

	MetricBeat beatToFind = MetricBeat( 0, 0,
			(- m_x) * FramesPerPixel,
			Tempo(0), Meter(0,0) );

	MeatList::iterator i = qLowerBound(
			m_meats.begin(), m_meats.end(), beatToFind, meatPredicate );
	MetricBeat fb = *i;
	int x = 0;

	QPainter p( this );
	QPen barPen( QColor(255, 0, 0), 2 );
	QPen beatPen( QColor(0, 0, 255), 1 );
	p.fillRect( _pe->rect(), QColor(255, 255, 255) );
	p.setPen( beatPen );

	if( i != m_meats.begin() )
	{
		i--;
		if( i != m_meats.begin() )
		{
			i--;
		}
	}
	Tempo lastTempo = i->tempo;
	Meter lastMeter = i->meter;

	while( i != m_meats.end() ) // && x <= _pe->rect().right() )
	{
		MetricBeat b = *i;
		printf("%d:%d:  %d,  %.2f bpm, %d / %d \n", b.bar, b.beat, b.frame, b.tempo.bpm(), b.meter.beatsPerBar(), b.meter.noteType());
		x = m_x + ( b.frame / FramesPerPixel );
		if( b.beat == 0 )
		{
			p.setPen( barPen );
			p.drawLine( x, 0, x, height() - 1 );
			p.setPen( beatPen );
		}
		else
		{
			p.drawLine( x, 0, x, height() / 2 );
		}
		if( lastTempo.bpm() != i->tempo.bpm() )
		{
			p.drawText( x+1, 16, QString::number( i->tempo.bpm() ) );
		}
		if( lastMeter.beatsPerBar() != i->meter.beatsPerBar() ||
			lastMeter.noteType() != i->meter.noteType() )
		{
			p.drawText( x+1, 32, QString::number( i->meter.beatsPerBar() ) + "/" + QString::number( i->meter.noteType() ) );
		}

		lastTempo = i->tempo;
		lastMeter = i->meter;
		++i;
	}
}


void TimeLine::mousePressEvent( QMouseEvent * _me )
{
}



void TimeLine::mouseMoveEvent( QMouseEvent * _me )
{
}



void TimeLine::mouseReleaseEvent( QMouseEvent * _me )
{
}



#include "moc_TimeLine.cxx"
