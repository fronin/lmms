/*
 * engine.h - engine-system of LMMS
 *
 * Copyright (c) 2006-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef _ENGINE_H
#define _ENGINE_H

#include "lmmsconfig.h"

#include <QtCore/QMap>

#include "export.h"
#include "lmms_style.h"

class AutomationRecorder;
class bbTrackContainer;
class DummyTrackContainer;
class FxMixer;
class FxMixerView;
class MainWindow;
class mixer;
class projectNotes;
class ResourceDB;
class Sequencer;
class Song;
class ladspa2LMMS;
class ControllerRackView;
class MidiControlListener;
class QDomDocument;


class EXPORT engine
{
public:
	static void init( const bool _has_gui = true );
	static void destroy();

	static bool hasGUI()
	{
		return s_hasGUI;
	}

	static void setSuppressMessages( bool _on )
	{
		s_suppressMessages = _on;
	}

	static bool suppressMessages()
	{
		return !s_hasGUI || s_suppressMessages;
	}

	static void setDevMode( bool _on )
	{
		s_devMode = _on;
	}

	static bool devMode()
	{
		return s_devMode;
	}

	// core
	static mixer * getMixer()
	{
		return s_mixer;
	}

	static FxMixer * fxMixer()
	{
		return s_fxMixer;
	}

	static Song * song( void )
	{
		return s_song;
	}

	static Sequencer * sequencer( void )
	{
		return s_sequencer;
	}

	static bbTrackContainer * getBBTrackContainer()
	{
		return s_bbTrackContainer;
	}

	static ResourceDB * workingDirResourceDB()
	{
		return s_workingDirResourceDB;
	}

	static ResourceDB * webResourceDB()
	{
		return s_webResourceDB;
	}

	static ResourceDB * mergedResourceDB()
	{
		return s_mergedResourceDB;
	}

	// GUI
	static MainWindow * mainWindow()
	{
		return s_mainWindow;
	}

	static FxMixerView * fxMixerView()
	{
		return s_fxMixerView;
	}

	static projectNotes * getProjectNotes()
	{
		return s_projectNotes;
	}

	static AutomationRecorder * getAutomationRecorder()
	{
		return s_automationRecorder;
	}

	static ladspa2LMMS * getLADSPAManager()
	{
		return s_ladspaManager;
	}

	static DummyTrackContainer * dummyTrackContainer()
	{
		return s_dummyTC;
	}

	static ControllerRackView * getControllerRackView()
	{
		return s_controllerRackView;
	}

	static float framesPerTick()
	{
		return s_framesPerTick;
	}
	static void updateFramesPerTick();

	static const QMap<QString, QString> & pluginFileHandling()
	{
		return s_pluginFileHandling;
	}

	static void setLmmsStyle( LmmsStyle * _style )
	{
		s_lmmsStyle = _style;
	}

	static LmmsStyle * getLmmsStyle()
	{
		return s_lmmsStyle;
	}
	
	static void saveConfiguration( QDomDocument & doc );

	static void loadConfiguration( QDomDocument & doc );

	static MidiControlListener * getMidiControlListener()
	{
		return s_midiControlListener;
	}

private:
	static bool s_hasGUI;
	static bool s_suppressMessages;
	static bool s_devMode;
	static float s_framesPerTick;

	// core
	static mixer * s_mixer;
	static Sequencer * s_sequencer;
	static Song * s_song;
	static FxMixer * s_fxMixer;
	static ResourceDB * s_workingDirResourceDB;
	static ResourceDB * s_webResourceDB;
	static ResourceDB * s_mergedResourceDB;
	static bbTrackContainer * s_bbTrackContainer;
	static DummyTrackContainer * s_dummyTC;
	static ControllerRackView * s_controllerRackView;
	static MidiControlListener * s_midiControlListener;

	// GUI
	static MainWindow * s_mainWindow;
	static FxMixerView * s_fxMixerView;
	static AutomationRecorder * s_automationRecorder;
	static projectNotes * s_projectNotes;
	static ladspa2LMMS * s_ladspaManager;

	static LmmsStyle * s_lmmsStyle;

	static QMap<QString, QString> s_pluginFileHandling;

	static void initPluginFileHandling();

} ;




#endif
