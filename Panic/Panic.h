/***************************************************************************
 *   This file is part of SmartStrip.                                      *
 *                                                                         *
 *   Copyright (C) 2012 by SukkoPera                                       *
 *                                                                         *
 *   SmartStrip is free software: you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   SmartStrip is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with SmartStrip.  If not, see <http://www.gnu.org/licenses/>.   *
 ***************************************************************************/

#ifndef _PANIC_H_
#define _PANIC_H_

#include <Arduino.h>

/* This can be defined to disable the messages output on the serial port. If
 * you don't do this it is *YOUR* responsibility to call Serial.begin().
 */
// #define PANIC_DISABLE_SERIAL


// This stuff was inspired from GCC
#if defined __cplusplus// && __GNUC_PREREQ (2,95)
# define __ASSERT_VOID_CAST static_cast<void>
#else
# define __ASSERT_VOID_CAST (void)
#endif

#ifdef NDEBUG

	#define assert(expr)           (__ASSERT_VOID_CAST (0))

#else /* Not NDEBUG.  */

	#define __STRING(x)     #x

	#define panic_assert(p, expr)                                           \
	  ((expr)                                                               \
	   ? __ASSERT_VOID_CAST (0)                                             \
	   : p.assert_fail (__STRING(expr), __FILE__, __LINE__, __PRETTY_FUNCTION__))

	#define panic_assert_not_reached(p)                                    \
	  (p.assert_fail ("Shall not be reached", __FILE__, __LINE__, __PRETTY_FUNCTION__))

#endif


// Here we use #define's to spare some bytes

// Pin to blink in case of panic. Now, pin 13 has an LED connected on most Arduino boards, so the default should be reasonable.
#define DEFAULT_PANIC_LED 13

// Interval to blink the led at, in case of panic (milliseconds)
#define DEFAULT_PANIC_BLINK_INTERVAL 1000

class Panic {
private:
	byte led;
	int interval;

	void infiniteBlink ();
	
public:
	Panic (byte _led = DEFAULT_PANIC_LED, int _interval = DEFAULT_PANIC_BLINK_INTERVAL);

	/* We only support panic reasons stored in PROGMEM, so use the F() macro.
	 * C'mon, don't waste precious SRAM!
	 */
	void panic (const __FlashStringHelper *reason);
	
#ifndef NDEBUG
	void assert_fail (const char *message, const char *file, const int line, const char *function);
#endif
};

#endif
