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

#include <Arduino.h>
#include "Content.h"
#include "webbino_common.h"

#ifdef WEBBINO_ENABLE_LITTLEFS

#include <FS.h>

#ifdef ARDUINO_ARCH_ESP32
#include <LITTLEFS.h>	// https://github.com/lorol/LITTLEFS
#define LittleFS LITTLEFS
#elif defined (WEBBINO_USE_ESP8266_STANDALONE)
#include <LittleFS.h>
#else
#error "LittleFS can only be enabled on ESP8266 or ESP32 standalone"
#endif

struct LittleFSContent: public Content {
private:
	File file;

public:
	LittleFSContent () {
	}

	LittleFSContent (const char* filename): Content (filename) {
		file = LittleFS.open (filename, "r");
	}

	LittleFSContent (const LittleFSContent& o): Content (*this) {
		file = o.file;
	}

	LittleFSContent& operator= (LittleFSContent o) {
		if (file)
			file.close ();

		Content::operator= (o);		// This must be called explicitly!!!

		file = LittleFS.open (filename, "r");
		return *this;
	}

	~LittleFSContent () {
		if (file)
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

class LittleFSStorage: public Storage {
private:
	LittleFSContent content;

public:
	void begin () {
		LittleFS.begin ();

#ifndef WEBBINO_NDEBUG
		DPRINTLN (F("Pages available in LittleFS:"));
		Dir dir = LittleFS.openDir ("/");
		for (byte i = 0; dir.next (); i++) {
			DPRINT (i);
			DPRINT (F(". "));
			DPRINTLN (dir.fileName());
		}
#endif
	}

	boolean exists (const char* filename) override {
		return LittleFS.exists (filename);
	}

	Content& get (const char* filename) override {
		content = LittleFSContent (filename);

		return content;
	}
};

#endif
