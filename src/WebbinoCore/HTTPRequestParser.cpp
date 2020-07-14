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
#include "HTTPRequestParser.h"

#ifdef ENABLE_HTTPAUTH
#include <Base64.h>
#endif


static const char METHOD_GET_STR[] PROGMEM = "GET";
static const char METHOD_PUT_STR[] PROGMEM = "PUT";
static const char METHOD_POST_STR[] PROGMEM = "POST";
static const char METHOD_DELETE_STR[] PROGMEM = "DELETE";

const byte N_METHODS = 4;

// These must be in the same order as HttpMethod
static const char * const HTTP_METHOD_STRINGS[N_METHODS] = {
	METHOD_GET_STR,
	METHOD_PUT_STR,
	METHOD_POST_STR,
	METHOD_DELETE_STR
};

HTTPRequestParser::HTTPRequestParser () {
	uri[0] = '\0';
}

/* Request syntax:
 * METHOD URI HTTP/1.x
 */
boolean HTTPRequestParser::parse (const char *request) {
	boolean ret = false;

#ifdef VERBOSE_REQUEST_PARSER
	DPRINT (F("Parsing request: \""));
	DPRINT (request);
	DPRINTLN (F("\""));
#endif

	// Clear current data
	uri[0] = '\0';
	method = METHOD_UNKNOWN;

	// Look for the first space in the request, which separates method and URI
	const char* uriStart = strchr (request, ' ');
	if (uriStart) {
		// Find out what method was requested
		const ptrdiff_t methodLen = uriStart - request;

		for (byte i = 0; i < N_METHODS; ++i) {
			PGM_P candidate = F_TO_PSTR (HTTP_METHOD_STRINGS[i]);
			if (strlen_P (candidate) == methodLen && strncasecmp_P (request, candidate, methodLen) == 0) {
				DPRINT (F("Method = "));
				DPRINTLN (i);
				method = static_cast<HttpMethod> (i);
				break;
			}
		}

		/* OK, we got the method, now find the first non-space character
		 * after it and look for the first space after that, which marks the
		 * end of the URI
		 */
		while (isSpace (*uriStart)) {
			++uriStart;		// I find it odd I can increment a const variable...
		}
		const char *uriEnd = strchr (uriStart, ' ');
		if (uriEnd) {
			// Found another space
			const ptrdiff_t uriLen = uriEnd - uriStart;
			// The strlcpy() function copies up to size - 1 characters
			strlcpy (uri, uriStart, uriLen + 1 < MAX_URL_LEN ? uriLen + 1: MAX_URL_LEN);
		} else {
			// No other space found, assume someone "forgot" the HTTP version...
			strlcpy (uri, uriStart, MAX_URL_LEN);
		}

#ifdef VERBOSE_REQUEST_PARSER
		DPRINT (F("Extracted URL: \""));
		DPRINT (uri);
		DPRINTLN (F("\""));
#endif

		// Let's ignore the HTTP version, for the moment :)

#ifdef ENABLE_HTTPAUTH
		// Look for the "Authorize" header
		username = NULL;
		password = NULL;
		char *p = strstr_P (request, PSTR ("Authorization: Basic "));
		if (p) {
			char *authStart = p + 21;
			char *authEnd = strchr (authStart, '\r');
			if (authEnd) {
				const ptrdiff_t uriLen = authEnd - authStart;
				DPRINTLN (F("Found auth header"));
				int decodedLength = Base64.decodedLength (authStart, uriLen);
				if (decodedLength <= MAX_USERPASS_LEN) {
					Base64.decode (userpassBuf, authStart, uriLen);
					DPRINT (F("Decoded auth data is: "));
					DPRINTLN (userpassBuf);

					char *sep = strchr (userpassBuf, ':');
					if (sep) {
						*sep = '\0';
						username = userpassBuf;
						password = sep + 1;
					}
				} else {
					DPRINTLN (F("Decoded auth data would overflow buffer"));
				}
			}
		}
#endif

		ret = true;
	} else {
		// No space in request, can't do much
		DPRINTLN (F("Cannot extract URL"));
	}

	return ret;
}

char *HTTPRequestParser::get_basename () {
	buffer[0] = '\0';
	char *qMark = strchr (uri, '?');
	if (qMark) {
		strlcpy (buffer, uri, qMark - uri + 1 < BUF_LEN ? qMark - uri + 1 : BUF_LEN);
	} else {
		strlcpy (buffer, uri, BUF_LEN);
	}

#ifdef VERBOSE_REQUEST_PARSER
	DPRINT (F("Extracted basename: \""));
	DPRINT (buffer);
	DPRINTLN (F("\""));
#endif

	return buffer;
}

char *HTTPRequestParser::get_parameter (const char param[]) {
	char *start;
	boolean found = false;

#ifdef VERBOSE_REQUEST_PARSER
	/* Print this now, because if we got called by
	 * get_parameter(WebbinoFStr), param is actually stored in
	 * buffer and will be overwritten.
	 */
	DPRINT (F("Extracting GET parameter: \""));
	DPRINT (param);
	DPRINT (F("\": \""));
#endif

	for (start = strchr (uri, '?'); !found && start; start = strchr (start + 1, '&')) {
		char *end = strchr (start, '=');
		if (end && (end - start - 1) == (int) strlen (param) && strncmp (start + 1, param, end - start - 1) == 0) {
			// Found!
			char *x = strchr (end + 1, '&');
			if (x) {
				strlcpy (buffer, end + 1, x - end < BUF_LEN ? x - end : BUF_LEN);
			} else {
				strlcpy (buffer, end + 1, BUF_LEN);
			}
			found = true;
		}
	}

	if (!found)
		buffer[0] = '\0';

#ifdef VERBOSE_REQUEST_PARSER
	DPRINT (buffer);
	DPRINTLN (F("\""));
#endif

	return buffer;
}

#ifdef ENABLE_FLASH_STRINGS
char *HTTPRequestParser::get_parameter (WebbinoFStr param) {
	strncpy_P (buffer, F_TO_PSTR (param), BUF_LEN);
	return get_parameter (buffer);
}
#endif

boolean HTTPRequestParser::parametricMatch (const char *str, const char *expr, MatchResult& result) {
	result.nMatches = 0;

	boolean matchOk = true;
	byte i = 0, j = 0;
	while (matchOk && i < strlen (expr)) {
		if (expr[i] != str[j]) {
			if (expr[i] == '*') {
				// Capture
				if (expr[i + 1] == '/') {		// Always safe
					// Pattern goes on, there must be a slash in str
					char *slash = strchr (str + j, '/');
					if (slash) {
						// Found, save capture and keep on matching
						ptrdiff_t len = slash - (str + j);
						result.matchPositions[result.nMatches] = j;
						result.matchLengths[result.nMatches] = (int) len;
						++(result.nMatches);
						++i;
						j += len;
					} else {
						matchOk = false;
					}
				} else {
					// Capture char is last char or patter, there must be no slash in str
					char *slash = strchr (str + j, '/');
					if (!slash) {
						// OK, save capture
						result.matchPositions[result.nMatches] = j;
						result.matchLengths[result.nMatches] = strlen (str) - j;
						++(result.nMatches);
						++i;
					} else {
						// String goes on, no match
						matchOk = false;
					}
				}
			} else {
				matchOk = false;
			}
		} else {
			++i, ++j;
		}
	}

	return matchOk;
}

boolean HTTPRequestParser::matchAssociation (const char *assocPath) {
	return HTTPRequestParser::parametricMatch (uri, assocPath, matchResult);
}
