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

#include "WebServer_WIZ5100.h"

#ifdef WEBBINO_USE_WIZ5100

#include "webbino_debug.h"


void WebClientWIZ5100::init (EthernetClient& c, char* req) {
	internalClient = c;
	request.parse (req);
}

size_t WebClientWIZ5100::write (uint8_t c) {
	return internalClient.write (c);
}

void WebClientWIZ5100::sendReply () {
	internalClient.stop ();
	DPRINTLN (F("Client disconnected"));
}

/****************************************************************************/

byte NetworkInterfaceWIZ5100::retBuffer[6];

// FIXME
NetworkInterfaceWIZ5100::NetworkInterfaceWIZ5100 (): server (SERVER_PORT) {
}

boolean NetworkInterfaceWIZ5100::begin (byte *mac) {
	boolean ret;

	DPRINTLN (F("Using Arduino Ethernet library"));

	memcpy (macAddress, mac, 6);
	if ((ret = Ethernet.begin (mac))) {
		server.begin ();
		dhcp = true;

 		DPRINT (F("Server is at "));
 		DPRINTLN (Ethernet.localIP ());
	}

	//realIp = Ethernet.localIP ();

	return ret;
}

boolean NetworkInterfaceWIZ5100::begin (byte *mac, byte *ip, byte *gw, byte *mask) {
	DPRINTLN (F("Using Arduino Ethernet library"));

	memcpy (macAddress, mac, 6);
	Ethernet.begin (mac, IPAddress (ip), IPAddress (gw), IPAddress (mask));
	server.begin ();
	dhcp = false;

	DPRINT (F("Server is at "));
	DPRINTLN (Ethernet.localIP ());

	return true;
}

WebClient* NetworkInterfaceWIZ5100::processPacket () {
	WebClient *ret = NULL;

	EthernetClient client = server.available ();
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
				// character) and the line is blank, the http request has ended,
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

		// give the web browser time to receive the data
		//delay (500);

		// If we are not returning a client, close the connection
		if (!ret) {
			client.stop ();
			DPRINTLN (F("Client disconnected"));
		}
	}

	return ret;
}

boolean NetworkInterfaceWIZ5100::usingDHCP () {
	return dhcp;
}

byte *NetworkInterfaceWIZ5100::getMAC () {
	return macAddress;
}

IPAddress NetworkInterfaceWIZ5100::getIP () {
	return Ethernet.localIP ();
}

IPAddress NetworkInterfaceWIZ5100::getNetmask () {
	return Ethernet.subnetMask ();
}

IPAddress NetworkInterfaceWIZ5100::getGateway () {
	return Ethernet.gatewayIP ();
}

#endif
