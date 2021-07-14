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

#ifndef WEBBINOCOMMON_H_
#define WEBBINOCOMMON_H_

#include <Arduino.h>

/* NOTE: For more detailed and up-to-date information about all the options in
 * this configuration file, please refer to the dedicated wiki page:
 * https://github.com/SukkoPera/Webbino/wiki/The-Configuration-File
 */


/* FIRST: Network device selection
 * -------------------------------
 *
 * Please enable *only one* of the following,
 * corresponding to the network device you will be using
 */
#define WEBBINO_USE_WIZ5100
//~ #define WEBBINO_USE_WIZ5500
//~ #define WEBBINO_USE_ENC28J60
//~ #define WEBBINO_USE_ENC28J60_UIP
//~ #define WEBBINO_USE_ESP8266
//~ #define WEBBINO_USE_ESP8266_STANDALONE
//~ #define WEBBINO_USE_WIFI
//~ #define WEBBINO_USE_WIFI101
//~ #define WEBBINO_USE_FISHINO
//~ #define WEBBINO_USE_DIGIFI
//~ #define WEBBINO_USE_TEENSY41_NATIVE


/* SECOND: Webpages storage
 * ------------------------
 *
 * You have to choose where your webpages are stored. You can select from SD
 * (using or standard Arduino SD library or SDFAT library), integrated filesystem
 * (using or the deprecated SPIFFS o the LittleFS) or from flash memory.
 * Please, enable only one storage corresponding to one you will be using.
 */

/*   Define to enable serving webpages from SD. This will use Arduino's SD
 *   library, which only allows DOS-style (i.e. 8+3 characters) file names. This
 *   means that you will have to name your pages with a .htm extension, instead of
 *   .html. If you don't like this, install the SdFat library and see below.
 */
//~ #define WEBBINO_ENABLE_SD

/*   Define to enable serving webpages from SD. This will use the SDFat
 *   library (https://github.com/greiman/SdFat). This library allows access to
 *   files with long names (LFNs), if properly configured (see SdFatConfig.h in
 *   the library sources).
 */
//~ #define WEBBINO_ENABLE_SDFAT
#if defined (WEBBINO_ENABLE_SD) && defined (WEBBINO_ENABLE_SDFAT)
#error "You can't enable both SD and SDFAT libraries"
#endif

/*   Define to enable serving webpages from the ESP8266 integrated filesystem on
 *   flash. There are actually two filesystems: SPIFFS is deprecated and will be
 *   removed soon, LittleFS has more capabilities and is the way to go for the
 *   future.
 */
#if defined (WEBBINO_USE_ESP8266_STANDALONE) || (defined (ARDUINO_ARCH_ESP32) && defined (WEBBINO_USE_WIFI))
#define WEBBINO_ENABLE_LITTLEFS
//~ #define WEBBINO_ENABLE_SPIFFS
#endif
#if defined (WEBBINO_ENABLE_LITTLEFS) && defined (WEBBINO_ENABLE_SPIFFS)
#error "You can't enable both LITTLEFS and SPIFFS at the same time"
#endif

/*   Name of the index page, i.e. the page requests for / get redirected to.
 */
#ifdef WEBBINO_ENABLE_SD
// Long File Names are not supported, cope with it
#define REDIRECT_ROOT_PAGE "index.htm"
#else
#define REDIRECT_ROOT_PAGE "index.html"
#endif


/* THIRD: Enable/Disable optional features
 * ---------------------------------------
 */

/*   Define to enable support for replacement tag, i.e.: replace $TAGS$ in the
 *   served pages
 */
#define ENABLE_TAGS

/*   Define to enable running functions upon request of certain pages
 */
#define ENABLE_PAGE_FUNCTIONS

/*   Define to enable HTTP Basic Authorization support
 */
//~ #define ENABLE_HTTPAUTH

/*   By default only the GET HTTP method/verb is supported (i.e.: all requests are
 *   implicitly assumed to be GETs). Define this to enable the parsing of the
 *   actual method, which will be available in page functions.
 */
//~ #define ENABLE_ALL_METHODS

/*   Enable some features that will help implementing REST services. Note that you
 *   should probably enable ENABLE_ALL_METHODS with this.
 */
//~ #define ENABLE_REST

/*   By default, only MIME types for html, css, js, png, jpeg, gif and ico files
 *   are enabled. Define this to enable some extra types, namely xml, pdf, zip and
 *   gz files.
 */
//~ #define ENABLE_EXTRA_MIMETYPES


/* FOURTH: Adjust, only if necessary, some library parameters
 * ----------------------------------------------------------
 */

/*   Character that delimits tags - Make sure this is a *byte* and not a char
 */
const byte TAG_CHAR = static_cast<byte> ('$');

/*   Maximum length of a tag name
 */
#define MAX_TAG_LEN 24

/*   Maximum length of a filename in the Flash storage
 */
#define MAX_FLASH_FNLEN 16

/*   Maximum length of a get parameter name and value
 */
#define BUF_LEN 32

/*   Maximum length of an URL to process
 */
#define MAX_URL_LEN 128

/*   TCP port the server will listen on
 *
 *   NOTE: Port 80 can not be used with DigiFi
 *   NOTE: Currently changing this will have no effect with most cards, FIXME
 */
#define SERVER_PORT 80

/*   Size of output buffer. This speeds up transmission, by sending clients more
 *   than one character at a time. Size it appropriately according to available
 *   RAM. Theoretically it could be reduced to 1, but this has not been tested.
 */
#define CLIENT_BUFSIZE 64

/*   Maximum length of the username:password string. Only used if
 *   ENABLE_HTTPAUTH is defined.
 */
#define MAX_USERPASS_LEN 32

/*   Maximum time in milliseconds without receiving characters after which a
 *   client connection is dropped. Solves hanging connection from Chrome on OSX.
 *   Undefine to turn off this feature.
 *
 *   NOTE: This feature is not available on ENC28J60 when using the EtherCard
 *         library.
 *
 *   Implemented by gpb01, thanks!
 */
#define CLIENT_TIMEOUT 2500

/*
 * Set the Access-Control-Allow-Origin response header to wildcard (allow cross origin).
 * Set to 1 t enable.
 */
#define ALLOW_CROSS_ORIGIN 0

/* FIFTH: choose if you want to see on the serial terminal the Debug messages
 * --------------------------------------------------------------------------
 */

/*   DEFINE this to DISABLE debug messages
 */
#define WEBBINO_NDEBUG

/*   Enable verbose HTTP request parser
 */
//~ #define VERBOSE_REQUEST_PARSER

#endif
