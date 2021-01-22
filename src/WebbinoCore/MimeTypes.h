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

#ifndef MIMETYPES_H_INCLUDED
#define MIMETYPES_H_INCLUDED

struct MimeType {
	PGM_P ext;
	PGM_P type;

	/* NOTE: If this field is of type boolean, code will crash on ESP8266 for
	 * some reason! It appears to work fine with uint32_t. I guess it's a matter
	 * of alignment with the PROGMEM stuff (which should do nothing on ESP???).
	 */
#ifdef ARDUINO_ARCH_ESP8266
	uint32_t supportsReplacements;
#else
	boolean supportsReplacements;
#endif

	// Methods that (try to) hide the complexity of accessing PROGMEM data
	PGM_P getExtension () const {
		return reinterpret_cast<PGM_P> (pgm_read_ptr (&ext));
	}

	PGM_P getType () const {
		return reinterpret_cast<PGM_P> (pgm_read_ptr (&type));
	}
};

// Macros that simplify things (somehow)
#define MT_STR(s) #s
#define MT_EXTVAR(ext) _mtExt_ ## ext
#define MT_TYPE(type, subtype) #type "/" #subtype
#define MT_TYPEVAR(ext) _mtType_ ## ext

#define MimeTypeEntry(var, ext, type, subtype, doTags) \
                const char MT_EXTVAR(ext)[] PROGMEM = MT_STR(ext); \
                const char MT_TYPEVAR(ext)[] PROGMEM = MT_TYPE(type, subtype); \
                const MimeType var PROGMEM = {MT_EXTVAR(ext), MT_TYPEVAR(ext), doTags}

MimeTypeEntry (mt_htm, htm, text, html, true);
MimeTypeEntry (mt_html, html, text, html, true);
MimeTypeEntry (mt_css, css, text, css, true);
MimeTypeEntry (mt_js, js, application, javascript, false);
MimeTypeEntry (mt_txt, txt, text, plain, true);
MimeTypeEntry (mt_png, png, image, png, false);
MimeTypeEntry (mt_gif, gif, image, gif, false);
MimeTypeEntry (mt_jpg, jpg, image, jpeg, false);
MimeTypeEntry (mt_ico, ico, image, x-icon, false);

#ifdef ENABLE_EXTRA_MIMETYPES
MimeTypeEntry (mt_xml, xml, text, xml);
MimeTypeEntry (mt_pdf, pdf, application, x-pdf, false);
MimeTypeEntry (mt_zip, zip, application, x-zip, false);
MimeTypeEntry (mt_gz, gz, application, x-gzip, false);
#endif

#ifdef ENABLE_REST
// This is used when sending REST replies
MimeTypeEntry (mt_wwf, _wwwf_, application, x-www-form-urlencoded, true);
#endif

typedef const MimeType* const MimeTypeArray;

const MimeTypeArray mimeTypes[] PROGMEM = {
	&mt_htm,
	&mt_html,
	&mt_css,
	&mt_js,
	&mt_txt,
	&mt_png,
	&mt_gif,
	&mt_jpg,
	&mt_ico,
#ifdef ENABLE_EXTRA_MIMETYPES
	&mt_xml,
	&mt_pdf,
	&mt_zip,
	&mt_gz,
#endif
#ifdef ENABLE_REST
	&mt_wwf,
#endif
	NULL		// Keep at end
};

// "Fallback" mimetype, to be used for files with unknown extensions
MimeTypeEntry (FALLBACK_MIMETYPE, ___, application, octet-stream, false);

#endif
