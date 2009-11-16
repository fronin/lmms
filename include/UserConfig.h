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

class UserConfig : public Configuration::Object
{
public:
	UserConfig();
	~UserConfig();

	ADD_CONFIG_INT_PROPERTY( bufferSize, setBufferSize, "BufferSize", "Mixer" );

	ADD_CONFIG_PROPERTY( backgroundArtwork, setBackgroundArtwork, "BackgroundArtwork", "UI" );
	ADD_CONFIG_BOOL_PROPERTY( toolTips, setToolTips, "ToolTips", "UI" );
	ADD_CONFIG_BOOL_PROPERTY( displayDBV, setDisplayDBV, "DisplayDBV", "UI" );
	ADD_CONFIG_BOOL_PROPERTY( pianoFeedback, setPianoFeedback, "PianoFeedback", "UI" );
	ADD_CONFIG_BOOL_PROPERTY( instrumentFadeButton, setInstrumentFadeButton, "InstrumentFadeButton", "UI" );

	ADD_CONFIG_PROPERTY( defaultSoundFont, setDefaultSoundfont, "DefaultSoundFont", "Plugins" );

} ;

#endif
