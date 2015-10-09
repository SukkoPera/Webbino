/***************************************************************************
 *   This file is part of SmartStrip.                                      *
 *                                                                         *
 *   Copyright (C) 2012-2015 by SukkoPera                                  *
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

#ifndef _WEBSERVER28J60_H_
#define _WEBSERVER28J60_H_

#include "webbino_common.h"

#ifdef USE_ENC28J60

#include <Arduino.h>
#include <../EtherCard/EtherCard.h>
#include "WebClientBase.h"
#include "WebServerBase.h"


class WebClientENC28J60: public WebClientBase {
private:
	BufferFiller bfill;

public:
	void initReply ();

	void sendReply ();

	size_t write (uint8_t c);
};


class WebServerENC28J60: public WebServerBase {
private:
	bool initChip (byte *mac);
	
public:
	bool begin (byte *mac);
	
	bool begin (byte *mac, byte *ip, byte *gw, byte *mask);
	
	bool processPacket ();
	
	byte *getMAC ();
	
	byte *getIP ();

	byte *getNetmask ();
	
	byte *getGateway ();
};

class WebClient: public WebClientENC28J60 {
};

class WebServer: public WebServerENC28J60 {
};

#endif

#endif
