/*
 * PathConfig.cpp - implementation of class PathConfig
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

#include <QtCore/QCoreApplication>
#include <QtCore/QDir>
#include <QtGui/QMessageBox>

#include "PathConfig.h"
#include "UserConfig.h"
#include "RuntimeConfig.h"
#include "MainWindow.h"
#include "lmmsconfig.h"


const QString PathConfig::ProjectsPath = "projects/";
const QString PathConfig::PresetsPath = "presets/";
const QString PathConfig::SamplesPath = "samples/";
const QString PathConfig::ThemesPath = "themes/";
const QString PathConfig::LocalePath = "locale/";


PathConfig::PathConfig() :
	Configuration::Object( Configuration::Backend::XmlFile, "PathConfig" ),
	m_workingDir( QDir::home().absolutePath() + QDir::separator() +
						"lmms" + QDir::separator() ),
	m_dataDir( QCoreApplication::applicationDirPath()
#ifdef LMMS_BUILD_WIN32
			+ QDir::separator() + "data" + QDir::separator()
#else
				.section( '/', 0, -2 ) + "/share/lmms/"
#endif
									),
#ifdef LMMS_BUILD_WIN32
	m_pluginDir( QCoreApplication::applicationDirPath()
			+ QDir::separator() + "plugins" + QDir::separator() )
#else
	m_pluginDir( QString( PLUGIN_DIR ) )
#endif
{
	if( vstDir().value().isEmpty() || vstDir().value() == QDir::separator() )
	{
		vstDir() = QDir::home().absolutePath() + QDir::separator();
	}

	if( flDir().value().isEmpty() || flDir().value() == QDir::separator() )
	{
		flDir() = QDir::home().absolutePath() + QDir::separator();
	}

	if( ladspaDir().value().isEmpty() ||
			ladspaDir().value() == QDir::separator() )
	{
#ifdef LMMS_BUILD_WIN32
		ladspaDir() = pluginDir() + "ladspa" + QDir::separator();
#else
		ladspaDir() = QString( LIB_DIR ) + "/ladspa/";
#endif
	}

	if( stkDir().value().isEmpty() || stkDir().value() == QDir::separator() )
	{
#ifdef LMMS_BUILD_WIN32
		stkDir() = dataDir() + "stk/rawwaves/";
#else
		stkDir() = "/usr/share/stk/rawwaves/";
#endif
	}

	if( !QDir( workingDir() ).exists() && Global::runtimeConfig().hasGui() )
	{
		if( QMessageBox::question( 0,
			MainWindow::tr( "Working directory" ),
			MainWindow::tr( "A working directory for LMMS does not "
				"exist. Create a default one at %1 now? You can change "
				"the directory later via Edit -> Preferences." ).
												arg( workingDir() ),
					QMessageBox::Yes, QMessageBox::No ) ==
								QMessageBox::Yes )
		{
			QDir().mkpath( workingDir() );
		}
	}
	if( QDir( workingDir() ).exists() )
	{
		QDir().mkpath( userProjectsDir() );
		QDir().mkpath( userSamplesDir() );
		QDir().mkpath( userPresetsDir() );
	}
}




PathConfig::~PathConfig()
{
}




QString PathConfig::activeThemePath() const
{
	return themesPath() + Global::userConfig().uiTheme() + QDir::separator();
}


