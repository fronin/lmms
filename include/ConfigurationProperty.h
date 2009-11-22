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

/*! \brief The Configuration::PropertyBase class stores basic information about a configuration property
 *
 * A configuration property is identified by a Configuration::Object it
 * belongs to, a key, a parent key (can be nested with "/") and a data type.
 */

class PropertyBase
{
public:
	/*! Lists all possible data types for configuration properties */
	enum DataTypes
	{
		String,		/*!< value is a QString */
		Integer,	/*!< value is an int */
		Boolean,	/*!< value is a bool */
		Float		/*!< value is a float */
	} ;
	typedef DataTypes DataType;

	/*! \brief Constructs a PropertyBase object
	*
	* \param obj The associated Configuration::Object
	* \param key The key used for looking up the actual configuration value
	* \param parentKey The parent key (can be nested with "/", e.g. "Plugins/Foo/UI")
	* \param dataType The desired DataType
	*/
	PropertyBase( Object * _obj, const QString & _key,
				const QString & _parentKey, DataType _dataType ) :
		m_obj( _obj ),
		m_key( _key ),
		m_parentKey( _parentKey ),
		m_dataType( _dataType )
	{
	}

	/*! \brief Reads configuration value
	*
	* \return Configuration value to which this property points
	*/
	QString value() const
	{
		return m_obj->value( m_key, m_parentKey );
	}

	/*! \brief Sets configuration value
	* \param val The desired configuration value to be set */
	void setValue( const QString & _val )
	{
		m_obj->setValue( m_key, _val, m_parentKey );
	}

	/*! \brief Reads data type
	*
	* \return Data type for which this property item was created
	*/
	DataType dataType() const
	{
		return m_dataType;
	}

private:
	Object * m_obj;
	const QString m_key;
	const QString m_parentKey;
	DataType m_dataType;
} ;



/*! \brief Support for configuration properties with specific data types
 *
 * This template class is designed to allow easy access to configuration
 * properties that are "shaped" for a certain data type. This is achieved
 * via template specialization.
 */

template<typename Type>
class Property : public PropertyBase
{
public:
	/*! \brief Constructs a Property object
	*
	* \param obj The associated Configuration::Object
	* \param key The key used for looking up the actual configuration value
	* \param parentKey The parent key (can be nested with "/", e.g. "Plugins/Foo/UI")
	*/
	Property( Object * _obj, const QString & _key,
				const QString & _parentKey ) :
		PropertyBase( _obj, _key, _parentKey, templateDataType() )
	{
	}

	/*! \brief Reads data type
	*
    * Method is implemented via template specialization
	* \return Data type with which the Configuration::Property template was instantiated
	*/
	DataType templateDataType() const;

	/* \brief Returns data type specific configuration value */
	operator Type() const;

	/* \brief Sets data type specific configuration value */
	Type operator=( const Type & _val )
	{
		setValue( QString::number( _val ) );
		return _val;
	}

} ;


// declarations of specializations for common types
template<> PropertyBase::DataType Property<QString>::templateDataType() const;
template<> PropertyBase::DataType Property<int>::templateDataType() const;
template<> PropertyBase::DataType Property<bool>::templateDataType() const;
template<> PropertyBase::DataType Property<float>::templateDataType() const;
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
