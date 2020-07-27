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
	  IPAddress ip, IPAddress dns, IPAddress gw, IPAddress mask) {

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

WebClient* FishinoInterface::processPacket () {
	WebClient *ret = NULL;

	FishinoClient client = server.available ();
	if (client) {
		DPRINTLN (F("New client"));

		// An http request ends with a blank line
		boolean currentLineIsBlank = true;
		ethernetBufferSize = 0;
		boolean copy = true;
		while (client.connected ()) {
			if (client.available ()) {
				char c = client.read ();
				if (copy) {
					if (ethernetBufferSize < sizeof (ethernetBuffer)) {
						ethernetBuffer[ethernetBufferSize++] = c;
					} else {
						DPRINTLN (F("Ethernet buffer overflow"));
						break;
					}
				}

				// If you've gotten to the end of the line (received a newline
				// character) and the line is blank, the http request has ended
				if (c == '\n' && currentLineIsBlank) {
					webClient.begin (client, reinterpret_cast<char *> (ethernetBuffer));
					ret = &webClient;
					break;
				}

				if (c == '\n') {
					// See if we got the URL line
					if (strncmp_P ((char *) ethernetBuffer, PSTR ("GET "), 4) == 0) {
						// Yes, ignore the rest
						// FIXME: Avoid buffer underflow
						ethernetBuffer[ethernetBufferSize - 1] = '\0';
						copy = false;
					} else {
						// No, start over
						DPRINT (F("Discarding header line: \""));
						DPRINT (reinterpret_cast<char *> (ethernetBuffer));
						DPRINTLN (F("\""));

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
