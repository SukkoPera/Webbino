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
#if defined (WEBBINO_USE_ENC28J60)
	#include <WebbinoInterfaces/ENC28J60.h>
	NetworkInterfaceENC28J60 netint;

	#define MAC_ADDRESS 0x00,0x11,0x22,0x33,0x44,0x55

	// Ethernet Slave Select pin
	const byte ETH_SS_PIN = SS;
#elif defined (WEBBINO_USE_WIZ5100) || defined (WEBBINO_USE_WIZ5500) || \
	  defined (WEBBINO_USE_ENC28J60_UIP) || defined (WEBBINO_USE_TEENSY41_NATIVE)
	#include <WebbinoInterfaces/WIZ5x00.h>
	NetworkInterfaceWIZ5x00 netint;

	#define MAC_ADDRESS 0x00,0x11,0x22,0x33,0x44,0x55

	// This is ignored for Teensy 4.1
	const byte ETH_SS_PIN = SS;
#elif defined (WEBBINO_USE_ESP8266)
	#include <WebbinoInterfaces/AllWiFi.h>

	#include <SoftwareSerial.h>
	SoftwareSerial espSerial (6, 7);

	// Wi-Fi parameters
	#define WIFI_SSID        "ssid"
	#define WIFI_PASSWORD    "password"

	NetworkInterfaceWiFi netint;
#elif defined (WEBBINO_USE_WIFI) || defined (WEBBINO_USE_WIFI101) || \
	  defined (WEBBINO_USE_ESP8266_STANDALONE)
	#include <WebbinoInterfaces/AllWiFi.h>

	// Wi-Fi parameters
	#define WIFI_SSID        "ssid"
	#define WIFI_PASSWORD    "password"

	NetworkInterfaceWiFi netint;
#elif defined (WEBBINO_USE_FISHINO)
	#include <WebbinoInterfaces/FishinoIntf.h>

	// Wi-Fi parameters
	#define WIFI_SSID        "ssid"
	#define WIFI_PASSWORD    "password"

	FishinoInterface netint;
#elif defined (WEBBINO_USE_DIGIFI)
	#include <WebbinoInterfaces/DigiFi.h>
	NetworkInterfaceDigiFi netint;
#endif


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

	Serial.println (F("Trying to get an IP address through DHCP"));
#if defined (WEBBINO_USE_ENC28J60) || defined (WEBBINO_USE_WIZ5100) || \
	defined (WEBBINO_USE_WIZ5500) || defined (WEBBINO_USE_ENC28J60_UIP)
	byte mac[6] = {MAC_ADDRESS};
	bool ok = netint.begin (mac, ETH_SS_PIN);
#elif defined (WEBBINO_USE_TEENSY41_NATIVE)
	byte mac[6] = {MAC_ADDRESS};
	bool ok = netint.begin (mac);
#elif defined (WEBBINO_USE_ESP8266)
	espSerial.begin (9600);
	bool ok = netint.begin (espSerial, WIFI_SSID, WIFI_PASSWORD);
#elif defined (WEBBINO_USE_WIFI) || defined (WEBBINO_USE_WIFI101) || \
	  defined (WEBBINO_USE_ESP8266_STANDALONE) || defined (WEBBINO_USE_FISHINO)
	bool ok = netint.begin (WIFI_SSID, WIFI_PASSWORD);
#elif defined (WEBBINO_USE_DIGIFI)
	bool ok = netint.begin ();
#endif

	if (!ok) {
		Serial.println (F("Failed to get configuration from DHCP"));
		while (42)
			;
	} else {
		Serial.println (F("DHCP configuration done:"));
		Serial.print (F("- IP: "));
		Serial.println (netint.getIP ());
		Serial.print (F("- Netmask: "));
		Serial.println (netint.getNetmask ());
		Serial.print (F("- Default Gateway: "));
		Serial.println (netint.getGateway ());

		webserver.begin (netint);

		flashStorage.begin (pages);
		webserver.addStorage (flashStorage);
	}
}

void loop () {
	webserver.loop ();
}
