/*
 * ConfigurationXmlBackend.cpp - implementation of Configuration::XmlBackend
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

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtGui/QMessageBox>
#include <QtXml/QDomDocument>

#include "ConfigurationXmlBackend.h"
#include "ConfigurationObject.h"
#include "MainWindow.h"
#include "engine.h"


namespace Configuration
{

XmlBackend::XmlBackend( const QString & _fileName ) :
	Backend( Backend::XmlFile ),
	m_fileName( _fileName )
{
	if( m_fileName.isNull() )
	{
		m_fileName = QDir::homePath() + QDir::separator() + ".lmmsrc";
	}
}




static void loadXmlTree( Object * _obj, QDomNode & _parentNode,
				const QString & _parentKey )
{
	QDomNode node = _parentNode.firstChild();

	while( !node.isNull() )
	{
		if( node.hasChildNodes() )
		{
			const QString subParentKey = _parentKey +
				( _parentKey.isEmpty() ? "" : "/" ) +
					node.nodeName();
			loadXmlTree( _obj, node, subParentKey );
		}
		else if( node.isElement() )
		{
			_obj->setValue( node.nodeName(),
					node.toElement().text(),
					_parentKey );
		}
		node = node.nextSibling();
	}
}




void XmlBackend::load( Object * _obj )
{
	QDomDocument doc;
	QFile xmlFile( m_fileName );
	if( !xmlFile.open( QFile::ReadOnly ) || !doc.setContent( &xmlFile ) )
	{
	/*	QMessageBox::information(
			engine::mainWindow(),
			_obj->tr( "No configuration file found" ),
			_obj->tr( "Could not open configuration file %1. "
						"Default settings will be loaded." ).arg( m_fileName ) );*/
		return;
	}

	QDomElement root = doc.documentElement();
	loadXmlTree( _obj, root, QString() );
}




static void saveXmlTree( const Object::DataMap & _dataMap,
				QDomDocument & _doc,
				QDomNode & _parentNode,
				const QString & _parentKey )
{
	for( Object::DataMap::ConstIterator it = _dataMap.begin();
						it != _dataMap.end(); ++it )
	{
		if( it.value().type() == QVariant::Map )
		{
			// create a new element with current key as tagname
			QDomNode node = _doc.createElement( it.key() );

			const QString subParentKey = _parentKey +
				( _parentKey.isEmpty() ? "" : "/" ) + it.key();
			saveXmlTree( it.value().toMap(),
					_doc,
					node,
					subParentKey );
			_parentNode.appendChild( node );
		}
		else if( it.value().type() == QVariant::String )
		{
			QDomElement e = _doc.createElement( it.key() );
			QDomText t = _doc.createTextNode( it.value().toString() );
			e.appendChild( t );
			_parentNode.appendChild( e );
		}
	}
}




void XmlBackend::flush( Object * _obj )
{
	QDomDocument doc( "LmmsXmlConfiguration" );
	const Object::DataMap & data = _obj->data();

	QDomElement root = doc.createElement( "LMMS" );
	saveXmlTree( data, doc, root, QString() );
	doc.appendChild( root );

	QFile xmlFile( m_fileName );
	if( !xmlFile.open( QFile::ReadWrite ) || !doc.setContent( &xmlFile ) )
	{
	/*	QMessageBox::information(
			engine::mainWindow(),
			_obj->tr( "Could not write configuration file" ),
			_obj->tr( "Could not write configuration file %1. "
						"Make sure you have write permission to this file." ).
															arg( m_fileName ) );*/
		return;
	}

	QTextStream( &xmlFile ) << doc.toString( 2 );
}


}

