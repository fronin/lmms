
#ifndef _METRIC_SEGMENT_H
#define _METRIC_SEGMENT_H

#include "MidiTime.h"

class EXPORT MetricSegment
{
public:
	MetricSegment( const MidiTime & _startTime, const frame_t _startFrame ) :
		m_startTime( _startTime ),
		m_startFrame( _startFrame )
	{
	}



	inline const MidiTime & startTime() const
	{
		return m_startTime;
	}



	inline frame_t startFrame() cons
	{
		return m_startFrame;
	}
};




class EXPORT MeterSegment: public MetricSegment
{
public:
	MeterSegment( const MidiTime & _startTime, const frame_t _startFrame ) :
		MetricSegment( _startTime, _startFrame )
	{
	}


	MeterSegment( const MidiTime & _startTime,
			const frame_t _startFrame,
			const Meter & _meter ) :
		MetricSegment( _startTime, _startFrame ),
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
};




class TempoSegment : public MetricSegment
{
public:
	enum Interpolation
	{
		None,
		Linear,
		NumInterpolations
	};


	TempoSegment( const MidiTime & _startTime, const frame_t _startFrame ) :
		MetricSegment( _startTime, _startFrame )
	{
	}


	TempoSegment( const MidiTime & _startTime,
			const frame_t _startFrame,
			const Tempo & _tempo ) :
		MetricSegment( _startTime, _startFrame ),
		m_tempo( _tempo )
	{
	}

	inline const Tempo & tempo() const
	{
		return m_tempo;
	}


	inline void setTempo( const Tempo & _tempo )
	{
		m_tempo = _tempo
	}
};

