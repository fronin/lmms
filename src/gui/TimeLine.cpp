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
	//setAutoFillBackground();
}



void TimeLine::scroll ( int dx, int dy )
{
	m_x += dx;
	QWidget::scroll( dx, dy );
}




TimeLine::~TimeLine()
{
}



void TimeLine::paintEvent ( QPaintEvent * event )
{
	printf( "Redrawing in [%d, %d, %d, %d]  (%d)\n",
			event->rect().x(),
			event->rect().y(),
			event->rect().width(),
			event->rect().height(),
			m_x);
	int color = rand();
	QBrush brush = QBrush( QColor(
			color & 0xFF,
			(color >> 8) & 0xFF,
			(color >> 16) & 0xFF ) );
	QPainter p( this );
	p.fillRect( event->rect(), brush );
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
