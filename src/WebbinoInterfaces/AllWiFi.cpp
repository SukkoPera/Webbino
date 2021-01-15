/***************************************************************************
 *   This file is part of Webbino                                          *
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

/* This backend handles the following network interfaces:
 * Arduino + Official WiFi Shield (Untested)
 * Arduino + Official WiFi01 Shield (Untested)
 * Arduino MKR1000 (Untested)
 * Arduino + WiFiEsp (+ ESP8266)
 * ESP8266 Standalone
 * ESP32 Standalone
 */

#include "AllWiFi.h"

#if defined (WEBBINO_USE_WIFI) || defined (WEBBINO_USE_WIFI101) || \
	  defined (WEBBINO_USE_ESP8266) || defined (WEBBINO_USE_ESP8266_STANDALONE)

#include "webbino_debug.h"

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
	}
	DPRINT (F("Joined AP, server is at "));
	DPRINTLN (WiFi.localIP ());

	server.begin ();

	return true;
}

#ifndef WEBBINO_USE_ESP8266		// WiFiEsp does not support this
boolean NetworkInterfaceWiFi::begin (const char *_ssid, const char *_password, IPAddress ip, IPAddress mask, IPAddress gw, IPAddress dns) {
	boolean ret = false;

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

	// Set IP configuration
#if (defined (WEBBINO_USE_WIFI) && defined (ARDUINO_ARCH_AVR)) || defined (WEBBINO_USE_WIFI101)
	// The original Arduino API has this parameter order and returns nothing
	WiFi.config (ip, dns, gw, mask);
	if (false) {
#else
	/* Other implementations seem to converge to the following (including ESP32,
	 * hence the check for ARDUINO_ARCH_AVR above).
	 */
	if (!WiFi.config (ip, gw, mask, dns)) {
#endif
		DPRINTLN (F("Cannot set static IP address configuration"));
	} else {
		// Attempt to connect to WiFi network
		DPRINT (F("Connecting to AP: "));
		DPRINTLN (_ssid);
		WiFi.begin (const_cast<char *> (_ssid), _password);
		while (WiFi.status () != WL_CONNECTED) {
			delay (500);
		}
		DPRINT (F("Joined AP, server is at "));
		DPRINTLN (WiFi.localIP ());

		server.begin ();

		ret = true;
	}

	return ret;
}
#endif

// ESP8266 and ESP32 also support Access-Point mode - Thanks gpb01!
#if defined (WEBBINO_USE_ESP8266_STANDALONE) || defined (ARDUINO_ARCH_ESP32)
boolean NetworkInterfaceWiFi::beginAP (const char *_ssid, const char *_password, IPAddress& address) {
	boolean ret = false;

    DPRINT (F("Creating AP: "));
	DPRINTLN (_ssid);
	WiFi.mode (WIFI_AP);
    ret = WiFi.softAP ((_ssid), _password);
    if (ret) {
		address = WiFi.softAPIP ();
		server.begin ();
	}

	return ret;
}
#endif

boolean lineIsInteresting (const char *line) {
	return strncmp_P (line, PSTR ("Authorization: Basic "), 21) == 0;
}

enum RequestState {
	RS_URI,
	RS_HEADERS,
	RS_BODY,
	RS_COMPLETE
};

WebClient* NetworkInterfaceWiFi::processPacket () {
	WebClient *ret = NULL;

	InternalClient client = server.available ();
	if (client) {
		DPRINT (F("New client from "));
		DPRINTLN (client.remoteIP ());

#ifdef CLIENT_TIMEOUT
		lastPacketReceived = millis ();
#endif

		// An http request ends with a blank line
		boolean currentLineIsBlank = true;
		ethernetBufferSize = 0;
		RequestState state = RS_URI;
		unsigned int lastLineStart = 0;
		while (client.connected () && state != RS_COMPLETE) {
#ifdef CLIENT_TIMEOUT
		    // Check for connection timeout
		    if (millis () - lastPacketReceived > CLIENT_TIMEOUT) {
		        DPRINTLN (F("Client connection timeout"));
		        break;
		    }
#endif

			if (client.available ()) {
				char c = client.read ();

#ifdef CLIENT_TIMEOUT
				// Something was received, reset timeout
				lastPacketReceived = millis();
#endif

				/* We are only interested in the first line of the HTTP request
				 * (i.e.: the one that contains the method and the URI), so if
				 * we haven't seen an LF yet, let's append to our buffer
				 */
				// Reserve one place for the terminator we'll append later
				if (ethernetBufferSize < sizeof (ethernetBuffer) - 1) {
					ethernetBuffer[ethernetBufferSize++] = c;
				} else {
					// No more space in buffer, ignore
					DPRINTLN (F("Ethernet buffer overflow"));
				}

				if (c == '\n') {		// End of a line
					switch (state) {
						case RS_URI:
							/* Great, we have extracted the request line! The
							 * following lines will be header lines.
							 */
							state = RS_HEADERS;
							lastLineStart = ethernetBufferSize;
							break;;
						case RS_HEADERS:
							if (currentLineIsBlank) {
								/* We got to the end of the line and the line is blank,
								 * this means the http request has ended
								 */
								 state = RS_BODY;
							} else {
								/* Got a header line, see if it's an interesting one
								 * or discard it
								 */
								const char *line = reinterpret_cast<char *> (ethernetBuffer + lastLineStart);
								ethernetBuffer[ethernetBufferSize] = '\0';	// Terminate
								if (lineIsInteresting (line)) {
									lastLineStart = ethernetBufferSize;
								} else {
									DPRINT (F("Discarding header line: "));
									DPRINTLN (line);
									ethernetBufferSize = lastLineStart;
								}
							}
							break;
						case RS_BODY:
							// Just keep it
							lastLineStart = ethernetBufferSize;

							// Request complete, get out
							ethernetBuffer[ethernetBufferSize] = '\0';	// Terminate

							state = RS_COMPLETE;
							break;
						default:
							break;
					}

					// A new line is starting
					currentLineIsBlank = true;
				} else if (c != '\r') {
					// Got a character on the current line
					currentLineIsBlank = false;
				}
			} else if (state == RS_BODY) {
				DPRINTLN (F("Detected end of headers"));
				ethernetBuffer[ethernetBufferSize] = '\0';	// Terminate
				state = RS_COMPLETE;
			}
		}

		// FIXME: Make const
		if (state == RS_COMPLETE && ethernetBufferSize > 0) {
			// Got a request to parse
			char *req = reinterpret_cast<char *> (ethernetBuffer);
			webClient.begin (client, req);
			ret = &webClient;
		} else {
			// Close the connection
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

IPAddress NetworkInterfaceWiFi::getDns() {
	return WiFi.dnsIP ();
}

#endif
