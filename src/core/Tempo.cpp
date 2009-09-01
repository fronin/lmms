#include "Tempo.h"


double Tempo::framesPerBeat( const Meter & _meter, sample_rate_t _rate ) const
{
	// frames    seconds   minutes   tempo.noteType
	// ------- * --------* ------- * --------------
	// second    minute    beats     meter.noteType
	
	return ( _rate * 60.0 * noteType() ) / ( m_bpm * _meter.noteType() );
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
