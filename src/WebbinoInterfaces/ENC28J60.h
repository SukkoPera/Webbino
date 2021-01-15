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

#ifndef _WEBSERVER28J60_H_
#define _WEBSERVER28J60_H_

#include "webbino_config.h"

#ifdef WEBBINO_USE_ENC28J60

#include <Arduino.h>
#include <EtherCard.h>
#include "WebbinoCore/WebClient.h"
#include "WebbinoCore/NetworkInterface.h"


class WebClientENC28J60: public WebClient {
public:
	void begin (char* req) override;

	size_t doWrite (const uint8_t *buf, size_t n) override;

	void sendReply () override;

private:
	BufferFiller bfill;
};


class NetworkInterfaceENC28J60: public NetworkInterface {
public:
	static const unsigned int ETHERNET_BUFSIZE = 850;

	static const byte DEFAULT_CS_PIN = 10;

	boolean begin (byte *mac, byte csPin = DEFAULT_CS_PIN);

	boolean begin (byte *mac, IPAddress ip, IPAddress mask, IPAddress gw, IPAddress dns, byte csPin = DEFAULT_CS_PIN);

	WebClient* processPacket () override;

	boolean usingDHCP () override;

	byte* getMAC () override;

	IPAddress getIP () override;

	IPAddress getNetmask () override;

	IPAddress getGateway () override;

	IPAddress getDns () override;

private:
	boolean dhcp;

	WebClientENC28J60 client;
};

#endif

#endif
