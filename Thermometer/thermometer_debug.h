/***************************************************************************
 *   This file is part of Sukkino.                                         *
 *                                                                         *
 *   Copyright (C) 2013-2016 by SukkoPera                                  *
 *                                                                         *
 *   Sukkino is free software: you can redistribute it and/or modify       *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   Sukkino is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with Sukkino.  If not, see <http://www.gnu.org/licenses/>.      *
 ***************************************************************************/

#ifndef _THERMOMETERDEBUG_H_
#define _THERMOMETERDEBUG_H_

#include "thermometer_common.h"

#ifndef THERMOMETER_NDEBUG
	#ifndef DPRINT
		#define DPRINT(...) Serial.print(__VA_ARGS__)
	#endif

	#ifndef DPRINTLN
		#define DPRINTLN(...) Serial.println(__VA_ARGS__)
	#endif
#else
	#ifndef DPRINT
		#define DPRINT(...) do {} while (0)
	#endif

	#ifndef DPRINTLN
		#define DPRINTLN(...) do {} while (0)
	#endif
#endif

#endif
