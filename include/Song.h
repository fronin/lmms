#ifndef _SONG_H
#define _SONG_H

#include <QtCore/QVector>

#include "TrackContainer.h"
#include "AutomatableModel.h"
#include "Controller.h"
#include "MeterModel.h"  // Remove?
#include "MetricMap.h"


class automationTrack;
class Pattern;
class timeLine;

// Already defined in song.h ATM
//const bpm_t MinTempo = 10;
//const bpm_t DefaultTempo = 140;
//const bpm_t MaxTempo = 999;
//const tick_t MaxSongLength = 9999 * DefaultTicksPerTact;


class EXPORT Song : public TrackContainer
{
	Q_OBJECT
	// TODO{TNG} Something has to happen here... these models are time-based
	//mapPropertyFromModel(int,getTempo,setTempo,m_tempoModel);
	mapPropertyFromModel(int,masterPitch,setMasterPitch,m_masterPitchModel);
	mapPropertyFromModel(int,masterVolume,setMasterVolume,m_masterVolumeModel);
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


	MetricMap & metricMap() const
	{
		return * m_metricMap;
	}


public slots:
	// file management
	void createNewProjectFromTemplate( const QString & _template );
	void loadProject( const QString & _file_name );
	bool saveProject( void );
	bool saveProjectAs( const QString & _file_name );

	void createNewProject( void );
	void importProject( void );
	void exportProject( void );

	void setModified( void );

	void clearProject( void );

	Track * addTrack( Track::TrackTypes _type );

private slots:
	void insertBar( void );
	void removeBar( void );
	//void addBbTrack( void );
	//void addSampleTrack( void );
	//void addAutomationTrack( void );

	//void setTempo( void );
	//void setTimeSignature( void );

	//void masterVolumeChanged( void );

	//void updateFramesPerTick( void );



private:
	Song( void );
	Song( const Song & );
	virtual ~Song();

	/* TODO! ???? */
	void saveControllerStates( QDomDocument & _doc, QDomElement & _this );
	void restoreControllerStates( const QDomElement & _this );

	automationTrack * m_globalAutomationTrack;
	MetricMap * m_metricMap;

	IntModel m_masterVolumeModel;
	IntModel m_masterPitchModel;

	ControllerVector m_controllers;

	QString m_fileName;
	QString m_oldFileName;
	bool m_modified;

	bool m_loadingProject;

	tact_t m_length;


	/* TODO! Remove as many friends as possible */
	friend class engine;
	friend class MainWindow;
	friend class ControllerRackView;
	friend class Sequencer;

signals:
	void lengthChanged( int _tacts );
/*	void tempoChanged( bpm_t _new_bpm );
	void timeSignatureChanged( int _old_ticks_per_tact,
							int _ticks_per_tact );*/

} ;


#endif
