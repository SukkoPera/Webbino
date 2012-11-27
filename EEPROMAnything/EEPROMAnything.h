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

/* How to decide whether to use the C-style or the C++-style interface:
 * If you don't mind about code size, choose whatever you prefer, otherwise:
 * The C++-style interface results in slightly smaller code if it is used WITH
 * A SINGLE TYPE throughout the whole code (since it only has two parameters).
 * Because of how templates are implemented in C++, a new function is generated
 * for every different type the method is called with. This means that if you
 * are calling it with two different types, the C++-style interface will yield
 * a slightly bigger binary. While negligible in geneal, The difference in size
 * will increase with the number of different types you use, so be aware.
 *
 * You can experience an example of the size difference with the USE_TEMPLATE
 * macro of the EEPROMStruct example.
 */
class EEPROMAnythingClass {
public:
    uint8_t read (int);
    void write (int, uint8_t);
	
	/* C-style interface */
	int writeAnything (int ee, const void *value, const unsigned int size);
	int readAnything (int ee, const void *value, const unsigned int size);

	/* C++-style interface */
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
