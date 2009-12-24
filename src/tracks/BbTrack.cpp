/*
 * bb_track.cpp - implementation of class bbTrack and bbTCO
 *
 * Copyright (c) 2004-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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
#include <QtGui/QColorDialog>
#include <QtGui/QMenu>
#include <QtGui/QPainter>

#include "BbTrack.h"
#include "bb_track_container.h"
#include "embed.h"
#include "engine.h"
#include "gui_templates.h"
#include "Mixer.h"
#include "rename_dialog.h"
#include "Song.h"
#include "templates.h"



BbTrack::infoMap BbTrack::s_infoMap;


BbSegment::BbSegment( Track * _track, unsigned int _color ) :
	TrackSegment( _track ),
	m_color( _color > 0 ? _color : qRgb( 64, 128, 255 ) )
{
	tact_t t = engine::getBBTrackContainer()->lengthOfBB(
					BbTrack::numOfBBTrack( getTrack() ) );
	if( t > 0 )
	{
		saveJournallingState( false );
		changeLength( midiTime( t, 0 ) );
		restoreJournallingState();
	}
}




BbSegment::~BbSegment()
{
}




void BbSegment::saveSettings( QDomDocument & _doc, QDomElement & _this )
{
	_this.setAttribute( "name", name() );
	if( _this.parentNode().nodeName() == "clipboard" )
	{
		_this.setAttribute( "pos", -1 );
	}
	else
	{
		_this.setAttribute( "pos", startPosition() );
	}
	_this.setAttribute( "len", length() );
	_this.setAttribute( "muted", isMuted() );
	_this.setAttribute( "color", m_color );
}




void BbSegment::loadSettings( const QDomElement & _this )
{
	setName( _this.attribute( "name" ) );
	if( _this.attribute( "pos" ).toInt() >= 0 )
	{
		movePosition( _this.attribute( "pos" ).toInt() );
	}
	changeLength( _this.attribute( "len" ).toInt() );
	if( _this.attribute( "muted" ).toInt() != isMuted() )
	{
		toggleMute();
	}

	if( _this.attribute( "color" ).toUInt() != 0 )
	{
		m_color = _this.attribute( "color" ).toUInt();
	}
}





BbTrack::BbTrack( TrackContainer * _tc ) :
	Track( Track::BBTrack, _tc )
{
	int bbNum = s_infoMap.size();
	s_infoMap[this] = bbNum;

	setName( tr( "Beat/Bassline %1" ).arg( bbNum ) );
	engine::getBBTrackContainer()->setCurrentBB( bbNum );
	engine::getBBTrackContainer()->updateComboBox();

	connect( this, SIGNAL( nameChanged() ),
		engine::getBBTrackContainer(), SLOT( updateComboBox() ) );
}




BbTrack::~BbTrack()
{
	engine::getMixer()->removePlayHandles( this );

	const int bb = s_infoMap[this];
	engine::getBBTrackContainer()->removeBB( bb );
	for( infoMap::iterator it = s_infoMap.begin(); it != s_infoMap.end();
									++it )
	{
		if( it.value() > bb )
		{
			--it.value();
		}
	}
	s_infoMap.remove( this );

	// remove us from TC so bbTrackContainer::numOfBBs() returns a smaller
	// value and thus combobox-updating in bbTrackContainer works well
	trackContainer()->removeTrack( this );
	engine::getBBTrackContainer()->updateComboBox();
}




// play _frames frames of given TCO within starting with _start
bool BbTrack::play( const midiTime & _start, const fpp_t _frames,
				const f_cnt_t _offset, Sint16 _tco_num )
{
	/* TODO{TNG} Reimplement playing code
	if( isMuted() )
	{
		return( false );
	}

	if( _tco_num >= 0 )
	{
		return( engine::getBBTrackContainer()->play( _start, _frames,
							_offset,
							s_infoMap[this] ) );
	}

	tcoVector tcos;
	getTCOsInRange( tcos, _start, _start + static_cast<int>( _frames /
						engine::framesPerTick() ) );

	if( tcos.size() == 0 )
	{
		return( false );
	}

	midiTime lastPosition;
	midiTime lastLen;
	for( tcoVector::iterator it = tcos.begin(); it != tcos.end(); ++it )
	{
		if( !( *it )->isMuted() &&
				( *it )->startPosition() >= lastPosition )
		{
			lastPosition = ( *it )->startPosition();
			lastLen = ( *it )->length();
		}
	}

	if( _start - lastPosition < lastLen )
	{
		return( engine::getBBTrackContainer()->play( _start -
								lastPosition,
							_frames,
							_offset,
							s_infoMap[this] ) );
	}
	*/
	return( false );
}





TrackSegment * BbTrack::createSegment( const midiTime & _pos )
{
	// if we're creating a new bbTCO, we colorize it according to the
	// previous bbTCO, so we have to get all TCOs from 0 to _pos and
	// pickup the last and take the color if it
	SegmentVector tcos;
	/* TODO{TNG} Rethink color code
	getTCOsInRange( tcos, 0, _pos );
	if( tcos.size() > 0 && dynamic_cast<bbTCO *>( tcos.back() ) != NULL )
	{
		return( new bbTCO( this, 
			dynamic_cast<bbTCO *>( tcos.back() )->color() ) );
		
	}
	*/
	return( new BbSegment( this ) );
}






void BbTrack::saveTrackSpecificSettings( QDomDocument & _doc,
							QDomElement & _this )
{
//	_this.setAttribute( "icon", m_trackLabel->pixmapFile() );
/*	_this.setAttribute( "current", s_infoMap[this] ==
					engine::getBBEditor()->currentBB() );*/
	if( s_infoMap[this] == 0 &&
			_this.parentNode().parentNode().nodeName() != "clone" &&
			_this.parentNode().nodeName() != "journaldata" )
	{
		( (JournallingObject *)( engine::getBBTrackContainer() ) )->
						saveState( _doc, _this );
	}
	if( _this.parentNode().parentNode().nodeName() == "clone" )
	{
		_this.setAttribute( "clonebbt", s_infoMap[this] );
	}
}




void BbTrack::loadTrackSpecificSettings( const QDomElement & _this )
{
/*	if( _this.attribute( "icon" ) != "" )
	{
		m_trackLabel->setPixmapFile( _this.attribute( "icon" ) );
	}*/

	/* TODO{TNG} WHAT IS THIS CLONEBBT STUFF ABOUT?
	if( _this.hasAttribute( "clonebbt" ) )
	{
		const int src = _this.attribute( "clonebbt" ).toInt();
		const int dst = s_infoMap[this];
		engine::getBBTrackContainer()->createTCOsForBB( dst );
		TrackContainer::TrackList tl =
					engine::getBBTrackContainer()->tracks();
		for( TrackContainer::TrackList::iterator it = tl.begin();
							it != tl.end(); ++it )
		{
			( *it )->getTCO( src )->copy();
			( *it )->getTCO( dst )->paste();
		}
	}
	else
	*/
	{
		QDomNode node = _this.namedItem(
					TrackContainer::classNodeName() );
		if( node.isElement() )
		{
			( (JournallingObject *)engine::getBBTrackContainer() )->
					restoreState( node.toElement() );
		}
	}
/*	doesn't work yet because bbTrack-ctor also sets current bb so if
	bb-tracks are created after this function is called, this doesn't
	help at all....
	if( _this.attribute( "current" ).toInt() )
	{
		engine::getBBEditor()->setCurrentBB( s_infoMap[this] );
	}*/
}




// return pointer to bbTrack specified by _bb_num
BbTrack * BbTrack::findBBTrack( int _bb_num )
{
	for( infoMap::iterator it = s_infoMap.begin(); it != s_infoMap.end();
									++it )
	{
		if( it.value() == _bb_num )
		{
			return( it.key() );
		}
	}
	return( NULL );
}




int BbTrack::numOfBBTrack( Track * _track )
{
	if( dynamic_cast<BbTrack *>( _track ) != NULL )
	{
		return( s_infoMap[dynamic_cast<BbTrack *>( _track )] );
	}
	return( 0 );
}




void BbTrack::swapBBTracks( Track * _track1, Track * _track2 )
{
	BbTrack * t1 = dynamic_cast<BbTrack *>( _track1 );
	BbTrack * t2 = dynamic_cast<BbTrack *>( _track2 );
	if( t1 != NULL && t2 != NULL )
	{
		qSwap( s_infoMap[t1], s_infoMap[t2] );
		engine::getBBTrackContainer()->swapBB( s_infoMap[t1],
								s_infoMap[t2] );
		engine::getBBTrackContainer()->setCurrentBB( s_infoMap[t1] );
	}
}


#include "moc_BbTrack.cxx"


