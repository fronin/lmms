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

class Object : public QObject
{
	Q_OBJECT
public:
	typedef QMap<QString, QVariant> DataMap;

	Object( Backend::Type _type, const QString & _configName = QString::null );
	~Object();

	QString value( const QString & _key,
			const QString & _parentKey = QString() ) const;

	void setValue( const QString & _key,
			const QString & _value,
			const QString & _parentKey = QString() );

	void flush()
	{
		m_backend->flush( this );
	}

	const DataMap & data() const
	{
		return m_data;
	}


private:
	DataMap insertSubValue( const QString & _key, const QString & _value,
								QStringList & subLevels, DataMap & _map );

	Configuration::Backend * m_backend;
	DataMap m_data;

} ;

}

#endif
