#ifndef _METRIC_MAP_H
#define _METRIC_MAP_H

#include <QObject>
#include <QReadWriteLock>

#include "Tempo.h"
#include "lmms_basics.h"
#include "SerializingObject.h"

class MetricSegment;
class TempoSegment;
class MeterSegment;

class MetricBeat;
typedef QList<MetricBeat> MeatList;


class MetricMap : public QObject, SerializingObject
{
	Q_OBJECT
public:
	MetricMap( sample_rate_t _rate );

	QString nodeName() const
	{
		return "metricMap";
	}

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

	f_cnt_t length() const
	{
		return m_length;
	}

	void setLength( f_cnt_t _length )
	{
		m_length = _length;
	}

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

	MeatList meats( f_cnt_t _begin, f_cnt_t _end ) const;

signals:
	void dataChanged( f_cnt_t _begin, f_cnt_t _end );


private:
	enum Calculation { FromFrame, FromMidiTime };

	void add( MetricSegment * _segment, Calculation _from );
	void recalculate( Calculation _from );

	static Tempo s_defaultTempo;
	static Meter s_defaultMeter;

	sample_rate_t m_sampleRate;
	f_cnt_t m_length;

	QVector<MetricSegment *> m_segments;
	TempoSegment * m_firstTempoSegment;
	MeterSegment * m_firstMeterSegment;

	mutable QReadWriteLock m_lock;
};


class MetricBeat {
public:
	MetricBeat( bar_t _bar, beat_t _beat, f_cnt_t _frame,
				const Tempo & _tempo, const Meter & _meter ) :
		bar( _bar ),
		beat( _beat ),
		frame( _frame ),
		tempo( _tempo ),
		meter( _meter )
	{}

	Tempo tempo;
	Meter meter;
	bar_t bar;
	beat_t beat;
	f_cnt_t frame;
};

#endif
