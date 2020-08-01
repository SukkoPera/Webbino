#!/usr/bin/env python

# This file is part of Webbino
#
# Copyright (C) 2012-2020 by SukkoPera
#
# Webbino is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# Webbino is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with Webbino. If not, see <http://www.gnu.org/licenses/>.
#
#
# Run this script like this:
# ./html2h.py webroot > html.h
#
# Note that this is untested on Windows/OSX!

import os
import sys

# https://www.safaribooksonline.com/library/view/python-cookbook/0596001673/ch04s16.html
def splitall (path):
    allparts = []
    while 1:
        parts = os.path.split(path)
        if parts[0] == path:  # sentinel for absolute paths
            allparts.insert(0, parts[0])
            break
        elif parts[1] == path: # sentinel for relative paths
            allparts.insert(0, parts[1])
            break
        else:
            path = parts[0]
            allparts.insert(0, parts[1])
    return allparts

def shallStrip (filename):
	name, ext = os.path.splitext (filename)
	if len (ext) > 1:
		ext = ext[1:]
	ext = ext.lower ()
	return ext == "htm" or ext == "html"

def make_page_name (filename):
	# Make up a unique ID for every file to use in C identifiers
	parts = splitall (filename[2:])
	parts = [parts[0]] + [x.capitalize () for x in parts[1:]]
	code = "".join (parts)
	code = code.replace ('.', '_')
	code = code.replace ('-', '_')
	pagename = filename[1:]

	# Convert Windows slashes to Posix slashes
	pagename = pagename.replace ('\\', '/')

	return code, pagename

def process_binary_file (filename):
	print ("Processing binary file: %s" % filename, file=sys.stderr)
	try:
		with open (filename, 'rb') as fp:
			code, pagename = make_page_name (filename)
			print ("const char %s_name[] PROGMEM = \"%s\";" % (code, pagename))
			print ()
			print ("const byte %s[] PROGMEM = {" % code)

			i = 0
			bs = fp.read (1024)
			while bs:
				for b in bs:
					if i % 8 == 0:
						print ("\t", end = '')
					print ("0x%02x, " % b, end = '')
					i += 1
					if i % 8 == 0:
						print ("")
				bs = fp.read (1024)

			print ("\n};")
			print ()
			print ("const unsigned int %s_len PROGMEM = %u;" % (code, i))
			print ()
	except IOError as ex:
		print ("Cannot open file %s: %s" % (filename, str (ex)))
		code = None

	return code

# Thanks to jfs at https://stackoverflow.com/questions/898669/how-can-i-detect-if-a-file-is-binary-non-text-in-python
textchars = bytearray ({7,8,9,10,12,13,27} | set (range (0x20, 0x100)) - {0x7f})
is_binary_string = lambda bytes: bool (bytes.translate (None, textchars))
is_binary_file = lambda fn: is_binary_string (open (fn, 'rb').read(1024))

def process_text_file (filename, defines, nostrip = False):
	print ("Processing text file: %s" % filename, file=sys.stderr)

	if not nostrip and not shallStrip (filename):
		print ("- File will not be stripped", file=sys.stderr)
		nostrip = True

	try:
		with open (filename, 'r') as fp:
			code, pagename = make_page_name (filename)
			print ("const char %s_name[] PROGMEM = \"%s\";" % (code, pagename))
			print ()
			print ("const byte %s[] PROGMEM = {" % code)

			i = 0
			line = fp.readline ()
			tokens = []
			while line:
				if line.startswith ("#ifdef"):
					parts = line.split ()
					assert len (parts) == 2
					token = parts[1]
					tokens.append (token)
				elif line.startswith ("#ifndef"):
					token = line.split ()[1]
					tokens.append ("!%s" % token)
				elif line.startswith ("#else"):
					token = tokens.pop ()
					tokens.append ("!%s" % token)
				elif line.startswith ("#endif"):
					tokens.pop ()
				elif all (d[1:] not in defines if d[0] == '!' else d in defines for d in tokens):
					# This works because all ([]) == True
					for b in line:
						if nostrip or (b != '\n' and b != '\r' and b != '\t'):
							if i % 8 == 0:
								print ("\t", end = '')
							print ("0x%02x, " % ord (b), end = '')
							i += 1
							if i % 8 == 0:
								print ("")
				else:
					print ("Skipping line: %s" % line.strip (), file=sys.stderr)
				line = fp.readline ()

			print ("\n};")
			print ()
			print ("const unsigned int %s_len PROGMEM = %u;" % (code, i))
			print ()
	except IOError as ex:
		print ("Cannot open file %s: %s" % (filename, str (ex)))
		code = None

	return code

def process_file (filename, defines, nostrip = False):
	if is_binary_file (filename):
		return process_binary_file (filename)
	else:
		return process_text_file (filename, defines, nostrip)

def process_dir (dirpath, defines, nostrip = False):
	print ("Processing directory: %s" % dirpath, file=sys.stderr)
	idents = []
	for filename in sorted (os.listdir (dirpath)):
		fullfile = os.path.join (dirpath, filename)
		if os.path.isfile (fullfile):
			ident = process_file (fullfile, defines, nostrip)
			if ident is not None:
				idents.append (ident)
		elif os.path.isdir (fullfile):
			idents += process_dir (fullfile, defines, nostrip)
		else:
			print ("Skipping %s" % filename)
	return idents

def make_include_code (idents):
	ret = ""

	for n, ident in enumerate (idents):
		ret += "const Page page%02d PROGMEM = {%s_name, %s, %s_len};\n" % (n + 1, ident, ident, ident)

	ret += "\n"

	ret += "const Page* const pages[] PROGMEM = {\n"
	for n in range (1, n_pages + 1):
		ret += "\t&page%02d,\n" % n

	ret += "\tNULL\n"
	ret += "};"

	return ret


### MAIN ###

if __name__ == "__main__":
	import argparse

	parser = argparse.ArgumentParser (description = 'Convert a website to be put into flash memory for use with Webbino')
	parser.add_argument ('webroot', metavar = "WEBROOT", help = "Path to website root directory")
	parser.add_argument ('--nostrip', "-n", action = 'store_true', default = False,
						 help = "Do not strip CR/LF/TABs")
	parser.add_argument ("--define", "-D", action = "append")

	args = parser.parse_args ()

	# The above will raise an error if webroot was not specified, so we can
	# assume it was
	os.chdir (args.webroot)
	idents = process_dir (".", args.define or [], args.nostrip)
	n_pages = len (idents)

	print ("/*** CODE TO INCLUDE IN SKETCH ***\n")
	print (make_include_code (idents))
	print ("\n***/")

	print ("Total files processed: %d" % n_pages, file=sys.stderr)

	if n_pages > 0:
		# Help with code to be put in sketch
		print (file=sys.stderr)
		print ("Put the following in your sketch:", file=sys.stderr)
		print (file=sys.stderr)
		print ('#include "html.h"', file=sys.stderr)
		print (file=sys.stderr)
		print (make_include_code (idents), file=sys.stderr)
