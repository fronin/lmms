/*
 * Timeline.h - class TimeLine, representing a time-line with position marker
 *
 * Copyright (c) 2009 Paul Giblock
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

#ifndef _TIMELINE_H_TNG
#define _TIMELINE_H_TNG

#include <QtGui/QWidget>



class QPixmap;
class textFloat;
class MetricMap;


class TimeLine : public QWidget
{
	Q_OBJECT
public:

	TimeLine( QWidget * _parent, MetricMap * _metric );
	virtual ~TimeLine();

public slots:

protected:
	virtual void paintEvent( QPaintEvent * _pe );
	virtual void mousePressEvent( QMouseEvent * _me );
	virtual void mouseMoveEvent( QMouseEvent * _me );
	virtual void mouseReleaseEvent( QMouseEvent * _me );


	MetricMap * m_metricMap;

} ;


#endif
