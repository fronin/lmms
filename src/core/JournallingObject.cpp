/*
 * JournallingObject.cpp - implementation of journalling-object related stuff
 *
 * Copyright (c) 2006-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#include <QtXml/QDomElement>

#include <cstdio>

#include "JournallingObject.h"
#include "AutomatableModel.h"
#include "base64.h"
#include "engine.h"



JournallingObject::JournallingObject() :
	SerializingObject(),
	m_id( 0 ),
	m_journalEntries(),
	m_currentJournalEntry( m_journalEntries.end() ),
	m_journalling( true ),
	m_journallingStateStack()
{
}




JournallingObject::~JournallingObject()
{
	// Killing JournallingObject
}




void JournallingObject::undo()
{
	// Killing JournallingObject
}




void JournallingObject::redo()
{
	// Killing JournallingObject
}




QDomElement JournallingObject::saveState( QDomDocument & _doc,
							QDomElement & _parent )
{
	return SerializingObject::saveState( _doc, _parent );
}




void JournallingObject::restoreState( const QDomElement & _this )
{
	// Killing JournallingObject
}




void JournallingObject::addJournalEntry( const JournalEntry & _je )
{
	// Killing JournallingObject
}




void JournallingObject::changeID( jo_id_t _id )
{
	// Killing JournallingObject
}




void JournallingObject::saveJournal( QDomDocument & _doc,
							QDomElement & _parent )
{
// Killing JournallingObject
}




void JournallingObject::loadJournal( const QDomElement & _this )
{
// Killing JournallingObject
}


