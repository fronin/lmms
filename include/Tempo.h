#ifndef _TEMPO_TNG_H
#define _TEMPO_TNG_H

#include "lmms_basics.h"
#include "MidiTime.h"

class Meter;

class Tempo
{
public:
	// FIXME: Should divisor be float or double?
	Tempo( bpm_t _bpm, note_type_t _noteType = 4.0 ) : // defaulting to quarter note
		m_bpm( _bpm ),
		m_note( _noteType )
	{
	}

    Tempo( const Tempo & _t ) :
		m_bpm( _t.m_bpm ),
		m_note( _t.m_note )
	{
	}
     
	Tempo & operator=( const Tempo & _t )
	{
		m_bpm = _t.m_bpm;
		m_note = _t.m_note;
		return *this;
	}

	bpm_t bpm() const
	{
		return m_bpm;
	}

	note_type_t noteType() const
	{
		return m_note;
	}

	double framesPerBeat (const Meter & meter, sample_rate_t sr) const;
	double framesPerBar (const Meter & meter, sample_rate_t sr) const;

protected:
    bpm_t m_bpm;
	float m_note;
};



class Meter
{
public:
	Meter( float _beats, float _noteType ) : 
		m_beatsPerBar( _beats ), 
		m_note( _noteType )
	{
	}

	Meter( const Meter & _m ) :
		m_beatsPerBar( _m.m_beatsPerBar ), 
		m_note( _m.m_note )
	{
	}

	Meter & operator=( const Meter & _m )
	{
		m_beatsPerBar = _m.m_beatsPerBar;
		m_note = _m.m_note;
		return *this;
	}

	float beatsPerBar() const
	{
		return m_beatsPerBar;
	}

	note_type_t noteType() const
	{
		return m_note;
	}

	/*
	double framesPerBeat( const Tempo &, sample_rate_t sr ) const;
	double framesPerBar( const Tempo &, sample_rate_t sr ) const;
	*/

protected:
	float m_beatsPerBar;
	note_type_t m_note;
};


#endif

