/*
 * Pattern.h - declaration of class pattern, which contains all informations
 *             about a pattern
 *
 * Copyright (c) 2004-2009 Tobias Doerffel <tobydox/at/users.sourceforge.net>
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

#ifndef _PATTERN_TNG_H
#define _PATTERN_TNG_H

#include <QtCore/QVector>
#include <QtGui/QWidget>
#include <QtGui/QDialog>
#include <QtCore/QThread>
#include <QtGui/QPixmap>


#include "note.h"
#include "Track.h"


class QAction;
class QProgressBar;
class QPushButton;

class InstrumentTrack;
class PatternFreezeThread;
class sampleBuffer;



class EXPORT Pattern : public TrackSegment
{
	Q_OBJECT
public:
	enum PatternTypes
	{
		BeatPattern,
		MelodyPattern
	} ;

	Pattern( InstrumentTrack * _instrumentTrack );
	Pattern( const Pattern & _patToCopy );
	virtual ~Pattern();

	void init();


	virtual midiTime length() const;
	midiTime beatPatternLength() const;

	// note management
	note * addNote( const note & _new_note, const bool _quant_pos = true );

	void removeNote( const note * _note_to_del );

	note * rearrangeNote( const note * _note_to_proc,
						const bool _quant_pos = true );
	void rearrangeAllNotes();
	void clearNotes();

	inline const NoteVector & notes() const
	{
		return m_notes;
	}

	void setStep( int _step, bool _enabled );

	// pattern-type stuff
	inline PatternTypes type() const
	{
		return m_patternType;
	}
	void setType( PatternTypes _new_pattern_type );
	void checkType();


	// functions which are part of freezing-feature
	inline bool freezing() const
	{
		return m_freezing;
	}

	inline bool frozen() const
	{
		return m_frozenPattern != NULL;
	}

	sampleBuffer * frozenPattern()
	{
		return m_frozenPattern;
	}

	// settings-management
	virtual void saveSettings( QDomDocument & _doc, QDomElement & _parent );
	virtual void loadSettings( const QDomElement & _this );
	inline virtual QString nodeName() const
	{
		return "pattern";
	}

	inline InstrumentTrack * instrumentTrack() const
	{
		return m_instrumentTrack;
	}

	bool empty();


	void addSteps( int _n );
	void removeSteps( int _n );

	using Model::dataChanged;


protected:
	void ensureBeatNotes();
	void updateBBTrack();


protected slots:
	void clear();
	void freeze();
	void unfreeze();
	void abortFreeze();
	void changeTimeSignature();


private:
	InstrumentTrack * m_instrumentTrack;

	PatternTypes m_patternType;

	// data-stuff
	NoteVector m_notes;
	int m_steps;

	// pattern freezing
	sampleBuffer * m_frozenPattern;
	bool m_freezing;
	volatile bool m_freezeAborted;

	friend class PatternFreezeThread;

} ;




class PatternFreezeStatusDialog : public QDialog
{
	Q_OBJECT
public:
	PatternFreezeStatusDialog( QThread * _thread );
	virtual ~PatternFreezeStatusDialog();

	void setProgress( int _p );


protected:
	void closeEvent( QCloseEvent * _ce );


protected slots:
	void cancelBtnClicked();
	void updateProgress();


private:
	QProgressBar * m_progressBar;
	QPushButton * m_cancelBtn;

	QThread * m_freezeThread;

	int m_progress;


signals:
	void aborted();

} ;




class PatternFreezeThread : public QThread
{
public:
	PatternFreezeThread( Pattern * _pattern );
	virtual ~PatternFreezeThread();


protected:
	virtual void run();


private:
	Pattern * m_pattern;
	PatternFreezeStatusDialog * m_statusDlg;

} ;


#endif
