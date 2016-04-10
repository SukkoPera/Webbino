/***************************************************************************
 *   This file is part of SmartStrip.                                      *
 *                                                                         *
 *   Copyright (C) 2012-2016 by SukkoPera                                  *
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

#include "WebServer_WIZ5100.h"

#ifdef USE_WIZ5100


WebClientWIZ5100::WebClientWIZ5100 (EthernetClient& client): internalClient (client) {
}

size_t WebClientWIZ5100::write (uint8_t c) {
	return internalClient.write (c);
}

/****************************************************************************/

#ifdef WEBBINO_VERBOSE
const char info[] PROGMEM = "Using Arduino Ethernet library";
#endif

byte WebServerWIZ5100::retBuffer[6];

WebServerWIZ5100::WebServerWIZ5100 (): server (SERVER_PORT) {
}

bool WebServerWIZ5100::begin (byte *mac) {
	bool ret;

#ifdef WEBBINO_VERBOSE
	// start the Ethernet connection and the server:
	Serial.println (reinterpret_cast<const __FlashStringHelper *> (info));
#endif

	memcpy (macAddress, mac, 6);
	if ((ret = WebServer::begin (mac)) && (ret = Ethernet.begin (mac)))
		server.begin ();
// 		Serial.print (F("Server is at "));
// 		Serial.println (Ethernet.localIP ());

	//realIp = Ethernet.localIP ();

	return ret;
}

bool WebServerWIZ5100::begin (byte *mac, byte *ip, byte *gw, byte *mask) {
	bool ret;

#ifdef WEBBINO_VERBOSE
	// start the Ethernet connection and the server:
	Serial.println (reinterpret_cast<const __FlashStringHelper *> (info));
#endif

	memcpy (macAddress, mac, 6);
	if ((ret = WebServer::begin (mac, ip, gw, mask))) {
		Ethernet.begin (mac, IPAddress (ip), IPAddress (gw), IPAddress (mask));
		server.begin ();
	// 		Serial.print (F("Server is at "));
	// 		Serial.println (Ethernet.localIP ());
	}

	return ret;
}

bool WebServerWIZ5100::processPacket () {
	EthernetClient client = server.available ();
	if (client) {
// 			Serial.println (F("New client"));

		// An http request ends with a blank line
		bool currentLineIsBlank = true;
		ethernetBufferSize = 0;
		bool copy = true;
		while (client.connected ()) {
			if (client.available ()) {
				char c = client.read ();

				/* OK, I don't like this, but in order to maintain the same
					* program logic as for the ENC28J60, it is necessary.*/
				if (ethernetBufferSize < sizeof (ethernetBuffer)) {
					if (copy)
						ethernetBuffer[ethernetBufferSize++] = c;
				} else {
#ifdef WEBBINO_VERBOSE
					Serial.println (F("Ethernet buffer overflow"));
#endif
				}

				// If you've gotten to the end of the line (received a newline
				// character) and the line is blank, the http request has ended,
				if (c == '\n' && currentLineIsBlank) {
					HTTPRequestParser request = HTTPRequestParser ((char *) ethernetBuffer);

#ifdef WEBBINO_VERBOSE
					Serial.print (F("Request for \""));
					Serial.print (request.url);
					Serial.println (F("\""));
#endif

					// Send the requested page
					WebClientWIZ5100 webClient (client);
					sendPage (request, webClient);
					break;
				}

				if (c == '\n') {
					// See if we got the URL line
					if (strncmp_P ((char *) ethernetBuffer, PSTR ("GET "), 4) == 0) {
						// Yes, ignore the rest
						ethernetBuffer[ethernetBufferSize - 1] = '\0';
						copy = false;
					} else {
						// No, start over
						ethernetBufferSize = 0;
					}

					// you're starting a new line
					currentLineIsBlank = true;
				} else if (c != '\r') {
					// you've gotten a character on the current line
					currentLineIsBlank = false;
				}
			}
		}

		// give the web browser time to receive the data
		//delay (500);
		// close the connection:
		client.stop ();
// 		Serial.println (F("Client disconnected"));
	}
}

byte *WebServerWIZ5100::getMAC () {
	return macAddress;
}

byte *WebServerWIZ5100::getIP () {
	(*(uint32_t *) &retBuffer) = static_cast<uint32_t> (Ethernet.localIP ());
	return retBuffer;
}

byte *WebServerWIZ5100::getNetmask () {
	(*(uint32_t *) &retBuffer) = static_cast<uint32_t> (Ethernet.subnetMask ());
	return retBuffer;
}

byte *WebServerWIZ5100::getGateway () {
	(*(uint32_t *) &retBuffer) = static_cast<uint32_t> (Ethernet.gatewayIP ());
	return retBuffer;
}

#endif
