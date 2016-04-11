/***************************************************************************
 *   This file is part of Webbino                                          *
 *                                                                         *
 *   Copyright (C) 2012-2016 by SukkoPera                                  *
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

#include "WebServer.h"
#include "WebClient.h"
#include "webbino_debug.h"

#define REDIRECT_ROOT_PAGE "/index.html"
#define REDIRECT_HEADER "HTTP/1.0 301 Moved Permanently\r\nLocation: " REDIRECT_ROOT_PAGE "\r\n\r\n"

#define OK_HEADER "HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n"


boolean WebServer::begin (NetworkInterface& _netint) {
	this -> netint = &_netint;

	return true;
}


Page *WebServer::get_page (const char *name) {
	Page *p;

	for (unsigned int i = 0; (p = reinterpret_cast<Page *> (pgm_read_word (&pages[i]))); i++) {
		if (strcmp_P (name, p -> getName ()) == 0)
			break;
	}

	return p;
}

#ifdef ENABLE_TAGS
char *WebServer::findSubstitutionTag (char *tag) {
	var_substitution *sub;

	for (byte i = 0; (sub = reinterpret_cast<var_substitution *> (pgm_read_word (&substitutions[i]))); i++) {
		if (strcmp_P (tag, sub -> getName ()) == 0)
			break;
	}

	return (sub ? (sub -> getFunction ()) (sub -> getData ()) : NULL);
}

char *WebServer::findSubstitutionTagGetParameter (HTTPRequestParser& request, const char *tag) {
	return request.get_get_parameter (tag);
}

// FIXME: Handle unterminated tags
void WebServer::sendPage (WebClient* client) {
	client -> initReply ();

	char *basename = client -> request.get_basename ();
	if (strlen (basename) == 0) {
		// Request for "/", redirect
		client -> print (F(REDIRECT_HEADER));
	} else {
		// Emit HTTP 200 OK
		client -> print (F(OK_HEADER));

		Page *page = get_page (basename);
		if (page) {
			// Call page function
			PageFunction func = page -> getFunction ();
			if (func)
				func (client -> request);

			// Read the page, perform tag substitutions and send it over
			PGM_P content = page -> getContent ();
			char c, tag[MAX_TAG_LEN];
			boolean inTag;
			int tagLen = 0;

			inTag = false;
			while ((c = pgm_read_byte (content++))) {
				if (inTag) {
					if (c == '#') {
						char *rep;

						// Tag complete
						inTag = false;

						DPRINT (F("Processing replacement tag: \""));
						DPRINT (tag);
						DPRINTLN (F("\""));

						if (strncmp_P (tag, PSTR ("GETP_"), 5) == 0)
							rep = findSubstitutionTagGetParameter (client -> request, tag + 5);
						else
							rep = findSubstitutionTag (tag);

						if (rep) {
							DPRINT (F("Replacement is: \""));
							DPRINT (rep);
							DPRINTLN (F("\""));

							client -> print (rep);
						} else {
							// Tag not found, emit it
							DPRINTLN (F("Tag not found"));

							client -> print (F("#"));
							client -> print (tag);
							client -> print (F("#"));
						}
					} else if (tagLen < MAX_TAG_LEN - 1) {
						tag[tagLen++] = c;
						tag[tagLen] = '\0';
					} else {
						// Tag too long, emit what we got so far
						// FIXME: This will detect a fake tag at the closing #
						char tmp[2] = {c, '\0'};		// FIXME
						client -> print (F("#"));
						client -> print (tag);
						client -> print (tmp);
						inTag = false;
					}
				} else {
					if (c == '#') {
						// Check if we have a tag to be replaced
						tag[0] = '\0';
						inTag = true;
						tagLen = 0;
					} else {
						char tmp[2] = {c, '\0'};		// FIXME
						client -> print (tmp);
					}
				}
			}
		} else {
			client -> print (F("<html><body><h3>No such page: \""));
			client -> print (basename);
			client -> print (F("\"</h3></body></html>"));
		}
	}

	client -> sendReply ();
}

#else

void WebServer::sendPage (WebClient* client) {
	client -> initReply ();

	char *basename = client -> request.get_basename ();
	if (strlen (basename) == 0) {
		// Request for "/", redirect
		client -> print (F(REDIRECT_HEADER));
	} else {
		// Emit HTTP 200 OK
		client -> print (F(OK_HEADER));

		Page *page = get_page (basename);
		if (page) {
			PageFunction func = page -> getFunction ();
			if (func)
				func (client -> request);

			PGM_P content = page -> getContent ();
			client -> print (reinterpret_cast<const __FlashStringHelper *> (content));
		} else {
			client -> print (F("<html><body><h3>No such page: &quot;"));
			client -> print (basename);
			client -> print (F("&quot;</h3></body></html>"));
		}
	}

	client -> sendReply ();
}

#endif

void WebServer::setPages (const Page * const _pages[]) {
	pages = _pages;
}

#ifdef ENABLE_TAGS
void WebServer::setSubstitutions (const var_substitution * const _substitutions[]) {
	substitutions = _substitutions;
}
#endif

boolean WebServer::loop () {
	WebClient *c = netint -> processPacket ();
	if (c != NULL) {
		// Got a client with a request, process it
		DPRINT (F("Request for \""));
		DPRINT (c -> request.url);
		DPRINTLN (F("\""));

		sendPage (c);
	}

	return c != NULL;
}
