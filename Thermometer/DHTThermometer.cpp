/***************************************************************************
 *   This file is part of Sukkino.                                         *
 *                                                                         *
 *   Copyright (C) 2013-2016 by SukkoPera                                  *
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

#include "DHTThermometer.h"

#ifdef USE_DHT_THERMO

DHTThermometer::DHTThermometer (): sensor (0, 0) {		// Temporary values
}

void DHTThermometer::begin (byte busPin, byte type) {
	// locate devices on the bus
	DPRINT (F("Scanning for temperature sensors on pin "));
	DPRINT (busPin);
	DPRINTLN ();
	sensor = DHT (busPin, type);						// Reinit with correct values. This is safe as of 22/01/2013
	sensor.begin ();
	available = true;			 // Can we really detect if the sensor is available?
}

bool DHTThermometer::refreshTemperature () {
	if (available) {
		/* Call sensors.requestTemperatures() to issue a global temperature
		* request to all devices on the bus.
		*/
		DPRINT (F("Requesting temperature..."));
		currentTemp.celsius = sensor.readTemperature ();
		currentTemp.valid = !isnan (currentTemp.celsius);
		DPRINTLN (F(" Done"));
	}
	
	return currentTemp.valid;
}

#endif
