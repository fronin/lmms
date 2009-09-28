/*
 * SongEditorWindow.h -
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

#ifndef _SONG_EDITOR_WINDOW_H
#define _SONG_EDITOR_WINDOW_H

#include <QtGui/QMainWindow>

class SongEditor;


class SongEditorWindow : public QMainWindow
{
	Q_OBJECT
public:
	SongEditorWindow( QWidget * _parent = 0 );

	virtual ~SongEditorWindow();

protected slots:
	void newSampleTrack();

protected:
	SongEditor * m_editor;

	// Actions
	QAction * m_newBbTrackAction;
};

#endif
