/*
 * PathConfig.h - provides global information about paths
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

#ifndef _PATH_CONFIG_H
#define _PATH_CONFIG_H

#include "ConfigurationObject.h"
#include "export.h"


class EXPORT PathConfig : public Configuration::Object
{
public:
	PathConfig();
	~PathConfig();

	ADD_CONFIG_PROPERTY( artworkDir, setArtworkDir, "ArtworkDirectory", "Paths" );
	ADD_CONFIG_PROPERTY( vstDir, setVstDir, "VstPluginDirectory", "Paths" );
	ADD_CONFIG_PROPERTY( flDir, setFlDir, "FlDirectory", "Paths" );
	ADD_CONFIG_PROPERTY( ladspaDir, setLadspaDir, "LadspaDirectory", "Paths" );
	ADD_CONFIG_PROPERTY( stkDir, setStkDir, "StkDirectory", "Paths" );
	ADD_CONFIG_PROPERTY( lameLibrary, setLameLibrary, "LameLibrary", "Paths" );


	const QString & dataDir() const
	{
		return m_dataDir;
	}

	const QString & workingDir() const
	{
		return m_workingDir;
	}

	QString userProjectsDir() const
	{
		return workingDir() + ProjectsPath;
	}

	QString userPresetsDir() const
	{
		return workingDir() + PresetsPath;
	}

	QString userSamplesDir() const
	{
		return workingDir() + SamplesPath;
	}

	QString factoryProjectsDir() const
	{
		return dataDir() + ProjectsPath;
	}

	QString factoryPresetsDir() const
	{
		return dataDir() + PresetsPath;
	}

	QString factorySamplesDir() const
	{
		return dataDir() + SamplesPath;
	}

	QString defaultArtworkDir() const
	{
		return dataDir() + DefaultThemePath;
	}
	QString localeDir() const
	{
		return dataDir() + LocalePath;
	}

	const QString & pluginDir() const
	{
		return m_pluginDir;
	}


private:
	static const QString ProjectsPath;
	static const QString PresetsPath;
	static const QString SamplesPath;
	static const QString DefaultThemePath;
	static const QString LocalePath;

	QString m_workingDir;
	QString m_dataDir;
	QString m_pluginDir;

} ;

#endif
