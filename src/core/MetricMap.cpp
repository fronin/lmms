#include <QVector>

#include "MetricMap.h"
#include "MetricSegment.h"

Tempo MetricMap::s_defaultTempo = Tempo( 120 );
Meter MetricMap::s_defaultMeter = Meter( 4, 4 );

MetricMap::MetricMap( sample_rate_t _rate )
{
	m_sampleRate = _rate;
	m_firstTempoSegment = new TempoSegment( 0, defaultTempo() );
	m_firstMeterSegment = new MeterSegment( 0, defaultMeter() );

	m_segments.append( m_firstTempoSegment );
	m_segments.append( m_firstMeterSegment );
}


void MetricMap::addMeter( const Meter & _meter, MidiTime _where )
{
	add( new MeterSegment( _where, _meter ), FromMidiTime );
}


void MetricMap::addMeter( const Meter & _meter, f_cnt_t _where )
{
	add( new MeterSegment( _where, _meter ), FromFrame );
}


void MetricMap::addTempo( const Tempo & _tempo, MidiTime _where )
{
	add( new TempoSegment( _where, _tempo ), FromMidiTime );
}


void MetricMap::addTempo( const Tempo & _tempo, f_cnt_t _where )
{
	add( new TempoSegment( _where, _tempo ), FromFrame );
}



void MetricMap::add( MetricSegment * _segment, Calculation _from )
{
	QMutableVectorIterator<MetricSegment *> i( m_segments );
	while( i.hasNext() )
	{
		MetricSegment * ms = i.next();

		if( _from == FromMidiTime )
		{
			if( ms->time() < _segment->time() )
				break;
		}
		else if( _from == FromFrame )
		{
			if( ms->frame() < _segment->frame() )
				break;
		}
	}

	i.insert( _segment );
	recalculate( _from );
}



void MetricMap::recalculate( Calculation _from )
{
	QMutableVectorIterator<MetricSegment *> i( m_segments );
	MetricSegment * ms;

	const Meter * meter = &m_firstMeterSegment->meter();
	const Tempo * tempo = &m_firstTempoSegment->tempo();
	MeterSegment * castedMeter;
	TempoSegment * castedTempo;

	if( _from == FromMidiTime )
	{
		f_cnt_t currentFrame = 0;
		MidiTime currentTime;
		MidiTime prevTime;

		while( i.hasNext() )
		{
			ms = i.next();

			currentTime = ms->time();

			// BEGIN Calculate frames for this segment
			int beats, frames;
			bar_t bar = prevTime.bar();
			beat_t beat = prevTime.beat();

			// Sum up beats for partial-first and any full bars
			while( bar < currentTime.bar() ) {
				while( beat < meter->beatsPerBar() ) {
					++beat;
					++beats;
				}
				beat = 0;
			}
			// Now sum up the possible incomplete bar at the end
			while( beat < currentTime.beat() ) {
				++beat;
				++beats;
			}

			currentFrame += tempo->framesPerBeat( *meter, m_sampleRate ) * beats;

			// Now assign frames, and get ready for next segment
			ms->setFrame( currentFrame );
			prevTime = currentTime;

			if( (castedTempo = dynamic_cast<TempoSegment *>(ms)) != NULL )
			{
				tempo = &castedTempo->tempo();
			}
			else if ( (castedMeter = dynamic_cast<MeterSegment *>(ms)) != NULL )
			{
				meter = &castedMeter->meter();
			}
		}
	}
	else if( _from == FromFrame )
	{
		// TODO: All of this logic needs to be rewritten most likely to
		// properly handle remainders, rounding-errors, etc..
		f_cnt_t currentFrame;
		f_cnt_t prevFrame = 0;
		MidiTime prevTime;

		while( i.hasNext() )
		{
			ms = i.next();

			currentFrame = ms->frame();

			f_cnt_t frameLen = currentFrame - prevFrame;
			float beatLen = frameLen / tempo->framesPerBeat( *meter, m_sampleRate );
			int bars = beatLen / meter->beatsPerBar();
			int beats = beatLen - ( bars * meter->beatsPerBar() );

			// Now assign time, and get ready for next segment
			prevFrame = currentFrame;
			prevTime = MidiTime(
					prevTime.bar() + bars, 
					prevTime.beat() + beats,
					prevTime.ticks() );
			
			ms->setTime( prevTime );

			if( (castedTempo = dynamic_cast<TempoSegment *>(ms)) != NULL )
			{
				tempo = &castedTempo->tempo();
			}
			else if ( (castedMeter = dynamic_cast<MeterSegment *>(ms)) != NULL)
			{
				meter = &castedMeter->meter();
			}
		}
	}
}


void MetricMap::saveSettings( QDomDocument & _doc, QDomElement & _this )
{
}


void MetricMap::loadSettings( const QDomElement & _this )
{
}
