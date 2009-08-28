#ifndef _MIDI_TIME_TNG_H
#define _MIDI_TIME_TNG_H

#include "lmms_basics.h"
#include "export.h"

const int TicksPerBeat = 192;

class EXPORT MidiTime
{
public:
	inline MidiTime() :
		m_bar( 0 ),
		m_beat( 0 ),
		m_ticks( 0 )
	{
	}
	inline MidiTime( const bar_t _bar, const beat_t _beat, const tick_t _ticks = 0 ) :
		m_bar( _bar ),
		m_beat( _beat),
		m_ticks( _ticks )
	{
	}

	inline MidiTime( const MidiTime & _t ) :
		m_bar( _t.m_bar ),
		m_beat( _t.m_beat ),
		m_ticks( _t.m_ticks )
	{
	}

	inline MidiTime & operator=( const MidiTime & _t )
	{
		m_bar = _t.m_bar;
		m_beat = _t.m_beat;
		m_ticks = _t.m_ticks;
		return *this;
	}

	inline bar_t bar( void ) const
	{
		return m_bar;
	}

	inline beat_t beat( void ) const
	{
		return m_beat;
	}

	inline void setTicks( tick_t _t )
	{
		m_ticks = _t;
	}

	inline tick_t ticks( void ) const
	{
		return m_ticks;
	}

	bool operator< ( const MidiTime & _other ) const
	{
		return ( bar() < _other.bar() ) || ( bar() == _other.bar() &&
			   ( beat() < _other.beat() || ( beat() == _other.beat() &&
			   ( ticks() < _other.ticks() ) ) ) );
		/*
		return ( bar() < _other.bar() ) ||
			   ( bar() == _other.bar() && beat() < _other.beat() );
		*/
	}

	bool operator== ( const MidiTime & _other ) const
	{
		return ( bar() == _other.bar() ) && ( beat() == _other.beat() );
	}


private:
	bar_t  m_bar;
	beat_t m_beat;
	tick_t m_ticks;
} ;

#endif

