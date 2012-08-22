#!/usr/bin/env python

# Run this script like this:
# ./html2h.py *.html > html.h

# Enable this to skip encoding of CR and LF. They aren't necessary for browsers
# but might be useful during debugging.
SKIP_CRLF = True

import os
import sys
import platform

if len (sys.argv) > 0:
	if platform.system () == 'Windows':
		# We must expand the command line ourselves, thank Microsoft
		import glob
		files = []
		for l in [glob.glob (arg) for arg in sys.argv[1:]]:
			files.extend (l)
	else:
		files = sys.argv[1:]

	for filename in files:
		fp = open (filename, 'r');
		if fp:
			code = filename.replace ('.', '_')
			print "const char %s_name[] PROGMEM = \"%s\";" % (code, filename)
			print
			print "const char %s[] PROGMEM = {" % code
			i = 0
			b = fp.read (1)
			while b:
				if not SKIP_CRLF or (b != '\n' and b != '\r'):
					if i % 8 == 0:
						print "\t",
					print "0x%02x, " % ord (b),
					i = i + 1
					if i % 8 == 0:
						print ""
				b = fp.read (1)
			fp.close ()
		else:
			print "Cannot open file %s" % filename
		
		print "0x00"
		print "};"
		print
		#print "unsigned int %s_len = %u;" % (code, i + 1)
else:
	print "Usage: %s <file> ... <file>" % sys.argv[0]
