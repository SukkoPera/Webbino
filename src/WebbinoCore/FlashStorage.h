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

typedef const byte* PGM_BYTES_P;

struct Page {
	PGM_P name;
	PGM_BYTES_P content;
	unsigned int length;

	// Methods that (try to) hide the complexity of accessing PROGMEM data
	PGM_P getName () const {
		return reinterpret_cast<PGM_P> (pgm_read_ptr (&(this -> name)));
	}

	PGM_BYTES_P getContent () const {
		return reinterpret_cast<PGM_BYTES_P> (pgm_read_ptr (&(this -> content)));
	}

	unsigned int getLength () const {
		// I'm not a fan of this, but well... (See #5)
#if __SIZEOF_INT__ == 2
		return pgm_read_word (&(this -> length));
#elif __SIZEOF_INT__ == 4
		return pgm_read_dword (&(this -> length));
#else
		#error "Mmmmh... Compiling on a weird architecture?"
#endif
	}
};

/******************************************************************************/



class FlashContent: public Content {
private:
	const Page* page;
	PGM_BYTES_P next;
	unsigned int offset;
	char filenameRam[MAX_FLASH_FNLEN];

public:
	FlashContent (): page (nullptr), next (nullptr), offset (-1) {
		filenameRam[0] = '\0';
	}

	FlashContent (const Page* p): Content (nullptr), page (p),
		next (p -> getContent ()), offset (0) {
		strncpy_P (filenameRam, p -> getName (), MAX_FLASH_FNLEN - 1);
		filenameRam[MAX_FLASH_FNLEN - 1] = '\0';
	}

	FlashContent (const FlashContent& o): Content (*this), page (o.page),
		next (o.next), offset (o.offset) {

		strncpy (filenameRam, o.filenameRam, MAX_FLASH_FNLEN - 1);
		filenameRam[MAX_FLASH_FNLEN - 1] = '\0';
	}

	FlashContent& operator= (const FlashContent& o) {
		Content::operator= (o);		// This must be called explicitly!!!

		page = o.page;
		next = o.next;
		offset = o.offset;

		strncpy (filenameRam, o.filenameRam, MAX_FLASH_FNLEN - 1);
		filenameRam[MAX_FLASH_FNLEN - 1] = '\0';

		return *this;
	}

	/* This needs to be overridden because page -> getFilename () is in flash
	 * memory :(.
	 */
	const char* getFilename () const override {
		return filenameRam;
	}

	//~ FlashContent& operator= (FlashContent o) {
		//~ swap (*this, o);
		//~ return *this;
	//~ }

	//~ friend void swap (FlashContent& first, FlashContent& second) {	// nothrow
        //~ // enable ADL (not necessary in our case, but good practice)
        //~ using mystd::swap;

        //~ // by swapping the members of two objects,
        //~ // the two objects are effectively swapped
        //~ swap (first.page, second.page);
        //~ swap (first.next, second.next);
        //~ swap (first.offset, second.offset);
    //~ }


	boolean available () override {
		return offset < page -> getLength ();
	}

	byte getNextByte () override {
		++offset;
		return pgm_read_byte (next++);
	}
};

/******************************************************************************/


class FlashStorage: public Storage {
private:
	const Page* const *pages = nullptr;
	FlashContent content;

public:
	void begin (const Page* const _pages[]) {
		pages = _pages;

#ifndef WEBBINO_NDEBUG
		DPRINTLN (F("Pages available in flash memory:"));
		const Page *p = nullptr;
		for (byte i = 0; pages && (p = reinterpret_cast<const Page *> (pgm_read_ptr (&pages[i]))); i++) {
			DPRINT (i);
			DPRINT (F(". "));
			DPRINTLN (PSTR_TO_F (p -> getName ()));
		}
#endif
	}

	boolean exists (const char* filename) override {
		const Page *p = nullptr;

		// For some reason, if we make i a byte here, the code uses 8 more bytes, so don't!
		for (unsigned int i = 0; pages && (p = reinterpret_cast<const Page*> (pgm_read_ptr (&pages[i]))); ++i) {
			if (strcmp_P (filename, p -> getName ()) == 0)
				break;
		}

		return p ? true : false;
	}

	Content& get (const char* filename) override {
		content = FlashContent ();

		// For some reason, if we make i a byte here, the code uses 8 more bytes, so don't!
		const Page *p = nullptr;
		for (unsigned int i = 0; pages && (p = reinterpret_cast<const Page*> (pgm_read_ptr (&pages[i]))); ++i) {
			if (strcmp_P (filename, p -> getName ()) == 0) {
				content = FlashContent (p);
				break;
			}
		}

		return content;
	}

	void release (Content& content) override {
		content = FlashContent ();
	}
};
