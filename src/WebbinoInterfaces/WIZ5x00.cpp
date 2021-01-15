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

#include <Arduino.h>		// See https://github.com/arduino-libraries/Ethernet/issues/72
#include "WIZ5x00.h"

#if defined (WEBBINO_USE_WIZ5100) || defined (WEBBINO_USE_WIZ5500) || \
    defined (WEBBINO_USE_ENC28J60_UIP) || defined (WEBBINO_USE_TEENSY41_NATIVE)

#include "webbino_debug.h"


void WebClientWIZ5x00::begin (EthernetClient& c, char* req) {
	WebClient::begin (req);
	internalClient = c;
}

size_t WebClientWIZ5x00::doWrite (const uint8_t *buf, size_t n) {
	return internalClient.write (buf, n);
}

void WebClientWIZ5x00::sendReply () {
	WebClient::sendReply ();
	internalClient.stop ();
	DPRINTLN (F("Client disconnected"));
}

/****************************************************************************/

byte NetworkInterfaceWIZ5x00::retBuffer[6];

// FIXME
NetworkInterfaceWIZ5x00::NetworkInterfaceWIZ5x00 (): server (SERVER_PORT) {
}

#if defined (WEBBINO_USE_TEENSY41_NATIVE)
boolean NetworkInterfaceWIZ5x00::begin (byte *mac) {
#else
boolean NetworkInterfaceWIZ5x00::begin (byte *mac, const byte ssPin) {
	Ethernet.init (ssPin);
#endif

	boolean ret;

	memcpy (macAddress, mac, 6);
	if ((ret = Ethernet.begin (mac))) {
		server.begin ();
		dhcp = true;

 		DPRINT (F("Server is at "));
 		DPRINTLN (Ethernet.localIP ());
	}

	return ret;
}

#if defined (WEBBINO_USE_TEENSY41_NATIVE)
boolean NetworkInterfaceWIZ5x00::begin (byte *mac, IPAddress ip, IPAddress mask, IPAddress gw, IPAddress dns) {
#else
boolean NetworkInterfaceWIZ5x00::begin (byte *mac, IPAddress ip, IPAddress mask, IPAddress gw, IPAddress dns, const byte ssPin) {
	Ethernet.init (ssPin);
#endif

	memcpy (macAddress, mac, 6);
	Ethernet.begin (mac, ip, dns, gw, mask);
	server.begin ();
	dhcp = false;

	DPRINT (F("Server is at "));
	DPRINTLN (Ethernet.localIP ());

	return true;
}

boolean lineIsInteresting (const char *line) {
	return strncmp_P (line, PSTR ("Authorization: Basic "), 21) == 0;
}

enum RequestState {
	RS_URI,
	RS_HEADERS,
	RS_BODY,
	RS_COMPLETE
};

WebClient* NetworkInterfaceWIZ5x00::processPacket () {
	WebClient *ret = NULL;

	EthernetClient client = server.available ();
	if (client) {
#ifndef WEBBINO_USE_ENC28J60_UIP
		// ArduinoUIP has no remoteIP()
		DPRINT (F("New client from "));
		DPRINTLN (client.remoteIP ());
#else
		DPRINTLN (F("New client"));
#endif

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
				DPRINTLN (F("COMPLETE"));
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

boolean NetworkInterfaceWIZ5x00::usingDHCP () {
	return dhcp;
}

byte *NetworkInterfaceWIZ5x00::getMAC () {
	return macAddress;
}

IPAddress NetworkInterfaceWIZ5x00::getIP () {
	return Ethernet.localIP ();
}

IPAddress NetworkInterfaceWIZ5x00::getNetmask () {
	return Ethernet.subnetMask ();
}

IPAddress NetworkInterfaceWIZ5x00::getGateway () {
	return Ethernet.gatewayIP ();
}

IPAddress NetworkInterfaceWIZ5x00::getDns() {
	return Ethernet.dnsServerIP ();
}

#endif
