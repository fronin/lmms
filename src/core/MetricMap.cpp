#include <QVector>
#include <QMutex>

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
	QWriteLocker locker( &m_lock );
	add( new MeterSegment( _where, _meter ), FromMidiTime );
}


void MetricMap::addMeter( const Meter & _meter, f_cnt_t _where )
{
	QWriteLocker locker( &m_lock );
	add( new MeterSegment( _where, _meter ), FromFrame );
}


void MetricMap::addTempo( const Tempo & _tempo, MidiTime _where )
{
	QWriteLocker locker( &m_lock );
	add( new TempoSegment( _where, _tempo ), FromMidiTime );
}


void MetricMap::addTempo( const Tempo & _tempo, f_cnt_t _where )
{
	QWriteLocker locker( &m_lock );
	add( new TempoSegment( _where, _tempo ), FromFrame );
}



void MetricMap::add( MetricSegment * _segment, Calculation _from )
{
	QMutableVectorIterator<MetricSegment *> i( m_segments );
	printf( "Adding Segment at: %d %d:\n",
		   _segment->time().bar(),
		   _segment->time().beat() );

	if( _from == FromMidiTime )
	{
		while( i.hasNext() && i.peekNext()->time() < _segment->time() )
		{
			printf( "  Skipping Segment at: %d %d:\n",
				i.peekNext()->time().bar(),
				i.peekNext()->time().beat() );
			i.next();
		}
	}
	else if( _from == FromFrame )
	{
		while( i.hasNext() && _segment->frame() < i.peekNext()->frame() )
		{
			i.next();
		}
	}

	printf("  Inserting\n");
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
			int beats = 0;
			bar_t bar = prevTime.bar();
			beat_t beat = prevTime.beat();

			// Sum up beats for partial-first and any full bars
			while( bar < currentTime.bar() ) {
				while( beat < meter->beatsPerBar() ) {
					++beat;
					++beats;
				}
				++bar;
				beat = 0;
			}
			// Now sum up the possible incomplete bar at the end
			while( beat < currentTime.beat() ) {
				++beat;
				++beats;
			}

			//currentFrame += tempo->framesPerBeat( *meter, m_sampleRate ) * beats;
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

	// TODO: Don't be so lazy. Signal the actual range changed
	emit dataChanged( 0, length() );
}



f_cnt_t MetricMap::toNearestBar( f_cnt_t _frame )
{
	QReadLocker locker( &m_lock );

	// Binary search instead?
	//m_segments.
}
f_cnt_t MetricMap::toNearestBeat( f_cnt_t _frame )
{
	QReadLocker locker( &m_lock );
}
f_cnt_t MetricMap::toNearestTick( f_cnt_t _frame )
{
	QReadLocker locker( &m_lock );
}




MeatList MetricMap::meats( f_cnt_t _begin, f_cnt_t _end ) const
{
	QReadLocker locker( &m_lock );
	const Meter * meter = &m_firstMeterSegment->meter();
	const Tempo * tempo = &m_firstTempoSegment->tempo();

	f_cnt_t curFrame = 0;

	// Now the fun
	bar_t bar = 0;
	beat_t beat = 0;


	QVectorIterator<MetricSegment *> i( m_segments );
	MetricSegment * nextSegment = i.next();
	MeatList theMeats;

	f_cnt_t framesPerBeat = tempo->framesPerBeat( *meter, m_sampleRate );
	f_cnt_t thisFrames;
	beat_t beatsPerBar = meter->beatsPerBar();

	// Loop over bars
	// TODO: Properly handle _begin
	while( curFrame < _end )
	{
		// Loop over beats
		for( beat = 0; beat < beatsPerBar; ++beat)
		{
			//thisFrames = framesPerBeat;

			// Update meter or tempo for any segments this time
			while( nextSegment != NULL &&
				   bar == nextSegment->time().bar() &&
				   beat == nextSegment->time().beat() )
			{
				//Assign meter or tempo
				TempoSegment * tempoSegment;
				MeterSegment * meterSegment;
				if( tempoSegment = dynamic_cast<TempoSegment *>( nextSegment ) )
				{
					tempo = &tempoSegment->tempo();
					framesPerBeat = tempo->framesPerBeat( *meter, m_sampleRate );
				}
				else if( meterSegment = dynamic_cast<MeterSegment *>( nextSegment ) )
				{
					meter = &meterSegment->meter();
					beatsPerBar = meter->beatsPerBar();
					framesPerBeat = tempo->framesPerBeat( *meter, m_sampleRate );
				}

				// Get next segment
				if( i.hasNext() )
				{
					nextSegment = i.next();
				}
				else
				{
					nextSegment = NULL;
				}
			}

			// Now save the beat
			theMeats.append( MetricBeat( bar, beat, curFrame, *tempo, *meter ) );

			curFrame += framesPerBeat;
		}
		++bar;
	}

	return theMeats;
}


void MetricMap::saveSettings( QDomDocument & _doc, QDomElement & _this )
{
	QReadLocker locker( &m_lock );
}


void MetricMap::loadSettings( const QDomElement & _this )
{
	QWriteLocker locker( &m_lock );
}


#include "moc_MetricMap.cxx"
