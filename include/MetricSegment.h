#ifndef _METRIC_SEGMENT_H
#define _METRIC_SEGMENT_H

#include "MidiTime.h"
#include "lmms_basics.h"

class EXPORT MetricSegment
{
public:
	MetricSegment( const MidiTime & _startTime ) :
		m_startTime( _startTime ),
		m_startFrame( 0 )
	{
	}


	MetricSegment( const f_cnt_t _startFrame ) :
		m_startTime( MidiTime( 0, 0, 0 ) ),
		m_startFrame( _startFrame )
	{
	}


	virtual ~MetricSegment()
	{
	}


	inline const MidiTime & time() const
	{
		return m_startTime;
	}



	inline f_cnt_t frame() const
	{
		return m_startFrame;
	}



	void setFrame( f_cnt_t _frame )
	{
		m_startFrame = _frame;
	}



	void setTime( const MidiTime & _time)
	{
		m_startTime = _time;
	}


protected:
	MidiTime m_startTime;
	f_cnt_t m_startFrame;
};




class EXPORT MeterSegment: public MetricSegment
{
public:
	MeterSegment( const MidiTime & _startTime,
			const Meter & _meter ) :
		MetricSegment( _startTime ),
		m_meter( _meter )
	{
	}


	MeterSegment( const f_cnt_t _startFrame,
			const Meter & _meter ) :
		MetricSegment( _startFrame ),
		m_meter( _meter )
	{
	}


	inline const Meter & meter() const
	{
		return m_meter;
	}


	inline void setMeter( const Meter & _meter )
	{
		m_meter = _meter;
	}


protected:
	Meter m_meter;
};




class EXPORT TempoSegment : public MetricSegment
{
public:
	enum Interpolation
	{
		None,
		Linear,
		NumInterpolations
	};


	TempoSegment( const MidiTime & _startTime,
			const Tempo & _tempo ) :
		MetricSegment( _startTime ),
		m_tempo( _tempo )
	{
	}


	TempoSegment( const f_cnt_t _startFrame,
			const Tempo & _tempo ) :
		MetricSegment( _startFrame ),
		m_tempo( _tempo )
	{
	}

	inline const Tempo & tempo() const
	{
		return m_tempo;
	}


	inline void setTempo( const Tempo & _tempo )
	{
		m_tempo = _tempo;
	}


protected:
	Tempo m_tempo;
};

#endif
