/*
 * engine.cpp - implementation of LMMS' engine-system
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

#include <QtCore/QDir>
#include <QtXml/QDomDocument>

#include "engine.h"
#include "bb_track_container.h"
#include "config_mgr.h"
#include "ControllerRackView.h"
#include "FxMixer.h"
#include "FxMixerView.h"
#include "InstrumentTrack.h"
#include "ladspa_2_lmms.h"
#include "MainWindow.h"
#include "mixer.h"
#include "Pattern.h"
#include "project_notes.h"
#include "Plugin.h"
#include "Song.h"
#include "Sequencer.h"
#include "MidiControlListener.h"

#include "ResourceDB.h"
#include "LocalResourceProvider.h"
#include "WebResourceProvider.h"
#include "UnifiedResourceProvider.h"


bool engine::s_hasGUI = true;
bool engine::s_suppressMessages = false;
float engine::s_framesPerTick;
mixer * engine::s_mixer = NULL;
FxMixer * engine::s_fxMixer = NULL;
FxMixerView * engine::s_fxMixerView = NULL;
MainWindow * engine::s_mainWindow = NULL;
bbTrackContainer * engine::s_bbTrackContainer = NULL;
Sequencer * engine::s_sequencer = NULL;
Song * engine::s_song = NULL;
projectNotes * engine::s_projectNotes = NULL;
ResourceDB * engine::s_workingDirResourceDB = NULL;
ResourceDB * engine::s_webResourceDB = NULL;
ResourceDB * engine::s_mergedResourceDB = NULL;
ladspa2LMMS * engine::s_ladspaManager = NULL;
DummyTrackContainer * engine::s_dummyTC = NULL;
ControllerRackView * engine::s_controllerRackView = NULL;
MidiControlListener * engine::s_midiControlListener = NULL;
QMap<QString, QString> engine::s_pluginFileHandling;
LmmsStyle * engine::s_lmmsStyle = NULL;
bool engine::s_devMode = false; 

tick_t midiTime::s_ticksPerTact = 192;


void engine::init( const bool _has_gui )
{
	s_hasGUI = _has_gui;

	initPluginFileHandling();

	s_mixer = new mixer;
	s_sequencer = new Sequencer();
	s_song = new Song();


	// init resource framework
	s_workingDirResourceDB =
		( new LocalResourceProvider( ResourceItem::BaseWorkingDir,
													QString() ) )->database();
	ResourceDB * shippedResourceDB =
		( new LocalResourceProvider( ResourceItem::BaseDataDir,
													QString() ) )->database();
	s_webResourceDB =
		( new WebResourceProvider( "http://lmms.sourceforge.net" ) )
																->database();

	UnifiedResourceProvider * unifiedResource = new UnifiedResourceProvider;
	unifiedResource->addDatabase( s_workingDirResourceDB );
	unifiedResource->addDatabase( shippedResourceDB );
	unifiedResource->addDatabase( s_webResourceDB );

	s_mergedResourceDB = unifiedResource->database();


	s_fxMixer = new FxMixer;
	s_bbTrackContainer = new bbTrackContainer;

	s_ladspaManager = new ladspa2LMMS;

	s_mixer->initDevices();

	s_midiControlListener = new MidiControlListener();

	if( s_hasGUI )
	{
		s_mainWindow = new MainWindow;
		s_fxMixerView = new FxMixerView;
		s_controllerRackView = new ControllerRackView;
		s_projectNotes = new projectNotes;

		s_mainWindow->finalize();
	}

	s_dummyTC = new DummyTrackContainer;

	s_mixer->startProcessing();
}




void engine::destroy()
{
	configManager::inst()->saveConfigFile();

	s_mixer->stopProcessing();

	delete s_projectNotes;
	s_projectNotes = NULL;

	delete s_fxMixerView;
	s_fxMixerView = NULL;

	//InstrumentTrackView::cleanupWindowPool();

	s_song->clearProject();
	delete s_bbTrackContainer;
	s_bbTrackContainer = NULL;
	delete s_dummyTC;
	s_dummyTC = NULL;

	delete s_mixer;
	s_mixer = NULL;
	delete s_fxMixer;
	s_fxMixer = NULL;

	delete s_ladspaManager;

	s_mainWindow = NULL;

	delete s_song;
	s_song = NULL;

	delete s_mergedResourceDB->provider();
	s_mergedResourceDB = NULL;

	delete configManager::inst();
}




void engine::initPluginFileHandling()
{
	Plugin::DescriptorList pluginDescriptors;
	Plugin::getDescriptorsOfAvailPlugins( pluginDescriptors );
	for( Plugin::DescriptorList::ConstIterator it = pluginDescriptors.begin();
										it != pluginDescriptors.end(); ++it )
	{
		if( it->type == Plugin::Instrument )
		{
			const char * * suppFileTypes = it->supportedFileTypes;
			while( suppFileTypes && *suppFileTypes != NULL )
			{
				s_pluginFileHandling[*suppFileTypes] = it->name;
				++suppFileTypes;
			}
		}
	}
}




void engine::loadConfiguration( QDomDocument & doc )
{
	// must be a call to a static method as the engine
	// is not yet created and initialized and 
	// s_midiControlListener is still NULL.
	MidiControlListener::rememberConfiguration( doc );
}




void engine::saveConfiguration( QDomDocument & doc )
{
	s_midiControlListener->saveConfiguration( doc );
}


