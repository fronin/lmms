/*
 * ConfigurationObject.h - Configuration::Object class
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

#ifndef _CONFIGURATION_OBJECT_H
#define _CONFIGURATION_OBJECT_H

#include <QtCore/QMap>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVariant>

#include "ConfigurationBackend.h"

namespace Configuration
{

/*! \brief The Configuration::Object class loads and stores configuration properties with their respective values.
 *
 * A configuration object is a container for a key-based hierarchical data set.
 * It loads and stores this set from/to a given backend.
 */

class Object : public QObject
{
	Q_OBJECT
public:
	/*! \brief Type of our data container */
	typedef QMap<QString, QVariant> DataMap;

	/*! \brief Constructs a Configuration::Object object
	*
	* \param backendType The desired Configuration::Backend
	* \param configName A name for this configuration object (used as scope or filename)
	*/
	Object( Backend::Type backendType, const QString & configName = QString::null );
	~Object();

	/*! \brief Reads value of a given configuration property
	* \param key The configuration property key
	* \param parentKey The configuration property parent key
	* \return Value of the given configuration property
	*/
	QString value( const QString & key,
			const QString & parentKey = QString() ) const;

	/*! \brief Sets value of a given configuration property
	* \param key The configuration property key
	* \param value The actual value to be set
	* \param parentKey The configuration property parent key
	*/
	void setValue( const QString & key,
			const QString & value,
			const QString & parentKey = QString() );

	/*! \brief Flushes all configuration data to backend */
	void flush()
	{
		m_backend->flush( this );
	}

	/*! \brief Returns const reference to data - used by Configuration::Backend */
	const DataMap & data() const
	{
		return m_data;
	}


private:
	/*! \brief Recursive helper function for inserting with nested parent keys */
	DataMap insertSubValue( const QString & key, const QString & value,
								QStringList & subLevels, DataMap & map );

	Configuration::Backend * m_backend;
	DataMap m_data;

} ;

}

#endif
