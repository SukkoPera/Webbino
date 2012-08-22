/***************************************************************************
 *   This file is part of SmartStrip.                                      *
 *                                                                         *
 *   Copyright (C) 2012 by SukkoPera                                       *
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

#ifndef _WEBSERVER5100_H_
#define _WEBSERVER5100_H_

#ifndef USE_ENC28J60

#include <SPI.h>
#include <Ethernet.h>
#include "WebClientBase.h"
#include "WebServerBase.h"


class WebClientWIZ5100: public WebClientBase {
	EthernetClient& internalClient;

public:
	WebClientWIZ5100 (EthernetClient& client): internalClient (client) {
	}

	size_t write (uint8_t c) {
		return internalClient.write (c);
	}
};
	

const char info[] PROGMEM = "Using Arduino Ethernet library";

class WebServerWIZ5100: public WebServerBase {
	IPAddress realIp;
	IPAddress netmask;
	IPAddress gateway;
	EthernetServer server;
	byte ethernetBuffer[ETHERNET_BUFSIZE];
	unsigned int ethernetBufferSize;
	
public:
	WebServerWIZ5100 (): server (SERVER_PORT) {
	}

	bool begin (byte *mac) {
		// start the Ethernet connection and the server:
		Serial.println (reinterpret_cast<const __FlashStringHelper *> (info));

		Ethernet.begin (mac);
		server.begin ();
// 		Serial.print (F("Server is at "));
// 		Serial.println (Ethernet.localIP ());

		realIp = Ethernet.localIP ();
		
		return true;
	}
	
	bool begin (byte *mac, byte *ip, byte *gw, byte *mask) {
		// start the Ethernet connection and the server:
		Serial.println (reinterpret_cast<const __FlashStringHelper *> (info));

		realIp = IPAddress (ip[0], ip[1], ip[2], ip[3]);
		netmask = IPAddress (mask[0], mask[1], mask[2], mask[3]);
		gateway = IPAddress (gw[0], gw[1], gw[2], gw[3]);
		Ethernet.begin (mac, realIp, gateway, netmask);
		server.begin ();
// 		Serial.print (F("Server is at "));
// 		Serial.println (Ethernet.localIP ());

		return true;
	}

	bool processPacket () {
		EthernetClient client = server.available ();
		if (client) {
// 			Serial.println (F("New client"));

			// An http request ends with a blank line
			boolean currentLineIsBlank = true;
			ethernetBufferSize = 0;
			while (client.connected ()) {
				if (client.available ()) {
					char c = client.read ();

					/* OK, I don't like this, but in order to maintain the same
					 * program logic as for the ENC28J60, it is necessary.*/
					if (ethernetBufferSize < ETHERNET_BUFSIZE)
						ethernetBuffer[ethernetBufferSize++] = c;
					else
						Serial.println (F("Ethernet buffer overflow"));
					
					// If you've gotten to the end of the line (received a newline
					// character) and the line is blank, the http request has ended,
					if (c == '\n' && currentLineIsBlank) {
						HTTPRequestParser request = HTTPRequestParser ((char *) ethernetBuffer);

						Serial.print (F("Request for \""));
						Serial.print (request.url);
						Serial.println (F("\""));
						//Serial.println ((char *) Ethernet::buffer + pos);

						// Send the requested page
						WebClientWIZ5100 webClient (client);
						sendPage (request, webClient);
						break;
					}

					if (c == '\n') {
						// you're starting a new line
						currentLineIsBlank = true;
					} else if (c != '\r') {
						// you've gotten a character on the current line
						currentLineIsBlank = false;
					}
				}
			}

			// give the web browser time to receive the data
			delay (500);
			// close the connection:
			client.stop ();
// 			Serial.println (F("Client disonnected"));
		}
	}

	byte *getIP () {
// 		realIp = Ethernet.localIP ();
		static uint32_t tmp = static_cast<uint32_t> (realIp);
		return reinterpret_cast<byte *> (&tmp);
	}

	byte *getNetmask () {
// 		realIp = Ethernet.localIP ();
		static uint32_t tmp = static_cast<uint32_t> (netmask);
		return reinterpret_cast<byte *> (&tmp);
	}

	byte *getGateway () {
// 		realIp = Ethernet.localIP ();
		static uint32_t tmp = static_cast<uint32_t> (gateway);
		return reinterpret_cast<byte *> (&tmp);
	}
};


class WebClient: public WebClientWIZ5100 {
};

class WebServer: public WebServerWIZ5100 {
};

#endif

#endif
