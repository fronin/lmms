/*
 * AudioBackend.h - base-class for audio-devices, used by LMMS-mixer
 *
 * Copyright (c) 2004-2010 Tobias Doerffel <tobydox/at/users.sourceforge.net>
 *
 * This file is part of Linux MultiMedia Studio - http://lmms.sourceforge.net
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#ifndef _AUDIO_DEVICE_H
#define _AUDIO_DEVICE_H

#include <QtCore/QPair>
#include <QtCore/QThread>

#include "Mixer.h"
#include "tab_widget.h"


class AudioPort;

/*! \brief The AudioBackend class is the base class for all kinds of AudioBackends.
 *
 * All classes derived from AudioBackend receive audio data so they can output
 * it.
 */
class AudioBackend
{
public:
	/*! Lists all known methods for output of audio data. */
	enum OutputMethods
	{
		ActiveOutput,		/*!< Data is pushed via the API of the backend */
		CallbackOutput,		/*!< Data is fetched via a callback by the
								according backend */
		NumOutputMethods
	} ;
	typedef OutputMethods OutputMethod;

	/*! \brief Constructs an AudioBackend object for the given AudioOutputContext. */
	AudioBackend( const ch_cnt_t _channels, AudioOutputContext * context );
	virtual ~AudioBackend();

	/*! If the audio backend supports ports, classes creating an AudioPort
	 * (e.g. InstrumentTrack) can register themselves for making
	 * audio backend able to collect their individual output and provide
	 * them at a specific port - currently only supported by JACK
	 */
	virtual void registerPort( AudioPort * _port );
	virtual void unregisterPort( AudioPort * _port );
	virtual void renamePort( AudioPort * _port );

	/*! \brief Returns output method of backend.
	 *
	 * This is merely important for AudioOutputContext::BufferFifo which
	 * optimizes its behaviour accordingly. When operating in #CallbackOutput
	 * mode, reading from the FIFO should be realtime safe, i.e. use spinlocks
	 * rather than semaphores.
	 */
	OutputMethod outputMethod() const
	{
		return m_outputMethod;
	}

	inline bool supportsCapture() const
	{
		return m_supportsCapture;
	}

	inline sample_rate_t sampleRate() const
	{
		return m_sampleRate;
	}

	ch_cnt_t channels() const
	{
		return m_channels;
	}

	/*! \brief Fetches one buffer and writes it to output device.
	 *
	 * \return Number of frames processed
	 */
	int processNextBuffer();

	virtual void startProcessing()
	{
	}

	virtual void stopProcessing();

	virtual void applyQualitySettings();



	class setupWidget : public tabWidget
	{
	public:
		setupWidget( const QString & _caption, QWidget * _parent ) :
			tabWidget( tabWidget::tr( "Settings for %1" ).arg(
					tabWidget::tr( _caption.toAscii() ) ).
							toUpper(), _parent )
		{
		}

		virtual ~setupWidget()
		{
		}

		virtual void saveSettings() = 0;

		virtual void show()
		{
			parentWidget()->show();
			QWidget::show();
		}

	} ;


	/*! \brief Returns const pointer to AudioOutputContext this AudioBackend acts for. */
	const AudioOutputContext * outputContext() const
	{
		return m_context;
	}

	/*! \brief Returns const pointer to Mixer this AudioBackend acts for. */
	const Mixer * mixer() const;


protected:
	/*! \brief Writes given buffer to actual device.
	 *
	 * Subclasses can reimplement this for being used in conjunction with
	 * processNextBuffer()
	 */
	virtual void writeBuffer( const sampleFrameA * /* _buf*/,
						const fpp_t /*_frames*/,
						const float /*_master_gain*/ )
	{
	}

	/*! \brief Called by according backend for fetching new audio data. */
	int getNextBuffer( sampleFrameA * _ab );

	/*! \brief Clears given signed-int-16-buffer. */
	void clearS16Buffer( intSampleFrameA * _outbuf, const fpp_t _frames );

	inline void setSampleRate( const sample_rate_t _new_sr )
	{
		m_sampleRate = _new_sr;
	}

	bool hqAudio() const;

	AudioOutputContext * outputContext()
	{
		return m_context;
	}

	/*! \brief Allows the backend to set its preferred output method.
	 *
	 * \sa outputMethod()
	 */
	void setOutputMethod( OutputMethod method )
	{
		m_outputMethod = method;
	}

	Mixer * mixer();


protected:
	bool m_supportsCapture;


private:
	AudioOutputContext * m_context;
	sample_rate_t m_sampleRate;
	ch_cnt_t m_channels;

	OutputMethod m_outputMethod;

	sampleFrameA * m_buffer;

} ;


#endif
