/*
 * song.h - class song - the root of the model-tree
 *
 * Copyright (c) 2004-2011 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef _SONG_H
#define _SONG_H

#include <QtCore/QVector>

#include "track_container.h"
#include "AutomatableModel.h"
#include "Controller.h"
#include "MeterModel.h"


class AutomationTrack;
class pattern;
class timeLine;


const bpm_t MinTempo = 10;
const bpm_t DefaultTempo = 140;
const bpm_t MaxTempo = 999;
const tick_t MaxSongLength = 9999 * DefaultTicksPerTact;


class EXPORT song : public trackContainer
{
	Q_OBJECT
	mapPropertyFromModel(int,getTempo,setTempo,m_tempoModel);
	mapPropertyFromModel(int,masterPitch,setMasterPitch,m_masterPitchModel);
	mapPropertyFromModel(int,masterVolume,setMasterVolume,
							m_masterVolumeModel);
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
			m_timeLine( NULL ),
			m_timeLineUpdate( true ),
			m_currentFrame( 0.0f )
		{
		}
		inline void setCurrentFrame( const float _f )
		{
			m_currentFrame = _f;
		}
		inline float currentFrame() const
		{
			return m_currentFrame;
		}
		timeLine * m_timeLine;
		bool m_timeLineUpdate;

	private:
		float m_currentFrame;

	} ;



	void processNextBuffer();


	inline bool isPaused() const
	{
		return m_paused;
	}

	inline bool isPlaying() const
	{
		return m_playing && m_exporting == false;
	}

	bool isFreezingPattern() const;

	inline bool isExporting() const
	{
		return m_exporting;
	}

	inline void setExportLoop(bool export_loop)
	{
		m_export_loop = export_loop;
	}

	inline bool isRecording() const
	{
		return m_recording;
	}

	bool realTimeTask() const;

	inline bool isExportDone() const
	{
		if ( m_export_loop )
		{
			return m_exporting == true &&
				m_playPos[Mode_PlaySong].getTact() >= length();
		}
		else
		{
			return m_exporting == true &&
				m_playPos[Mode_PlaySong].getTact() >= length() + 1;
		}
	}

	inline PlayModes playMode() const
	{
		return m_playMode;
	}

	inline playPos & getPlayPos( PlayModes _pm )
	{
		return m_playPos[_pm];
	}

	void updateLength();
	tact_t length() const
	{
		return m_length;
	}


	bpm_t getTempo();
	virtual AutomationPattern * tempoAutomationPattern();

	AutomationTrack * globalAutomationTrack()
	{
		return m_globalAutomationTrack;
	}

	// file management
	void createNewProject();
	void createNewProjectFromTemplate( const QString & _template );
	void loadProject( const QString & _filename );
	bool guiSaveProject();
	bool guiSaveProjectAs( const QString & _filename );
    bool saveProjectFile( const QString & _filename );
	inline const QString & projectFileName() const
	{
		return m_fileName;
	}
	inline bool isLoadingProject() const
	{
		return m_loadingProject;
	}
	inline bool isModified() const
	{
		return m_modified;
	}

	inline virtual QString nodeName() const
	{
		return "song";
	}

	virtual inline bool fixedTCOs() const
	{
		return false;
	}

	void addController( Controller * _c );
	void removeController( Controller * _c );
	

	const ControllerVector & controllers() const
	{
		return m_controllers;
	}


	MeterModel & getTimeSigModel()
	{
		return m_timeSigModel;
	}

	inline void toggleLoopPoints( int _mode )
	{
		emit loopPointsChanged( _mode );
	}
	
public slots:
	void play();
	void record();
	void playAndRecord();
	void stop();
	void playTrack( track * _trackToPlay );
	void playBB();
	void playPattern( pattern * _patternToPlay, bool _loop = true );
	void pause();
	void resumeFromPause();

	void importProject();
	void exportProject();

	void startExport();
	void stopExport();


	void setModified();

	void clearProject();

	void addBBTrack();


private slots:
	void insertBar();
	void removeBar();
	void addSampleTrack();
	void addAutomationTrack();

	void setTempo();
	void setTimeSignature();

	void masterVolumeChanged();

	void doActions();

	void updateFramesPerTick();



private:
	song();
	song( const song & );
	virtual ~song();


	inline int ticksPerTact() const
	{
		return DefaultTicksPerTact *
				m_timeSigModel.getNumerator() /
					 m_timeSigModel.getDenominator();
	}

	inline tact_t currentTact() const
	{
		return m_playPos[m_playMode].getTact();
	}

	inline tick_t currentTick() const
	{
		return m_playPos[m_playMode].getTicks();
	}
	void setPlayPos( tick_t _ticks, PlayModes _play_mode );

	void saveControllerStates( QDomDocument & _doc, QDomElement & _this );
	void restoreControllerStates( const QDomElement & _this );


	AutomationTrack * m_globalAutomationTrack;

	IntModel m_tempoModel;
	MeterModel m_timeSigModel;
	int m_oldTicksPerTact;
	IntModel m_masterVolumeModel;
	IntModel m_masterPitchModel;

	ControllerVector m_controllers;


	QString m_fileName;
	QString m_oldFileName;
	bool m_modified;

	volatile bool m_recording;
	volatile bool m_exporting;
	volatile bool m_export_loop;
	volatile bool m_playing;
	volatile bool m_paused;

	bool m_loadingProject;

	PlayModes m_playMode;
	playPos m_playPos[Mode_Count];
	tact_t m_length;

	track * m_trackToPlay;
	pattern * m_patternToPlay;
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


	friend class engine;
	friend class songEditor;
	friend class MainWindow;
	friend class ControllerRackView;

signals:
	void projectLoaded();
	void lengthChanged( int _tacts );
	void tempoChanged( bpm_t _new_bpm );
	void timeSignatureChanged( int _old_ticks_per_tact,
							int _ticks_per_tact );
	void loopPointsChanged( int _mode );
} ;


#endif
