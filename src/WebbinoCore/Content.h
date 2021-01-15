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

#ifndef CONTENT_H_INCLUDED
#define CONTENT_H_INCLUDED

#include <webbino_config.h>
#include <webbino_debug.h>

class HttpRequest;

/* Note that filename is NOT copied, so it must be kept pointing to a valid
 * string during the life of the object.
 */
class Content {
protected:
	const char* filename;

public:
	Content (): filename (nullptr) {
	}

	Content (const char* _filename): filename (_filename) {
	}

	virtual const char* getFilename () const {
		return filename;
	}

	// Please override
	virtual boolean available () = 0;

	// Please override
	virtual byte getNextByte () = 0;
};

#endif
