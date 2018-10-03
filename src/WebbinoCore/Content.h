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

#ifndef CONTENT_H_INCLUDED
#define CONTENT_H_INCLUDED

#include <webbino_config.h>
#include <webbino_debug.h>

/* Note that filename is NOT copied, so it must be kept pointing to a valid
 * string during the usage of the object.
 */
class PageContent {
protected:
	const char* filename;
	
	PageContent (const char* _filename) {
		filename = _filename;
	}
	
public:
	virtual const char* getFilename () const {
		return filename;
	}
	
	virtual boolean available () = 0;
	virtual char getNextByte () = 0;
};

/******************************************************************************/


class FlashContent: public PageContent {
private:
	const Page& page;
	PGM_P next;
	unsigned int offset;

public:
	FlashContent (const Page* p): PageContent (p -> getName ()), page (*p),
		next (p -> getContent ()), offset (0) {
	}
	
	boolean available () override {
		return offset < page.getLength ();
	}

	char getNextByte () override {
		++offset;
		return pgm_read_byte (next++);
	}
};

/******************************************************************************/


#if defined (WEBBINO_ENABLE_SD) || defined (WEBBINO_ENABLE_SDFAT)

#if defined (WEBBINO_ENABLE_SD)
#include <SD.h>
#elif defined (WEBBINO_ENABLE_SDFAT)
#include <SdFat.h>

static SdFat SD;
#endif

struct SDContent: public PageContent {
private:
	File file;

public:
	SDContent (const char* filename): PageContent (filename) {
		file = SD.open (filename);
	}

	~SDContent () {
		file.close ();
	}
	
	boolean available () override {
		return file.available ();
	}

	char getNextByte () override {
		return file.read ();
	}
};

#endif

/******************************************************************************/


#ifdef WEBBINO_ENABLE_SPIFFS

#include <FS.h>

struct SPIFFSContent: public PageContent {
private:
	File file;

public:
	SPIFFSContent (const char* filename): PageContent (filename) {
		file = SPIFFS.open (filename, "r");
	}

	~SPIFFSContent () {
		file.close ();
	}
	
	boolean available () override {
		return file.available ();
	}

	char getNextByte () override {
		return file.read ();
	}
};

#endif

#endif
