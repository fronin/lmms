/*
 * sample_track.cpp - implementation of class sampleTrack, a track which
 *                    provides arrangement of samples
 *
 * Copyright (c) 2005-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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
#include <QtGui/QDropEvent>
#include <QtGui/QMenu>
#include <QtGui/QLayout>
#include <QtGui/QMdiArea>
#include <QtGui/QPainter>
#include <QtGui/QPushButton>

#include "gui_templates.h"
#include "SampleTrack.h"
#include "Song.h"
#include "embed.h"
#include "engine.h"
#include "tooltip.h"
#include "AudioPort.h"
#include "sample_play_handle.h"
#include "sample_record_handle.h"
#include "string_pair_drag.h"
#include "knob.h"
#include "MainWindow.h"
#include "EffectRackView.h"



SampleSegment::SampleSegment( Track * _track ) :
	TrackSegment( _track ),
	m_sampleBuffer( new sampleBuffer )
{
	saveJournallingState( false );
	setSampleFile( "" );
	restoreJournallingState();

	// we need to receive bpm-change-events, because then we have to
	// change length of this TCO
	connect( engine::song(), SIGNAL( tempoChanged( bpm_t ) ),
					this, SLOT( updateLength( bpm_t ) ) );
}




SampleSegment::~SampleSegment()
{
	sharedObject::unref( m_sampleBuffer );
}




void SampleSegment::changeLength( const midiTime & _length )
{
	TrackSegment::changeLength( qMax( static_cast<Sint32>( _length ),
							DefaultTicksPerTact ) );
}




const QString & SampleSegment::sampleFile() const
{
	return m_sampleBuffer->audioFile();
}



void SampleSegment::setSampleBuffer( sampleBuffer * _sb )
{
	sharedObject::unref( m_sampleBuffer );
	m_sampleBuffer = _sb;
	updateLength();

	emit sampleChanged();
}



void SampleSegment::setSampleFile( const QString & _sf )
{
	m_sampleBuffer->setAudioFile( _sf );
	updateLength();

	emit sampleChanged();
}




void SampleSegment::toggleRecord()
{
	m_recordModel.setValue( !m_recordModel.value() );
	emit dataChanged();
}




void SampleSegment::updateLength( bpm_t )
{
	changeLength( sampleLength() );
}




midiTime SampleSegment::sampleLength() const
{
	return (int)( m_sampleBuffer->frames() / engine::framesPerTick() );
}




void SampleSegment::saveSettings( QDomDocument & _doc, QDomElement & _this )
{
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
	_this.setAttribute( "src", sampleFile() );
	if( sampleFile() == "" )
	{
		QString s;
		_this.setAttribute( "data", m_sampleBuffer->toBase64( s ) );
	}
	// TODO: start- and end-frame
}




void SampleSegment::loadSettings( const QDomElement & _this )
{
	if( _this.attribute( "pos" ).toInt() >= 0 )
	{
		movePosition( _this.attribute( "pos" ).toInt() );
	}
	setSampleFile( _this.attribute( "src" ) );
	if( sampleFile().isEmpty() && _this.hasAttribute( "data" ) )
	{
		m_sampleBuffer->loadFromBase64( _this.attribute( "data" ) );
	}
	changeLength( _this.attribute( "len" ).toInt() );
	setMuted( _this.attribute( "muted" ).toInt() );
}





SampleTrack::SampleTrack( TrackContainer * _tc ) :
	Track( Track::SampleTrack, _tc ),
	m_audioPort( tr( "Sample track" ) ),
	m_volumeModel( DefaultVolume, MinVolume, MaxVolume, 1.0, this,
							tr( "Volume" ) )
{
	setName( tr( "Sample track" ) );
}




SampleTrack::~SampleTrack()
{
	engine::getMixer()->removePlayHandles( this );
}




bool SampleTrack::play( const midiTime & _start, const fpp_t _frames,
						const f_cnt_t _offset,
							Sint16 /*_tco_num*/ )
{
	/* TODO{TNG} Reimplement playing
	m_audioPort.effects()->startRunning();
	bool played_a_note = false;	// will be return variable

	for( int i = 0; i < numOfTCOs(); ++i )
	{
		TrackSegment * tco = getTCO( i );
		if( tco->startPosition() != _start )
		{
			continue;
		}
		sampleTCO * st = dynamic_cast<sampleTCO *>( tco );
		if( !st->isMuted() )
		{
			playHandle * handle;
			if( st->isRecord() )
			{
				if( !engine::song()->isRecording() )
				{
					return played_a_note;
				}
				sampleRecordHandle * smpHandle = new sampleRecordHandle( st );
				handle = smpHandle;
			}
			else
			{
				samplePlayHandle * smpHandle = new samplePlayHandle( st );
				smpHandle->setVolumeModel( &m_volumeModel );
				handle = smpHandle;
			}
//TODO: check whether this works
//			handle->setBBTrack( _tco_num );
			handle->setOffset( _offset );
			// send it to the mixer
			engine::getMixer()->addPlayHandle( handle );
			played_a_note = true;
		}
	}

	return played_a_note;
	*/
	return false;
}





TrackSegment * SampleTrack::createSegment( const midiTime & _time )
{
	SampleSegment * ss = new SampleSegment( this );
	addSegment( ss );
	return ss;
}




void SampleTrack::saveTrackSpecificSettings( QDomDocument & _doc,
							QDomElement & _this )
{
	m_audioPort.effects()->saveState( _doc, _this );
#if 0
	_this.setAttribute( "icon", tlb->pixmapFile() );
#endif
	m_volumeModel.saveSettings( _doc, _this, "vol" );
}




void SampleTrack::loadTrackSpecificSettings( const QDomElement & _this )
{
	QDomNode node = _this.firstChild();
	m_audioPort.effects()->clear();
	while( !node.isNull() )
	{
		if( node.isElement() )
		{
			if( m_audioPort.effects()->nodeName() == node.nodeName() )
			{
				m_audioPort.effects()->restoreState( node.toElement() );
			}
		}
		node = node.nextSibling();
	}
	m_volumeModel.loadSettings( _this, "vol" );
}



#include "moc_SampleTrack.cxx"


