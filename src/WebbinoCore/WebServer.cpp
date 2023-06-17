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

#include "WebServer.h"
#include "WebClient.h"
#include "Content.h"
#include "MimeTypes.h"
#include "webbino_debug.h"




#define RESPONSE_START "HTTP/1.0 "
#define REDIRECT_HEADER "Location: "
//~ #define NOCACHE_HEADER "Pragma: no-cache
#define CONT_TYPE_HEADER "Content-Type: "
#define ACCESS_CONTROL_ALLOW_ORIGIN "Access-Control-Allow-Origin: *"
#define HEADER_END "\r\n"

#ifdef ENABLE_HTTPAUTH
#define AUTH_HEADER "WWW-Authenticate: Basic realm="
#endif


// FIXME: Store in PROGMEM
const char *HttpStatusMessages[] = {
	"OK",
	"Created",
	"No Content",
	"Moved Permanently",
	"Bad Request",
	"Unauthorized",
	"Forbidden",
	"Not Found"
};

const char *responseCodeToMessage (HttpStatusCode code) {
	const char *msg = "N/A";

	static const HttpStatusCode map[] = {
		HTTP_OK,
		HTTP_OK_CREATED,
		HTTP_OK_NO_CONTENT,
		HTTP_MOVED_PERMANENTLY,
		HTTP_BAD_REQUEST,
		HTTP_UNAUTHORIZED,
		HTTP_FORBIDDEN,
		HTTP_NOT_FOUND
	};

	for (byte i = 0; i < sizeof (map) / sizeof (HttpStatusCode); ++i) {
		if (map[i] == code) {
			msg = HttpStatusMessages[i];
			break;
		}
	}

	return msg;
}

#define replyIsSuccessful(x) ((x) / 100) == 2)

boolean WebServer::begin (NetworkInterface& _netint) {
	nStorage = 0;
	netint = &_netint;

#ifdef ENABLE_HTTPAUTH
	realm = NULL;
	authorizeFunc = NULL;
#endif

#ifndef WEBBINO_NDEBUG
	DPRINTLN (F("Available MIME Types:"));
	const MimeType* mt;
	for (byte i = 0; (mt = reinterpret_cast<const MimeType*> (pgm_read_ptr (&mimeTypes[i]))); ++i) {
		DPRINT (i);
		DPRINT (F(". "));
		DPRINT (PSTR_TO_F (mt -> getExtension ()));
		DPRINT (F(" -> "));
		DPRINTLN (PSTR_TO_F (mt -> getType ()));
	}
#endif

	return true;
}

#ifdef ENABLE_TAGS
void WebServer::enableReplacementTags (const ReplacementTag* const _substitutions[]) {
	substitutions = _substitutions;

#ifndef WEBBINO_NDEBUG
	DPRINTLN (F("Available Tags:"));
	const ReplacementTag* sub;
	for (byte i = 0; substitutions && (sub = reinterpret_cast<const ReplacementTag*> (pgm_read_ptr (&substitutions[i]))); i++) {
		DPRINT (i);
		DPRINT (F(". "));
		DPRINTLN (PSTR_TO_F (sub -> getName ()));
	}
#endif
}
#endif


#ifdef ENABLE_PAGE_FUNCTIONS
void WebServer::associateFunctions (FileFuncAssociationArray* _associations) {
	associations = _associations;

#ifndef WEBBINO_NDEBUG
	DPRINTLN (F("Available Associations:"));
	const FileFuncAssociation* ffa;
	for (byte i = 0; associations && (ffa = reinterpret_cast<const FileFuncAssociation*> (pgm_read_ptr (&associations[i]))); i++) {
		DPRINT (i);
		DPRINT (F(". "));
		DPRINT (PSTR_TO_F (ffa -> getPath ()));
		DPRINT (F(" at 0x"));
// I hate this, but well...
#if __SIZEOF_POINTER__ == 2
		DPRINTLN ((uint16_t) ffa -> getFunction (), HEX);
#elif __SIZEOF_POINTER__ == 4
		DPRINTLN ((uint32_t) ffa -> getFunction (), HEX);
#else
		#error "Mmmmh... Compiling on a weird architecture?"
#endif
	}
#endif
}
#endif

boolean WebServer::addStorage (Storage& storage) {
	boolean ret = false;

	if (nStorage < MAX_STORAGES) {
		storages[nStorage++] = &storage;
		ret = true;
	}

	return ret;
}

const MimeType& WebServer::getContentType (const char* filename) {
	const MimeType* mt = NULL;

#ifndef WEBBINO_NDEBUG
	if (!filename) {
		DPRINTLN (F("WARNING: getContentType() called with NULL filename!"));
	} else {
#endif

	DPRINT (F("Filename is: "));
	DPRINTLN (filename);

	char* ext = strrchr (filename, '.');
	if (ext) {
		++ext;	// Now points to actual extension
		DPRINT (F("Extension is: "));
		DPRINTLN (ext);

		for (byte i = 0; (mt = reinterpret_cast<const MimeType*> (pgm_read_ptr (&mimeTypes[i]))); ++i) {
			if (strcmp_P (ext, mt -> getExtension ()) == 0)
				break;
		}
	}

#ifndef WEBBINO_NDEBUG
	}
#endif

	return mt ? *mt : FALLBACK_MIMETYPE;
}

void WebServer::handleClient (WebClient& client) {
	unsigned int l = strlen (client.request.uri);
	if (l == 0 || client.request.uri[l - 1] == '/') {
		// Request for "/", redirect
		DPRINT (F("Redirecting to "));
		DPRINT (client.request.uri);
		DPRINTLN (F(REDIRECT_ROOT_PAGE));

		client.print (F(RESPONSE_START));
		client.print (HTTP_MOVED_PERMANENTLY);
		client.print (' ');
		client.print (responseCodeToMessage (HTTP_MOVED_PERMANENTLY));
		client.print (F(HEADER_END));

		client.print (F(REDIRECT_HEADER));
		if (l == 0)
			client.print ('/');
		else
			client.print (client.request.uri);
		client.print (F(REDIRECT_ROOT_PAGE HEADER_END HEADER_END));
	} else {
		boolean authOk = false;

#ifdef ENABLE_HTTPAUTH
		if (realm != NULL) {
			// Auth required
			DPRINTLN (F("Auth required"));
			if (client.request.username == NULL || client.request.password == NULL) {
				// Auth data not provided
				DPRINTLN (F("Client did not provide auth data"));
			} else if (authorizeFunc != NULL) {
				// Browser sent username/passwd, check them
				DPRINTLN (F("Calling user auth function"));
				authOk = authorizeFunc (client.request.username, client.request.password);
			} else {
				// Browser sent username/password but user did not set an auth func
				DPRINTLN (F("Auth requested but no authorization function set"));
			}
		} else {
			authOk = true;
		}
#else
		// The compiler will optimize out this variable/check
		authOk = true;
#endif

		if (authOk) {
			const char *pagename = client.request.get_basename ();

			byte i;
			for (i = 0; i < nStorage; ++i) {
				Storage& stor = *storages[i];

				if (stor.exists (pagename)) {
					HttpStatusCode responseCode = HTTP_OK;

					DPRINT (F("Page found on storage "));
					DPRINTLN (i);

					/* Get the content NOW: pagename is stored in buffer, and the
					 * PageFunction, if any, might call request.get_parameter()
					 * which would overwrite the buffer and thus pagename. Not
					 * pretty, but it works.
					 */
					Content& content = stor.get (pagename);

#ifdef ENABLE_PAGE_FUNCTIONS
					// Look up page function, if available
					if (associations != nullptr) {
						const FileFuncAssociation* ass;

						for (byte i = 0; (ass = reinterpret_cast<const FileFuncAssociation*> (pgm_read_ptr (&associations[i]))); i++) {
#ifdef ENABLE_REST
							if (client.request.matchAssociation (PSTR_TO_F (ass -> getPath ()))) {
#else
							if (strcmp_P (pagename, ass -> getPath ()) == 0) {
#endif
								// Found!
								DPRINTLN (F("Page has an associated function"));
								PageFunction func = ass -> getFunction ();
								responseCode = func (client.request);		// The page function can alter the response code
								break;
							}
						}
					}
#endif

					/* content.getFilename() points to the buffer returned by
					 * client.request.get_basename (), which might get modified if
					 * the associated function called get_parameter(), thus let's
					 * restore it so that it keeps pointing to what is should.
					 *
					 * I guess this is crap and should be a FIXME...
					 */
					client.request.get_basename ();

					const MimeType& contType = getContentType (content.getFilename ());
					DPRINT (F("Content type is "));
					DPRINTLN (contType.getType ());

					// Send headers
					client.print (F(RESPONSE_START));
					client.print (responseCode);
					client.print (' ');
					client.print (responseCodeToMessage (responseCode));
					client.print (F(HEADER_END));

					if (replyIsSuccessful (responseCode) {
						client.print (F(CONT_TYPE_HEADER));
						client.print (PSTR_TO_F (contType.getType ()));
						client.print (F(HEADER_END));
						
						if(ALLOW_CROSS_ORIGIN){
							client.print (F(ACCESS_CONTROL_ALLOW_ORIGIN));
							client.print (F(HEADER_END));
						}
						
						client.print (F(HEADER_END));

						sendContent (client, content, contType);
					} else {
						DPRINTLN (F("Not sending output since reply indicates error"));
					}

					stor.release (content);

					// Stop looping on storages
					break;
				}
			}

			if (i >= nStorage) {
				// Page not found
				client.print (F(RESPONSE_START));
				client.print (HTTP_NOT_FOUND);
				client.print (' ');
				client.print (responseCodeToMessage (HTTP_NOT_FOUND));
				client.print (F(HEADER_END));

				client.print (F(HEADER_END));

				client.print (F("<html><body><h3>No such page: \""));
				client.print (pagename);
				client.print (F("\"</h3></body></html>"));
			}
		} else {
#ifdef ENABLE_HTTPAUTH
			// Auth failed (or no auth data provided)
			client.print (F(RESPONSE_START));
			client.print (HTTP_UNAUTHORIZED);
			client.print (' ');
			client.print (responseCodeToMessage (HTTP_UNAUTHORIZED));
			client.print (F(HEADER_END));

			client.print (F(AUTH_HEADER));
			client.print (realm);
			client.print (F(HEADER_END));

			client.print (F(HEADER_END));
#endif
		}
	}

	client.sendReply ();
}

#ifdef ENABLE_TAGS
PString* WebServer::findSubstitutionTag (const char *tag) const {
	const ReplacementTag* sub;
	PString* ret = nullptr;

	for (byte i = 0; !ret && (sub = reinterpret_cast<const ReplacementTag*> (pgm_read_ptr (&substitutions[i]))); i++) {
		if (strcmp_P (tag, sub -> getName ()) == 0) {
			PString& pb = (sub -> getFunction ()) (sub -> getData ());
			ret = &pb;
		}
	}

	return ret;
}

char *WebServer::findSubstitutionTagGetParameter (HttpRequest& request, const char *tag) {
	return request.get_parameter (tag);
}
#endif

// Read the page, perform tag substitutions and send it over
// FIXME: Handle unterminated tags
void WebServer::sendContent (WebClient& client, Content& content, const MimeType& contType) {
#ifdef ENABLE_TAGS
	char tag[MAX_TAG_LEN];
	int8_t tagLen = -1;			// If >= 0 we are inside a tag
#endif
	while (content.available ()) {
		byte c = content.getNextByte ();

#ifdef ENABLE_TAGS
		if (contType.supportsReplacements) {		// We do not want to do replacements on binary files...
			if (tagLen >= 0) {
				// A tag is in progress
				if (c == TAG_CHAR) {
					// End of tag
					DPRINT (F("Processing replacement tag: \""));
					DPRINT (tag);
					DPRINTLN (F("\""));

					if (tagLen >= MAX_TAG_LEN - 1) {
						DPRINT (F("WARNING: Tag was truncated (Max length is "));
						DPRINT (MAX_TAG_LEN - 1);
						DPRINTLN ((byte) ')');
					}

					boolean found = false;
					if (strncmp_P (tag, PSTR ("GETP_"), 5) == 0) {
						char* rep = findSubstitutionTagGetParameter (client.request, tag + 5);
						if (rep) {
							DPRINT (F("Replacement is: \""));
							DPRINT (rep);
							DPRINTLN (F("\""));

							client.print (rep);
							found = true;
						}
					} else {
						PString* pstr = findSubstitutionTag (tag);
						if (pstr) {
							DPRINT (F("Replacement is: \""));
							DPRINT (*pstr);
							DPRINTLN (F("\""));

							client.print (*pstr);
							pstr -> begin ();		// Reset for next usage
							found = true;
						}
					}

					if (!found) {
						// Tag not found, emit it
						DPRINTLN (F("Tag not found"));

						client.write (TAG_CHAR);
						client.print (tag);
						client.write (TAG_CHAR);
					}

					// Prepare for next tag
					tagLen = -1;
				} else if (tagLen < MAX_TAG_LEN - 1) {
					// Tag continues
					tag[tagLen++] = c;
					tag[tagLen] = '\0';
				} else {
					// Tag too long, just count for debugging purposes
					++tagLen;
				}
			} else {
				if (c == TAG_CHAR) {
					// (Possible) New tag
					tag[0] = '\0';
					tagLen = 0;
				} else {
					client.write (c);		// c is a raw byte
				}
			}
		} else {
			client.write (c);
		}
#else
		client.write (c);
#endif
	}
}

boolean WebServer::loop () {
	WebClient *client = netint -> processPacket ();
	if (client != NULL) {
		// Got a client with a request, process it
		DPRINT (F("Request for \""));
		DPRINT (client -> request.uri);
		DPRINTLN (F("\""));

		handleClient (*client);
	}

	return client != NULL;
}

#ifdef ENABLE_HTTPAUTH
void WebServer::enableAuth (const char *_realm, AuthorizeFunc _authFunc) {
	realm = _realm;

	authorizeFunc = _authFunc;
}
#endif
