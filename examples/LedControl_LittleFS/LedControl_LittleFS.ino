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
LittleFSStorage littlefsStorage;

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


/* Pin to control, make sure this makes sense:
 * - If using an Uno with the Ethernet shield, remember that it is driven
 *   through SPI, which means that pin 13 (i.e. the pin for LED_BUILTIN) is used
 *   by the SPI clock line, so put a led on a different pin.
 * - Use D0 on NodeMCU!
 * - On Teensy 4.1, pin 13 it's automatically chosen
 */
#ifdef ARDUINO_TEENSY41
const byte ledPin = 13;
#else
const byte ledPin = D0;
#endif

// Logic level turns the led on: on NodeMCU and with most relays, this should
// be LOW
const byte LED_ACTIVE_LEVEL = LOW;

// Pin state (True -> ON)
boolean ledState = false;


/******************************************************************************
 * PAGE FUNCTIONS                                                             *
 ******************************************************************************/

#ifndef ENABLE_PAGE_FUNCTIONS
#error Please define ENABLE_PAGE_FUNCTIONS in webbino_config.h
#endif

HttpStatusCode ledToggle (HttpRequest& request) {
	char *param;

	param = request.get_parameter (F("state"));
	if (strlen (param) > 0) {
		if (strcmp_P (param, PSTR ("on")) == 0) {
			ledState = true;
			digitalWrite (ledPin, LED_ACTIVE_LEVEL);
		} else {
			ledState = false;
			digitalWrite (ledPin, !LED_ACTIVE_LEVEL);
		}
	}
	return HTTP_OK;
}

FileFuncAssoc (indexAss, "/index.html", ledToggle);

FileFuncAssociationArray associations[] PROGMEM = {
    &indexAss,
    NULL
};


/******************************************************************************
 * DEFINITION OF TAGS                                                         *
 ******************************************************************************/

#define REP_BUFFER_LEN 32
char replaceBuffer[REP_BUFFER_LEN];
PString pBuffer (replaceBuffer, REP_BUFFER_LEN);

PString& evaluate_onoff_checked (void *data) {
	boolean st = reinterpret_cast<int> (data);
	if (ledState == st) {
		pBuffer.print ("checked");
	}

	return pBuffer;
}

PString& evaluate_webbino_version (void *data __attribute__ ((unused))) {
	pBuffer.print (WEBBINO_VERSION);

	return pBuffer;
}


EasyReplacementTag (tagStateOnChecked, ST_ON_CHK, evaluate_onoff_checked, true);
EasyReplacementTag (tagStateOffChecked, ST_OFF_CHK, evaluate_onoff_checked, false);
EasyReplacementTag (tagWebbinoVer, WEBBINO_VER, evaluate_webbino_version);

EasyReplacementTagArray tags[] PROGMEM = {
	&tagStateOnChecked,
	&tagStateOffChecked,
	&tagWebbinoVer,
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
	}


	Serial.println (F("DHCP configuration done:"));
	Serial.print (F("- IP: "));
	Serial.println (netint.getIP ());
	Serial.print (F("- Netmask: "));
	Serial.println (netint.getNetmask ());
	Serial.print (F("- Default Gateway: "));
	Serial.println (netint.getGateway ());

	webserver.begin (netint);
	webserver.enableReplacementTags (tags);

	littlefsStorage.begin ();
	webserver.addStorage (littlefsStorage);
	webserver.associateFunctions (associations);

	// Prepare pin
	digitalWrite (ledPin, !LED_ACTIVE_LEVEL);		// Off
	pinMode (ledPin, OUTPUT);
}

void loop () {
	webserver.loop ();
}
