/*
 * sequencer.h - main renderer of a song
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


#ifndef _SEQUENCER_H
#define _SEQUENCER_H

#include <QtCore/QVector>

#include "track_container.h"
#include "automatable_model.h"
#include "controller.h"

class automationTrack;
class pattern;
class Song;
class timeLine;

class EXPORT Sequencer : public QObject
{
	Q_OBJECT

public:
	enum PlayModes
	{
		Mode_PlaySong,
		Mode_PlayTrack,
		Mode_PlayBB,
		Mode_PlayPattern,
		Mode_PlayAutomationPattern,
		Mode_Count
	} ;


	class playPos : public midiTime
	{
	public:
		playPos( const int _abs = 0 ) :
			midiTime( _abs ),
//			m_timeLine( NULL ),
//			m_timeLineUpdate( true ),
			m_currentFrame( 0.0f )
		{
		}

        inline void setCurrentFrame( const float _f )
		{
			m_currentFrame = _f;
		}
		
        inline float currentFrame( void ) const
		{
			return m_currentFrame;
		}
		
//		timeLine * m_timeLine;
//		bool m_timeLineUpdate;

	private:
		float m_currentFrame;

	};


	/**
	 * Sets the Song for sequencer to play next.
	 *
	 * @param song The new Song to play
	 * @return false if Song couldn't be set due to playing state
	 *
	inline void setSong( Song song )
	{
		if( isPlaying() )
		{
			return false;
		}
		else {
			m_song = song;
			return true;
		}
	}


	**
	 * Retreive the Song assigned to this Sequencer
	 *
	 * @return the current Song.
	 *
	inline Song song( void ) const
	{
		return m_song;
	}*/


	/**
	 * Process the next buffer according to the song-position
	 */
	void processNextBuffer( void );


	inline bool isPaused( void ) const
	{
		return m_paused;
	}


	inline bool isPlaying( void ) const
	{
		return m_playing && m_exporting == false;
	}

	/* TODO! THESE
	inline bool isExporting( void ) const
	{
		return m_exporting;
	}

	inline bool isRecording( void ) const
	{
		return m_recording;
	}

	inline bool isExportDone( void ) const
	{
		return m_exporting == true &&
			m_playPos[Mode_PlaySong].getTact() >= length() + 1;
	}

	*/

	bool realTimeTask( void ) const;

	inline PlayModes playMode( void ) const
	{
		return m_playMode;
	}

	inline playPos & getPlayPos( PlayModes _pm )
	{
		return m_playPos[_pm];
	}

	/**
	 * Retrieve the current tempo.
	 */
	bpm_t getTempo( void );

	/* TODO! This should probably go to Song??
	virtual automationPattern * tempoAutomationPattern( void );
	

	automationTrack * globalAutomationTrack( void )
	{
		return m_globalAutomationTrack;
	}
	*/


public slots:
	/* TODO! Instead of setSong maybe just make a play( Song * ) ?? */
	void play( Song * _song );
	void record( void );
	void playAndRecord( void );
	void stop( void );
	void playTrack( track * _trackToPlay );
	void playBB( void );
	void playPattern( pattern * _patternToPlay, bool _loop = true );
	void pause( void );
	void resumeFromPause( void );

	void importProject( void );
	void exportProject( void );

	void startExport( void );
	void stopExport( void );


private slots:
	//void setTempo( void );
	//void setTimeSignature( void );

	//void masterVolumeChanged( void );

	/* TODO! ??? */
	void doActions( void );

	void updateFramesPerTick( void );



private:
	Sequencer( void );
	virtual ~Sequencer();


	/*
	inline int ticksPerTact( void ) const
	{
		return DefaultTicksPerTact *
				m_timeSigModel.getNumerator() /
					 m_timeSigModel.getDenominator();
	}*/

	inline tact_t currentTact( void ) const
	{
		return m_playPos[m_playMode].getTact();
	}

	inline tick_t currentTick( void ) const
	{
		return m_playPos[m_playMode].getTicks();
	}
	
	void setPlayPos( tick_t _ticks, PlayModes _play_mode );

	/* TODO! ??? 
	void saveControllerStates( QDomDocument & _doc, QDomElement & _this );
	void restoreControllerStates( const QDomElement & _this );
	*/

	//intModel m_tempoModel;
	//meterModel m_timeSigModel;
	//int m_oldTicksPerTact;
	//intModel m_masterVolumeModel;
	//intModel m_masterPitchModel;



	volatile bool m_recording;
	volatile bool m_exporting;
	volatile bool m_playing;
	volatile bool m_paused;

	PlayModes m_playMode;
	playPos m_playPos[Mode_Count];

	track * m_trackToPlay;
	pattern * m_patternToPlay;
	Song * m_sontToPlay;
	bool m_loopPattern;

	enum Actions
	{
		ActionStop,
		ActionPlaySong,
		ActionPlayTrack,
		ActionPlayBB,
		ActionPlayPattern,
		ActionPause,
		ActionResumeFromPause
	} ;
	QVector<Actions> m_actions;

	/* TODO! Remove as many friends as possible */
	friend class engine;
	friend class songEditor;
	friend class mainWindow;
	friend class controllerRackView;

signals:
	/* Probably still need these ?? */
	void tempoChanged( bpm_t _new_bpm );
	void timeSignatureChanged( int _old_ticks_per_tact,
							int _ticks_per_tact );

} ;


#endif
