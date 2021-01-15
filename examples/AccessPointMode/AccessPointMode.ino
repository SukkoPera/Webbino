/***************************************************************************
 *   This file is part of Webbino.                                         *
 *                                                                         *
 *   Copyright (C) 2012-2021 by SukkoPera                                  *
 *                                                                         *
 *   Webbino is free software: you can redistribute it and/or modify       *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation, either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   Webbino is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with Webbino. If not, see <http://www.gnu.org/licenses/>.       *
 ***************************************************************************/

#include <Webbino.h>

// Instantiate the WebServer and page storage
WebServer webserver;
FlashStorage flashStorage;

// Instantiate the network interface defined in the Webbino headers
#if !defined (WEBBINO_USE_ESP8266_STANDALONE) && !defined (ARDUINO_ARCH_ESP32)
#error "Access Point Mode is only supported on ESP8266 and ESP32"
#endif

#include <WebbinoInterfaces/AllWiFi.h>

/* Wi-Fi parameters: To have an open AP set the password to "" or NULL. If you
 * want to use a password, it must be between 8 and 63 characters long.
 */
#define WIFI_SSID        "Webbino"
#define WIFI_PASSWORD    ""

NetworkInterfaceWiFi netint;


/******************************************************************************
 * DEFINITION OF PAGES                                                        *
 ******************************************************************************/

#include "html.h"

const Page page01 PROGMEM = {index_html_name, index_html, index_html_len};
const Page page02 PROGMEM = {logo_gif_name, logo_gif, logo_gif_len};

const Page* const pages[] PROGMEM = {
	&page01,
	&page02,
	NULL
};


/******************************************************************************
 * MAIN STUFF                                                                 *
 ******************************************************************************/

void setup () {
	Serial.begin (115200);
	while (!Serial)
		;

	Serial.println (F("Webbino " WEBBINO_VERSION));

	IPAddress ip;
	Serial.println (F("Trying to enable Access Point Mode..."));
	if (!netint.beginAP (WIFI_SSID, WIFI_PASSWORD, ip)) {
		Serial.println (F("Failed :("));
		while (42)
			yield ();
	} else {
		Serial.print (F("Ready, server is at "));
		Serial.println (ip);

		webserver.begin (netint);

		flashStorage.begin (pages);
		webserver.addStorage (flashStorage);
	}
}

void loop () {
	webserver.loop ();
}
