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

#ifndef _WEBSERVER5100_H_
#define _WEBSERVER5100_H_

#include "webbino_common.h"

#ifdef USE_WIZ5100

#include <SPI.h>
#include <Ethernet.h>
#include "WebClient.h"
#include "WebServer.h"


class WebClientWIZ5100: public WebClient {
private:
	EthernetClient& internalClient;

public:
	WebClientWIZ5100 (EthernetClient& client);

	size_t write (uint8_t c);
};


class WebServerWIZ5100: public WebServer {
private:
	static byte retBuffer[6];

	byte macAddress[6];
	EthernetServer server;
	byte ethernetBuffer[MAX_URL_LEN + 16];			// MAX_URL_LEN + X is enough, since we only store the "GET <url> HTTP/1.x" request line
	unsigned int ethernetBufferSize;

public:
	WebServerWIZ5100 ();

	bool begin (byte *mac);

	bool begin (byte *mac, byte *ip, byte *gw, byte *mask);

	bool processPacket ();

	byte *getMAC ();

	byte *getIP ();

	byte *getNetmask ();

	byte *getGateway ();
};


class WebClient: public WebClientWIZ5100 {
};

class WebServer: public WebServerWIZ5100 {
};

#endif

#endif
