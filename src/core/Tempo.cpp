#include "Tempo.h"


double Tempo::framesPerBeat( const Meter & _meter, sample_rate_t _rate ) const
{
	// frames    minutes   noteType
    // ------- * ------- * ------------
	// minutes   beats     noteDivisor
	
	return  (60.0 * _rate) / (m_bpm * _meter.noteType() / m_note);
}



double Tempo::framesPerBar( const Meter & _meter, sample_rate_t _rate ) const
{
	return framesPerBeat( _meter, _rate ) * _meter.beatsPerBar();
}


/*
double Meter::framesPerBar( const Tempo & _tempo, sample_rate_t _rate ) const
{
	return _tempo.framesPerBar( this, _rate );
}


double Meter::framesPerBeat( const Tempo & _tempo, sample_rate_t _rate ) const
{
	return _tempo.framesPerBeat( this, _rate );
}
*/
