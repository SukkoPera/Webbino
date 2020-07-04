/***************************************************************************
 *   This file is part of Webbino.                                         *
 *                                                                         *
 *   Copyright (C) 2012-2019 by SukkoPera                                  *
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
DummyStorage dummyStorage;

// Instantiate the network interface defined in the Webbino headers
#if defined (WEBBINO_USE_ENC28J60)
	#include <WebbinoInterfaces/ENC28J60.h>
	NetworkInterfaceENC28J60 netint;
#elif defined (WEBBINO_USE_WIZ5100) || defined (WEBBINO_USE_WIZ5500)
	#include <WebbinoInterfaces/WIZ5x00.h>
	NetworkInterfaceWIZ5x00 netint;
#elif defined (WEBBINO_USE_ESP8266)
	#include <WebbinoInterfaces/AllWiFi.h>

	#include <SoftwareSerial.h>
	SoftwareSerial swSerial (6, 7);

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
 * DEFINITION OF TAGS                                                         *
 ******************************************************************************/

#define REP_BUFFER_LEN 32
static char replaceBuffer[REP_BUFFER_LEN];
PString subBuffer (replaceBuffer, REP_BUFFER_LEN);

static PString& evaluate_content (void *data) {
	(void) data;

	// This has already been filled in by the page function, so return it right away
	return subBuffer;
}

EasyReplacementTag (tagDummy, DUMMY, evaluate_content);

EasyReplacementTagArray tags[] PROGMEM = {
	&tagDummy,
	NULL
};


/******************************************************************************
 * PAGE FUNCTIONS                                                             *
 ******************************************************************************/

#ifndef ENABLE_PAGE_FUNCTIONS
#error Please define ENABLE_PAGE_FUNCTIONS in webbino_config.h
#endif

void relayFunc (HTTPRequestParser& request) {
	if (request.matchResult.nMatches == 1) {
		char temp[8];
		strlcpy (temp, request.uri + request.matchResult.matchPositions[0], request.matchResult.matchLengths[0] + 1);
		byte relayNo = atoi (temp);

		Serial.print (F("Working on relay "));
		Serial.println (relayNo);

		switch (request.method) {
			case HTTPRequestParser::METHOD_DELETE:
				subBuffer.print (F("ICH MUSS ZERSTOEREN!"));
				break;
			default:
				subBuffer.print (F("FEUER FREI!"));
				break;
		}
	} else {
		Serial.println (F("Invalid request"));
	}
}

FileFuncAssoc (indexAss, "/rest/relay/*", relayFunc);

FileFuncAssociationArray associations[] PROGMEM = {
	&indexAss,
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
#if defined (WEBBINO_USE_ENC28J60) || defined (WEBBINO_USE_WIZ5100) || defined (WEBBINO_USE_WIZ5500)
	byte mac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
	bool ok = netint.begin (mac);
#elif defined (WEBBINO_USE_ESP8266)
	swSerial.begin (9600);
	bool ok = netint.begin (swSerial, WIFI_SSID, WIFI_PASSWORD);
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

		//~ dummyStorage.begin ();		// Even unnecessary!
		webserver.addStorage (dummyStorage);
		webserver.associateFunctions (associations);
		webserver.enableReplacementTags (tags);
	}
}

void loop () {
	webserver.loop ();
}
