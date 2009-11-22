/*
 * PreferencesDialog.cpp - implementation of PreferencesDialog
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

#include "PreferencesDialog.h"
#include "embed.h"
#include "engine.h"
#include "MainWindow.h"
#include "PathConfig.h"
#include "UserConfig.h"

#include "ui_PreferencesDialog.h"


/*! \brief Provides an adaptor between widgets and a configuration property
 *
 * This is a helper class allowing to link an input widget and a configuration
 * property. All we need in the preferences dialog is to create one adaptor
 * for each input widget+property, e.g. a QCheckBox and a boolean property
 */

class ConfigUiAdaptor
{
public:
	/*! An enumeration listing all types of controls ConfigUiAdaptor can handle */
	enum ControlTypes
	{
		AbstractButton,	/*!< all classes derived from QAbstractButton */
		ComboBox		/*!< QComboBox and subclasses */
	} ;
	typedef ControlTypes ControlType;

	/*! \brief initialize adaptor for a button */
	ConfigUiAdaptor( QAbstractButton * _btn,
						const Configuration::PropertyBase & _prop ) :
		m_controlType( AbstractButton ),
		m_abstractButton( _btn ),
		m_comboBox( NULL ),
		m_property( _prop )
	{
	}

	/*! \brief initialize adaptor for a combobox */
	ConfigUiAdaptor( QComboBox * _combobox,
						const Configuration::PropertyBase & _prop ) :
		m_controlType( ComboBox ),
		m_abstractButton( NULL ),
		m_comboBox( _combobox ),
		m_property( _prop )
	{
	}

	/*! \brief load value of configuration property to associated input widget */
	void loadProperty()
	{
		switch( m_property.dataType() )
		{
			case Configuration::PropertyBase::String:
				switch( m_controlType )
				{
					case ComboBox:
						m_comboBox->setCurrentIndex(
							m_comboBox->findText( m_property.value() ) );
						break;
					default:
						break;
				}
				break;
			case Configuration::PropertyBase::Integer:
				switch( m_controlType )
				{
					case ComboBox:
						m_comboBox->setCurrentIndex( m_property.value().toInt() );
						break;
					default:
						break;
				}
				break;
			case Configuration::PropertyBase::Boolean:
				switch( m_controlType )
				{
					case AbstractButton:
						m_abstractButton->setChecked( m_property.value().toInt() );
						break;
					default:
						break;
				}
				break;
			case Configuration::PropertyBase::Float:
			default:
				break;
		}
	}

	/*! \brief store value of input widget to associated configuration property */
	void storeProperty()
	{
	}

private:
	ControlType m_controlType;
	QAbstractButton * m_abstractButton;
	QComboBox * m_comboBox;
	Configuration::PropertyBase m_property;
} ;



/*! \brief Construct and initialize preferences dialog
 *
 * \param _tabToOpen The configuration tab that should be opened by default
 */
PreferencesDialog::PreferencesDialog( ConfigTab _tabToOpen ) :
	QDialog( engine::mainWindow() ),
	ui( new Ui::PreferencesDialog )
{
	ui->setupUi( this );

	// set up icons in page selector view on the left side
	static const char * icons[NumTabs] = {
		"preferences-system",
		"folder-64",
		"preferences-desktop-sound",
		"setup-midi",
		"setup-plugins"
	} ;
	for( int i = 0; i < qMin<int>( NumTabs,
								ui->configPageSelector->count() ); ++i )
	{
		ui->configPageSelector->item( i )->setIcon(
									embed::getIconPixmap( icons[i] ) );
	}

	if( _tabToOpen != GeneralSettings )
	{
		ui->configPageSelector->setCurrentRow( _tabToOpen );
	}

#define ADD_USER_CONFIG_UI_ADAPTOR(property)		\
			m_configUiAdaptors << new ConfigUiAdaptor( ui->property, Global::userConfig().property() );

	// user interface
	ADD_USER_CONFIG_UI_ADAPTOR(toolTipsEnabled);
	ADD_USER_CONFIG_UI_ADAPTOR(showVolumeAsDBV);
	ADD_USER_CONFIG_UI_ADAPTOR(showWelcomeScreen);
	ADD_USER_CONFIG_UI_ADAPTOR(pianoFeedback);
	ADD_USER_CONFIG_UI_ADAPTOR(instrumentActivityFeedback);

	ADD_USER_CONFIG_UI_ADAPTOR(bufferSize);
	ADD_USER_CONFIG_UI_ADAPTOR(sampleRate);

	loadConfig();
}




PreferencesDialog::~PreferencesDialog()
{
	foreach( ConfigUiAdaptor * c, m_configUiAdaptors )
	{
		delete c;
	}
}




/*! \brief Save configuration and close dialog */
void PreferencesDialog::accept()
{
	storeConfig();

	QDialog::accept();
}



/*! \brief Load all configuration property values to UI widgets */
void PreferencesDialog::loadConfig()
{
	foreach( ConfigUiAdaptor * c, m_configUiAdaptors )
	{
		c->loadProperty();
	}
}




/*! \brief Load values of UI widgets back to configuration properties */
void PreferencesDialog::storeConfig()
{
	foreach( ConfigUiAdaptor * c, m_configUiAdaptors )
	{
		c->storeProperty();
	}
}


