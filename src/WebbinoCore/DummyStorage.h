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

struct DummyContent: public Content {
	static const char content[];

	byte pos;

public:
	DummyContent (): pos (0) {
	}

	//~ SdContent (const char* filename): Content (filename) {
		//~ file = SD.open (filename);
	//~ }

	//~ SdContent (const SdContent& o): Content (*this) {
		//~ file = o.file;
	//~ }

	void reset () {
		pos = 0;
	}

	DummyContent& operator= (DummyContent o) {
		Content::operator= (o);		// This must be called explicitly!!!

		pos = o.pos;
		return *this;
	}

	//~ ~SdContent () {
		//~ if (file)
			//~ file.close ();
	//~ }

	const char* getFilename () const override {
		return "a._wwwf_";		// This hackish extension will have us return a content-type of x-www-form-urlencoded
	}

	boolean available () override {
		return pos < strlen (content);
	}

	byte getNextByte () override {
		return content[pos++];
	}
};


const char DummyContent::content[] = "$DUMMY$";


/******************************************************************************/


class DummyStorage: public Storage {
private:
	DummyContent content;

public:
	boolean begin () {
		return true;
	}

	boolean exists (const char* filename) override {
		(void) filename;
		return true;
	}

	Content& get (const char* filename) override {
		(void) filename;
		content.reset ();
		return content;
	}
};
