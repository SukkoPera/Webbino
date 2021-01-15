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

#ifndef _WEBSERVER5x00_H_
#define _WEBSERVER5x00_H_

#include <webbino_config.h>

#if defined (WEBBINO_USE_WIZ5100) || defined (WEBBINO_USE_WIZ5500) || \
    defined (WEBBINO_USE_ENC28J60_UIP) || defined (WEBBINO_USE_TEENSY41_NATIVE)

#if defined (WEBBINO_USE_WIZ5100)
	#include <Ethernet.h>
#elif defined (WEBBINO_USE_WIZ5500)
	#include <Ethernet2.h>
#elif defined (WEBBINO_USE_ENC28J60_UIP)
	#include <UIPEthernet.h>
#elif defined (WEBBINO_USE_TEENSY41_NATIVE)
	#ifndef ARDUINO_TEENSY41
		#error "You can *only* use WEBBINO_USE_TEENSY41_NATIVE with Teensy 4.1"
	#endif

	#include <NativeEthernet.h>
#endif

#include <WebbinoCore/WebClient.h>
#include <WebbinoCore/WebServer.h>


class WebClientWIZ5x00: public WebClient {
private:
	EthernetClient internalClient;

public:
	void begin (EthernetClient& c, char* req);

	size_t doWrite (const uint8_t *buf, size_t n) override;

	void sendReply () override;
};


class NetworkInterfaceWIZ5x00: public NetworkInterface {
private:
	static byte retBuffer[6];

	boolean dhcp;
	byte macAddress[6];
	EthernetServer server;
	byte ethernetBuffer[MAX_URL_LEN + 16];			// MAX_URL_LEN + X is enough, since we only store the "GET <url> HTTP/1.x" request line
	unsigned int ethernetBufferSize;

#ifdef CLIENT_TIMEOUT
	unsigned long lastPacketReceived = 0;
#endif

	WebClientWIZ5x00 webClient;

public:
	NetworkInterfaceWIZ5x00 ();

#if defined (WEBBINO_USE_TEENSY41_NATIVE)
	boolean begin (byte *mac);
#else
	boolean begin (byte *mac, const byte ssPin);
#endif

#if defined (WEBBINO_USE_TEENSY41_NATIVE)
	boolean begin (byte *mac, IPAddress ip, IPAddress mask, IPAddress gw, IPAddress dns);
#else
	boolean begin (byte *mac, IPAddress ip, IPAddress mask, IPAddress gw, IPAddress dns, const byte ssPin);
#endif

	WebClient* processPacket () override;

	boolean usingDHCP () override;

	byte *getMAC () override;

	IPAddress getIP () override;

	IPAddress getNetmask () override;

	IPAddress getGateway () override;

	IPAddress getDns () override;
};

#endif

#endif
