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

#include <Thermometer.h>

#ifdef USE_DALLAS_THERMO
	#include <OneWire.h>
	#include <DallasTemperature.h>
#elif defined USE_DHT_THERMO
	#include <DHT.h>
#endif


// Thermometer's data wire is connected to pin 5
#define THERMOMETER_PIN 5

// Delay between temperature readings
#define THERMO_READ_INTERVAL (1 * 1000U)

Thermometer thermometer;

unsigned long lastTemperatureRequest = 0;



void setup () {
	Serial.begin (9600);
	thermometer.begin (THERMOMETER_PIN);
}

void loop () {
	if (thermometer.available && millis () - lastTemperatureRequest > THERMO_READ_INTERVAL) {
		// Print out the data
		//printTemperature (currentTemp.celsius);
		Temperature& temp = thermometer.getTemp ();
		Serial.print ("Current temperature: ");
		Serial.print (temp.celsius);
		Serial.print (" C (");
		Serial.print (temp.toFahrenheit ());
		Serial.println (" F)");

		lastTemperatureRequest = millis ();
	}
}
