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

#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_

#include <webbino_config.h>
#include "NetworkInterface.h"
#include "HTTPRequest.h"
#include "Storage.h"
#include "MimeTypes.h"

// http://arduiniana.org/libraries/pstring/
#include <PString.h>

class WebClient;
class Content;

enum HttpStatusCode {
	// 2xx success
	HTTP_OK = 200,
	HTTP_OK_CREATED = 201,
	HTTP_OK_NO_CONTENT = 204,

	// 3xx redirection
	HTTP_MOVED_PERMANENTLY = 301,

	// 4xx client errors
	HTTP_BAD_REQUEST = 400,
	HTTP_UNAUTHORIZED = 401,
	HTTP_FORBIDDEN = 403,
	HTTP_NOT_FOUND = 404
};

#ifdef ENABLE_TAGS

typedef PString& (*TagEvaluateFn) (void* data);

struct ReplacementTag {
	PGM_P name;				// Max length: MAX_TAG_LEN
	TagEvaluateFn function;
	void *data;

	// Methods that (try to) hide the complexity of accessing PROGMEM data
	PGM_P getName () const {
		return reinterpret_cast<PGM_P> (pgm_read_ptr (&(this -> name)));
	}

	TagEvaluateFn getFunction () const {
		return reinterpret_cast<TagEvaluateFn> (pgm_read_ptr (&(this -> function)));
	}

	void *getData () const {
		return reinterpret_cast<void *> (const_cast<void *> (pgm_read_ptr (&(this -> data))));
	}
};

/* Macros that simplify things
 * Note that max length of tag name is MAX_TAG_LEN (24)
 */
#define REPTAG_STR(tag) #tag
#define REPTAG_STR_VAR(tag) _rtStr_ ## tag
//#define REPTAG_VAR(tag) _rtTag_ ## tag
//#define REPTAG_PTR(tag) &REPTAG_VAR(tag)

#define RepTagWithArg(var, tag, fn, arg) \
                const char REPTAG_STR_VAR(tag)[] PROGMEM = REPTAG_STR(tag); \
                const ReplacementTag var PROGMEM = {REPTAG_STR_VAR(tag), fn, reinterpret_cast<void *> (arg)};

#define RepTagNoArg(var, tag, fn) RepTagWithArg(var, tag, fn, NULL)

// http://stackoverflow.com/questions/11761703/overloading-macro-on-number-of-arguments
#define GET_MACRO(_1, _2, _3, _4, NAME, ...) NAME
#define EasyReplacementTag(...) GET_MACRO (__VA_ARGS__, RepTagWithArg, RepTagNoArg) (__VA_ARGS__)

typedef const ReplacementTag* const EasyReplacementTagArray;

#endif

/******************************************************************************/

#ifdef ENABLE_PAGE_FUNCTIONS
typedef HttpStatusCode (*PageFunction) (HttpRequest& request);


struct FileFuncAssociation {
	PGM_P path;
	PageFunction function;

	// Methods that (try to) hide the complexity of accessing PROGMEM data
	PGM_P getPath () const {
		return reinterpret_cast<PGM_P> (pgm_read_ptr (&(this -> path)));
	}

	PageFunction getFunction () const {
		return reinterpret_cast<PageFunction> (pgm_read_ptr (&(this -> function)));
	}
};

#define REPTAG_FFA_VAR(var) _ffa_ ## var
#define FileFuncAssoc(var, path, fn) \
                const char REPTAG_FFA_VAR(var)[] PROGMEM = path; \
const FileFuncAssociation var PROGMEM = {REPTAG_FFA_VAR(var), fn};

/* Use this for pages in flash, where a PROGMEM variable with the filename is
 * already available
 */
#define FlashFileFuncAssoc(var, path, fn) \
	const FileFuncAssociation var PROGMEM = {path, fn};

typedef const FileFuncAssociation* const FileFuncAssociationArray;
#endif

/******************************************************************************/

const byte MAX_STORAGES = 3;

class WebServer {
private:
	NetworkInterface* netint;

	Storage* storages[MAX_STORAGES];
	byte nStorage;

#ifdef ENABLE_TAGS
	const ReplacementTag* const * substitutions = nullptr;
#endif

#ifdef ENABLE_PAGE_FUNCTIONS
	FileFuncAssociationArray *associations = nullptr;
#endif

	void handleClient (WebClient& client);

	void sendContent (WebClient& client, Content& content, const MimeType& contType);

	const MimeType& getContentType (const char* filename);

#ifdef ENABLE_TAGS
	PString* findSubstitutionTag (const char* tag) const;

	char *findSubstitutionTagGetParameter (HttpRequest& request, const char* tag);
#endif

public:
	boolean begin (NetworkInterface& _netint);

	boolean addStorage (Storage& storage);

#ifdef ENABLE_TAGS
	void enableReplacementTags (const ReplacementTag* const _substitutions[]);
#endif

#ifdef ENABLE_PAGE_FUNCTIONS
	void associateFunctions (FileFuncAssociationArray* _associations);
#endif

#ifdef ENABLE_HTTPAUTH
	typedef boolean (*AuthorizeFunc) (const char *user, const char *passwd);

	void enableAuth (const char *realm, AuthorizeFunc authFunc);
#endif

	boolean loop ();

private:
#ifdef ENABLE_HTTPAUTH
	const char *realm;

	AuthorizeFunc authorizeFunc;
#endif
};

#endif
