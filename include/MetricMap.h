#ifndef _METRIC_MAP_H
#define _METRIC_MAP_H

#include "Tempo.h"
#include "lmms_basics.h"
#include "SerializingObject.h"

class MetricSegment;
class TempoSegment;
class MeterSegment;


class MetricMap : SerializingObject
{
public:
	MetricMap( sample_rate_t _rate );

	const Meter & meterAt( f_cnt_t _where );
	const Tempo & tempoAt( f_cnt_t _where );

	void addMeter( const Meter & _meter, MidiTime _where );
	void addMeter( const Meter & _meter, f_cnt_t _where );

	void addTempo( const Tempo & _tempo, MidiTime _where );
	void addTempo( const Tempo & _tempo, f_cnt_t _where );

	// TODO: move remove and replace functions..

	// TODO: Add direction parameter?  and a NearestDivisionOfBeat?
	f_cnt_t toNearestBar( f_cnt_t _frame );
	f_cnt_t toNearestBeat( f_cnt_t _frame );
	f_cnt_t toNearestTick( f_cnt_t _frame );

	virtual void saveSettings( QDomDocument & _doc, QDomElement & _this );
	virtual void loadSettings( const QDomElement & _this );

	static const Tempo & defaultTempo()
	{
		return s_defaultTempo;
	}

	static const Meter & defaultMeter()
	{
		return s_defaultMeter;
	}

private:
	enum Calculation { FromFrame, FromMidiTime };

	void add( MetricSegment * _segment, Calculation _from );
	void recalculate( Calculation _from );

	static Tempo s_defaultTempo;
	static Meter s_defaultMeter;

	sample_rate_t m_sampleRate;

	QVector<MetricSegment *> m_segments;
	TempoSegment * m_firstTempoSegment;
	MeterSegment * m_firstMeterSegment;
};

#endif
