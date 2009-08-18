/*
 * Sequencer.cpp - main renderer of a song
 *
 * Copyright (c) 2009      Paul Giblock    <pgib/at/users.sourceforge.net>
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


#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>

#include <math.h>

// TODO: Remove unused includes
#include "Sequencer.h"
#include "Song.h"
#include "automation_track.h"
#include "automation_editor.h"
#include "automation_recorder.h"
#include "bb_editor.h"
#include "bb_track.h"
#include "bb_track_container.h"
#include "config_mgr.h"
#include "ControllerRackView.h"
#include "ControllerConnection.h"
#include "embed.h"
#include "envelope_and_lfo_parameters.h"
#include "export_project_dialog.h"
#include "fx_mixer.h"
#include "fx_mixer_view.h"
#include "import_filter.h"
#include "instrument_track.h"
#include "main_window.h"
#include "MidiClient.h"
#include "mmp.h"
#include "note_play_handle.h"
#include "pattern.h"
#include "piano_roll.h"
#include "project_journal.h"
#include "project_notes.h"
#include "ProjectRenderer.h"
#include "rename_dialog.h"
#include "song_editor.h"
#include "templates.h"
#include "text_float.h"
#include "timeline.h"


Sequencer::Sequencer() :
	m_recording( false ),
	m_exporting( false ),
	m_playing( false ),
	m_paused( false ),
	m_playMode( Mode_PlaySong ),
	m_trackToPlay( NULL ),
	m_patternToPlay( NULL ),
	m_loopPattern( false )
{
	connect( engine::getMixer(), SIGNAL( sampleRateChanged() ), this,
						SLOT( updateFramesPerTick() ) );

	qRegisterMetaType<note>( "note" );
}




Sequencer::~Sequencer()
{
	m_playing = false;
}




void Sequencer::doActions()
{
	while( !m_actions.empty() )
	{
		switch( m_actions.front() )
		{
		case ActionStop:
			{
				timeLine * tl =
					m_playPos[m_playMode].m_timeLine;
				m_playing = false;
				m_recording = true;
				if( tl != NULL )
				{
					switch( tl->behaviourAtStop() )
					{
					case timeLine::BackToZero:
						m_playPos[m_playMode].setTicks( 0 );
						break;

					case timeLine::BackToStart:
						if( tl->savedPos() >= 0 )
						{
							m_playPos[m_playMode].setTicks(
								tl->savedPos().getTicks() );
							tl->savePos( -1 );
						}
						break;

					case timeLine::KeepStopPosition:
					default:
						break;
					}
				}
				else
				{
					m_playPos[m_playMode].setTicks( 0 );
				}

				m_playPos[m_playMode].setCurrentFrame( 0 );

				// remove all note-play-handles that are active
				engine::getMixer()->clear();

				break;
			}

		case ActionPlaySong:
			m_playMode = Mode_PlaySong;
			m_playing = true;
			Controller::resetFrameCounter();
			break;

		case ActionPlayTrack:
			m_playMode = Mode_PlayTrack;
			m_playing = true;
			break;

		case ActionPlayBB:
			m_playMode = Mode_PlayBB;
			m_playing = true;
			break;

		case ActionPlayPattern:
			m_playMode = Mode_PlayPattern;
			m_playing = true;
			break;

		case ActionPause:
			m_playing = false;// just set the play-flag
			m_paused = true;
			break;

		case ActionResumeFromPause:
			m_playing = true;// just set the play-flag
			m_paused = false;
			break;
		}

		// a second switch for saving pos when starting to play
		// anything (need pos for restoring it later in certain
		// timeline-modes)
		switch( m_actions.front() )
		{
		case ActionPlaySong:
		case ActionPlayTrack:
		case ActionPlayBB:
		case ActionPlayPattern:
			{
				timeLine * tl =
					m_playPos[m_playMode].m_timeLine;
				if( tl != NULL )
				{
					tl->savePos( m_playPos[m_playMode] );
				}
				break;
			}

		// keep GCC happy...
		default:
			break;
		}

		m_actions.erase( m_actions.begin() );

	}

}




void Sequencer::processNextBuffer()
{
	doActions();

	if( m_playing == false )
	{
		return;
	}

	trackContainer::trackList track_list;
	int tco_num = -1;

	switch( m_playMode )
	{
		case Mode_PlaySong:
			track_list = m_song->tracks();
			// at song-start we have to reset the LFOs
			if( m_playPos[Mode_PlaySong] == 0 )
			{
				envelopeAndLFOParameters::resetLFO();
			}
			break;

		case Mode_PlayTrack:
			track_list.push_back( m_trackToPlay );
			break;

		case Mode_PlayBB:
			if( engine::getBBTrackContainer()->numOfBBs() > 0 )
			{
				tco_num = engine::getBBTrackContainer()->
								currentBB();
				track_list.push_back( bbTrack::findBBTrack(
								tco_num ) );
			}
			break;

		case Mode_PlayPattern:
			if( m_patternToPlay != NULL )
			{
				tco_num = m_patternToPlay->getTrack()->
						getTCONum( m_patternToPlay );
				track_list.push_back(
						m_patternToPlay->getTrack() );
			}
			break;

		default:
			return;

	}

	if( track_list.empty() == true )
	{
		return;
	}

	// check for looping-mode and act if neccessary
	timeLine * tl = m_playPos[m_playMode].m_timeLine;
	bool check_loop = tl != NULL && m_exporting == false &&
				tl->loopPointsEnabled() &&
				!( m_playMode == Mode_PlayPattern &&
					m_patternToPlay->freezing() == true );
	if( check_loop )
	{
		if( m_playPos[m_playMode] < tl->loopBegin() ||
					m_playPos[m_playMode] >= tl->loopEnd() )
		{
			m_playPos[m_playMode].setTicks(
						tl->loopBegin().getTicks() );
		}
	}

	f_cnt_t total_frames_played = 0;
	const float frames_per_tick = engine::framesPerTick();

	// TODO: PRG: Rework this whole block for tempo-mapping
	while( total_frames_played
				< engine::getMixer()->framesPerPeriod() )
	{
		f_cnt_t played_frames = engine::getMixer()
				->framesPerPeriod() - total_frames_played;

		float current_frame = m_playPos[m_playMode].currentFrame();
		// did we play a tick?
		if( current_frame >= frames_per_tick )
		{
			int ticks = m_playPos[m_playMode].getTicks()
				+ (int)( current_frame / frames_per_tick );
			// did we play a whole tact?
			if( ticks >= midiTime::ticksPerTact() )
			{
				// per default we just continue playing even if
				// there's no more stuff to play
				// (song-play-mode)
				int max_tact = m_playPos[m_playMode].getTact()
									+ 2;

				// then decide whether to go over to next tact
				// or to loop back to first tact
				if( m_playMode == Mode_PlayBB )
				{
					max_tact = engine::getBBTrackContainer()
							->lengthOfCurrentBB();
				}
				else if( m_playMode == Mode_PlayPattern &&
					m_loopPattern == true &&
					tl != NULL &&
					tl->loopPointsEnabled() == false )
				{
					max_tact = m_patternToPlay->length()
								.getTact();
				}

				// end of played object reached?
				if( m_playPos[m_playMode].getTact() + 1
								>= max_tact )
				{
					// then start from beginning and keep
					// offset
					ticks = ticks % ( max_tact *
						midiTime::ticksPerTact() );
				}
			}
			m_playPos[m_playMode].setTicks( ticks );

			if( check_loop )
			{
				if( m_playPos[m_playMode] >= tl->loopEnd() )
				{
					m_playPos[m_playMode].setTicks(
						tl->loopBegin().getTicks() );
				}
			}

			current_frame = fmodf( current_frame, frames_per_tick );
			m_playPos[m_playMode].setCurrentFrame( current_frame );
		}

		f_cnt_t last_frames = (f_cnt_t)frames_per_tick -
						(f_cnt_t) current_frame;
		// skip last frame fraction
		if( last_frames == 0 )
		{
			++total_frames_played;
			m_playPos[m_playMode].setCurrentFrame( current_frame
								+ 1.0f );
			continue;
		}
		// do we have some samples left in this tick but these are
		// less then samples we have to play?
		if( last_frames < played_frames )
		{
			// then set played_samples to remaining samples, the
			// rest will be played in next loop
			played_frames = last_frames;
		}

		if( (f_cnt_t) current_frame == 0 )
		{
			if( m_playMode == Mode_PlaySong )
			{
				m_song->m_globalAutomationTrack->play(
						m_playPos[m_playMode],
						played_frames,
						total_frames_played, tco_num );
			}

			// loop through all tracks and play them
			for( int i = 0; i < track_list.size(); ++i )
			{
				track_list[i]->play( m_playPos[m_playMode],
						played_frames,
						total_frames_played, tco_num );
			}
		}

		// update frame-counters
		total_frames_played += played_frames;
		m_playPos[m_playMode].setCurrentFrame( played_frames +
								current_frame );
	}
}




bool Sequencer::realTimeTask() const
{
	return !( m_exporting == true || ( m_playMode == Mode_PlayPattern &&
		  	m_patternToPlay != NULL &&
			m_patternToPlay->freezing() == true ) );
}




void Sequencer::play( Song * _song )
{
	m_recording = false;
	if( m_playing == true )
	{
		if( m_playMode != Mode_PlaySong )
		{
			// make sure, bb-editor updates/resets it play-button
			engine::getBBTrackContainer()->stop();
			//pianoRoll::inst()->stop();
		}
		else
		{
			pause();
			return;
		}
	}
	m_actions.push_back( ActionPlaySong );
}




void Sequencer::record()
{
	m_recording = true;
	// TODO: Implement
}




void Sequencer::playAndRecord( Song * _song )
{
	play( _song );
	m_recording = true;
}




void Sequencer::playTrack( track * _trackToPlay )
{
	if( m_playing == true )
	{
		stop();
	}
	m_trackToPlay = _trackToPlay;

	m_actions.push_back( ActionPlayTrack );
}




void Sequencer::playBB()
{
	if( m_playing == true )
	{
		stop();
	}
	m_actions.push_back( ActionPlayBB );
}




void Sequencer::playPattern( pattern * _patternToPlay, bool _loop )
{
	if( m_playing == true )
	{
		stop();
	}
	m_patternToPlay = _patternToPlay;
	m_loopPattern = _loop;
	if( m_patternToPlay != NULL )
	{
		m_actions.push_back( ActionPlayPattern );
	}
}




void Sequencer::setPlayPos( tick_t _ticks, PlayModes _play_mode )
{
	m_playPos[_play_mode].setTicks( _ticks );
	m_playPos[_play_mode].setCurrentFrame( 0.0f );
}




void Sequencer::stop()
{
	m_actions.push_back( ActionStop );

	engine::getAutomationRecorder()->initRecord();
}






void Sequencer::pause()
{
	m_actions.push_back( ActionPause );
}




void Sequencer::resumeFromPause()
{
	m_actions.push_back( ActionResumeFromPause );
}




void Sequencer::startExport( Song * _song )
{
	stop();
	doActions();

	play( _song );
	doActions();

	m_exporting = true;
}




void Sequencer::stopExport()
{
	stop();
	m_exporting = false;
}




void Sequencer::updateFramesPerTick()
{
	engine::updateFramesPerTick();
}






#include "moc_Sequencer.cxx"


