/* Structured EEPROM access routines
 * Found at http://www.arduino.cc/playground/Code/EEPROMWriteAnything
 *
 * Unknown license, assuming PUBLIC DOMAIN.
 */

#ifndef EEPROM_ANYTHING_H_
#define EEPROM_ANYTHING_H_

#include <Arduino.h>
#include <inttypes.h>

/* This can be used to redirect all EEPROM read/writes to a different EEPROM
 * area. It is mainly useful during development, in order to avoid wearing a
 * single EEPROM area too much.
 */
#define EEPROM_GENERAL_OFFSET 0


class EEPROMAnythingClass {
public:
    uint8_t read (int);
    void write (int, uint8_t);

	template <class T> int writeAnything (int ee, const T& value) {
		const byte* p = (const byte *) (const void *) &value;
		unsigned int i;
		
		for (i = 0; i < sizeof (value); i++)
			write (ee++, *p++);
		
		return i;
	}

	template <class T> int readAnything (int ee, T& value) {
		byte* p = (byte *) (void *) &value;
		unsigned int i;
		
		for (i = 0; i < sizeof (value); i++)
			*p++ = read (ee++);
		
		return i;
	}
};

extern EEPROMAnythingClass EEPROMAnything;

#endif
