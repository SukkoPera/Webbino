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

#include "WebServer_ENC28J60.h"

#ifdef USE_ENC28J60

// Ethernet packet buffer
byte Ethernet::buffer[ETHERNET_BUFSIZE];


void WebClientENC28J60::initReply () {
	bfill = ether.tcpOffset ();
}

void WebClientENC28J60::sendReply () {
	ether.httpServerReply (bfill.position ());
}

size_t WebClientENC28J60::write (uint8_t c) {
	char tmp[2] = {c, '\0'};
	bfill.emit_p (PSTR ("$S"), tmp);

	return 1;
}

/****************************************************************************/

bool WebServerENC28J60::initChip (byte *mac) {
	Serial.println (F("Using EtherCard library"));

	return ether.begin (sizeof (Ethernet::buffer), mac, 10);
}

bool WebServerENC28J60::begin (byte *mac) {
	bool ret;

	if ((ret = WebServerBase::begin (mac)) && (ret = initChip (mac)))
		ret = ether.dhcpSetup ();

	return ret;
}

bool WebServerENC28J60::begin (byte *mac, byte *ip, byte *gw, byte *mask) {
	bool ret;
	
	if ((ret = WebServerBase::begin (mac, ip, gw, mask)) && (ret = initChip (mac)))
		ret = ether.staticSetup (ip, gw);		// Netmask not supported?!

	return ret;
}

bool WebServerENC28J60::processPacket () {
	word len = ether.packetReceive ();
	word pos = ether.packetLoop (len);

	if (pos) {
		// Got a packet
		HTTPRequestParser request = HTTPRequestParser ((char *) Ethernet::buffer + pos);

		Serial.print (F("Request for \""));
		Serial.print (request.url);
		Serial.println (F("\""));
		//Serial.println ((char *) Ethernet::buffer + pos);

		// Send the requested page
		WebClientENC28J60 webClient;
		sendPage (request, webClient);
	}
	
	return pos;
}

byte *WebServerENC28J60::getMAC () {
	return EtherCard::mymac;
}

byte *WebServerENC28J60::getIP () {
	return EtherCard::myip;
}

byte *WebServerENC28J60::getNetmask () {
	return EtherCard::mymask;
}

byte *WebServerENC28J60::getGateway () {
	return EtherCard::gwip;
}

#endif
