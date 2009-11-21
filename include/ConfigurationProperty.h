/*
 * ConfigurationProperty.h - Configuration::Property class
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

#ifndef _CONFIGURATION_PROPERTY_H
#define _CONFIGURATION_PROPERTY_H

#include "ConfigurationObject.h"

namespace Configuration
{

template<typename TYPE>
class Property
{
public:
	Property( Object * _obj, const QString & _key,
				const QString & _parentKey ) :
		m_obj( _obj ),
		m_key( _key ),
		m_parentKey( _parentKey )
	{
	}

	QString value() const
	{
		return m_obj->value( m_key, m_parentKey );
	}

	void setValue( const QString & _val )
	{
		m_obj->setValue( m_key, _val, m_parentKey );
	}

	operator TYPE() const;
	TYPE operator=( const TYPE & _val )
	{
		setValue( QString::number( _val ) );
		return _val;
	}

private:
	Object * m_obj;
	const QString m_key;
	const QString m_parentKey;
} ;


// declarations of specializations for common types
template<> Property<QString>::operator QString() const;
template<> QString Property<QString>::operator=( const QString & _val );
template<> Property<int>::operator int() const;
template<> Property<bool>::operator bool() const;
template<> Property<float>::operator float() const;



#define ADD_CONFIG_GENERIC_PROPERTY(prop,key,parentKey,type)	\
		inline Configuration::Property<type> prop()				\
		{														\
			return Configuration::Property<type>( this,			\
									 key, parentKey );			\
		}

#define ADD_CONFIG_PROPERTY(prop,key,parentKey) ADD_CONFIG_GENERIC_PROPERTY(prop,key,parentKey,QString)
#define ADD_CONFIG_INT_PROPERTY(prop,key,parentKey) ADD_CONFIG_GENERIC_PROPERTY(prop,key,parentKey,int)
#define ADD_CONFIG_FLOAT_PROPERTY(prop,key,parentKey) ADD_CONFIG_GENERIC_PROPERTY(prop,key,parentKey,float)
#define ADD_CONFIG_BOOL_PROPERTY(prop,key,parentKey) ADD_CONFIG_GENERIC_PROPERTY(prop,key,parentKey,bool)

}

#endif
