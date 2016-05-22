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

#ifndef _WEBSERVER5100_H_
#define _WEBSERVER5100_H_

#include "webbino_common.h"

#ifdef WEBBINO_USE_WIZ5100

#include <Ethernet.h>
#include "WebClient.h"
#include "WebServer.h"


class WebClientWIZ5100: public WebClient {
private:
	EthernetClient internalClient;

public:
	void init (EthernetClient& c, char* req);

	size_t write (uint8_t c) override;

	void sendReply () override;
};


class NetworkInterfaceWIZ5100: public NetworkInterface {
private:
	static byte retBuffer[6];

	boolean dhcp;
	byte macAddress[6];
	EthernetServer server;
	byte ethernetBuffer[MAX_URL_LEN + 16];			// MAX_URL_LEN + X is enough, since we only store the "GET <url> HTTP/1.x" request line
	unsigned int ethernetBufferSize;

	WebClientWIZ5100 webClient;

public:
	NetworkInterfaceWIZ5100 ();

	boolean begin (byte *mac);

	boolean begin (byte *mac, byte *ip, byte *gw, byte *mask);

	WebClient* processPacket () override;

	boolean usingDHCP () override;

	byte *getMAC () override;

	IPAddress getIP () override;

	IPAddress getNetmask () override;

	IPAddress getGateway () override;
};

#endif

#endif
