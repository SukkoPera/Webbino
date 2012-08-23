/***************************************************************************
 *   This file is part of SmartStrip.                                      *
 *                                                                         *
 *   Copyright (C) 2012 by SukkoPera                                       *
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

//#include <stdlib.h>
#include <string.h>

#include "HTTPRequestParser.h"


HTTPRequestParser::HTTPRequestParser (char *req): request (req) {
	extract_url ();
}

// HTTPRequestParser::~HTTPRequestParser () {
// }

void HTTPRequestParser::extract_url () {
	char *p, *q;

	url[0] = '\0';
	if ((p = strstr_P (request, PSTR ("GET ")))) {
		if ((q = strchr (p + 4,  ' ')))
			strlcpy (url, p + 4, q - p - 4 + 1 < MAX_URL_LEN ? q - p - 4 + 1 : MAX_URL_LEN);
		else
			strlcpy (url, p + 4, MAX_URL_LEN);

#ifdef VERBOSE_REQUEST_PARSER
		Serial.print (F("Extracted URL: \""));
		Serial.print (url);
		Serial.println (F("\""));
#endif
	} else {
		Serial.println (F("Cannot extract URL"));
	}
}

char *HTTPRequestParser::get_basename () {
	buffer[0] = '\0';
	char *slash = strrchr (url, '/');
	if (slash) {
		char *qMark = strchr (slash + 1, '?');
		if (qMark)
			strlcpy (buffer, slash + 1, qMark - slash < BUF_LEN ? qMark - slash : BUF_LEN);
		else
			strlcpy (buffer, slash + 1, BUF_LEN);
	}

#ifdef VERBOSE_REQUEST_PARSER
	Serial.print (F("Extracted basename: \""));
	Serial.print (buffer);
	Serial.println (F("\""));
#endif

	return buffer;
}

char *HTTPRequestParser::get_get_parameter (const char param[]) {
	char *start;
	bool found;

	buffer[0] = '\0';
	found = false;
	for (start = strchr (url, '?'); !found && start; start = strchr (start + 1, '&')) {
		char *end = strchr (start, '=');
		if (end && (end - start - 1) == (int) strlen (param) && strncmp (start + 1, param, end - start - 1) == 0) {
			// Found!
			char *x = strchr (end + 1, '&');
			if (x)
				strlcpy (buffer, end + 1, x - end < BUF_LEN ? x - end : BUF_LEN);
			else
				strlcpy (buffer, end + 1, BUF_LEN);
			found = true;
		}
	}

#ifdef VERBOSE_REQUEST_PARSER
	Serial.print (F("Extracted GET parameter: \""));
	Serial.print (param);
	Serial.print (F("\": \""));
	Serial.print (buffer);
	Serial.println (F("\""));
#endif

	return buffer;
}

char *HTTPRequestParser::get_get_parameter (__FlashStringHelper *param) {
	char *start;
	bool found;

	buffer[0] = '\0';
	found = false;
	for (start = strchr (url, '?'); !found && start; start = strchr (start + 1, '&')) {
		char *end = strchr (start, '=');
		if (end && (end - start - 1) == (int) 	strlen_P (reinterpret_cast<PGM_P> (param)) && strncmp_P (start + 1, reinterpret_cast<PGM_P> (param), end - start - 1) == 0) {
			// Found!
			char *x = strchr (end + 1, '&');
			if (x)
				strlcpy (buffer, end + 1, x - end < BUF_LEN ? x - end : BUF_LEN);
			else
				strlcpy (buffer, end + 1, BUF_LEN);
			found = true;
		}
	}

#ifdef VERBOSE_REQUEST_PARSER
	Serial.print (F("Extracted GET parameter: \""));
	Serial.print (param);
	Serial.print (F("\": \""));
	Serial.print (buffer);
	Serial.println (F("\""));
#endif

	return buffer;
}
