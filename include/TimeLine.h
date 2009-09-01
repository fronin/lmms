/*
 * Timeline.h - class TimeLine, representing a time-line with position marker
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

#ifndef _TIMELINE_H_TNG
#define _TIMELINE_H_TNG

#include <QtGui/QWidget>

#include "lmms_basics.h"
#include "MetricMap.h"  // For MeatList

class QPixmap;


class TimeLine : public QWidget
{
	Q_OBJECT
public:

	TimeLine( QWidget * _parent, MetricMap & _metric );
	virtual ~TimeLine();
	void scroll( int _dx, int _dy );

public slots:
	// TODO: use MV instead?
	void updateView( f_cnt_t _begin, f_cnt_t _end );

protected:
	virtual void paintEvent( QPaintEvent * _pe );
	virtual void mousePressEvent( QMouseEvent * _me );
	virtual void mouseMoveEvent( QMouseEvent * _me );
	virtual void mouseReleaseEvent( QMouseEvent * _me );
/*
	void paintTempoLine( QPaintEvent * _pe, int _height );
	void paintLine( QPaintEvent * _pe, int _height );
	void paintTempoLine( QPaintEvent * _pe, int _height );
	void paintTempoLine( QPaintEvent * _pe, int _height );
*/


	MetricMap & m_metricMap;
	MeatList m_meats;
	int m_x;

} ;


#endif
