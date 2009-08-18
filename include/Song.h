#ifndef _SONG_TNG_H
#define _SONG_TNG_H

#include <QtCore/QVector>

#include "track_container.h"
#include "automatable_model.h"
#include "Controller.h"
#include "meter_model.h"


class automationTrack;
class pattern;
class timeLine;

// Already defined in song.h ATM
//const bpm_t MinTempo = 10;
//const bpm_t DefaultTempo = 140;
//const bpm_t MaxTempo = 999;
//const tick_t MaxSongLength = 9999 * DefaultTicksPerTact;


class EXPORT Song : public trackContainer
{
	Q_OBJECT
	//mapPropertyFromModel(int,getTempo,setTempo,m_tempoModel);
	//mapPropertyFromModel(int,masterPitch,setMasterPitch,m_masterPitchModel);
	//mapPropertyFromModel(int,masterVolume,setMasterVolume,
	//						m_masterVolumeModel);
public:

	void updateLength( void );
	tact_t length( void ) const
	{
		return m_length;
	}


	//bpm_t getTempo( void );
	//virtual automationPattern * tempoAutomationPattern( void );

	automationTrack * globalAutomationTrack( void )
	{
		return m_globalAutomationTrack;
	}

	// file management
	void createNewProject( void );
	void createNewProjectFromTemplate( const QString & _template );
	void loadProject( const QString & _file_name );
	bool saveProject( void );
	bool saveProjectAs( const QString & _file_name );

	inline const QString & projectFileName( void ) const
	{
		return m_fileName;
	}

	inline bool isLoadingProject( void ) const
	{
		return m_loadingProject;
	}

	inline bool isModified( void ) const
	{
		return m_modified;
	}

	inline virtual QString nodeName( void ) const
	{
		return "song";
	}

	virtual inline bool fixedTCOs( void ) const
	{
		return false;
	}

	void addController( Controller * _c );
	void removeController( Controller * _c );
	

	const ControllerVector & controllers( void ) const
	{
		return m_controllers;
	}


	/*
	meterModel & getTimeSigModel( void )
	{
		return m_timeSigModel;
	}
	*/


public slots:
	void importProject( void );
	void exportProject( void );

	void setModified( void );

	void clearProject( void );


private slots:
	void insertBar( void );
	void removeBar( void );
	void addBBTrack( void );
	void addSampleTrack( void );
	void addAutomationTrack( void );

	//void setTempo( void );
	//void setTimeSignature( void );

	//void masterVolumeChanged( void );

	//void updateFramesPerTick( void );



private:
	Song( void );
	Song( const Song & );
	virtual ~Song();


	/*
	inline int ticksPerTact( void ) const
	{
		return DefaultTicksPerTact *
				m_timeSigModel.getNumerator() /
					 m_timeSigModel.getDenominator();
	}
	*/

	/* TODO! ???? */
	void saveControllerStates( QDomDocument & _doc, QDomElement & _this );
	void restoreControllerStates( const QDomElement & _this );

	automationTrack * m_globalAutomationTrack;

	//intModel m_tempoModel;
	//meterModel m_timeSigModel;
	//int m_oldTicksPerTact;
	intModel m_masterVolumeModel;
	intModel m_masterPitchModel;

	ControllerVector m_controllers;

	QString m_fileName;
	QString m_oldFileName;
	bool m_modified;

	volatile bool m_recording;
	volatile bool m_exporting;
	volatile bool m_playing;
	volatile bool m_paused;

	bool m_loadingProject;

	tact_t m_length;


	/* TODO! Remove as many friends as possible */
	friend class engine;
	friend class mainWindow;
	friend class ControllerRackView;
	friend class Sequencer;

signals:
	void lengthChanged( int _tacts );
/*	void tempoChanged( bpm_t _new_bpm );
	void timeSignatureChanged( int _old_ticks_per_tact,
							int _ticks_per_tact );*/

} ;


#endif
