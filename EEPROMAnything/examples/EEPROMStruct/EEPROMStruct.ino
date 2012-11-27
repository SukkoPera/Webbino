/***************************************************************************
 *   This file is part of Sukkino.                                      *
 *                                                                         *
 *   Copyright (C) 2012 by SukkoPera                                       *
 *                                                                         *
 *   Sukkino is free software: you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   Sukkino is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with Sukkino.  If not, see <http://www.gnu.org/licenses/>.   *
 ***************************************************************************/

#include <EEPROMAnything.h>

#define EEPROM_ADDR 100				// An usually-unused address, to minimize EEPROM wear

#define MAXSTR 16

// #define USE_TEMPLATE

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
#ifndef USE_TEMPLATE
	EEPROMAnything.writeAnything (EEPROM_ADDR, &s1, sizeof (MyStruct));
#else
	EEPROMAnything.writeAnything (EEPROM_ADDR, s1);
#endif
	
	// Re-read and print values
#ifndef USE_TEMPLATE
	EEPROMAnything.readAnything (EEPROM_ADDR, &s2, sizeof (MyStruct));
#else
	EEPROMAnything.readAnything (EEPROM_ADDR, s2);
#endif
	Serial.print ("i = ");
	Serial.println (s2.i);
	Serial.print ("f = ");
	Serial.println (s2.f);
	Serial.print ("s = \"");
	Serial.print (s2.s);
	Serial.println ("\"");
	
	int i;
#ifdef USE_TEMPLATE
	EEPROMAnything.writeAnything (EEPROM_ADDR, i);
	EEPROMAnything.readAnything (EEPROM_ADDR, i);
#else
	EEPROMAnything.writeAnything (EEPROM_ADDR, &i, sizeof (int));
	EEPROMAnything.readAnything (EEPROM_ADDR, &i, sizeof (int));
#endif
	Serial.print ("i = ");
	Serial.println (i);
	
	float f;
#ifdef USE_TEMPLATE
	EEPROMAnything.writeAnything (EEPROM_ADDR, f);
	EEPROMAnything.readAnything (EEPROM_ADDR, f);
#else
	EEPROMAnything.writeAnything (EEPROM_ADDR, &f, sizeof (float));
	EEPROMAnything.readAnything (EEPROM_ADDR, &f, sizeof (float));
#endif
	Serial.print ("f = ");
	Serial.println (f);
	
	while (42)
		;
}
