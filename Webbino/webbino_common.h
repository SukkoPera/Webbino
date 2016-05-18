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

#ifndef WEBBINOCOMMON_H_
#define WEBBINOCOMMON_H_

/* Network device selection: please enable *only one* of the following,
 * corresponding to the network device you will be using
 */
#define WEBBINO_USE_WIZ5100
//~ #define WEBBINO_USE_ENC28J60
//~ #define WEBBINO_USE_ESP8266

/* Define to enable serving webpages from SD. This will use the SD
 * library, but you will have to initialize it in your sketch's setup()
 */
#define WEBBINO_ENABLE_SD

/* Define to enable support for tag substitutions, i.e.: replace #TAGS#
 * in served pages
 */
#define ENABLE_TAGS

/* Character that delimits tags
 */
#define TAG_CHAR '#'

/* TCP port the server will listen on
 * NOTE: Currently changing this will have no effect, FIXME
 */
#define SERVER_PORT 80

// DEFINE this to DISABLE debug messages
//~ #define NDEBUG

// Don't touch :)
#define WEBBINO_VERSION "20160518"

#endif
