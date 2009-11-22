/*
 * PreferencesDialog.h - declaration of class PreferencesDialog
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

#ifndef _PREFERENCES_DIALOG_H
#define _PREFERENCES_DIALOG_H

#include <QtCore/QList>
#include <QtGui/QDialog>

namespace Ui { class PreferencesDialog; }

class ConfigUiAdaptor;

class PreferencesDialog : public QDialog
{
public:
	/*! An enumeration listing all available configuration tabs */
	enum ConfigTabs
	{
		GeneralSettings,	/*!< tab with general settings */
		PathSettings,		/*!< tab with various path settings */
		AudioSettings,		/*!< all audio related settings such as backend, device, buffersize etc */
		MidiSettings,		/*!< all MIDI related settings such as backend and device */
		PluginSettings,		/*!< settings for individual plugins */
		NumTabs
	} ;
	typedef ConfigTabs ConfigTab;

	PreferencesDialog( ConfigTab _tabToOpen = GeneralSettings );
	virtual ~PreferencesDialog();

	virtual void accept();


private:
	void loadConfig();
	void storeConfig();

	QList<ConfigUiAdaptor *> m_configUiAdaptors;

	Ui::PreferencesDialog * ui;

} ;

#endif

