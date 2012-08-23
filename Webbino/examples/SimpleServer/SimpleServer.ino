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

#include <Webbino.h>

/* See http://provideyourown.com/2011/advanced-arduino-including-multiple-libraries/
 * to understand why this is unfortunately necessary.
 */
#ifdef USE_ENC28J60
	#include <EtherCard.h>
#else
	#include <SPI.h>
	#include <Ethernet.h>
#endif

#include <avr/pgmspace.h>

// Instantiate the WebServer
WebServer webserver;


/******************************************************************************
 * DEFINITION OF PAGES                                                        *
 ******************************************************************************/

#include "html.h"

static Page indexPage PROGMEM = {index_html_name, index_html, NULL};

static Page *pages[] PROGMEM = {
	&indexPage,
 	NULL
};


/******************************************************************************
 * MAIN STUFF                                                                 *
 ******************************************************************************/

void setup () {
	byte mac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};

	Serial.begin (9600);
	// Serial.println (F("Webbino " PROGRAM_VERSION));

	webserver.setPages (pages);

	Serial.println (F("Trying to get an IP address through DHCP"));
	if (!webserver.begin (mac)) {
		Serial.println (F("Failed to get configuration from DHCP"));
		while (42)
			;
	} else {
		Serial.println (F("DHCP configuration done"));
#if 0
		ether.printIp ("IP:\t", webserver.getIP ());
		ether.printIp ("Mask:\t", webserver.getNetmask ());
		ether.printIp ("GW:\t", webserver.getGateway ());
		Serial.println ();
#endif
	}
}

void loop () {
	webserver.processPacket ();
}
