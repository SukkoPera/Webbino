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

#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_

#include "webbino_config.h"
#include "NetworkInterface.h"
#include "HTTPRequestParser.h"

// http://arduiniana.org/libraries/pstring/
#include <PString.h>

#if defined (WEBBINO_ENABLE_SD)
#include <SD.h>
#elif defined (WEBBINO_ENABLE_SDFAT)
#include <SdFat.h>
#endif


class WebClient;


/******************************************************************************/

typedef void (*PageFunction) (HTTPRequestParser& request);

struct Page {
	PGM_P name;
	PGM_P content;
	PageFunction function;

	// Methods that (try to) hide the complexity of accessing PROGMEM data
	PGM_P getName () {
		return reinterpret_cast<PGM_P> (pgm_read_word (&(this -> name)));
	}

	PageFunction getFunction () {
		return reinterpret_cast<PageFunction> (pgm_read_word (&(this -> function)));
	}

	PGM_P getContent () {
		return reinterpret_cast<PGM_P> (pgm_read_word (&(this -> content)));
	}
};

/******************************************************************************/


class PageContent {
public:
	virtual char getNextByte () = 0;
};

/******************************************************************************/


class FlashContent: public PageContent {
private:
	const Page& page;
	PGM_P next;

public:
	FlashContent (Page* p);

	char getNextByte () override;
};

/******************************************************************************/


#if defined (WEBBINO_ENABLE_SD) || defined (WEBBINO_ENABLE_SDFAT)

struct SDContent: public PageContent {
private:
	File file;

public:
	SDContent (const char* filename);
	~SDContent ();

	char getNextByte () override;
};

#endif

/******************************************************************************/


#ifdef ENABLE_TAGS

typedef PString& (*var_evaluate_func) (void* data);

struct var_substitution {
	PGM_P name;				// Max length: MAX_TAG_LEN
	var_evaluate_func function;
	void *data;

	// Methods that (try to) hide the complexity of accessing PROGMEM data
	PGM_P getName () {
		return reinterpret_cast<PGM_P> (pgm_read_word (&(this -> name)));
	}

	var_evaluate_func getFunction () {
		return reinterpret_cast<var_evaluate_func> (pgm_read_word (&(this -> function)));
	}

	void *getData () {
		return reinterpret_cast<void *> (pgm_read_word (&(this -> data)));
	}
};

#endif

/******************************************************************************/


class WebServer {
private:
	NetworkInterface* netint;

	const Page* const *pages;

#ifdef ENABLE_TAGS
	const var_substitution* const * substitutions;
#endif

	void sendPage (WebClient* client);

	void sendContent (WebClient* client, PageContent* content);

	Page *get_page (const char* name);

#ifdef ENABLE_TAGS
	PString* findSubstitutionTag (const char* tag);

	char *findSubstitutionTagGetParameter (HTTPRequestParser& request, const char* tag);
#endif

public:
	boolean begin (NetworkInterface& _netint, const Page* const _pages[] = NULL
#ifdef ENABLE_TAGS
		, const var_substitution* const _substitutions[] = NULL
#endif
	);

#if defined (WEBBINO_ENABLE_SD) || defined (WEBBINO_ENABLE_SDFAT)
	boolean enableSD (byte pin);
#endif

	boolean loop ();
};

#endif
