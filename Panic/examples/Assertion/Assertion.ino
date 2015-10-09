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
	/* This must be called by the sketch, it is not done by Panic.
	 *
	 * Note that you can disable all messages on the serial port used by Panic
	 * definind the PANIC_DISABLE_SERIAL macro in Panic.h (and only there - or
	 * on the compiler command line, if you find a way - doing it here will
	 * not work).
	 */
	Serial.begin (9600);
}

void loop () {
	int i = 42;
	
	// This assertion will be successful, and nothing will happen
	panic_assert (panic, i == 42);
	
	// This one will fail, halt the program and let the led on pin 13 blink
	panic_assert (panic, true == false);
}
