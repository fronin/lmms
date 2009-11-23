/*
 * ConfigurationBackend.h - Configuration::Backend class
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

#ifndef _CONFIGURATION_BACKEND_H
#define _CONFIGURATION_BACKEND_H

namespace Configuration
{

class Object;

/*! \brief The Configuration::Backend class is the storage backend for Configuration::Object
 *
 * The Configuration::Backend class is responsible for storing the data of a
 * Configuration::Object. This is just an abstract base class. Functionality is
 * provided by the individual subclasses.
 */

class Backend
{
public:
	/*! Lists all configuration backends. */
	enum Types
	{
		None,	/*!< for storing volatile data in RAM only */
		Native,	/*!< registry or similiar via QSettings */
		XmlFile	/*!< a QtXml-based XML file IO backend */
	} ;
	typedef Types Type;

	/*! \brief Constructs a Backend object
	* \param type The type of this backend instance
	*/
	Backend( Type type ) :
		m_type( type )
	{
	}

	/*! \brief Returns type of this backend instance */
	inline Type type() const
	{
		return m_type;
	}

	/*! \brief Loads data to a given Configuration::Object
	* \param obj The Configuration::Object to load data to
	*/
	virtual void load( Object * obj ) = 0;

	/*! \brief Saves data of a given Configuration::Object
	* \param obj The Configuration::Object to save data from
	*/
	virtual void flush( Object * obj ) = 0;


private:
	const Type m_type;

} ;

}

#endif
