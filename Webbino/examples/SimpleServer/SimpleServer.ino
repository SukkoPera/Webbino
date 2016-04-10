/***************************************************************************
 *   This file is part of SmartStrip.                                      *
 *                                                                         *
 *   Copyright (C) 2012-2015 by SukkoPera                                  *
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
#include <avr/pgmspace.h>

// Instantiate the WebServer
WebServer webserver;

#define USE_ENC28J60
#include <WebServer_ENC28J60.h>
NetworkInterfaceENC28J60 netint;

//~ #define USE_ESP8266
//~ #include <WebServer_ESP8266.h>

//~ #include <SoftwareSerial.h>
//~ SoftwareSerial swSerial (7, 8);

//~ // Wi-Fi parameters
//~ #define WIFI_SSID        "ssid"
//~ #define WIFI_PASSWORD    "password"

//~ NetworkInterfaceESP8266 netint;


/******************************************************************************
 * DEFINITION OF PAGES                                                        *
 ******************************************************************************/

#include "html.h"

static const Page indexPage PROGMEM = {index_html_name, index_html, NULL};

static const Page * const pages[] PROGMEM = {
	&indexPage,
 	NULL
};


/******************************************************************************
 * MAIN STUFF                                                                 *
 ******************************************************************************/

void setup () {
	Serial.begin (9600);
	// Serial.println (F("Webbino " PROGRAM_VERSION));

#if defined (USE_ENC28J60)
	byte mac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
	netint.begin (mac);
#elif defined (USE_ESP8266)
	swSerial.begin (9600);
	netint.begin (swSerial, WIFI_SSID, WIFI_PASSWORD);
#endif

	webserver.setPages (pages);

	Serial.println (F("Trying to get an IP address through DHCP"));
	if (!webserver.begin (netint)) {
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
	webserver.loop ();
}
