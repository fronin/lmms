/*
 * ConfigurationObject.cpp - implementation of ConfigurationObject
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

#include <QtCore/QStringList>

#include "ConfigurationObject.h"
#include "ConfigurationNativeBackend.h"
#include "ConfigurationXmlBackend.h"


namespace Configuration
{

class DummyBackend : public Backend
{
public:
    DummyBackend() :
		Backend( None )
	{
	}
	virtual ~DummyBackend()
	{
	}

    virtual void load( Object * )
	{
	}
    virtual void flush( Object * )
	{
	}

} ;


Object::Object( Backend::Type _type, const QString & _configName ) :
	m_backend( NULL )
{
	switch( _type )
	{
		case Backend::None:
			m_backend = new DummyBackend();
			break;
		case Backend::Native:
			m_backend = new NativeBackend();
			break;
		case Backend::XmlFile:
			m_backend = new XmlBackend( _configName );
			break;
		default:
			qCritical( "Invalid Backend::Type %d selected in "
					"Object::Object()", _type );
			break;
	}

	m_backend->load( this );
}




Object::~Object()
{
	// flush data to backend
	flush();

	// and delete it
	delete m_backend;
}




QString Object::value( const QString & _key, const QString & _parentKey ) const
{
	// empty parentKey?
	if( _parentKey.isEmpty() )
	{
		// search for key in toplevel data map
		if( m_data.contains( _key ) )
		{
			return m_data[_key].toString();
		}
		return QString();
	}

	// recursively search through data maps and sub data-maps until
	// all levels of the parentKey are processed
	const QStringList subLevels = _parentKey.split( '/' );
	DataMap currentMap = m_data;
	foreach( const QString & _level, subLevels )
	{
		if( currentMap.contains( _level ) &&
			currentMap[_level].type() == QVariant::Map )
		{
			currentMap = currentMap[_level].toMap();
		}
		else
		{
			return QString();
		}
	}

	// ok, we're there - does the current submap then contain our key?
	if( currentMap.contains( _key ) )
	{
		return currentMap[_key].toString();
	}
	return QString();
}




Object::DataMap Object::insertSubValue( const QString & _key,
								const QString & _value,
								QStringList & subLevels,
								DataMap & _map )
{
	if( !subLevels.isEmpty() )
	{
		const QString level = subLevels.first();
		if( !_map.contains( level ) )
		{
			_map[level] = DataMap();
		}
		subLevels.removeFirst();
		if( _map[level].type() == QVariant::Map )
		{
			DataMap tmpMap = _map[level].toMap();
			_map[level] = insertSubValue( _key, _value, subLevels, tmpMap );
			return _map;
		}
		else
		{
			qWarning( "parent-key points to a string value "
						"rather than a sub data map!" );
			return _map;
		}
	}

	_map[_key] = _value;

	return _map;
}




void Object::setValue( const QString & _key,
			const QString & _value,
			const QString & _parentKey )
{
	if( _parentKey.isEmpty() )
	{
		// search for key in toplevel data map
		if( m_data.contains( _key ) && m_data[_key].type() !=
							QVariant::String )
		{
			qWarning( "cannot replace sub data map with a "
					"string value!" );
			return;
		}
		m_data[_key] = _value;
		return;
	}

	// recursively search through data maps and sub data-maps until
	// all levels of the parentKey are processed
	QStringList subLevels = _parentKey.split( '/' );

	m_data = insertSubValue( _key, _value, subLevels, m_data );
}

}


#include "moc_ConfigurationObject.cxx"

