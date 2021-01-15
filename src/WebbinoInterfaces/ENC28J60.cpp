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

#include "ENC28J60.h"

#ifdef WEBBINO_USE_ENC28J60

#include "webbino_debug.h"

// Ethernet packet buffer
byte Ethernet::buffer[NetworkInterfaceENC28J60::ETHERNET_BUFSIZE];


void WebClientENC28J60::begin (char* req) {
	WebClient::begin (req);
	bfill = ether.tcpOffset ();
}

size_t WebClientENC28J60::doWrite (const uint8_t *buf, size_t n) {
	bfill.emit_raw (reinterpret_cast<const char*> (buf), n);
	return n;
}

void WebClientENC28J60::sendReply () {
	WebClient::sendReply ();
	ether.httpServerReply (bfill.position ());
}

/****************************************************************************/


boolean NetworkInterfaceENC28J60::begin (byte *mac, byte csPin) {
	boolean ret;

	DPRINTLN (F("Using EtherCard library"));

	if ((ret = ether.begin (sizeof (Ethernet::buffer), mac, csPin))) {
		ret = ether.dhcpSetup ();
		dhcp = true;
	}

	return ret;
}

boolean NetworkInterfaceENC28J60::begin (byte *mac, IPAddress ip, IPAddress mask, IPAddress gw, IPAddress dns, byte csPin) {
	boolean ret;

	DPRINTLN (F("Using EtherCard library"));

	if ((ret = ether.begin (sizeof (Ethernet::buffer), mac, csPin))) {
		ret = ether.staticSetup (&ip[0], &gw[0], &dns[0], &mask[0]);
		dhcp = false;
	}

	return ret;
}

WebClient *NetworkInterfaceENC28J60::processPacket () {
	WebClient* ret = NULL;

	word len = ether.packetReceive ();
	word pos = ether.packetLoop (len);

	if (pos) {
		// Got a packet
		client.begin (reinterpret_cast<char *> (Ethernet::buffer + pos));
		//DPRINTLN ((char *) Ethernet::buffer + pos);
		ret = &client;
	}

	return ret;
}

boolean NetworkInterfaceENC28J60::usingDHCP () {
	return dhcp;
}

byte *NetworkInterfaceENC28J60::getMAC () {
	return EtherCard::mymac;
}

// These need to be tested!
IPAddress NetworkInterfaceENC28J60::getIP () {
	IPAddress ip = EtherCard::myip;
	return ip;
}

IPAddress NetworkInterfaceENC28J60::getNetmask () {
	IPAddress ip = EtherCard::netmask;
	return ip;
}

IPAddress NetworkInterfaceENC28J60::getGateway () {
	IPAddress ip = EtherCard::gwip;
	return ip;
}

IPAddress NetworkInterfaceENC28J60::getDns () {
	IPAddress ip = EtherCard::dnsip;
	return ip;
}

#endif
