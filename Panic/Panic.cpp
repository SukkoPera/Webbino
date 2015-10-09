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

#include "Panic.h"

#ifndef PANIC_DISABLE_SERIAL
	#define panic_print(x) Serial.print (F("PANIC - ASSERTION_FAILED at "));
#else
	#define panic_print(x)
#endif


Panic::Panic (byte _led, int _interval): led (_led), interval (_interval) {
	// Initialize the panic pin as an output.
	pinMode (led, OUTPUT);
}

void Panic::infiniteBlink () {
	while (42) {
		digitalWrite (led, HIGH);
		delay (interval);
		digitalWrite (led, LOW);
		delay (interval);
	}
}

void Panic::panic (const __FlashStringHelper *reason) {
	panic_print (F("PANIC: "));
	if (reason) {
		panic_print (reason);
	} else {
		panic_print (F("Unknown reason"));
	}

	infiniteBlink ();
}
	
#ifndef NDEBUG
void Panic::assert_fail (const char *message, const char *file, const int line, const char *function) {
	panic_print (F("PANIC - ASSERTION_FAILED at "));
	panic_print (file);
	panic_print (F(":"));
	panic_print (line);
	panic_print (F(": "));
	panic_print (message);

	infiniteBlink ();
}
#endif
