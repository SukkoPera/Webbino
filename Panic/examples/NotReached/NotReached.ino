/***************************************************************************
 *   This file is part of Sukkino.                                         *
 *                                                                         *
 *   Copyright (C) 2012-2015 by SukkoPera                                  *
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

#include <Panic.h>

Panic panic;


void setup () {
	Serial.begin (9600);
}

void loop () {
	int i = 42;
	
	switch (i) {
		case 1:
		case 2:
		case 3:
			Serial.print ("OK");
			break;
		default:
			panic_assert_not_reached (panic);
			break;
	}
}
