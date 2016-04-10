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

#ifndef _WEBSERVER8266_H_
#define _WEBSERVER8266_H_

#include "webbino_common.h"

#ifdef WEBBINO_USE_ESP8266

#include <WiFiEsp.h>
#include "WebClient.h"
#include "WebServer.h"


#define CLIENT_BUFSIZE 256

class WebClientESP8266: public WebClient {
private:
	WiFiEspClient internalClient;
	byte buf[CLIENT_BUFSIZE];
	int avail;

	void flushBuffer ();

public:
	void init (WiFiEspClient& c, char* req);

	size_t write (uint8_t c) override;

	void sendReply () override;
};


class NetworkInterfaceESP8266: public NetworkInterface {
private:
	static byte retBuffer[6];

	WiFiEspServer server;
	byte ethernetBuffer[MAX_URL_LEN + 16];			// MAX_URL_LEN + X is enough, since we only store the "GET <url> HTTP/1.x" request line
	unsigned int ethernetBufferSize;

	WebClientESP8266 webClient;

public:
	NetworkInterfaceESP8266 ();

	boolean begin (Stream& _serial, const char *_ssid, const char *_password);

	WebClient* processPacket () override;

	boolean usingDHCP () override;

	byte *getMAC () override;

	byte *getIP () override;

	byte *getNetmask () override;

	byte *getGateway () override;
};

#endif

#endif
