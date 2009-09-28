/*
 * ResourcePreviewer.cpp - implementation of ResourcePreviewer
 *
 * Copyright (c) 2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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


#include <QtCore/QFileInfo>

#include "ResourceAction.h"
#include "ResourcePreviewer.h"
#include "ResourceFileMapper.h"
#include "ResourceItem.h"
#include "engine.h"
#include "Instrument.h"
#include "InstrumentTrack.h"
#include "mmp.h"


ResourcePreviewer::ResourcePreviewer() :
	m_previewTrackContainer(),
	m_previewTrack( NULL ),
	m_defaultSettings( multimediaProject::InstrumentTrackSettings )
{
	m_previewTrack = dynamic_cast<InstrumentTrack *>(
				Track::create( Track::InstrumentTrack,
						&m_previewTrackContainer ) );

	// save default settings so we can restore them later
	m_previewTrack->saveSettings( m_defaultSettings,
					m_defaultSettings.content() );

	// make sure a default instrument is loaded
	m_previewTrack->loadInstrument( "tripleoscillator" );
}




ResourcePreviewer::~ResourcePreviewer()
{
	delete m_previewTrack;
}




void ResourcePreviewer::preview( ResourceItem * _item )
{
	// stop any existing preview sounds
	stopPreview();

	engine::setSuppressMessages( true );

	// handle individual resource types
	bool handledItem = true;
	switch( _item->type() )
	{
		case ResourceItem::TypePreset:
			// restore default settings, in case we're going to load
			// an incomplete preset
			m_previewTrack->loadTrackSpecificSettings(
				m_defaultSettings.content().
					firstChild().toElement() );
			ResourceAction( _item ).loadPreset( m_previewTrack );
			m_previewTrack->midiPort()->setMode( MidiPort::Disabled );
			break;
		case ResourceItem::TypeSample:
		case ResourceItem::TypePluginSpecificResource:
			// restore default settings we are going to preview a
			// sample (which should be played at a default
			// instrument track)
			m_previewTrack->loadTrackSpecificSettings(
				m_defaultSettings.content().
					firstChild().toElement() );
			ResourceAction( _item ).loadByPlugin( m_previewTrack );
			break;
		default:
			handledItem = false;
			break;
	}

	// re-enable journalling
	engine::setSuppressMessages( false );

	if( handledItem )
	{
		// playback default note
		m_previewTrack->processInEvent(
			midiEvent( MidiNoteOn, 0, DefaultKey, MidiMaxVelocity ),
								midiTime() );
	}
}




void ResourcePreviewer::stopPreview()
{
	m_previewTrack->silenceAllNotes();
}




Piano * ResourcePreviewer::pianoModel()
{
	return m_previewTrack->pianoModel();
}


#include "moc_ResourcePreviewer.cxx"

/* vim: set tw=0 noexpandtab: */
