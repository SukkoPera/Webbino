#include <avr/eeprom.h>
#include <Arduino.h>
#include "EEPROMAnything.h"

uint8_t EEPROMAnythingClass::read (int address) {
        return eeprom_read_byte ((unsigned char *) address + EEPROM_GENERAL_OFFSET);
}

void EEPROMAnythingClass::write (int address, uint8_t value) {
        eeprom_write_byte ((unsigned char *) address + EEPROM_GENERAL_OFFSET, value);
}

int EEPROMAnythingClass::writeAnything (int ee, const void *value, const unsigned int size) {
	const byte *p = static_cast<const byte *> (value);
	unsigned int i;
	
	for (i = 0; i < size; i++)
		write (ee++, *p++);
	
	return i;
}

int EEPROMAnythingClass::readAnything (int ee, const void *value, const unsigned int size) {
	byte *p = static_cast<byte *> (const_cast<void *> (value));
	unsigned int i;
	
	for (i = 0; i < size; i++)
		*p++ = read (ee++);
	
	return i;
}

EEPROMAnythingClass EEPROMAnything;
