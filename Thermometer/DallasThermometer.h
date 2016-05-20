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

#ifndef _DALLASTHERMO_H_
#define _DALLASTHERMO_H_

#include "thermometer_common.h"

#ifdef USE_DALLAS_THERMO

#include <OneWire.h>
#include <DallasTemperature.h>
#include "ThermometerBase.h"
#include "thermometer_debug.h"

// Chosen data resolution
#define THERMOMETER_RESOLUTION 9


class DallasThermometer: public ThermometerBase {
private:
	//byte busPin;

	// OneWire instance to communicate with OneWire devices
	OneWire oneWire;
	DallasTemperature sensors;
	DeviceAddress thermometerAddress;

	// function to print a device address
	static void printAddress (DeviceAddress deviceAddress);

	bool refreshTemperature ();

public:
	DallasThermometer ();



	void begin (byte busPin);
};

class Thermometer: public DallasThermometer {
};

#endif

#endif
