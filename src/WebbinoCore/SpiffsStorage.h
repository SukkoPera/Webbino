/***************************************************************************
 *   This file is part of Webbino                                          *
 *                                                                         *
 *   Copyright (C) 2012-2019 by SukkoPera                                  *
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

#include <Arduino.h>
#include "Content.h"
#include "webbino_common.h"

#ifdef WEBBINO_ENABLE_SPIFFS

#ifndef WEBBINO_USE_ESP8266_STANDALONE
#error "SPIFFS can only be enabled on ESP8266 standalone"
#endif

#include <FS.h>

struct SpiffsContent: public Content {
private:
	File file;

public:
	SpiffsContent (const char* filename): Content (filename) {
		file = SPIFFS.open (filename, "r");
	}
	
	SpiffsContent (const SpiffsContent& o) {
		file = o.file;
	}
	
	SpiffsContent& operator= (SpiffsContent o) {
		swap (*this, o);
		return *this;
	}

	~SpiffsContent () {
		file.close ();
	}
	
	boolean available () override {
		return file.available ();
	}

	byte getNextByte () override {
		return file.read ();
	}
};

/******************************************************************************/


class SpiffsStorage: public Storage {
private:
	SpiffsContent content;
	
public:
	void begin () {
		SPIFFS.begin ();

#ifndef WEBBINO_NDEBUG
		DPRINTLN (F("Pages available in SPIFFS:"));
		Dir dir = SPIFFS.openDir ("/");
		for (byte i = 0; dir.next (); i++) {
			DPRINT (i);
			DPRINT (F(". "));
			DPRINTLN (dir.fileName());
		}
#endif
	}
	
	boolean exists (const char* filename) override {
		return SPIFFS.exists (filename);
	}
	
	Content& get (const char* filename) override {
		content = SpiffsContent (filename);
	}
};

#endif
