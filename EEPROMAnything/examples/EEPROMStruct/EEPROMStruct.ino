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

#include <EEPROMAnything.h>

#define EEPROM_ADDR 100				// An usually-unused address, to minimize EEPROM wear

#define MAXSTR 16

struct MyStruct {
	int i;
	float f;
	char s[MAXSTR];
};


void setup () {
	Serial.begin (9600);
}

void loop () {
	MyStruct s1, s2;
	
	// Fill s1 with some values and write it to EEPROM
	s1.i = 42;
	s1.f = 3.14;
	strlcpy_P (s1.s, PSTR ("Don't panic!"), MAXSTR);
	EEPROMAnything.writeAnything (EEPROM_ADDR, s1);
	
	// Re-read and print values
	EEPROMAnything.readAnything (EEPROM_ADDR, s2);
	Serial.print ("i = ");
	Serial.println (s2.i);
	Serial.print ("f = ");
	Serial.println (s2.f);
	Serial.print ("s = \"");
	Serial.print (s2.s);
	Serial.println ("\"");
	
	while (42)
		;
}
