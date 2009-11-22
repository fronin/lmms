/*
 * ConfigurationProperty.cpp - implementation of ConfigurationProperty
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

#include "ConfigurationProperty.h"


namespace Configuration
{

// specializations for common types
template<> PropertyBase::DataType Property<QString>::templateDataType() const
{
	return String;
}

template<> PropertyBase::DataType Property<int>::templateDataType() const
{
	return Integer;
}

template<> PropertyBase::DataType Property<bool>::templateDataType() const
{
	return Boolean;
}

template<> PropertyBase::DataType Property<float>::templateDataType() const
{
	return Float;
}

template<> Property<QString>::operator QString() const
{
	return value();
}

template<> QString Property<QString>::operator=( const QString & _val )
{
	setValue( _val );
	return _val;
}


template<> Property<int>::operator int() const
{
	return value().toInt();
}


template<> Property<bool>::operator bool() const
{
	return static_cast<bool>( value().toInt() );
}


template<> Property<float>::operator float() const
{
	return value().toFloat();
}


}

