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
#include "HTTPRequest.h"

#ifdef ENABLE_HTTPAUTH
#include <Base64.h>
#endif

#ifdef ENABLE_ALL_METHODS
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
#endif

HttpRequest::HttpRequest () {
	uri[0] = '\0';
}

/* Request syntax:
 * METHOD URI HTTP/1.x
 *
 * This stores a pointer to the request, which must thus be kept valid.
 */
boolean HttpRequest::parse (const char *_request) {
	boolean ret = false;

	// Clear current data
	uri[0] = '\0';

#ifdef ENABLE_ALL_METHODS
	method = METHOD_UNKNOWN;
#endif

	// Save a pointer to the request
	request = _request;

#ifdef VERBOSE_REQUEST_PARSER
	DPRINT (F("Parsing request: \""));
	DPRINT (request);
	DPRINTLN (F("\""));
#endif

	// Look for the first space in the request, which separates method and URI
	const char* uriStart = strchr (request, ' ');
	if (uriStart) {
#ifdef ENABLE_ALL_METHODS
		// Find out what method was requested
		const size_t methodLen = uriStart - request;

		for (byte i = 0; i < N_METHODS; ++i) {
			PGM_P candidate = F_TO_PSTR (HTTP_METHOD_STRINGS[i]);
			if (strlen_P (candidate) == methodLen && strncasecmp_P (request, candidate, methodLen) == 0) {
				DPRINT (F("Method = "));
				DPRINTLN (i);
				method = static_cast<HttpMethod> (i);
				break;
			}
		}
#endif

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
			const size_t uriLen = uriEnd - uriStart;
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

char *HttpRequest::get_basename () {
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

#ifdef ENABLE_REST

#define isHexDigit(c) (((c) >= '0' && (c) <= '9') || (tolower (c) >= 'a' && tolower (c) <= 'f'))
#define hex2int(c) ((c) <= '9' ? ((c) - '0') : ((c) - 'a' + 10))

/* Copy up to len characters from src to dst decoding space-encoded characters
 * and replacing '+' with spaces
 */
char *HttpRequest::cpyndec (char *dst, const char *src, const size_t len) {
	size_t i = 0, j = 0;

	while (src[i] != '\0' && j < len - 1) {
		if (src[i] == '+') {
			// That's the encoding of a space
			dst[j++] = ' ';
			++i;
		} else if (src[i] == '%') {
			// Decode
			if (i + 2 < len && isHexDigit (src[i + 1]) && isHexDigit (src[i + 2])) {
				int d = hex2int (src[i + 1]);
				int u = hex2int (src[i + 2]);
				int n = d * 16 + u;
				dst[j++] = n;

				i += 3;
			} else {
				// Fallback
				dst[j++] = src[i++];
			}
		} else {
			// Copy as-is
			dst[j++] = src[i++];
		}
	}
	dst[j] = '\0';

	return dst;
}

#else

char *HttpRequest::cpyndec (char *dst, const char *src, const size_t len) {
	strlcpy (dst, src, len);

	return dst;
}

#endif

// Extract the value of a parameter from an x-www-form-urlencoded string
char *HttpRequest::getFormParameter (const char str[], const char param[]) {
	const char *start;
	bool found = false;

#ifdef VERBOSE_REQUEST_PARSER
	/* Print this now, because if we got called by
	 * get_parameter(WebbinoFStr), param is actually stored in
	 * buffer and will be overwritten.
	 */
	DPRINT (F("Extracting form parameter: \""));
	DPRINT (param);
	DPRINT (F("\": \""));
#endif

	for (start = str; !found && start; start = strchr (start + 1, '&')) {
		if (start[0] == '&')
			++start;
		char *end = strchr (start + 1, '=');
		size_t nameLen = end - start;
		if (end && nameLen == strlen (param) && strncmp (start, param, nameLen) == 0) {
			// Found!
			size_t valueLen = BUF_LEN;
			char *x = strchr (end + 1, '&');
			if (x) {
				valueLen = min (x - end - 1, BUF_LEN);
			}
			cpyndec (buffer, end + 1, valueLen + 1);	// FIXME: This +1 is dangerous
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

char *HttpRequest::get_parameter (const char param[]) {
	char *paramStart = strchr (uri, '?');
	if (paramStart != NULL) {
		getFormParameter (paramStart + 1, param);
	} else {
		buffer[0] = '\0';
	}

	return buffer;
}

char *HttpRequest::get_parameter (WebbinoFStr param) {
	char *paramStart = strchr (uri, '?');
	if (paramStart != NULL) {
		strncpy_P (buffer, F_TO_PSTR (param), BUF_LEN);
		getFormParameter (paramStart + 1, buffer);
	} else {
		buffer[0] = '\0';
	}

	return buffer;
}

#ifdef ENABLE_REST
boolean HttpRequest::parametricMatch (const char *str, WebbinoFStr expr, MatchResult& result) {
	result.nMatches = 0;

	boolean matchOk = true;
	byte i = 0, j = 0;
	while (matchOk && i < strlen_P (F_TO_PSTR (expr))) {
		const char ecur = pgm_read_byte (&((const char *) expr)[i]);		// FIXME
		if (ecur != str[j]) {
			if (ecur == '*') {
				// Capture
				const char enext = pgm_read_byte (&((const char *) expr)[i + 1]);
				if (enext == '/') {		// Always safe
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
					// Capture char is last char of pattern, there must be no slash in str
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

boolean HttpRequest::matchAssociation (WebbinoFStr assocPath) {
	return HttpRequest::parametricMatch (uri, assocPath, matchResult);
}

char *HttpRequest::getBodyStart () {
	char *headerEnd = strstr_P (request, PSTR ("\r\n\r\n"));
	if (headerEnd) {
		headerEnd += 4;		// Get past the string we searched

		if (*headerEnd == '\0') {
			// We're the no-bodies
			headerEnd = NULL;
		}
	}

	return headerEnd;
}
#endif

#ifdef ENABLE_ALL_METHODS
// Extract the value of a parameter from the x-www-form-urlencoded request body
char *HttpRequest::getPostValue (const char param[]) {
	char *body = getBodyStart ();
	if (body != NULL) {
		getFormParameter (body, param);
	} else {
		buffer[0] = '\0';
	}

	return buffer;
}
#endif
