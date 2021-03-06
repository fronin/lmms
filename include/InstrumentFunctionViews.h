/*
 * InstrumentFunctionViews.h - views for instrument-functions-tab
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

#ifndef _INSTRUMENT_FUNCTION_VIEWS_H
#define _INSTRUMENT_FUNCTION_VIEWS_H

#include "ModelView.h"

#include <QtGui/QWidget>

class QLabel;
class comboBox;
class groupBox;
class knob;
class TempoSyncKnob;

class Arpeggiator;
class ChordCreator;



class ChordCreatorView : public QWidget, public ModelView
{
	Q_OBJECT
public:
	ChordCreatorView( ChordCreator * _cc, QWidget * _parent );
	virtual ~ChordCreatorView();


private:
	virtual void modelChanged();

	ChordCreator * m_cc;

	groupBox * m_chordsGroupBox;
	comboBox * m_chordsComboBox;
	knob * m_chordRangeKnob;

} ;





class ArpeggiatorView : public QWidget, public ModelView
{
	Q_OBJECT
public:
	ArpeggiatorView( Arpeggiator * _arp, QWidget * _parent );
	virtual ~ArpeggiatorView();


private:
	virtual void modelChanged();

	Arpeggiator * m_a;
	groupBox * m_arpGroupBox;
	comboBox * m_arpComboBox;
	knob * m_arpRangeKnob;
	TempoSyncKnob * m_arpTimeKnob;
	knob * m_arpGateKnob;

	QLabel * m_arpDirectionLbl;

	comboBox * m_arpDirectionComboBox;
	comboBox * m_arpModeComboBox;

} ;


#endif
