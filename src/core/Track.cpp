/*
 * Track.cpp - implementation of class Track and TrackContentObject
 * neccessary for all track-like objects (beat/bassline, sample-track...)
 *
 * Copyright (c) 2004-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 * Copyright (c) 2009 Paul Giblock <pgib/at/users.sourceforge.net>
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

/** \file track.cpp
 *  \brief All classes concerning tracks and track-like objects
 */

/*
 * \mainpage Track classes
 *
 * \section introduction Introduction
 * 
 * \todo fill this out
 */

#include <assert.h>
#include <cstdio>

#include "Track.h"
#include "TrackContainer.h"
#include "Clipboard.h"

// For Track::create()
//#include "AutomationTrack.h"
#include "BbTrack.h"
#include "SampleTrack.h"
#include "InstrumentTrack.h"


#include "embed.h"
#include "engine.h"
#include "gui_templates.h"
#include "mmp.h"
#include "pixmap_button.h"
#include "Song.h"
#include "string_pair_drag.h"
#include "templates.h"
#include "text_float.h"
#include "tooltip.h"


// ===========================================================================
// TrackSegment
// ===========================================================================
/*! \brief Create a new trackContentObject
 *
 *  Creates a new track content object for the given track.
 *
 * \param _track The track that will contain the new object
 */
TrackSegment::TrackSegment( Track * _track ) :
	Model( _track ),
	m_track( _track ),
	m_name( QString::null ),
	m_startPosition(),
	m_length(),
	m_muted( false, this, tr( "Muted" ) )
{
	/*
	if( getTrack() )
	{
		getTrack()->addTCO( this );
	}
	*/
	setJournalling( false );
	movePosition( 0 );
	changeLength( 0 );
	setJournalling( true );
}




/*! \brief Destroy a trackContentObject
 *
 *  Destroys the given track content object.
 *
 */
TrackSegment::~TrackSegment()
{
	emit destroyedTCO();

	/*
	if( getTrack() )
	{
		getTrack()->removeTCO( this );
	}
	*/
}




/*! \brief Move this trackContentObject's position in time
 *
 *  If the track content object has moved, update its position.  We
 *  also add a journal entry for undo and update the display.
 *
 * \param _pos The new position of the track content object.
 */
void TrackSegment::movePosition( const midiTime & _pos )
{
	if( m_startPosition != _pos )
	{
		addJournalEntry( JournalEntry( Move, m_startPosition - _pos ) );
		m_startPosition = _pos;
		engine::song()->updateLength();
	}
	emit positionChanged();
}




/*! \brief Change the length of this trackContentObject
 *
 *  If the track content object's length has chaanged, update it.  We
 *  also add a journal entry for undo and update the display.
 *
 * \param _length The new length of the track content object.
 */
void TrackSegment::changeLength( const midiTime & _length )
{
	if( m_length != _length )
	{
		addJournalEntry( JournalEntry( Resize, m_length - _length ) );
		m_length = _length;
		engine::song()->updateLength();
	}
	emit lengthChanged();
}




/*! \brief Undo one journal entry of this trackContentObject
 *
 *  Restore the previous state of this track content object.  This will
 *  restore the position or the length of the track content object
 *  depending on what was changed.
 *
 * \param _je The journal entry to undo
 */
void TrackSegment::undoStep( JournalEntry & _je )
{
	saveJournallingState( false );
	switch( _je.actionID() )
	{
		case Move:
			movePosition( startPosition() + _je.data().toInt() );
			break;
		case Resize:
			changeLength( length() + _je.data().toInt() );
			break;
	}
	restoreJournallingState();
}




/*! \brief Redo one journal entry of this trackContentObject
 *
 *  Undoes one 'undo' of this track content object.
 *
 * \param _je The journal entry to redo
 */
void TrackSegment::redoStep( JournalEntry & _je )
{
	JournalEntry je( _je.actionID(), -_je.data().toInt() );
	undoStep( je );
}




/*! \brief Copy this trackContentObject to the clipboard.
 *
 *  Copies this track content object to the clipboard.
 */
void TrackSegment::copy()
{
	Clipboard::copy( this );
}




/*! \brief Pastes this trackContentObject into a track.
 *
 *  Pastes this track content object into a track.
 *
 * \param _je The journal entry to undo
 */
void TrackSegment::paste()
{
	if( Clipboard::getContent( nodeName() ) != NULL )
	{
		restoreState( *( Clipboard::getContent( nodeName() ) ) );
	}
}




/*! \brief Mutes this trackContentObject
 *
 *  Restore the previous state of this track content object.  This will
 *  restore the position or the length of the track content object
 *  depending on what was changed.
 *
 * \param _je The journal entry to undo
 */
void TrackSegment::toggleMute()
{
	m_muted.setValue( !m_muted.value() );
	emit dataChanged();
}



// ===========================================================================
// Track
// ===========================================================================

/*! \brief Create a new (empty) track object
 *
 *  The track object is the whole track, linking its contents, its
 *  automation, name, type, and so forth.
 *
 * \param _type The type of track (Song Editor or Beat+Bassline Editor)
 * \param _tc The track Container object to encapsulate in this track.
 *
 * \todo check the definitions of all the properties - are they OK?
 */
Track::Track( TrackTypes _type, TrackContainer * _tc ) :
	Model( _tc ),                   /*!< The track Model */
	m_trackContainer( _tc ),        /*!< The track container object */
	m_type( _type ),                /*!< The track type */
	m_name(),                       /*!< The track's name */
	m_muted( false, this, tr( "Muted" ) ),
					 /*!< For controlling track muting */
	m_solo( false, this, tr( "Solo" ) ),
					/*!< For controlling track soloing */
	m_simpleSerializingMode( false ),
	m_trackSegments()         /*!< The track content objects (segments) */
{
	m_trackContainer->addTrack( this );
}




/*! \brief Destroy this track
 *
 *  If the track container is a Beat+Bassline container, step through
 *  its list of tracks and remove us.
 *
 *  Then delete the trackContentObject's contents, remove this track from
 *  the track container.
 *
 *  Finally step through this track's automation and forget all of them.
 */
Track::~Track()
{
	emit destroyedTrack();

	while( !m_trackSegments.isEmpty() )
	{
		delete m_trackSegments.last();
	}

	//m_trackContainer->removeTrack( this );
}




/*! \brief Create a track based on the given track type and container.
 *
 *  \param _tt The type of track to create
 *  \param _tc The track container to attach to
 */
Track * Track::create( TrackTypes _tt, TrackContainer * _tc )
{
	Track * t = NULL;
	switch( _tt )
	{
		case InstrumentTrack: t = new ::InstrumentTrack( _tc ); break;
		case BBTrack: t = new BbTrack( _tc ); break;
		case SampleTrack: t = new ::SampleTrack( _tc ); break;
		/*case AutomationTrack: t = new automationTrack( _tc ); break;
		case HiddenAutomationTrack:
				t = new automationTrack( _tc, true ); break;*/
		default: break;
	}
	// TODO{TNG}: Huh?
	//_tc->updateAfterTrackAdd();

	return t;
}




/*! \brief Create a track inside trackContainer from track type in a QDomElement and restore state from XML
 *
 *  \param _this The QDomElement containing the type of track to create
 *  \param _tc The track container to attach to
 */
Track * Track::create( const QDomElement & _this, TrackContainer * _tc )
{
	Track * t = create(
		static_cast<TrackTypes>( _this.attribute( "type" ).toInt() ),
									_tc );
	if( t != NULL )
	{
		t->restoreState( _this );
	}
	return t;
}




/*! \brief Clone a track from this track
 *
 */
void Track::clone()
{
	QDomDocument doc;
	QDomElement parent = doc.createElement( "clone" );
	saveState( doc, parent );
	create( parent.firstChild().toElement(), m_trackContainer );
}






/*! \brief Save this track's settings to file
 *
 *  We save the track type and its muted state, then append the track-
 *  specific settings.  Then we iterate through the trackContentObjects
 *  and save all their states in turn.
 *
 *  \param _doc The QDomDocument to use to save
 *  \param _this The The QDomElement to save into
 *  \todo Does this accurately describe the parameters?  I think not!?
 *  \todo Save the track height
 */
void Track::saveSettings( QDomDocument & _doc, QDomElement & _this )
{
	if( !m_simpleSerializingMode )
	{
		_this.setTagName( "track" );
	}
	_this.setAttribute( "type", type() );
	_this.setAttribute( "name", name() );
	_this.setAttribute( "muted", isMuted() );
// ### TODO
//	_this.setAttribute( "height", m_trackView->height() );

	QDomElement ts_de = _doc.createElement( nodeName() );
	// let actual track (InstrumentTrack, bbTrack, sampleTrack etc.) save
	// its settings
	_this.appendChild( ts_de );
	saveTrackSpecificSettings( _doc, ts_de );

	if( m_simpleSerializingMode )
	{
		m_simpleSerializingMode = false;
		return;
	}

	// now save settings of all TCO's
	for( SegmentVector::const_iterator it = m_trackSegments.begin();
				it != m_trackSegments.end(); ++it )
	{
		( *it )->saveState( _doc, _this );
	}
}




/*! \brief Load the settings from a file
 *
 *  We load the track's type and muted state, then clear out our
 *  current trackContentObject.
 *
 *  Then we step through the QDomElement's children and load the
 *  track-specific settings and trackContentObjects states from it
 *  one at a time.
 *
 *  \param _this the QDomElement to load track settings from
 *  \todo Load the track height.
 */
void Track::loadSettings( const QDomElement & _this )
{
	if( _this.attribute( "type" ).toInt() != type() )
	{
		qWarning( "Current track-type does not match track-type of "
							"settings-node!\n" );
	}

	setName( _this.hasAttribute( "name" ) ? _this.attribute( "name" ) :
			_this.firstChild().toElement().attribute( "name" ) );

	setMuted( _this.attribute( "muted" ).toInt() );

	if( m_simpleSerializingMode )
	{
		QDomNode node = _this.firstChild();
		while( !node.isNull() )
		{
			if( node.isElement() && node.nodeName() == nodeName() )
			{
				loadTrackSpecificSettings( node.toElement() );
				break;
			}
			node = node.nextSibling();
		}
		m_simpleSerializingMode = false;
		return;
	}

	while( !m_trackSegments.empty() )
	{
		delete m_trackSegments.front();
//		m_trackContentObjects.erase( m_trackContentObjects.begin() );
	}

	QDomNode node = _this.firstChild();
	while( !node.isNull() )
	{
		if( node.isElement() )
		{
			if( node.nodeName() == nodeName() )
			{
				loadTrackSpecificSettings( node.toElement() );
			}
			else if(
			!node.toElement().attribute( "metadata" ).toInt() )
			{
				TrackSegment * segment = createSegment(
								midiTime( 0 ) );
				segment->restoreState( node.toElement() );
				saveJournallingState( false );
				restoreJournallingState();
			}
		}
		node = node.nextSibling();
        }
/*
	if( _this.attribute( "height" ).toInt() >= MINIMAL_TRACK_HEIGHT )
	{
		m_trackView->setFixedHeight(
					_this.attribute( "height" ).toInt() );
	}*/
}




/*! \brief Return the number of trackContentObjects we contain
 *
 *  \return the number of trackContentObjects we currently contain.
 */
int Track::countSegments()
{
	return m_trackSegments.size();
}



/*! \brief Get a trackContentObject by number
 *
 *  If the TCO number is less than our TCO array size then fetch that
 *  numbered object from the array.  Otherwise we warn the user that
 *  we've somehow requested a TCO that is too large, and create a new
 *  TCO for them.
 *  \param _tco_number The number of the trackContentObject to fetch.
 *  \return the given trackContentObject or a new one if out of range.
 *  \todo reject TCO numbers less than zero.
 *  \todo if we create a TCO here, should we somehow attach it to the
 *     track?
 */
TrackSegment * Track::getSegment( int _segmentNum )
{
	if( _segmentNum < m_trackSegments.size() )
	{
		return m_trackSegments[_segmentNum];
	}
	printf( "called track::getTCO( %d ), "
			"but TCO %d doesn't exist\n", _segmentNum, _segmentNum );
	return createSegment( _segmentNum * midiTime::ticksPerTact() );

}




/*! \brief Determine the given trackContentObject's number in our array.
 *
 *  \param _tco The trackContentObject to search for.
 *  \return its number in our array.
 */
int Track::getSegmentNum( TrackSegment * _segment )
{
//	for( int i = 0; i < getTrackContentWidget()->numOfTCOs(); ++i )
	SegmentVector::iterator it = qFind( m_trackSegments.begin(),
					m_trackSegments.end(),
					_segment );
	if( it != m_trackSegments.end() )
	{
/*		if( getTCO( i ) == _tco )
		{
			return i;
		}*/
		return it - m_trackSegments.begin();
	}
	qWarning( "track::getTCONum(...) -> _tco not found!\n" );
	return 0;
}


/*! \brief Invert the track's solo state.
 *
 *  We have to go through all the tracks determining if any other track
 *  is already soloed.  Then we have to save the mute state of all tracks,
 *  and set our mute state to on and all the others to off.
 */
void Track::toggleSolo()
{
	/* TODO{TNG}: Move this code to TrackContainer
	const TrackContainer::TrackList & tl = m_trackContainer->tracks();

	bool soloBefore = false;
	for( TrackContainer::TrackList::const_iterator it = tl.begin();
							it != tl.end(); ++it )
	{
		if( *it != this )
		{
			if( ( *it )->m_solo.value() )
			{
				soloBefore = true;
				break;
			}
		}
	}

	const bool solo = m_solo.value();
	for( TrackContainer::TrackList::const_iterator it = tl.begin();
							it != tl.end(); ++it )
	{
		if( solo )
		{
			// save mute-state in case no track was solo before
			if( !soloBefore )
			{
				( *it )->m_mutedBeforeSolo = ( *it )->isMuted();
			}
			( *it )->setMuted( *it == this ? false : true );
			if( *it != this )
			{
				( *it )->m_solo.setValue( false );
			}
		}
		else if( !soloBefore )
		{
			( *it )->setMuted( ( *it )->m_mutedBeforeSolo );
		}
	}
	*/
}




void Track::addSegment( TrackSegment * _segment )
{
	m_trackSegments.append( _segment );
	emit trackSegmentAdded( _segment );
}




#include "moc_Track.cxx"


