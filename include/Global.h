/*
 * Global.h - global data and other stuff
 *
 * Copyright (c) 2009 Tobias Doerffel <tobydox/at/users/dot/sf/dot/net>
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
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 *
 */

#ifndef _LMMS_GLOBAL_H
#define _LMMS_GLOBAL_H

#include <QtCore/QMap>

#include "export.h"

class LmmsStyle;

// add new classes and the name of their "get"-functions here
#define AddGlobals(AddFunc)					\
	AddFunc(RuntimeConfig,runtimeConfig);	\
	AddFunc(PathConfig,paths);				\
	AddFunc(UserConfig,userConfig);

#define ForwardDecl(className,unused)		\
	class className;

AddGlobals(ForwardDecl);


class EXPORT Global
{
public:
	static void init( RuntimeConfig * _runtimeConfig );
	static void destroy();

	static const QMap<QString, QString> & pluginFileHandling()
	{
		return s_pluginFileHandling;
	}

	static void setLmmsStyle( LmmsStyle * _style )
	{
		s_lmmsStyle = _style;
	}

	static LmmsStyle * getLmmsStyle()
	{
		return s_lmmsStyle;
	}


#define GlobalMemberDecl(type,name)			\
			public:							\
				static inline type & name()	\
				{							\
					return *__##name;		\
				}							\
			private:						\
				static type * __##name;		\

	AddGlobals(GlobalMemberDecl);


private:
	static LmmsStyle * s_lmmsStyle;

	static QMap<QString, QString> s_pluginFileHandling;

	static void initPluginFileHandling();


} ;

#endif
