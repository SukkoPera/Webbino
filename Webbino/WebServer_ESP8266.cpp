/***************************************************************************
 *   This file is part of Webbino                                          *
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

#include "WebServer_ESP8266.h"

#ifdef WEBBINO_USE_ESP8266

#include "webbino_debug.h"

void WebClientESP8266::init (WiFiEspClient& c, char* req) {
	internalClient = c;
	request.parse (req);
	avail = 0;
}

size_t WebClientESP8266::write (uint8_t c) {
	buf[avail++] = c;

	if (avail >= CLIENT_BUFSIZE) {
		flushBuffer ();
	}

	return 1;
}

void WebClientESP8266::flushBuffer () {
	if (avail > 0) {
		//~ DPRINT (F("Flushing "));
		//~ DPRINT (avail);
		//~ DPRINTLN (F(" bytes to client"));

		internalClient.write (buf, avail);
		avail = 0;
	}
}

void WebClientESP8266::sendReply () {
	flushBuffer ();
	internalClient.stop ();
	DPRINTLN (F("Client disconnected"));
}


/****************************************************************************/

byte NetworkInterfaceESP8266::retBuffer[6];

// FIXME
NetworkInterfaceESP8266::NetworkInterfaceESP8266 (): server (80) {
}

boolean NetworkInterfaceESP8266::begin (Stream& _serial, const char *_ssid, const char *_password) {
	WiFi.init (&_serial);

	// Check for the presence of ESP
	if (WiFi.status() == WL_NO_SHIELD) {
		DPRINTLN (F("ESP8266 not found"));
		return false;
	}

	DPRINT (F("FW Version: "));
	DPRINTLN (WiFi.firmwareVersion ());

	// Attempt to connect to WiFi network
	int status;
	do {
		DPRINT (F("Connecting to AP: "));
		DPRINTLN (_ssid);
		status = WiFi.begin (const_cast<char *> (_ssid), _password);
	} while (status != WL_CONNECTED);

	DPRINT (F("Joined AP, local IP address: "));
	DPRINTLN (WiFi.localIP ());

	server.begin ();

	return true;

}

WebClient* NetworkInterfaceESP8266::processPacket () {
	WebClient *ret = NULL;

	WiFiEspClient client = server.available ();
	if (client) {
		DPRINTLN (F("New client"));

		// An http request ends with a blank line
		boolean currentLineIsBlank = true;
		ethernetBufferSize = 0;
		boolean copy = true;
		while (client.connected ()) {
			if (client.available ()) {
				char c = client.read ();
				if (ethernetBufferSize < sizeof (ethernetBuffer)) {
					if (copy)
						ethernetBuffer[ethernetBufferSize++] = c;
				} else {
					DPRINTLN (F("Ethernet buffer overflow"));
				}

				// If you've gotten to the end of the line (received a newline
				// character) and the line is blank, the http request has ended
				if (c == '\n' && currentLineIsBlank) {
					webClient.init (client, (char *) ethernetBuffer);
					ret = &webClient;
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

		// If we are not returning a client, close the connection
		if (!ret) {
			client.stop ();
			DPRINTLN (F("Client disconnected"));
		}
	}

	return ret;
}

boolean NetworkInterfaceESP8266::usingDHCP () {
	// FIXME
	return true;
}

byte *NetworkInterfaceESP8266::getMAC () {
	return WiFi.macAddress (retBuffer);
}

byte *NetworkInterfaceESP8266::getIP () {
	(*(uint32_t *) &retBuffer) = static_cast<uint32_t> (WiFi.localIP ());
	return retBuffer;
}

byte *NetworkInterfaceESP8266::getNetmask () {
	(*(uint32_t *) &retBuffer) = static_cast<uint32_t> (WiFi.subnetMask ());
	return retBuffer;
}

byte *NetworkInterfaceESP8266::getGateway () {
	(*(uint32_t *) &retBuffer) = static_cast<uint32_t> (WiFi.gatewayIP ());
	return retBuffer;
}

#endif
