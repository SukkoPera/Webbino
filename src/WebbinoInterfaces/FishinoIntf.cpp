/***************************************************************************
 *   This file is part of Webbino                                          *
 *                                                                         *
 *   Copyright (C) 2012-2016 by SukkoPera                                  *
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

#include "FishinoIntf.h"

#ifdef WEBBINO_USE_FISHINO

#include "webbino_debug.h"

void FishinoWebClient::begin (FishinoClient& c, char* req) {
	WebClient::begin (req);

	internalClient = c;
}

size_t FishinoWebClient::doWrite (const uint8_t *buf, size_t n) {
	return internalClient.write (buf, n);
}

void FishinoWebClient::sendReply () {
	WebClient::sendReply ();

	internalClient.stop ();
	DPRINTLN (F("Client disconnected"));
}


/****************************************************************************/

byte FishinoInterface::retBuffer[6];

FishinoInterface::FishinoInterface (): server (SERVER_PORT) {
}

boolean FishinoInterface::begin (const char *_ssid, const char *_password) {
	// Reset and test WiFi module
	while (!Fishino.reset ())
		DPRINTLN (F("Fishino RESET FAILED, RETRYING"));
	DPRINTLN (F("Fishino WiFi RESET OK"));

	// Setup PHY mode -- some AP don't like 11B mode
	Fishino.setPhyMode (PHY_MODE_11G);

	// Go into station mode
	Fishino.setMode (STATION_MODE);

	// Try forever to connect to AP
	DPRINT (F("Connecting to AP: "));
	DPRINTLN (_ssid);
	while (!Fishino.begin (_ssid, _password)) {
		delay (500);
	}
	DPRINTLN (F("Joined AP"));

	// Start DHCP client
	Fishino.staStartDHCP ();

	// Wait till connection is established
	DPRINTLN (F("Waiting for IP..."));
	while (Fishino.status () != STATION_GOT_IP) {
		delay (500);
	}
	DPRINT (F("Server is at "));
	DPRINTLN (Fishino.localIP ());
	dhcp = true;

	server.begin ();

	return true;
}

boolean FishinoInterface::begin (const char *_ssid, const char *_password,
	  IPAddress ip, IPAddress mask, IPAddress gw, IPAddress dns) {

	// Reset and test WiFi module
	while (!Fishino.reset ())
		DPRINTLN (F("Fishino RESET FAILED, RETRYING"));
	DPRINTLN (F("Fishino WiFi RESET OK"));

	// Setup PHY mode -- some AP don't like 11B mode
	Fishino.setPhyMode (PHY_MODE_11G);

	// Go into station mode
	Fishino.setMode (STATION_MODE);

	// Try forever to connect to AP
	DPRINT (F("Connecting to AP: "));
	DPRINTLN (_ssid);
	while (!Fishino.begin (_ssid, _password)) {
		delay (500);
	}
	DPRINT (F("Joined AP"));

	// Configure IP address
	Fishino.config (ip, gw, mask, dns);

	dhcp = false;

	server.begin ();

	DPRINT (F("Server is at "));
	DPRINTLN (Fishino.localIP ());

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

WebClient* FishinoInterface::processPacket () {
	WebClient *ret = NULL;

	FishinoClient client = server.available ();
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

boolean FishinoInterface::usingDHCP () {
	return dhcp;
}

byte *FishinoInterface::getMAC () {
	const byte *mac = Fishino.macAddress ();
	memcpy (retBuffer, mac, 6);
	return retBuffer;
}

IPAddress FishinoInterface::getIP () {
	return Fishino.localIP ();
}

IPAddress FishinoInterface::getNetmask () {
	return Fishino.subnetMask ();
}

IPAddress FishinoInterface::getGateway () {
	return Fishino.gatewayIP ();
}

#endif
