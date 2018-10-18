/***************************************************************************
 *   This file is part of Webbino                                          *
 *                                                                         *
 *   Copyright (C) 2012-2018 by SukkoPera                                  *
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
		mystd::swap (*this, o);
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

public:
	boolean begin (int8_t pin) {
		boolean ret = false;

		DPRINT (F("Initializing SD card..."));
		if (!SD.begin (pin)) {
			DPRINTLN (F(" failed"));
		} else {
			DPRINTLN (F(" done"));
			ret = true;
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
