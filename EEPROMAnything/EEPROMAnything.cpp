#include <avr/eeprom.h>
#include <Arduino.h>
#include "EEPROMAnything.h"

uint8_t EEPROMAnythingClass::read (int address) {
        return eeprom_read_byte ((unsigned char *) address + EEPROM_GENERAL_OFFSET);
}

void EEPROMAnythingClass::write (int address, uint8_t value) {
        eeprom_write_byte ((unsigned char *) address + EEPROM_GENERAL_OFFSET, value);
}

EEPROMAnythingClass EEPROMAnything;
