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

#if defined (WEBBINO_ENABLE_SD) || defined (WEBBINO_ENABLE_SDFAT)

#if defined (WEBBINO_ENABLE_SD)
#include <SD.h>
#elif defined (WEBBINO_ENABLE_SDFAT)
#include <SdFat.h>

static SdFat SD;
#endif

struct SdContent: public Content {
private:
	File file;

public:
	SdContent () {
	}

	SdContent (const char* filename): Content (filename) {
		file = SD.open (filename);
	}

	SdContent (const SdContent& o): Content (*this) {
		file = o.file;
	}

	SdContent& operator= (SdContent o) {
		if (file)
			file.close ();

		Content::operator= (o);		// This must be called explicitly!!!

		//~ mystd::swap (*this, o);
		file = o.file;
		return *this;
	}

	~SdContent () {
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


class SdStorage: public Storage {
private:
	SdContent content;

	void printDirectory (File dir, int numTabs = 0) {
		while (true) {
			File entry =  dir.openNextFile();
			if (!entry) {
				// no more files
				break;
			}

			for (uint8_t i = 0; i < numTabs; i++) {
				DPRINT ('\t');
			}
			DPRINT (entry.name());
			if (entry.isDirectory()) {
				DPRINTLN ("/");
				printDirectory (entry, numTabs + 1);
			} else {
				// files have sizes, directories do not
				DPRINT("\t\t");
				DPRINTLN (entry.size (), DEC);
			}
			entry.close ();
		}
	}

public:
	boolean begin (int8_t pin) {
		boolean ret = false;

		DPRINT (F("Initializing SD card..."));
		if (!SD.begin (pin)) {
			DPRINTLN (F(" failed"));
		} else {
			DPRINTLN (F(" done"));
			ret = true;

#ifndef WEBBINO_NDEBUG
		DPRINTLN (F("Pages available on SD card:"));
		File root = SD.open("/");
		printDirectory (root);
#endif
		}

		return ret;
	}

	boolean exists (const char* filename) override {
		return SD.exists (filename);
	}

	Content& get (const char* filename) override {
		content = SdContent (filename);

		return content;
	}
};

#endif
