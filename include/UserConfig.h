/*
 * UserConfig.h - class UserConfig
 *
 * Copyright (c) 2009 Tobias Doerffel <tobydox/at/users/dot/sf/dot/net>
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
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#ifndef _USER_CONFIG_H
#define _USER_CONFIG_H

#include "ConfigurationObject.h"
#include "Global.h"

class UserConfig : public Configuration::Object
{
public:
	UserConfig();
	~UserConfig();

	void addRecentlyOpenedProject( const QString & _file );
	QStringList recentlyOpenedProjects();

	ADD_CONFIG_BOOL_PROPERTY( isConfigured, setIsConfigured, "IsConfigured", "App" );

	ADD_CONFIG_PROPERTY( audioBackend, setAudioBackend, "AudioBackend", "Mixer" );
	ADD_CONFIG_PROPERTY( midiBackend, setMidiBackend, "MidiBackend", "Mixer" );
	ADD_CONFIG_INT_PROPERTY( bufferSize, setBufferSize, "BufferSize", "Mixer" );
	ADD_CONFIG_INT_PROPERTY( sampleRate, setSampleRate, "SampleRate", "Mixer" );
	ADD_CONFIG_BOOL_PROPERTY( hqAudio, setHqAudio, "HqAudio", "Mixer" );

	ADD_CONFIG_PROPERTY( uiTheme, setUiTheme, "Theme", "UI" );
	ADD_CONFIG_PROPERTY( backgroundArtwork, setBackgroundArtwork, "BackgroundArtwork", "UI" );
	ADD_CONFIG_BOOL_PROPERTY( toolTipsEnabled, setToolTipsEnabled, "ToolTipsEnabled", "UI" );
	ADD_CONFIG_BOOL_PROPERTY( displayKnobDBV, setDisplayKnobDBV, "DisplayKnobDBV", "UI" );
	ADD_CONFIG_BOOL_PROPERTY( pianoFeedback, setPianoFeedback, "PianoFeedback", "UI" );
	ADD_CONFIG_BOOL_PROPERTY( instrumentFadeButton, setInstrumentFadeButton, "InstrumentFadeButton", "UI" );

	ADD_CONFIG_PROPERTY( defaultSoundFont, setDefaultSoundfont, "DefaultSoundFont", "Plugins" );

} ;


class UserConfigAdaptor
{
public:
	UserConfigAdaptor( const QString & _scope ) :
		m_scope( _scope )
	{
	}

	void setValue( const QString & _key, const QString & _value )
	{
		Global::userConfig().setValue( _key, _value, m_scope );
	}

	QString value( const QString & _key )
	{
		return Global::userConfig().value( _key, m_scope );
	}


private:
	const QString m_scope;
} ;


#define ADD_USER_CONFIG_ADAPTOR(scope)				\
			static UserConfigAdaptor cfg()			\
			{										\
				return UserConfigAdaptor( scope );	\
			}


#endif