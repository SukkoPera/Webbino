/***************************************************************************
 *   This file is part of Webbino                                          *
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

#include "AllWiFi.h"

#if defined (WEBBINO_USE_WIFI) || defined (WEBBINO_USE_WIFI101) || \
	  defined (WEBBINO_USE_ESP8266) || defined (WEBBINO_USE_ESP8266_STANDALONE)

#include <webbino_debug.h>

void WebClientWifi::begin (InternalClient& c, char* req) {
	WebClient::begin (req);

	internalClient = c;
}

size_t WebClientWifi::doWrite (const uint8_t *buf, size_t n) {
	return internalClient.write (buf, n);
}

void WebClientWifi::sendReply () {
	WebClient::sendReply ();

	internalClient.stop ();
	DPRINTLN (F("Client disconnected"));
}


/****************************************************************************/

byte NetworkInterfaceWiFi::retBuffer[6];

// FIXME
NetworkInterfaceWiFi::NetworkInterfaceWiFi (): server (80) {
}

#if defined (WEBBINO_USE_WIFI) || defined (WEBBINO_USE_WIFI101) || \
	  defined (WEBBINO_USE_ESP8266_STANDALONE)
boolean NetworkInterfaceWiFi::begin (const char *_ssid, const char *_password) {
#elif defined (WEBBINO_USE_ESP8266)
boolean NetworkInterfaceWiFi::begin (Stream& _serial, const char *_ssid, const char *_password) {
	WiFi.init (&_serial);
#endif

/* It seems that these two API calls do not work as expected on ESP32, so let's
 * just skip them.
 */
#ifndef ARDUINO_ARCH_ESP32
	// Check for the presence of the WiFi interface
	if (WiFi.status () == WL_NO_SHIELD) {
		DPRINTLN (F("WiFi interface not found"));
		return false;
	}

#ifndef WEBBINO_USE_ESP8266_STANDALONE
	DPRINT (F("FW Version: "));
	DPRINTLN (WiFi.firmwareVersion ());
#endif
#endif

	// Attempt to connect to WiFi network
	// FIXME: Maybe only allow a finite number of attempts
	DPRINT (F("Connecting to AP: "));
	DPRINTLN (_ssid);
	WiFi.begin (const_cast<char *> (_ssid), _password);
	while (WiFi.status () != WL_CONNECTED) {
		delay (500);
	};
	DPRINT (F("Joined AP, server is at "));
	DPRINTLN (WiFi.localIP ());

	server.begin ();

	return true;

}

WebClient* NetworkInterfaceWiFi::processPacket () {
	WebClient *ret = NULL;

	InternalClient client = server.available ();
	if (client) {
		DPRINTLN (F("New client"));

		// An http request ends with a blank line
		boolean currentLineIsBlank = true;
		ethernetBufferSize = 0;
		boolean firstLine = true;
		while (client.connected ()) {
			if (client.available ()) {
				char c = client.read ();

				/* We are only interested in the first line of the HTTP request
				 * (i.e.: the one that contains the method and the URI), so if
				 * we haven't seen an LF yet, let's append to our buffer
				 */
				if (firstLine) {
					// Reserve one place for the terminator we'll append later
					if (ethernetBufferSize < sizeof (ethernetBuffer) - 1) {
						ethernetBuffer[ethernetBufferSize++] = c;
					} else {
						// No more space in buffer, ignore
						DPRINTLN (F("Ethernet buffer overflow"));
					}
				}

				if (c == '\n') {		// End of a line
					if (currentLineIsBlank) {
						/* We got to the end of the line and the line is blank,
						 * this means the http request has ended
						 */
						if (!firstLine) {
							webClient.begin (client, reinterpret_cast<char *> (ethernetBuffer));
							ret = &webClient;
						} else {
							// Got an empty request, don't be fooled!
						}

						// In any case, let's get out
						break;
					} else if (firstLine) {
						// Great, we have extracted the request line!
						firstLine = false;

						/* Terminate the line. Note that this cannot underflow,
						 * as at least one char was copied for sure
						 */
						ethernetBuffer[ethernetBufferSize - 1] = '\0';	// Terminate
					} else {
						/* This is a header line, at the moment we'll just
						 * ignore it altogether
						 */
					}

					// A new line is starting
					currentLineIsBlank = true;
				} else if (c != '\r') {
					// Got a character on the current line
					currentLineIsBlank = false;
				}
			}
		}

		// If we are not returning a client, close the connection
		if (!ret) {
			client.stop ();
			DPRINTLN (F("Client disconnected"));
		}
	}

	return ret;
}

boolean NetworkInterfaceWiFi::usingDHCP () {
	// FIXME
	return true;
}

byte *NetworkInterfaceWiFi::getMAC () {
	return WiFi.macAddress (retBuffer);
}

IPAddress NetworkInterfaceWiFi::getIP () {
	return WiFi.localIP ();
}

IPAddress NetworkInterfaceWiFi::getNetmask () {
	return WiFi.subnetMask ();
}

IPAddress NetworkInterfaceWiFi::getGateway () {
	return WiFi.gatewayIP ();
}

#endif
